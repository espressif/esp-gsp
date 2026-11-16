/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

#include "bench_cpu.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if !CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS || \
    !CONFIG_FREERTOS_RUN_TIME_COUNTER_TYPE_U64 || \
    !CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
#error "Benchmark CPU measurement requires ESP_TIMER 64-bit FreeRTOS runtime stats"
#endif

_Static_assert(sizeof(configRUN_TIME_COUNTER_TYPE) == sizeof(uint64_t),
               "Benchmark CPU runtime counters must be 64-bit");

static TaskStatus_t s_tasks[BENCH_CPU_MAX_TASKS];

bench_cpu_snapshot_t bench_cpu_snapshot(void)
{
    const int64_t start_us = esp_timer_get_time();
    configRUN_TIME_COUNTER_TYPE total = 0;
    const UBaseType_t count = uxTaskGetSystemState(s_tasks, BENCH_CPU_MAX_TASKS, &total);
    const int64_t end_us = esp_timer_get_time();
    bench_cpu_snapshot_t result = {
        .valid = count > 0 && count <= BENCH_CPU_MAX_TASKS && total > 0 &&
        start_us >= 0 && end_us >= start_us &&
        total >= (uint64_t)start_us && total <= (uint64_t)end_us,
        /* Keep the FPS clock usable when CPU capture is unavailable. */
        .total_us = total > 0 ? total : (uint64_t)end_us,
    };
    TaskHandle_t idle[BENCH_CPU_CORES];
    for (unsigned core = 0; core < BENCH_CPU_CORES; ++core) {
        idle[core] = xTaskGetIdleTaskHandleForCore((BaseType_t)core);
        unsigned found = 0;
        for (UBaseType_t task = 0; task < count && task < BENCH_CPU_MAX_TASKS; ++task) {
            if (idle[core] != NULL && s_tasks[task].xHandle == idle[core]) {
                result.idle_us[core] = s_tasks[task].ulRunTimeCounter;
                ++found;
            }
        }
        result.valid = result.valid && found == 1 &&
                       (result.idle_us[core] <= total ||
                        result.idle_us[core] - total <= BENCH_CPU_TOLERANCE_US);
        for (unsigned previous = 0; previous < core; ++previous) {
            result.valid = result.valid && idle[previous] != idle[core];
        }
    }
    const int64_t finished_us = esp_timer_get_time();
    result.valid = result.valid && finished_us >= end_us;
    result.snapshot_us = finished_us >= start_us ? (uint64_t)(finished_us - start_us) : 0;
    return result;
}

bench_cpu_window_t bench_cpu_window(bench_cpu_snapshot_t before,
                                    bench_cpu_snapshot_t after)
{
    bench_cpu_window_t result = {
        .valid = before.valid && after.valid && after.total_us > before.total_us,
        .windows = 1,
        .start_us = before.total_us,
        .end_us = after.total_us,
        .total_us = after.total_us >= before.total_us ? after.total_us - before.total_us : 0,
        .snapshot_us = before.snapshot_us + after.snapshot_us,
    };
    for (unsigned core = 0; core < BENCH_CPU_CORES; ++core) {
        result.idle_us[core] = after.idle_us[core] >= before.idle_us[core] ?
                               after.idle_us[core] - before.idle_us[core] : 0;
        result.idle_adjust_us[core] = result.idle_us[core] > result.total_us ?
                                      result.idle_us[core] - result.total_us : 0;
        result.valid = result.valid && after.idle_us[core] >= before.idle_us[core] &&
                       result.idle_adjust_us[core] <= BENCH_CPU_TOLERANCE_US;
    }
    result.valid = result.valid && result.total_us > 0 && result.snapshot_us <= result.total_us &&
                   result.total_us <= UINT64_MAX / (BENCH_CPU_CORES * 10000U);
    return result;
}

void bench_cpu_accumulate(bench_cpu_window_t *sum, const bench_cpu_window_t *window)
{
    if (sum->windows == 0) {
        sum->valid = true;
    }
    sum->valid = sum->valid && window->valid &&
                 (sum->windows == 0 || window->start_us >= sum->end_us) &&
                 UINT64_MAX - sum->total_us >= window->total_us &&
                 UINT64_MAX - sum->snapshot_us >= window->snapshot_us;
    if (sum->windows == 0) {
        sum->start_us = window->start_us;
    }
    sum->end_us = window->end_us;
    sum->windows += window->windows;
    sum->total_us += window->total_us;
    sum->snapshot_us += window->snapshot_us;
    for (unsigned core = 0; core < BENCH_CPU_CORES; ++core) {
        sum->valid = sum->valid && UINT64_MAX - sum->idle_us[core] >= window->idle_us[core] &&
                     UINT64_MAX - sum->idle_adjust_us[core] >= window->idle_adjust_us[core];
        sum->idle_us[core] += window->idle_us[core];
        sum->idle_adjust_us[core] += window->idle_adjust_us[core];
        sum->valid = sum->valid && sum->idle_adjust_us[core] <= sum->idle_us[core] &&
                     sum->idle_us[core] - sum->idle_adjust_us[core] <= sum->total_us;
    }
    sum->valid = sum->valid && sum->total_us <= UINT64_MAX / (BENCH_CPU_CORES * 10000U);
}

void bench_cpu_print(const char *name, const bench_cpu_window_t *window)
{
    uint64_t idle_us = 0;
    uint64_t idle_adjust_us = 0;
    for (unsigned core = 0; core < BENCH_CPU_CORES; ++core) {
        idle_us += window->idle_us[core];
        idle_adjust_us += window->idle_adjust_us[core];
    }
    const uint64_t capacity_us = window->total_us * BENCH_CPU_CORES;
    const int busy_bp = window->valid && capacity_us > 0 ?
                        (int)((capacity_us - (idle_us - idle_adjust_us)) * 10000U / capacity_us) : -1;
    if (name != NULL) {
        printf("bench: cpu[%s]", name);
    } else {
        printf("bench: aggregate cpu");
    }
    printf(" valid=%u busy_bp=%d cores=%u windows=%"PRIu32
           " window_us=%"PRIu64" total_us=%"PRIu64" idle_us=%"PRIu64
           " core_idle_us=", window->valid ? 1U : 0U, busy_bp,
           (unsigned)BENCH_CPU_CORES, window->windows,
           window->total_us, window->total_us, idle_us);
    for (unsigned core = 0; core < BENCH_CPU_CORES; ++core) {
        printf("%s%"PRIu64, core == 0 ? "" : ",", window->idle_us[core]);
    }
    printf(" tolerance_us=%"PRIu64" idle_adjust_us=%"PRIu64" core_idle_adjust_us=",
           BENCH_CPU_TOLERANCE_US, idle_adjust_us);
    for (unsigned core = 0; core < BENCH_CPU_CORES; ++core) {
        printf("%s%"PRIu64, core == 0 ? "" : ",", window->idle_adjust_us[core]);
    }
    printf(" snapshot_us=%"PRIu64, window->snapshot_us);
    if (name != NULL) {
        printf(" start_us=%"PRIu64" end_us=%"PRIu64, window->start_us, window->end_us);
    }
    printf("\n");
}
