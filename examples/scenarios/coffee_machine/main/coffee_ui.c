/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "coffee_ui.h"
#include "bundle_gsp.h"
#include <stdio.h>
#include <string.h>

static const char *const drinks[] = {"Espresso", "Latte", "Cappuccino"};
static const char *const notes[] = {
    "Rich crema. A bold little ritual.", "Silky milk. A mellow espresso heart.",
    "Velvety foam. Balanced espresso."
};
static const char *const profiles[] = {
    "DARK / INTENSE", "SILKY / CARAMEL", "BALANCED / VELVET"
};
static const char *const strengths[] = {"Mild", "Balanced", "Strong"};

static void select_drink(coffee_ui_t *s, unsigned drink)
{
    s->drink = drink;
    s->amount = drink == 0 ? 40 : drink == 1 ? 240 : 180;
}

static void record(coffee_ui_t *s, esp_gsp_err_t rc)
{
    if (rc != ESP_GSP_OK && s->last_error == ESP_GSP_OK) {
        s->last_error = rc;
    }
}
#define SET(name, value) record(s, gsp_coffee_##name##_set_text(ui, value))
#define VIS(name, value) record(s, gsp_coffee_##name##_set_visible(ui, value))

static void page(esp_gsp_handle_t ui, coffee_ui_t *s, unsigned target)
{
    record(s, esp_gsp_page_flow_set_page(ui, GSP_COFFEE_OBJ_KEY_JOURNEY, target, true));
}

static void motion(esp_gsp_handle_t ui, coffee_ui_t *s, bool playing)
{
    if (s->motion_active == playing) {
        return;
    }
    s->motion_active = playing;
    record(s, gsp_coffee_brew_ring_set_playing(ui, playing));
    record(s, gsp_coffee_extraction_set_playing(ui, playing));
    record(s, gsp_coffee_steam_set_playing(ui, playing));
}

static void set_recipe_enabled(esp_gsp_handle_t ui, coffee_ui_t *s, bool enabled)
{
    /* PageFlow remains available while brewing, but all recipe mutations are
     * disabled as well as rejected in event(). This keeps the busy lock
     * visible and prevents a drag from changing a queued recipe. */
    record(s, esp_gsp_component_set_enabled(ui, GSP_COFFEE_OBJ_KEY_DRINK_CAROUSEL, enabled));
    record(s, esp_gsp_component_set_enabled(ui, GSP_COFFEE_OBJ_KEY_ESPRESSO, enabled));
    record(s, esp_gsp_component_set_enabled(ui, GSP_COFFEE_OBJ_KEY_LATTE, enabled));
    record(s, esp_gsp_component_set_enabled(ui, GSP_COFFEE_OBJ_KEY_CAPPUCCINO, enabled));
    record(s, esp_gsp_component_set_enabled(ui, GSP_COFFEE_OBJ_KEY_AMOUNT_SLIDER, enabled));
    record(s, esp_gsp_component_set_enabled(ui, GSP_COFFEE_OBJ_KEY_STRENGTH_SLIDER, enabled));
    record(s, esp_gsp_component_set_enabled(ui, GSP_COFFEE_OBJ_KEY_TEMPERATURE_SLIDER, enabled));
}

static void render(esp_gsp_handle_t ui, coffee_ui_t *s)
{
    char value[80];
    VIS(brew_espresso, s->drink == 0);
    VIS(brew_latte, s->drink == 1);
    VIS(brew_cup, s->drink == 2);
    VIS(hero0, s->drink == 0);
    VIS(hero1, s->drink == 1);
    VIS(hero2, s->drink == 2);
    VIS(selected0, s->drink == 0);
    VIS(selected1, s->drink == 1);
    VIS(selected2, s->drink == 2);
    SET(primary, s->brewing ? "View preparation" : "Prepare coffee");
    SET(drink, drinks[s->drink]);
    SET(description, notes[s->drink]);
    snprintf(value, sizeof(value), "%u ml", s->amount);
    SET(amount, value);
    SET(strength, strengths[s->strength]);
    snprintf(value, sizeof(value), "%u C", s->temperature);
    SET(temperature, value);
    SET(water, s->water ? "Water tank: FULL" : "Water empty - tap to refill");
    SET(beans, s->beans ? "Bean hopper: FULL" : "Beans empty - tap to refill");
    SET(supply_note, (!s->water || !s->beans) ?
        "Refill both supplies before preparing coffee." :
        "Keep your water and beans topped up.");
    SET(explore_drink, drinks[s->drink]);
    SET(explore_note, profiles[s->drink]);
    snprintf(value, sizeof(value), "%u ml  /  %s", s->amount, strengths[s->strength]);
    SET(explore_meta, value);
    record(s, gsp_coffee_drink_carousel_set_value(ui, (int32_t)s->drink));
    record(s, gsp_coffee_amount_slider_set_value(ui, s->amount));
    record(s, gsp_coffee_strength_slider_set_value(ui, s->strength));
    record(s, gsp_coffee_temperature_slider_set_value(ui, s->temperature));
    VIS(modal, s->supplies);
    /* Keep full-page navigation available during preparation. The controller
     * rejects edits, but the user can return to the active cup and cancel. */
    record(s, gsp_coffee_journey_set_enabled(ui, !s->supplies));
    record(s, gsp_coffee_supply_panel_set_enabled(ui, s->supplies));
    set_recipe_enabled(ui, s, !s->brewing);
    VIS(brew_ring, s->brewing || s->done);
    VIS(extraction, s->brewing);
    VIS(steam, s->brewing);
    motion(ui, s, s->brewing);
    SET(brew_action, s->brewing ? "Cancel preparation" :
        s->done ? "Enjoy another moment" : "Return to recipe");
}

