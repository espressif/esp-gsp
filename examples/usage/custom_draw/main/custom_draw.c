/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "custom_draw.h"

#include <stdlib.h>
#include <string.h>

#define GSP_BUNDLE_ENABLE_RAW_IDS
#include "bundle_gsp.h"
#undef GSP_BUNDLE_ENABLE_RAW_IDS

#ifndef GSP_CUSTOM_DRAW_BIND_WAVE_CANVAS
#error "custom_draw scene did not export GSP_CUSTOM_DRAW_BIND_WAVE_CANVAS"
#endif

static uint8_t triangle_wave(uint32_t value)
{
    uint32_t phase = value & 0xFFU;
    uint32_t folded = phase & 0x7FU;
    uint32_t value8 = folded << 1;
    return (uint8_t)((phase & 0x80U) != 0 ? 255U - value8 : value8);
}

static void custom_draw_draw(
    const esp_gsp_canvas_surface_t *surface, void *user_ctx)
{
    custom_draw_state_t *state = user_ctx;
    const uint32_t phase = atomic_load_explicit(
                               &state->phase, memory_order_relaxed);
    const size_t pixel_bytes = surface->pixel_format ==
                               ESP_GSP_CANVAS_PIXEL_RGB565 ?
                               2U :
                               3U;
    const int32_t center = (int32_t)CUSTOM_DRAW_CANVAS_HEIGHT / 2;
    const int32_t amplitude = center > 4 ? center / 2 : 1;

    for (uint16_t row = 0; row < surface->height; ++row) {
        uint8_t *line = (uint8_t *)surface->pixels +
                        (size_t)row * surface->stride_bytes;
        for (uint16_t column = 0; column < surface->width; ++column) {
            const int32_t x = (int32_t)surface->x + column;
            const int32_t wave = center +
                                 ((int32_t)triangle_wave(
                                      phase + (uint32_t)x * 3U) -
                                  128) *
                                 amplitude /
                                 128;
            const bool highlight = abs((int32_t)surface->y + row - wave) <= 1;
            const uint8_t red = highlight ? 36U : 18U;
            const uint8_t green = highlight ? 220U : 32U;
            const uint8_t blue = highlight ? 170U : 56U;
            if (pixel_bytes == 2U) {
                const uint16_t native = (uint16_t)(((red >> 3) << 11) |
                                                   ((green >> 2) << 5) |
                                                   (blue >> 3));
                memcpy(line + (size_t)column * pixel_bytes, &native,
                       sizeof(native));
            } else {
                uint8_t *pixel = line + (size_t)column * pixel_bytes;
                pixel[0] = red;
                pixel[1] = green;
                pixel[2] = blue;
            }
        }
    }
    atomic_fetch_add_explicit(&state->draw_calls, 1, memory_order_relaxed);
}

static void custom_draw_extension(
    esp_gsp_handle_t ui,
    const esp_gsp_extension_event_t *event,
    void *user_ctx)
{
    (void)ui;
    custom_draw_state_t *state = user_ctx;
    if (event == NULL || event->struct_size < sizeof(*event)) {
        return;
    }
    if (event->kind != ESP_GSP_EXTENSION_CANVAS_RESULT ||
            event->data.canvas.struct_size < sizeof(event->data.canvas) ||
            event->data.canvas.bind != GSP_CUSTOM_DRAW_BIND_WAVE_CANVAS) {
        return;
    }
    atomic_fetch_add_explicit(&state->canvas_results, 1, memory_order_relaxed);
    if (event->data.canvas.operation == ESP_GSP_CANVAS_CHECK &&
            event->data.canvas.request_id == CUSTOM_DRAW_CHECK_REQUEST_ID) {
        atomic_store_explicit(&state->check_result_status,
                              event->data.canvas.status, memory_order_relaxed);
        atomic_store_explicit(&state->check_result_seen, true,
                              memory_order_release);
    }
    if (event->data.canvas.operation == ESP_GSP_CANVAS_SET_DRAW &&
            event->data.canvas.request_id == CUSTOM_DRAW_DRAW_REQUEST_ID) {
        atomic_store_explicit(&state->draw_result_status,
                              event->data.canvas.status, memory_order_relaxed);
        atomic_store_explicit(&state->draw_result_seen, true,
                              memory_order_release);
        atomic_store_explicit(&state->draw_ready,
                              event->data.canvas.status == GSP_OK,
                              memory_order_release);
    }
}

