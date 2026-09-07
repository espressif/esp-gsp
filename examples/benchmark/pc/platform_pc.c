/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "gsp_sim_bridge.h"
#include "bench_workload.h"
#include <stdio.h>
static bench_workload_t workload;
static void *timer;
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    /* Matches the host_rgb565 profile selected in this PC CMake project. */
    bench_workload_init(&workload, false);
    timer = esp_gsp_timer_create(ui, 1000, bench_workload_drive_tweens, &workload);
    return timer ? ESP_GSP_OK : ESP_GSP_ERR_NO_MEM;
}
void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    if (timer) {
        (void)esp_gsp_timer_delete(ui, timer);
    }
    timer = NULL;
    fprintf(stderr, "benchmark_backend: tween_ticks=%u\n", (unsigned)workload.tween_tick);
}
