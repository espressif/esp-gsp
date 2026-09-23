/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once
#include "esp_gsp.h"
typedef enum { THERMOSTAT_DECREASE, THERMOSTAT_INCREASE, THERMOSTAT_CONFIRM, THERMOSTAT_BACK } thermostat_action_t;
typedef struct {
    int half_degrees, mode, page, hours, seconds, preset;
    void *timer;
    void *navigation_timer;
    esp_gsp_err_t last_error;
} thermostat_ui_t;
esp_gsp_err_t thermostat_ui_init(esp_gsp_handle_t ui, thermostat_ui_t *state);
void thermostat_ui_action(esp_gsp_handle_t ui, thermostat_ui_t *state, thermostat_action_t action);
void thermostat_ui_deinit(esp_gsp_handle_t ui, thermostat_ui_t *state);
