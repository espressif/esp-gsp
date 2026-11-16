/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */
/* LCD-only adaptation of esp-dev-kits ESP32-P4-EYE esp32_p4_eye.c.
 * Retains its SPI pins/mode, vendor commands and inverted LEDC backlight. */

#include "hw_init.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "hw_p4_eye";

typedef struct {
    int cmd;
    const void *data;
    size_t data_bytes;
    unsigned int delay_ms;
} st7789_lcd_init_cmd_t;

static const st7789_lcd_init_cmd_t vendor_specific_init[] = {
    {0x11, (uint8_t []){0x00}, 1, 120},
    {0xB2, (uint8_t []){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5, 0},
    {0x35, (uint8_t []){0x00}, 1, 0},
    {0x36, (uint8_t []){0x00}, 1, 0},
    {0x3A, (uint8_t []){0x05}, 1, 0},
    {0xB7, (uint8_t []){0x35}, 1, 0},
    {0xBB, (uint8_t []){0x2D}, 1, 0},
    {0xC0, (uint8_t []){0x2C}, 1, 0},
    {0xC2, (uint8_t []){0x01}, 1, 0},
    {0xC3, (uint8_t []){0x15}, 1, 0},
    {0xC4, (uint8_t []){0x20}, 1, 0},
    {0xC6, (uint8_t []){0x0F}, 1, 0},
    {0xD0, (uint8_t []){0xA4, 0xA1}, 2, 0},
    {0xD6, (uint8_t []){0xA1}, 1, 0},
    {0xE0, (uint8_t []){0x70, 0x05, 0x0A, 0x0B, 0x0A, 0x27, 0x2F, 0x44, 0x47, 0x37, 0x14, 0x14, 0x29, 0x2F}, 14, 0},
    {0xE1, (uint8_t []){0x70, 0x07, 0x0C, 0x08, 0x08, 0x04, 0x2F, 0x33, 0x46, 0x18, 0x15, 0x15, 0x2B, 0x2D}, 14, 0},
    {0x21, (uint8_t []){0x00}, 1, 0},
    {0x29, (uint8_t []){0x00}, 1, 0},
    {0x2C, (uint8_t []){0x00}, 1, 0},
};

esp_err_t hw_lcd_init(esp_display_present_target_config_t *out_target_config)
{
    ESP_RETURN_ON_FALSE(out_target_config, ESP_ERR_INVALID_ARG, TAG, "missing target");
    const ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_1,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    const ledc_channel_config_t backlight = {
        .gpio_num = GPIO_NUM_20,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_1,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = true,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "backlight timer");
    ESP_RETURN_ON_ERROR(ledc_channel_config(&backlight), TAG, "backlight channel");

    esp_err_t ret;
    bool bus_initialized = false;
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_handle_t panel = NULL;
    const spi_bus_config_t bus = {
        .sclk_io_num = GPIO_NUM_17,
        .mosi_io_num = GPIO_NUM_16,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = HW_LCD_H_RES * HW_LCD_V_RES * sizeof(uint16_t),
    };
    ESP_GOTO_ON_ERROR(spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO), fail, TAG, "SPI bus");
    bus_initialized = true;
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = GPIO_NUM_19,
        .cs_gpio_num = GPIO_NUM_18,
        .pclk_hz = 80 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 3,
        .trans_queue_depth = 2,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                      &io_config, &io), fail, TAG, "panel IO");
    const esp_lcd_panel_dev_config_t config = {
        .reset_gpio_num = GPIO_NUM_15,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7789(io, &config, &panel), fail, TAG, "panel create");
    /* Reset before the vendor sequence so GPIO reset cannot clear it. */
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(panel), fail, TAG, "panel reset");
    for (size_t i = 0; i < sizeof(vendor_specific_init) / sizeof(vendor_specific_init[0]); ++i) {
        const st7789_lcd_init_cmd_t *cmd = &vendor_specific_init[i];
        ESP_GOTO_ON_ERROR(esp_lcd_panel_io_tx_param(io, cmd->cmd, cmd->data,
                          cmd->data_bytes), fail, TAG, "vendor command");
        vTaskDelay(pdMS_TO_TICKS(cmd->delay_ms));
    }
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(panel), fail, TAG, "panel init");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_invert_color(panel, true), fail, TAG, "panel invert");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_disp_on_off(panel, true), fail, TAG, "panel on");
    ESP_GOTO_ON_ERROR(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 1023), fail, TAG, "backlight duty");
    ESP_GOTO_ON_ERROR(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1), fail, TAG, "backlight update");
    ESP_LOGI(TAG, "ST7789 240x240 RGB565 SPI80MHz mode3, inverted backlight GPIO20");
    *out_target_config = (esp_display_present_target_config_t) {
        .hw = {
            .panel = panel,
            .io = io,
            .panel_type = ESP_DISPLAY_PRESENT_PANEL_IO,
            .input_pixel_format = ESP_DISPLAY_PRESENT_PIXEL_FORMAT_RGB565,
            .rotation = HW_DISPLAY_ROTATION,
            .swap_bytes = true,
        },
        .fb.mode = ESP_DISPLAY_PRESENT_MODE_AUTO,
    };
    return ESP_OK;

fail:
    if (panel) {
        esp_lcd_panel_del(panel);
    }
    if (io) {
        esp_lcd_panel_io_del(io);
    }
    if (bus_initialized) {
        spi_bus_free(SPI2_HOST);
    }
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    return ret;
}
