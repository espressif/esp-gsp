/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "gsp/platform/esp_gsp_idf_touch.h"

#include <string.h>

#include "esp_log.h"

static const char *TAG = "gsp_touch";

/* Polling-only controllers can occasionally expose one empty report while a
 * finger is still down. Interrupt mode consumes only fresh IRQ reports, so a
 * fresh empty report is the controller's release state and needs no guessing. */

static void touch_interrupt_callback(esp_lcd_touch_handle_t handle)
{
    gsp_esp_touch_t *touch = handle->config.user_data;
    if (touch == NULL) {
        return;
    }
    ++touch->interrupt_sequence;
    if (touch->external_wake_from_isr != NULL) {
        touch->external_wake_from_isr(touch->external_wake_ctx);
    }
    if (touch->wake_from_isr != NULL) {
        touch->wake_from_isr(touch->wake_ctx);
    }
}

void gsp_esp_touch_init(gsp_esp_touch_t *touch,
                        esp_lcd_touch_handle_t handle,
                        bool interrupt_mode,
                        bool interrupt_required,
                        uint8_t release_confirm_polls,
                        gsp_esp_touch_wake_from_isr_t external_wake_from_isr,
                        void *external_wake_ctx)
{
    if (touch != NULL) {
        memset(touch, 0, sizeof(*touch));
        touch->handle = handle;
        touch->interrupt_mode = interrupt_mode;
        touch->interrupt_required = interrupt_required;
        touch->release_confirm_polls = release_confirm_polls;
        touch->external_wake_from_isr = external_wake_from_isr;
        touch->external_wake_ctx = external_wake_ctx;
    }
}

esp_err_t gsp_esp_touch_start(
    gsp_esp_touch_t *touch,
    gsp_esp_touch_wake_from_isr_t wake_from_isr,
    void *wake_ctx)
{
    if (touch == NULL || touch->handle == NULL ||
            (touch->interrupt_mode && wake_from_isr == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!touch->interrupt_mode) {
        return ESP_OK;
    }
    if (!gsp_esp_touch_interrupt_available(touch->handle)) {
        if (touch->interrupt_required) {
            return ESP_ERR_INVALID_STATE;
        }
        ESP_LOGW(TAG, "Touch interrupt is unavailable; using polling");
        touch->interrupt_mode = false;
        return ESP_OK;
    }

    touch->wake_from_isr = wake_from_isr;
    touch->wake_ctx = wake_ctx;
    touch->saved_user_data = touch->handle->config.user_data;
    esp_err_t ret = esp_lcd_touch_register_interrupt_callback_with_data(
                        touch->handle, touch_interrupt_callback, touch);
    if (ret != ESP_OK) {
        /* esp_lcd_touch records the callback before all GPIO setup has
         * succeeded. Clear that partial ownership before returning. */
        touch->handle->config.interrupt_callback = NULL;
        touch->handle->config.user_data = touch->saved_user_data;
        touch->wake_from_isr = NULL;
        touch->wake_ctx = NULL;
        if (!touch->interrupt_required) {
            ESP_LOGW(TAG, "Touch interrupt registration failed (%s); using polling",
                     esp_err_to_name(ret));
            touch->interrupt_mode = false;
            return ESP_OK;
        }
        return ret;
    }
    touch->interrupt_registered = true;
    return ESP_OK;
}

void gsp_esp_touch_stop(gsp_esp_touch_t *touch)
{
    if (touch == NULL || !touch->interrupt_registered) {
        return;
    }
    touch->wake_from_isr = NULL;
    touch->wake_ctx = NULL;
    touch->handle->config.user_data = NULL;
    esp_err_t ret = esp_lcd_touch_register_interrupt_callback(
                        touch->handle, NULL);
    if (ret == ESP_OK) {
        touch->handle->config.user_data = touch->saved_user_data;
    } else {
        ESP_LOGE(TAG, "Touch interrupt unregister failed: %s",
                 esp_err_to_name(ret));
    }
    touch->interrupt_registered = false;
}

esp_err_t gsp_esp_touch_poll_frame(
    gsp_esp_touch_t *touch,
    gsp_platform_touch_frame_t *out_frame,
    bool *out_has_sample)
{
    if (touch == NULL || touch->handle == NULL || out_frame == NULL ||
            out_has_sample == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *out_has_sample = false;

    uint32_t sequence = touch->interrupt_sequence;
    if (touch->interrupt_mode && sequence == touch->consumed_sequence) {
        /* No new hardware state. Keep the last pressed frame; reading a
         * controller without a fresh IRQ can return an empty report and must
         * not synthesize a release. */
        return ESP_OK;
    }

    esp_err_t ret = esp_lcd_touch_read_data(touch->handle);
    if (ret != ESP_OK) {
        return ret;
    }
    esp_lcd_touch_point_data_t points[GSP_PLATFORM_MAX_TOUCH_POINTS] = {0};
    uint8_t count = 0;
    ret = esp_lcd_touch_get_data(
              touch->handle, points, &count, GSP_PLATFORM_MAX_TOUCH_POINTS);
    if (ret != ESP_OK) {
        return ret;
    }
    if (count > GSP_PLATFORM_MAX_TOUCH_POINTS) {
        count = GSP_PLATFORM_MAX_TOUCH_POINTS;
    }

    if (count > 0) {
        if (touch->release_misses > 0) {
            ESP_LOGD(TAG, "ignored %u transient empty touch poll(s)",
                     touch->release_misses);
        }
        touch->release_misses = 0;
        *out_frame = (gsp_platform_touch_frame_t) {
            .count = count,
        };
        for (uint8_t index = 0; index < count; ++index) {
            out_frame->contacts[index] =
            (gsp_platform_touch_contact_t) {
                .x = points[index].x,
                .y = points[index].y,
                .strength = points[index].strength,
                .id = points[index].track_id,
            };
        }
        touch->last_frame = *out_frame;
        touch->last_x = out_frame->contacts[0].x;
        touch->last_y = out_frame->contacts[0].y;
        touch->was_pressed = true;
        touch->consumed_sequence = sequence;
        *out_has_sample = true;
        return ESP_OK;
    }
    if (touch->last_frame.count == 0) {
        touch->consumed_sequence = sequence;
        return ESP_OK;
    }
    if (!touch->interrupt_mode) {
        ++touch->release_misses;
        if (touch->release_misses <
                touch->release_confirm_polls) {
            touch->consumed_sequence = sequence;
            return ESP_OK;
        }
    }
    touch->release_misses = 0;
    memset(out_frame, 0, sizeof(*out_frame));
    memset(&touch->last_frame, 0, sizeof(touch->last_frame));
    touch->was_pressed = false;
    touch->consumed_sequence = sequence;
    *out_has_sample = true;
    return ESP_OK;
}

esp_err_t gsp_esp_touch_poll(gsp_esp_touch_t *touch,
                             gsp_platform_pointer_t *out_pointer,
                             bool *out_has_sample)
{
    if (touch == NULL || out_pointer == NULL || out_has_sample == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    gsp_platform_touch_frame_t frame;
    esp_err_t ret = gsp_esp_touch_poll_frame(
                        touch, &frame, out_has_sample);
    if (ret != ESP_OK || !*out_has_sample) {
        return ret;
    }
    bool pressed = frame.count > 0;
    *out_pointer = (gsp_platform_pointer_t) {
        .x = pressed ? frame.contacts[0].x : touch->last_x,
        .y = pressed ? frame.contacts[0].y : touch->last_y,
        .pressed = pressed,
    };
    return ESP_OK;
}