static void progress(esp_gsp_handle_t ui, coffee_ui_t *s)
{
    char value[80];
    unsigned duration = s->drink == 0 ? 18 : 24;
    unsigned percent = s->elapsed * 100 / duration;
    record(s, gsp_coffee_progress_set_value(ui, percent));
    record(s, gsp_coffee_brew_ring_set_value(ui, percent));
    snprintf(value, sizeof(value), "%u%%", percent);
    SET(percent, s->done ? "ENJOY" : s->brewing ? value : "READY");
    SET(status_title, s->done ? "Your moment is ready" :
        s->brewing ? "Making your moment" : "Ready when you are");
    SET(stage, s->done ? "Perfectly prepared, just for you" :
        !s->brewing ? "Personalise your recipe first" :
        s->elapsed < 4 ? "01 / Grinding fresh beans" :
        s->elapsed < 8 ? "02 / Warming to perfection" :
        s->elapsed < 18 ? "03 / Extracting espresso" :
        "04 / Finishing with silky foam");
    snprintf(value, sizeof(value), "%s / %u ml / %u C",
             drinks[s->drink], s->amount, s->temperature);
    SET(progress_caption, value);
    SET(status_hint, s->done ? "Take a breath. Savour the first sip." :
        s->brewing ? "Please keep your cup in place." :
        "Swipe right to return to your recipe.");
}

static void navigation_tick(esp_gsp_handle_t ui, void *ctx)
{
    coffee_ui_t *s = ctx;
    /* Drawer gestures use the same close state as the explicit Close button. */
    if (s->supplies) {
        esp_gsp_component_motion_t state;
        esp_gsp_err_t result = esp_gsp_component_get_motion(ui, GSP_COFFEE_OBJ_KEY_SUPPLY_PANEL, &state);
        record(s, result);
        if (result == ESP_GSP_OK && !state.value && !state.target &&
                !state.dragging && !state.settling) {
            s->supplies = false;
            render(ui, s);
        }
    }
    record(s, esp_gsp_page_flow_get_page(ui, GSP_COFFEE_OBJ_KEY_JOURNEY, &s->page));
    motion(ui, s, s->brewing && s->page == 2);
}

static void tick(esp_gsp_handle_t ui, void *ctx)
{
    coffee_ui_t *s = ctx;
    if (!s->brewing) {
        return;
    }
    if (++s->elapsed >= (s->drink == 0 ? 18U : 24U)) {
        s->brewing = false;
        s->done = true;
        render(ui, s);
    }
    progress(ui, s);
}

static void open_supplies(esp_gsp_handle_t ui, coffee_ui_t *s)
{
    s->supplies = true;
    esp_gsp_component_motion_t state;
    esp_gsp_err_t result = esp_gsp_component_get_motion(ui, GSP_COFFEE_OBJ_KEY_SUPPLY_PANEL, &state);
    record(s, result);
    if (result == ESP_GSP_OK && !state.target) {
        record(s, esp_gsp_drawer_open(ui, GSP_COFFEE_OBJ_KEY_SUPPLY_PANEL,
                                      !state.dragging));
    }
}

