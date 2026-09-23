/* SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include <inttypes.h>
#include "esp_gsp_esp_lcd.h"
#include "esp_gsp_debug.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "hw_init.h"
#include "bundle_gsp.h"
#ifndef GSP_EFFECTS_RGB888
#define GSP_EFFECTS_RGB888 0
#endif
#ifndef GSP_EFFECTS_AUTOPLAY
#define GSP_EFFECTS_AUTOPLAY 0
#endif
#include "effects_preview.h"
static const char *TAG = "gsp_effects";
static effects_preview_t preview;
typedef struct {
    uint32_t frames;
    uint64_t busy, raster, submit;
    int64_t time;
    unsigned tick, selected;
} preview_stats_t;
static preview_stats_t stats;

/* Serialize preview changes with touch callbacks and release the app task. */
static void preview_tick(esp_gsp_handle_t ui, void *ctx)
{
    preview_stats_t *previous = ctx;
    uint32_t frames, errors;
    uint64_t busy, raster, submit;
    gsp_err_t last;
    esp_gsp_render_stats(ui, &frames, &busy);
    esp_gsp_render_error_stats(ui, &errors, &last);
    esp_gsp_render_phases(ui, &raster, &submit);
    int64_t now = esp_timer_get_time();
    uint32_t count = frames - previous->frames;
    ESP_LOGI(TAG, "sample page=%u style=%u frames=%" PRIu32 " fps_x10=%" PRIu64 " busy_us=%" PRIu64 " raster_us=%" PRIu64 " submit_us=%" PRIu64 " errors=%" PRIu32 " last=%d ui_error=%d heap=%u min_heap=%u",
             preview.page, preview.style[preview.page], count,
             (uint64_t)count * 10000000 / (uint64_t)(now - previous->time),
             count ? (busy - previous->busy) / count : 0,
             count ? (raster - previous->raster) / count : 0,
             count ? (submit - previous->submit) / count : 0,
             errors, last, preview.last_error,
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
             (unsigned)heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL));
    previous->frames = frames;
    previous->busy = busy;
    previous->raster = raster;
    previous->submit = submit;
    previous->time = now;
#if GSP_EFFECTS_AUTOPLAY
    if (preview.page == 2) {
        uint32_t choice = ++previous->selected % 5;
        ESP_ERROR_CHECK(gsp_effects_menu_flat_set_selected(ui, choice));
        ESP_ERROR_CHECK(gsp_effects_menu_tilt_set_selected(ui, choice));
        ESP_ERROR_CHECK(gsp_effects_menu_perspective_set_selected(ui, choice));
    }
    preview.style[preview.page] = previous->tick % 3;
    if (previous->tick % 5 == 4) {
        preview.page = (preview.page + 1) % EFFECTS_PAGE_COUNT;
    }
    ESP_ERROR_CHECK(effects_preview_apply(ui, &preview));
    if (preview.page == 6) {
        /* Automatic preview has no pointer; manual taps use the play_glow action. */
        esp_gsp_component_info_t feedback;
        ESP_ERROR_CHECK(gsp_effects_feedback_get_info(ui, &feedback));
        ESP_ERROR_CHECK(esp_gsp_component_animate_value(ui, feedback.key, 100, 0, 850, ESP_GSP_EASE_OUT));
    }
    if (preview.page == 7) {
        ESP_ERROR_CHECK(effects_preview_flip(ui, &preview));
    }
    if (preview.page == 4) {
        ESP_ERROR_CHECK(effects_preview_play(ui, &preview));
    }
    if (previous->tick % (EFFECTS_PAGE_COUNT * 5) == EFFECTS_PAGE_COUNT * 5 - 1) {
        ESP_LOGI(TAG, "qualification cycle complete errors=%" PRIu32, errors);
    }
    ++previous->tick;
#endif
}
void app_main(void)
{
    esp_display_present_target_config_t display;
#if GSP_EFFECTS_RGB888 && CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
    ESP_ERROR_CHECK(hw_lcd_init_mipi(&display, 24));
#else
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#endif
    esp_lcd_touch_handle_t touch = NULL;
    esp_err_t touch_ret = hw_touch_init(&touch);
    if (touch_ret != ESP_OK) {
        ESP_LOGW(TAG, "Touch unavailable: %s", esp_err_to_name(touch_ret));
    }
    esp_gsp_config_t config = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t panel = ESP_GSP_ESP_LCD_CONFIG_INIT();
    panel.display = display; panel.touch = touch;
    esp_gsp_handle_t ui;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&config, &panel, &ui));
    ESP_ERROR_CHECK(effects_preview_init(ui, &preview));
    ESP_LOGI(TAG, "Effects started: format=%s autoplay=%d touch=%d", GSP_EFFECTS_RGB888 ? "RGB888" : "RGB565", GSP_EFFECTS_AUTOPLAY, touch != NULL);
    stats.time = esp_timer_get_time();
    if (esp_gsp_timer_create(ui, 2000, preview_tick, &stats) == NULL) {
        ESP_LOGE(TAG, "Cannot create preview timer");
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
}
