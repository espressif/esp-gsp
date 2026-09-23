/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "gsp_sim_bridge.h"
#include "media_ui.h"
#include <stdio.h>
static media_ui_t state;
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    esp_gsp_err_t rc = media_ui_init(ui, &state);
    if (rc == ESP_GSP_OK) {
        /* PC offscreen work must be uploaded before the host render fence. */
        rc = gsp_sim_bridge_poll(ui, 0);
    }
    if (rc == ESP_GSP_OK) {
        rc = esp_gsp_flush(ui, 1000);
    }
    return rc;
}
void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    media_ui_deinit(ui, &state);
    fprintf(stderr, "media_backend: list_rows=%u grid_cells=%u draws=%u last_error=%d\n",
            (unsigned)state.list_rows, (unsigned)state.grid_cells, (unsigned)state.draws, state.last_error);
    fprintf(stderr, "image lifecycle: completed=%u released=%u status=%d\n",
            (unsigned)state.image_completions, (unsigned)state.image_releases, state.image_status);
}