static void event(esp_gsp_handle_t ui, const esp_gsp_event_t *ev, void *ctx)
{
    coffee_ui_t *s = ctx;
    gsp_coffee_call_event_t call;
    if (!gsp_coffee_event_decode_call(ev, &call)) {
        return;
    }
    if (s->supplies) {
        if (call.action == GSP_COFFEE_ACTION_WATER) {
            s->water = !s->water;
        } else if (call.action == GSP_COFFEE_ACTION_BEANS) {
            s->beans = !s->beans;
        } else if (call.action == GSP_COFFEE_ACTION_CLOSE_SUPPLIES) {
            s->supplies = false;
            esp_gsp_component_motion_t state;
            esp_gsp_err_t result = esp_gsp_component_get_motion(ui, GSP_COFFEE_OBJ_KEY_SUPPLY_PANEL, &state);
            record(s, result);
            if (result == ESP_GSP_OK && state.target) {
                record(s, esp_gsp_drawer_close(ui, GSP_COFFEE_OBJ_KEY_SUPPLY_PANEL,
                                               !state.dragging));
            }
        }
    } else if (call.action == GSP_COFFEE_ACTION_BREW_ACTION) {
        s->brewing = s->done = false;
        s->elapsed = 0;
        page(ui, s, 1);
        progress(ui, s);
    } else if (s->brewing && call.action == GSP_COFFEE_ACTION_PRIMARY) {
        page(ui, s, 2);
    } else if (!s->brewing) {
        int32_t value = 0;
        switch (call.action) {
        case GSP_COFFEE_ACTION_PRIMARY:
            if (!s->water || !s->beans) {
                open_supplies(ui, s);
            } else {
                s->done = false;
                s->brewing = true;
                s->elapsed = 0;
                page(ui, s, 2);
                progress(ui, s);
            }
            break;
        case GSP_COFFEE_ACTION_SUPPLIES:
            open_supplies(ui, s);
            break;
        case GSP_COFFEE_ACTION_CUSTOMISE:
            page(ui, s, 1);
            break;
        case GSP_COFFEE_ACTION_EXPLORE:
            page(ui, s, 3);
            break;
        case GSP_COFFEE_ACTION_EXPLORE_RECIPE:
            page(ui, s, 1);
            break;
        case GSP_COFFEE_ACTION_EXPLORE_COLLECTION:
            page(ui, s, 0);
            break;
        case GSP_COFFEE_ACTION_BACK:
            page(ui, s, 0);
            break;
        case GSP_COFFEE_ACTION_CAROUSEL_SELECT:
            /* Carousel CALL args are authored; selection is committed state. */
            record(s, gsp_coffee_drink_carousel_get_value(ui, &value));
            if (s->last_error == ESP_GSP_OK && value >= 0 && value < 3) {
                select_drink(s, (unsigned)value);
            }
            break;
        case GSP_COFFEE_ACTION_ESPRESSO:
            select_drink(s, 0);
            break;
        case GSP_COFFEE_ACTION_LATTE:
            select_drink(s, 1);
            break;
        case GSP_COFFEE_ACTION_CAPPUCCINO:
            select_drink(s, 2);
            break;
        case GSP_COFFEE_ACTION_AMOUNT_CHANGE:
            record(s, gsp_coffee_amount_slider_get_value(ui, &value));
            s->amount = value < 20 ? 20 : value > 300 ? 300 : (unsigned)value;
            break;
        case GSP_COFFEE_ACTION_STRENGTH_CHANGE:
            record(s, gsp_coffee_strength_slider_get_value(ui, &value));
            s->strength = value < 0 ? 0 : value > 2 ? 2 : (unsigned)value;
            break;
        case GSP_COFFEE_ACTION_TEMPERATURE_CHANGE:
            record(s, gsp_coffee_temperature_slider_get_value(ui, &value));
            s->temperature = value < 86 ? 86 : value > 96 ? 96 : (unsigned)value;
            break;
        default:
            break;
        }
    }
    if (!s->brewing && (call.action == GSP_COFFEE_ACTION_ESPRESSO ||
                        call.action == GSP_COFFEE_ACTION_LATTE ||
                        call.action == GSP_COFFEE_ACTION_CAPPUCCINO ||
                        call.action == GSP_COFFEE_ACTION_CAROUSEL_SELECT ||
                        call.action == GSP_COFFEE_ACTION_AMOUNT_CHANGE ||
                        call.action == GSP_COFFEE_ACTION_STRENGTH_CHANGE ||
                        call.action == GSP_COFFEE_ACTION_TEMPERATURE_CHANGE)) {
        s->done = false;
        s->elapsed = 0;
    }
    render(ui, s);
    progress(ui, s);
}

esp_gsp_err_t coffee_ui_init(esp_gsp_handle_t ui, coffee_ui_t *s)
{
    if (!ui || !s) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    memset(s, 0, sizeof(*s));
    s->drink = 2;
    s->amount = 180;
    s->strength = 1;
    s->temperature = 92;
    s->water = s->beans = true;
    s->motion_active = true; /* Force authored effects to stop during setup. */
    render(ui, s);
    progress(ui, s);
    record(s, esp_gsp_on_event(ui, event, s));
    if (s->last_error != ESP_GSP_OK) {
        return s->last_error;
    }
    s->timer = esp_gsp_timer_create(ui, 1000, tick, s);
    if (!s->timer) {
        (void)esp_gsp_on_event(ui, NULL, NULL);
        return ESP_GSP_ERR_NO_MEM;
    }
    s->navigation_timer = esp_gsp_timer_create(ui, 50, navigation_tick, s);
    if (!s->navigation_timer) {
        coffee_ui_deinit(ui, s);
        return ESP_GSP_ERR_NO_MEM;
    }
    return ESP_GSP_OK;
}

void coffee_ui_deinit(esp_gsp_handle_t ui, coffee_ui_t *s)
{
    if (!ui || !s) {
        return;
    }
    (void)esp_gsp_on_event(ui, NULL, NULL);
    if (s->timer) {
        (void)esp_gsp_timer_delete(ui, s->timer);
    }
    if (s->navigation_timer) {
        (void)esp_gsp_timer_delete(ui, s->navigation_timer);
    }
    s->timer = s->navigation_timer = NULL;
}
