/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_mosaico_power.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32S31 && (CONFIG_EXAMPLE_LCD_INTERFACE_QSPI_CO5300 || GSP_BENCH_BOARD_S31_MOSAICO)

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_check.h"
#include "esp_log.h"
#include "soc/gpio_reg.h"
#include "soc/soc.h"

/* Power-only subset of esp-mosaico-bsp/onboard/esp_mosaico.c (Apache-2.0).
 * GPIO57 is the power-button net: release it, never generate a falling edge.
 * Both supported board mappings use these same power-control pins. */
#define MOSAICO_VCC_GPIO       GPIO_NUM_60
#define MOSAICO_SWITCH_GPIO    GPIO_NUM_57
#define MOSAICO_VCC_ON         0
#define MOSAICO_VCC_OFF        1
#define MOSAICO_SWITCH_RELEASE 1
#define MOSAICO_RAMP_MODE       LEDC_LOW_SPEED_MODE
#define MOSAICO_RAMP_TIMER      LEDC_TIMER_1
#define MOSAICO_RAMP_CHANNEL    LEDC_CHANNEL_1
#define MOSAICO_RAMP_RES        LEDC_TIMER_8_BIT
#define MOSAICO_RAMP_FREQ_HZ    100000
#define MOSAICO_RAMP_MS         50

static const char *TAG = "bench_mosaico_power";
static bool s_initialized;

static esp_err_t configure_output(gpio_num_t pin, gpio_mode_t mode, int level)
{
    const gpio_config_t config = {
        .pin_bit_mask = BIT64(pin),
        .mode = mode,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_set_level(pin, level), TAG, "preset GPIO%d failed", pin);
    return gpio_config(&config);
}

static int configured_vcc_level(void)
{
    const uint32_t bit = BIT(MOSAICO_VCC_GPIO - 32);
    if ((REG_READ(GPIO_ENABLE1_REG) & bit) == 0) {
        return -1;
    }
    return (REG_READ(GPIO_OUT1_REG) & bit) != 0;
}

static esp_err_t ramp_vcc_on(void)
{
    const ledc_timer_config_t timer = {
        .speed_mode = MOSAICO_RAMP_MODE,
        .duty_resolution = MOSAICO_RAMP_RES,
        .timer_num = MOSAICO_RAMP_TIMER,
        .freq_hz = MOSAICO_RAMP_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "configure power ramp timer failed");

    const ledc_channel_config_t channel = {
        .gpio_num = MOSAICO_VCC_GPIO,
        .speed_mode = MOSAICO_RAMP_MODE,
        .channel = MOSAICO_RAMP_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = MOSAICO_RAMP_TIMER,
        .duty = (1U << MOSAICO_RAMP_RES) - 1U,
        .hpoint = 0,
    };
    esp_err_t ret = ledc_channel_config(&channel);
    if (ret != ESP_OK) {
        esp_err_t restore_ret = configure_output(MOSAICO_VCC_GPIO, GPIO_MODE_OUTPUT, MOSAICO_VCC_OFF);
        ESP_RETURN_ON_ERROR(restore_ret, TAG, "restore disabled power rail failed");
        return ret;
    }

    const esp_err_t installed = ledc_fade_func_install(0);
    ret = installed == ESP_ERR_INVALID_STATE ? ESP_OK : installed;
    if (ret == ESP_OK) {
        ret = ledc_set_fade_with_time(MOSAICO_RAMP_MODE, MOSAICO_RAMP_CHANNEL, 0, MOSAICO_RAMP_MS);
    }
    if (ret == ESP_OK) {
        ret = ledc_fade_start(MOSAICO_RAMP_MODE, MOSAICO_RAMP_CHANNEL, LEDC_FADE_WAIT_DONE);
    }
    if (installed == ESP_OK) {
        ledc_fade_func_uninstall();
    }

    const esp_err_t stop_ret = ledc_stop(MOSAICO_RAMP_MODE, MOSAICO_RAMP_CHANNEL,
                                         ret == ESP_OK ? MOSAICO_VCC_ON : MOSAICO_VCC_OFF);
    if (ret == ESP_OK) {
        ret = stop_ret;
    }
    /* Return the pad to GPIO; a failed ramp must leave the rail off. */
    const esp_err_t restore_ret = configure_output(MOSAICO_VCC_GPIO, GPIO_MODE_OUTPUT,
                                  ret == ESP_OK ? MOSAICO_VCC_ON : MOSAICO_VCC_OFF);
    ESP_RETURN_ON_ERROR(restore_ret, TAG, "restore power GPIO failed");
    return ret;
}

esp_err_t bench_mosaico_power_init(void)
{
    if (s_initialized) {
        return ESP_OK;
    }
    int vcc_level = configured_vcc_level();
    const bool adopted = vcc_level >= 0;
    if (!adopted) {
        vcc_level = MOSAICO_VCC_OFF;
    }
    ESP_RETURN_ON_ERROR(configure_output(MOSAICO_VCC_GPIO, GPIO_MODE_OUTPUT, vcc_level), TAG,
                        "configure power rail failed");
    ESP_RETURN_ON_ERROR(configure_output(MOSAICO_SWITCH_GPIO, GPIO_MODE_OUTPUT_OD,
                                         MOSAICO_SWITCH_RELEASE), TAG, "release power switch failed");
    ESP_RETURN_ON_ERROR(gpio_hold_dis(MOSAICO_VCC_GPIO), TAG, "release power rail hold failed");
    ESP_RETURN_ON_ERROR(gpio_hold_dis(MOSAICO_SWITCH_GPIO), TAG, "release power switch hold failed");
    if (vcc_level != MOSAICO_VCC_ON) {
        ESP_RETURN_ON_ERROR(ramp_vcc_on(), TAG, "power rail soft-start failed");
    }
    s_initialized = true;
    ESP_LOGI(TAG, "GPIO57 released; VCC_PW GPIO60=0 (%s)",
             adopted && vcc_level == MOSAICO_VCC_ON ? "already on" : "50 ms soft-start");
    return ESP_OK;
}

#else

esp_err_t bench_mosaico_power_init(void)
{
    return ESP_OK;
}

#endif
