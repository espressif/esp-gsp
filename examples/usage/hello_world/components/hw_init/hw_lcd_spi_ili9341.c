/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* ILI9341 320x240 panel on plain SPI (PSRAM board, GT911 touch).
 * Pin map carries the board values validated by the esp-iot-solution
 * display examples. */

#include "hw_init.h"

#if CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_ili9341.h"
#include "esp_log.h"

static const char *TAG = "hw_lcd_ili9341";

#define HW_LCD_SPI_HOST     SPI2_HOST
#define HW_LCD_DATA0        GPIO_NUM_6
#define HW_LCD_PCLK         GPIO_NUM_7
#define HW_LCD_CS           GPIO_NUM_5
#define HW_LCD_DC           GPIO_NUM_4
#define HW_LCD_RST          GPIO_NUM_48
#define HW_LCD_BK_LIGHT     GPIO_NUM_47
#define HW_LCD_BK_ON_LEVEL  1

#define HW_LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)
#define HW_LCD_MAX_TRANSFER_SZ (HW_LCD_H_RES * HW_LCD_V_RES * 2)

/* Vendor init for this panel module (extended command set, gamma,
 * frame rate): the driver defaults leave it misconfigured. Matches
 * the sequence the board's reference project ships. */
static const ili9341_lcd_init_cmd_t vendor_specific_init_default[] = {
    {0xC8, (uint8_t []){0xFF, 0x93, 0x42}, 3, 0},
    {0xC0, (uint8_t []){0x0E, 0x0E}, 2, 0},
    {0xC5, (uint8_t []){0xD0}, 1, 0},
    {0xC1, (uint8_t []){0x02}, 1, 0},
    {0xB4, (uint8_t []){0x02}, 1, 0},
    {
        0xE0, (uint8_t [])
        {
            0x00, 0x03, 0x08, 0x06, 0x13, 0x09, 0x39,
            0x39, 0x48, 0x02, 0x0a, 0x08, 0x17, 0x17,
            0x0F
        }, 15, 0
    },
    {
        0xE1, (uint8_t [])
        {
            0x00, 0x28, 0x29, 0x01, 0x0d, 0x03, 0x3f,
            0x33, 0x52, 0x04, 0x0f, 0x0e, 0x37, 0x38,
            0x0F
        }, 15, 0
    },
    {0xB1, (uint8_t []){0x00, 0x1B}, 2, 0},
    {0x36, (uint8_t []){0x08}, 1, 0},
    {0x3A, (uint8_t []){0x55}, 1, 0},
    {0xB7, (uint8_t []){0x06}, 1, 0},
    {0x11, (uint8_t []){0}, 0x80, 0},
    {0x29, (uint8_t []){0}, 0x80, 0},
    {0, (uint8_t []){0}, 0xff, 0},
};

esp_err_t hw_lcd_init(
    esp_display_present_target_config_t *out_target_config)
{
    ESP_LOGI(TAG, "Initialize ILI9341 SPI panel (%dx%d)",
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
        .trans_queue_depth = 1,
    };
    esp_lcd_panel_io_handle_t io;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(
                        (esp_lcd_spi_bus_handle_t)HW_LCD_SPI_HOST, &io_config, &io));

    ili9341_vendor_config_t vendor_config = {
        .init_cmds = vendor_specific_init_default,
        .init_cmds_size = sizeof(vendor_specific_init_default) /
        sizeof(vendor_specific_init_default[0]),
    };
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = HW_LCD_RST,
        /* This module's reset rail is ACTIVE HIGH (shared with the
         * GT911): the default polarity holds the touch controller in
         * permanent reset. */
        .flags.reset_active_high = true,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    esp_lcd_panel_handle_t panel;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io, &panel_config,
                    &panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel, true, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

    gpio_config_t backlight_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << HW_LCD_BK_LIGHT,
    };
    ESP_ERROR_CHECK(gpio_config(&backlight_config));
    gpio_set_level(HW_LCD_BK_LIGHT, HW_LCD_BK_ON_LEVEL);

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

#endif /* CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM */
