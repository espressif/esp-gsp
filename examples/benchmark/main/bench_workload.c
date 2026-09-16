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

int bench_workload_vector_index(uint16_t bind)
{
    const uint16_t pages[] = {GSP_BIND_P_VECTOR_SIZE, GSP_BIND_P_VECTOR_ROTATE, GSP_BIND_P_VECTOR_TINT,
                              GSP_BIND_P_VECTOR_MORPH, GSP_BIND_P_VECTOR_MOVE, GSP_BIND_P_VECTOR_FIT, GSP_BIND_P_VECTOR_STYLE, GSP_BIND_P_VECTOR_EYES
                             };
    for (unsigned i = 0; i < sizeof(pages) / sizeof(pages[0]); ++i) {
        if (pages[i] == bind) {
            return (int)i;
        }
    }
    return -1;
}

static esp_err_t property_range(esp_gsp_handle_t ui, gsp_component_key_t object,
                                gsp_property_key_t property, int32_t *center, int32_t *extent)
{
    gsp_property_info_t info;
    esp_err_t ret = esp_gsp_component_get_property_info(ui, object, property, &info);
    if (ret == ESP_OK) {
        *center = (int32_t)((info.min_value + info.max_value) / 2);
        *extent = (int32_t)((info.max_value - info.min_value) / 2);
    }
    return ret;
}

void bench_workload_drive_eyes(esp_gsp_handle_t ui, void *user_ctx)
{
    bench_workload_t *workload = user_ctx;
    if (ui == NULL || workload == NULL || !page_visible(ui, GSP_BIND_P_VECTOR_EYES)) {
        return;
    }
    bench_vector_stats_t *stats = &workload->vectors[7];
    if (stats->updates == 0) {
        const gsp_component_key_t keys[] = {GSP_OBJ_KEY_EYE_IRIS_LEFT, GSP_OBJ_KEY_EYE_IRIS_RIGHT};
        for (unsigned i = 0; i < 2; ++i) {
            stats->errors += property_range(ui, keys[i], GSP_PROP_KEY_X,
                                            &workload->eye_center_x[i], &workload->eye_extent_x[i]) != ESP_OK;
            stats->errors += property_range(ui, keys[i], GSP_PROP_KEY_Y,
                                            &workload->eye_center_y[i], &workload->eye_extent_y[i]) != ESP_OK;
        }
    }
    unsigned phase = stats->updates++ % 32;
    int32_t observed = -1;
    stats->errors += gsp_bench_eye_mask_left_get_morph(ui, &observed) != ESP_OK;
    workload->eyes_closed += observed == 100;
    workload->eyes_open += observed == 0;
    int32_t closed = (phase == 4 || phase == 5 || phase == 18 || phase == 19) ? 100 : 0;
    uint32_t duration = closed ? 80 : 180;
    static const int32_t gaze[][2] = {{-100, -25}, {100, -10}, {35, 75}, {0, -65}};
    const int32_t *look = gaze[phase / 8];
    esp_err_t results[8];
    unsigned count = 0;
    if (phase == 0 || phase == 4 || phase == 6 || phase == 18 || phase == 20) {
        results[count++] = gsp_bench_eye_mask_left_animate_morph_to(ui, closed, duration, ESP_GSP_EASE_IN_OUT);
        results[count++] = gsp_bench_eye_mask_right_animate_morph_to(ui, closed, duration, ESP_GSP_EASE_IN_OUT);
        results[count++] = gsp_bench_eye_rim_left_animate_morph_to(ui, closed, duration, ESP_GSP_EASE_IN_OUT);
        results[count++] = gsp_bench_eye_rim_right_animate_morph_to(ui, closed, duration, ESP_GSP_EASE_IN_OUT);
    }
    if (phase % 8 == 0) {
        results[count++] = gsp_bench_eye_iris_left_animate_x_to(ui, workload->eye_center_x[0] + look[0] * workload->eye_extent_x[0] / 100, 180, ESP_GSP_EASE_IN_OUT);
        results[count++] = gsp_bench_eye_iris_right_animate_x_to(ui, workload->eye_center_x[1] + look[0] * workload->eye_extent_x[1] / 100, 180, ESP_GSP_EASE_IN_OUT);
        results[count++] = gsp_bench_eye_iris_left_animate_y_to(ui, workload->eye_center_y[0] + look[1] * workload->eye_extent_y[0] / 100, 180, ESP_GSP_EASE_IN_OUT);
        results[count++] = gsp_bench_eye_iris_right_animate_y_to(ui, workload->eye_center_y[1] + look[1] * workload->eye_extent_y[1] / 100, 180, ESP_GSP_EASE_IN_OUT);
    }
    stats->commands += count;
    for (unsigned i = 0; i < count; ++i) {
        stats->errors += results[i] != ESP_OK;
    }
}

void bench_workload_init(bench_workload_t *workload, bool rgb888)
{
    if (workload != NULL) {
        memset(workload, 0, sizeof(*workload));
        workload->rgb888 = rgb888;
        atomic_init(&workload->effects_published, 0);
        atomic_init(&workload->effects_failed, 0);
    }
}

