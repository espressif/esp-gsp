/* SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#pragma once
#include "esp_gsp.h"

enum { EFFECTS_PAGE_COUNT = 9 };

typedef struct {
    unsigned page;
    unsigned style[EFFECTS_PAGE_COUNT];
    bool flipped;
    bool playing;
    gsp_component_key_t pages[EFFECTS_PAGE_COUNT], views[EFFECTS_PAGE_COUNT][3], marks[3], pause;
    esp_gsp_err_t last_error;
} effects_preview_t;

typedef esp_gsp_err_t (*preview_info_fn_t)(esp_gsp_handle_t, esp_gsp_component_info_t *);
typedef esp_gsp_err_t (*preview_play_fn_t)(esp_gsp_handle_t, bool);

#define PREVIEW_CHECK(call) do { esp_gsp_err_t ret_=(call); if(ret_!=ESP_GSP_OK){return ret_;} } while(0)

static esp_gsp_err_t effects_preview_apply(esp_gsp_handle_t ui, effects_preview_t *state)
{
    static const char *const titles[] = {"Charging orb", "Soft ring", "Icon carousel", "Soft pulse", "SVG morph", "Highlight sweep", "Touch glow", "Flip card", "Frosted glass"};
    static const char *const labels[EFFECTS_PAGE_COUNT][3] = {{"Liquid", "Halo", "Plasma"}, {"Solid", "Glow", "Trail"}, {"Flat", "Tilt", "Depth"}, {"Quiet", "Soft", "Bright"}, {"Slow", "Normal", "Fast"}, {"Linear", "Soft", "Slant"}, {"Blue", "Teal", "Rose"}, {"Flat", "Soft", "Depth"}, {"Mist", "Frost", "Smoke"}};
    gsp_component_update_t updates[40]; unsigned count = 0;
    for (unsigned i = 0; i < EFFECTS_PAGE_COUNT; ++i) {
        updates[count++] = (gsp_component_update_t) {
            .key = state->pages[i], .prop = GSP_COMPONENT_PROP_VISIBLE, .value = {.type = GSP_VALUE_BOOL, .data.boolean = i == state->page}
        };
    }
    for (unsigned page = 0; page < EFFECTS_PAGE_COUNT; ++page) {
        for (unsigned i = 0; i < 3; ++i) {
            if (!state->views[page][i]) {
                continue;
            }
            updates[count++] = (gsp_component_update_t) {
                .key = state->views[page][i], .prop = GSP_COMPONENT_PROP_VISIBLE, .value = {.type = GSP_VALUE_BOOL, .data.boolean = i == state->style[page]}
            };
        }
    }
    for (unsigned i = 0; i < 3; ++i) {
        updates[count++] = (gsp_component_update_t) {
            .key = state->marks[i], .prop = GSP_COMPONENT_PROP_VISIBLE, .value = {.type = GSP_VALUE_BOOL, .data.boolean = i == state->style[state->page]}
        };
    }
    /* The carousel follows touch/selection; it has no continuous playback. */
    updates[count++] = (gsp_component_update_t) {
        .key = state->pause, .prop = GSP_COMPONENT_PROP_VISIBLE,
        .value = {.type = GSP_VALUE_BOOL, .data.boolean = state->page != 2 && state->page != 6 && state->page != 8}
    };
    /* One retained instance per page; style buttons update appearance in place. */
    if (state->page == 3) {
        static const uint32_t strengths[] = {45, 70, 95}, periods[] = {4500, 3600, 2800};
        PREVIEW_CHECK(gsp_effects_pulse_set_strength(ui, strengths[state->style[3]]));
        PREVIEW_CHECK(gsp_effects_pulse_set_period_ms(ui, periods[state->style[3]]));
    } else if (state->page == 6) {
        static const uint32_t colors[] = {0x7AACFF, 0x6AE5CE, 0xE9A3DC};
        PREVIEW_CHECK(gsp_effects_feedback_set_fg_color_rgb888(ui, colors[state->style[6]]));
    } else if (state->page == 7) {
        static const uint32_t depths[] = {0, 45, 80};
        PREVIEW_CHECK(gsp_effects_card_set_depth(ui, depths[state->style[7]]));
        PREVIEW_CHECK(gsp_effects_card_set_image_set(ui, state->style[7] == 2 ? 1 : 0));
    }
    PREVIEW_CHECK(esp_gsp_component_set_many(ui, updates, count));
    PREVIEW_CHECK(gsp_effects_title_set_text(ui, titles[state->page]));
    PREVIEW_CHECK(gsp_effects_charge_status_set_text(ui, state->playing ? "CHARGING" : "NOT CHARGING"));
    PREVIEW_CHECK(gsp_effects_pause_set_text(ui, state->page == 7 ? "Flip" : state->playing ? (state->page == 0 ? "Stop" : "Pause") : (state->page == 0 ? "Charge" : "Play")));
    PREVIEW_CHECK(gsp_effects_style_0_set_text(ui, labels[state->page][0]));
    PREVIEW_CHECK(gsp_effects_style_1_set_text(ui, labels[state->page][1]));
    PREVIEW_CHECK(gsp_effects_style_2_set_text(ui, labels[state->page][2]));
    return ESP_GSP_OK;
}

