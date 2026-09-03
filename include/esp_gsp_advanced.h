/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

/* Low-level access for framework integrations. Applications should use
 * esp_gsp.h or the compiler-generated bundle header. */

#include "esp_gsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gsp_context gsp_context_t;

/** Return the active low-level rendering context. */
gsp_context_t *esp_gsp_context(esp_gsp_handle_t gsp);

#ifdef __cplusplus
}
#endif
