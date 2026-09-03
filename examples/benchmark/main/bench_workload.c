/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "bench_workload.h"

#include <string.h>

#define GSP_BUNDLE_ENABLE_LEGACY_NAMES 1
#ifndef BENCH_BUNDLE_HEADER
#define BENCH_BUNDLE_HEADER "bundle_gsp.h"
#endif
#include BENCH_BUNDLE_HEADER

static uint32_t native_color(const bench_workload_t *workload,
                             uint32_t rgb888, uint16_t rgb565)
{
    return workload->rgb888 ? rgb888 : rgb565;
}

static bool page_visible(esp_gsp_handle_t ui, uint16_t bind)
{
    bool visible = false;
    return esp_gsp_get_visible(ui, bind, &visible) == ESP_GSP_OK && visible;
}

void bench_workload_init(bench_workload_t *workload, bool rgb888)
{
    if (workload != NULL) {
        memset(workload, 0, sizeof(*workload));
        workload->rgb888 = rgb888;
    }
}

void bench_workload_drive_tweens(esp_gsp_handle_t ui, void *user_ctx)
{
    bench_workload_t *workload = user_ctx;
    if (ui == NULL || workload == NULL) {
        return;
    }
    uint32_t tick = ++workload->tween_tick;
    bool up = (tick & 1U) != 0;
    static const uint16_t values[] = {
        GSP_BIND_V0, GSP_BIND_V1, GSP_BIND_V2, GSP_BIND_V3,
        GSP_BIND_V4, GSP_BIND_V5, GSP_BIND_V6, GSP_BIND_V7,
    };
    static const uint16_t arcs[] = {
        GSP_BIND_A0, GSP_BIND_A1, GSP_BIND_A2,
        GSP_BIND_A3, GSP_BIND_A4, GSP_BIND_A5,
    };

    if (page_visible(ui, GSP_BIND_P_RECT1)) {
        (void)esp_gsp_animate(ui, GSP_BIND_RV0, ESP_GSP_ANIM_CURRENT,
                              up ? 100 : 0, 900, ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_RECTS)) {
        for (uint8_t index = 0; index < 8; ++index) {
            (void)esp_gsp_animate(ui, values[index], ESP_GSP_ANIM_CURRENT,
                                  ((tick + index) & 1U) ? 100 : 0, 900,
                                  ESP_GSP_EASE_IN_OUT);
        }
    } else if (page_visible(ui, GSP_BIND_P_ARCS)) {
        for (uint8_t index = 0; index < 6; ++index) {
            (void)esp_gsp_animate(ui, arcs[index], ESP_GSP_ANIM_CURRENT,
                                  ((tick + index) & 1U) ? 100 : 0, 900,
                                  ESP_GSP_EASE_OUT);
        }
    } else if (page_visible(ui, GSP_BIND_P_SHAPES)) {
        uint32_t violet = native_color(workload, 0x420042U, 0x4008U);
        uint32_t blue = native_color(workload, 0x004184U, 0x0210U);
        (void)gsp_bench_theme_animate_shape_accent(
            ui, up ? violet : blue, up ? blue : violet, 900,
            ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_MOTION)) {
        uint32_t violet = native_color(workload, 0x420042U, 0x4008U);
        uint32_t blue = native_color(workload, 0x004184U, 0x0210U);
        (void)esp_gsp_animate_color(ui, GSP_BIND_WALL,
                                    up ? violet : blue,
                                    up ? blue : violet, 900,
                                    ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_OVER) ||
               page_visible(ui, GSP_BIND_P_BIGTEXT)) {
        uint16_t bind = page_visible(ui, GSP_BIND_P_OVER)
                        ? GSP_BIND_WALL2 : GSP_BIND_WALL3;
        uint32_t violet = native_color(workload, 0x290042U, 0x2808U);
        uint32_t blue = native_color(workload, 0x082484U, 0x0930U);
        (void)esp_gsp_animate_color(ui, bind, up ? violet : blue,
                                    up ? blue : violet, 900,
                                    ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_WALL)) {
        uint32_t violet = native_color(workload, 0x420042U, 0x4008U);
        uint32_t blue = native_color(workload, 0x004184U, 0x0210U);
        (void)esp_gsp_animate_color(ui, GSP_BIND_WALL4,
                                    up ? violet : blue,
                                    up ? blue : violet, 900,
                                    ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_CARDS)) {
        static const uint16_t binds[] = {
            GSP_BIND_CC0, GSP_BIND_CC1, GSP_BIND_CC2,
            GSP_BIND_CC3, GSP_BIND_CC4, GSP_BIND_CC5,
        };
        uint32_t dark = native_color(workload, 0x29284AU, 0x2949U);
        uint32_t light = native_color(workload, 0x948239U, 0x9407U);
        for (uint8_t index = 0; index < 6; ++index) {
            bool phase = ((tick + index) & 1U) != 0;
            (void)esp_gsp_animate_color(ui, binds[index],
                                        phase ? dark : light,
                                        phase ? light : dark, 900,
                                        ESP_GSP_EASE_IN_OUT);
        }
    } else if (page_visible(ui, GSP_BIND_P_OPA)) {
        static const uint16_t binds[] = {
            GSP_BIND_OC0, GSP_BIND_OC1, GSP_BIND_OC2,
            GSP_BIND_OC3, GSP_BIND_OC4, GSP_BIND_OC5,
        };
        uint32_t blue = native_color(workload, 0x6392BDU, 0x6497U);
        uint32_t red = native_color(workload, 0xE75D5AU, 0xE2EBU);
        for (uint8_t index = 0; index < 6; ++index) {
            bool phase = ((tick + index) & 1U) != 0;
            (void)esp_gsp_animate_color(ui, binds[index],
                                        phase ? blue : red,
                                        phase ? red : blue, 900,
                                        ESP_GSP_EASE_IN_OUT);
        }
    } else if (page_visible(ui, GSP_BIND_P_OPAL)) {
        static const uint16_t binds[] = {
            GSP_BIND_LC0, GSP_BIND_LC1, GSP_BIND_LC2,
        };
        uint32_t blue = native_color(workload, 0x4241C6U, 0x4218U);
        uint32_t dark = native_color(workload, 0x181831U, 0x18C6U);
        for (uint8_t index = 0; index < 3; ++index) {
            bool phase = ((tick + index) & 1U) != 0;
            (void)esp_gsp_animate_color(ui, binds[index],
                                        phase ? blue : dark,
                                        phase ? dark : blue, 900,
                                        ESP_GSP_EASE_IN_OUT);
        }
    }
}
