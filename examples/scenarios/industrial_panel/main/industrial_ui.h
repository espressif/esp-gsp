/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#pragma once
#include "esp_gsp.h"

#define INDUSTRIAL_TREND_SAMPLES 30

typedef struct {
    void *timer, *model_timer;
    unsigned seconds, page;
    int target, draft, pressure;
    int history[INDUSTRIAL_TREND_SAMPLES];
    unsigned selected_sample;
    bool alarm, acknowledged, editing, drawer_seen_open;
    bool alerts_active, alerts_open, alerts_dragging;
    esp_gsp_err_t last_error;
} industrial_ui_t;
esp_gsp_err_t industrial_ui_init(esp_gsp_handle_t ui, industrial_ui_t *state);
void industrial_ui_deinit(esp_gsp_handle_t ui, industrial_ui_t *state);
