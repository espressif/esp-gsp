/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once
#include "esp_gsp.h"
typedef struct {
    esp_gsp_handle_t ui;
    esp_gsp_list_t list;
    esp_gsp_grid_t grid;
    void *timer;
    uint32_t ticks, list_rows, grid_cells, draws;
    uint32_t image_completions, image_releases;
    gsp_err_t image_status;
    esp_gsp_err_t last_error;
} media_ui_t;
esp_gsp_err_t media_ui_init(esp_gsp_handle_t ui, media_ui_t *state);
void media_ui_deinit(esp_gsp_handle_t ui, media_ui_t *state);
