/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* GC9A01 240x240 round panel on plain SPI (knob board, no PSRAM).
 * Pin map and panel quirks carry the board values validated by the
 * esp-iot-solution display examples. */

#include "hw_init.h"

#if CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM

#include "driver/spi_master.h"
#include "esp_lcd_gc9a01.h"
#include "esp_log.h"

static const char *TAG = "hw_lcd_gc9a01";

#define HW_LCD_SPI_HOST SPI2_HOST
#define HW_LCD_DATA0    GPIO_NUM_0
#define HW_LCD_PCLK     GPIO_NUM_1
#define HW_LCD_CS       GPIO_NUM_7
#define HW_LCD_DC       GPIO_NUM_2
#define HW_LCD_RST      GPIO_NUM_NC

#define HW_LCD_PIXEL_CLOCK_HZ (80 * 1000 * 1000)
/* Bounded transfer chunks keep the DMA descriptors in internal RAM. */
#define HW_LCD_MAX_TRANSFER_SZ (HW_LCD_H_RES * 80 * 2)

esp_err_t hw_lcd_init(
    esp_display_present_target_config_t *out_target_config)
{
    ESP_LOGI(TAG, "Initialize GC9A01 SPI panel (%dx%d)",
             HW_LCD_H_RES, HW_LCD_V_RES);
    const spi_bus_config_t bus_config = {
        .sclk_io_num = HW_LCD_PCLK,
        .mosi_io_num = HW_LCD_DATA0,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = HW_LCD_MAX_TRANSFER_SZ,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HW_LCD_SPI_HOST, &bus_config,
                                       SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = HW_LCD_DC,
        .cs_gpio_num = HW_LCD_CS,
        .pclk_hz = HW_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    esp_lcd_panel_io_handle_t io;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(
                        (esp_lcd_spi_bus_handle_t)HW_LCD_SPI_HOST, &io_config, &io));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = HW_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    esp_lcd_panel_handle_t panel;
    ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(io, &panel_config, &panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

    *out_target_config = (esp_display_present_target_config_t) {
        .hw = {
            .panel = panel,
            .io = io,
            .panel_type = ESP_DISPLAY_PRESENT_PANEL_IO,
            .input_pixel_format = ESP_DISPLAY_PRESENT_PIXEL_FORMAT_RGB565,
            .rotation = HW_DISPLAY_ROTATION,
            .swap_bytes = true,
        },
        .fb = {
            .mode = ESP_DISPLAY_PRESENT_MODE_AUTO,
        },
    };
    return ESP_OK;
}

#endif /* CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM */
