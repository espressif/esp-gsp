/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include <stdbool.h>
#include <stdint.h>

#include "esp_gsp_esp_lcd.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hw_init.h"

#define GSP_BUNDLE_ENABLE_RAW_IDS 1
#include "bundle_gsp.h"

#ifndef GSP_SHOWCASE_RGB888
#define GSP_SHOWCASE_RGB888 0
#endif

#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
#define SHOWCASE_WIDTH 1024
#define SHOWCASE_HEIGHT 600
#else
#define SHOWCASE_WIDTH 800
#define SHOWCASE_HEIGHT 480
#endif

static const char *TAG = "gsp_showcase";

static uint16_t s_scene = GSP_BUNDLE_SCENE_SHOWCASE_COMMAND;
static int64_t s_clock_started_us;
static esp_gsp_list_t s_mode_wheel = ESP_GSP_LIST_NONE;
static bool s_lab_components_visible;

static void log_gsp_error(const char *operation, esp_gsp_err_t err)
{
    if (err != ESP_GSP_OK) {
        ESP_LOGE(TAG, "%s failed: %d", operation, (int)err);
    }
}

static void clock_tick(esp_gsp_handle_t ui, void *user_ctx)
{
    (void)user_ctx;
    if (s_scene != GSP_BUNDLE_SCENE_SHOWCASE_COMMAND) {
        return;
    }

    uint32_t elapsed_seconds = (uint32_t)(
                                   (esp_timer_get_time() - s_clock_started_us) / 1000000);
    const uint32_t total_seconds = 19U * 3600U + 42U * 60U + elapsed_seconds;
    log_gsp_error(
        "update clock",
        gsp_showcase_command_hero_clock_set_time(
            ui, (uint8_t)((total_seconds / 3600U) % 24U),
            (uint8_t)((total_seconds / 60U) % 60U),
            (uint8_t)(total_seconds % 60U)));
}

static void show_lab_components(esp_gsp_handle_t ui, bool show)
{
    s_lab_components_visible = show;
    log_gsp_error(
        "set Lab navigation visibility",
        esp_gsp_set_visible(ui, GSP_SHOWCASE_LAB_BIND_LAB_NAVIGATION, !show));
    log_gsp_error(
        "set Lab component visibility",
        esp_gsp_set_visible(ui, GSP_SHOWCASE_LAB_BIND_LAB_COMPONENTS, show));

    if (show) {
        log_gsp_error(
            "clear keyboard input",
            esp_gsp_set_text(ui, GSP_SHOWCASE_LAB_BIND_KEYBOARD_INPUT, ""));
        log_gsp_error(
            "set keyboard cursor",
            esp_gsp_set_cursor(ui, GSP_SHOWCASE_LAB_BIND_KEYBOARD_INPUT));
        log_gsp_error(
            "attach keyboard",
            esp_gsp_keyboard_attach(
                ui, GSP_SHOWCASE_LAB_ACT_ID_FEATURE_KEYBOARD_KEY,
                GSP_SHOWCASE_LAB_BIND_KEYBOARD_INPUT));
        log_gsp_error(
            "select component tab",
            esp_gsp_page_flow_set_page(
                ui, GSP_SHOWCASE_LAB_OBJ_KEY_FEATURE_TABS, 0, false));
    } else {
        log_gsp_error(
            "clear keyboard cursor",
            esp_gsp_set_cursor(ui, ESP_GSP_NO_CURSOR));
        log_gsp_error(
            "detach keyboard",
            esp_gsp_keyboard_attach(
                ui, ESP_GSP_KEYBOARD_NONE,
                GSP_SHOWCASE_LAB_BIND_KEYBOARD_INPUT));
    }
}

