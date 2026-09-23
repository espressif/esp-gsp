/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "gsp_sim_bridge.h"
#include "industrial_ui.h"
#include <stdio.h>
static industrial_ui_t state;
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    return industrial_ui_init(ui, &state);
}
void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    industrial_ui_deinit(ui, &state);
    fprintf(stderr, "industrial: last_error=%d\n", state.last_error);
}
