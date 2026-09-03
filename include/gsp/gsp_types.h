/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Open-interval rectangle [x1, x2) x [y1, y2): x2/y2 sit one past the
 *  last affected pixel. The presenter-side counterpart
 *  esp_display_present_area_t (esp_display_present_types.h) is
 *  closed-interval; gsp_esp_lcd_canvas.c holds the conversion helpers. */
typedef struct {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} gsp_rect_t;

/** Retained template-instance reference:
 *  (generation << 16) | slot index; generation is never zero. */
typedef uint32_t gsp_instance_handle_t;

#define GSP_INSTANCE_HANDLE_INVALID 0U

typedef enum {
    GSP_OK = 0,
    GSP_ERR_INVALID_ARG = -1,
    GSP_ERR_TRUNCATED = -2,
    GSP_ERR_BAD_MAGIC = -3,
    GSP_ERR_UNSUPPORTED_VERSION = -4,
    GSP_ERR_BAD_CRC = -5,
    GSP_ERR_INVALID_FORMAT = -6,
    GSP_ERR_BAD_ALIGNMENT = -7,
    GSP_ERR_SECTION_BOUNDS = -8,
    GSP_ERR_SECTION_OVERLAP = -9,
    GSP_ERR_UNKNOWN_REQUIRED_SECTION = -10,
    GSP_ERR_LIMIT_EXCEEDED = -11,
    GSP_ERR_PROFILE_MISMATCH = -12,
    GSP_ERR_UNSUPPORTED = -13,
    GSP_ERR_IO = -14,
    GSP_ERR_INVALID_STATE = -15,
    GSP_ERR_NOT_FOUND = -16,
    GSP_ERR_CANCELLED = -17,
} gsp_err_t;

#ifdef __cplusplus
}
#endif
