/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* Render-profiler read-out — the only engine-level diagnostic an
 * application consumes (the benchmark example plots per-opcode render
 * cost). The renderer interfaces themselves are engine-private. */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef GSP_PROFILE_RENDER
#define GSP_PROFILE_RENDER 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Opcode slots covered by the render profiler (opcode value range). */
#define GSP_RENDERER_PROFILE_OPCODES 24U

/** Copies the per-opcode cumulative render time / call counters
 *  (zeros unless the benchmark enables GSP_PROFILE_RENDER).
 *  Index = opcode value. */
void gsp_renderer_profile_snapshot(uint64_t *out_us,
                                   uint32_t *out_calls, size_t cap);

#ifdef __cplusplus
}
#endif
