/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "watch_ui.h"
#include "bundle_gsp.h"
#include <stdio.h>
#define CHECK(call) do { esp_gsp_err_t rc = (call); if (rc != ESP_GSP_OK) return rc; } while (0)

/* Keep dismissed drawers out of closed-edge gesture arbitration. An open
 * drawer remains visible and enabled until its close animation settles. */
static esp_gsp_err_t drawer_visibility(esp_gsp_handle_t ui,
                                       gsp_component_key_t key, bool desired)
{
    bool visible, enabled;
    esp_gsp_component_motion_t motion;
    CHECK(esp_gsp_component_get_motion(ui, key, &motion));
    CHECK(esp_gsp_component_get_visible(ui, key, &visible));
    CHECK(esp_gsp_component_get_enabled(ui, key, &enabled));
    bool active = desired || motion.value || motion.dragging || motion.settling;
    if (enabled != active) {
        CHECK(esp_gsp_component_set_enabled(ui, key, active));
    }
    if (visible != active) {
        CHECK(esp_gsp_component_set_visible(ui, key, active));
    }
    return ESP_GSP_OK;
}
static esp_gsp_err_t drawer_state(esp_gsp_handle_t ui, gsp_component_key_t key, bool desired)
{
    esp_gsp_component_motion_t motion;
    CHECK(drawer_visibility(ui, key, desired));
    CHECK(esp_gsp_component_get_motion(ui, key, &motion));
    if (motion.target == desired) {
        return ESP_GSP_OK;
    }
    /* An active pointer is cancelled immediately; settles can reverse. */
    bool animated = !motion.dragging;
    return desired ? esp_gsp_drawer_open(ui, key, animated) : esp_gsp_drawer_close(ui, key, animated);
}
static esp_gsp_err_t update_indicator(esp_gsp_handle_t ui, unsigned page)
{
    char text[16];
    snprintf(text, sizeof(text), "%u / %u", page + 1, WATCH_PAGE_COUNT);
    return gsp_watch_indicator_set_text(ui, text);
}
static esp_gsp_err_t apply(esp_gsp_handle_t ui, watch_ui_t *s)
{
    if (!s->detail) {
        s->drawer_seen_open = false;
    }
    CHECK(drawer_state(ui, GSP_WATCH_OBJ_KEY_MESSAGE_PAGE, s->detail == 1));
    CHECK(drawer_state(ui, GSP_WATCH_OBJ_KEY_DETAIL_PAGE, s->detail == 2));
    CHECK(gsp_watch_orb_set_charging(ui, s->page == 3 && !s->detail && s->battery < 100));
    CHECK(gsp_watch_breath_glow_set_playing(ui, s->page == 5 && !s->detail && !s->breath_paused));
    CHECK(gsp_watch_breath_status_set_text(ui, s->breath_paused ? "PAUSED" : "BREATHE"));
    CHECK(gsp_watch_breath_toggle_set_text(ui, s->breath_paused ? "Resume" : "Pause"));
    CHECK(gsp_watch_charge_status_set_text(ui, s->battery < 100 ? "CHARGING" : "READY TO EXPLORE"));
    CHECK(gsp_watch_unread_set_text(ui, s->replied ? "REPLY SENT" : "1 NEW MESSAGE"));
    return ESP_GSP_OK;
}
static esp_gsp_err_t route_summary(esp_gsp_handle_t ui, unsigned route)
{
    static const struct {
        const char *title, *distance, *stats;
    } routes[] = {
        {"RIDGE WALK", "4.8 km", "32 MIN   /   286 KCAL"},
        {"COAST WALK", "3.2 km", "24 MIN   /   192 KCAL"},
        {"FOREST WALK", "6.1 km", "48 MIN   /   364 KCAL"}
    };
    if (route >= sizeof(routes) / sizeof(routes[0])) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    CHECK(gsp_watch_route_title_set_text(ui, routes[route].title));
    CHECK(gsp_watch_route_distance_set_text(ui, routes[route].distance));
    CHECK(gsp_watch_route_stats_set_text(ui, routes[route].stats));
    CHECK(esp_gsp_component_set_visible(ui, GSP_WATCH_OBJ_KEY_ROUTE_PHOTO, route == 0));
    CHECK(esp_gsp_component_set_visible(ui, GSP_WATCH_OBJ_KEY_ROUTE_COAST, route == 1));
    CHECK(esp_gsp_component_set_visible(ui, GSP_WATCH_OBJ_KEY_ROUTE_FOREST, route == 2));
    return ESP_GSP_OK;
}
static void event(esp_gsp_handle_t ui, const esp_gsp_event_t *e, void *ctx)
{
    watch_ui_t *s = ctx;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    bool navigate = false;
    if (gsp_watch_event_is_home(e)) {
        s->page = 0;
        s->detail = 0;
        navigate = true;
    } else if (gsp_watch_event_is_inbox(e)) {
        s->page = 2;
        s->detail = 0;
        navigate = true;
    } else if (gsp_watch_event_is_message(e)) {
        s->detail = 1;
    } else if (gsp_watch_event_is_activity_detail(e)) {
        s->detail = 2;
        s->last_error = route_summary(ui, 0);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
    } else if (gsp_watch_event_is_route_detail(e)) {
        s->detail = 2;
        s->last_error = route_summary(ui, s->trail);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
    } else if (gsp_watch_event_is_close_detail(e)) {
        s->detail = 0;
    } else if (gsp_watch_event_is_charge(e)) {
        s->page = 3;
        s->detail = 0;
        navigate = true;
    } else if (gsp_watch_event_is_reply(e)) {
        s->replied = true;
        s->detail = 0;
    } else if (gsp_watch_event_is_trail_select(e)) {
        static const char *const titles[] = {"Ridge / 4.8 km", "Coast / 3.2 km", "Forest / 6.1 km"};
        uint32_t selected;
        s->last_error = gsp_watch_trails_get_selected(ui, &selected);
        if (s->last_error == ESP_GSP_OK && selected < sizeof(titles) / sizeof(titles[0])) {
            s->trail = selected;
            s->last_error = gsp_watch_trail_title_set_text(ui, titles[selected]);
        }
        return;
    } else if (gsp_watch_event_is_breath_level_change(e)) {
        int32_t value;
        s->last_error = gsp_watch_breath_slider_get_value(ui, &value);
        if (s->last_error == ESP_GSP_OK) {
            s->glow = (unsigned)value;
            s->last_error = gsp_watch_breath_glow_set_value(ui, value);
        }
        if (s->last_error == ESP_GSP_OK) {
            char text[20];
            snprintf(text, sizeof(text), "GLOW %u%%", s->glow);
            s->last_error = gsp_watch_breath_level_set_text(ui, text);
        }
        return;
    } else if (gsp_watch_event_is_breath_pause(e)) {
        s->breath_paused = !s->breath_paused;
    } else {
        return;
    }
    if (navigate) {
        s->last_error = esp_gsp_page_flow_set_page(ui, GSP_WATCH_OBJ_KEY_PAGES, s->page, true);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
    }
    if (navigate) {
        s->last_error = update_indicator(ui, s->page);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
    }
    s->last_error = apply(ui, s);
}
static esp_gsp_err_t advance(esp_gsp_handle_t ui, watch_ui_t *s)
{
    char text[32];
    ++s->seconds;
    if (s->seconds % 60 == 0) {
        unsigned minutes = (608 + s->seconds / 60) % 1440;
        snprintf(text, sizeof(text), "%02u:%02u", minutes / 60, minutes % 60);
        CHECK(gsp_watch_time_set_text(ui, text));
    }
    if (s->page == 0 && !s->detail) {
        CHECK(gsp_watch_second_hand_set_rotation(ui, (int)((s->seconds * 6 + 180) % 360) - 180));
        if (s->seconds % 5 == 0) {
            snprintf(text, sizeof(text), "%u", 70 + s->seconds % 7);
            CHECK(gsp_watch_heart_set_text(ui, text));
        }
    }
    if (s->page == 3 && !s->detail && s->battery < 100 && s->seconds % 3 == 0) {
        ++s->battery;
        snprintf(text, sizeof(text), "%u%%", s->battery);
        CHECK(gsp_watch_battery_set_text(ui, text));
        CHECK(gsp_watch_charge_button_set_text(ui, text));
        CHECK(gsp_watch_orb_set_value(ui, s->battery));
        CHECK(apply(ui, s));
    }
    return ESP_GSP_OK;
}
static void poll_page(esp_gsp_handle_t ui, void *ctx)
{
    watch_ui_t *s = ctx;
    uint16_t page;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    s->last_error = esp_gsp_page_flow_get_page(ui, GSP_WATCH_OBJ_KEY_PAGES, &page);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    if (s->page != page) {
        s->page = page;
        s->last_error = update_indicator(ui, page);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
        s->last_error = apply(ui, s);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
    }
    if (s->detail) {
        bool open;
        s->last_error = esp_gsp_drawer_is_open(ui, s->detail == 1 ? GSP_WATCH_OBJ_KEY_MESSAGE_PAGE : GSP_WATCH_OBJ_KEY_DETAIL_PAGE, &open);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
        if (open) {
            s->drawer_seen_open = true;
        } else if (s->drawer_seen_open) {
            s->detail = 0;
            s->last_error = apply(ui, s);
            if (s->last_error != ESP_GSP_OK) {
                return;
            }
        }
    }
    s->last_error = drawer_visibility(ui, GSP_WATCH_OBJ_KEY_MESSAGE_PAGE, s->detail == 1);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    s->last_error = drawer_visibility(ui, GSP_WATCH_OBJ_KEY_DETAIL_PAGE, s->detail == 2);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
}
static void tick(esp_gsp_handle_t ui, void *ctx)
{
    watch_ui_t *s = ctx;
    if (s->last_error == ESP_GSP_OK) {
        s->last_error = advance(ui, s);
    }
}
esp_gsp_err_t watch_ui_init(esp_gsp_handle_t ui, watch_ui_t *s)
{
    if (!ui || !s) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *s = (watch_ui_t) {
        .battery = 74, .glow = 70
    };
    CHECK(gsp_watch_breath_glow_set_value(ui, s->glow));
    CHECK(apply(ui, s));
    CHECK(update_indicator(ui, s->page));
    CHECK(esp_gsp_on_event(ui, event, s));
    s->timer = esp_gsp_timer_create(ui, 1000, tick, s);
    if (!s->timer) {
        (void)esp_gsp_on_event(ui, NULL, NULL);
        return ESP_GSP_ERR_NO_MEM;
    }
    s->page_timer = esp_gsp_timer_create(ui, 50, poll_page, s);
    if (!s->page_timer) {
        watch_ui_deinit(ui, s);
        return ESP_GSP_ERR_NO_MEM;
    }
    return ESP_GSP_OK;
}
void watch_ui_deinit(esp_gsp_handle_t ui, watch_ui_t *s)
{
    if (!ui || !s) {
        return;
    }
    if (s->page_timer) {
        (void)esp_gsp_timer_delete(ui, s->page_timer);
        s->page_timer = NULL;
    }
    if (s->timer) {
        (void)esp_gsp_timer_delete(ui, s->timer);
        s->timer = NULL;
    }
    (void)esp_gsp_on_event(ui, NULL, NULL);
}
