/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* Minimal ESP-GSP application: initialize the panel, start the compiled
 * bundle and update one generated control from a timer. */

#include "esp_gsp_esp_lcd.h"
#include "esp_log.h"
#include "hw_init.h"

#include "bundle_gsp.h"
#include "hello_ui.h"

#ifndef GSP_HELLO_RGB888
#define GSP_HELLO_RGB888 0
#endif

static const char *TAG = "gsp_hello";

static hello_ui_t s_hello;

void app_main(void)
{
    esp_display_present_target_config_t display;
#if GSP_HELLO_RGB888
#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
    ESP_ERROR_CHECK(hw_lcd_init_mipi(&display, 24));
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#else
#error "GSP_HELLO_RGB888 requires MIPI-DPI or 24-bit RGB"
#endif
#else
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#endif

    esp_lcd_touch_handle_t touch = NULL;
    (void)hw_touch_init(&touch);    /* optional: run without touch */

    esp_gsp_handle_t ui;
    esp_gsp_config_t app_config = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t esp_config =
        ESP_GSP_ESP_LCD_CONFIG_INIT();
    esp_config.display = display;
    esp_config.touch = touch;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app_config, &esp_config, &ui));
    ESP_ERROR_CHECK(hello_ui_init(ui, &s_hello));
    ESP_LOGI(TAG, "Minimal UI started, pixel_format=%s",
             GSP_HELLO_RGB888 ? "RGB888" : "RGB565");
}
