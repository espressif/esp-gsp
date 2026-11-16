/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "hw_init.h"

esp_err_t hw_touch_init(esp_lcd_touch_handle_t *out_touch)
{
    if (out_touch == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *out_touch = NULL;
    return ESP_ERR_NOT_SUPPORTED;
}
