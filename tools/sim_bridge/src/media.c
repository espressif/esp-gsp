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

#define ROW_CAPACITY 256
#define MAX_BINARY_BYTES (16U * 1024U * 1024U)
#define CANVAS_QUEUE_CAPACITY 8
struct bridge_canvas_frame {
    uint16_t bind, scene;
    const void *pixels;
    size_t stride;
    bool has_dirty;
    gsp_rect_t dirty;
    esp_gsp_canvas_release_cb_t release;
    void *ctx;
};
struct bridge_binding {
    struct bridge_binding *next;
    uint16_t id;
    esp_gsp_list_bind_cb_t list_cb;
    esp_gsp_grid_bind_cb_t grid_cb;
    void *ctx;
};
struct bridge_canvas {
    struct bridge_canvas *next;
    uint16_t bind, scene, width, height;
    uint8_t format;
    size_t stride;
    void *pixels;
    esp_gsp_canvas_draw_cb_t draw;
    void *ctx;
    bool pending;
};
struct bridge_media {
    struct bridge_binding *bindings;
    struct bridge_canvas *canvases;
    esp_gsp_grid_cell_t rows[ROW_CAPACITY];
    unsigned head, count;
    struct bridge_canvas_frame frames[CANVAS_QUEUE_CAPACITY];
    unsigned frame_head, frame_count;
};

static struct bridge_media *media(esp_gsp_handle_t ui, bool create)
{
    struct bridge_media **slot = bridge_media_slot(ui);
    if (!slot) {
        return NULL;
    }
    if (!*slot && create) {
        *slot = calloc(1, sizeof(**slot));
    }
    return *slot;
}

bool bridge_media_pending(esp_gsp_handle_t ui)
{
    struct bridge_media *m = media(ui, false);
    if (!m) {
        return false;
    }
    if (m->count || m->frame_count) {
        return true;
    }
    for (struct bridge_canvas *c = m->canvases; c; c = c->next)
        if (c->draw && c->pending) {
            return true;
        }
    return false;
}

void bridge_media_scene_changed(esp_gsp_handle_t ui)
{
    struct bridge_media *m = media(ui, false);
    for (struct bridge_canvas *c = m ? m->canvases : NULL; c; c = c->next) {
        c->draw = NULL; c->pending = false;
        /* A read-only RPC inside draw may receive this notification.
         * Keep the in-use buffer alive until replacement or close. */
    }
}

bool bridge_canvas_pending(esp_gsp_handle_t ui)
{
    struct bridge_media *m = media(ui, false);
    if (m && m->frame_count) {
        return true;
    }
    for (struct bridge_canvas *c = m ? m->canvases : NULL; c; c = c->next)
        if (c->draw && c->pending) {
            return true;
        }
    return false;
}

bool bridge_media_notify(esp_gsp_handle_t ui, const char *method, const char *params)
{
    if (!strncmp(method, "\"list_bind_overflow\"", 20)) {
        fprintf(stderr, "sim_bridge: host row queue overflow; restart the session\n");
        return false;
    }
    if (strncmp(method, "\"list_bind\"", 11)) {
        return true;
    }
    struct bridge_media *m = media(ui, true);
    if (!m || m->count == ROW_CAPACITY) {
        return false;
    }
    const char *names[] = {"list", "slot", "instance", "item", "resource_slot", "text_slot"};
    uint32_t v[6];
    for (unsigned i = 0; i < 6; ++i) {
        int64_t n = bridge_json_number(params, names[i], i >= 4 ? UINT16_MAX : -1);
        if (n < 0 || n > UINT32_MAX || ((i < 2 || i >= 4) && n > UINT16_MAX)) {
            return false;
        }
        v[i] = (uint32_t)n;
    }
    m->rows[(m->head + m->count++) % ROW_CAPACITY] = (esp_gsp_grid_cell_t) {
        .row = { .list = v[0], .slot = v[1], .instance = v[2], .item = v[3] },
        .resource_slot = v[4], .text_slot = v[5],
    };
    return true;
}

