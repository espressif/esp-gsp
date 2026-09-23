/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "gsp_sim_bridge.h"
#include "thermostat_ui.h"
static thermostat_ui_t state;
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    return thermostat_ui_init(ui, &state);
}
void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    thermostat_ui_deinit(ui, &state);
}
