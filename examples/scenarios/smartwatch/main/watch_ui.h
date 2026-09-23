/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#pragma once
#include "esp_gsp.h"
enum { WATCH_PAGE_COUNT = 6 };
typedef struct {
    void *timer, *page_timer;
    unsigned page, detail, seconds, battery, trail, glow;
    bool replied, drawer_seen_open, breath_paused;
    esp_gsp_err_t last_error;
} watch_ui_t;
esp_gsp_err_t watch_ui_init(esp_gsp_handle_t ui, watch_ui_t *state);
void watch_ui_deinit(esp_gsp_handle_t ui, watch_ui_t *state);
