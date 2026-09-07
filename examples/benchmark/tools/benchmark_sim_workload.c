/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "bench_workload.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GSP_BUNDLE_ENABLE_LEGACY_NAMES 1
#ifndef BENCH_BUNDLE_HEADER
#define BENCH_BUNDLE_HEADER "bundle_gsp.h"
#endif
#include BENCH_BUNDLE_HEADER

#include "gsp/format/gsp_common_format.h"
#include "gsp/gsp_app.h"
#include "gsp/gsp_context.h"
#include "gsp_app_port.h"

static gsp_app_t *s_app;
static bench_workload_t s_workload;
static bool s_grid_ready;
static bool s_messages_ready;
static bool s_drawer_ready;
static bool s_tab_ready;

/* Visual fixtures, not a replacement for the board's measured drivers. */
static gsp_err_t preview_grid_cell(esp_gsp_handle_t ui,
                                   esp_gsp_grid_cell_t cell, uint32_t item,
                                   void *user_ctx)
{
    (void)ui;
    (void)cell;
    (void)item;
    (void)user_ctx;
    return GSP_OK;
}

static uint32_t preview_message_count(void *user_ctx)
{
    (void)user_ctx;
    return 16;
}

static bool preview_message_get(void *user_ctx, uint32_t index,
                                esp_gsp_message_t *message)
{
    (void)user_ctx;
    static const char *text[] = {
        "Render pipeline ready.", "Images, layers and text share the scene.",
        "Can history keep its anchor?", "Yes. Variable-height rows stay scrollable.",
    };
    if (message == NULL || index >= 16) {
        return false;
    }
    *message = (esp_gsp_message_t) {
        .id = index, .text = text[index % 4],
        .direction = index % 2 ? ESP_GSP_MESSAGE_OUTGOING : ESP_GSP_MESSAGE_INCOMING,
    };
    return true;
}

static void preview_components(esp_gsp_handle_t ui, void *user_ctx)
{
    (void)user_ctx;
    bool visible = false;
    if (!s_grid_ready && esp_gsp_get_visible(ui, GSP_BIND_P_GRID, &visible) == ESP_GSP_OK && visible) {
        s_grid_ready = gsp_bench_bench_grid_bind(ui, preview_grid_cell, NULL) != ESP_GSP_GRID_NONE;
    }
    if (!s_messages_ready && esp_gsp_get_visible(ui, GSP_BIND_P_MESSAGES, &visible) == ESP_GSP_OK && visible) {
        static const esp_gsp_message_source_t source = {
            .struct_size = sizeof(esp_gsp_message_source_t),
            .count = preview_message_count, .get = preview_message_get,
        };
        s_messages_ready = gsp_bench_bench_messages_bind(ui, &source) != ESP_GSP_LIST_NONE;
    }
    if (!s_drawer_ready && esp_gsp_get_visible(ui, GSP_BIND_P_DRAWER, &visible) == ESP_GSP_OK && visible) {
        s_drawer_ready = esp_gsp_drawer_open(ui, GSP_OBJ_KEY_QUICK_DRAWER, false) == ESP_GSP_OK;
    }
    const char *state = getenv("GSP_BENCH_PREVIEW_STATE");
    if (!s_tab_ready && state != NULL && strcmp(state, "default") != 0 &&
            esp_gsp_get_visible(ui, GSP_BIND_P_COMPOSITES, &visible) == ESP_GSP_OK && visible) {
        bool keyboard = strcmp(state, "keyboard") == 0;
        s_tab_ready = esp_gsp_page_flow_set_page(ui, GSP_OBJ_KEY_BENCH_TABS, keyboard ? 1 : 2, false) == ESP_GSP_OK;
        (void)esp_gsp_set_text(ui, GSP_BIND_KT0, "GSP / keyboard input preview");
        (void)gsp_bench_bench_modal_set_visible(ui, !keyboard);
    }
}

void gsp_sim_application_reset(void)
{
    s_app = NULL;
    s_grid_ready = false;
    s_messages_ready = false;
    s_drawer_ready = false;
    s_tab_ready = false;
}

void gsp_sim_application_setup(gsp_app_t *app)
{
    if (app == NULL || app == s_app) {
        return;
    }
    gsp_context_t *context = gsp_app_current_context(app);
    if (context == NULL) {
        return;
    }
    s_app = app;
    const char *preview = getenv("GSP_BENCH_RESULTS_PREVIEW");
    if (preview != NULL && preview[0] == '1') {
        static const uint16_t names[] = {
            GSP_BIND_RESULT_NAME0, GSP_BIND_RESULT_NAME1, GSP_BIND_RESULT_NAME2,
            GSP_BIND_RESULT_NAME3, GSP_BIND_RESULT_NAME4, GSP_BIND_RESULT_NAME5,
        };
        static const uint16_t values[] = {
            GSP_BIND_RESULT_VALUE0, GSP_BIND_RESULT_VALUE1, GSP_BIND_RESULT_VALUE2,
            GSP_BIND_RESULT_VALUE3, GSP_BIND_RESULT_VALUE4, GSP_BIND_RESULT_VALUE5,
        };
        static const uint16_t fps[] = {
            GSP_BIND_RESULT_FPS0, GSP_BIND_RESULT_FPS1, GSP_BIND_RESULT_FPS2,
            GSP_BIND_RESULT_FPS3, GSP_BIND_RESULT_FPS4, GSP_BIND_RESULT_FPS5,
        };
        static const char *labels[] = {
            "fade through black", "image rotation", "dropdown select",
            "blend load x64", "clock needles", "render storm",
        };
        esp_gsp_handle_t ui = gsp_app_handle(app);
        (void)esp_gsp_set_text(ui, GSP_BIND_RESULT_TITLE, "PREVIEW / NOT A SCORE");
        for (size_t row = 0; row < 6; ++row) {
            (void)esp_gsp_set_text(ui, names[row], labels[row]);
            (void)esp_gsp_set_text(ui, fps[row], "123.4");
            (void)esp_gsp_set_text(ui, values[row], "R 123.4 ms\nS 123.4 ms");
        }
    }
    bench_workload_init(
        &s_workload,
        context->scene.color_profile ==
        GSP_FORMAT_COMMON_PIXEL_FORMAT_RGB888);
    (void)esp_gsp_timer_create(gsp_app_handle(app), 1000,
                               bench_workload_drive_tweens, &s_workload);
    const char *visual = getenv("GSP_BENCH_VISUAL_PREVIEW");
    if (visual != NULL && visual[0] == '1') {
        (void)esp_gsp_timer_create(gsp_app_handle(app), 16, preview_components, NULL);
        printf("gsp_sim: benchmark visual fixtures enabled (not hardware workload)\n");
    }
    printf("gsp_sim: benchmark tween workload enabled (%s)\n",
           s_workload.rgb888 ? "RGB888" : "RGB565");
}