static void showcase_event(esp_gsp_handle_t ui,
                           const esp_gsp_event_t *event, void *user_ctx)
{
    (void)user_ctx;
    if (event == NULL) {
        return;
    }

    if (event->type == ESP_GSP_EVENT_SCENE_CHANGED) {
        s_scene = event->scene_id;
        ESP_LOGI(TAG, "Scene %u ready", (unsigned)s_scene);
        if (s_scene == GSP_BUNDLE_SCENE_SHOWCASE_LAB) {
            show_lab_components(ui, false);
            if (s_mode_wheel == ESP_GSP_LIST_NONE) {
                s_mode_wheel = gsp_showcase_lab_mode_wheel_bind(
                                   ui, NULL, NULL);
                if (s_mode_wheel == ESP_GSP_LIST_NONE) {
                    ESP_LOGE(TAG, "Failed to bind mode wheel");
                }
            }
            if (s_mode_wheel != ESP_GSP_LIST_NONE) {
                log_gsp_error(
                    "snap mode wheel",
                    esp_gsp_list_snap(ui, s_mode_wheel, true));
            }
        }
        return;
    }

    if (event->scene_id == GSP_BUNDLE_SCENE_SHOWCASE_COMMAND &&
            event->action_id == GSP_SHOWCASE_COMMAND_ACT_ID_QUICK_ACTION) {
        bool open = false;
        esp_gsp_err_t err = esp_gsp_drawer_is_open(
                                ui, GSP_SHOWCASE_COMMAND_OBJ_KEY_QUICK_SETTINGS, &open);
        if (err == ESP_GSP_OK) {
            err = open
                  ? esp_gsp_drawer_close(
                      ui, GSP_SHOWCASE_COMMAND_OBJ_KEY_QUICK_SETTINGS, true)
                  : esp_gsp_drawer_open(
                      ui, GSP_SHOWCASE_COMMAND_OBJ_KEY_QUICK_SETTINGS, true);
        }
        log_gsp_error("toggle quick settings", err);
    } else if (event->scene_id == GSP_BUNDLE_SCENE_SHOWCASE_LAB &&
               event->action_id == GSP_SHOWCASE_LAB_ACT_ID_LAB_TRIGGER) {
        show_lab_components(ui, !s_lab_components_visible);
    } else if (event->scene_id == GSP_BUNDLE_SCENE_SHOWCASE_LAB &&
               event->action_id == GSP_SHOWCASE_LAB_ACT_ID_MODAL_CHOICE) {
        const char *status = event->arg == 1
                             ? "Applied  |  Focus mode  |  Level 64"
                             : "Cancelled  |  Settings remain unchanged";
        log_gsp_error(
            "update confirmation status",
            gsp_showcase_lab_confirm_status_set_text(ui, status));
    }
}

void app_main(void)
{
    esp_display_present_target_config_t display;
#if GSP_SHOWCASE_RGB888
#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
    ESP_ERROR_CHECK(hw_lcd_init_mipi(&display, 24));
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#else
#error "GSP_SHOWCASE_RGB888 needs MIPI-DPI or 24-bit RGB"
#endif
#else
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#endif

    esp_lcd_touch_handle_t touch = NULL;
    esp_err_t touch_ret = hw_touch_init(&touch);
    if (touch_ret != ESP_OK) {
        ESP_LOGW(TAG, "Touch unavailable: %s", esp_err_to_name(touch_ret));
    }

    esp_gsp_config_t app_config = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t lcd_config = ESP_GSP_ESP_LCD_CONFIG_INIT();
    lcd_config.display = display;
    lcd_config.touch = touch;

    esp_gsp_handle_t ui;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app_config, &lcd_config, &ui));
    ESP_ERROR_CHECK(esp_gsp_on_event(ui, showcase_event, NULL));
    ESP_ERROR_CHECK(esp_gsp_set_swipe_transition(
                        ui, ESP_GSP_SWIPE_SLIDE_FADE_THROUGH_BLACK));
    ESP_ERROR_CHECK(esp_gsp_set_swipe_fade_black_point(ui, 42));

    s_clock_started_us = esp_timer_get_time();
    if (esp_gsp_timer_create(ui, 1000, clock_tick, NULL) == NULL) {
        ESP_LOGE(TAG, "Failed to create clock timer");
        return;
    }

    ESP_LOGI(TAG, "Showcase started: %ux%u %s, user control + %s",
             SHOWCASE_WIDTH, SHOWCASE_HEIGHT,
             GSP_SHOWCASE_RGB888 ? "RGB888" : "RGB565",
             touch != NULL ? "touch" : "no touch");
}