static void custom_draw_timer(esp_gsp_handle_t ui, void *user_ctx)
{
    custom_draw_state_t *state = user_ctx;
    atomic_fetch_add_explicit(&state->timer_ticks, 1, memory_order_relaxed);
    atomic_fetch_add_explicit(&state->phase, 3, memory_order_relaxed);
    if (!atomic_load_explicit(&state->draw_ready, memory_order_acquire)) {
        return;
    }
    const esp_gsp_err_t ret =
        gsp_custom_draw_wave_canvas_canvas_invalidate(ui);
    atomic_store_explicit(&state->timer_last_error, ret, memory_order_relaxed);
    if (ret != ESP_GSP_OK) {
        atomic_fetch_add_explicit(&state->invalidate_failures, 1,
                                  memory_order_relaxed);
    }
}

void custom_draw_prepare(esp_gsp_handle_t ui, void *ctx)
{
    custom_draw_state_t *state = ctx;
    state->ui = ui;
    atomic_store(&state->prepare_status, ESP_GSP_OK);
    atomic_store(&state->observer_submit_status, CUSTOM_DRAW_STATUS_UNSEEN);
    atomic_store(&state->check_submit_status, CUSTOM_DRAW_STATUS_UNSEEN);
    atomic_store(&state->draw_submit_status, CUSTOM_DRAW_STATUS_UNSEEN);
    atomic_store(&state->check_result_status, CUSTOM_DRAW_STATUS_UNSEEN);
    atomic_store(&state->draw_result_status, CUSTOM_DRAW_STATUS_UNSEEN);
    atomic_store(&state->timer_last_error, CUSTOM_DRAW_STATUS_UNSEEN);

    esp_gsp_err_t ret = esp_gsp_on_extension_event_ex(
                            ui, custom_draw_extension, state,
                            ESP_GSP_EXTENSION_MASK_CANVAS_RESULT,
                            ESP_GSP_CANVAS_MASK_CHECK | ESP_GSP_CANVAS_MASK_SET_DRAW);
    atomic_store(&state->observer_submit_status, ret);
    if (ret != ESP_GSP_OK) {
        atomic_store(&state->prepare_status, ret);
    }

    ret = esp_gsp_canvas_check(
              ui, GSP_CUSTOM_DRAW_BIND_WAVE_CANVAS, CUSTOM_DRAW_CHECK_REQUEST_ID);
    atomic_store(&state->check_submit_status, ret);
    if (ret != ESP_GSP_OK &&
            atomic_load(&state->prepare_status) == ESP_GSP_OK) {
        atomic_store(&state->prepare_status, ret);
    }

    ret = esp_gsp_canvas_set_draw_cb_ex(
              ui, GSP_CUSTOM_DRAW_BIND_WAVE_CANVAS, custom_draw_draw, state,
              CUSTOM_DRAW_DRAW_REQUEST_ID);
    atomic_store(&state->draw_submit_status, ret);
    if (ret != ESP_GSP_OK &&
            atomic_load(&state->prepare_status) == ESP_GSP_OK) {
        atomic_store(&state->prepare_status, ret);
    }

    state->timer = esp_gsp_timer_create(ui, 100, custom_draw_timer, state);
    if (state->timer == NULL &&
            atomic_load(&state->prepare_status) == ESP_GSP_OK) {
        atomic_store(&state->prepare_status, ESP_GSP_ERR_NO_MEM);
    }
}
