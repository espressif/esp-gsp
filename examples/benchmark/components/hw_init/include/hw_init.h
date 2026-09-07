/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* Shared example bring-up: one hw_lcd_init()/hw_touch_init() pair,
 * with the panel selected by the EXAMPLE_LCD_INTERFACE Kconfig
 * choice. Structure follows esp-iot-solution's examples/display
 * common layer; the panel parameters are the board values validated
 * by the esp-gsp bring-up (RGB565, TE quirks, touch mirroring). */

#pragma once

#include "esp_err.h"
#include "esp_display_present_config.h"
#include "esp_lcd_touch.h"
#include "sdkconfig.h"

/* Panel rotation selected in menuconfig (Example HW Init). The macro
 * expands to esp_display_present_rotation_t enumerators from the
 * display-present target contract. */
#if CONFIG_EXAMPLE_DISPLAY_ROTATION_90
#define HW_DISPLAY_ROTATION ESP_DISPLAY_PRESENT_ROTATE_90
#elif CONFIG_EXAMPLE_DISPLAY_ROTATION_180
#define HW_DISPLAY_ROTATION ESP_DISPLAY_PRESENT_ROTATE_180
#elif CONFIG_EXAMPLE_DISPLAY_ROTATION_270
#define HW_DISPLAY_ROTATION ESP_DISPLAY_PRESENT_ROTATE_270
#else
#define HW_DISPLAY_ROTATION ESP_DISPLAY_PRESENT_ROTATE_0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
#define HW_LCD_H_RES       1024
#define HW_LCD_V_RES       600
#define HW_USE_TOUCH       1
#define HW_LCD_SWAP_BYTES  0
#define HW_LCD_TE_GPIO     (-1)
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB
#define HW_LCD_H_RES       800
#define HW_LCD_V_RES       480
#define HW_USE_TOUCH       1
#define HW_LCD_SWAP_BYTES  0
#define HW_LCD_TE_GPIO     (-1)
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
#define HW_LCD_H_RES       800
#define HW_LCD_V_RES       480
#define HW_USE_TOUCH       0
#define HW_LCD_SWAP_BYTES  0
#define HW_LCD_TE_GPIO     (-1)
#elif CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM
#define HW_LCD_H_RES       320
#define HW_LCD_V_RES       240
#define HW_USE_TOUCH       1
/* SPI/QSPI panels take big-endian RGB565 over the wire. */
#define HW_LCD_SWAP_BYTES  1
#define HW_LCD_TE_GPIO     (-1)
#elif CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM
#define HW_LCD_H_RES       240
#define HW_LCD_V_RES       240
#define HW_USE_TOUCH       0   /* knob-driven board, no touch */
#define HW_LCD_SWAP_BYTES  1
#define HW_LCD_TE_GPIO     (-1)
#elif CONFIG_EXAMPLE_LCD_INTERFACE_QSPI
#define HW_LCD_H_RES       360
#define HW_LCD_V_RES       360
#define HW_USE_TOUCH       1
/* SPI/QSPI panels take big-endian RGB565 over the wire. */
#define HW_LCD_SWAP_BYTES  1
/* Panel TE output (TEON is sent by the init sequence); the QSPI bus
 * parameters let the present layer size the safe transfer window. */
#define HW_LCD_TE_GPIO           8
#define HW_LCD_BUS_FREQ_HZ       (80 * 1000 * 1000)
#define HW_LCD_BUS_DATA_LINES    4
#define HW_LCD_BITS_PER_PIXEL    16
#elif CONFIG_EXAMPLE_LCD_INTERFACE_QSPI_CO5300
#define HW_LCD_H_RES       480
#define HW_LCD_V_RES       480
#define HW_USE_TOUCH       1
/* SPI/QSPI panels take big-endian RGB565 over the wire. */
#define HW_LCD_SWAP_BYTES  1
#define HW_LCD_TE_GPIO           38
#define HW_LCD_BUS_FREQ_HZ       (40 * 1000 * 1000)
#define HW_LCD_BUS_DATA_LINES    4
#define HW_LCD_BITS_PER_PIXEL    16
#else
/* No board profile is available for this target. Keep hw_touch.c buildable
 * so configuration fails at the selected LCD implementation, rather than
 * pretending the target is the ESP32-S3 QSPI touch board. */
#define HW_LCD_H_RES       0
#define HW_LCD_V_RES       0
#define HW_USE_TOUCH       0
#define HW_LCD_SWAP_BYTES  0
#define HW_LCD_TE_GPIO     (-1)
#endif

/**
 * Brings the selected panel up and returns its complete present target.
 * Interface type, framebuffer count, byte order and TE wiring remain in
 * this hardware layer; applications do not select anti-tearing modes.
 */
esp_err_t hw_lcd_init(
    esp_display_present_target_config_t *out_target_config);

#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
/** MIPI-DPI variant with an explicit panel input depth. RGB565 uses 16 and
 *  RGB888 uses 24; the compiled scene profile must match. */
esp_err_t hw_lcd_init_mipi(
    esp_display_present_target_config_t *out_target_config,
    uint8_t bits_per_pixel);
#endif

/**
 * Brings the touch controller up. Returns ESP_ERR_NOT_SUPPORTED when
 * the selected interface has no touch (QSPI board), ESP_ERR_NOT_FOUND
 * when the controller does not respond (run without touch).
 */
esp_err_t hw_touch_init(esp_lcd_touch_handle_t *out_touch);

#ifdef __cplusplus
}
#endif
