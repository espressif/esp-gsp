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

#define IMAGE_CAPACITY 128
struct bridge_image {
    struct bridge_image *next;
    uint32_t id;
    uint16_t bind;
    bool accepted, complete_seen, complete_fired, released;
    gsp_err_t status;
    const void *data;
    esp_gsp_image_options_t options;
};
struct bridge_images {
    struct bridge_image *head;
    uint32_t next;
    unsigned count;
};

static struct bridge_images *images(esp_gsp_handle_t ui, bool create)
{
    struct bridge_images **slot = bridge_images_slot(ui);
    if (!slot) {
        return NULL;
    }
    if (!*slot && create) {
        *slot = calloc(1, sizeof(**slot));
    }
    return *slot;
}

static void release_image(struct bridge_image *image)
{
    if (image->options.ownership != ESP_GSP_IMAGE_COPY) {
        if (image->options.on_release) {
            image->options.on_release(image->options.release_ctx);
        } else if (image->options.ownership == ESP_GSP_IMAGE_TAKE) {
            free((void *)image->data);
        }
    }
    free(image);
}

bool bridge_images_pending(esp_gsp_handle_t ui)
{
    struct bridge_images *m = images(ui, false);
    for (struct bridge_image *i = m ? m->head : NULL; i; i = i->next)
        if (i->accepted && ((!i->complete_fired && i->complete_seen) || i->released)) {
            return true;
        }
    return false;
}

bool bridge_images_notify(esp_gsp_handle_t ui, const char *method, const char *params)
{
    if (!bridge_images_enabled(ui)) {
        return true;
    }
    bool complete = !strncmp(method, "\"image_complete\"", 16);
    if (!complete && strncmp(method, "\"image_release\"", 15)) {
        return true;
    }
    int64_t id = bridge_json_number(params, "request", -1);
    int64_t bind = bridge_json_number(params, "bind", -1);
    int64_t status = bridge_json_number(params, "status", INT64_MAX);
    if (id <= 0 || id > UINT32_MAX || bind < 0 || bind > UINT16_MAX ||
            status < INT32_MIN || status > INT32_MAX) {
        return false;
    }
    struct bridge_images *m = images(ui, false);
    for (struct bridge_image *i = m ? m->head : NULL; i; i = i->next) {
        if (i->id != (uint32_t)id) {
            continue;
        }
        if (i->bind != bind || (complete ? i->complete_seen : (i->released || !i->complete_seen))) {
            return false;
        }
        if (complete) {
            i->complete_seen = true;
            i->status = (gsp_err_t)status;
        } else {
            i->released = true;
        }
        return true;
    }
    return false;
}

void bridge_images_poll(esp_gsp_handle_t ui)
{
    struct bridge_images *m = images(ui, false);
    if (!m) {
        return;
    }
    /* Re-scan after each callback: callbacks may submit and prepend images. */
    unsigned budget = IMAGE_CAPACITY * 2;
    while (budget--) {
        struct bridge_image **p = &m->head;
        while (*p && (!(*p)->accepted || (!(*p)->released &&
                                          (!(*p)->complete_seen || (*p)->complete_fired)))) {
            p = &(*p)->next;
        }
        struct bridge_image *i = *p;
        if (!i) {
            break;
        }
        if (!i->complete_fired) {
            i->complete_fired = true;
            if (i->options.on_complete) {
                i->options.on_complete(ui, i->bind, i->status, i->options.complete_ctx);
            }
        } else {
            *p = i->next; --m->count;
            release_image(i);
        }
    }
}

void bridge_images_close(esp_gsp_handle_t ui, gsp_err_t status)
{
    struct bridge_images *m = images(ui, false);
    if (!m) {
        return;
    }
    while (m->head) {
        struct bridge_image *i = m->head;
        m->head = i->next;
        if (i->accepted) {
            if (!i->complete_fired && i->options.on_complete) {
                i->options.on_complete(ui, i->bind, i->complete_seen ? i->status : status, i->options.complete_ctx);
            }
            release_image(i);
        } else {
            free(i);
        }
    }
    free(m); *bridge_images_slot(ui) = NULL;
}

static esp_gsp_err_t submit(esp_gsp_handle_t ui, uint16_t bind, const esp_gsp_row_t *row,
                            bool widget, uint16_t slot, const void *data, size_t size,
                            const esp_gsp_image_options_t *options)
{
    if (!ui || !data || !size || size > 16U * 1024U * 1024U) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (options && (options->ownership < ESP_GSP_IMAGE_COPY || options->ownership > ESP_GSP_IMAGE_TAKE ||
                    (options->ownership == ESP_GSP_IMAGE_COPY && options->on_release) ||
                    (options->ownership == ESP_GSP_IMAGE_BORROW && !options->on_release))) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (!options || (options->ownership == ESP_GSP_IMAGE_COPY && !options->on_complete && !options->cache_key)) {
        if (widget) {
            char headers[160];
            snprintf(headers, sizeof(headers), "X-GSP-Kind: widget-image\r\nX-GSP-Widget: %u\r\nX-GSP-Resource-Slot: %u\r\n", bind, slot);
            return bridge_binary(ui, headers, data, size);
        }
        return row ? esp_gsp_row_set_image(ui, *row, slot, data, size) : esp_gsp_set_image(ui, bind, data, size);
    }
    if (!bridge_images_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    struct bridge_images *m = images(ui, true);
    if (!m) {
        return ESP_GSP_ERR_NO_MEM;
    }
    if (m->count == IMAGE_CAPACITY) {
        return ESP_GSP_ERR_NO_MEM;
    }
    struct bridge_image *image = calloc(1, sizeof(*image));
    if (!image) {
        return ESP_GSP_ERR_NO_MEM;
    }
    bool used;
    do {
        ++m->next; used = m->next == 0;
        for (struct bridge_image *i = m->head; i; i = i->next) if (i->id == m->next) {
                used = true;
            }
    } while (used);
    image->id = m->next; image->bind = bind; image->options = *options;
    image->data = options->ownership == ESP_GSP_IMAGE_COPY ? NULL : data;
    image->next = m->head; m->head = image; ++m->count;
    char headers[512];
    int count = snprintf(headers, sizeof(headers),
                         "X-GSP-Image-Request: %" PRIu32 "\r\nX-GSP-Cache-Key: %" PRIu32 "\r\n", image->id, options->cache_key);
    if (widget) snprintf(headers + count, sizeof(headers) - (size_t)count,
                             "X-GSP-Kind: widget-image\r\nX-GSP-Widget: %u\r\nX-GSP-Resource-Slot: %u\r\n", bind, slot);
    else if (row) snprintf(headers + count, sizeof(headers) - (size_t)count,
                               "X-GSP-Kind: row-image\r\nX-GSP-List: %u\r\nX-GSP-Slot: %u\r\nX-GSP-Instance: %" PRIu32
                               "\r\nX-GSP-Item: %" PRIu32 "\r\nX-GSP-Resource-Slot: %u\r\n", row->list, row->slot, row->instance, row->item, slot);
    else {
        snprintf(headers + count, sizeof(headers) - (size_t)count, "X-GSP-Kind: image\r\nX-GSP-Bind: %u\r\n", bind);
    }
    esp_gsp_err_t rc = bridge_binary(ui, headers, data, size);
    if (rc == ESP_GSP_OK) {
        image->accepted = true;
    } else {
        m->head = image->next;
        --m->count;
        free(image);
    }
    return rc;
}

esp_gsp_err_t esp_gsp_set_image_ex(esp_gsp_handle_t ui, uint16_t bind, const void *data, size_t size, const esp_gsp_image_options_t *options)
{
    return submit(ui, bind, NULL, false, 0, data, size, options);
}
esp_gsp_err_t esp_gsp_row_set_image_ex(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot, const void *data, size_t size, const esp_gsp_image_options_t *options)
{
    return submit(ui, row.list, &row, false, slot, data, size, options);
}
static esp_gsp_err_t widget_submit(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint16_t slot,
                                   const void *data, size_t size, const esp_gsp_image_options_t *options)
{
    if (!ui || widget == ESP_GSP_WIDGET_NONE) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (!bridge_widget_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    return submit(ui, widget, NULL, true, slot, data, size, options);
}
esp_gsp_err_t esp_gsp_widget_set_image(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint16_t slot, const void *data, size_t size)
{
    return widget_submit(ui, widget, slot, data, size, NULL);
}
esp_gsp_err_t esp_gsp_widget_set_image_borrowed(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint16_t slot, const void *data, size_t size, esp_gsp_image_release_cb_t cb, void *ctx)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_BORROW, .on_release = cb, .release_ctx = ctx};
    return widget_submit(ui, widget, slot, data, size, &options);
}
esp_gsp_err_t esp_gsp_widget_set_image_owned(esp_gsp_handle_t ui, esp_gsp_widget_t widget, uint16_t slot, void *data, size_t size)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_TAKE};
    return widget_submit(ui, widget, slot, data, size, &options);
}
esp_gsp_err_t esp_gsp_grid_cell_set_image_ex(esp_gsp_handle_t ui, esp_gsp_grid_cell_t cell, const void *data, size_t size, const esp_gsp_image_options_t *options)
{
    return cell.resource_slot == UINT16_MAX ? ESP_GSP_ERR_NOT_SUPPORTED : esp_gsp_row_set_image_ex(ui, cell.row, cell.resource_slot, data, size, options);
}

esp_gsp_err_t esp_gsp_set_image_borrowed(esp_gsp_handle_t ui, uint16_t bind, const void *data, size_t size, esp_gsp_image_release_cb_t cb, void *ctx)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_BORROW, .on_release = cb, .release_ctx = ctx};
    return esp_gsp_set_image_ex(ui, bind, data, size, &options);
}
esp_gsp_err_t esp_gsp_set_image_owned(esp_gsp_handle_t ui, uint16_t bind, void *data, size_t size)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_TAKE};
    return esp_gsp_set_image_ex(ui, bind, data, size, &options);
}
esp_gsp_err_t esp_gsp_row_set_image_borrowed(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot, const void *data, size_t size, esp_gsp_image_release_cb_t cb, void *ctx)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_BORROW, .on_release = cb, .release_ctx = ctx};
    return esp_gsp_row_set_image_ex(ui, row, slot, data, size, &options);
}
esp_gsp_err_t esp_gsp_row_set_image_owned(esp_gsp_handle_t ui, esp_gsp_row_t row, uint16_t slot, void *data, size_t size)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_TAKE};
    return esp_gsp_row_set_image_ex(ui, row, slot, data, size, &options);
}
esp_gsp_err_t esp_gsp_grid_cell_set_image_borrowed(esp_gsp_handle_t ui, esp_gsp_grid_cell_t cell, const void *data, size_t size, esp_gsp_image_release_cb_t cb, void *ctx)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_BORROW, .on_release = cb, .release_ctx = ctx};
    return esp_gsp_grid_cell_set_image_ex(ui, cell, data, size, &options);
}
esp_gsp_err_t esp_gsp_grid_cell_set_image_owned(esp_gsp_handle_t ui, esp_gsp_grid_cell_t cell, void *data, size_t size)
{
    const esp_gsp_image_options_t options = {.ownership = ESP_GSP_IMAGE_TAKE};
    return esp_gsp_grid_cell_set_image_ex(ui, cell, data, size, &options);
}
