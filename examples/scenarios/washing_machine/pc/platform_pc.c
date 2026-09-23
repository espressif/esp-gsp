/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "gsp_sim_bridge.h"
#include "washing_machine_ui.h"
static washing_machine_ui_t state;
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    return washing_machine_ui_init(ui, &state);
}
void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    washing_machine_ui_deinit(ui, &state);
}
