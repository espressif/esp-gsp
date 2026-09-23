/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "knob_input.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"

/* Defaults match ESP32-C3-LCDkit BSP_ENCODER_{A,B,PRESS}. No GSP calls
 * from the sampling task: the render-context timer owns all UI mutations. */
static QueueHandle_t queue;
static void *consumer_timer;
static const int pin_a = CONFIG_THERMOSTAT_ENCODER_A;
static const int pin_b = CONFIG_THERMOSTAT_ENCODER_B;
static const int pin_press = CONFIG_THERMOSTAT_ENCODER_PRESS;
static void sample(void *arg)
{
    (void)arg;
    static const int8_t transitions[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
    int previous = (gpio_get_level(pin_a) << 1) | gpio_get_level(pin_b), accum = 0;
    int stable = gpio_get_level(pin_press), candidate = stable;
    int64_t changed = esp_timer_get_time(), pressed = changed;
    bool long_sent = false;
    for (;;) {
        int current = (gpio_get_level(pin_a) << 1) | gpio_get_level(pin_b);
        if ((previous ^ current) == 3) {
            accum = 0;    /* Reject missed/invalid transitions. */
        } else {
            accum += transitions[(previous << 2) | current];
        }
        previous = current;
        if (accum >= 4 || accum <= -4) {
            bool increase = accum > 0;
#ifdef CONFIG_THERMOSTAT_ENCODER_REVERSE
            increase = !increase;
#endif
            thermostat_action_t action = increase ? THERMOSTAT_INCREASE : THERMOSTAT_DECREASE;
            (void)xQueueSend(queue, &action, 0); /* Bounded queue: excess rotation coalesces by dropping. */
            accum = 0;
        }
        int raw = gpio_get_level(pin_press);
        int64_t now = esp_timer_get_time();
        if (raw != candidate) {
            candidate = raw;
            changed = now;
        }
        if (candidate != stable && now - changed >= 20000) {
            stable = candidate;
            if (!stable) {
                pressed = now;
                long_sent = false;
            } else if (!long_sent) {
                thermostat_action_t action = THERMOSTAT_CONFIRM;
                (void)xQueueSend(queue, &action, 0);
            }
        }
        if (!stable && !long_sent && now - pressed >= 800000) {
            thermostat_action_t action = THERMOSTAT_BACK;
            (void)xQueueSend(queue, &action, 0);
            long_sent = true;
        }
        TickType_t poll_ticks = pdMS_TO_TICKS(2);
        vTaskDelay(poll_ticks ? poll_ticks : 1);
    }
}
static void consume(esp_gsp_handle_t ui, void *ctx)
{
    thermostat_action_t action;
    for (int n = 0; n < 16 && xQueueReceive(queue, &action, 0) == pdTRUE; ++n) {
        thermostat_ui_action(ui, ctx, action);
    }
}
esp_err_t knob_input_start(esp_gsp_handle_t ui, thermostat_ui_t *state)
{
    if (queue || !ui || !state) {
        return ESP_ERR_INVALID_STATE;
    }
    if (pin_a == pin_b || pin_a == pin_press || pin_b == pin_press) {
        return ESP_ERR_INVALID_ARG;
    }
    gpio_config_t config = {.pin_bit_mask = (1ULL << pin_a) | (1ULL << pin_b) | (1ULL << pin_press),
                            .mode = GPIO_MODE_INPUT, .pull_up_en = GPIO_PULLUP_ENABLE, .intr_type = GPIO_INTR_DISABLE
                           };
    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) {
        return err;
    }
    queue = xQueueCreate(16, sizeof(thermostat_action_t));
    if (!queue) {
        return ESP_ERR_NO_MEM;
    }
    consumer_timer = esp_gsp_timer_create(ui, 20, consume, state);
    if (!consumer_timer) {
        vQueueDelete(queue);
        queue = NULL;
        return ESP_ERR_NO_MEM;
    }
    if (xTaskCreate(sample, "thermostat_knob", 2048, NULL, 4, NULL) != pdPASS) {
        /* Deletion stops scheduling, but an already selected consumer may
         * still access the queue. Keep it alive until the UI task retires. */
        esp_gsp_err_t cleanup = esp_gsp_timer_delete(ui, consumer_timer);
        if (cleanup != ESP_GSP_OK) {
            return cleanup;
        }
        consumer_timer = NULL;
        cleanup = esp_gsp_flush(ui, 1000);
        if (cleanup != ESP_GSP_OK) {
            return cleanup; /* Queue remains valid for an in-flight callback. */
        }
        vQueueDelete(queue);
        queue = NULL;
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
