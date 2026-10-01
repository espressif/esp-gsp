/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "custom_draw.h"

#include "bundle_gsp.h"
#include "esp_gsp_esp_lcd.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hw_init.h"

#ifndef GSP_CUSTOM_DRAW_RGB888
#define GSP_CUSTOM_DRAW_RGB888 0
#endif

static const char *TAG = "custom_draw";
static custom_draw_state_t s_state;

static void custom_draw_log_loop(void *user_ctx)
{
    custom_draw_state_t *state = user_ctx;
    for (;;) {
        custom_draw_hook_snapshot_t hooks;
        custom_draw_hooks_snapshot(&hooks);
        ESP_LOGI(TAG,
                 "diag draw_seen=%d draw_ready=%d draw_status=%ld "
                 "draw_submit=%ld check_seen=%d check_status=%ld "
                 "draw_calls=%u timer_ticks=%u invalidates_failed=%u "
                 "hooks(created=%u destroying=%u scene=%u action=%u "
                 "canvas=%u render_error=%u)",
                 atomic_load(&state->draw_result_seen),
                 atomic_load(&state->draw_ready),
                 (long)atomic_load(&state->draw_result_status),
                 (long)atomic_load(&state->draw_submit_status),
                 atomic_load(&state->check_result_seen),
                 (long)atomic_load(&state->check_result_status),
                 (unsigned)atomic_load(&state->draw_calls),
                 (unsigned)atomic_load(&state->timer_ticks),
                 (unsigned)atomic_load(&state->invalidate_failures),
                 (unsigned)hooks.app_created,
                 (unsigned)hooks.app_destroying,
                 (unsigned)hooks.scene_changed,
                 (unsigned)hooks.actions,
                 (unsigned)hooks.canvas_results,
                 (unsigned)hooks.render_errors);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    esp_display_present_target_config_t display;
#if GSP_CUSTOM_DRAW_RGB888
#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
    ESP_ERROR_CHECK(hw_lcd_init_mipi(&display, 24));
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#else
#error "GSP_CUSTOM_DRAW_RGB888 requires MIPI-DSI or 24-bit RGB"
#endif
#else
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#endif

    esp_gsp_config_t app_config = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t esp_config = ESP_GSP_ESP_LCD_CONFIG_INIT();
    esp_config.display = display;
    esp_gsp_handle_t ui = NULL;
    const esp_err_t start_ret = esp_gsp_esp_lcd_start_prepared(
                                    &app_config, &esp_config, custom_draw_prepare, &s_state, &ui);
    if (start_ret != ESP_OK) {
        ESP_LOGE(TAG, "GSP start failed: %d", (int)start_ret);
        return;
    }
    const int32_t prepare_status =
        atomic_load_explicit(&s_state.prepare_status, memory_order_acquire);
    if (prepare_status != ESP_GSP_OK) {
        ESP_LOGE(TAG, "prepare submission failed: %ld", (long)prepare_status);
    }
    ESP_LOGI(TAG,
             "started format=%s; Canvas draw admission is reported "
             "asynchronously, not inferred from start success",
             GSP_CUSTOM_DRAW_RGB888 ? "RGB888" : "RGB565");
    custom_draw_log_loop(&s_state);
}
