# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

idf_component_register(
    SRCS esp_lcd_gc9503.c
    INCLUDE_DIRS include
    REQUIRES esp_lcd
    PRIV_REQUIRES esp_driver_gpio
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    ESP_LCD_GC9503_VER_MAJOR=3
    ESP_LCD_GC9503_VER_MINOR=0
    ESP_LCD_GC9503_VER_PATCH=1)
