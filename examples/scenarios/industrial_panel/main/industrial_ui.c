/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "industrial_ui.h"
#include "bundle_gsp.h"
#include <stdio.h>

#define CHECK(call) do { esp_gsp_err_t rc = (call); if (rc != ESP_GSP_OK) return rc; } while (0)

enum {
    TREND_PLOT_LEFT = 60,
    TREND_PLOT_TOP = 232,
    TREND_PLOT_WIDTH = 598,
    TREND_PLOT_HEIGHT = 166,
};

/* A fully dismissed settings drawer must not claim a fresh edge gesture.
 * Keep it enabled during close animation so its panel never turns gray. */
static esp_gsp_err_t drawer_visibility(esp_gsp_handle_t ui, bool desired)
{
    const gsp_component_key_t key = GSP_INDUSTRIAL_OBJ_KEY_SETTINGS_PAGE;
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

/* A closed alerts drawer stays visible and enabled for native edge capture.
 * Settings keeps exclusive ownership until its closing motion has finished. */
static esp_gsp_err_t alerts_availability(esp_gsp_handle_t ui, bool editing)
{
    bool settings_visible, enabled;
    CHECK(esp_gsp_component_get_visible(ui, GSP_INDUSTRIAL_OBJ_KEY_SETTINGS_PAGE, &settings_visible));
    CHECK(esp_gsp_component_get_enabled(ui, GSP_INDUSTRIAL_OBJ_KEY_ALERTS_PAGE, &enabled));
    bool desired = !editing && !settings_visible;
    if (enabled != desired) {
        CHECK(esp_gsp_component_set_enabled(ui, GSP_INDUSTRIAL_OBJ_KEY_ALERTS_PAGE, desired));
    }
    return ESP_GSP_OK;
}
static esp_gsp_err_t status(esp_gsp_handle_t ui, industrial_ui_t *s)
{
    if (s->alerts_active) {
        char text[48];
        snprintf(text, sizeof(text), "%d.%d bar", s->pressure / 10, s->pressure % 10);
        CHECK(gsp_industrial_alert_pressure_set_text(ui, text));
        snprintf(text, sizeof(text), "TARGET  %d.%d bar", s->target / 10, s->target % 10);
        CHECK(gsp_industrial_alert_target_set_text(ui, text));
        CHECK(gsp_industrial_alert_state_set_text(ui, s->alarm ?
                (s->acknowledged ? "High pressure / Acknowledged" : "High pressure / Action required") :
                "Normal / No active alarm"));
        CHECK(gsp_industrial_alert_ack_set_enabled(ui, s->alarm && !s->acknowledged));
    }
    CHECK(gsp_industrial_status_set_text(ui, s->alarm ? (s->acknowledged ? "HIGH PRESSURE / ACKNOWLEDGED" : "HIGH PRESSURE / Review alert") : "RUNNING / All systems nominal"));
    CHECK(gsp_industrial_alarm_text_set_text(ui, s->alarm ? (s->acknowledged ? "High pressure\nAcknowledged" : "High pressure\nAction required") : "No active alarm"));
    CHECK(gsp_industrial_alarm_detail_set_text(ui, s->alarm ? "Pressure exceeds 7.2 bar.\nReview the regulator setpoint." : "Pressure is within the operating band.\nRaise target above 7.2 bar to test an alert."));
    return ESP_GSP_OK;
}

static esp_gsp_err_t trend_inspection(esp_gsp_handle_t ui, industrial_ui_t *s)
{
    char text[48];
    unsigned selected = s->selected_sample;
    if (selected >= INDUSTRIAL_TREND_SAMPLES) {
        selected = INDUSTRIAL_TREND_SAMPLES - 1;
        s->selected_sample = selected;
    }
    int value = s->history[selected];
    unsigned age = INDUSTRIAL_TREND_SAMPLES - 1 - selected;
    int x = TREND_PLOT_LEFT +
            ((int)selected * (TREND_PLOT_WIDTH - 1) +
             (INDUSTRIAL_TREND_SAMPLES - 2) / 2) /
            (INDUSTRIAL_TREND_SAMPLES - 1);
    int y = TREND_PLOT_TOP +
            ((100 - value) * TREND_PLOT_HEIGHT + 50) / 100;

    snprintf(text, sizeof(text), "%d.%d bar", value / 10, value % 10);
    CHECK(gsp_industrial_trend_readout_set_text(ui, text));
    snprintf(text, sizeof(text), "T-%02u s  /  SAMPLE %02u OF %02u",
             age, selected + 1, INDUSTRIAL_TREND_SAMPLES);
    CHECK(gsp_industrial_trend_meta_set_text(ui, text));
    snprintf(text, sizeof(text), "LIVE TARGET  %d.%d bar",
             s->target / 10, s->target % 10);
    CHECK(gsp_industrial_trend_target_set_text(ui, text));
    snprintf(text, sizeof(text), age == 0 ? "NOW" : "T-%02u s", age);
    CHECK(gsp_industrial_trend_scrub_value_set_text(ui, text));
    CHECK(gsp_industrial_trend_scrubber_set_value(ui, (int32_t)selected));
    CHECK(esp_gsp_component_set_position(ui, GSP_INDUSTRIAL_OBJ_KEY_TREND_CURSOR,
                                         x - 1, TREND_PLOT_TOP));
    CHECK(esp_gsp_component_set_position(ui, GSP_INDUSTRIAL_OBJ_KEY_TREND_MARKER,
                                         x - 8, y - 8));
    return ESP_GSP_OK;
}

static esp_gsp_err_t apply(esp_gsp_handle_t ui, industrial_ui_t *s)
{
    char text[40];
    if (!s->editing) {
        s->drawer_seen_open = false;
    }
    CHECK(drawer_visibility(ui, s->editing));
    CHECK(alerts_availability(ui, s->editing));
    esp_gsp_component_motion_t motion;
    CHECK(esp_gsp_component_get_motion(ui, GSP_INDUSTRIAL_OBJ_KEY_SETTINGS_PAGE, &motion));
    if (motion.target != s->editing) {
        bool animated = !motion.dragging;
        CHECK(s->editing ? esp_gsp_drawer_open(ui, GSP_INDUSTRIAL_OBJ_KEY_SETTINGS_PAGE, animated) : esp_gsp_drawer_close(ui, GSP_INDUSTRIAL_OBJ_KEY_SETTINGS_PAGE, animated));
    }
    bool uncovered = !s->editing && !s->alerts_active;
    CHECK(gsp_industrial_scan_set_visible(ui, s->page == 1 && uncovered));
    snprintf(text, sizeof(text), "TARGET  %d.%d bar", s->target / 10, s->target % 10);
    CHECK(gsp_industrial_target_set_text(ui, text));
    snprintf(text, sizeof(text), "%d.%d bar", s->draft / 10, s->draft % 10);
    CHECK(gsp_industrial_draft_set_text(ui, text));
    CHECK(gsp_industrial_draft_slider_set_value(ui, s->draft));
    CHECK(gsp_industrial_draft_hint_set_text(ui, s->draft > 72 ?
            "Above alarm threshold / Apply to commit" : "Preview only / Apply to commit"));
    CHECK(gsp_industrial_live_0_set_playing(ui, s->page == 0 && uncovered));
    CHECK(gsp_industrial_live_1_set_playing(ui, s->page == 1 && uncovered));
    CHECK(gsp_industrial_live_2_set_playing(ui, s->page == 2 && uncovered));
    CHECK(gsp_industrial_live_3_set_playing(ui, s->page == 3 && uncovered));
    CHECK(gsp_industrial_flow_153_set_playing(ui, s->page == 1 && uncovered));
    CHECK(gsp_industrial_flow_365_set_playing(ui, s->page == 1 && uncovered));
    CHECK(gsp_industrial_flow_581_set_playing(ui, s->page == 1 && uncovered));
    CHECK(gsp_industrial_health_ring_set_playing(ui, s->page == 2 && uncovered));
    if (s->page == 3 && uncovered) {
        CHECK(trend_inspection(ui, s));
    }
    return status(ui, s);
}
static void event(esp_gsp_handle_t ui, const esp_gsp_event_t *e, void *ctx)
{
    industrial_ui_t *s = ctx;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    if (gsp_industrial_event_is_alert_close(e)) {
        s->last_error = esp_gsp_drawer_close(ui, GSP_INDUSTRIAL_OBJ_KEY_ALERTS_PAGE, true);
        return;
    }
    esp_gsp_component_motion_t alerts;
    s->last_error = esp_gsp_component_get_motion(ui, GSP_INDUSTRIAL_OBJ_KEY_ALERTS_PAGE, &alerts);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    if (alerts.value || alerts.target || alerts.dragging || alerts.settling) {
        if (gsp_industrial_event_is_ack(e)) {
            if (s->alarm) {
                s->acknowledged = true;
            }
            s->last_error = status(ui, s);
        }
        return;
    }
    if (s->editing) {
        if (gsp_industrial_event_is_plus(e)) {
            if (s->draft < 80) {
                ++s->draft;
            }
        } else if (gsp_industrial_event_is_minus(e)) {
            if (s->draft > 40) {
                --s->draft;
            }
        } else if (gsp_industrial_event_is_draft_change(e)) {
            int32_t value;
            s->last_error = gsp_industrial_draft_slider_get_value(ui, &value);
            if (s->last_error != ESP_GSP_OK) {
                return;
            }
            s->draft = value < 40 ? 40 : value > 80 ? 80 : value;
        } else if (gsp_industrial_event_is_apply(e)) {
            s->target = s->draft;
            s->editing = false;
        } else if (gsp_industrial_event_is_cancel(e)) {
            s->editing = false;
        } else {
            return;
        }
    } else if (gsp_industrial_event_is_trend_select(e)) {
        int32_t value;
        s->last_error = gsp_industrial_trend_scrubber_get_value(ui, &value);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
        s->selected_sample = value < 0 ? 0 :
                             value >= INDUSTRIAL_TREND_SAMPLES ?
                             INDUSTRIAL_TREND_SAMPLES - 1 : (unsigned)value;
        s->last_error = trend_inspection(ui, s);
        return;
    } else if (gsp_industrial_event_is_settings(e)) {
        s->draft = s->target;
        s->editing = true;
    } else if (gsp_industrial_event_is_ack(e)) {
        if (s->alarm) {
            s->acknowledged = true;
        }
    } else {
        uint16_t page;
        if (gsp_industrial_event_is_nav_0(e)) {
            page = 0;
        } else if (gsp_industrial_event_is_nav_1(e)) {
            page = 1;
        } else if (gsp_industrial_event_is_nav_2(e)) {
            page = 2;
        } else if (gsp_industrial_event_is_nav_3(e)) {
            page = 3;
        } else {
            return;
        }
        s->last_error = esp_gsp_page_flow_set_page(ui, GSP_INDUSTRIAL_OBJ_KEY_SCREENS, page, true);
        return;
    }
    s->last_error = apply(ui, s);
}
static esp_gsp_err_t advance(esp_gsp_handle_t ui, industrial_ui_t *s)
{
    char text[32];
    ++s->seconds;
    int desired = s->target + (int)(s->seconds % 7) - 3;
    if (s->pressure < desired) {
        ++s->pressure;
    } else if (s->pressure > desired) {
        --s->pressure;
    }
    bool before = s->alarm;
    if (s->pressure > 72) {
        s->alarm = true;
    } else if (s->pressure < 70) {
        s->alarm = false;
    }
    if (!before && s->alarm) {
        s->acknowledged = false;
    }
    snprintf(text, sizeof(text), "%d.%d", s->pressure / 10, s->pressure % 10);
    CHECK(gsp_industrial_pressure_set_text(ui, text));
    CHECK(gsp_industrial_pressure_arc_set_value(ui, s->pressure));
    snprintf(text, sizeof(text), "%u", 1460 + s->seconds % 9 * 5);
    CHECK(gsp_industrial_rpm_set_text(ui, text));
    snprintf(text, sizeof(text), "94.%u%%", s->seconds % 7);
    CHECK(gsp_industrial_efficiency_set_text(ui, text));
    CHECK(gsp_industrial_trend_append(ui, s->pressure));
    CHECK(gsp_industrial_trend_inspector_append(ui, s->pressure));
    for (unsigned i = 1; i < INDUSTRIAL_TREND_SAMPLES; ++i) {
        s->history[i - 1] = s->history[i];
    }
    s->history[INDUSTRIAL_TREND_SAMPLES - 1] = s->pressure;
    if (s->selected_sample > 0 && s->selected_sample < INDUSTRIAL_TREND_SAMPLES - 1) {
        --s->selected_sample;
    }
    if (s->page == 3 && !s->editing && !s->alerts_active) {
        CHECK(trend_inspection(ui, s));
    }
    return status(ui, s);
}
static void tick(esp_gsp_handle_t ui, void *ctx)
{
    industrial_ui_t *s = ctx; uint16_t page = 0;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    s->last_error = esp_gsp_page_flow_get_page(ui, GSP_INDUSTRIAL_OBJ_KEY_SCREENS, &page);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    if (page != s->page) {
        s->page = page;
        s->last_error = apply(ui, s);
    }
    bool open = false;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    if (s->editing) {
        s->last_error = esp_gsp_drawer_is_open(ui, GSP_INDUSTRIAL_OBJ_KEY_SETTINGS_PAGE, &open);
        if (s->last_error == ESP_GSP_OK) {
            if (open) {
                s->drawer_seen_open = true;
            } else if (s->drawer_seen_open) {
                s->editing = false;
                s->last_error = apply(ui, s);
            }
        }
    }
    if (s->last_error == ESP_GSP_OK) {
        s->last_error = drawer_visibility(ui, s->editing);
    }
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    s->last_error = alerts_availability(ui, s->editing);
    esp_gsp_component_motion_t alerts;
    if (s->last_error == ESP_GSP_OK) {
        s->last_error = esp_gsp_component_get_motion(ui, GSP_INDUSTRIAL_OBJ_KEY_ALERTS_PAGE, &alerts);
    }
    if (s->last_error == ESP_GSP_OK) {
        bool active = alerts.value || alerts.target || alerts.dragging || alerts.settling;
        s->alerts_open = alerts.value;
        s->alerts_dragging = alerts.dragging;
        if (active != s->alerts_active) {
            s->alerts_active = active;
            s->last_error = apply(ui, s);
        }
    }
}
static void model_tick(esp_gsp_handle_t ui, void *ctx)
{
    industrial_ui_t *s = ctx;
    if (s->last_error == ESP_GSP_OK) {
        s->last_error = advance(ui, s);
    }
}
esp_gsp_err_t industrial_ui_init(esp_gsp_handle_t ui, industrial_ui_t *s)
{
    if (!ui || !s) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *s = (industrial_ui_t) {
        .target = 65, .draft = 65, .pressure = 62,
        .selected_sample = INDUSTRIAL_TREND_SAMPLES - 1,
    };
    for (unsigned i = 0; i < INDUSTRIAL_TREND_SAMPLES; ++i) {
        s->history[i] = s->pressure;
    }
    CHECK(apply(ui, s)); CHECK(esp_gsp_on_event(ui, event, s));
    s->timer = esp_gsp_timer_create(ui, 50, tick, s);
    if (!s->timer) {
        (void)esp_gsp_on_event(ui, NULL, NULL);
        return ESP_GSP_ERR_NO_MEM;
    }
    s->model_timer = esp_gsp_timer_create(ui, 1000, model_tick, s);
    if (!s->model_timer) {
        industrial_ui_deinit(ui, s);
        return ESP_GSP_ERR_NO_MEM;
    }
    return ESP_GSP_OK;
}
void industrial_ui_deinit(esp_gsp_handle_t ui, industrial_ui_t *s)
{
    if (!ui || !s) {
        return;
    }
    if (s->model_timer) {
        (void)esp_gsp_timer_delete(ui, s->model_timer);
        s->model_timer = NULL;
    }
    if (s->timer) {
        (void)esp_gsp_timer_delete(ui, s->timer);
        s->timer = NULL;
    }
    (void)esp_gsp_on_event(ui, NULL, NULL);
}
