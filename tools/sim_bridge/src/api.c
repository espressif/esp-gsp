/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "transport.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static esp_gsp_err_t pair(esp_gsp_handle_t ui, const char *method,
                          const char *key, uint32_t id, const char *name, int64_t value)
{
    char params[160];
    snprintf(params, sizeof(params), "{\"%s\":%" PRIu32 ",\"%s\":%" PRId64 "}", key, id, name, value);
    return bridge_command(ui, method, params);
}

static esp_gsp_err_t single(esp_gsp_handle_t ui, const char *method,
                            const char *key, uint32_t value)
{
    char params[96];
    snprintf(params, sizeof(params), "{\"%s\":%" PRIu32 "}", key, value);
    return bridge_command(ui, method, params);
}

esp_gsp_err_t esp_gsp_set_value(esp_gsp_handle_t ui, uint16_t bind, int32_t value)
{
    return pair(ui, "set_value", "bind_id", bind, "value", value);
}
esp_gsp_err_t esp_gsp_set_color(esp_gsp_handle_t ui, uint16_t bind, uint32_t value)
{
    return pair(ui, "set_color", "bind_id", bind, "color", value);
}
esp_gsp_err_t esp_gsp_set_visible(esp_gsp_handle_t ui, uint16_t bind, bool value)
{
    char params[96];
    snprintf(params, sizeof(params), "{\"bind_id\":%u,\"visible\":%s}", bind, value ? "true" : "false");
    return bridge_command(ui, "set_visible", params);
}
static esp_gsp_err_t text_command(esp_gsp_handle_t ui, const char *method, const char *key, uint32_t id, const char *text)
{
    if (!text) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    char *quoted = bridge_json_quote(text);
    if (!quoted) {
        return ESP_GSP_ERR_NO_MEM;
    }
    size_t size = strlen(quoted) + 96;
    char *params = malloc(size);
    if (!params) {
        free(quoted);
        return ESP_GSP_ERR_NO_MEM;
    }
    snprintf(params, size, "{\"%s\":%" PRIu32 ",\"text\":%s}", key, id, quoted);
    esp_gsp_err_t rc = bridge_command(ui, method, params);
    free(params); free(quoted); return rc;
}
esp_gsp_err_t esp_gsp_set_text(esp_gsp_handle_t ui, uint16_t bind, const char *text)
{
    return text_command(ui, "set_text", "bind_id", bind, text);
}
esp_gsp_err_t esp_gsp_component_set_text(esp_gsp_handle_t ui, gsp_component_key_t key, const char *text)
{
    return text_command(ui, "component_set_text", "component_key", key, text);
}

#define GETTER(name, type, op) \
esp_gsp_err_t name(esp_gsp_handle_t ui, uint16_t key, type *out) { \
    if (!out) return ESP_GSP_ERR_INVALID_ARG; \
    uint32_t args[8] = {key}, values[8]; \
    esp_gsp_err_t rc = bridge_scalar(ui, op, args, values); \
    if (rc == ESP_GSP_OK) *out = (type)values[0]; \
    return rc; \
}
GETTER(esp_gsp_get_value, int32_t, GSP_BRIDGE_GET_VALUE)
GETTER(esp_gsp_get_color, uint32_t, GSP_BRIDGE_GET_COLOR)
GETTER(esp_gsp_get_visible, bool, GSP_BRIDGE_GET_VISIBLE)
GETTER(esp_gsp_get_toggle, bool, GSP_BRIDGE_GET_TOGGLE)

#define COMPONENT_GETTER(name, type, op) \
esp_gsp_err_t name(esp_gsp_handle_t ui, gsp_component_key_t key, type *out) { \
    if (!out) return ESP_GSP_ERR_INVALID_ARG; \
    uint32_t args[8] = {key}, values[8]; \
    esp_gsp_err_t rc = bridge_scalar(ui, op, args, values); \
    if (rc == ESP_GSP_OK) *out = (type)values[0]; \
    return rc; \
}
COMPONENT_GETTER(esp_gsp_component_get_value, int32_t, GSP_BRIDGE_COMPONENT_GET_VALUE)
COMPONENT_GETTER(esp_gsp_component_get_color, uint32_t, GSP_BRIDGE_COMPONENT_GET_COLOR)
COMPONENT_GETTER(esp_gsp_component_get_visible, bool, GSP_BRIDGE_COMPONENT_GET_VISIBLE)
COMPONENT_GETTER(esp_gsp_component_get_checked, bool, GSP_BRIDGE_COMPONENT_GET_CHECKED)
COMPONENT_GETTER(esp_gsp_component_get_enabled, bool, GSP_BRIDGE_COMPONENT_GET_ENABLED)

