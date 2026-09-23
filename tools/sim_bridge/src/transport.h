/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once
#include "gsp_sim_bridge.h"
#include "../protocol.h"

/* Internal synchronous JSON call. Params must be one JSON object.
 * Returned JSON belongs to the caller. Failure poisons the connection. */
char *bridge_rpc(esp_gsp_handle_t ui, const char *method, const char *params);
char *bridge_json_quote(const char *text);
int64_t bridge_json_number(const char *json, const char *key, int64_t fallback);
esp_gsp_err_t bridge_command(esp_gsp_handle_t ui, const char *method, const char *params);
esp_gsp_err_t bridge_scalar(esp_gsp_handle_t ui, unsigned op, const uint32_t args[8], uint32_t out[8]);

struct bridge_media;
struct bridge_media **bridge_media_slot(esp_gsp_handle_t ui);
bool bridge_media_enabled(esp_gsp_handle_t ui);
bool bridge_canvas_patch_enabled(esp_gsp_handle_t ui);
bool bridge_pointer_enabled(esp_gsp_handle_t ui);
bool bridge_api_extensions_v2(esp_gsp_handle_t ui);
bool bridge_widget_enabled(esp_gsp_handle_t ui);
bool bridge_component_motion_enabled(esp_gsp_handle_t ui);
bool bridge_json_motion(const char *json, esp_gsp_component_motion_t *out);
void bridge_set_pointer_observer(esp_gsp_handle_t ui,
                                 esp_gsp_pointer_observer_cb_t cb, void *user_ctx);
bool bridge_media_pending(esp_gsp_handle_t ui);
void bridge_media_scene_changed(esp_gsp_handle_t ui);
bool bridge_drawing(esp_gsp_handle_t ui);
void bridge_set_drawing(esp_gsp_handle_t ui, bool drawing);
int64_t bridge_json_first_number(const char *json, const char *key, int64_t fallback);
esp_gsp_err_t bridge_binary(esp_gsp_handle_t ui, const char *headers, const void *data, size_t size);
bool bridge_media_notify(esp_gsp_handle_t ui, const char *method, const char *params);
esp_gsp_err_t bridge_media_poll(esp_gsp_handle_t ui);
void bridge_media_close(esp_gsp_handle_t ui);
bool bridge_canvas_pending(esp_gsp_handle_t ui);
bool bridge_canvas_queue_allowed(esp_gsp_handle_t ui);
void bridge_canvas_drain(esp_gsp_handle_t ui);
void bridge_canvas_cancel(esp_gsp_handle_t ui);
struct bridge_images;
struct bridge_images **bridge_images_slot(esp_gsp_handle_t ui);
bool bridge_images_enabled(esp_gsp_handle_t ui);
bool bridge_images_pending(esp_gsp_handle_t ui);
bool bridge_images_notify(esp_gsp_handle_t ui, const char *method, const char *params);
void bridge_images_poll(esp_gsp_handle_t ui);
void bridge_images_close(esp_gsp_handle_t ui, gsp_err_t status);
void bridge_payloads_shutdown(esp_gsp_handle_t ui);