static void effect_image_complete(esp_gsp_handle_t ui, uint16_t bind,
                                  gsp_err_t status, void *ctx)
{
    (void)ui; (void)bind;
    bench_workload_t *workload = ctx;
    if (status == GSP_OK) {
        atomic_fetch_add(&workload->effects_published, 1);
    } else if (status != GSP_ERR_CANCELLED) {
        atomic_fetch_add(&workload->effects_failed, 1);
    }
}

static void drive_effects(esp_gsp_handle_t ui, bench_workload_t *workload)
{
    bench_vector_stats_t *stats = &workload->effects;
    uint32_t tick = stats->updates++;
    /* Immutable encoded sources outlive asynchronous image publication. */
    /* 32x32 matches the baked placeholder even with the image cache disabled.
     * First pixel + sixteen 62-pixel runs + one 31-pixel run = 1024 pixels. */
    static const uint8_t images[2][43] = {
        {0x71, 0x6f, 0x69, 0x66, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x03, 0x00, 0xfe, 0x30, 0x60, 0xc0, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xde, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
        {0x71, 0x6f, 0x69, 0x66, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x03, 0x00, 0xfe, 0xc0, 0x60, 0x30, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xfd, 0xde, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
    };
    const esp_gsp_image_options_t options = {.on_complete = effect_image_complete, .complete_ctx = workload};
    esp_gsp_err_t results[] = {
        gsp_bench_bench_effect_orb_set_value(ui, tick % 101),
        gsp_bench_bench_effect_pulse_set_opacity(ui, (tick * 17U) & 255U),
        gsp_bench_bench_effect_flip_set_flipped(ui, (tick / 32U) % 2 != 0),
        gsp_bench_bench_effect_carousel_set_selected(ui, (tick / 24U) % 3),
        esp_gsp_set_image_ex(ui, GSP_BIND_EFFECT_DYNAMIC, images[tick % 2], sizeof(images[0]), &options),
    };
    stats->commands += sizeof(results) / sizeof(results[0]);
    for (unsigned i = 0; i < sizeof(results) / sizeof(results[0]); ++i) {
        stats->errors += results[i] != ESP_GSP_OK;
    }
}

void bench_workload_drive_vectors(esp_gsp_handle_t ui, void *user_ctx)
{
    bench_workload_t *workload = user_ctx;
    if (ui == NULL || workload == NULL) {
        return;
    }
    if (page_visible(ui, GSP_BIND_P_EFFECTS)) {
        drive_effects(ui, workload);
        return;
    }
    const uint16_t pages[] = {GSP_BIND_P_VECTOR_SIZE, GSP_BIND_P_VECTOR_ROTATE, GSP_BIND_P_VECTOR_TINT, GSP_BIND_P_VECTOR_MORPH,
                              GSP_BIND_P_VECTOR_MOVE, GSP_BIND_P_VECTOR_FIT, GSP_BIND_P_VECTOR_STYLE
                             };
    for (unsigned page = 0; page < 7; ++page) {
        if (!page_visible(ui, pages[page])) {
            continue;
        }
        bench_vector_stats_t *stats = &workload->vectors[page];
        uint32_t tick = stats->updates++;
        esp_err_t results[4];
        unsigned count = 2;
        if (page == 0) {
            unsigned phase = tick % 128;
            int32_t size = 32 + (phase <= 64 ? phase : 128 - phase);
            results[0] = gsp_bench_vector_size_color_set_width(ui, size);
            results[1] = gsp_bench_vector_size_color_set_height(ui, size);
            results[2] = gsp_bench_vector_size_mask_set_width(ui, 128 - size);
            results[3] = gsp_bench_vector_size_mask_set_height(ui, 128 - size);
            count = 4;
        } else if (page == 1) {
            int32_t angle = (int32_t)((tick % 120) * 3);
            results[0] = gsp_bench_vector_rotate_color_set_rotation(ui, angle);
            results[1] = gsp_bench_vector_rotate_mask_set_rotation(ui, -angle);
        } else if (page == 2) {
            uint32_t rgb = ((tick * 7U) & 255U) << 16 | ((tick * 3U) & 255U) << 8 | 0x60U;
            uint16_t rgb565 = (uint16_t)(((rgb >> 19) & 31U) << 11 |
                                         ((rgb >> 10) & 63U) << 5 | ((rgb >> 3) & 31U));
            uint32_t color = native_color(workload, rgb, rgb565);
            uint32_t inverse = color ^ (workload->rgb888 ? 0xffffffU : 0xffffU);
            results[0] = gsp_bench_vector_tint_a_set_tint(ui, color);
            results[1] = gsp_bench_vector_tint_b_set_tint(ui, inverse);
        } else if (page == 3) {
            unsigned phase = tick % 200;
            int32_t progress = phase <= 100 ? phase : 200 - phase;
            results[0] = gsp_bench_vector_morph_color_set_morph(ui, progress);
            results[1] = gsp_bench_vector_morph_mask_set_morph(ui, 100 - progress);
        } else if (page == 4) {
            if (tick == 0) {
                int32_t extent;
                stats->errors += property_range(ui, GSP_OBJ_KEY_VECTOR_MOVE_COLOR, GSP_PROP_KEY_X, &workload->move_center[0], &extent) != ESP_OK;
                stats->errors += property_range(ui, GSP_OBJ_KEY_VECTOR_MOVE_MASK, GSP_PROP_KEY_X, &workload->move_center[1], &extent) != ESP_OK;
            }
            unsigned phase = tick % 32;
            int32_t offset = (int32_t)(phase <= 16 ? phase : 32 - phase) - 8;
            results[0] = gsp_bench_vector_move_color_set_x(ui, workload->move_center[0] + offset);
            results[1] = gsp_bench_vector_move_mask_set_x(ui, workload->move_center[1] - offset);
        } else if (page == 5) {
            unsigned phase = tick % 120;
            uint32_t scale = 49152 + (phase <= 60 ? phase : 120 - phase) * 49152 / 60;
            results[0] = gsp_bench_vector_fit_contain_set_scale_q16(ui, scale);
            results[1] = gsp_bench_vector_fit_cover_set_scale_q16(ui, scale);
            results[2] = gsp_bench_vector_fit_stretch_set_scale_q16(ui, scale);
            count = 3;
        } else {
            results[0] = gsp_bench_vector_paints_mask_set_visible(ui, tick % 80 < 64);
            count = 1;
        }
        stats->commands += count;
        for (unsigned i = 0; i < count; ++i) {
            stats->errors += results[i] != ESP_OK;
        }
        return;
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
                              up ? 100 : 0, 1000, ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_RECTS)) {
        for (uint8_t index = 0; index < 8; ++index) {
            (void)esp_gsp_animate(ui, values[index], ESP_GSP_ANIM_CURRENT,
                                  ((tick + index) & 1U) ? 100 : 0, 1000,
                                  ESP_GSP_EASE_IN_OUT);
        }
    } else if (page_visible(ui, GSP_BIND_P_ARCS)) {
        for (uint8_t index = 0; index < 6; ++index) {
            (void)esp_gsp_animate(ui, arcs[index], ESP_GSP_ANIM_CURRENT,
                                  ((tick + index) & 1U) ? 100 : 0, 1000,
                                  ESP_GSP_EASE_OUT);
        }
    } else if (page_visible(ui, GSP_BIND_P_SHAPES)) {
        uint32_t violet = native_color(workload, 0x420042U, 0x4008U);
        uint32_t blue = native_color(workload, 0x004184U, 0x0210U);
        (void)gsp_bench_theme_animate_shape_accent(
            ui, up ? violet : blue, up ? blue : violet, 1000,
            ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_MOTION)) {
        uint32_t violet = native_color(workload, 0x420042U, 0x4008U);
        uint32_t blue = native_color(workload, 0x004184U, 0x0210U);
        (void)esp_gsp_animate_color(ui, GSP_BIND_WALL,
                                    up ? violet : blue,
                                    up ? blue : violet, 1000,
                                    ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_OVER) ||
               page_visible(ui, GSP_BIND_P_BIGTEXT)) {
        uint16_t bind = page_visible(ui, GSP_BIND_P_OVER)
                        ? GSP_BIND_WALL2 : GSP_BIND_WALL3;
        uint32_t violet = native_color(workload, 0x290042U, 0x2808U);
        uint32_t blue = native_color(workload, 0x082484U, 0x0930U);
        (void)esp_gsp_animate_color(ui, bind, up ? violet : blue,
                                    up ? blue : violet, 1000,
                                    ESP_GSP_EASE_IN_OUT);
    } else if (page_visible(ui, GSP_BIND_P_WALL)) {
        uint32_t violet = native_color(workload, 0x420042U, 0x4008U);
        uint32_t blue = native_color(workload, 0x004184U, 0x0210U);
        (void)esp_gsp_animate_color(ui, GSP_BIND_WALL4,
                                    up ? violet : blue,
                                    up ? blue : violet, 1000,
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
                                        phase ? light : dark, 1000,
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
                                        phase ? red : blue, 1000,
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
                                        phase ? dark : blue, 1000,
                                        ESP_GSP_EASE_IN_OUT);
        }
    }
}

unsigned bench_workload_message_actions(bench_message_clock_t *clock, uint32_t elapsed_ms,
                                        int32_t extent)
{
    unsigned actions = BENCH_MESSAGES_SCROLL;
    if (!clock->prepended && elapsed_ms >= 750U) {
        clock->prepended = true;
        actions |= BENCH_MESSAGES_PREPEND;
    }
    if (!clock->appended && elapsed_ms >= 2250U) {
        clock->appended = true;
        actions |= BENCH_MESSAGES_APPEND;
    }
    clock->step = (clock->step + 1U) % 8U;
    unsigned distance = clock->step <= 4U ? clock->step : 8U - clock->step;
    clock->offset = extent - (int32_t)((int64_t)extent * distance / 4);
    if (clock->step > 4U || clock->step == 0U) {
        actions |= BENCH_MESSAGES_DOWN;
    }
    return actions;
}
