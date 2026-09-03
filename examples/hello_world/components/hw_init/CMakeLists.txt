# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
idf_component_register(
    SRCS "hw_lcd_mipi_dsi.c" "hw_lcd_qspi.c" "hw_lcd_rgb.c"
         "hw_lcd_spi_gc9a01.c" "hw_lcd_spi_ili9341.c" "hw_touch.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_display_present esp_lcd_touch
    PRIV_REQUIRES esp_lcd esp_driver_gpio esp_driver_spi esp_driver_i2c
)