/* Existing integer-property RPC is sufficient for the generated VALUE setter. */
esp_gsp_err_t esp_gsp_component_set_value(esp_gsp_handle_t ui, gsp_component_key_t key, int32_t value)
{
    char params[160];
    snprintf(params, sizeof(params), "{\"component_key\":%" PRIu32 ",\"property_key\":%" PRIu32 ",\"value\":%" PRId32 "}",
             key, GSP_PROPERTY_KEY_VALUE, value);
    return bridge_command(ui, "set_component_i32", params);
}
#define COMPONENT_SETTER(name, type, op) \
esp_gsp_err_t name(esp_gsp_handle_t ui, gsp_component_key_t key, type value) { \
    uint32_t args[8] = {key, (uint32_t)value}; return bridge_scalar(ui, op, args, NULL); \
}
COMPONENT_SETTER(esp_gsp_component_set_color, uint32_t, GSP_BRIDGE_COMPONENT_SET_COLOR)
COMPONENT_SETTER(esp_gsp_component_set_visible, bool, GSP_BRIDGE_COMPONENT_SET_VISIBLE)
COMPONENT_SETTER(esp_gsp_component_set_checked, bool, GSP_BRIDGE_COMPONENT_SET_CHECKED)
COMPONENT_SETTER(esp_gsp_component_set_enabled, bool, GSP_BRIDGE_COMPONENT_SET_ENABLED)

esp_gsp_err_t esp_gsp_component_get_info(esp_gsp_handle_t ui, gsp_component_key_t key, esp_gsp_component_info_t *out)
{
    if (!out) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t args[8] = {key}, values[8];
    esp_gsp_err_t rc = bridge_scalar(ui, GSP_BRIDGE_COMPONENT_INFO, args, values);
    if (rc == ESP_GSP_OK) {
        out->key = key; out->kind = (gsp_component_kind_t)values[0];
        out->capabilities = values[1]; out->property_count = values[2];
    }
    return rc;
}

static uint32_t value_bits(const gsp_value_t *v)
{
    return v->type == GSP_VALUE_BOOL ? (uint32_t)v->data.boolean : v->data.u32;
}

esp_gsp_err_t esp_gsp_component_get_property(esp_gsp_handle_t ui, gsp_component_key_t key,
        gsp_property_key_t prop, gsp_value_t *out)
{
    if (!out) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t args[8] = {key, prop}, values[8];
    esp_gsp_err_t rc = bridge_scalar(ui, GSP_BRIDGE_GET_PROPERTY, args, values);
    if (rc == ESP_GSP_OK) {
        out->type = (gsp_value_type_t)values[0];
        if (out->type == GSP_VALUE_BOOL) {
            out->data.boolean = values[1] != 0;
        } else {
            out->data.u32 = values[1];
        }
    }
    return rc;
}
esp_gsp_err_t esp_gsp_component_set_property(esp_gsp_handle_t ui, gsp_component_key_t key,
        gsp_property_key_t prop, const gsp_value_t *value)
{
    if (!value || value->type > GSP_VALUE_COLOR) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (value->type == GSP_VALUE_I32) {
        char params[160];
        snprintf(params, sizeof(params), "{\"component_key\":%" PRIu32 ",\"property_key\":%" PRIu32 ",\"value\":%" PRId32 "}",
                 key, prop, value->data.i32);
        return bridge_command(ui, "set_component_i32", params);
    }
    uint32_t args[8] = {key, prop, value->type, value_bits(value)};
    return bridge_scalar(ui, GSP_BRIDGE_SET_PROPERTY, args, NULL);
}

