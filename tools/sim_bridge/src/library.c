/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "gsp_sim_backend.h"
#include "transport.h"
#include "gsp/gsp_version.h"

#include <stddef.h>
#include <string.h>

_Static_assert(sizeof(GSP_VERSION_STRING) <= GSP_SIM_BACKEND_GSP_VERSION_CAPACITY,
               "GSP version does not fit the simulator backend descriptor");

static int32_t backend_run(const gsp_sim_host_api_v1_t *host)
{
    esp_gsp_handle_t ui = NULL;
    esp_gsp_err_t rc = bridge_open_inproc(host, &ui);
    if (rc != ESP_GSP_OK) {
        return rc;
    }
    rc = gsp_bridge_app_init(ui);
    while (rc == ESP_GSP_OK) {
        rc = gsp_sim_bridge_poll(ui, 20);
        if (bridge_inproc_closed(ui)) {
            rc = ESP_GSP_OK;
            break;
        }
    }
    bridge_payloads_shutdown(ui);
    gsp_bridge_app_deinit(ui);
    gsp_sim_bridge_close(ui);
    return rc;
}

int32_t gsp_sim_backend_query(uint32_t requested_abi, size_t descriptor_size,
                              gsp_sim_backend_descriptor_v1_t *descriptor)
{
    if (requested_abi != GSP_SIM_BACKEND_ABI_VERSION || descriptor == NULL ||
            descriptor_size < sizeof(*descriptor)) {
        return -1;
    }
    memset(descriptor, 0, sizeof(*descriptor));
    descriptor->struct_size = sizeof(*descriptor);
    descriptor->abi_version = GSP_SIM_BACKEND_ABI_VERSION;
    memcpy(descriptor->gsp_version, GSP_VERSION_STRING,
           sizeof(GSP_VERSION_STRING));
    descriptor->run = backend_run;
    return 0;
}
