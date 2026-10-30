/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "gsp_sim_bridge.h"
#include "watch_ui.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
static watch_ui_t state;
static uint64_t clock_ms(void)
{
#ifdef _WIN32
    return GetTickCount64();
#else
    struct timespec time = {0};
    (void)clock_gettime(CLOCK_MONOTONIC, &time);
    return (uint64_t)time.tv_sec * 1000 + (uint64_t)time.tv_nsec / 1000000;
#endif
}
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    return watch_ui_init(ui, &state, clock_ms);
}
void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    watch_ui_deinit(ui, &state);
    fprintf(stderr, "watch: last_error=%d\n", state.last_error);
}
