/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once
#include "esp_gsp.h"
typedef struct {
    int program, temperature, rpm, elapsed;
    unsigned page, angle;
    bool door_open, running, paused, done, effects_covered;
    void *timer, *page_timer, *motion_timer;
    esp_gsp_err_t last_error;
} washing_machine_ui_t;
esp_gsp_err_t washing_machine_ui_init(esp_gsp_handle_t ui, washing_machine_ui_t *state);
void washing_machine_ui_deinit(esp_gsp_handle_t ui, washing_machine_ui_t *state);
