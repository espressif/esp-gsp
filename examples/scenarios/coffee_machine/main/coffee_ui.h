/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#pragma once
#include "esp_gsp.h"

typedef struct {
    void *timer;
    void *navigation_timer;
    uint16_t page;
    unsigned drink, amount, strength, temperature, elapsed;
    bool brewing, done, water, beans, supplies, pending_brew;
    bool showcase;
    unsigned showcase_step, showcase_seconds;
    esp_gsp_err_t last_error;
} coffee_ui_t;
esp_gsp_err_t coffee_ui_init(esp_gsp_handle_t ui, coffee_ui_t *state);
void coffee_ui_deinit(esp_gsp_handle_t ui, coffee_ui_t *state);
void coffee_ui_set_showcase(coffee_ui_t *state, bool enabled);
