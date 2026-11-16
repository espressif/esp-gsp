/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include "esp_err.h"
#include "esp_display_present_config.h"
#include "esp_lcd_touch.h"
#include "sdkconfig.h"

#define HW_DISPLAY_ROTATION ESP_DISPLAY_PRESENT_ROTATE_0
#define HW_USE_TOUCH 0
#if GSP_BENCH_BOARD_S31_MOSAICO
#define HW_LCD_TE_GPIO 43
#else
#define HW_LCD_TE_GPIO (-1)
#endif

#if GSP_BENCH_BOARD_C5_CO5300
#define HW_LCD_H_RES 466
#define HW_LCD_V_RES 466
#define HW_LCD_SWAP_BYTES 1
#elif GSP_BENCH_BOARD_P4_EYE_SPI
#define HW_LCD_H_RES 240
#define HW_LCD_V_RES 240
#define HW_LCD_SWAP_BYTES 1
#elif GSP_BENCH_BOARD_S31_MOSAICO
#define HW_LCD_H_RES 480
#define HW_LCD_V_RES 480
#define HW_LCD_SWAP_BYTES 1
#elif GSP_BENCH_BOARD_S31_GC9503
#define HW_LCD_H_RES 480
#define HW_LCD_V_RES 480
#define HW_LCD_SWAP_BYTES 0
#else
#error "No benchmark-local board profile selected"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Ownership of a successful target is transferred to the benchmark presenter. */
esp_err_t hw_lcd_init(esp_display_present_target_config_t *out_target_config);
/* Automatic benchmark adapters deliberately leave physical touch disabled. */
esp_err_t hw_touch_init(esp_lcd_touch_handle_t *out_touch);

#ifdef __cplusplus
}
#endif
