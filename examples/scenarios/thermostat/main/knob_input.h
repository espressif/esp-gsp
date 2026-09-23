/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once
#include "thermostat_ui.h"
#include "esp_err.h"
esp_err_t knob_input_start(esp_gsp_handle_t ui, thermostat_ui_t *state);
