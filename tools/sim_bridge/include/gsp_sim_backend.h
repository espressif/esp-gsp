/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GSP_SIM_BACKEND_ABI_VERSION 1U
#define GSP_SIM_BACKEND_GSP_VERSION_CAPACITY 32U

#ifdef _WIN32
#define GSP_SIM_BACKEND_EXPORT __declspec(dllexport)
#else
#define GSP_SIM_BACKEND_EXPORT __attribute__((visibility("default")))
#endif

enum gsp_sim_backend_io_result {
    GSP_SIM_BACKEND_IO_OK = 0,
    GSP_SIM_BACKEND_IO_TIMEOUT = 1,
    GSP_SIM_BACKEND_IO_CLOSED = 2,
    GSP_SIM_BACKEND_IO_ERROR = -1,
};

typedef struct {
    uint32_t struct_size;
    uint32_t abi_version;
    void *ctx;
    int32_t (*send_json)(void *ctx, const uint8_t *json, size_t size);
    int32_t (*send_binary)(void *ctx, const uint8_t *headers,
                           size_t header_size, const uint8_t *payload,
                           size_t payload_size);
    int32_t (*recv_json)(void *ctx, uint8_t *json, size_t capacity,
                         size_t *out_size, uint32_t timeout_ms);
} gsp_sim_host_api_v1_t;

typedef int32_t (*gsp_sim_backend_run_fn)(const gsp_sim_host_api_v1_t *host);

typedef struct {
    uint32_t struct_size;
    uint32_t abi_version;
    char gsp_version[GSP_SIM_BACKEND_GSP_VERSION_CAPACITY];
    gsp_sim_backend_run_fn run;
    uintptr_t reserved[4];
} gsp_sim_backend_descriptor_v1_t;

/* The host zero-initializes descriptor before this call. Implementations must
 * reject a different requested ABI or a short output structure. */
GSP_SIM_BACKEND_EXPORT int32_t gsp_sim_backend_query(
    uint32_t requested_abi, size_t descriptor_size,
    gsp_sim_backend_descriptor_v1_t *descriptor);

#ifdef __cplusplus
}
#endif
