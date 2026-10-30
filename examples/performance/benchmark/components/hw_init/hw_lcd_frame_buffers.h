/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include "sdkconfig.h"
#include "esp_display_present_config.h"

#ifdef CONFIG_EXAMPLE_LCD_FRAME_BUFFERS
#define HW_LCD_FRAME_BUFFERS CONFIG_EXAMPLE_LCD_FRAME_BUFFERS
#else
#define HW_LCD_FRAME_BUFFERS 3
#endif

#if HW_LCD_FRAME_BUFFERS == 2
#define HW_LCD_FRAME_BUFFER_MODE ESP_DISPLAY_PRESENT_MODE_DOUBLE_PARTIAL
#elif HW_LCD_FRAME_BUFFERS == 3
#define HW_LCD_FRAME_BUFFER_MODE ESP_DISPLAY_PRESENT_MODE_AUTO
#else
#error "Native LCD profiles require two or three frame buffers"
#endif
