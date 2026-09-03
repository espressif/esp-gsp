/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_gsp.h"

typedef struct {
    uint32_t tween_tick;
    bool rgb888;
} bench_workload_t;

void bench_workload_init(bench_workload_t *workload, bool rgb888);

/** Timer callback shared by the ESP benchmark and its host runner. */
void bench_workload_drive_tweens(esp_gsp_handle_t ui, void *user_ctx);
