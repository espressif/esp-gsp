/* SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "thermostat_ui.h"
#include "bundle_gsp.h"
#include <stdio.h>
#define CHECK(call) do { esp_gsp_err_t e=(call); if(e!=ESP_GSP_OK)return e; } while(0)
static const struct {
    const char *name;
    int half_degrees;
} presets[] = {
    {"HOME", 44}, {"AWAY", 36}, {"NIGHT", 38}
};
static esp_gsp_err_t refresh(esp_gsp_handle_t ui, thermostat_ui_t *s)
{
    char value[24], status[32];
    static const char *const modes[] = {"HEAT", "COOL", "OFF"};
    static const char *const hints[] = {"Comfort, naturally.", "Fresh air, softly.", "Saving energy."};
    snprintf(value, sizeof(value), "%d.%d", s->half_degrees / 2, s->half_degrees % 2 * 5);
    CHECK(gsp_thermostat_value_set_text(ui, value));
    snprintf(status, sizeof(status), "%s · ROOM 20.5°", modes[s->mode]);
    CHECK(gsp_thermostat_status_set_text(ui, status));
    CHECK(gsp_thermostat_mode_value_set_text(ui, modes[s->mode]));
    CHECK(gsp_thermostat_warm_house_set_visible(ui, s->mode == 0));
    CHECK(gsp_thermostat_cool_house_set_visible(ui, s->mode == 1));
    CHECK(gsp_thermostat_eco_leaf_set_visible(ui, s->mode == 2));
    CHECK(gsp_thermostat_mode_hint_set_text(ui, hints[s->mode]));
    snprintf(value, sizeof(value), "%02d", s->hours);
    CHECK(gsp_thermostat_hours_set_text(ui, value));
    CHECK(gsp_thermostat_timer_hint_set_text(ui, s->hours ? "HOURS · AUTO OFF" : "HOURS · DISABLED"));
    CHECK(gsp_thermostat_ring_set_value(ui, (s->half_degrees - 32) * 100 / 28));
    CHECK(gsp_thermostat_warmth_set_playing(ui, s->page == 0 && s->mode != 2));
    CHECK(gsp_thermostat_warmth_set_fg_color_rgb888(ui, s->mode == 1 ? 0x7BC7DD : 0xE8BA73));
    CHECK(gsp_thermostat_preset_name_set_text(ui, presets[s->preset].name));
    snprintf(status, sizeof(status), "%d C / PRESS TO APPLY", presets[s->preset].half_degrees / 2);
    CHECK(gsp_thermostat_preset_hint_set_text(ui, status));
    CHECK(gsp_thermostat_preset_ring_set_value(ui, (presets[s->preset].half_degrees - 32) * 100 / 28));
    return ESP_GSP_OK;
}
void thermostat_ui_action(esp_gsp_handle_t ui, thermostat_ui_t *s, thermostat_action_t action)
{
    if (!ui || !s) {
        return;
    }
    /* A just-settled swipe may precede the next navigation poll. Resolve the
     * real page now so an immediate knob/button action edits that page. */
    uint16_t page;
    s->last_error = esp_gsp_page_flow_get_page(ui, GSP_THERMOSTAT_OBJ_KEY_PAGES, &page);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    s->page = page;
    if (action == THERMOSTAT_BACK || action == THERMOSTAT_CONFIRM) {
        if (action == THERMOSTAT_CONFIRM && s->page == 3) {
            s->half_degrees = presets[s->preset].half_degrees;
            s->mode = 0;
        }
        s->page = action == THERMOSTAT_BACK ? 0 : (s->page + 1) % 4;
        /* Knob navigation is atomic on the no-PSRAM board. Pointer navigation
         * still follows the finger through the full PageFlow viewport. */
        s->last_error = esp_gsp_page_flow_set_page(ui, GSP_THERMOSTAT_OBJ_KEY_PAGES, s->page, false);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
    } else {
        int delta = action == THERMOSTAT_INCREASE ? 1 : -1;
        if (s->page == 0) {
            s->half_degrees += delta;
            if (s->half_degrees < 32) {
                s->half_degrees = 32;
            } if (s->half_degrees > 60) {
                s->half_degrees = 60;
            }
        } else if (s->page == 1) {
            s->mode = (s->mode + delta + 3) % 3;
        } else if (s->page == 3) {
            s->preset = (s->preset + delta + 3) % 3;
        } else {
            s->hours += delta;
            s->seconds = 0;
            if (s->hours < 0) {
                s->hours = 0;
            } if (s->hours > 12) {
                s->hours = 12;
            }
        }
    }
    s->last_error = refresh(ui, s);
}
static void event(esp_gsp_handle_t ui, const esp_gsp_event_t *e, void *ctx)
{
    if (gsp_thermostat_event_is_decrease(e)) {
        thermostat_ui_action(ui, ctx, THERMOSTAT_DECREASE);
    } else if (gsp_thermostat_event_is_increase(e)) {
        thermostat_ui_action(ui, ctx, THERMOSTAT_INCREASE);
    } else if (gsp_thermostat_event_is_confirm(e)) {
        thermostat_ui_action(ui, ctx, THERMOSTAT_CONFIRM);
    } else if (gsp_thermostat_event_is_back(e)) {
        thermostat_ui_action(ui, ctx, THERMOSTAT_BACK);
    }
}
static void navigation_tick(esp_gsp_handle_t ui, void *ctx)
{
    thermostat_ui_t *s = ctx; uint16_t page = 0;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    s->last_error = esp_gsp_page_flow_get_page(ui, GSP_THERMOSTAT_OBJ_KEY_PAGES, &page);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    if (page != s->page) {
        s->page = page;
        s->last_error = refresh(ui, s);
    }
}
static void timer_tick(esp_gsp_handle_t ui, void *ctx)
{
    thermostat_ui_t *s = ctx;
    if (s->last_error != ESP_GSP_OK || s->hours == 0) {
        return;
    }
    if (++s->seconds < 3600) {
        return;
    }
    s->seconds = 0; if (--s->hours == 0)s->mode = 2;
    s->last_error = refresh(ui, s);
}
esp_gsp_err_t thermostat_ui_init(esp_gsp_handle_t ui, thermostat_ui_t *s)
{
    if (!ui || !s) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *s = (thermostat_ui_t) {
        .half_degrees = 44
    };
    s->last_error = refresh(ui, s); if (s->last_error != ESP_GSP_OK)return s->last_error;
    s->timer = esp_gsp_timer_create(ui, 1000, timer_tick, s); if (!s->timer)return ESP_GSP_ERR_NO_MEM;
    s->navigation_timer = esp_gsp_timer_create(ui, 50, navigation_tick, s);
    if (!s->navigation_timer) {
        thermostat_ui_deinit(ui, s);
        return ESP_GSP_ERR_NO_MEM;
    }
    s->last_error = esp_gsp_on_event(ui, event, s); if (s->last_error != ESP_GSP_OK)thermostat_ui_deinit(ui, s);
    return s->last_error;
}
void thermostat_ui_deinit(esp_gsp_handle_t ui, thermostat_ui_t *s)
{
    if (!ui || !s) {
        return;
    }
    (void)esp_gsp_on_event(ui, NULL, NULL);
    if (s->navigation_timer) {
        (void)esp_gsp_timer_delete(ui, s->navigation_timer);
        s->navigation_timer = NULL;
    }
    if (s->timer) {
        (void)esp_gsp_timer_delete(ui, s->timer);
        s->timer = NULL;
    }
}
