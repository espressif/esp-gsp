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
    CHECK(gsp_watch_page_dot0_set_visible(ui, page == 0));
    CHECK(gsp_watch_page_dot1_set_visible(ui, page == 1));
    CHECK(gsp_watch_page_dot2_set_visible(ui, page == 2));
    CHECK(gsp_watch_page_dot3_set_visible(ui, page == 3));
    CHECK(gsp_watch_page_dot4_set_visible(ui, page == 4));
    CHECK(gsp_watch_page_dot5_set_visible(ui, page == 5));
    return ESP_GSP_OK;
}
enum { BREATH_DURATION_MS = 60000, BREATH_CYCLE_MS = 10000, BREATH_IN_MS = 4000 };

static unsigned breath_value(const watch_ui_t *s)
{
    if (s->breath_elapsed_ms >= BREATH_DURATION_MS) {
        return 100;
    }
    unsigned phase = s->breath_elapsed_ms % BREATH_CYCLE_MS;
    return phase < BREATH_IN_MS ? phase * 100 / BREATH_IN_MS :
           (BREATH_CYCLE_MS - phase) * 100 / (BREATH_CYCLE_MS - BREATH_IN_MS);
}

static esp_gsp_err_t render_breath(esp_gsp_handle_t ui, const watch_ui_t *s)
{
    char count[16], remaining[40];
    bool complete = s->breath_elapsed_ms >= BREATH_DURATION_MS;
    unsigned phase = s->breath_elapsed_ms % BREATH_CYCLE_MS;
    const char *status = complete ? "Complete" : !s->breath_running ? "Ready" :
                         s->breath_paused ? "Paused" : phase < BREATH_IN_MS ? "Inhale" : "Exhale";
    if (!s->breath_running) {
        snprintf(count, sizeof(count), "60");
        snprintf(remaining, sizeof(remaining), "%s", complete ? "A quiet minute, complete." : "4 s in / 6 s out");
    } else {
        unsigned end = phase < BREATH_IN_MS ? BREATH_IN_MS : BREATH_CYCLE_MS;
        snprintf(count, sizeof(count), "%u", (end - phase + 999) / 1000);
        snprintf(remaining, sizeof(remaining), "%u s remaining", (BREATH_DURATION_MS - s->breath_elapsed_ms + 999) / 1000);
    }
    CHECK(gsp_watch_breath_status_set_text(ui, status));
    CHECK(gsp_watch_breath_count_set_text(ui, count));
    CHECK(gsp_watch_breath_remaining_set_text(ui, remaining));
    CHECK(gsp_watch_breath_toggle_set_text(ui, complete ? "Again" : !s->breath_running ? "Start" : s->breath_paused ? "Resume" : "Pause"));
    CHECK(gsp_watch_breath_ring_set_value(ui, breath_value(s)));
    return ESP_GSP_OK;
}

static void sample_breath(watch_ui_t *s)
{
    uint64_t elapsed = 50; /* Deterministic scheduler time in the native runner. */
    if (s->clock_ms) {
        uint64_t now = s->clock_ms();
        elapsed = now >= s->breath_sample_ms ? now - s->breath_sample_ms : 0;
        s->breath_sample_ms = now;
    }
    unsigned left = BREATH_DURATION_MS - s->breath_elapsed_ms;
    s->breath_elapsed_ms += elapsed >= left ? left : (unsigned)elapsed;
    if (s->breath_elapsed_ms == BREATH_DURATION_MS) {
        s->breath_running = s->breath_paused = false;
    }
}

static esp_gsp_err_t advance_breath(esp_gsp_handle_t ui, watch_ui_t *s)
{
    if (!s->breath_running || s->breath_paused || s->page != 5 || s->detail) {
        return ESP_GSP_OK;
    }
    unsigned before = s->breath_elapsed_ms;
    unsigned value = breath_value(s);
    sample_breath(s);
    if (before / 1000 != s->breath_elapsed_ms / 1000) {
        return render_breath(ui, s);
    }
    if (value != breath_value(s)) {
        CHECK(gsp_watch_breath_ring_set_value(ui, breath_value(s)));
    }
    return ESP_GSP_OK;
}

