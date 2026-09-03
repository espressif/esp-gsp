/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_lcd_touch.h"
#include "gsp/gsp_platform.h"

typedef void (*gsp_esp_touch_wake_from_isr_t)(void *ctx);

/* Interrupt-driven touch needs a wired INT GPIO and an unclaimed
 * callback slot; single predicate for mode selection (assembly) and
 * registration (start), so the two never drift apart. */
static inline bool gsp_esp_touch_interrupt_available(
    esp_lcd_touch_handle_t handle)
{
    return handle != NULL &&
           handle->config.int_gpio_num != GPIO_NUM_NC &&
           handle->config.interrupt_callback == NULL;
}

typedef struct {
    esp_lcd_touch_handle_t handle;
    gsp_esp_touch_wake_from_isr_t wake_from_isr;
    void *wake_ctx;
    gsp_esp_touch_wake_from_isr_t external_wake_from_isr;
    void *external_wake_ctx;
    void *saved_user_data;
    int32_t last_x;
    int32_t last_y;
    gsp_platform_touch_frame_t last_frame;
    volatile uint32_t interrupt_sequence;
    uint32_t consumed_sequence;
    uint8_t release_misses;
    uint8_t release_confirm_polls;
    bool was_pressed;
    bool interrupt_mode;
    bool interrupt_required;
    bool interrupt_registered;
} gsp_esp_touch_t;

void gsp_esp_touch_init(gsp_esp_touch_t *touch,
                        esp_lcd_touch_handle_t handle,
                        bool interrupt_mode,
                        bool interrupt_required,
                        uint8_t release_confirm_polls,
                        gsp_esp_touch_wake_from_isr_t external_wake_from_isr,
                        void *external_wake_ctx);

esp_err_t gsp_esp_touch_start(
    gsp_esp_touch_t *touch,
    gsp_esp_touch_wake_from_isr_t wake_from_isr,
    void *wake_ctx);

void gsp_esp_touch_stop(gsp_esp_touch_t *touch);

esp_err_t gsp_esp_touch_poll_frame(
    gsp_esp_touch_t *touch,
    gsp_platform_touch_frame_t *out_frame,
    bool *out_has_sample);

esp_err_t gsp_esp_touch_poll(gsp_esp_touch_t *touch,
                             gsp_platform_pointer_t *out_pointer,
                             bool *out_has_sample);
