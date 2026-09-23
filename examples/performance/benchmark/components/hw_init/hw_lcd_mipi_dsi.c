/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "hw_init.h"

#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI

#include "esp_check.h"
#include "esp_lcd_ek79007.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_ldo_regulator.h"
#include "esp_log.h"

static const char *TAG = "hw_lcd";

#define LDO_MIPI_CHAN 3
#define LDO_MIPI_VOLTAGE_MV 2500
#define PIN_NUM_LCD_RST 27

esp_err_t hw_lcd_init(
    esp_display_present_target_config_t *out_target_config);

esp_err_t hw_lcd_init_mipi(
    esp_display_present_target_config_t *out_target_config,
    uint8_t bits_per_pixel)
{
    ESP_RETURN_ON_FALSE(bits_per_pixel == 16 || bits_per_pixel == 24,
                        ESP_ERR_INVALID_ARG, TAG,
                        "MIPI input depth must be 16 or 24 bpp");
    ESP_LOGI(TAG, "Initialize MIPI-DSI panel (%dx%d, %u bpp, %u fbs)",
             HW_LCD_H_RES, HW_LCD_V_RES, (unsigned)bits_per_pixel,
             3U);
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_config = {
        .chan_id = LDO_MIPI_CHAN,
        .voltage_mv = LDO_MIPI_VOLTAGE_MV,
    };
    ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_config,
                        &ldo_mipi_phy),
                        TAG, "MIPI LDO");

    esp_lcd_dsi_bus_handle_t dsi_bus;
    esp_lcd_dsi_bus_config_t bus_config =
        EK79007_PANEL_BUS_DSI_2CH_CONFIG();
    ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&bus_config, &dsi_bus),
                        TAG, "DSI bus");

    esp_lcd_panel_io_handle_t dbi_io;
    esp_lcd_dbi_io_config_t dbi_config = EK79007_PANEL_IO_DBI_CONFIG();
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_dbi(dsi_bus, &dbi_config,
                        &dbi_io),
                        TAG, "DBI io");

    /* EK79007 1024x600 @60Hz timing (spelled out for IDF 6.x). */
    esp_lcd_dpi_panel_config_t dpi_config = {
        .virtual_channel = 0,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = 52,
        .in_color_format = bits_per_pixel == 24 ?
        LCD_COLOR_FMT_RGB888 : LCD_COLOR_FMT_RGB565,
        .num_fbs = 3,
        .video_timing = {
            .h_size = HW_LCD_H_RES,
            .v_size = HW_LCD_V_RES,
            .hsync_pulse_width = 10,
            .hsync_back_porch = 160,
            .hsync_front_porch = 160,
            .vsync_pulse_width = 1,
            .vsync_back_porch = 23,
            .vsync_front_porch = 12,
        },
    };
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = bits_per_pixel,
        .vendor_config = &vendor_config,
    };
    esp_lcd_panel_handle_t panel;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_ek79007(dbi_io, &panel_config,
                        &panel),
                        TAG, "panel");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel), TAG, "reset");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel), TAG, "init");
    *out_target_config = (esp_display_present_target_config_t) {
        .hw = {
            .panel = panel,
            .panel_type = ESP_DISPLAY_PRESENT_PANEL_MIPI_DSI,
            .input_pixel_format = bits_per_pixel == 24
            ? ESP_DISPLAY_PRESENT_PIXEL_FORMAT_RGB888
            : ESP_DISPLAY_PRESENT_PIXEL_FORMAT_RGB565,
            .rotation = HW_DISPLAY_ROTATION,
        },
        .fb = {
            .mode = ESP_DISPLAY_PRESENT_MODE_AUTO,
        },
    };
    return ESP_OK;
}

esp_err_t hw_lcd_init(
    esp_display_present_target_config_t *out_target_config)
{
    return hw_lcd_init_mipi(out_target_config, 16);
}

#endif /* CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI */
