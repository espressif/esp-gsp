/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "media_ui.h"
#define GSP_BUNDLE_ENABLE_LEGACY_NAMES 1
#include "bundle_gsp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 2x2 opaque QOI: one RGB pixel followed by a three-pixel run. */
static const uint8_t qoi[] = {
    'q', 'o', 'i', 'f', 0, 0, 0, 2, 0, 0, 0, 2, 3, 0,
    0xfe, 40, 180, 220, 0xc2, 0, 0, 0, 0, 0, 0, 0, 1
};
static gsp_err_t list_row(esp_gsp_handle_t ui, esp_gsp_row_t row, uint32_t index, void *ctx)
{
    media_ui_t *state = ctx;
    char text[32]; snprintf(text, sizeof(text), "Record %u", (unsigned)index);
    esp_gsp_err_t rc = esp_gsp_row_text(ui, row, text);
    if (rc == ESP_GSP_OK) {
        ++state->list_rows;
    }
    return rc == ESP_GSP_OK ? GSP_OK : GSP_ERR_INVALID_ARG;
}
static gsp_err_t grid_cell(esp_gsp_handle_t ui, esp_gsp_grid_cell_t cell, uint32_t index, void *ctx)
{
    media_ui_t *state = ctx;
    char text[32]; snprintf(text, sizeof(text), "Cell %u", (unsigned)index);
    esp_gsp_err_t rc = esp_gsp_grid_cell_set_text(ui, cell, text);
    if (rc == ESP_GSP_OK) {
        void *encoded = malloc(sizeof(qoi));
        if (!encoded) {
            return GSP_ERR_LIMIT_EXCEEDED;
        }
        memcpy(encoded, qoi, sizeof(qoi));
        rc = esp_gsp_grid_cell_set_image_owned(ui, cell, encoded, sizeof(qoi));
        if (rc != ESP_GSP_OK) {
            free(encoded);    /* TAKE transfers only on success. */
        }
    }
    if (rc == ESP_GSP_OK) {
        ++state->grid_cells;
    }
    return rc == ESP_GSP_OK ? GSP_OK : GSP_ERR_INVALID_ARG;
}
static void image_released(void *ctx)
{
    media_ui_t *state = ctx;
    ++state->image_releases;
}
static void image_completed(esp_gsp_handle_t ui, uint16_t bind, gsp_err_t status, void *ctx)
{
    (void)ui; (void)bind;
    media_ui_t *state = ctx;
    ++state->image_completions; state->image_status = status;
}
static void draw(const esp_gsp_canvas_surface_t *surface, void *ctx)
{
    media_ui_t *state = ctx;
    for (uint16_t y = 0; y < surface->height; ++y) {
        uint8_t *line = (uint8_t *)surface->pixels + y * surface->stride_bytes;
        for (uint16_t x = 0; x < surface->width; ++x) {
            uint8_t r = (uint8_t)((x + surface->x + state->draws * 9) & 255);
            uint8_t g = (uint8_t)((y + surface->y) & 255);
            if (surface->pixel_format == ESP_GSP_CANVAS_PIXEL_RGB565) {
                uint16_t color = (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | 20);
                memcpy(line + x * 2, &color, sizeof(color));
            } else {
                line[x * 3] = r;
                line[x * 3 + 1] = g;
                line[x * 3 + 2] = 160;
            }
        }
    }
    ++state->draws;
}
static void tick(esp_gsp_handle_t ui, void *ctx)
{
    media_ui_t *state = ctx;
    ++state->ticks;
    state->last_error = esp_gsp_canvas_invalidate(ui, GSP_BIND_SURFACE);
}
esp_gsp_err_t media_ui_init(esp_gsp_handle_t ui, media_ui_t *state)
{
    if (!ui || !state) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    memset(state, 0, sizeof(*state)); state->ui = ui;
    state->list = esp_gsp_list_bind_component(ui, GSP_OBJ_KEY_FEED, list_row, state);
    state->grid = esp_gsp_grid_bind_component(ui, GSP_OBJ_KEY_GALLERY, grid_cell, state);
    if (state->list == ESP_GSP_LIST_NONE || state->grid == ESP_GSP_GRID_NONE) {
        return ESP_GSP_FAIL;
    }
    esp_gsp_err_t rc = esp_gsp_list_set_total(ui, state->list, 30);
    if (rc == ESP_GSP_OK) {
        const esp_gsp_image_options_t options = {
            .ownership = ESP_GSP_IMAGE_BORROW,
            .on_release = image_released, .release_ctx = state,
            .on_complete = image_completed, .complete_ctx = state,
            .cache_key = 0x716f6901, /* Immutable QOI content identity. */
        };
        rc = esp_gsp_set_image_ex(ui, GSP_BIND_ENCODED, qoi, sizeof(qoi), &options);
    }
    if (rc == ESP_GSP_OK) {
        rc = esp_gsp_canvas_set_draw_cb(ui, GSP_BIND_SURFACE, draw, state);
    }
    if (rc != ESP_GSP_OK) {
        return rc;
    }
    state->timer = esp_gsp_timer_create(ui, 250, tick, state);
    return state->timer ? ESP_GSP_OK : ESP_GSP_ERR_NO_MEM;
}
void media_ui_deinit(esp_gsp_handle_t ui, media_ui_t *state)
{
    if (!state) {
        return;
    }
    if (state->timer) {
        (void)esp_gsp_timer_delete(ui, state->timer);
    }
    state->timer = NULL;
    (void)esp_gsp_canvas_stop(ui, GSP_BIND_SURFACE);
}
