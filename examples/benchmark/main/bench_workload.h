/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdbool.h>
#include <stdatomic.h>
#include <stdint.h>

#include "esp_gsp.h"

typedef struct {
    uint32_t updates;
    uint32_t commands;
    uint32_t errors;
} bench_vector_stats_t;

typedef struct {
    uint32_t tween_tick;
    bool rgb888;
    bench_vector_stats_t vectors[8];
    bench_vector_stats_t effects;
    atomic_uint effects_published, effects_failed;
    int32_t move_center[2];
    int32_t eye_center_x[2], eye_center_y[2], eye_extent_x[2], eye_extent_y[2];
    uint32_t eyes_closed, eyes_open;
} bench_workload_t;

void bench_workload_init(bench_workload_t *workload, bool rgb888);

/** Timer callback shared by the ESP benchmark and its host runner. */
void bench_workload_drive_tweens(esp_gsp_handle_t ui, void *user_ctx);

/** Per-frame vector updates shared by device and simulator. */
void bench_workload_drive_vectors(esp_gsp_handle_t ui, void *user_ctx);
void bench_workload_drive_eyes(esp_gsp_handle_t ui, void *user_ctx);
int bench_workload_vector_index(uint16_t bind);

enum {
    BENCH_MESSAGES_PREPEND = 1U << 0,
    BENCH_MESSAGES_APPEND = 1U << 1,
    BENCH_MESSAGES_SCROLL = 1U << 2,
    BENCH_MESSAGES_DOWN = 1U << 3,
};

typedef struct {
    uint32_t step;
    int32_t offset;
    bool prepended, appended;
} bench_message_clock_t;

/* One quarter-screen step per rendered frame; data updates use elapsed time. */
unsigned bench_workload_message_actions(bench_message_clock_t *clock, uint32_t elapsed_ms, int32_t extent);
