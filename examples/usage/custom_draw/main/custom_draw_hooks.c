/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "custom_draw.h"

#include <string.h>

static struct {
    _Atomic uint32_t app_created;
    _Atomic uint32_t app_destroying;
    _Atomic uint32_t scene_changed;
    _Atomic uint32_t actions;
    _Atomic uint32_t canvas_results;
    _Atomic uint32_t render_errors;
    _Atomic uint16_t last_scene_previous;
    _Atomic uint16_t last_scene_current;
    _Atomic uint16_t last_canvas_bind;
    _Atomic uint8_t last_canvas_operation;
    _Atomic int32_t last_canvas_status;
    _Atomic int32_t last_render_error;
} s_hooks;

void esp_gsp_hook_app_created(esp_gsp_handle_t gsp)
{
    (void)gsp;
    atomic_fetch_add_explicit(&s_hooks.app_created, 1, memory_order_relaxed);
}

void esp_gsp_hook_app_destroying(esp_gsp_handle_t gsp)
{
    (void)gsp;
    atomic_fetch_add_explicit(&s_hooks.app_destroying, 1,
                              memory_order_relaxed);
}

void esp_gsp_hook_scene_changed(esp_gsp_handle_t gsp,
                                uint16_t previous_scene,
                                uint16_t current_scene)
{
    (void)gsp;
    atomic_fetch_add_explicit(&s_hooks.scene_changed, 1,
                              memory_order_relaxed);
    atomic_store_explicit(&s_hooks.last_scene_previous, previous_scene,
                          memory_order_relaxed);
    atomic_store_explicit(&s_hooks.last_scene_current, current_scene,
                          memory_order_relaxed);
}

void esp_gsp_hook_action(esp_gsp_handle_t gsp,
                         const esp_gsp_event_t *event)
{
    (void)gsp;
    (void)event;
    atomic_fetch_add_explicit(&s_hooks.actions, 1, memory_order_relaxed);
}

void esp_gsp_hook_canvas_result(
    esp_gsp_handle_t gsp,
    const esp_gsp_canvas_result_t *result)
{
    (void)gsp;
    if (result == NULL || result->struct_size < sizeof(*result)) {
        return;
    }
    atomic_fetch_add_explicit(&s_hooks.canvas_results, 1,
                              memory_order_relaxed);
    atomic_store_explicit(&s_hooks.last_canvas_bind, result->bind,
                          memory_order_relaxed);
    atomic_store_explicit(&s_hooks.last_canvas_operation, result->operation,
                          memory_order_relaxed);
    atomic_store_explicit(&s_hooks.last_canvas_status, result->status,
                          memory_order_relaxed);
}

void esp_gsp_hook_render_error(esp_gsp_handle_t gsp, gsp_err_t error)
{
    (void)gsp;
    atomic_fetch_add_explicit(&s_hooks.render_errors, 1, memory_order_relaxed);
    atomic_store_explicit(&s_hooks.last_render_error, error,
                          memory_order_relaxed);
}

void custom_draw_hooks_snapshot(custom_draw_hook_snapshot_t *out_snapshot)
{
    if (out_snapshot == NULL) {
        return;
    }
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->app_created =
        atomic_load_explicit(&s_hooks.app_created, memory_order_relaxed);
    out_snapshot->app_destroying =
        atomic_load_explicit(&s_hooks.app_destroying, memory_order_relaxed);
    out_snapshot->scene_changed =
        atomic_load_explicit(&s_hooks.scene_changed, memory_order_relaxed);
    out_snapshot->actions =
        atomic_load_explicit(&s_hooks.actions, memory_order_relaxed);
    out_snapshot->canvas_results =
        atomic_load_explicit(&s_hooks.canvas_results, memory_order_relaxed);
    out_snapshot->render_errors =
        atomic_load_explicit(&s_hooks.render_errors, memory_order_relaxed);
    out_snapshot->last_scene_previous =
        atomic_load_explicit(&s_hooks.last_scene_previous, memory_order_relaxed);
    out_snapshot->last_scene_current =
        atomic_load_explicit(&s_hooks.last_scene_current, memory_order_relaxed);
    out_snapshot->last_canvas_bind =
        atomic_load_explicit(&s_hooks.last_canvas_bind, memory_order_relaxed);
    out_snapshot->last_canvas_operation =
        atomic_load_explicit(&s_hooks.last_canvas_operation, memory_order_relaxed);
    out_snapshot->last_canvas_status =
        atomic_load_explicit(&s_hooks.last_canvas_status, memory_order_relaxed);
    out_snapshot->last_render_error =
        atomic_load_explicit(&s_hooks.last_render_error, memory_order_relaxed);
}