#define ANIMATE(name, key_type, value_type, op) \
esp_gsp_err_t name(esp_gsp_handle_t ui, key_type key, value_type from, value_type to, uint32_t ms, esp_gsp_ease_t ease) { \
    uint32_t args[8] = {key, (uint32_t)from, (uint32_t)to, ms, (uint32_t)ease}; \
    return bridge_scalar(ui, op, args, NULL); \
}
ANIMATE(esp_gsp_animate, uint16_t, int32_t, GSP_BRIDGE_ANIMATE)
ANIMATE(esp_gsp_animate_color, uint16_t, uint32_t, GSP_BRIDGE_ANIMATE_COLOR)
ANIMATE(esp_gsp_component_animate_value, gsp_component_key_t, int32_t, GSP_BRIDGE_COMPONENT_ANIMATE_VALUE)
ANIMATE(esp_gsp_component_animate_color, gsp_component_key_t, uint32_t, GSP_BRIDGE_COMPONENT_ANIMATE_COLOR)
#define ANIMATE_TO(name, value_type, op) \
esp_gsp_err_t name(esp_gsp_handle_t ui, gsp_component_key_t key, value_type to, uint32_t ms, esp_gsp_ease_t ease) { \
    uint32_t args[8] = {key, (uint32_t)to, ms, (uint32_t)ease}; \
    return bridge_scalar(ui, op, args, NULL); \
}
ANIMATE_TO(esp_gsp_component_animate_value_to, int32_t, GSP_BRIDGE_COMPONENT_ANIMATE_VALUE_TO)
ANIMATE_TO(esp_gsp_component_animate_color_to, uint32_t, GSP_BRIDGE_COMPONENT_ANIMATE_COLOR_TO)
esp_gsp_err_t esp_gsp_component_animate_property(esp_gsp_handle_t ui, gsp_component_key_t key,
        gsp_property_key_t prop, const gsp_value_t *from, const gsp_value_t *to, uint32_t ms, esp_gsp_ease_t ease)
{
    if (!from || !to || from->type != to->type || to->type > GSP_VALUE_COLOR) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t args[8] = {key, prop, to->type, value_bits(from), value_bits(to), ms, (uint32_t)ease};
    return bridge_scalar(ui, GSP_BRIDGE_ANIMATE_PROPERTY, args, NULL);
}
esp_gsp_err_t esp_gsp_component_animate_property_to(esp_gsp_handle_t ui, gsp_component_key_t key,
        gsp_property_key_t prop, const gsp_value_t *to, uint32_t ms, esp_gsp_ease_t ease)
{
    if (!to || to->type > GSP_VALUE_COLOR) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t args[8] = {key, prop, to->type, 0, value_bits(to), ms, (uint32_t)ease};
    return bridge_scalar(ui, GSP_BRIDGE_ANIMATE_PROPERTY_TO, args, NULL);
}

esp_gsp_err_t esp_gsp_goto_scene(esp_gsp_handle_t ui, uint16_t scene, esp_gsp_transition_t transition)
{
    return pair(ui, "goto_scene", "scene", scene, "transition", transition);
}
esp_gsp_err_t esp_gsp_keyboard_attach(esp_gsp_handle_t ui, uint16_t action_id, uint16_t text_bind)
{
    return pair(ui, "keyboard_attach", "action_id", action_id, "text_bind", text_bind);
}
esp_gsp_err_t esp_gsp_set_cursor(esp_gsp_handle_t ui, uint16_t bind)
{
    return single(ui, "set_cursor", "bind_id", bind);
}
esp_gsp_err_t esp_gsp_set_swipe_enabled(esp_gsp_handle_t ui, bool enabled)
{
    return bridge_command(ui, "set_swipe_enabled", enabled ? "{\"enabled\":true}" : "{\"enabled\":false}");
}
esp_gsp_err_t esp_gsp_set_swipe_transition(esp_gsp_handle_t ui, esp_gsp_swipe_transition_t transition)
{
    return single(ui, "set_swipe_transition", "transition", transition);
}
esp_gsp_err_t esp_gsp_set_swipe_fade_black_point(esp_gsp_handle_t ui, uint8_t percent)
{
    return single(ui, "set_swipe_fade_black_point", "drag_percent", percent);
}

static esp_gsp_err_t drawer(esp_gsp_handle_t ui, const char *method, gsp_component_key_t key, bool animated)
{
    char params[128];
    snprintf(params, sizeof(params), "{\"component_key\":%" PRIu32 ",\"animated\":%s}", key, animated ? "true" : "false");
    return bridge_command(ui, method, params);
}
esp_gsp_err_t esp_gsp_drawer_open(esp_gsp_handle_t ui, gsp_component_key_t key, bool animated)
{
    return drawer(ui, "drawer_open", key, animated);
}
esp_gsp_err_t esp_gsp_drawer_close(esp_gsp_handle_t ui, gsp_component_key_t key, bool animated)
{
    return drawer(ui, "drawer_close", key, animated);
}
esp_gsp_err_t esp_gsp_drawer_is_open(esp_gsp_handle_t ui, gsp_component_key_t key, bool *out)
{
    if (!out) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    char params[80];
    snprintf(params, sizeof(params), "{\"component_key\":%" PRIu32 "}", key);
    char *reply = bridge_rpc(ui, "drawer_is_open", params);
    if (!reply) {
        return ESP_GSP_FAIL;
    }
    int64_t value = bridge_json_number(reply, "open", -1);
    free(reply);
    if (value < 0) {
        return ESP_GSP_FAIL;
    }
    *out = value != 0; return ESP_GSP_OK;
}
esp_gsp_err_t esp_gsp_page_flow_set_page(esp_gsp_handle_t ui, gsp_component_key_t key, uint16_t page, bool animated)
{
    char params[160];
    snprintf(params, sizeof(params), "{\"component_key\":%" PRIu32 ",\"page\":%u,\"animated\":%s}", key, page, animated ? "true" : "false");
    return bridge_command(ui, "page_flow_set_page", params);
}