static uint16_t bind_component(esp_gsp_handle_t ui, uint32_t key,
                               esp_gsp_list_bind_cb_t list_cb, esp_gsp_grid_bind_cb_t grid_cb, void *ctx, bool grid)
{
    if (!ui || bridge_drawing(ui) || (grid && !grid_cb) ||
            ((grid || list_cb) && !bridge_media_enabled(ui))) {
        return ESP_GSP_LIST_NONE;
    }
    struct bridge_media *m = media(ui, true);
    struct bridge_binding *binding = m ? calloc(1, sizeof(*binding)) : NULL;
    if (!binding) {
        return ESP_GSP_LIST_NONE;
    }
    char params[80];
    snprintf(params, sizeof(params), "{\"component_key\":%" PRIu32 "}", key);
    char *reply = bridge_rpc(ui, grid ? "grid_bind_remote" :
                             list_cb ? "list_bind_remote" : "list_bind_component", params);
    int64_t id = reply ? bridge_json_number(reply, "list", -1) : -1;
    free(reply);
    if (id < 0 || id >= ESP_GSP_LIST_NONE) {
        free(binding);
        return ESP_GSP_LIST_NONE;
    }
    binding->id = (uint16_t)id; binding->list_cb = list_cb; binding->grid_cb = grid_cb;
    binding->ctx = ctx; binding->next = m->bindings; m->bindings = binding;
    return binding->id;
}

esp_gsp_list_t esp_gsp_list_bind_component(esp_gsp_handle_t ui, gsp_component_key_t key,
        esp_gsp_list_bind_cb_t cb, void *ctx)
{
    return bind_component(ui, key, cb, NULL, ctx, false);
}
esp_gsp_grid_t esp_gsp_grid_bind_component(esp_gsp_handle_t ui, gsp_component_key_t key,
        esp_gsp_grid_bind_cb_t cb, void *ctx)
{
    return bind_component(ui, key, NULL, cb, ctx, true);
}

esp_gsp_err_t esp_gsp_list_set_total(esp_gsp_handle_t ui, esp_gsp_list_t list, uint32_t total)
{
    char params[96];
    snprintf(params, sizeof(params), "{\"list\":%u,\"total\":%" PRIu32 "}", list, total);
    return bridge_command(ui, "list_set_total", params);
}
esp_gsp_err_t esp_gsp_list_refresh(esp_gsp_handle_t ui, esp_gsp_list_t list)
{
    char params[64];
    snprintf(params, sizeof(params), "{\"list\":%u}", list);
    return bridge_command(ui, "list_refresh", params);
}
esp_gsp_err_t esp_gsp_grid_set_total(esp_gsp_handle_t ui, esp_gsp_grid_t grid, uint32_t total)
{
    return esp_gsp_list_set_total(ui, grid, total);
}
esp_gsp_err_t esp_gsp_grid_refresh(esp_gsp_handle_t ui, esp_gsp_grid_t grid)
{
    return esp_gsp_list_refresh(ui, grid);
}
esp_gsp_err_t esp_gsp_list_snap(esp_gsp_handle_t ui, esp_gsp_list_t list, bool enable)
{
    char params[96];
    snprintf(params, sizeof(params), "{\"list\":%u,\"enable\":%s}", list, enable ? "true" : "false");
    return bridge_command(ui, "list_snap", params);
}
esp_gsp_err_t esp_gsp_list_fling(esp_gsp_handle_t ui, esp_gsp_list_t list, int32_t velocity)
{
    char params[96];
    snprintf(params, sizeof(params), "{\"list\":%u,\"velocity_px_s\":%" PRId32 "}", list, velocity);
    return bridge_command(ui, "list_fling", params);
}
esp_gsp_err_t esp_gsp_list_scroll_to(esp_gsp_handle_t ui, esp_gsp_list_t list, int32_t offset)
{
    char params[96];
    snprintf(params, sizeof(params), "{\"list\":%u,\"offset_px\":%" PRId32 "}", list, offset);
    return bridge_command(ui, "list_scroll_to", params);
}

static esp_gsp_err_t publish(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot,
                             const char *kind, const char *value)
{
    if (!ui) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!bridge_media_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    size_t size = strlen(value) + 256;
    char *params = malloc(size);
    if (!params) {
        return ESP_GSP_ERR_NO_MEM;
    }
    snprintf(params, size, "{\"list\":%u,\"slot\":%u,\"instance\":%" PRIu32
             ",\"item\":%" PRIu32 ",\"updates\":[{\"kind\":\"%s\",\"slot\":%u,\"%s\":%s}]}",
             row.list, row.slot, row.instance, row.item, kind, slot, kind, value);
    char *reply = bridge_rpc(ui, "row_publish", params);
    free(params);
    esp_gsp_err_t rc = reply ? (esp_gsp_err_t)bridge_json_first_number(reply, "result_codes", ESP_GSP_FAIL) : ESP_GSP_FAIL;
    free(reply); return rc;
}
esp_gsp_err_t esp_gsp_row_set_text(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot, const char *text)
{
    if (!text) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    char *quoted = bridge_json_quote(text);
    if (!quoted) {
        return ESP_GSP_ERR_NO_MEM;
    }
    esp_gsp_err_t rc = publish(ui, row, slot, "text", quoted);
    free(quoted); return rc;
}
esp_gsp_err_t esp_gsp_row_set_value(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot, uint32_t value)
{
    char number[32]; snprintf(number, sizeof(number), "%" PRIu32, value);
    return publish(ui, row, slot, "value", number);
}
esp_gsp_err_t esp_gsp_row_set_color(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot, uint32_t color)
{
    char number[32]; snprintf(number, sizeof(number), "%" PRIu32, color);
    return publish(ui, row, slot, "color", number);
}
esp_gsp_err_t esp_gsp_row_text(esp_gsp_handle_t ui, esp_gsp_row_t row, const char *text)
{
    return esp_gsp_row_set_text(ui, row, UINT16_MAX, text);
}
esp_gsp_err_t esp_gsp_row_value(esp_gsp_handle_t ui, esp_gsp_row_t row, uint32_t value)
{
    return esp_gsp_row_set_value(ui, row, UINT16_MAX, value);
}
esp_gsp_err_t esp_gsp_row_color(esp_gsp_handle_t ui, esp_gsp_row_t row, uint32_t color)
{
    return esp_gsp_row_set_color(ui, row, UINT16_MAX, color);
}
esp_gsp_err_t esp_gsp_grid_cell_set_text(esp_gsp_handle_t ui, esp_gsp_grid_cell_t cell, const char *text)
{
    return cell.text_slot == UINT16_MAX ? ESP_GSP_ERR_NOT_SUPPORTED : esp_gsp_row_set_text(ui, cell.row, cell.text_slot, text);
}

esp_gsp_err_t esp_gsp_set_image(esp_gsp_handle_t ui, uint16_t bind, const void *data, size_t size)
{
    char headers[96];
    snprintf(headers, sizeof(headers), "X-GSP-Kind: image\r\nX-GSP-Bind: %u\r\n", bind);
    return bridge_binary(ui, headers, data, size);
}
esp_gsp_err_t esp_gsp_component_set_image(esp_gsp_handle_t ui, gsp_component_key_t key, const void *data, size_t size)
{
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!bridge_media_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    uint32_t args[8] = {key}, out[8];
    esp_gsp_err_t rc = bridge_scalar(ui, GSP_BRIDGE_COMPONENT_RESOURCE_BIND, args, out);
    return rc == ESP_GSP_OK ? esp_gsp_set_image(ui, (uint16_t)out[0], data, size) : rc;
}
esp_gsp_err_t esp_gsp_row_set_image(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot, const void *data, size_t size)
{
    char headers[256];
    snprintf(headers, sizeof(headers),
             "X-GSP-Kind: row-image\r\nX-GSP-List: %u\r\nX-GSP-Slot: %u\r\n"
             "X-GSP-Instance: %" PRIu32 "\r\nX-GSP-Item: %" PRIu32 "\r\nX-GSP-Resource-Slot: %u\r\n",
             row.list, row.slot, row.instance, row.item, slot);
    return bridge_binary(ui, headers, data, size);
}
esp_gsp_err_t esp_gsp_grid_cell_set_image(esp_gsp_handle_t ui, esp_gsp_grid_cell_t cell, const void *data, size_t size)
{
    return cell.resource_slot == UINT16_MAX ? ESP_GSP_ERR_NOT_SUPPORTED : esp_gsp_row_set_image(ui, cell.row, cell.resource_slot, data, size);
}

