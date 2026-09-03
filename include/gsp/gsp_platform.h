/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once
#include "gsp/gsp_build_caps.h"

#include <stdbool.h>
#include <stdint.h>

#include "gsp/gsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Normalized pointer sample exchanged between a platform adapter and
 * gsp_app. Coordinates are always logical scene coordinates. */
typedef struct {
    int32_t x;
    int32_t y;
    bool pressed;
} gsp_platform_pointer_t;

#define GSP_PLATFORM_MAX_TOUCH_POINTS ESP_GSP_BUILD_CAP_MAX_TOUCH_POINTS

typedef struct {
    int32_t x;
    int32_t y;
    uint16_t strength;
    uint8_t id;
} gsp_platform_touch_contact_t;

typedef struct {
    gsp_platform_touch_contact_t contacts[GSP_PLATFORM_MAX_TOUCH_POINTS];
    uint8_t count;
} gsp_platform_touch_frame_t;

#ifdef __cplusplus
}
#endif