static esp_gsp_err_t effects_preview_play(esp_gsp_handle_t ui, effects_preview_t *state)
{
    static const preview_play_fn_t players[] = {gsp_effects_charge_liquid_set_charging, gsp_effects_charge_halo_set_charging, gsp_effects_charge_plasma_set_charging,
                                                gsp_effects_ring_solid_set_playing, gsp_effects_ring_glow_set_playing, gsp_effects_ring_trail_set_playing,
                                                gsp_effects_pulse_set_playing,
                                                gsp_effects_shine_linear_set_playing, gsp_effects_shine_soft_set_playing, gsp_effects_shine_diagonal_set_playing
                                               };
    for (unsigned i = 0; i < sizeof(players) / sizeof(players[0]); ++i) {
        PREVIEW_CHECK(players[i](ui, state->playing));
    }
    if (state->playing) {
        static const uint32_t durations[] = {1600, 900, 450};
        const esp_gsp_animation_config_t config = {.duration_ms = durations[state->style[4]], .iterations = 0, .alternate = true, .ease = ESP_GSP_EASE_IN_OUT};
        PREVIEW_CHECK(gsp_effects_emblem_play_morph(ui, 0, 100, &config));
    } else {
        PREVIEW_CHECK(gsp_effects_emblem_stop_morph(ui));
    }
    return ESP_GSP_OK;
}

static esp_gsp_err_t effects_preview_flip(esp_gsp_handle_t ui, effects_preview_t *state)
{
    PREVIEW_CHECK(gsp_effects_card_get_flipped(ui, &state->flipped));
    state->flipped = !state->flipped;
    return gsp_effects_card_set_flipped(ui, state->flipped);
}

static void effects_preview_event(esp_gsp_handle_t ui, const esp_gsp_event_t *event, void *ctx)
{
    effects_preview_t *state = ctx;
    if (state->page == 7 && gsp_effects_event_is_pause(event)) {
        state->last_error = effects_preview_flip(ui, state);
        return;
    }
    if (gsp_effects_event_is_pause(event)) {
        state->playing = !state->playing; state->last_error = effects_preview_play(ui, state);
        if (state->last_error == ESP_GSP_OK) {
            state->last_error = effects_preview_apply(ui, state);
        }
        return;
    }
    if (gsp_effects_event_is_next(event)) {
        state->page = (state->page + 1) % EFFECTS_PAGE_COUNT;
    } else if (gsp_effects_event_is_style_0(event)) {
        state->style[state->page] = 0;
    } else if (gsp_effects_event_is_style_1(event)) {
        state->style[state->page] = 1;
    } else if (gsp_effects_event_is_style_2(event)) {
        state->style[state->page] = 2;
    } else {
        return;
    }
    state->last_error = effects_preview_apply(ui, state);
    if (state->last_error == ESP_GSP_OK && state->page == 4) {
        state->last_error = effects_preview_play(ui, state);
    }
}

static esp_gsp_err_t effects_preview_init(esp_gsp_handle_t ui, effects_preview_t *state)
{
    static const preview_info_fn_t page_info[] = {gsp_effects_charge_page_get_info, gsp_effects_ring_page_get_info, gsp_effects_orbit_page_get_info, gsp_effects_pulse_page_get_info, gsp_effects_morph_page_get_info, gsp_effects_shimmer_page_get_info, gsp_effects_feedback_page_get_info, gsp_effects_flip_page_get_info, gsp_effects_glass_page_get_info};
    static const preview_info_fn_t view_info[EFFECTS_PAGE_COUNT][3] = {
        {gsp_effects_charge_liquid_view_get_info, gsp_effects_charge_halo_view_get_info, gsp_effects_charge_plasma_view_get_info},
        {gsp_effects_ring_solid_view_get_info, gsp_effects_ring_glow_view_get_info, gsp_effects_ring_trail_view_get_info},
        {gsp_effects_orbit_flat_view_get_info, gsp_effects_orbit_tilt_view_get_info, gsp_effects_orbit_perspective_view_get_info},
        {NULL, NULL, NULL},
        {NULL, NULL, NULL},
        {gsp_effects_shimmer_linear_view_get_info, gsp_effects_shimmer_soft_view_get_info, gsp_effects_shimmer_diagonal_view_get_info},
        {NULL, NULL, NULL},
        {NULL, NULL, NULL},
        {gsp_effects_glass_mist_view_get_info, gsp_effects_glass_frost_view_get_info, gsp_effects_glass_smoke_view_get_info}
    };
    static const preview_info_fn_t mark_info[] = {gsp_effects_mark_0_get_info, gsp_effects_mark_1_get_info, gsp_effects_mark_2_get_info};
    *state = (effects_preview_t) {
        .playing = true, .style = {1, 2, 2, 1, 1, 1, 0, 1, 1}
    };
    esp_gsp_component_info_t info;
    PREVIEW_CHECK(gsp_effects_pause_view_get_info(ui, &info));
    state->pause = info.key;
    for (unsigned i = 0; i < EFFECTS_PAGE_COUNT; ++i) {
        PREVIEW_CHECK(page_info[i](ui, &info));
        state->pages[i] = info.key;
    }
    for (unsigned p = 0; p < EFFECTS_PAGE_COUNT; ++p) {
        for (unsigned i = 0; i < 3; ++i) {
            if (!view_info[p][i]) {
                continue;
            }
            PREVIEW_CHECK(view_info[p][i](ui, &info));
            state->views[p][i] = info.key;
        }
    }
    for (unsigned i = 0; i < 3; ++i) {
        PREVIEW_CHECK(mark_info[i](ui, &info));
        state->marks[i] = info.key;
    }
    PREVIEW_CHECK(effects_preview_apply(ui, state));
    PREVIEW_CHECK(effects_preview_play(ui, state));
    return esp_gsp_on_event(ui, effects_preview_event, state);
}
#undef PREVIEW_CHECK
