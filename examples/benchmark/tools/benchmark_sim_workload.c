/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "bench_workload.h"

#include <stdio.h>

#include "gsp/format/gsp_common_format.h"
#include "gsp/gsp_app.h"
#include "gsp/gsp_context.h"
#include "gsp_app_port.h"

static gsp_app_t *s_app;
static bench_workload_t s_workload;

void gsp_sim_application_reset(void)
{
    s_app = NULL;
}

void gsp_sim_application_setup(gsp_app_t *app)
{
    if (app == NULL || app == s_app) {
        return;
    }
    gsp_context_t *context = gsp_app_current_context(app);
    if (context == NULL) {
        return;
    }
    s_app = app;
    bench_workload_init(
        &s_workload,
        context->scene.color_profile ==
        GSP_FORMAT_COMMON_PIXEL_FORMAT_RGB888);
    (void)esp_gsp_timer_create(gsp_app_handle(app), 1000,
                               bench_workload_drive_tweens, &s_workload);
    printf("gsp_sim: benchmark tween workload enabled (%s)\n",
           s_workload.rgb888 ? "RGB888" : "RGB565");
}
