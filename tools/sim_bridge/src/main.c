/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "gsp_sim_bridge.h"
#include "transport.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile sig_atomic_t stopping;
static void stop_signal(int sig)
{
    (void)sig;
    stopping = 1;
}

int main(int argc, char **argv)
{
    const char *endpoint = NULL;
    uint64_t duration = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--endpoint") && i + 1 < argc) {
            endpoint = argv[++i];
        } else if (!strcmp(argv[i], "--duration-ms") && i + 1 < argc) {
            duration = strtoull(argv[++i], NULL, 10);
        } else {
            fprintf(stderr, "usage: %s --endpoint tcp://127.0.0.1:PORT [--duration-ms N]\n", argv[0]);
            return 2;
        }
    }
    if (!endpoint) {
        fprintf(stderr, "sim_bridge: --endpoint is required\n");
        return 2;
    }
    signal(SIGINT, stop_signal);
    signal(SIGTERM, stop_signal);
    esp_gsp_handle_t ui;
    esp_gsp_err_t rc = gsp_sim_bridge_open(endpoint, &ui);
    if (rc != ESP_GSP_OK) {
        return 1;
    }
    rc = gsp_bridge_app_init(ui);
    uint64_t start = gsp_sim_bridge_time_ms();
    while (rc == ESP_GSP_OK && !stopping &&
            (!duration || gsp_sim_bridge_time_ms() - start < duration)) {
        rc = gsp_sim_bridge_poll(ui, 20);
    }
    bridge_payloads_shutdown(ui);
    gsp_bridge_app_deinit(ui); /* Must tolerate partially completed init. */
    gsp_sim_bridge_close(ui);
    if (rc != ESP_GSP_OK) {
        fprintf(stderr, "sim_bridge: application failed (%d)\n", rc);
    }
    return rc == ESP_GSP_OK ? 0 : 1;
}
