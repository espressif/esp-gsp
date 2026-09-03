/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* 800x480 parallel RGB panels. */

#include "hw_init.h"

#if CONFIG_EXAMPLE_LCD_INTERFACE_RGB || CONFIG_EXAMPLE_LCD_INTERFACE_RGB24

#include "driver/gpio.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_log.h"

static const char *TAG = "hw_lcd_rgb";

#if CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
#define HW_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)
#define HW_LCD_DMA_BURST_SIZE 128
#define HW_LCD_DATA_WIDTH     24
#define HW_LCD_COLOR_FORMAT   LCD_COLOR_FMT_RGB888
#elif CONFIG_IDF_TARGET_ESP32S3
#define HW_LCD_PIXEL_CLOCK_HZ (18 * 1000 * 1000)
#define HW_LCD_DMA_BURST_SIZE 64
#define HW_LCD_DATA_WIDTH     16
#define HW_LCD_COLOR_FORMAT   LCD_COLOR_FMT_RGB565
#else
#define HW_LCD_PIXEL_CLOCK_HZ (26 * 1000 * 1000)
// #define HW_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)
#define HW_LCD_DMA_BURST_SIZE 128
#define HW_LCD_DATA_WIDTH     16
#define HW_LCD_COLOR_FORMAT   LCD_COLOR_FMT_RGB565
#endif

#define HW_LCD_HSYNC 1
#define HW_LCD_HBP   40
#define HW_LCD_HFP   20
#define HW_LCD_VSYNC 1
#define HW_LCD_VBP   10
#define HW_LCD_VFP   5

#if CONFIG_IDF_TARGET_ESP32S31
#define HW_LCD_RGB_VSYNC  GPIO_NUM_45
#define HW_LCD_RGB_HSYNC  GPIO_NUM_44
#define HW_LCD_RGB_DE     GPIO_NUM_43
#define HW_LCD_RGB_PCLK   GPIO_NUM_40
#define HW_LCD_RGB_DISP   GPIO_NUM_NC
#define HW_LCD_RGB_DATA0  GPIO_NUM_8
#define HW_LCD_RGB_DATA1  GPIO_NUM_9
#define HW_LCD_RGB_DATA2  GPIO_NUM_10
#define HW_LCD_RGB_DATA3  GPIO_NUM_11
#define HW_LCD_RGB_DATA4  GPIO_NUM_12
#define HW_LCD_RGB_DATA5  GPIO_NUM_13
#define HW_LCD_RGB_DATA6  GPIO_NUM_14
#define HW_LCD_RGB_DATA7  GPIO_NUM_15
#define HW_LCD_RGB_DATA8  GPIO_NUM_16
#define HW_LCD_RGB_DATA9  GPIO_NUM_17
#define HW_LCD_RGB_DATA10 GPIO_NUM_18
#define HW_LCD_RGB_DATA11 GPIO_NUM_19
#define HW_LCD_RGB_DATA12 GPIO_NUM_33
#define HW_LCD_RGB_DATA13 GPIO_NUM_34
#define HW_LCD_RGB_DATA14 GPIO_NUM_35
#define HW_LCD_RGB_DATA15 GPIO_NUM_36
#if CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
#define HW_LCD_RGB_DATA16 GPIO_NUM_37
#define HW_LCD_RGB_DATA17 GPIO_NUM_38
#define HW_LCD_RGB_DATA18 GPIO_NUM_39
#define HW_LCD_RGB_DATA19 GPIO_NUM_2
#define HW_LCD_RGB_DATA20 GPIO_NUM_3
#define HW_LCD_RGB_DATA21 GPIO_NUM_4
#define HW_LCD_RGB_DATA22 GPIO_NUM_5
#define HW_LCD_RGB_DATA23 GPIO_NUM_7
#endif
#else
#define HW_LCD_RGB_VSYNC  GPIO_NUM_3
#define HW_LCD_RGB_HSYNC  GPIO_NUM_46
#define HW_LCD_RGB_DE     GPIO_NUM_17
#define HW_LCD_RGB_PCLK   GPIO_NUM_9
#define HW_LCD_RGB_DISP   GPIO_NUM_NC
#define HW_LCD_RGB_DATA0  GPIO_NUM_10
#define HW_LCD_RGB_DATA1  GPIO_NUM_11
#define HW_LCD_RGB_DATA2  GPIO_NUM_12
#define HW_LCD_RGB_DATA3  GPIO_NUM_13
#define HW_LCD_RGB_DATA4  GPIO_NUM_14
#define HW_LCD_RGB_DATA5  GPIO_NUM_21
#define HW_LCD_RGB_DATA6  GPIO_NUM_8
#define HW_LCD_RGB_DATA7  GPIO_NUM_18
#define HW_LCD_RGB_DATA8  GPIO_NUM_45
#define HW_LCD_RGB_DATA9  GPIO_NUM_38
#define HW_LCD_RGB_DATA10 GPIO_NUM_39
#define HW_LCD_RGB_DATA11 GPIO_NUM_40
#define HW_LCD_RGB_DATA12 GPIO_NUM_41
#define HW_LCD_RGB_DATA13 GPIO_NUM_42
#define HW_LCD_RGB_DATA14 GPIO_NUM_2
#define HW_LCD_RGB_DATA15 GPIO_NUM_1
#endif

