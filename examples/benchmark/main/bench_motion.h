/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdint.h>

static inline int16_t bench_bounce_position(uint32_t phase,
        uint16_t screen_extent,
        uint16_t object_extent)
{
    if (screen_extent <= object_extent) {
        return 0;
    }
    uint32_t span = screen_extent - object_extent;
    uint32_t position = phase % (2U * span);
    return (int16_t)(position <= span ? position : 2U * span - position);
}

/* Preserve the original 10 Hz mover's pixels-per-second trajectory while
 * sampling it from the 16 ms high-rate benchmark timer. */
static inline uint32_t bench_move_phase(uint32_t tick, uint32_t index)
{
    return (uint32_t)(((uint64_t)tick * (6U + 3U * index) * 4U) / 25U);
}
