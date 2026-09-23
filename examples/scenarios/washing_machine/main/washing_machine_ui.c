/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "washing_machine_ui.h"
#include "bundle_gsp.h"
#include <stdio.h>
#define CHECK(call) do { esp_gsp_err_t e = (call); if (e != ESP_GSP_OK) return e; } while (0)
/* A simulated minute lasts one second. No real motor or interlock is driven. */
static const char *programs[] = {"Cotton", "Daily", "Delicates", "Quick 15"};
static const char *taglines[] = {"EVERYDAY ESSENTIALS", "MIXED & COLOURED", "A GENTLER TOUCH", "FRESH IN A MOMENT"};
static const char *loads[] = {"6 KG LOAD", "6 KG LOAD", "2 KG LOAD", "6 KG LOAD"};
static const int duration[] = {60, 45, 30, 15};
static const int default_temp[] = {40, 30, 20, 30};
static const int default_rpm[] = {1000, 800, 600, 800};
static esp_gsp_err_t update_indicator(esp_gsp_handle_t ui, unsigned page)
{
    char text[16];
    snprintf(text, sizeof(text), "%u / 4", page + 1);
    return gsp_washing_machine_indicator_set_text(ui, text);
}
static esp_gsp_err_t refresh(esp_gsp_handle_t ui, washing_machine_ui_t *s)
{
    char text[48];
    int progress = s->elapsed * 100 / duration[s->program];
    const char *phase = progress < 50 ? "WASH" : progress < 80 ? "RINSE" : "SPIN";
    const char *cycle_detail = s->done ? "COMPLETE" : s->paused ? "PAUSED" : s->running ? phase : "READY";
    const char *lock = s->done ? "UNLOCKED / READY TO UNLOAD" : s->running ? (s->paused ? "LOCKED / PAUSED SAFELY" : "LOCKED / IN MOTION") : "SETTINGS OPEN / READY";
    if (progress > 100) {
        progress = 100;
    }
    CHECK(gsp_washing_machine_program_gallery_set_selected(ui, (uint32_t)s->program));
    CHECK(esp_gsp_component_set_enabled(ui, GSP_WASHING_MACHINE_OBJ_KEY_PROGRAM_GALLERY, !s->running && !s->done));
    CHECK(gsp_washing_machine_program_set_text(ui, programs[s->program]));
    CHECK(gsp_washing_machine_tagline_set_text(ui, taglines[s->program]));
    snprintf(text, sizeof(text), "%02d / 04", s->program + 1); CHECK(gsp_washing_machine_program_count_set_text(ui, text));
    snprintf(text, sizeof(text), "%d min / %s", duration[s->program], loads[s->program]);
    CHECK(gsp_washing_machine_duration_set_text(ui, text));
    snprintf(text, sizeof(text), "%s / %d C / %d rpm", loads[s->program], s->temperature, s->rpm); CHECK(gsp_washing_machine_program_note_set_text(ui, text));
    snprintf(text, sizeof(text), "%d C", s->temperature); CHECK(gsp_washing_machine_temperature_set_text(ui, text));
    snprintf(text, sizeof(text), "%d rpm", s->rpm); CHECK(gsp_washing_machine_rpm_set_text(ui, text));
    CHECK(gsp_washing_machine_temperature_arc_set_value(ui, s->temperature));
    CHECK(gsp_washing_machine_spin_arc_set_value(ui, s->rpm));
    CHECK(gsp_washing_machine_program_pulse_set_playing(ui, !s->effects_covered && s->page == 0 && !s->running && !s->done));
    CHECK(gsp_washing_machine_settings_pulse_set_playing(ui, !s->effects_covered && s->page == 1));
    CHECK(gsp_washing_machine_settings_hint_set_text(ui, s->running ? "Settings locked during cycle" : "Swipe to start your cycle"));
    CHECK(gsp_washing_machine_status_set_text(ui, s->running ? (s->paused ? "Paused / door locked" : "Running / door locked") : s->door_open ? "Close door to start" : s->done ? "Clean / ready to unload" : "Door closed"));
    CHECK(gsp_washing_machine_badge_set_text(ui, s->done ? "COMPLETE" : s->paused ? "PAUSED" : s->running ? "LOCKED" : "READY"));
    CHECK(gsp_washing_machine_remaining_set_text(ui, s->done ? "FINISHED" : !s->running ? "READY" : phase));
    snprintf(text, sizeof(text), "%02d", duration[s->program] - s->elapsed); CHECK(gsp_washing_machine_phase_set_text(ui, text));
    snprintf(text, sizeof(text), "%s / %d%%", s->done ? "COMPLETE" : !s->running ? "READY" : phase, progress); CHECK(gsp_washing_machine_cycle_detail_set_text(ui, text));
    CHECK(gsp_washing_machine_start_set_text(ui, s->done ? "New load" : s->running ? (s->paused ? "Resume" : "Pause") : "Start"));
    CHECK(gsp_washing_machine_door_set_text(ui, s->running ? "Locked" : s->door_open ? "Close door" : "Open door"));
    CHECK(gsp_washing_machine_cancel_set_text(ui, s->running ? "Cancel" : "Reset"));
    CHECK(gsp_washing_machine_cycle_progress_set_value(ui, progress));
    CHECK(gsp_washing_machine_cycle_ring_set_value(ui, progress));
    CHECK(gsp_washing_machine_water_set_value(ui, s->done ? 0 : s->running && progress >= 80 ? 8 : 42));
    CHECK(gsp_washing_machine_water_set_charging(ui, !s->effects_covered && s->running && !s->paused && s->page == 2));
    CHECK(gsp_washing_machine_cycle_spark_set_playing(ui, !s->effects_covered && s->running && !s->paused && s->page == 2));
    snprintf(text, sizeof(text), "%s / %s", programs[s->program], cycle_detail); CHECK(gsp_washing_machine_detail_program_set_text(ui, text));
    snprintf(text, sizeof(text), "%02d%% COMPLETE", progress); CHECK(gsp_washing_machine_detail_progress_set_text(ui, text));
    snprintf(text, sizeof(text), "%02d MIN LEFT", duration[s->program] - s->elapsed); CHECK(gsp_washing_machine_detail_eta_set_text(ui, s->done ? "READY TO UNLOAD" : text));
    CHECK(gsp_washing_machine_detail_lock_set_text(ui, lock));
    CHECK(gsp_washing_machine_detail_ring_set_value(ui, progress));
    CHECK(gsp_washing_machine_detail_pulse_set_playing(ui, !s->effects_covered && s->running && !s->paused && s->page == 3));
    CHECK(gsp_washing_machine_quick_program_set_text(ui, programs[s->program]));
    snprintf(text, sizeof(text), "%02d MIN LEFT / %s", duration[s->program] - s->elapsed, cycle_detail);
    CHECK(gsp_washing_machine_quick_remaining_set_text(ui, text));
    CHECK(gsp_washing_machine_quick_lock_set_text(ui, s->running ? "DOOR LOCKED / SETTINGS LOCKED" : s->door_open ? "DOOR OPEN / CLOSE TO START" : "DOOR CLOSED / UNLOCKED"));
    CHECK(gsp_washing_machine_quick_start_set_text(ui, s->done ? "New load" : s->running ? (s->paused ? "Resume" : "Pause") : "Start"));
    return ESP_GSP_OK;
}
static void event(esp_gsp_handle_t ui, const esp_gsp_event_t *e, void *ctx)
{
    washing_machine_ui_t *s = ctx;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    if (gsp_washing_machine_event_is_quick_close(e)) {
        s->last_error = esp_gsp_drawer_close(ui, GSP_WASHING_MACHINE_OBJ_KEY_QUICK_PANEL, true);
        return;
    }
    if (gsp_washing_machine_event_is_program_select(e)) {
        uint32_t selected;
        s->last_error = gsp_washing_machine_program_gallery_get_selected(ui, &selected);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
        if (!s->running && !s->done && selected < 4) {
            s->program = (int)selected;
            s->temperature = default_temp[s->program];
            s->rpm = default_rpm[s->program];
        }
    } else if (gsp_washing_machine_event_is_cancel(e)) {
        s->running = s->paused = s->done = false;
        s->elapsed = 0;
    } else if (gsp_washing_machine_event_is_start(e)) {
        if (s->running) {
            s->paused = !s->paused;
        } else if (s->done) {
            s->done = false;
            s->elapsed = 0;
        } else if (!s->door_open) {
            s->running = true;
            s->elapsed = 0;
        }
    } else if (gsp_washing_machine_event_is_details(e)) {
        s->last_error = esp_gsp_page_flow_set_page(ui, GSP_WASHING_MACHINE_OBJ_KEY_PAGES, 3, true);
        return;
    } else if (gsp_washing_machine_event_is_back_cycle(e)) {
        s->last_error = esp_gsp_page_flow_set_page(ui, GSP_WASHING_MACHINE_OBJ_KEY_PAGES, 2, true);
        return;
    } else if (gsp_washing_machine_event_is_door(e) && !s->running) {
        s->door_open = !s->door_open;
    } else if (!s->running && !s->done) {
        if (gsp_washing_machine_event_is_program_next(e)) {
            s->program = (s->program + 1) % 4;
            s->temperature = default_temp[s->program];
            s->rpm = default_rpm[s->program];
        } else if (gsp_washing_machine_event_is_temperature(e)) {
            s->temperature = s->temperature >= 60 ? 20 : s->temperature + 10;
        } else if (gsp_washing_machine_event_is_spin(e)) {
            s->rpm = s->rpm >= 1200 ? 400 : s->rpm + 200;
        }
    }
    s->last_error = refresh(ui, s);
}
static void poll_page(esp_gsp_handle_t ui, void *ctx)
{
    washing_machine_ui_t *s = ctx;
    uint16_t page;
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    s->last_error = esp_gsp_page_flow_get_page(ui, GSP_WASHING_MACHINE_OBJ_KEY_PAGES, &page);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    bool changed = s->page != page;
    if (changed) {
        s->page = page;
        s->last_error = update_indicator(ui, page);
        if (s->last_error != ESP_GSP_OK) {
            return;
        }
    }
    esp_gsp_component_motion_t motion;
    s->last_error = esp_gsp_component_get_motion(ui, GSP_WASHING_MACHINE_OBJ_KEY_QUICK_PANEL, &motion);
    if (s->last_error != ESP_GSP_OK) {
        return;
    }
    bool covered = motion.value || motion.target || motion.dragging || motion.settling;
    changed |= covered != s->effects_covered;
    s->effects_covered = covered;
    if (changed) {
        s->last_error = refresh(ui, s);
    }
}
static void animate_drum(esp_gsp_handle_t ui, void *ctx)
{
    washing_machine_ui_t *s = ctx;
    if (s->last_error != ESP_GSP_OK || s->effects_covered || !s->running || s->paused || (s->page != 2 && s->page != 3)) {
        return;
    }
    unsigned speed = s->elapsed * 100 / duration[s->program] >= 80 ? 36 : 8;
    s->angle = (s->angle + speed) % 360;
    if (s->page == 2) {
        s->last_error = gsp_washing_machine_drum_art_set_rotation(ui, (int)s->angle - 180);
    } else {
        s->last_error = gsp_washing_machine_detail_drum_art_set_rotation(ui, (int)s->angle - 180);
    }
}
static void tick(esp_gsp_handle_t ui, void *ctx)
{
    washing_machine_ui_t *s = ctx;
    if (s->last_error != ESP_GSP_OK || !s->running || s->paused) {
        return;
    }
    if (++s->elapsed >= duration[s->program]) {
        s->running = false;
        s->done = true;
    }
    s->last_error = refresh(ui, s);
}
esp_gsp_err_t washing_machine_ui_init(esp_gsp_handle_t ui, washing_machine_ui_t *s)
{
    if (!ui || !s) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *s = (washing_machine_ui_t) {
        .temperature = 40, .rpm = 1000
    };
    CHECK(refresh(ui, s));
    CHECK(update_indicator(ui, s->page));
    CHECK(esp_gsp_on_event(ui, event, s));
    s->timer = esp_gsp_timer_create(ui, 1000, tick, s);
    if (!s->timer) {
        (void)esp_gsp_on_event(ui, NULL, NULL);
        return ESP_GSP_ERR_NO_MEM;
    }
    s->page_timer = esp_gsp_timer_create(ui, 50, poll_page, s);
    s->motion_timer = esp_gsp_timer_create(ui, 100, animate_drum, s);
    if (!s->page_timer || !s->motion_timer) {
        washing_machine_ui_deinit(ui, s);
        return ESP_GSP_ERR_NO_MEM;
    }
    return ESP_GSP_OK;
}
void washing_machine_ui_deinit(esp_gsp_handle_t ui, washing_machine_ui_t *s)
{
    if (!ui || !s) {
        return;
    }
    if (s->page_timer) {
        (void)esp_gsp_timer_delete(ui, s->page_timer);
        s->page_timer = NULL;
    }
    if (s->motion_timer) {
        (void)esp_gsp_timer_delete(ui, s->motion_timer);
        s->motion_timer = NULL;
    }
    if (s->timer) {
        (void)esp_gsp_timer_delete(ui, s->timer);
        s->timer = NULL;
    }
    (void)esp_gsp_on_event(ui, NULL, NULL);
}
