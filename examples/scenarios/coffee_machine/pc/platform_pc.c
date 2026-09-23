/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "gsp_sim_bridge.h"
#include "coffee_ui.h"
#include <stdio.h>
static coffee_ui_t state;
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    return coffee_ui_init(ui, &state);
}
void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    coffee_ui_deinit(ui, &state);
    fprintf(stderr, "coffee: last_error=%d\n", state.last_error);
}
