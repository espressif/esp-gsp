/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "esp_gsp_esp_lcd.h"
#include "esp_log.h"
#include "hw_init.h"
#include "bundle_gsp.h"
#include "industrial_ui.h"
static industrial_ui_t state;
static void prepare(esp_gsp_handle_t ui, void *ctx)
{
    *(esp_gsp_err_t *)ctx = industrial_ui_init(ui, &state);
}
void app_main(void)
{
    esp_display_present_target_config_t display;
    ESP_ERROR_CHECK(hw_lcd_init(&display));
    esp_lcd_touch_handle_t touch = NULL;
    ESP_ERROR_CHECK(hw_touch_init(&touch));
    esp_gsp_handle_t ui;
    esp_gsp_config_t app = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t config = ESP_GSP_ESP_LCD_CONFIG_INIT();
    config.display = display;
    config.touch = touch;
    esp_gsp_err_t initialized = ESP_GSP_FAIL;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start_prepared(&app, &config, prepare, &initialized, &ui));
    ESP_ERROR_CHECK(initialized);
}
