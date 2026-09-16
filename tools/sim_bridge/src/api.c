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

esp_gsp_widget_t esp_gsp_widget_create(esp_gsp_handle_t ui, uint16_t template_id,
                                       int16_t x, int16_t y)
{
    if (!ui || !bridge_widget_enabled(ui)) {
        return ESP_GSP_WIDGET_NONE;
    }
    char params[128];
    snprintf(params, sizeof(params), "{\"template_id\":%u,\"x\":%d,\"y\":%d}", template_id, x, y);
    char *reply = bridge_rpc(ui, "widget_create", params);
    int64_t widget = reply ? bridge_json_number(reply, "widget", ESP_GSP_WIDGET_NONE) : ESP_GSP_WIDGET_NONE;
    free(reply);
    return widget >= 0 && widget <= UINT16_MAX ? (esp_gsp_widget_t)widget : ESP_GSP_WIDGET_NONE;
}

static esp_gsp_err_t widget_command(esp_gsp_handle_t ui, const char *method,
                                    esp_gsp_widget_t widget, const char *suffix)
{
    if (!ui || widget == ESP_GSP_WIDGET_NONE) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (!bridge_widget_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    size_t size = strlen(suffix) + 64;
    char *params = malloc(size);
    if (!params) {
        return ESP_GSP_ERR_NO_MEM;
    }
    snprintf(params, size, "{\"widget\":%u%s}", widget, suffix);
    esp_gsp_err_t rc = bridge_command(ui, method, params);
    free(params);
    return rc;
}

esp_gsp_err_t esp_gsp_widget_destroy(esp_gsp_handle_t ui, esp_gsp_widget_t widget)
{
    return widget_command(ui, "widget_destroy", widget, "");
}
esp_gsp_err_t esp_gsp_widget_set_value(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint16_t slot, uint32_t value)
{
    char s[96];
    snprintf(s, sizeof(s), ",\"slot\":%u,\"value\":%" PRIu32, slot, value);
    return widget_command(ui, "widget_set_value", widget, s);
}
esp_gsp_err_t esp_gsp_widget_set_color(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint16_t slot, uint32_t color)
{
    char s[96];
    snprintf(s, sizeof(s), ",\"slot\":%u,\"color\":%" PRIu32, slot, color);
    return widget_command(ui, "widget_set_color", widget, s);
}
esp_gsp_err_t esp_gsp_widget_set_text(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint16_t slot, const char *text)
{
    if (!text) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    char *quoted = bridge_json_quote(text); if (!quoted) return ESP_GSP_ERR_NO_MEM;
    size_t n = strlen(quoted) + 96; char *s = malloc(n); if (!s) {
        free(quoted);
        return ESP_GSP_ERR_NO_MEM;
    }
    snprintf(s, n, ",\"slot\":%u,\"text\":%s", slot, quoted);
    esp_gsp_err_t rc = widget_command(ui, "widget_set_text", widget, s); free(s); free(quoted); return rc;
}
esp_gsp_err_t esp_gsp_widget_value(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint32_t value)
{
    return esp_gsp_widget_set_value(ui, widget, UINT16_MAX, value);
}
esp_gsp_err_t esp_gsp_widget_color(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint32_t color)
{
    return esp_gsp_widget_set_color(ui, widget, UINT16_MAX, color);
}
esp_gsp_err_t esp_gsp_widget_text(esp_gsp_handle_t ui, esp_gsp_widget_t widget, const char *text)
{
    return esp_gsp_widget_set_text(ui, widget, UINT16_MAX, text);
}
esp_gsp_err_t esp_gsp_widget_set_visible(esp_gsp_handle_t ui, esp_gsp_widget_t widget, bool visible)
{
    return widget_command(ui, "widget_set_visible", widget, visible ? ",\"visible\":true" : ",\"visible\":false");
}
esp_gsp_err_t esp_gsp_widget_set_position(esp_gsp_handle_t ui, esp_gsp_widget_t widget, int16_t x, int16_t y)
{
    char s[96];
    snprintf(s, sizeof(s), ",\"x\":%d,\"y\":%d", x, y);
    return widget_command(ui, "widget_set_position", widget, s);
}
esp_gsp_err_t esp_gsp_widget_animate(esp_gsp_handle_t ui, esp_gsp_widget_t widget, int32_t from,
                                     int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
{
    char s[160];
    snprintf(s, sizeof(s), ",\"from\":%" PRId32 ",\"to\":%" PRId32 ",\"duration_ms\":%" PRIu32 ",\"ease\":%u", from, to, duration_ms, (unsigned)ease);
    return widget_command(ui, "widget_animate", widget, s);
}

static esp_gsp_err_t chart_validate(const esp_gsp_chart_series_t *series)
{
    return series && series->point_binds && series->capacity >= 2 &&
           series->capacity <= ESP_GSP_CHART_MAX_POINTS &&
           series->plot_top <= series->plot_bottom &&
           series->value_min < series->value_max ? ESP_GSP_OK : ESP_GSP_ERR_INVALID_ARG;
}

esp_gsp_err_t esp_gsp_chart_set_series(esp_gsp_handle_t ui,
                                       const esp_gsp_chart_series_t *series, const int32_t *values, size_t count)
{
    if (!ui || chart_validate(series) != ESP_GSP_OK || !values || count != series->capacity) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    size_t size = 256U + count * 32U;
    char *params = malloc(size);
    if (!params) {
        return ESP_GSP_ERR_NO_MEM;
    }
    int n = snprintf(params, size,
                     "{\"scene_content_id\":%" PRIu32 ",\"plot_top\":%d,\"plot_bottom\":%d,"
                     "\"value_min\":%" PRId32 ",\"value_max\":%" PRId32 ",\"point_binds\":[",
                     series->scene_content_id, series->plot_top, series->plot_bottom,
                     series->value_min, series->value_max);
    for (size_t i = 0; i < count && n > 0 && (size_t)n < size; ++i) {
        n += snprintf(params + n, size - (size_t)n, "%s%u", i ? "," : "", series->point_binds[i]);
    }
    if (n > 0 && (size_t)n < size) {
        n += snprintf(params + n, size - (size_t)n, "],\"values\":[");
    }
    for (size_t i = 0; i < count && n > 0 && (size_t)n < size; ++i) {
        n += snprintf(params + n, size - (size_t)n, "%s%" PRId32, i ? "," : "", values[i]);
    }
    if (n <= 0 || (size_t)n >= size || snprintf(params + n, size - (size_t)n, "]}") >= (int)(size - (size_t)n)) {
        free(params); return ESP_GSP_ERR_INVALID_SIZE;
    }
    esp_gsp_err_t rc = bridge_command(ui, "chart_set_series", params); free(params); return rc;
}

esp_gsp_err_t esp_gsp_chart_append(esp_gsp_handle_t ui,
                                   const esp_gsp_chart_series_t *series, int32_t value)
{
    if (!ui || chart_validate(series) != ESP_GSP_OK) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    size_t size = 224U + (size_t)series->capacity * 8U; char *params = malloc(size);
    if (!params) {
        return ESP_GSP_ERR_NO_MEM;
    }
    int n = snprintf(params, size, "{\"scene_content_id\":%" PRIu32 ",\"plot_top\":%d,\"plot_bottom\":%d,\"value_min\":%" PRId32 ",\"value_max\":%" PRId32 ",\"value\":%" PRId32 ",\"point_binds\":[", series->scene_content_id, series->plot_top, series->plot_bottom, series->value_min, series->value_max, value);
    for (size_t i = 0; i < series->capacity && n > 0 && (size_t)n < size; ++i) {
        n += snprintf(params + n, size - (size_t)n, "%s%u", i ? "," : "", series->point_binds[i]);
    }
    if (n <= 0 || (size_t)n >= size || snprintf(params + n, size - (size_t)n, "]}") >= (int)(size - (size_t)n)) {
        free(params);
        return ESP_GSP_ERR_INVALID_SIZE;
    }
    esp_gsp_err_t rc = bridge_command(ui, "chart_append", params); free(params); return rc;
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

esp_gsp_err_t esp_gsp_component_set_color_rgb888(esp_gsp_handle_t ui,
        gsp_component_key_t key, uint32_t rgb888)
{
    uint32_t args[8] = {key, rgb888};
    return bridge_scalar(ui, GSP_BRIDGE_COMPONENT_SET_COLOR_RGB888, args, NULL);
}

esp_gsp_err_t esp_gsp_component_set_property_color_rgb888(esp_gsp_handle_t ui,
        gsp_component_key_t component, gsp_property_key_t property, uint32_t rgb888)
{
    uint32_t args[8] = {component, property, rgb888};
    return bridge_scalar(ui, GSP_BRIDGE_COMPONENT_SET_PROPERTY_COLOR_RGB888, args, NULL);
}

COMPONENT_GETTER(esp_gsp_component_get_color_rgb888, uint32_t,
                 GSP_BRIDGE_COMPONENT_GET_COLOR_RGB888)

esp_gsp_err_t esp_gsp_set_press_feedback_enabled(esp_gsp_handle_t ui, bool enabled)
{
    uint32_t args[8] = {enabled};
    return bridge_scalar(ui, GSP_BRIDGE_SET_PRESS_FEEDBACK_ENABLED, args, NULL);
}

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

esp_gsp_err_t esp_gsp_component_play_animation(esp_gsp_handle_t ui,
        gsp_component_key_t component, gsp_property_key_t property,
        const gsp_value_t *from, const gsp_value_t *to,
        const esp_gsp_animation_config_t *config)
{
    if (!from || !to || !config || from->type != to->type ||
            to->type > GSP_VALUE_COLOR || config->ease > ESP_GSP_EASE_IN_OUT) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t packed = config->iterations |
                      (config->alternate ? UINT32_C(1) << 16 : 0) |
                      ((uint32_t)config->ease << 17);
    uint32_t args[8] = {component, property, from->type, value_bits(from), value_bits(to),
                        config->duration_ms, config->delay_ms, packed
                       };
    return bridge_scalar(ui, GSP_BRIDGE_COMPONENT_PLAY_ANIMATION, args, NULL);
}

esp_gsp_err_t esp_gsp_component_stop_animation(esp_gsp_handle_t ui,
        gsp_component_key_t component, gsp_property_key_t property)
{
    uint32_t args[8] = {component, property};
    return bridge_scalar(ui, GSP_BRIDGE_COMPONENT_STOP_ANIMATION, args, NULL);
}

esp_gsp_err_t esp_gsp_query_visibility(esp_gsp_handle_t ui,
                                       const esp_gsp_visibility_target_t *target, bool *out_visible)
{
    if (!ui || !target || !out_visible ||
            (target->visible_slot_count != 0 && target->visible_slots == NULL) ||
            target->visible_slot_count > (1024U * 1024U - 256U) / 6U) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (!bridge_api_extensions_v2(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    size_t capacity = 256U + target->visible_slot_count * 6U;
    char *params = malloc(capacity);
    if (!params) {
        return ESP_GSP_ERR_NO_MEM;
    }
    int written = snprintf(params, capacity,
                           "{\"scene_content_id\":%" PRIu32 ",\"x1\":%" PRId32
                           ",\"y1\":%" PRId32 ",\"x2\":%" PRId32 ",\"y2\":%" PRId32
                           ",\"group_ref\":%u,\"visible_slots\":[",
                           target->scene_content_id, target->bounds.x1, target->bounds.y1,
                           target->bounds.x2, target->bounds.y2, target->group_ref);
    if (written < 0 || (size_t)written >= capacity) {
        free(params);
        return ESP_GSP_FAIL;
    }
    size_t used = (size_t)written;
    for (size_t i = 0; i < target->visible_slot_count; ++i) {
        written = snprintf(params + used, capacity - used, "%s%u",
                           i == 0 ? "" : ",", target->visible_slots[i]);
        if (written < 0 || (size_t)written >= capacity - used) {
            free(params);
            return ESP_GSP_FAIL;
        }
        used += (size_t)written;
    }
    written = snprintf(params + used, capacity - used,
                       "],\"width_slot_ref\":%u,\"height_slot_ref\":%u,\"width_min\":%" PRId32
                       ",\"width_max\":%" PRId32 ",\"height_min\":%" PRId32
                       ",\"height_max\":%" PRId32 "}",
                       target->width_slot_ref, target->height_slot_ref, target->width_min,
                       target->width_max, target->height_min, target->height_max);
    if (written < 0 || (size_t)written >= capacity - used) {
        free(params);
        return ESP_GSP_FAIL;
    }
    char *reply = bridge_rpc(ui, "query_visibility", params);
    free(params);
    if (!reply) {
        return ESP_GSP_FAIL;
    }
    int64_t rc = bridge_json_number(reply, "result_code", ESP_GSP_FAIL);
    int64_t visible = bridge_json_number(reply, "visible", -1);
    free(reply);
    if (rc != ESP_GSP_OK) {
        return (esp_gsp_err_t)rc;
    }
    if (visible < 0) {
        return ESP_GSP_FAIL;
    }
    *out_visible = visible != 0;
    return ESP_GSP_OK;
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
esp_gsp_err_t esp_gsp_page_flow_get_page(esp_gsp_handle_t ui, gsp_component_key_t key,
        uint16_t *out_page)
{
    if (!out_page) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t args[8] = {key}, values[8];
    esp_gsp_err_t rc = bridge_scalar(ui, GSP_BRIDGE_PAGE_FLOW_GET_PAGE, args, values);
    if (rc == ESP_GSP_OK) {
        *out_page = (uint16_t)values[0];
    }
    return rc;
}
esp_gsp_err_t esp_gsp_page_flow_get_offset(esp_gsp_handle_t ui, gsp_component_key_t key,
        int32_t *out_offset_px)
{
    if (!out_offset_px) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t args[8] = {key}, values[8];
    esp_gsp_err_t rc = bridge_scalar(ui, GSP_BRIDGE_PAGE_FLOW_GET_OFFSET, args, values);
    if (rc == ESP_GSP_OK) {
        *out_offset_px = (int32_t)values[0];
    }
    return rc;
}
esp_gsp_err_t esp_gsp_page_flow_is_dragging(esp_gsp_handle_t ui, gsp_component_key_t key,
        bool *out_dragging)
{
    if (!out_dragging) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    uint32_t args[8] = {key}, values[8];
    esp_gsp_err_t rc = bridge_scalar(ui, GSP_BRIDGE_PAGE_FLOW_IS_DRAGGING, args, values);
    if (rc == ESP_GSP_OK) {
        *out_dragging = values[0] != 0;
    }
    return rc;
}

esp_gsp_err_t esp_gsp_set_pointer_observer(esp_gsp_handle_t ui,
        esp_gsp_pointer_observer_cb_t cb, void *user_ctx)
{
    if (!ui) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (cb && !bridge_pointer_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    /* Delivery is serialized by gsp_sim_bridge_poll(), never during an RPC. */
    bridge_set_pointer_observer(ui, cb, user_ctx);
    return ESP_GSP_OK;
}
