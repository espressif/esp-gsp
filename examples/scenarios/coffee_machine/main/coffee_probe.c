/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "bundle_gsp.h"
#include "esp_gsp_debug.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void *timer;
static unsigned step;
static void probe(esp_gsp_handle_t ui, void *context)
{
    (void)context;
    uint32_t frames, errors;
    uint64_t busy;
    gsp_err_t last;
    esp_gsp_render_stats(ui, &frames, &busy);
    esp_gsp_render_error_stats(ui, &errors, &last);
    ESP_LOGI("coffee_probe", "step=%u frames=%lu errors=%lu last=%d stack_remaining=%u",
             step, (unsigned long)frames, (unsigned long)errors, last,
             (unsigned)uxTaskGetStackHighWaterMark(NULL));
    esp_gsp_err_t result;
    if (step % 4 == 0) {
        result = esp_gsp_page_flow_set_page(ui, GSP_COFFEE_OBJ_KEY_JOURNEY, 3, true);
    } else if (step % 4 == 3) {
        result = esp_gsp_page_flow_set_page(ui, GSP_COFFEE_OBJ_KEY_JOURNEY, 0, true);
    } else {
        /* Exercise the catalog through the same input and application path
         * as a customer; never change only its visual selection. */
        int16_t x = (int16_t)(148 + (step % 3) * 252);
        result = esp_gsp_inject_touch(ui, x, 270, true);
        if (result == ESP_GSP_OK) {
            result = esp_gsp_inject_touch(ui, x, 270, false);
        }
    }
    ESP_LOGI("coffee_probe", "step=%u result=%d", step, result);
    if (++step == 12) {
        (void)esp_gsp_timer_delete(ui, timer);
        ESP_LOGI("coffee_probe", "complete");
    }
}
void coffee_probe_start(esp_gsp_handle_t ui)
{
    timer = esp_gsp_timer_create(ui, 2500, probe, NULL);
    if (timer == NULL) {
        ESP_LOGE("coffee_probe", "timer allocation failed");
    }
}