static struct bridge_canvas *canvas_find(esp_gsp_handle_t ui, uint16_t bind)
{
    struct bridge_media *m = media(ui, false);
    for (struct bridge_canvas *c = m ? m->canvases : NULL; c; c = c->next)
        if (c->bind == bind && c->scene == gsp_sim_bridge_current_scene(ui)) {
            return c;
        }
    return NULL;
}
static esp_gsp_err_t canvas_info(esp_gsp_handle_t ui, uint16_t bind, uint32_t out[8])
{
    if (!ui) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (!bridge_media_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    uint32_t args[8] = {bind};
    return bridge_scalar(ui, GSP_BRIDGE_CANVAS_INFO, args, out);
}
static bool valid_dirty(gsp_rect_t r, uint32_t width, uint32_t height)
{
    return r.x1 >= 0 && r.y1 >= 0 && r.x2 > r.x1 && r.y2 > r.y1 && (uint32_t)r.x2 <= width && (uint32_t)r.y2 <= height;
}
static esp_gsp_err_t canvas_upload(esp_gsp_handle_t ui, uint16_t bind, const void *pixels,
                                   size_t stride, const uint32_t info[8], const gsp_rect_t *dirty)
{
    if (!pixels || !info[1] || stride < (size_t)info[0] * (info[2] == 0 ? 2 : 3) ||
            stride > MAX_BINARY_BYTES / info[1] || (dirty && !valid_dirty(*dirty, info[0], info[1]))) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    char headers[384];
    int count = snprintf(headers, sizeof(headers),
                         "X-GSP-Kind: canvas\r\nX-GSP-Bind: %u\r\nX-GSP-Stride: %zu\r\nX-GSP-Height: %" PRIu32 "\r\nX-GSP-Scene: %" PRIu32 "\r\n",
                         bind, stride, info[1], info[3]);
    if (dirty) snprintf(headers + count, sizeof(headers) - (size_t)count,
                            "X-GSP-Dirty: rect\r\nX-GSP-X1: %d\r\nX-GSP-Y1: %d\r\nX-GSP-X2: %d\r\nX-GSP-Y2: %d\r\n",
                            (int)dirty->x1, (int)dirty->y1, (int)dirty->x2, (int)dirty->y2);
    return bridge_binary(ui, headers, pixels, stride * info[1]);
}
static esp_gsp_err_t canvas_push(esp_gsp_handle_t ui, uint16_t bind, const void *pixels,
                                 size_t stride, const gsp_rect_t *dirty, esp_gsp_canvas_release_cb_t release, void *ctx)
{
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    uint32_t info[8];
    esp_gsp_err_t rc = canvas_info(ui, bind, info);
    if (rc == ESP_GSP_OK) {
        rc = canvas_upload(ui, bind, pixels, stride, info, dirty);
    }
    if (rc == ESP_GSP_OK) {
        struct bridge_canvas *c = canvas_find(ui, bind);
        if (c) {
            c->draw = NULL;
            c->pending = false;
        }
        /* The host owns its copy now; it never retains the native pointer. */
        if (release) {
            release(ctx);
        }
    }
    return rc;
}

static esp_gsp_err_t canvas_try_push(esp_gsp_handle_t ui, uint16_t bind,
                                     const void *pixels, size_t stride, const gsp_rect_t *dirty,
                                     esp_gsp_canvas_release_cb_t release, void *ctx)
{
    if (!ui || !pixels || !stride || stride > MAX_BINARY_BYTES ||
            (dirty && (dirty->x1 < 0 || dirty->y1 < 0 || dirty->x2 <= dirty->x1 || dirty->y2 <= dirty->y1))) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (!bridge_canvas_queue_allowed(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!bridge_media_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    struct bridge_media *m = media(ui, true);
    if (!m) {
        return ESP_GSP_ERR_NO_MEM;
    }
    if (m->frame_count == CANVAS_QUEUE_CAPACITY) {
        return ESP_GSP_ERR_TIMEOUT;
    }
    struct bridge_canvas_frame *f = &m->frames[(m->frame_head + m->frame_count++) % CANVAS_QUEUE_CAPACITY];
    *f = (struct bridge_canvas_frame) {
        .bind = bind, .scene = gsp_sim_bridge_current_scene(ui), .pixels = pixels,
        .stride = stride, .has_dirty = dirty != NULL, .release = release, .ctx = ctx,
    };
    if (dirty) {
        f->dirty = *dirty;
    }
    struct bridge_canvas *c = canvas_find(ui, bind);
    if (c) {
        c->draw = NULL;
        c->pending = false;
    }
    return ESP_GSP_OK;
}

esp_gsp_err_t esp_gsp_canvas_try_push(esp_gsp_handle_t ui, uint16_t bind, const void *pixels,
                                      size_t stride, esp_gsp_canvas_release_cb_t release, void *ctx)
{
    return canvas_try_push(ui, bind, pixels, stride, NULL, release, ctx);
}

esp_gsp_err_t esp_gsp_canvas_try_push_dirty(esp_gsp_handle_t ui, uint16_t bind, const void *pixels,
        size_t stride, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t release, void *ctx)
{
    return canvas_try_push(ui, bind, pixels, stride, &dirty, release, ctx);
}

static void queued_release(esp_gsp_handle_t ui, const struct bridge_canvas_frame *f)
{
    bool drawing = bridge_drawing(ui);
    bridge_set_drawing(ui, true);
    if (f->release) {
        f->release(f->ctx);
    }
    bridge_set_drawing(ui, drawing);
}

void bridge_canvas_drain(esp_gsp_handle_t ui)
{
    struct bridge_media *m = media(ui, false);
    while (m && m->frame_count) {
        struct bridge_canvas_frame f = m->frames[m->frame_head];
        m->frame_head = (m->frame_head + 1) % CANVAS_QUEUE_CAPACITY; --m->frame_count;
        uint32_t info[8];
        esp_gsp_err_t rc = canvas_info(ui, f.bind, info);
        if (rc == ESP_GSP_OK) {
            rc = info[3] != f.scene ? ESP_GSP_ERR_INVALID_STATE :
                 canvas_upload(ui, f.bind, f.pixels, f.stride, info, f.has_dirty ? &f.dirty : NULL);
        }
        if (rc != ESP_GSP_OK) {
            fprintf(stderr, "sim_bridge: queued Canvas frame rejected (bind=%u code=%d)\n", f.bind, rc);
        }
        queued_release(ui, &f);
    }
}

void bridge_canvas_cancel(esp_gsp_handle_t ui)
{
    struct bridge_media *m = media(ui, false);
    while (m && m->frame_count) {
        struct bridge_canvas_frame f = m->frames[m->frame_head];
        m->frame_head = (m->frame_head + 1) % CANVAS_QUEUE_CAPACITY; --m->frame_count;
        queued_release(ui, &f);
    }
}
esp_gsp_err_t esp_gsp_canvas_push(esp_gsp_handle_t ui, uint16_t bind, const void *pixels, size_t stride, esp_gsp_canvas_release_cb_t release, void *ctx)
{
    return canvas_push(ui, bind, pixels, stride, NULL, release, ctx);
}
esp_gsp_err_t esp_gsp_canvas_push_dirty(esp_gsp_handle_t ui, uint16_t bind, const void *pixels, size_t stride, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t release, void *ctx)
{
    return canvas_push(ui, bind, pixels, stride, &dirty, release, ctx);
}

esp_gsp_err_t esp_gsp_canvas_set_draw_cb(esp_gsp_handle_t ui, uint16_t bind, esp_gsp_canvas_draw_cb_t draw, void *ctx)
{
    if (!draw) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    uint32_t info[8];
    esp_gsp_err_t rc = canvas_info(ui, bind, info);
    if (rc != ESP_GSP_OK) {
        return rc;
    }
    size_t stride = (size_t)info[0] * (info[2] == 0 ? 2U : 3U);
    if (!info[1] || stride > MAX_BINARY_BYTES / info[1]) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    struct bridge_media *m = media(ui, true);
    if (!m) {
        return ESP_GSP_ERR_NO_MEM;
    }
    struct bridge_canvas *c = canvas_find(ui, bind);
    bool created = !c;
    if (!c) {
        c = calloc(1, sizeof(*c));
    }
    if (!c) {
        return ESP_GSP_ERR_NO_MEM;
    }
    void *pixels = calloc(info[1], stride);
    if (!pixels) {
        if (created) {
            free(c);
        } return ESP_GSP_ERR_NO_MEM;
    }
    free(c->pixels); c->pixels = pixels; c->stride = stride;
    c->bind = bind; c->scene = info[3]; c->width = info[0]; c->height = info[1]; c->format = info[2];
    c->draw = draw; c->ctx = ctx; c->pending = true;
    if (created) {
        c->next = m->canvases;
        m->canvases = c;
    }
    return ESP_GSP_OK;
}
esp_gsp_err_t esp_gsp_canvas_invalidate(esp_gsp_handle_t ui, uint16_t bind)
{
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    struct bridge_canvas *c = canvas_find(ui, bind);
    if (!c || !c->draw) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    c->pending = true; return ESP_GSP_OK;
}
esp_gsp_err_t esp_gsp_canvas_invalidate_dirty(esp_gsp_handle_t ui, uint16_t bind, gsp_rect_t dirty)
{
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    struct bridge_canvas *c = canvas_find(ui, bind);
    if (!c || !c->draw) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!valid_dirty(dirty, c->width, c->height)) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    c->pending = true; return ESP_GSP_OK; /* Offscreen mode repaints a full frame. */
}
esp_gsp_err_t esp_gsp_canvas_stop(esp_gsp_handle_t ui, uint16_t bind)
{
    char params[64]; snprintf(params, sizeof(params), "{\"bind_id\":%u}", bind);
    esp_gsp_err_t rc = bridge_command(ui, "canvas_stop", params);
    if (rc == ESP_GSP_OK) {
        struct bridge_canvas *c = canvas_find(ui, bind);
        if (c) {
            c->draw = NULL;
            c->ctx = NULL;
            c->pending = false;
            free(c->pixels);
            c->pixels = NULL;
        }
    }
    return rc;
}

esp_gsp_err_t bridge_media_poll(esp_gsp_handle_t ui)
{
    struct bridge_media *m = media(ui, false);
    if (!m) {
        return ESP_GSP_OK;
    }
    unsigned budget = ROW_CAPACITY;
    while (m->count && budget--) {
        esp_gsp_grid_cell_t cell = m->rows[m->head];
        m->head = (m->head + 1) % ROW_CAPACITY; --m->count;
        for (struct bridge_binding *b = m->bindings; b; b = b->next) {
            if (b->id != cell.row.list) {
                continue;
            }
            gsp_err_t rc = GSP_OK;
            if (b->grid_cb) {
                rc = b->grid_cb(ui, cell, cell.row.item, b->ctx);
            } else if (b->list_cb) {
                rc = b->list_cb(ui, cell.row, cell.row.item, b->ctx);
            }
            if (rc != GSP_OK) {
                fprintf(stderr, "sim_bridge: row binder returned %d (list=%u item=%" PRIu32 ")\n", rc, cell.row.list, cell.row.item);
            }
            break;
        }
    }
    for (struct bridge_canvas *c = m->canvases; c; c = c->next) {
        if (!c->draw || !c->pending) {
            continue;
        }
        if (c->scene != gsp_sim_bridge_current_scene(ui)) {
            c->draw = NULL;
            c->pending = false;
            continue;
        }
        c->pending = false;
        const esp_gsp_canvas_surface_t surface = {
            .pixels = c->pixels, .stride_bytes = c->stride, .x = 0, .y = 0,
            .width = c->width, .height = c->height, .pixel_format = c->format,
        };
        bridge_set_drawing(ui, true); c->draw(&surface, c->ctx); bridge_set_drawing(ui, false);
        if (!c->draw || c->scene != gsp_sim_bridge_current_scene(ui)) {
            continue;
        }
        uint32_t info[8] = {c->width, c->height, c->format, c->scene};
        esp_gsp_err_t rc = canvas_upload(ui, c->bind, c->pixels, c->stride, info, NULL);
        if (rc != ESP_GSP_OK) {
            return rc;
        }
    }
    return ESP_GSP_OK;
}
void bridge_media_close(esp_gsp_handle_t ui)
{
    bridge_canvas_cancel(ui);
    struct bridge_media *m = media(ui, false);
    if (!m) {
        return;
    }
    while (m->bindings) {
        struct bridge_binding *b = m->bindings;
        m->bindings = b->next;
        free(b);
    }
    while (m->canvases) {
        struct bridge_canvas *c = m->canvases;
        m->canvases = c->next;
        free(c->pixels);
        free(c);
    }
    free(m); *bridge_media_slot(ui) = NULL;
}
