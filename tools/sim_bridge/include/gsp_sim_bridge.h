/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once
#include "esp_gsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* All calls, callbacks and timers belong to the thread that calls open/poll.
 * No application callback is invoked recursively while awaiting an RPC reply.
 * Queued Canvas releases may run before a synchronous RPC starts; they must
 * not mutate GSP state, poll or close. poll's timeout is an idle wait, not a
 * total budget for uploads/callbacks. Before esp_gsp_flush, poll until local
 * Canvas uploads/draws have completed (otherwise flush returns INVALID_STATE).
 * Only numeric IPv4 loopback TCP endpoints are accepted in version 1. */
esp_gsp_err_t gsp_sim_bridge_open(const char *endpoint, esp_gsp_handle_t *out);
esp_gsp_err_t gsp_sim_bridge_poll(esp_gsp_handle_t ui, uint32_t timeout_ms);
void gsp_sim_bridge_close(esp_gsp_handle_t ui);
uint16_t gsp_sim_bridge_current_scene(esp_gsp_handle_t ui);
uint64_t gsp_sim_bridge_time_ms(void);

/* Default executable entry points supplied by the application's PC adapter. */
esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui);
void gsp_bridge_app_deinit(esp_gsp_handle_t ui);

#ifdef __cplusplus
}
#endif
