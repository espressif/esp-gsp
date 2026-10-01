/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "esp_gsp.h"
#include "esp_gsp_extension.h"

#define CUSTOM_DRAW_CANVAS_WIDTH 200U
#define CUSTOM_DRAW_CANVAS_HEIGHT 100U
#define CUSTOM_DRAW_CHECK_REQUEST_ID 0x4352434BU
#define CUSTOM_DRAW_DRAW_REQUEST_ID 0x44524157U
#define CUSTOM_DRAW_STATUS_UNSEEN INT32_MIN

typedef struct {
    esp_gsp_handle_t ui;
    void *timer;
    _Atomic uint32_t phase;
    _Atomic uint32_t draw_calls;
    _Atomic uint32_t timer_ticks;
    _Atomic uint32_t canvas_results;
    _Atomic uint32_t invalidate_failures;
    _Atomic bool check_result_seen;
    _Atomic bool draw_result_seen;
    _Atomic bool draw_ready;
    _Atomic int32_t prepare_status;
    _Atomic int32_t observer_submit_status;
    _Atomic int32_t check_submit_status;
    _Atomic int32_t draw_submit_status;
    _Atomic int32_t check_result_status;
    _Atomic int32_t draw_result_status;
    _Atomic int32_t timer_last_error;
} custom_draw_state_t;

typedef struct {
    uint32_t app_created;
    uint32_t app_destroying;
    uint32_t scene_changed;
    uint32_t actions;
    uint32_t canvas_results;
    uint32_t render_errors;
    uint16_t last_scene_previous;
    uint16_t last_scene_current;
    uint16_t last_canvas_bind;
    uint8_t last_canvas_operation;
    int32_t last_canvas_status;
    int32_t last_render_error;
} custom_draw_hook_snapshot_t;

void custom_draw_prepare(esp_gsp_handle_t ui, void *ctx);
void custom_draw_hooks_snapshot(custom_draw_hook_snapshot_t *out_snapshot);