#define HW_LCD_BOUNCE_BUFFER_HEIGHT 20

esp_err_t hw_lcd_init(
    esp_display_present_target_config_t *out_target_config)
{
    ESP_LOGI(TAG,
             "Initialize RGB panel (%dx%d, %u-bit, pclk=%u Hz,"
             " DMA burst=%u bytes, 3 fbs)",
             HW_LCD_H_RES, HW_LCD_V_RES, HW_LCD_DATA_WIDTH,
             HW_LCD_PIXEL_CLOCK_HZ, HW_LCD_DMA_BURST_SIZE);
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .dma_burst_size = HW_LCD_DMA_BURST_SIZE,
        .data_width = HW_LCD_DATA_WIDTH,
        .in_color_format = HW_LCD_COLOR_FORMAT,
        .de_gpio_num = HW_LCD_RGB_DE,
        .pclk_gpio_num = HW_LCD_RGB_PCLK,
        .vsync_gpio_num = HW_LCD_RGB_VSYNC,
        .hsync_gpio_num = HW_LCD_RGB_HSYNC,
        .disp_gpio_num = HW_LCD_RGB_DISP,
        .data_gpio_nums = {
            HW_LCD_RGB_DATA0, HW_LCD_RGB_DATA1, HW_LCD_RGB_DATA2,
            HW_LCD_RGB_DATA3, HW_LCD_RGB_DATA4, HW_LCD_RGB_DATA5,
            HW_LCD_RGB_DATA6, HW_LCD_RGB_DATA7, HW_LCD_RGB_DATA8,
            HW_LCD_RGB_DATA9, HW_LCD_RGB_DATA10, HW_LCD_RGB_DATA11,
            HW_LCD_RGB_DATA12, HW_LCD_RGB_DATA13, HW_LCD_RGB_DATA14,
            HW_LCD_RGB_DATA15,
#if CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
            HW_LCD_RGB_DATA16, HW_LCD_RGB_DATA17, HW_LCD_RGB_DATA18,
            HW_LCD_RGB_DATA19, HW_LCD_RGB_DATA20, HW_LCD_RGB_DATA21,
            HW_LCD_RGB_DATA22, HW_LCD_RGB_DATA23,
#endif
        },
        .timings = {
            .pclk_hz = HW_LCD_PIXEL_CLOCK_HZ,
            .h_res = HW_LCD_H_RES,
            .v_res = HW_LCD_V_RES,
            .hsync_back_porch = HW_LCD_HBP,
            .hsync_front_porch = HW_LCD_HFP,
            .hsync_pulse_width = HW_LCD_HSYNC,
            .vsync_back_porch = HW_LCD_VBP,
            .vsync_front_porch = HW_LCD_VFP,
            .vsync_pulse_width = HW_LCD_VSYNC,
            .flags = {
                .pclk_active_neg = true,
            },
        },
        .num_fbs = 3,
        .flags.fb_in_psram = 1,
#if CONFIG_IDF_TARGET_ESP32S3
        .bounce_buffer_size_px = HW_LCD_H_RES * HW_LCD_BOUNCE_BUFFER_HEIGHT,
#endif
    };
    esp_lcd_panel_handle_t panel;
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));

    *out_target_config = (esp_display_present_target_config_t) {
        .hw = {
            .panel = panel,
            .panel_type = ESP_DISPLAY_PRESENT_PANEL_RGB,
            .input_pixel_format = HW_LCD_DATA_WIDTH == 24
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

#endif
