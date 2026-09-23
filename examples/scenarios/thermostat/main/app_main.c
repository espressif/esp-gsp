/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "esp_gsp_esp_lcd.h"
#include "hw_init.h"
#include "bundle_gsp.h"
#include "thermostat_ui.h"
#include "knob_input.h"

static thermostat_ui_t state;
static void prepare(esp_gsp_handle_t ui, void *ctx)
{
    *(esp_gsp_err_t *)ctx = thermostat_ui_init(ui, &state);
}
void app_main(void)
{
    esp_display_present_target_config_t display;
    ESP_ERROR_CHECK(hw_lcd_init(&display));
    esp_gsp_esp_lcd_config_t panel = ESP_GSP_ESP_LCD_CONFIG_INIT();
    panel.display = display;
    esp_gsp_config_t config = gsp_bundle_config();
    esp_gsp_handle_t ui;
    esp_gsp_err_t initialized = ESP_GSP_FAIL;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start_prepared(&config, &panel, prepare, &initialized, &ui));
    ESP_ERROR_CHECK(initialized);
    ESP_ERROR_CHECK(knob_input_start(ui, &state));
}