static esp_gsp_err_t apply(esp_gsp_handle_t ui, watch_ui_t *s)
{
    if (!s->detail) {
        s->drawer_seen_open = false;
    }
    CHECK(drawer_state(ui, GSP_WATCH_OBJ_KEY_MESSAGE_PAGE, s->detail == 1));
    CHECK(drawer_state(ui, GSP_WATCH_OBJ_KEY_DETAIL_PAGE, s->detail == 2));
    if (s->breath_running && (s->page != 5 || s->detail)) {
        s->breath_paused = true;
    }
    CHECK(render_breath(ui, s));
    CHECK(gsp_watch_charge_status_set_text(ui, s->battery < 100 ? "CHARGING" : "READY TO EXPLORE"));
    CHECK(gsp_watch_unread_set_text(ui, s->replied ? "REPLY SENT" : "1 NEW MESSAGE"));
    return ESP_GSP_OK;
}
static const struct {
    const char *name, *title, *distance, *time, *stats;
} routes[] = {
    {"Ridge walk", "RIDGE WALK", "4.8 km", "32 min", "32 MIN   /   286 KCAL"},
    {"Coast walk", "COAST WALK", "3.2 km", "24 min", "24 MIN   /   192 KCAL"},
    {"Forest walk", "FOREST WALK", "6.1 km", "48 min", "48 MIN   /   364 KCAL"}
};

static esp_gsp_err_t route_selection(esp_gsp_handle_t ui, unsigned route)
{
    if (route >= sizeof(routes) / sizeof(routes[0])) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    CHECK(gsp_watch_trail_title_set_text(ui, routes[route].name));
    CHECK(gsp_watch_trail_distance_set_text(ui, routes[route].distance));
    CHECK(gsp_watch_trail_time_set_text(ui, routes[route].time));
    CHECK(gsp_watch_trail_ridge_set_visible(ui, route == 0));
    CHECK(gsp_watch_trail_coast_set_visible(ui, route == 1));
    CHECK(gsp_watch_trail_forest_set_visible(ui, route == 2));
    const gsp_component_key_t choices[] = {
        GSP_WATCH_OBJ_KEY_ROUTE_SELECT0,
        GSP_WATCH_OBJ_KEY_ROUTE_SELECT1,
        GSP_WATCH_OBJ_KEY_ROUTE_SELECT2
    };
    for (unsigned i = 0; i < sizeof(choices) / sizeof(choices[0]); ++i) {
        CHECK(esp_gsp_component_set_color_rgb888(ui, choices[i],
                route == i ? 0x3B5546 : 0x16261F));
    }
    return ESP_GSP_OK;
}

static esp_gsp_err_t route_summary(esp_gsp_handle_t ui, unsigned route)
{
    if (route >= sizeof(routes) / sizeof(routes[0])) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    CHECK(gsp_watch_route_title_set_text(ui, routes[route].title));
    CHECK(gsp_watch_route_distance_set_text(ui, routes[route].distance));
    CHECK(gsp_watch_route_stats_set_text(ui, routes[route].stats));
    CHECK(esp_gsp_component_set_visible(ui, GSP_WATCH_OBJ_KEY_ROUTE_RIDGE, route == 0));
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
    } else if (gsp_watch_event_is_route_ridge(e) ||
               gsp_watch_event_is_route_coast(e) || gsp_watch_event_is_route_forest(e)) {
        unsigned selected = gsp_watch_event_is_route_ridge(e) ? 0 :
                            gsp_watch_event_is_route_coast(e) ? 1 : 2;
        s->last_error = route_selection(ui, selected);
        if (s->last_error == ESP_GSP_OK) {
            s->trail = selected;
        }
        return;
    } else if (gsp_watch_event_is_breath_pause(e)) {
        if (!s->breath_running) {
            s->breath_elapsed_ms = 0;
            s->breath_running = true;
            s->breath_paused = false;
        } else {
            if (!s->breath_paused && s->clock_ms) {
                sample_breath(s);
            }
            if (s->breath_running) {
                s->breath_paused = !s->breath_paused;
            }
        }
        if (s->clock_ms) {
            s->breath_sample_ms = s->clock_ms();
        }
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
        CHECK(gsp_watch_battery_ring_set_value(ui, s->battery));
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
    s->last_error = advance_breath(ui, s);
}
static void tick(esp_gsp_handle_t ui, void *ctx)
{
    watch_ui_t *s = ctx;
    if (s->last_error == ESP_GSP_OK) {
        s->last_error = advance(ui, s);
    }
}
esp_gsp_err_t watch_ui_init(esp_gsp_handle_t ui, watch_ui_t *s, watch_clock_ms_t clock_ms)
{
    if (!ui || !s) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *s = (watch_ui_t) {
        .battery = 74, .clock_ms = clock_ms
    };
    CHECK(apply(ui, s));
    CHECK(route_selection(ui, s->trail));
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
