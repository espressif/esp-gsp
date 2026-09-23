/* SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include <assert.h>
#include "gsp_sim_bundle_gsp.h"
#include "gsp/gsp_app.h"
#include "gsp_app_port.h"
#include "app_logic.h"

void gsp_sim_application_reset(void)
{
}

void gsp_sim_application_setup(gsp_app_t *app)
{
    esp_gsp_err_t ret = quick_effects_init(gsp_app_handle(app));
    assert(ret == ESP_GSP_OK);
    (void)ret;
}
