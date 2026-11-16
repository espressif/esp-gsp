/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Call once from app_main before LCD initialization. Other board profiles are
 * no-ops. An enabled Mosaico rail is preserved; a cold rail starts over 50 ms. */
esp_err_t bench_mosaico_power_init(void);

#ifdef __cplusplus
}
#endif
