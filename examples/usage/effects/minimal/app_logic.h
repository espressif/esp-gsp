/* SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#pragma once
#include <stdio.h>
#include "esp_gsp.h"
/* Include the generated bundle header before this application helper. */

#define QUICK_EFFECTS_CHECK(call) do { esp_gsp_err_t ret_ = (call); if (ret_ != ESP_GSP_OK) { return ret_; } } while (0)

/* Call when battery state changes; the label remains ordinary application UI. */
static inline esp_gsp_err_t quick_effects_update(esp_gsp_handle_t ui, unsigned percent, bool charging)
{
    if (percent > 100) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    char text[5];
    snprintf(text, sizeof(text), "%u%%", percent);
    QUICK_EFFECTS_CHECK(gsp_quick_effects_charge_set_value(ui, percent));
    QUICK_EFFECTS_CHECK(gsp_quick_effects_level_set_text(ui, text));
    return gsp_quick_effects_charge_set_charging(ui, charging);
}

/* RGB values are portable across scene color profiles. */
static inline esp_gsp_err_t quick_effects_set_color(esp_gsp_handle_t ui, uint32_t rgb888)
{
    QUICK_EFFECTS_CHECK(gsp_quick_effects_charge_set_fg_color_rgb888(ui, rgb888));
    return gsp_quick_effects_feedback_set_fg_color_rgb888(ui, rgb888);
}

/* 0 is the original pair; 1 is the compiled teal pair. */
static inline esp_gsp_err_t quick_effects_set_images(esp_gsp_handle_t ui, unsigned group)
{
    return gsp_quick_effects_card_set_image_set(ui, group);
}

/* Stop charging motion while retaining the level and static halo. */
static inline esp_gsp_err_t quick_effects_stop(esp_gsp_handle_t ui)
{
    return gsp_quick_effects_charge_set_charging(ui, false);
}

/* Call once after creating the UI and activating this scene. */
static inline esp_gsp_err_t quick_effects_init(esp_gsp_handle_t ui)
{
    QUICK_EFFECTS_CHECK(quick_effects_set_color(ui, 0x5BCFF5));
    QUICK_EFFECTS_CHECK(quick_effects_set_images(ui, 0));
    return quick_effects_update(ui, 62, true);
}
#undef QUICK_EFFECTS_CHECK
