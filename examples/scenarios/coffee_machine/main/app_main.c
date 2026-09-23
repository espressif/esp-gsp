/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "esp_gsp_esp_lcd.h"
#include "esp_log.h"
#include "hw_init.h"
#include "bundle_gsp.h"
#include "coffee_ui.h"
static coffee_ui_t state;
static const char *TAG = "coffee";

static void initialize_controller(esp_gsp_handle_t ui, void *ctx)
{
    *(esp_gsp_err_t *)ctx = coffee_ui_init(ui, &state);
#ifdef COFFEE_DEMO_PROBE
    if (*(esp_gsp_err_t *)ctx == ESP_GSP_OK) {
        extern void coffee_probe_start(esp_gsp_handle_t ui);
        coffee_probe_start(ui);
    }
#endif
}

void app_main(void)
{
    esp_display_present_target_config_t display;
    ESP_ERROR_CHECK(hw_lcd_init(&display));
    esp_lcd_touch_handle_t touch = NULL;
#if HW_USE_TOUCH
    ESP_ERROR_CHECK(hw_touch_init(&touch));
#else
    ESP_LOGW(TAG, "Touch unavailable for the selected RGB888 profile");
#endif
    esp_gsp_config_t config = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t panel = ESP_GSP_ESP_LCD_CONFIG_INIT();
    panel.display = display; panel.touch = touch;
    esp_gsp_handle_t ui;
    esp_gsp_err_t initialized = ESP_GSP_FAIL;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start_prepared(
                        &config, &panel, initialize_controller, &initialized, &ui));
    ESP_ERROR_CHECK(initialized);
    ESP_LOGI(TAG, "AURA coffee: 800x480 %s, touch=%s, simulated appliance",
#if CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
             "RGB888",
#else
             "RGB565",
#endif
             touch ? "yes" : "no");
}
