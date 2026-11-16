/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"

#define BENCH_CPU_MAX_TASKS 64U
#define BENCH_CPU_CORES CONFIG_FREERTOS_NUMBER_OF_CORES
/* Idle runtime can cross a measurement endpoint at tick resolution. */
#define BENCH_CPU_TOLERANCE_US ((UINT64_C(1000000) + CONFIG_FREERTOS_HZ - 1U) / CONFIG_FREERTOS_HZ)

typedef struct {
    bool valid;
    uint64_t total_us;
    uint64_t idle_us[BENCH_CPU_CORES];
    uint64_t snapshot_us;
} bench_cpu_snapshot_t;

typedef struct {
    bool valid;
    uint32_t windows;
    uint64_t start_us;
    uint64_t end_us;
    uint64_t total_us; /* One wall-clock counter, before all-core normalization. */
    uint64_t idle_us[BENCH_CPU_CORES];
    uint64_t idle_adjust_us[BENCH_CPU_CORES]; /* Per-window raw excess above total_us. */
    uint64_t snapshot_us;
} bench_cpu_window_t;

/* Called only by the benchmark's UI task; uses shared bounded scratch space. */
bench_cpu_snapshot_t bench_cpu_snapshot(void);
bench_cpu_window_t bench_cpu_window(bench_cpu_snapshot_t before,
                                    bench_cpu_snapshot_t after);
/* A zero-initialized sum starts empty; any invalid window poisons the sum. */
void bench_cpu_accumulate(bench_cpu_window_t *sum, const bench_cpu_window_t *window);
/* name == NULL prints the aggregate, otherwise a per-case record. */
void bench_cpu_print(const char *name, const bench_cpu_window_t *window);
