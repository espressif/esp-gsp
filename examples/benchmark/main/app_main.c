/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* GSP benchmark, mirroring lv_demo_benchmark's methodology: one page
 * per render class, each with its own characteristic motion (card
 * color tweens, translucent panel pulses, blinking image grids, live
 * keyboard typing, template instances sweeping the screen, full-screen
 * slide transitions between two scenes). A fixed dwell per page,
 * rendered fps sampled from the engine frame counter, and separate
 * wall-time and active-engine aggregates at the end of every cycle. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_gsp_esp_lcd.h"
#include "esp_gsp_debug.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "hw_init.h"

#include "gsp/gsp_render_profile.h"

#define GSP_BUNDLE_ENABLE_LEGACY_NAMES 1
#include "bundle_gsp.h"
#include "esp_gsp_deployable.h"
#include "bench_motion.h"
#include "bench_present_modes.h"
#include "bench_workload.h"

/* Must match the generated bundle and the CMake option GSP_BENCH_RGB888
 * (default OFF). A mismatched default would init an RGB888 panel against an
 * RGB565 bundle and fail present_target_create. */
#ifndef GSP_BENCH_RGB888
#define GSP_BENCH_RGB888 0
#endif

#define BENCH_PROTOCOL_VERSION 13U
#define BENCH_ENABLE_DRAWER 1
#define BENCH_DRAG_FADE_BLACK_POINT_PERCENT 40U
#define BENCH_FRAME_PERIOD_MS 16U
#define BENCH_FRAME_TARGET_HZ 62.5
#define BENCH_STRESS_PERIOD_MS 4U
#define BENCH_STRESS_TARGET_HZ 250.0
#define BENCH_TRANSITION_SAMPLE_CAPACITY 16U
#define BENCH_KEYBOARD_TEXT_CAPACITY 64U
#ifdef GSP_BENCH_DISABLE_TRANSITION_SNAPSHOTS
#define BENCH_TRANSITION_SNAPSHOT_MODE "off"
#else
#define BENCH_TRANSITION_SNAPSHOT_MODE "on"
#endif

/* The JSON selected by CMake defines the logical panel size. */
#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
#define BENCH_W 1024
#define BENCH_H 600
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB || CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
#if CONFIG_EXAMPLE_DISPLAY_ROTATION_90 || CONFIG_EXAMPLE_DISPLAY_ROTATION_270
#define BENCH_W 480
#define BENCH_H 800
#else
#define BENCH_W 800
#define BENCH_H 480
#endif
#elif CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM
#define BENCH_W 320
#define BENCH_H 240
#elif CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM
#define BENCH_W 240
#define BENCH_H 240
#else
#define BENCH_W 360
#define BENCH_H 360
#endif

/* Sentinel binds for scene-transition workloads (no page layer). */
#define BENCH_PAGE_TRANSITION_LEFT  0xFFFFU
#define BENCH_PAGE_TRANSITION_RIGHT 0xFFFEU
#define BENCH_PAGE_TRANSITION_UP    0xFFFDU
#define BENCH_PAGE_TRANSITION_DOWN  0xFFFCU
#define BENCH_PAGE_TRANSITION_FADE  0xFFFBU
#define BENCH_PAGE_DRAG_COMMIT      0xFFFAU
#define BENCH_PAGE_DRAG_CANCEL      0xFFF9U
#define BENCH_PAGE_DRAG_FLICK       0xFFF8U
#define BENCH_PAGE_TRANSITION_FADE_THROUGH_BLACK 0xFFF7U
#define BENCH_PAGE_DRAG_FADE        0xFFF6U
#define BENCH_DRAWER_DWELL_MS        7000U

typedef struct {
    const char *name;
    uint16_t bind;
    uint32_t dwell_ms;
    float wall_fps;   /*!< frames over the page dwell (activity rate) */
    float busy_fps;   /*!< frames over time spent rendering (engine
                           throughput; the weighted score) */
    float render_ms;  /*!< avg rasterize ms per frame */
    float submit_ms;  /*!< avg submit ms per frame */
    float service_us; /*!< avg control-plane time per service iteration */
    float commands_s; /*!< application commands drained per second */
    uint32_t frames;  /*!< submitted frames in the measured interval */
    float busy_ms;    /*!< complete engine time per submitted frame */
    uint64_t busy_us; /*!< exact total engine time in the interval */
    uint64_t elapsed_us; /*!< exact wall time in the measured interval */
    uint64_t render_us;
    uint64_t submit_us;
    uint64_t service_us_total;
    uint32_t service_iterations;
    uint32_t service_commands;
    uint32_t internal_peak_bytes;
    uint32_t psram_peak_bytes;
    uint32_t transition_count;
    uint32_t transition_errors;
    uint32_t transition_no_visual;
    uint32_t transition_frames;
    uint64_t transition_total_us;
    uint64_t transition_max_us;
    uint64_t transition_samples[BENCH_TRANSITION_SAMPLE_CAPACITY];
    uint8_t transition_sample_count;
} bench_page_t;

#define BENCH_PAGE(name_, bind_, dwell_) \
    { .name = (name_), .bind = (bind_), .dwell_ms = (dwell_) }

static bench_page_t s_pages[] = {
    BENCH_PAGE("rect x1",     GSP_BIND_P_RECT1,      3000),
    BENCH_PAGE("rect x9",     GSP_BIND_P_RECTS,      3000),
    BENCH_PAGE("containers",  GSP_BIND_P_CARDS,      3000),
    BENCH_PAGE("shapes",      GSP_BIND_P_SHAPES,     3000),
    BENCH_PAGE("clock needles", GSP_BIND_P_CLOCK,     3000),
    BENCH_PAGE("gradients",   GSP_BIND_P_GRAD,       3000),
    BENCH_PAGE("opa fills",   GSP_BIND_P_OPA,        3000),
    BENCH_PAGE("opa layers",  GSP_BIND_P_OPAL,       3000),
    BENCH_PAGE("overlay",     GSP_BIND_P_OVER,       3000),
    BENCH_PAGE("labels",      GSP_BIND_P_TEXT,       5000),
    BENCH_PAGE("screen text", GSP_BIND_P_BIGTEXT,    5000),
    BENCH_PAGE("scroll rows", GSP_BIND_P_SCROLL,     5000),
    BENCH_PAGE("arcs x6",     GSP_BIND_P_ARCS,       3000),
    BENCH_PAGE("img rgb x9",  GSP_BIND_P_IMGRGB,     3000),
    BENCH_PAGE("img argb x9", GSP_BIND_P_IMGARGB,    3000),
    BENCH_PAGE("img qoi x9",  GSP_BIND_P_QOI,        3000),
    BENCH_PAGE("image scale", GSP_BIND_P_SCALE,      5000),
    BENCH_PAGE("gram_te",      GSP_BIND_P_STREAM,     5000),
    BENCH_PAGE("wallpaper",   GSP_BIND_P_WALL,       3000),
    BENCH_PAGE("static move", GSP_BIND_P_STATIC_MOVE, 5000),
    BENCH_PAGE("moving",      GSP_BIND_P_MOVE,       5000),
    BENCH_PAGE("grid album",  GSP_BIND_P_GRID,       5000),
    BENCH_PAGE("messages",    GSP_BIND_P_MESSAGES,   6000),
    BENCH_PAGE("widgets",     GSP_BIND_P_WIDGETS,    5000),
    BENCH_PAGE("page flow",   GSP_BIND_P_FLOW,       5000),
    BENCH_PAGE("stack view",  GSP_BIND_P_STACK,      5000),
    BENCH_PAGE("composites",  GSP_BIND_P_COMPOSITES, 6000),
#if BENCH_ENABLE_DRAWER
    BENCH_PAGE("drawer",      GSP_BIND_P_DRAWER,     BENCH_DRAWER_DWELL_MS),
#endif
    BENCH_PAGE("wheels x3",   GSP_BIND_P_WHEEL,      5000),
    BENCH_PAGE("anim gif x4", GSP_BIND_P_ANIM,       5000),
    BENCH_PAGE("slide left",  BENCH_PAGE_TRANSITION_LEFT,  5000),
    BENCH_PAGE("slide right", BENCH_PAGE_TRANSITION_RIGHT, 5000),
    BENCH_PAGE("slide up",    BENCH_PAGE_TRANSITION_UP,    5000),
    BENCH_PAGE("slide down",  BENCH_PAGE_TRANSITION_DOWN,  5000),
    BENCH_PAGE("cross fade",  BENCH_PAGE_TRANSITION_FADE,  5000),
    BENCH_PAGE("fade through black",
               BENCH_PAGE_TRANSITION_FADE_THROUGH_BLACK, 5000),
    BENCH_PAGE("drag commit", BENCH_PAGE_DRAG_COMMIT,      5000),
    BENCH_PAGE("drag cancel", BENCH_PAGE_DRAG_CANCEL,      5000),
    BENCH_PAGE("drag flick",  BENCH_PAGE_DRAG_FLICK,       5000),
    BENCH_PAGE("drag slide fade", BENCH_PAGE_DRAG_FADE,     5000),
    BENCH_PAGE("motion",      GSP_BIND_P_MOTION,     3000),
};
#define BENCH_PAGE_COUNT (sizeof(s_pages) / sizeof(s_pages[0]))

static uint8_t s_page;
static int64_t s_page_start_us;
static uint32_t s_page_start_frames;
static uint64_t s_page_start_busy_us;
static uint64_t s_page_start_render_us;
static uint64_t s_page_start_submit_us;
static uint32_t s_page_start_service_iterations;
static uint64_t s_page_start_service_us;
static uint32_t s_page_start_service_commands;
static uint64_t s_page_start_ops[GSP_RENDERER_PROFILE_OPCODES];
static esp_gsp_transition_stats_t s_page_start_transition_stats;
static bool s_page_waiting_for_scene;
static uint8_t s_pending_page;
static bool s_scene_cleanup_pending;
static bool s_visibility_initialized;
static size_t s_page_internal_start;
static size_t s_page_internal_min;
static size_t s_page_psram_start;
static size_t s_page_psram_min;
static uint16_t s_drag_tick;
static uint16_t s_drawer_tick;
static uint32_t s_drawer_opens;
static uint32_t s_drawer_closes;
static uint32_t s_drawer_errors;
static uint16_t s_wheel_tick;
static uint32_t s_wheel_command_errors;
static esp_gsp_grid_t s_grid = ESP_GSP_GRID_NONE;
static uint16_t s_grid_tick;
static bool s_grid_pressed;
static uint32_t s_grid_binds;
static uint32_t s_grid_drags;
static uint32_t s_grid_errors;
static uint32_t s_static_move_commands;
static uint32_t s_static_move_errors;
static uint32_t s_static_move_tick;
static uint32_t s_move_commands;
static uint32_t s_move_errors;
static uint32_t s_move_tick;
static uint32_t s_component_commands;
static uint32_t s_component_errors;
static uint32_t s_clock_updates;
static uint32_t s_clock_commands;
static uint32_t s_clock_errors;
static uint32_t s_clock_tick;
static uint16_t s_composite_tab;
static uint16_t s_keyboard_step;
static bool s_keyboard_pressed;
static uint32_t s_keyboard_key_presses;
static uint32_t s_keyboard_text_updates;
static uint32_t s_keyboard_backspaces;
static char s_keyboard_last_text[BENCH_KEYBOARD_TEXT_CAPACITY];
static bool s_modal_cleanup_pending;
static esp_gsp_region_stats_t s_measured_region_start;
static esp_gsp_media_stats_t s_measured_media_start;
static uint32_t s_frame_tick;
static uint32_t s_text_frame;
static uint32_t s_scroll_offset;
static int32_t s_rect_load;
static bench_workload_t s_bench_workload;
static bool s_transition_alt;
static bool s_drag_left = true;
static bool s_drag_pressed;
static bool s_drag_finishing;
static bool s_drag_settled;
static bool s_drawer_pressed;
static esp_gsp_widget_t s_movers[4];
static uint8_t s_mover_count;

typedef struct {
    bool active;
    uint8_t page;
    uint16_t expected_scene;
    int64_t start_us;
    uint32_t start_frames;
} bench_transition_measure_t;

static bench_transition_measure_t s_transition_measure;

static const char *const s_scroll_pool[] = {
    "alpha service ready", "beta channel 42 ok", "gamma load 61%",
    "delta rssi -48 dBm", "epsilon queue empty", "zeta sync done",
    "eta uplink active", "theta temp 36.2C", "iota fan 1200rpm",
    "kappa io idle", "lambda buf 12/32", "mu retry count 0",
};
/* Runtime color APIs take profile-native values. Keep one semantic RGB888
 * color beside its quantized RGB565 representation so both benchmark builds
 * exercise the same visual scene without render-loop conversion. */
static inline uint32_t bench_native_color(uint32_t rgb888, uint16_t rgb565)
{
#if GSP_BENCH_RGB888
    (void)rgb565;
    return rgb888;
#else
    (void)rgb888;
    return rgb565;
#endif
}

static size_t make_solid_qoi(uint8_t *out, uint16_t width,
                             uint16_t height, uint8_t red,
                             uint8_t green, uint8_t blue)
{
    size_t offset = 0;
    memcpy(out + offset, "qoif", 4);
    offset += 4;
    out[offset++] = (uint8_t)(width >> 24);
    out[offset++] = (uint8_t)(width >> 16);
    out[offset++] = (uint8_t)(width >> 8);
    out[offset++] = (uint8_t)width;
    out[offset++] = (uint8_t)(height >> 24);
    out[offset++] = (uint8_t)(height >> 16);
    out[offset++] = (uint8_t)(height >> 8);
    out[offset++] = (uint8_t)height;
    out[offset++] = 4;
    out[offset++] = 0;
    out[offset++] = 0xFF;
    out[offset++] = red;
    out[offset++] = green;
    out[offset++] = blue;
    out[offset++] = 255;
    size_t remaining = (size_t)width * height - 1U;
    while (remaining != 0) {
        size_t run = remaining > 62U ? 62U : remaining;
        out[offset++] = (uint8_t)(0xC0U | (run - 1U));
        remaining -= run;
    }
    memset(out + offset, 0, 7);
    offset += 7;
    out[offset++] = 1;
    return offset;
}

static uint32_t s_dynamic_qoi_accepted;
static uint32_t s_dynamic_qoi_errors;
static uint32_t s_image_scale_updates;
static uint32_t s_image_scale_commands;
static uint32_t s_image_scale_errors;
static uint32_t s_image_scale_tick;

static bool benchmark_programmatic_transition_page(uint16_t bind)
{
    switch (bind) {
    case BENCH_PAGE_TRANSITION_LEFT:
    case BENCH_PAGE_TRANSITION_RIGHT:
    case BENCH_PAGE_TRANSITION_UP:
    case BENCH_PAGE_TRANSITION_DOWN:
    case BENCH_PAGE_TRANSITION_FADE:
    case BENCH_PAGE_TRANSITION_FADE_THROUGH_BLACK:
        return true;
    default:
        return false;
    }
}

static bool benchmark_drag_page(uint16_t bind)
{
    return bind == BENCH_PAGE_DRAG_COMMIT ||
           bind == BENCH_PAGE_DRAG_CANCEL ||
           bind == BENCH_PAGE_DRAG_FLICK ||
           bind == BENCH_PAGE_DRAG_FADE;
}

static bool benchmark_transition_page(uint16_t bind)
{
    return benchmark_programmatic_transition_page(bind) ||
           benchmark_drag_page(bind);
}

static esp_gsp_transition_t benchmark_transition_kind(uint16_t bind)
{
    switch (bind) {
    case BENCH_PAGE_TRANSITION_LEFT:
        return ESP_GSP_SLIDE_LEFT;
    case BENCH_PAGE_TRANSITION_RIGHT:
        return ESP_GSP_SLIDE_RIGHT;
    case BENCH_PAGE_TRANSITION_UP:
        return ESP_GSP_SLIDE_UP;
    case BENCH_PAGE_TRANSITION_DOWN:
        return ESP_GSP_SLIDE_DOWN;
    case BENCH_PAGE_TRANSITION_FADE:
        return ESP_GSP_CROSS_FADE;
    case BENCH_PAGE_TRANSITION_FADE_THROUGH_BLACK:
        return ESP_GSP_FADE_THROUGH_BLACK;
    default:
        return ESP_GSP_NO_TRANSITION;
    }
}

static void transition_record(esp_gsp_handle_t ui, uint8_t page,
                              int64_t start_us, uint32_t start_frames)
{
    if (page >= BENCH_PAGE_COUNT) {
        return;
    }
    bench_page_t *result = &s_pages[page];
    uint64_t elapsed_us = (uint64_t)(esp_timer_get_time() - start_us);
    uint32_t frames = esp_gsp_frame_count(ui) - start_frames;
    ++result->transition_count;
    if (frames == 0) {
        ++result->transition_no_visual;
    }
    result->transition_frames += frames;
    result->transition_total_us += elapsed_us;
    if (elapsed_us > result->transition_max_us) {
        result->transition_max_us = elapsed_us;
    }
    if (result->transition_sample_count <
            BENCH_TRANSITION_SAMPLE_CAPACITY) {
        result->transition_samples[result->transition_sample_count++] =
            elapsed_us;
    }
}

static bool transition_measure_begin(esp_gsp_handle_t ui,
                                     uint16_t expected_scene)
{
    if (s_transition_measure.active) {
        ++s_pages[s_page].transition_errors;
        return false;
    }
    s_transition_measure = (bench_transition_measure_t) {
        .active = true,
        .page = s_page,
        .expected_scene = expected_scene,
        .start_us = esp_timer_get_time(),
        .start_frames = esp_gsp_frame_count(ui),
    };
    return true;
}

static void transition_measure_abort(void)
{
    if (s_transition_measure.active &&
            s_transition_measure.page < BENCH_PAGE_COUNT) {
        ++s_pages[s_transition_measure.page].transition_errors;
    }
    s_transition_measure.active = false;
}

static void benchmark_event(esp_gsp_handle_t ui,
                            const esp_gsp_event_t *event,
                            void *user_ctx)
{
    (void)user_ctx;
    if (event == NULL || event->type != ESP_GSP_EVENT_SCENE_CHANGED) {
        return;
    }
    s_transition_alt = event->scene_id != 0;
    if (!s_transition_measure.active) {
        if (s_scene_cleanup_pending && event->scene_id == 0U) {
            s_scene_cleanup_pending = false;
            return;
        }
        printf("bench: unexpected scene change page=%s scene=%u"
               " drawer_tick=%u wheel_tick=%u\n",
               s_pages[s_page].name, event->scene_id,
               s_drawer_tick, s_wheel_tick);
        return;
    }
    if (event->scene_id != s_transition_measure.expected_scene) {
        transition_measure_abort();
        return;
    }
    transition_record(ui, s_transition_measure.page,
                      s_transition_measure.start_us,
                      s_transition_measure.start_frames);
    s_transition_measure.active = false;
}

static uint64_t transition_percentile(const bench_page_t *page,
                                      uint32_t percentile)
{
    uint64_t samples[BENCH_TRANSITION_SAMPLE_CAPACITY];
    size_t count = page->transition_sample_count;
    memcpy(samples, page->transition_samples,
           count * sizeof(samples[0]));
    for (size_t index = 1; index < count; ++index) {
        uint64_t value = samples[index];
        size_t insert = index;
        while (insert > 0 && samples[insert - 1] > value) {
            samples[insert] = samples[insert - 1];
            --insert;
        }
        samples[insert] = value;
    }
    if (count == 0) {
        return 0;
    }
    size_t rank = ((size_t)percentile * count + 99U) / 100U;
    return samples[rank == 0 ? 0 : rank - 1U];
}

static void sample_page_memory(void)
{
    size_t internal = heap_caps_get_free_size(
                          MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t psram = heap_caps_get_free_size(
                       MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (internal < s_page_internal_min) {
        s_page_internal_min = internal;
    }
    if (psram < s_page_psram_min) {
        s_page_psram_min = psram;
    }
}

static void movers_destroy(esp_gsp_handle_t ui)
{
    for (uint8_t index = 0; index < s_mover_count; ++index) {
        (void)esp_gsp_widget_destroy(ui, s_movers[index]);
    }
    s_mover_count = 0;
}

/* Momentum wheels: three lists bound once (the list quota is not
 * recyclable); leaving the page empties them so the floating row
 * instances vanish, re-entering restores the item count. */
static esp_gsp_list_t s_wheels[3] = {
    ESP_GSP_LIST_NONE, ESP_GSP_LIST_NONE, ESP_GSP_LIST_NONE,
};
#define BENCH_WHEEL_ITEMS 48U

static esp_gsp_list_t s_messages = ESP_GSP_LIST_NONE;
static uint32_t s_message_first = 8;
static uint32_t s_message_count = 32;
static uint16_t s_message_tick;
static uint32_t s_message_errors;

typedef struct {
    const char *text;
    esp_gsp_message_direction_t direction;
} bench_message_t;

static const bench_message_t s_message_samples[] = {
    {"Hey, is the new message list ready?", ESP_GSP_MESSAGE_INCOMING},
    {
        "Yes. Bubbles now size themselves to the text.",
        ESP_GSP_MESSAGE_OUTGOING
    },
    {
        "Nice. Can I scroll back through the full history?",
        ESP_GSP_MESSAGE_INCOMING
    },
    {
        "Absolutely. Drag or fling naturally to browse older messages.",
        ESP_GSP_MESSAGE_OUTGOING
    },
    {
        "Does loading history keep my reading position?",
        ESP_GSP_MESSAGE_INCOMING
    },
    {
        "It does. Prepended messages preserve the visible anchor.",
        ESP_GSP_MESSAGE_OUTGOING
    },
    {"Great.", ESP_GSP_MESSAGE_INCOMING},
    {
        "Colors, spacing, radius and row templates are customizable.",
        ESP_GSP_MESSAGE_OUTGOING
    },
    {"That feels much more like a real chat.", ESP_GSP_MESSAGE_INCOMING},
    {
        "And long messages wrap without forcing every row to one height.",
        ESP_GSP_MESSAGE_OUTGOING
    },
    {"What happens when a new message arrives?", ESP_GSP_MESSAGE_INCOMING},
    {
        "It appears at the bottom while history remains scrollable.",
        ESP_GSP_MESSAGE_OUTGOING
    },
    {"Perfect. Let's run it on the boards.", ESP_GSP_MESSAGE_INCOMING},
    {
        "Already testing RGB, SPI, QSPI and MIPI displays.",
        ESP_GSP_MESSAGE_OUTGOING
    },
    {"No fixed history limit?", ESP_GSP_MESSAGE_INCOMING},
    {
        "No fixed item limit. Only visible rows stay instantiated.",
        ESP_GSP_MESSAGE_OUTGOING
    },
};

static uint32_t message_count(void *user_ctx)
{
    (void)user_ctx;
    return s_message_count;
}

static bool message_get(void *user_ctx, uint32_t index,
                        esp_gsp_message_t *out_message)
{
    (void)user_ctx;
    if (out_message == NULL || index >= s_message_count) {
        return false;
    }
    uint32_t physical = s_message_first + index;
    const bench_message_t *sample =
        &s_message_samples[physical %
                           (sizeof(s_message_samples) /
                            sizeof(s_message_samples[0]))];
    *out_message = (esp_gsp_message_t) {
        .text = sample->text,
        .id = physical,
        .revision = 0,
        .direction = sample->direction,
    };
    return true;
}

static const esp_gsp_message_source_t s_message_source = {
    .struct_size = sizeof(esp_gsp_message_source_t),
    .count = message_count,
    .get = message_get,
};

static void wheel_record(esp_gsp_err_t result)
{
    if (result != ESP_GSP_OK) {
        ++s_wheel_command_errors;
    }
}

static gsp_err_t wheel_bind_row(esp_gsp_handle_t gsp,
                                esp_gsp_row_t row, uint32_t item,
                                void *user_ctx)
{
    (void)user_ctx;
    char text[16];
    snprintf(text, sizeof(text), "item %u", (unsigned)item);
    (void)esp_gsp_row_text(gsp, row, text);
    return GSP_OK;
}

static gsp_err_t grid_bind_cell(esp_gsp_handle_t gsp,
                                esp_gsp_grid_cell_t cell, uint32_t item,
                                void *user_ctx)
{
    (void)gsp;
    (void)cell;
    (void)item;
    (void)user_ctx;
    ++s_grid_binds;
    return GSP_OK;
}

/* Synthetic Canvas producer: the renderer lends only the dirty destination
 * rows to this callback, so the 25 fps stream needs no intermediate frames. */
#define BENCH_CANVAS_W (BENCH_W < 320 ? 64 : BENCH_W / 2)
#define BENCH_CANVAS_H (BENCH_W < 320 ? 32 : BENCH_H / 3)
#define BENCH_CANVAS_BAND_H (BENCH_CANVAS_H >= 16 ? 8U : 4U)
static uint16_t s_canvas_last_y = UINT16_MAX;
static uint16_t s_canvas_band_y = UINT16_MAX;
static uint32_t s_canvas_phase;
static uint32_t s_canvas_seq;
static uint32_t s_canvas_measured_frames;
static uint64_t s_canvas_measured_dirty_pixels;

static void canvas_stream_draw(const esp_gsp_canvas_surface_t *surface,
                               void *user_ctx)
{
    (void)user_ctx;
    size_t row_bytes = (size_t)surface->width *
                       (surface->pixel_format == ESP_GSP_CANVAS_PIXEL_RGB888 ?
                        3U : 2U);
    uint8_t *active_row = NULL;
    for (uint16_t row_index = 0; row_index < surface->height; ++row_index) {
        uint16_t canvas_y = (uint16_t)(surface->y + row_index);
        uint8_t *row = (uint8_t *)surface->pixels +
                       (size_t)row_index * surface->stride_bytes;
        bool active = canvas_y >= s_canvas_band_y &&
                      canvas_y < s_canvas_band_y + BENCH_CANVAS_BAND_H;
        if (!active) {
            memset(row, 0, row_bytes);
            continue;
        }
        if (active_row != NULL) {
            memcpy(row, active_row, row_bytes);
            continue;
        }
        active_row = row;
        for (uint16_t x = 0; x < surface->width; ++x) {
#if GSP_BENCH_RGB888
            /* Native RGB888 storage is B, G, R. */
            row[x * 3U] = 0xFFU;
            row[x * 3U + 1U] = (uint8_t)(s_canvas_phase * 11U);
            row[x * 3U + 2U] = (uint8_t)(s_canvas_phase * 19U);
#else
            uint16_t color = (uint16_t)(0x001FU |
                                        ((s_canvas_phase * 3U & 0x3FU) << 5) |
                                        ((s_canvas_phase * 5U & 0x1FU) << 11));
            memcpy(row + x * 2U, &color, sizeof(color));
#endif
        }
    }
}

static void canvas_stream_tick(esp_gsp_handle_t ui)
{
    uint32_t phase = s_canvas_seq++;
    uint32_t travel = BENCH_CANVAS_H - BENCH_CANVAS_BAND_H;
    uint32_t position = travel != 0 ? (phase * 2U) % (travel * 2U) : 0;
    uint16_t band_y = (uint16_t)(position <= travel ?
                                 position : travel * 2U - position);
    gsp_rect_t dirty = {
        .x1 = 0,
        .y1 = s_canvas_last_y == UINT16_MAX ||
        band_y < s_canvas_last_y ? band_y : s_canvas_last_y,
        .x2 = BENCH_CANVAS_W,
        .y2 = s_canvas_last_y == UINT16_MAX ? BENCH_CANVAS_H :
        (band_y > s_canvas_last_y ? band_y : s_canvas_last_y) +
        BENCH_CANVAS_BAND_H,
    };
    s_canvas_phase = phase;
    s_canvas_band_y = band_y;
    if (esp_gsp_canvas_invalidate_dirty(ui, GSP_BIND_CV0, dirty) != ESP_OK) {
        return;
    }
    s_canvas_last_y = band_y;
    ++s_canvas_measured_frames;
    s_canvas_measured_dirty_pixels +=
        (uint64_t)(dirty.x2 - dirty.x1) * (dirty.y2 - dirty.y1);
}

static void canvas_stream_open(esp_gsp_handle_t ui)
{
    s_canvas_last_y = UINT16_MAX;
    s_canvas_band_y = UINT16_MAX;
    s_canvas_seq = 0;
    ESP_ERROR_CHECK(esp_gsp_canvas_set_draw_cb(
                        ui, GSP_BIND_CV0, canvas_stream_draw, NULL));
}

static void canvas_stream_close(esp_gsp_handle_t ui)
{
    (void)esp_gsp_canvas_stop(ui, GSP_BIND_CV0);
}

static void activate_page(esp_gsp_handle_t ui, uint8_t page)
{
    uint8_t previous = s_page;
    s_page = page;
    s_pages[page].wall_fps = 0;
    s_pages[page].busy_fps = 0;
    s_pages[page].render_ms = 0;
    s_pages[page].submit_ms = 0;
    s_pages[page].service_us = 0;
    s_pages[page].commands_s = 0;
    s_pages[page].frames = 0;
    s_pages[page].busy_ms = 0;
    s_pages[page].elapsed_us = 0;
    s_pages[page].internal_peak_bytes = 0;
    s_pages[page].psram_peak_bytes = 0;
    s_pages[page].transition_count = 0;
    s_pages[page].transition_errors = 0;
    s_pages[page].transition_no_visual = 0;
    s_pages[page].transition_frames = 0;
    s_pages[page].transition_total_us = 0;
    s_pages[page].transition_max_us = 0;
    s_pages[page].transition_sample_count = 0;
    s_frame_tick = 0;
    s_text_frame = 0;
    s_scroll_offset = 0;
    s_rect_load = 0;
    s_transition_alt = false;
    s_component_commands = 0;
    s_component_errors = 0;
    if (s_pages[page].bind == GSP_BIND_P_CLOCK) {
        s_clock_updates = 0;
        s_clock_commands = 0;
        s_clock_errors = 0;
        s_clock_tick = 0;
    }
    s_composite_tab = 0;
    s_keyboard_step = 0;
    s_keyboard_pressed = false;
    s_keyboard_key_presses = 0;
    s_keyboard_text_updates = 0;
    s_keyboard_backspaces = 0;
    s_keyboard_last_text[0] = '\0';
    if (benchmark_drag_page(s_pages[page].bind)) {
        s_drag_tick = 0;
        s_drag_left = true;
        s_drag_pressed = false;
        s_drag_finishing = false;
        s_drag_settled = false;
    }
    bool drag_fade = s_pages[page].bind == BENCH_PAGE_DRAG_FADE;
    (void)esp_gsp_set_swipe_transition(
        ui, drag_fade ? ESP_GSP_SWIPE_SLIDE_FADE_THROUGH_BLACK
        : ESP_GSP_SWIPE_SLIDE);
    (void)esp_gsp_set_swipe_fade_black_point(
        ui, drag_fade ? BENCH_DRAG_FADE_BLACK_POINT_PERCENT : 50U);
    if (drag_fade) {
        printf("bench: drag_fade black_point=%u%%\n",
               BENCH_DRAG_FADE_BLACK_POINT_PERCENT);
    } else if (s_pages[page].bind == BENCH_PAGE_DRAG_CANCEL) {
        printf("bench: drag_cancel distance=20%% expected=return-to-source\n");
    }
#if BENCH_ENABLE_DRAWER
    if (s_pages[page].bind == GSP_BIND_P_DRAWER) {
        s_drawer_tick = 0;
        s_drawer_opens = 0;
        s_drawer_closes = 0;
        s_drawer_errors = 0;
        s_drawer_pressed = false;
        (void)esp_gsp_drawer_close(
            ui, GSP_OBJ_KEY_QUICK_DRAWER, false);
    }
#endif
    if (s_pages[page].bind == GSP_BIND_P_STREAM) {
        canvas_stream_open(ui);
        canvas_stream_tick(ui);
    }
    if (s_pages[page].bind == GSP_BIND_P_SCALE) {
        enum {
            SCALE_DYNAMIC_QOI_W = 80,
            SCALE_DYNAMIC_QOI_H = 60,
        };
        static uint8_t dynamic_qoi[512];
        size_t dynamic_size = make_solid_qoi(
                                  dynamic_qoi, SCALE_DYNAMIC_QOI_W,
                                  SCALE_DYNAMIC_QOI_H, 32, 180, 230);
        s_image_scale_updates = 0;
        s_image_scale_commands = 0;
        s_image_scale_errors = 0;
        s_image_scale_tick = 0;
        if (esp_gsp_set_image(
                    ui, GSP_BIND_SCALE_DYN,
                    dynamic_qoi, dynamic_size) != ESP_GSP_OK) {
            ++s_image_scale_errors;
        }
    }
    if (s_pages[page].bind == GSP_BIND_P_STATIC_MOVE) {
        s_static_move_commands = 0;
        s_static_move_errors = 0;
        s_static_move_tick = 0;
    }
    if (s_pages[page].bind == GSP_BIND_P_MOVE) {
        s_move_commands = 0;
        s_move_errors = 0;
        s_move_tick = 0;
    }
    if (!s_visibility_initialized) {
        for (uint8_t index = 0; index < BENCH_PAGE_COUNT; ++index) {
            if (!benchmark_transition_page(s_pages[index].bind)) {
                (void)esp_gsp_set_visible(ui, s_pages[index].bind,
                                          index == page);
            }
        }
        s_visibility_initialized = true;
    } else {
        if (!benchmark_transition_page(s_pages[previous].bind)) {
            (void)esp_gsp_set_visible(ui, s_pages[previous].bind, false);
        }
        if (!benchmark_transition_page(s_pages[page].bind)) {
            (void)esp_gsp_set_visible(ui, s_pages[page].bind, true);
        }
    }
    if (s_pages[page].bind == GSP_BIND_P_MOVE) {
        for (uint8_t index = 0; index < 4; ++index) {
            s_movers[index] = esp_gsp_widget_create(
                                  ui, GSP_TEMPLATE_BOX, (int16_t)(index * 60),
                                  (int16_t)(index * 40));
        }
        s_mover_count = 4;
    }
    if (s_pages[page].bind == GSP_BIND_P_GRID) {
        s_grid_tick = 0;
        s_grid_pressed = false;
        s_grid_binds = 0;
        s_grid_drags = 0;
        s_grid_errors = 0;
        if (s_grid == ESP_GSP_GRID_NONE) {
            s_grid = gsp_bench_bench_grid_bind(
                         ui, grid_bind_cell, NULL);
            if (s_grid == ESP_GSP_GRID_NONE) {
                ++s_grid_errors;
            }
        } else if (gsp_bench_bench_grid_refresh(ui, s_grid) !=
                   ESP_GSP_OK) {
            ++s_grid_errors;
        }
    }
    if (s_pages[page].bind == GSP_BIND_P_COMPOSITES) {
        /* Establish a known hidden baseline before the workload starts,
         * retrying from drive_fx if the command queue is temporarily full. */
        s_modal_cleanup_pending = true;
        (void)esp_gsp_set_text(ui, GSP_BIND_KT0, "");
        (void)esp_gsp_set_cursor(ui, GSP_BIND_KT0);
        if (esp_gsp_keyboard_attach(
                    ui, GSP_ACT_ID_BENCH_KEYBOARD_KEY,
                    GSP_BIND_KT0) != ESP_GSP_OK) {
            ++s_component_errors;
        }
    }
    if (s_pages[page].bind == GSP_BIND_P_MESSAGES) {
        s_message_first = 8;
        s_message_count = 32;
        s_message_tick = 0;
        if (s_messages == ESP_GSP_LIST_NONE) {
            s_messages = gsp_bench_bench_messages_bind(
                             ui, &s_message_source);
            if (s_messages == ESP_GSP_LIST_NONE) {
                ++s_message_errors;
            }
        } else if (gsp_bench_bench_messages_changed(
                       ui, s_messages, 0) != ESP_GSP_OK) {
            ++s_message_errors;
        }
    }
    if (s_pages[page].bind == GSP_BIND_P_WHEEL) {
        s_wheel_tick = 0;
        s_wheel_command_errors = 0;
        if (s_wheels[0] == ESP_GSP_LIST_NONE) {
            s_wheels[0] = gsp_bench_whl0_bind(
                              ui, wheel_bind_row, NULL);
            s_wheels[1] = gsp_bench_whl1_bind(
                              ui, wheel_bind_row, NULL);
            s_wheels[2] = gsp_bench_whl2_bind(
                              ui, wheel_bind_row, NULL);
            for (uint8_t index = 0; index < 3; ++index) {
                if (s_wheels[index] != ESP_GSP_LIST_NONE) {
                    wheel_record(esp_gsp_list_set_total(
                                     ui, s_wheels[index], BENCH_WHEEL_ITEMS));
                    wheel_record(esp_gsp_list_snap(
                                     ui, s_wheels[index], true));
                    wheel_record(esp_gsp_list_fade(
                                     ui, s_wheels[index],
                                     bench_native_color(0x161E2CU, 0x18E5U), true));
                } else {
                    ++s_wheel_command_errors;
                }
            }
        } else {
            for (uint8_t index = 0; index < 3; ++index) {
                if (s_wheels[index] != ESP_GSP_LIST_NONE) {
                    wheel_record(esp_gsp_list_set_total(
                                     ui, s_wheels[index], BENCH_WHEEL_ITEMS));
                    wheel_record(esp_gsp_list_scroll_to(
                                     ui, s_wheels[index], 0));
                }
            }
        }
    }
    printf("bench: page %s\n", s_pages[page].name);
    s_page_start_us = esp_timer_get_time();
    esp_gsp_render_stats(ui, &s_page_start_frames,
                         &s_page_start_busy_us);
    esp_gsp_render_phases(ui, &s_page_start_render_us,
                          &s_page_start_submit_us);
    esp_gsp_service_stats(ui, &s_page_start_service_iterations,
                          &s_page_start_service_us,
                          &s_page_start_service_commands);
    gsp_renderer_profile_snapshot(s_page_start_ops, NULL,
                                  GSP_RENDERER_PROFILE_OPCODES);
    esp_gsp_transition_stats(ui, &s_page_start_transition_stats);
    s_page_internal_start = heap_caps_get_free_size(
                                MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    s_page_psram_start = heap_caps_get_free_size(
                             MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    s_page_internal_min = s_page_internal_start;
    s_page_psram_min = s_page_psram_start;
}

static void show_page(esp_gsp_handle_t ui, uint8_t page)
{
    uint16_t leaving = s_pages[s_page].bind;
    if (benchmark_transition_page(leaving)) {
        transition_measure_abort();
        if (s_drag_pressed) {
            (void)esp_gsp_inject_touch(ui, 0, 0, false);
            s_drag_pressed = false;
        }
        /* goto is consumed before the next UI step. Do not apply scene-0
         * binds synchronously while the transition's scene-1 snapshot is
         * still current; finish this page switch on the next scheduler tick. */
        s_scene_cleanup_pending = s_transition_alt;
        (void)esp_gsp_goto_scene(ui, 0, ESP_GSP_NO_TRANSITION);
        s_pending_page = page;
        s_page_waiting_for_scene = true;
        return;
    }
    if (leaving == GSP_BIND_P_MOVE) {
        movers_destroy(ui);
    }
    if (leaving == GSP_BIND_P_GRID && s_grid_pressed) {
        if (esp_gsp_inject_touch(ui, 0, 0, false) != ESP_GSP_OK) {
            ++s_grid_errors;
        }
        s_grid_pressed = false;
    }
    if (leaving == GSP_BIND_P_COMPOSITES) {
        /* Clear the transient caret and reset the modal before the next
         * composites run. The modal now inherits this page's visibility, so
         * hiding the page gates paint and hits immediately even if a
         * saturated command queue delays its own HIDE update. */
        if (s_keyboard_pressed) {
            (void)esp_gsp_inject_touch(ui, 0, 0, false);
            s_keyboard_pressed = false;
        }
        (void)esp_gsp_set_cursor(ui, ESP_GSP_NO_CURSOR);
        (void)esp_gsp_keyboard_attach(
            ui, ESP_GSP_KEYBOARD_NONE, GSP_BIND_KT0);
        s_modal_cleanup_pending = true;
        (void)gsp_bench_bench_modal_set_visible(ui, false);
    }
    if (leaving == GSP_BIND_P_WIDGETS) {
        /* Lift a possibly mid-press injected tap. */
        (void)esp_gsp_inject_touch(ui, 0, 0, false);
    }
#if BENCH_ENABLE_DRAWER
    if (leaving == GSP_BIND_P_DRAWER) {
        if (s_drawer_pressed) {
            (void)esp_gsp_inject_touch(ui, 0, 0, false);
            s_drawer_pressed = false;
        }
        (void)esp_gsp_drawer_close(
            ui, GSP_OBJ_KEY_QUICK_DRAWER, false);
    }
#endif
    if (leaving == GSP_BIND_P_STREAM) {
        canvas_stream_close(ui);
    }
    if (leaving == GSP_BIND_P_WHEEL) {
        for (uint8_t index = 0; index < 3; ++index) {
            if (s_wheels[index] != ESP_GSP_LIST_NONE) {
                (void)esp_gsp_list_set_total(ui, s_wheels[index], 0);
            }
        }
    }
    activate_page(ui, page);
}

static void print_summary(esp_gsp_handle_t ui)
{
    float legacy_weighted = 0;
    uint32_t total_ms = 0;
    uint64_t total_frames = 0;
    uint64_t total_busy_us = 0;
    uint64_t total_wall_us = 0;
    printf("bench: ---- summary ----\n");
    printf("bench: %-12s %9s %9s %7s %7s %7s %7s %5s\n", "page",
           "wall fps", "busy fps", "rndr ms", "subm ms", "svc us",
           "cmd/s", "sec");
    for (uint8_t index = 0; index < BENCH_PAGE_COUNT; ++index) {
        printf("bench: %-12s %9.1f %9.1f %7.1f %7.1f %7.1f %7.1f %5lu\n",
               s_pages[index].name,
               (double)s_pages[index].wall_fps,
               (double)s_pages[index].busy_fps,
               (double)s_pages[index].render_ms,
               (double)s_pages[index].submit_ms,
               (double)s_pages[index].service_us,
               (double)s_pages[index].commands_s,
               (unsigned long)(s_pages[index].dwell_ms / 1000U));
        legacy_weighted += s_pages[index].busy_fps *
                           (float)s_pages[index].dwell_ms;
        total_ms += s_pages[index].dwell_ms;
        total_frames += s_pages[index].frames;
        total_busy_us += s_pages[index].busy_us;
        total_wall_us += s_pages[index].elapsed_us;
    }
    printf("bench: aggregate wall throughput %.1f fps"
           " (observed)\n",
           total_wall_us != 0
           ? (double)total_frames * 1000000.0 /
           (double)total_wall_us
           : 0.0);
    printf("bench: aggregate active throughput %.1f fps"
           " (engine capacity)\n",
           total_busy_us != 0
           ? (double)total_frames * 1000000.0 /
           (double)total_busy_us
           : 0.0);
    printf("bench: aggregate raw frames=%llu wall_us=%llu busy_us=%llu"
           " utilization=%.1f%%\n",
           (unsigned long long)total_frames,
           (unsigned long long)total_wall_us,
           (unsigned long long)total_busy_us,
           total_wall_us != 0
           ? (double)total_busy_us * 100.0 / (double)total_wall_us
           : 0.0);
    printf("bench: legacy dwell-weighted busy score %.1f fps"
           "\n",
           total_ms != 0
           ? (double)(legacy_weighted / (float)total_ms)
           : 0.0);
    esp_gsp_region_stats_t regions;
    esp_gsp_region_stats(ui, &regions);
#define BENCH_DELTA(field_) \
    regions.field_ -= s_measured_region_start.field_
    BENCH_DELTA(plans);
    BENCH_DELTA(input_regions);
    BENCH_DELTA(output_regions);
    BENCH_DELTA(merge_candidates);
    BENCH_DELTA(merges_accepted);
    BENCH_DELTA(merges_rejected);
    BENCH_DELTA(full_promotions);
    BENCH_DELTA(input_pixels);
    BENCH_DELTA(output_pixels);
    BENCH_DELTA(estimated_before);
    BENCH_DELTA(estimated_after);
#undef BENCH_DELTA
    printf("bench: measured regions plans=%lu in=%lu out=%lu"
           " candidates=%lu merged=%lu rejected=%lu full=%lu"
           " pixels=%llu/%llu cost=%llu/%llu\n",
           (unsigned long)regions.plans,
           (unsigned long)regions.input_regions,
           (unsigned long)regions.output_regions,
           (unsigned long)regions.merge_candidates,
           (unsigned long)regions.merges_accepted,
           (unsigned long)regions.merges_rejected,
           (unsigned long)regions.full_promotions,
           (unsigned long long)regions.input_pixels,
           (unsigned long long)regions.output_pixels,
           (unsigned long long)regions.estimated_before,
           (unsigned long long)regions.estimated_after);
    esp_gsp_media_stats_t media;
    esp_gsp_media_stats(ui, &media);
#define BENCH_MEDIA_DELTA(field_) \
    media.field_ -= s_measured_media_start.field_
    BENCH_MEDIA_DELTA(decoded_images);
    BENCH_MEDIA_DELTA(decode_us);
    BENCH_MEDIA_DELTA(cache_hits);
    BENCH_MEDIA_DELTA(cache_misses);
    BENCH_MEDIA_DELTA(render_deferred);
    BENCH_MEDIA_DELTA(queue_wait_us);
    BENCH_MEDIA_DELTA(allocation_us);
    BENCH_MEDIA_DELTA(codec_decode_us);
    BENCH_MEDIA_DELTA(probe_us);
    BENCH_MEDIA_DELTA(scale_us);
    BENCH_MEDIA_DELTA(publish_us);
    BENCH_MEDIA_DELTA(qoi_decodes);
    BENCH_MEDIA_DELTA(qoi_failures);
    BENCH_MEDIA_DELTA(rle16_decodes);
    BENCH_MEDIA_DELTA(rle16_failures);
    BENCH_MEDIA_DELTA(png_decodes);
    BENCH_MEDIA_DELTA(png_failures);
    BENCH_MEDIA_DELTA(codec_failures);
    BENCH_MEDIA_DELTA(scale_hw);
    BENCH_MEDIA_DELTA(scale_sw);
    BENCH_MEDIA_DELTA(jpeg_hw_attempts);
    BENCH_MEDIA_DELTA(jpeg_hw_decodes);
    BENCH_MEDIA_DELTA(jpeg_hw_us);
    BENCH_MEDIA_DELTA(jpeg_sw_attempts);
    BENCH_MEDIA_DELTA(jpeg_sw_decodes);
    BENCH_MEDIA_DELTA(jpeg_sw_failures);
    BENCH_MEDIA_DELTA(jpeg_sw_us);
    BENCH_MEDIA_DELTA(jpeg_hw_ineligible);
    BENCH_MEDIA_DELTA(jpeg_hw_fallbacks);
    BENCH_MEDIA_DELTA(dynamic_image_requests);
    BENCH_MEDIA_DELTA(dynamic_image_queued);
    BENCH_MEDIA_DELTA(dynamic_image_published);
    BENCH_MEDIA_DELTA(dynamic_image_failures);
    BENCH_MEDIA_DELTA(dynamic_image_cancelled);
    BENCH_MEDIA_DELTA(dynamic_image_probe_us);
    BENCH_MEDIA_DELTA(animation_patches);
    BENCH_MEDIA_DELTA(animation_work_us);
    BENCH_MEDIA_DELTA(animation_patch_pixels);
    BENCH_MEDIA_DELTA(animation_copy_bytes);
    BENCH_MEDIA_DELTA(canvas_frames_applied);
    BENCH_MEDIA_DELTA(canvas_frames_rejected);
    BENCH_MEDIA_DELTA(canvas_frames_coalesced);
#undef BENCH_MEDIA_DELTA
    printf("bench: measured media decoded=%lu total=%lluus"
           " hit=%lu miss=%lu deferred=%lu\n",
           (unsigned long)media.decoded_images,
           (unsigned long long)media.decode_us,
           (unsigned long)media.cache_hits,
           (unsigned long)media.cache_misses,
           (unsigned long)media.render_deferred);
    printf("bench: media lifetime_state bytes=%lu cache_peak=%lu"
           " decode_max=%luus anim_max=%luus scratch_peak=%lu\n",
           (unsigned long)media.cache_bytes,
           (unsigned long)media.cache_peak_bytes,
           (unsigned long)media.max_decode_us,
           (unsigned long)media.max_animation_work_us,
           (unsigned long)media.animation_scratch_peak);
    printf("bench: media stages queue=%lluus/%luus"
           " alloc=%lluus/%luus codec=%lluus probe=%lluus"
           " scale=%lluus publish=%lluus/%luus\n",
           (unsigned long long)media.queue_wait_us,
           (unsigned long)media.max_queue_wait_us,
           (unsigned long long)media.allocation_us,
           (unsigned long)media.max_allocation_us,
           (unsigned long long)media.codec_decode_us,
           (unsigned long long)media.probe_us,
           (unsigned long long)media.scale_us,
           (unsigned long long)media.publish_us,
           (unsigned long)media.max_publish_us);
    printf("bench: codecs qoi=%lu/%lu rle16=%lu/%lu png=%lu/%lu"
           " failed=%lu scale_hw=%lu scale_sw=%lu\n",
           (unsigned long)media.qoi_decodes,
           (unsigned long)media.qoi_failures,
           (unsigned long)media.rle16_decodes,
           (unsigned long)media.rle16_failures,
           (unsigned long)media.png_decodes,
           (unsigned long)media.png_failures,
           (unsigned long)media.codec_failures,
           (unsigned long)media.scale_hw,
           (unsigned long)media.scale_sw);
    printf("bench: jpeg hw=%lu/%lu/%lluus sw=%lu/%lu/%lu/%lluus"
           " ineligible=%lu fallback=%lu\n",
           (unsigned long)media.jpeg_hw_attempts,
           (unsigned long)media.jpeg_hw_decodes,
           (unsigned long long)media.jpeg_hw_us,
           (unsigned long)media.jpeg_sw_attempts,
           (unsigned long)media.jpeg_sw_decodes,
           (unsigned long)media.jpeg_sw_failures,
           (unsigned long long)media.jpeg_sw_us,
           (unsigned long)media.jpeg_hw_ineligible,
           (unsigned long)media.jpeg_hw_fallbacks);
    printf("bench: dynamic image request=%lu queued=%lu published=%lu"
           " failed=%lu cancelled=%lu probe=%lluus\n",
           (unsigned long)media.dynamic_image_requests,
           (unsigned long)media.dynamic_image_queued,
           (unsigned long)media.dynamic_image_published,
           (unsigned long)media.dynamic_image_failures,
           (unsigned long)media.dynamic_image_cancelled,
           (unsigned long long)media.dynamic_image_probe_us);
    printf("bench: dynamic qoi accepted=%lu errors=%lu\n",
           (unsigned long)s_dynamic_qoi_accepted,
           (unsigned long)s_dynamic_qoi_errors);
    printf("bench: anim patches=%lu total=%lluus max=%luus"
           " pixels=%llu copy=%llu scratch_peak=%lu\n",
           (unsigned long)media.animation_patches,
           (unsigned long long)media.animation_work_us,
           (unsigned long)media.max_animation_work_us,
           (unsigned long long)media.animation_patch_pixels,
           (unsigned long long)media.animation_copy_bytes,
           (unsigned long)media.animation_scratch_peak);
    uint64_t canvas_full_pixels =
        (uint64_t)s_canvas_measured_frames * BENCH_CANVAS_W * BENCH_CANVAS_H;
    printf("bench: canvas frames=%lu dirty=%llu full=%llu ratio=%.1f%%\n",
           (unsigned long)s_canvas_measured_frames,
           (unsigned long long)s_canvas_measured_dirty_pixels,
           (unsigned long long)canvas_full_pixels,
           canvas_full_pixels != 0 ?
           (double)s_canvas_measured_dirty_pixels * 100.0 /
           (double)canvas_full_pixels : 0.0);
    printf("bench: canvas pushes applied=%lu rejected=%lu coalesced=%lu\n",
           (unsigned long)media.canvas_frames_applied,
           (unsigned long)media.canvas_frames_rejected,
           (unsigned long)media.canvas_frames_coalesced);
}

static void page_scheduler(esp_gsp_handle_t ui, void *user_ctx)
{
    (void)user_ctx;
    if (s_page_waiting_for_scene) {
        s_page_waiting_for_scene = false;
        activate_page(ui, s_pending_page);
        return;
    }
    int64_t now_us = esp_timer_get_time();
    int64_t elapsed_us = now_us - s_page_start_us;
    if (elapsed_us < (int64_t)s_pages[s_page].dwell_ms * 1000) {
        return;
    }
    if (benchmark_drag_page(s_pages[s_page].bind) && !s_drag_settled) {
        /* Finish the in-flight semantic gesture before closing its sample.
         * A timer boundary may otherwise lift a partial cancel/flick and
         * accidentally commit it after the result line was printed. */
        s_drag_finishing = true;
        return;
    }
    uint32_t frames_now;
    uint64_t busy_now;
    esp_gsp_render_stats(ui, &frames_now, &busy_now);
    uint32_t frames = frames_now - s_page_start_frames;
    uint64_t busy_us = busy_now - s_page_start_busy_us;
    sample_page_memory();
    s_pages[s_page].frames = frames;
    s_pages[s_page].busy_us = busy_us;
    s_pages[s_page].elapsed_us = (uint64_t)elapsed_us;
    s_pages[s_page].busy_ms = frames != 0
                              ? (float)((double)busy_us / 1e3 / frames) : 0;
    s_pages[s_page].internal_peak_bytes =
        s_page_internal_start > s_page_internal_min
        ? (uint32_t)(s_page_internal_start - s_page_internal_min) : 0;
    s_pages[s_page].psram_peak_bytes =
        s_page_psram_start > s_page_psram_min
        ? (uint32_t)(s_page_psram_start - s_page_psram_min) : 0;
    s_pages[s_page].wall_fps = (float)((double)frames * 1e6 /
                                       (double)elapsed_us);
    s_pages[s_page].busy_fps = busy_us != 0 ?
                               (float)((double)frames * 1e6 / (double)busy_us) : 0;
    uint64_t render_us;
    uint64_t submit_us;
    esp_gsp_render_phases(ui, &render_us, &submit_us);
    s_pages[s_page].render_us = render_us - s_page_start_render_us;
    s_pages[s_page].submit_us = submit_us - s_page_start_submit_us;
    if (frames != 0) {
        s_pages[s_page].render_ms = (float)(
                                        (double)s_pages[s_page].render_us / 1e3 / frames);
        s_pages[s_page].submit_ms = (float)(
                                        (double)s_pages[s_page].submit_us / 1e3 / frames);
    }
    uint32_t service_iterations;
    uint64_t service_us;
    uint32_t service_commands;
    esp_gsp_service_stats(ui, &service_iterations, &service_us,
                          &service_commands);
    uint32_t service_steps = service_iterations -
                             s_page_start_service_iterations;
    s_pages[s_page].service_iterations = service_steps;
    s_pages[s_page].service_us_total =
        service_us - s_page_start_service_us;
    s_pages[s_page].service_commands =
        service_commands - s_page_start_service_commands;
    s_pages[s_page].service_us = service_steps != 0 ?
                                 (float)((double)s_pages[s_page].service_us_total /
                                         service_steps) : 0;
    s_pages[s_page].commands_s = (float)(
                                     (double)s_pages[s_page].service_commands * 1e6 /
                                     (double)elapsed_us);
    printf("bench: detail[%s] frames=%lu elapsed_us=%llu busy_us=%llu"
           " render_us=%llu submit_us=%llu service_steps=%lu"
           " service_us=%llu commands=%lu"
           " busy=%.3fms"
           " internal_peak=%lu psram_peak=%lu\n",
           s_pages[s_page].name, (unsigned long)s_pages[s_page].frames,
           (unsigned long long)s_pages[s_page].elapsed_us,
           (unsigned long long)s_pages[s_page].busy_us,
           (unsigned long long)s_pages[s_page].render_us,
           (unsigned long long)s_pages[s_page].submit_us,
           (unsigned long)s_pages[s_page].service_iterations,
           (unsigned long long)s_pages[s_page].service_us_total,
           (unsigned long)s_pages[s_page].service_commands,
           (double)s_pages[s_page].busy_ms,
           (unsigned long)s_pages[s_page].internal_peak_bytes,
           (unsigned long)s_pages[s_page].psram_peak_bytes);
    if (benchmark_transition_page(s_pages[s_page].bind)) {
        const bench_page_t *result = &s_pages[s_page];
        esp_gsp_transition_stats_t paths;
        esp_gsp_transition_stats(ui, &paths);
        paths.snapshot -= s_page_start_transition_stats.snapshot;
        paths.direct -= s_page_start_transition_stats.direct;
        paths.inplace -= s_page_start_transition_stats.inplace;
        paths.streamed -= s_page_start_transition_stats.streamed;
        paths.failures -= s_page_start_transition_stats.failures;
        double average_us = result->transition_count != 0
                            ? (double)result->transition_total_us /
                            (double)result->transition_count : 0.0;
        double average_frames = result->transition_count != 0
                                ? (double)result->transition_frames /
                                (double)result->transition_count : 0.0;
        printf("bench: transition[%s] count=%lu errors=%lu no_visual=%lu"
               " frames=%lu frames_avg=%.1f latency_avg=%.1fus"
               " p50=%lluus p95=%lluus max=%lluus"
               " snapshot=%lu direct=%lu inplace=%lu streamed=%lu"
               " path_failures=%lu\n",
               result->name,
               (unsigned long)result->transition_count,
               (unsigned long)result->transition_errors,
               (unsigned long)result->transition_no_visual,
               (unsigned long)result->transition_frames,
               average_frames, average_us,
               (unsigned long long)transition_percentile(result, 50),
               (unsigned long long)transition_percentile(result, 95),
               (unsigned long long)result->transition_max_us,
               (unsigned long)paths.snapshot,
               (unsigned long)paths.direct,
               (unsigned long)paths.inplace,
               (unsigned long)paths.streamed,
               (unsigned long)paths.failures);
    }
#if BENCH_ENABLE_DRAWER
    if (s_pages[s_page].bind == GSP_BIND_P_DRAWER) {
        printf("bench: drawer opens=%lu closes=%lu errors=%lu\n",
               (unsigned long)s_drawer_opens,
               (unsigned long)s_drawer_closes,
               (unsigned long)s_drawer_errors);
    }
#endif
    if (s_pages[s_page].bind == GSP_BIND_P_WHEEL) {
        printf("bench: wheel command_errors=%lu\n",
               (unsigned long)s_wheel_command_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_GRID) {
        printf("bench: grid binds=%lu drags=%lu errors=%lu\n",
               (unsigned long)s_grid_binds,
               (unsigned long)s_grid_drags,
               (unsigned long)s_grid_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_MESSAGES) {
        printf("bench: messages count=%lu first=%lu errors=%lu\n",
               (unsigned long)s_message_count,
               (unsigned long)s_message_first,
               (unsigned long)s_message_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_STATIC_MOVE) {
        printf("bench: static_move period_ms=%u target_hz=%.1f controls=4"
               " commands=%lu errors=%lu\n",
               BENCH_STRESS_PERIOD_MS, BENCH_STRESS_TARGET_HZ,
               (unsigned long)s_static_move_commands,
               (unsigned long)s_static_move_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_MOVE) {
        printf("bench: move period_ms=%u target_hz=%.1f controls=4"
               " commands=%lu errors=%lu\n",
               BENCH_STRESS_PERIOD_MS, BENCH_STRESS_TARGET_HZ,
               (unsigned long)s_move_commands,
               (unsigned long)s_move_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_CLOCK) {
        printf("bench: clock period_ms=%u target_hz=%.1f updates=%lu"
               " commands=%lu errors=%lu\n",
               BENCH_FRAME_PERIOD_MS, BENCH_FRAME_TARGET_HZ,
               (unsigned long)s_clock_updates,
               (unsigned long)s_clock_commands,
               (unsigned long)s_clock_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_SCALE) {
        printf("bench: image scale updates=%lu commands=%lu errors=%lu\n",
               (unsigned long)s_image_scale_updates,
               (unsigned long)s_image_scale_commands,
               (unsigned long)s_image_scale_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_FLOW ||
            s_pages[s_page].bind == GSP_BIND_P_STACK ||
            s_pages[s_page].bind == GSP_BIND_P_COMPOSITES) {
        printf("bench: components commands=%lu errors=%lu\n",
               (unsigned long)s_component_commands,
               (unsigned long)s_component_errors);
    }
    if (s_pages[s_page].bind == GSP_BIND_P_COMPOSITES) {
        char final_text[BENCH_KEYBOARD_TEXT_CAPACITY];
        if (esp_gsp_keyboard_text(
                    ui, final_text, sizeof(final_text)) != ESP_GSP_OK) {
            final_text[0] = '\0';
            ++s_component_errors;
        }
        printf("bench: keyboard presses=%lu text_updates=%lu backspaces=%lu"
               " final=\"%s\"\n",
               (unsigned long)s_keyboard_key_presses,
               (unsigned long)s_keyboard_text_updates,
               (unsigned long)s_keyboard_backspaces, final_text);
    }
#ifdef GSP_BENCH_DIAGNOSTICS
    {
        /* Per-opcode hot spots for this page (top 3 by time). */
        uint64_t ops[GSP_RENDERER_PROFILE_OPCODES];
        gsp_renderer_profile_snapshot(ops, NULL,
                                      GSP_RENDERER_PROFILE_OPCODES);
        for (size_t op = 0; op < GSP_RENDERER_PROFILE_OPCODES; ++op) {
            ops[op] -= s_page_start_ops[op];
        }
        printf("bench: ops[%s]", s_pages[s_page].name);
        for (int rank = 0; rank < 3; ++rank) {
            size_t best = 0;
            for (size_t op = 1;
                    op < GSP_RENDERER_PROFILE_OPCODES; ++op) {
                if (ops[op] > ops[best]) {
                    best = op;
                }
            }
            if (ops[best] == 0) {
                break;
            }
            printf(" op%u=%lums", (unsigned)best,
                   (unsigned long)(ops[best] / 1000U));
            ops[best] = 0;
        }
        printf("\n");
    }
#endif
    if (s_pages[s_page].bind == GSP_BIND_P_QOI) {
        printf("bench: dynamic qoi accepted=%lu errors=%lu\n",
               (unsigned long)s_dynamic_qoi_accepted,
               (unsigned long)s_dynamic_qoi_errors);
    }
    uint8_t next = s_page + 1U;
    if (next == BENCH_PAGE_COUNT) {
        static bool warmed_up;
        if (warmed_up) {
            print_summary(ui);
        } else {
            /* First lap is the warm-up (boot bursts, cache fill),
             * mirroring lv_demo_benchmark. */
            warmed_up = true;
            s_canvas_measured_frames = 0;
            s_canvas_measured_dirty_pixels = 0;
            esp_gsp_region_stats(ui, &s_measured_region_start);
            esp_gsp_media_stats(ui, &s_measured_media_start);
            s_dynamic_qoi_accepted = 0;
            s_dynamic_qoi_errors = 0;
            printf("bench: warm-up lap done, measuring\n");
        }
        next = 0;
    }
    show_page(ui, next);
}

/* Slow (1 s) retriggered tweens: the pages whose motion IS a value or
 * color ramp. */
static void drive_tweens(esp_gsp_handle_t ui, void *user_ctx)
{
    (void)user_ctx;
    if (s_page_waiting_for_scene) {
        return;
    }
    bench_workload_drive_tweens(ui, &s_bench_workload);
}

/* High-rate effects run from the 4 ms saturation driver. Gesture sampling
 * remains on the separate 16 ms semantic driver. */
static void drive_fx(esp_gsp_handle_t ui, void *user_ctx)
{
    (void)user_ctx;
    if (s_page_waiting_for_scene) {
        return;
    }
    uint32_t tick = ++s_frame_tick;
    uint16_t page = s_pages[s_page].bind;
    if (s_modal_cleanup_pending &&
            (page != GSP_BIND_P_COMPOSITES || s_composite_tab != 2U)) {
        esp_gsp_err_t ret =
            gsp_bench_bench_modal_set_visible(ui, false);
        if (ret == ESP_GSP_OK) {
            s_modal_cleanup_pending = false;
        }
    }
    if (page == GSP_BIND_P_GRAD) {
        static const uint16_t bands[] = {GSP_BIND_GV0, GSP_BIND_GV1,
                                         GSP_BIND_GV2, GSP_BIND_GV3
                                        };
        for (uint8_t index = 0;
                index < sizeof(bands) / sizeof(bands[0]); ++index) {
            (void)esp_gsp_set_visible(ui, bands[index],
                                      ((tick + index) % 3) != 0);
        }
    } else if (page == GSP_BIND_P_IMGRGB) {
        static const uint16_t tiles[] = {
            GSP_BIND_IV0, GSP_BIND_IV1, GSP_BIND_IV2, GSP_BIND_IV3,
            GSP_BIND_IV4,
        };
        for (uint8_t index = 0; index < 5; ++index) {
            (void)esp_gsp_set_visible(ui, tiles[index],
                                      ((tick + index) & 3U) != 0);
        }
    } else if (page == GSP_BIND_P_QOI) {
        enum {
            DYNAMIC_QOI_W = 80,
            DYNAMIC_QOI_H = 60,
        };
        static uint8_t dynamic_qoi[512];
        size_t dynamic_size = make_solid_qoi(
                                  dynamic_qoi, DYNAMIC_QOI_W, DYNAMIC_QOI_H,
                                  (tick & 1U) ? 230 : 20, 40, (tick & 1U) ? 20 : 230);
        /* Replace one visible QOI tile so background decode, resize, cache
         * publication and repaint are all covered end to end. */
        esp_err_t image_ret = esp_gsp_set_image(
                                  ui, GSP_BIND_DYN0, dynamic_qoi, dynamic_size);
        if (image_ret == ESP_OK) {
            ++s_dynamic_qoi_accepted;
        } else {
            ++s_dynamic_qoi_errors;
        }
        /* qv0 owns the dynamic top-left image. Keep it visible so cache
         * publication can be judged independently from the visibility
         * stress; the other four layers still sweep every frame. */
        static const uint16_t tiles[] = {
            GSP_BIND_QV1, GSP_BIND_QV2, GSP_BIND_QV3, GSP_BIND_QV4,
        };
        for (uint8_t index = 0; index < 4; ++index) {
            (void)esp_gsp_set_visible(ui, tiles[index],
                                      ((tick + index) & 3U) != 0);
        }
    } else if (page == GSP_BIND_P_IMGARGB) {
        static const uint16_t tiles[] = {
            GSP_BIND_AV0, GSP_BIND_AV1, GSP_BIND_AV2, GSP_BIND_AV3,
            GSP_BIND_AV4,
        };
        for (uint8_t index = 0; index < 5; ++index) {
            (void)esp_gsp_set_visible(ui, tiles[index],
                                      ((tick + index) & 3U) != 0);
        }
    } else if (page == GSP_BIND_P_GRID) {
        enum {
            GRID_MOVE_STEPS = 30,
            GRID_CYCLE_STEPS = 160,
        };
        uint16_t phase = s_grid_tick++ % GRID_CYCLE_STEPS;
        uint32_t cycle = s_grid_tick / GRID_CYCLE_STEPS;
        int16_t margin = BENCH_W / 12;
        int16_t inner = BENCH_W - 2 * margin;
        int16_t gap = (BENCH_W < BENCH_H ? BENCH_W : BENCH_H) / 60;
        if (gap < 3) {
            gap = 3;
        }
        int16_t cell = (inner - 2 * gap) / 3;
        int16_t top = margin + cell / 2;
        int16_t bottom = BENCH_H - margin - cell / 2;
        bool reverse = (cycle & 1U) != 0U;
        int16_t start = reverse ? top : bottom;
        int16_t end = reverse ? bottom : top;
        if (phase <= GRID_MOVE_STEPS) {
            int16_t y = (int16_t)(start +
                                  (int32_t)(end - start) * phase /
                                  GRID_MOVE_STEPS);
            esp_gsp_err_t ret = esp_gsp_inject_touch(
                                    ui, margin + cell / 2, y, true);
            if (ret != ESP_GSP_OK) {
                ++s_grid_errors;
            } else {
                s_grid_pressed = true;
            }
        } else if (phase == GRID_MOVE_STEPS + 1U) {
            esp_gsp_err_t ret = esp_gsp_inject_touch(ui, 0, 0, false);
            if (ret != ESP_GSP_OK) {
                ++s_grid_errors;
            } else {
                ++s_grid_drags;
            }
            s_grid_pressed = false;
        }
    } else if (page == GSP_BIND_P_COMPOSITES) {
        /* One second per tab gives the real keyboard enough time for a
         * visible press/release typing sequence. The direct text setter is
         * deliberately not used here: characters must travel through hit
         * testing, key CALL actions, the keyboard edit buffer and repaint. */
        uint16_t tab = (uint16_t)((tick / 400U) % 3U);
        esp_gsp_err_t ret = ESP_GSP_OK;
        if ((tick % 400U) == 0U) {
            if (s_keyboard_pressed) {
                (void)esp_gsp_inject_touch(ui, 0, 0, false);
                s_keyboard_pressed = false;
            }
            s_composite_tab = tab;
            ret = esp_gsp_page_flow_set_page(
                      ui, GSP_OBJ_KEY_BENCH_TABS, s_composite_tab, true);
            ++s_component_commands;
            if (ret != ESP_GSP_OK) {
                ++s_component_errors;
            }
            if (s_composite_tab == 1U) {
                s_keyboard_step = 0;
                (void)esp_gsp_set_text(ui, GSP_BIND_KT0, "");
                ret = esp_gsp_keyboard_attach(
                          ui, GSP_ACT_ID_BENCH_KEYBOARD_KEY, GSP_BIND_KT0);
                ++s_component_commands;
                if (ret != ESP_GSP_OK) {
                    ++s_component_errors;
                }
            }
        }
        if ((tick % 32U) == 0U) {
            ret = gsp_bench_bench_modal_set_visible(
                      ui, s_composite_tab == 2U &&
                      ((tick / 32U) & 1U) != 0U);
            ++s_component_commands;
            if (ret != ESP_GSP_OK) {
                ++s_component_errors;
                if (s_composite_tab != 2U) {
                    s_modal_cleanup_pending = true;
                }
            } else if (s_composite_tab != 2U) {
                s_modal_cleanup_pending = false;
            }
        }
    } else if (page == GSP_BIND_P_FLOW) {
        if ((tick % 100U) == 0U) {
            esp_gsp_err_t ret = esp_gsp_page_flow_set_page(
                                    ui, GSP_OBJ_KEY_BENCH_FLOW,
                                    (uint16_t)((tick / 100U) % 3U), true);
            ++s_component_commands;
            if (ret != ESP_GSP_OK) {
                ++s_component_errors;
            }
        }
    } else if (page == GSP_BIND_P_STACK) {
        bool animating = false;
        uint16_t top = 0;
        esp_gsp_err_t ret = esp_gsp_stack_view_is_animating(
                                ui, GSP_OBJ_KEY_BENCH_STACK, &animating);
        if (ret == ESP_GSP_OK && !animating) {
            ret = esp_gsp_stack_view_get_top(
                      ui, GSP_OBJ_KEY_BENCH_STACK, &top);
            if (ret == ESP_GSP_OK) {
                ret = top < 2U
                      ? esp_gsp_stack_view_push(
                          ui, GSP_OBJ_KEY_BENCH_STACK,
                          (uint16_t)(top + 1U), true)
                      : esp_gsp_stack_view_pop(
                          ui, GSP_OBJ_KEY_BENCH_STACK, true);
            }
            ++s_component_commands;
        }
        if (ret != ESP_GSP_OK) {
            ++s_component_errors;
        }
    } else if (page == GSP_BIND_P_WIDGETS) {
        /* Frantic tapping through the real input pipeline: press one frame,
         * lift the next, cycling button -> toggle -> checkbox -> radio.
         * Geometry mirrors gen_scenes.py p_widgets. */
        const int16_t m = BENCH_W / 12;
        const int16_t inner = BENCH_W - 2 * m;
        const int16_t row = BENCH_H / 14 > 14 ? BENCH_H / 14 : 14;
        const int16_t gap = BENCH_H / 26 > 6 ? BENCH_H / 26 : 6;
        const int16_t col = inner / 2 - gap;
        int16_t tog_w = col / 3 > row * 2 ? col / 3 : row * 2;
        if (tog_w < 40) {
            tog_w = 40;
        }
        const int16_t wy = BENCH_H / 16;
        const int16_t y2 = wy + row + 2 * gap;
        /* Row walk mirrors gen_scenes: checkbox row, chart row, wheel
         * row — the dropdown sits right of the wheel. Tapping it
         * toggles the option panel open/closed every visit. */
        const int16_t y_dd = (int16_t)(y2 + row + 2 * gap +
                                       BENCH_H / 5 + gap);
        const int16_t dd_h = BENCH_H / 12 > 22 ? BENCH_H / 12 : 22;
        const int16_t taps[5][2] = {
            {(int16_t)(m + col / 2), (int16_t)(wy + (row + 6) / 2)},
            {
                (int16_t)(m + col + gap + tog_w / 2),
                (int16_t)(wy + row / 2)
            },
            {(int16_t)(m + row / 2), (int16_t)(y2 + row / 2)},
            {(int16_t)(m + 2 * row + row / 2), (int16_t)(y2 + row / 2)},
            {
                (int16_t)(m + col + gap + col / 2),
                (int16_t)(y_dd + dd_h / 2)
            },
        };
        uint8_t target = (uint8_t)((tick >> 1) % 5U);
        (void)esp_gsp_inject_touch(ui, taps[target][0],
                                   taps[target][1], (tick & 1U) == 0U);
        (void)esp_gsp_set_value(ui, GSP_BIND_W_TOG, (tick >> 1) & 1U);
        (void)esp_gsp_set_value(ui, GSP_BIND_W_CHK, (tick >> 2) & 1U);
        (void)esp_gsp_set_value(ui, GSP_BIND_W_RAD, (tick >> 1) & 1U);
    } else if (benchmark_programmatic_transition_page(page)) {
        if (tick == 1U || (tick % 50U) == 0U) {
            s_transition_alt = !s_transition_alt;
            uint16_t target = s_transition_alt ? 1U : 0U;
            if (transition_measure_begin(ui, target)) {
                esp_gsp_err_t ret = esp_gsp_goto_scene(
                                        ui, target, benchmark_transition_kind(page));
                if (ret != ESP_GSP_OK) {
                    transition_measure_abort();
                }
            }
        }
    } else if (page == GSP_BIND_P_MESSAGES) {
        uint16_t phase = s_message_tick++ % 1200U;
        if (phase == 120U && s_message_first >= 4U) {
            s_message_first -= 4U;
            s_message_count += 4U;
            if (gsp_bench_bench_messages_changed(
                        ui, s_messages, 4) != ESP_GSP_OK) {
                ++s_message_errors;
            }
        } else if (phase == 420U &&
                   s_message_first + s_message_count < 48U) {
            ++s_message_count;
            if (gsp_bench_bench_messages_changed(
                        ui, s_messages, 0) != ESP_GSP_OK) {
                ++s_message_errors;
            }
        } else if ((phase == 0U || phase == 700U) &&
                   s_messages != ESP_GSP_LIST_NONE) {
            if (esp_gsp_list_fling(
                        ui, s_messages, phase == 0U ? -1800 : 1800) !=
                    ESP_GSP_OK) {
                ++s_message_errors;
            }
        }
    } else if (page == GSP_BIND_P_WHEEL) {
        /* Staggered flings retain an approximately 1.8 s relaunch period while
         * the wheel physics itself advances from the high-rate frame loop. */
        uint16_t phase = s_wheel_tick++ % 450U;
        for (uint8_t index = 0; index < 3; ++index) {
            if (phase == index * 150U &&
                    s_wheels[index] != ESP_GSP_LIST_NONE) {
                int32_t velocity = 2000 + 900 * index;
                wheel_record(esp_gsp_list_fling(
                                 ui, s_wheels[index],
                                 (((s_wheel_tick - 1U) / 450U + index) & 1U)
                                 ? -velocity : velocity));
            }
        }
    } else if (page == GSP_BIND_P_RECTS) {
        s_rect_load = (s_rect_load + 1) % 101;
        (void)esp_gsp_set_value(ui, GSP_BIND_LOAD, s_rect_load);
    }
}

static void drive_static_movers(esp_gsp_handle_t ui)
{
    enum {
        STATIC_MOVER_W = BENCH_W / 2 < 120 ? BENCH_W / 2 : 120,
        STATIC_MOVER_H = BENCH_H / 2 < 100 ? BENCH_H / 2 : 100,
    };
    uint32_t tick = ++s_static_move_tick;
    for (uint8_t index = 0; index < 4; ++index) {
        int32_t x = bench_bounce_position(
                        tick * (2U + index) + index * (BENCH_W / 4U),
                        BENCH_W, STATIC_MOVER_W);
        int32_t y = bench_bounce_position(
                        tick * (1U + index) + index * (BENCH_H / 5U),
                        BENCH_H, STATIC_MOVER_H);
        esp_err_t ret;
        switch (index) {
        case 0:
            ret = gsp_bench_static_mover0_set_position(ui, x, y);
            break;
        case 1:
            ret = gsp_bench_static_mover1_set_position(ui, x, y);
            break;
        case 2:
            ret = gsp_bench_static_mover2_set_position(ui, x, y);
            break;
        default:
            ret = gsp_bench_static_mover3_set_position(ui, x, y);
            break;
        }
        ++s_static_move_commands;
        if (ret != ESP_OK) {
            ++s_static_move_errors;
        }
    }
}

static void drive_movers(esp_gsp_handle_t ui)
{
    uint32_t tick = ++s_move_tick;
    for (uint8_t index = 0; index < s_mover_count; ++index) {
        uint32_t phase = bench_move_phase(tick, index);
        int16_t x = bench_bounce_position(
                        phase + index * (BENCH_W / 4U), BENCH_W, 100);
        int16_t y = bench_bounce_position(
                        phase * 2U / 3U + index * (BENCH_H / 5U), BENCH_H, 100);
        esp_err_t ret = esp_gsp_widget_set_position(
                            ui, s_movers[index], x, y);
        ++s_move_commands;
        if (ret != ESP_OK) {
            ++s_move_errors;
        }
    }
}

static void drive_keyboard(esp_gsp_handle_t ui)
{
    typedef struct {
        uint16_t x_permille;
        uint8_t row;
    } key_position_t;
    /* "gsp benchmark", DEL, "k": the final two keys visibly exercise
     * backspace and correction through the actual keyboard component. */
    static const key_position_t sequence[] = {
        {500, 1}, {167, 1}, {950, 0}, {500, 3},
        {600, 2}, {250, 0}, {700, 2}, {400, 2}, {611, 1},
        {800, 2}, {56, 1}, {350, 0}, {833, 1},
        {925, 2}, {833, 1},
    };
    if (s_pages[s_page].bind != GSP_BIND_P_COMPOSITES ||
            s_composite_tab != 1U || s_page_waiting_for_scene) {
        return;
    }
    /* PageFlow's animated tab translation can otherwise move the first hit
     * underneath an adjacent key. Wait 320 ms (20 semantic frames), then
     * keep typing for the remainder of the 1.6 s Input-tab dwell. */
    if (s_keyboard_step < 20U) {
        ++s_keyboard_step;
        return;
    }
    uint16_t event = (uint16_t)(s_keyboard_step - 20U);
    size_t key_index = (event / 2U) %
                       (sizeof(sequence) / sizeof(sequence[0]));
    bool pressed = (event & 1U) == 0U;
    if (pressed) {
        char current[BENCH_KEYBOARD_TEXT_CAPACITY];
        if (esp_gsp_keyboard_text(
                    ui, current, sizeof(current)) == ESP_GSP_OK &&
                strcmp(current, s_keyboard_last_text) != 0) {
            ++s_keyboard_text_updates;
            strncpy(s_keyboard_last_text, current,
                    sizeof(s_keyboard_last_text) - 1U);
            s_keyboard_last_text[sizeof(s_keyboard_last_text) - 1U] = '\0';
        }
    }
    int16_t tabs_y = BENCH_H / 18 > 18 ? BENCH_H / 18 : 18;
    int16_t tabs_h = BENCH_H - tabs_y;
    int16_t bar_h = BENCH_H / 9 > 26 ? BENCH_H / 9 : 26;
    int16_t content_h = tabs_h - bar_h;
    int16_t keyboard_h = content_h * 3 / 4 > 80
                         ? content_h * 3 / 4 : 80;
    int16_t keyboard_y = tabs_y + bar_h +
                         (content_h / 5 > 20 ? content_h / 5 : 20);
    int16_t key_gap = BENCH_W / 128 > 2 ? BENCH_W / 128 : 2;
    int16_t key_h = (keyboard_h - 3 * key_gap) / 4;
    int16_t x = (int16_t)(
                    sequence[key_index].x_permille * BENCH_W / 1000U);
    int16_t y = (int16_t)(keyboard_y +
                          sequence[key_index].row * (key_h + key_gap) + key_h / 2);
    esp_gsp_err_t ret = esp_gsp_inject_touch(ui, x, y, pressed);
    ++s_component_commands;
    if (ret != ESP_GSP_OK) {
        ++s_component_errors;
    } else if (pressed) {
        ++s_keyboard_key_presses;
        if (key_index == 13U) {
            ++s_keyboard_backspaces;
        }
    }
    s_keyboard_pressed = pressed;
    ++s_keyboard_step;
}

static void drive_drag(esp_gsp_handle_t ui, void *user_ctx)
{
    enum {
        MOVE_STEPS = 20,
        CANCEL_MOVE_STEPS = 25,
        CANCEL_HOLD_STEPS = 1,
        CYCLE_STEPS = 30,
        CANCEL_CYCLE_STEPS = 45,
        EDGE_MARGIN = 4,
    };
    (void)user_ctx;
    if (s_page_waiting_for_scene) {
        return;
    }
    if (s_pages[s_page].bind == GSP_BIND_P_STATIC_MOVE) {
        drive_static_movers(ui);
        sample_page_memory();
        return;
    }
    if (s_pages[s_page].bind == GSP_BIND_P_MOVE) {
        drive_movers(ui);
        sample_page_memory();
        return;
    }
    uint16_t workload = s_pages[s_page].bind;
    if (!benchmark_drag_page(workload)) {
        return;
    }
    uint16_t cycle_steps = workload == BENCH_PAGE_DRAG_CANCEL
                           ? CANCEL_CYCLE_STEPS : CYCLE_STEPS;
    if (s_drag_finishing && (s_drag_tick % cycle_steps) == 0U) {
        if (!s_drag_pressed && !s_transition_measure.active) {
            s_drag_settled = true;
        }
        sample_page_memory();
        return;                 /* do not start another cycle */
    }
    uint16_t phase = (uint16_t)(s_drag_tick++ % cycle_steps);
    int32_t start_x = s_drag_left ? BENCH_W - EDGE_MARGIN : EDGE_MARGIN;
    int32_t y = BENCH_H / 2;
    if (phase == 0) {
        if (workload == BENCH_PAGE_DRAG_FADE) {
            uint16_t expected_scene = s_transition_alt ? 0U : 1U;
            (void)transition_measure_begin(ui, expected_scene);
        }
        (void)esp_gsp_inject_touch(ui, start_x, y, true);
        s_drag_pressed = true;
        return;
    }

    uint32_t distance = 0;
    uint16_t release_phase = 0;
    if (workload == BENCH_PAGE_DRAG_COMMIT ||
            workload == BENCH_PAGE_DRAG_FADE) {
        uint32_t full = BENCH_W - 2U * EDGE_MARGIN;
        distance = full;
        if (phase <= MOVE_STEPS) {
            distance = full * phase / MOVE_STEPS;
        }
        release_phase = MOVE_STEPS + 1U;
    } else if (workload == BENCH_PAGE_DRAG_CANCEL) {
        /* Move one fifth of the viewport over 400 ms. This stays below the
         * fling threshold and isolates the distance-cancel behavior. */
        uint32_t partial = BENCH_W / 5U;
        if (phase <= CANCEL_MOVE_STEPS) {
            distance = partial * phase / CANCEL_MOVE_STEPS;
        } else {
            distance = partial;
        }
        release_phase = CANCEL_MOVE_STEPS + CANCEL_HOLD_STEPS + 1U;
    } else {
        distance = phase == 1U ? BENCH_W / 3U
                   : BENCH_W * 2U / 5U;
        release_phase = 3U;
    }
    int32_t x = s_drag_left ? start_x - (int32_t)distance
                : start_x + (int32_t)distance;
    if (phase < release_phase) {
        (void)esp_gsp_inject_touch(ui, x, y, true);
    } else if (phase == release_phase) {
        bool cancel = workload == BENCH_PAGE_DRAG_CANCEL;
        uint16_t expected_scene = cancel
                                  ? (s_transition_alt ? 1U : 0U)
                                  : (s_transition_alt ? 0U : 1U);
        bool measuring = workload == BENCH_PAGE_DRAG_FADE
                         ? s_transition_measure.active
                         : transition_measure_begin(ui, expected_scene);
        esp_gsp_err_t ret = esp_gsp_inject_touch(ui, x, y, false);
        s_drag_pressed = false;
        if (ret != ESP_GSP_OK) {
            transition_measure_abort();
        } else if (cancel && measuring && s_transition_measure.active) {
            transition_record(ui, s_transition_measure.page,
                              s_transition_measure.start_us,
                              s_transition_measure.start_frames);
            s_transition_measure.active = false;
        }
        s_drag_left = !s_drag_left;
    }
    sample_page_memory();
}

static void drive_text(esp_gsp_handle_t ui, void *user_ctx)
{
    static const uint16_t binds[] = {GSP_BIND_T0, GSP_BIND_T1,
                                     GSP_BIND_T2, GSP_BIND_T3
                                    };
    (void)user_ctx;
    if (s_page_waiting_for_scene) {
        return;
    }
    ++s_text_frame;
    if (s_pages[s_page].bind == GSP_BIND_P_STREAM) {
        canvas_stream_tick(ui);
        return;
    }
    if (s_pages[s_page].bind != GSP_BIND_P_TEXT) {
        return;
    }
    char text[48];
    for (uint8_t index = 0; index < 4; ++index) {
        snprintf(text, sizeof(text), "glyph run %lu / lane %u",
                 (unsigned long)s_text_frame, (unsigned)index);
        (void)esp_gsp_set_text(ui, binds[index], text);
    }
}

static void drive_scroll(esp_gsp_handle_t ui, void *user_ctx)
{
    static const uint16_t binds[] = {
        GSP_BIND_S0, GSP_BIND_S1, GSP_BIND_S2, GSP_BIND_S3,
        GSP_BIND_S4, GSP_BIND_S5, GSP_BIND_S6, GSP_BIND_S7,
    };
    (void)user_ctx;
    if (s_page_waiting_for_scene) {
        return;
    }
    ++s_scroll_offset;
    if (s_pages[s_page].bind != GSP_BIND_P_SCROLL) {
        return;
    }
    size_t pool = sizeof(s_scroll_pool) / sizeof(s_scroll_pool[0]);
    for (uint8_t index = 0; index < 8; ++index) {
        (void)esp_gsp_set_text(ui, binds[index],
                               s_scroll_pool[(s_scroll_offset + index) % pool]);
    }
}

static void drive_drawer(esp_gsp_handle_t ui)
{
#if BENCH_ENABLE_DRAWER
    enum {
        DRAWER_CYCLE_TICKS = 49,
        DRAWER_CYCLES = 3,
        DRAWER_DRAG_STEPS = 8,
        DRAWER_OPEN_CHECK_TICK = 20,
        DRAWER_CLOSE_START_TICK = 32,
        DRAWER_CLOSE_RELEASE_TICK =
            DRAWER_CLOSE_START_TICK + DRAWER_DRAG_STEPS + 1,
        DRAWER_CLOSE_CHECK_TICK = 47,
    };
    if (s_pages[s_page].bind != GSP_BIND_P_DRAWER ||
            s_page_waiting_for_scene) {
        return;
    }
    if (s_drawer_tick >= DRAWER_CYCLE_TICKS * DRAWER_CYCLES) {
        return;
    }
    uint16_t phase = s_drawer_tick++ % DRAWER_CYCLE_TICKS;
    const int16_t pad = BENCH_W / 14 > 8 ? BENCH_W / 14 : 8;
    const int16_t gap = BENCH_H / 32 > 6 ? BENCH_H / 32 : 6;
    const int16_t col = (BENCH_W - 2 * pad - gap) / 2;
    const int16_t card_h =
        BENCH_H * 2 / 5 > 52 ? BENCH_H * 2 / 5 : 52;
    const int16_t rows_y = pad + card_h + gap;
    const int16_t row_h = (BENCH_H - rows_y - 2 * pad - gap) / 2 > 28 ?
                          (BENCH_H - rows_y - 2 * pad - gap) / 2 : 28;
    const int16_t open_y = BENCH_H / 5 > 120 ? 120 : BENCH_H / 5;
    const int16_t close_distance = BENCH_H - 6;
    if (phase == 0) {
        (void)esp_gsp_inject_touch(ui, BENCH_W / 2, 2, true);
        s_drawer_pressed = true;
    } else if (phase <= DRAWER_DRAG_STEPS) {
        int16_t y = 2 + (open_y - 2) * phase / DRAWER_DRAG_STEPS;
        (void)esp_gsp_inject_touch(ui, BENCH_W / 2, y, true);
    } else if (phase == DRAWER_DRAG_STEPS + 1) {
        (void)esp_gsp_inject_touch(ui, BENCH_W / 2, open_y, false);
        s_drawer_pressed = false;
    } else if (phase == DRAWER_OPEN_CHECK_TICK) {
        bool open = false;
        if (esp_gsp_drawer_is_open(
                    ui, GSP_OBJ_KEY_QUICK_DRAWER, &open) == ESP_GSP_OK &&
                open) {
            ++s_drawer_opens;
        } else {
            ++s_drawer_errors;
        }
    } else if (phase == 24) {
        (void)esp_gsp_inject_touch(
            ui, pad + col / 4, rows_y + row_h / 2, true);
        s_drawer_pressed = true;
    } else if (phase == 25) {
        (void)esp_gsp_inject_touch(
            ui, pad + 3 * col / 4, rows_y + row_h / 2, true);
    } else if (phase == 26) {
        (void)esp_gsp_inject_touch(
            ui, pad + 3 * col / 4, rows_y + row_h / 2, false);
        s_drawer_pressed = false;
    } else if (phase == DRAWER_CLOSE_START_TICK) {
        (void)esp_gsp_inject_touch(
            ui, BENCH_W / 2, BENCH_H - 4, true);
        s_drawer_pressed = true;
    } else if (phase > DRAWER_CLOSE_START_TICK &&
               phase <= DRAWER_CLOSE_START_TICK + DRAWER_DRAG_STEPS) {
        int16_t step = phase - DRAWER_CLOSE_START_TICK;
        int16_t y = BENCH_H - 4 -
                    close_distance * step / DRAWER_DRAG_STEPS;
        (void)esp_gsp_inject_touch(
            ui, BENCH_W / 2, y, true);
    } else if (phase == DRAWER_CLOSE_RELEASE_TICK) {
        (void)esp_gsp_inject_touch(ui, BENCH_W / 2, 2, false);
        s_drawer_pressed = false;
    } else if (phase == DRAWER_CLOSE_CHECK_TICK) {
        bool open = true;
        if (esp_gsp_drawer_is_open(
                    ui, GSP_OBJ_KEY_QUICK_DRAWER, &open) == ESP_GSP_OK &&
                !open) {
            ++s_drawer_closes;
        } else {
            ++s_drawer_errors;
        }
    } else if (phase == DRAWER_CYCLE_TICKS - 1) {
        if (s_drawer_pressed) {
            (void)esp_gsp_inject_touch(ui, 0, 0, false);
            s_drawer_pressed = false;
        }
        (void)esp_gsp_drawer_close(ui, GSP_OBJ_KEY_QUICK_DRAWER, false);
    }
#else
    (void)ui;
#endif
}

static void drive_frame(esp_gsp_handle_t ui, void *user_ctx)
{
    (void)user_ctx;
    uint16_t page = s_pages[s_page].bind;
    if (!s_page_waiting_for_scene && page == GSP_BIND_P_CLOCK) {
        uint32_t tick = ++s_clock_tick;
        esp_err_t results[] = {
            gsp_bench_bench_clock_set_hour_angle(
                ui, (int32_t)(tick % 360U)),
            gsp_bench_bench_clock_set_minute_angle(
                ui, (int32_t)((tick * 2U) % 360U)),
            gsp_bench_bench_clock_set_second_angle(
                ui, (int32_t)((tick * 6U) % 360U)),
            gsp_bench_bench_needle_set_angle(
                ui, (int32_t)((tick * 5U) % 360U)),
        };
        ++s_clock_updates;
        s_clock_commands += sizeof(results) / sizeof(results[0]);
        for (size_t index = 0;
                index < sizeof(results) / sizeof(results[0]); ++index) {
            if (results[index] != ESP_OK) {
                ++s_clock_errors;
            }
        }
    }
    if (!s_page_waiting_for_scene && page == GSP_BIND_P_SCALE) {
        /* 0.5x..2.0x triangle wave. Offset phases keep all fit modes on
         * different source/destination geometry in the same rendered frame. */
        static const gsp_component_key_t images[] = {
            GSP_OBJ_KEY_SCALE_STRETCH,
            GSP_OBJ_KEY_SCALE_CONTAIN,
            GSP_OBJ_KEY_SCALE_COVER,
            GSP_OBJ_KEY_SCALE_DYNAMIC,
        };
        uint32_t tick = ++s_image_scale_tick;
        for (size_t index = 0; index < sizeof(images) / sizeof(images[0]);
                ++index) {
            uint32_t phase = (tick + (uint32_t)index * 24U) % 192U;
            uint32_t ramp = phase <= 96U ? phase : 192U - phase;
            uint32_t scale_q16 = (UINT32_C(1) << 15) + ramp * 1024U;
            esp_gsp_err_t ret = esp_gsp_image_set_scale(
                                    ui, images[index], scale_q16);
            ++s_image_scale_commands;
            if (ret != ESP_GSP_OK) {
                ++s_image_scale_errors;
            }
        }
        ++s_image_scale_updates;
    }
    if (page != GSP_BIND_P_STATIC_MOVE && page != GSP_BIND_P_MOVE) {
        drive_drag(ui, NULL);
    }
    drive_keyboard(ui);
    drive_drawer(ui);
}

static void drive_stress(esp_gsp_handle_t ui, void *user_ctx)
{
    (void)user_ctx;
    drive_fx(ui, NULL);
    drive_text(ui, NULL);
    drive_scroll(ui, NULL);
    uint16_t page = s_pages[s_page].bind;
    if (page == GSP_BIND_P_STATIC_MOVE || page == GSP_BIND_P_MOVE) {
        drive_drag(ui, NULL);
    }
}

static void print_startup_memory(const char *stage)
{
    const uint32_t internal_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
    const uint32_t psram_caps = MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT;

    printf("Memory [%s]:\n", stage);
    printf("  internal: free=%zu min=%zu largest=%zu bytes\n",
           heap_caps_get_free_size(internal_caps),
           heap_caps_get_minimum_free_size(internal_caps),
           heap_caps_get_largest_free_block(internal_caps));
    printf("  psram:    free=%zu min=%zu largest=%zu bytes\n",
           heap_caps_get_free_size(psram_caps),
           heap_caps_get_minimum_free_size(psram_caps),
           heap_caps_get_largest_free_block(psram_caps));
}

void app_main(void)
{
    print_startup_memory("app_main entry");

    esp_display_present_target_config_t display;
#if GSP_BENCH_RGB888
#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
    ESP_ERROR_CHECK(hw_lcd_init_mipi(&display, 24));
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB24
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#else
#error "GSP_BENCH_RGB888 needs MIPI-DPI or 24-bit RGB"
#endif
#else
    ESP_ERROR_CHECK(hw_lcd_init(&display));
#endif
#if CONFIG_EXAMPLE_LCD_INTERFACE_QSPI
    /* ACM0 validation: opt in to TE producer/transfer overlap. Other boards
     * and all library users retain the single-compose default. */
    display.drawbuf.te_compose_buffers = 2;
#endif
#if CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM
    /* Shared partition drawbuf policy. NONE defaults to two buffers;
     * TE_SYNC and framebuffer repair default to one. */
    // display.drawbuf.lines = HW_LCD_V_RES / 2;
    // display.drawbuf.buffers = 1;
#endif
    /* Two runs are only comparable when this whole line matches; it is what
     * tools/compare_logs.py checks before trusting any delta. */
    printf("bench: config protocol=%u frame_ms=%u target_hz=%.1f"
           " stress_ms=%u stress_hz=%.1f service_profile=on diagnostics=%s"
           " target=%s logical=%ux%u"
           " pixel=%s panel=%u mode=%u fb=%u rotation=%u te=%u"
           " drawbuf=%u/%u/%u tebuf=%u"
           " transition_snapshots=%s\n",
           BENCH_PROTOCOL_VERSION, BENCH_FRAME_PERIOD_MS,
           BENCH_FRAME_TARGET_HZ, BENCH_STRESS_PERIOD_MS,
           BENCH_STRESS_TARGET_HZ,
#ifdef GSP_BENCH_DIAGNOSTICS
           "on",
#else
           "off",
#endif
           CONFIG_IDF_TARGET,
           (unsigned)BENCH_W, (unsigned)BENCH_H,
           GSP_BENCH_RGB888 ? "rgb888" : "rgb565",
           (unsigned)display.hw.panel_type,
           (unsigned)display.fb.mode,
           (unsigned)display.fb.frame_buffer_count,
           (unsigned)display.hw.rotation,
           display.hw.te_enabled ? 1U : 0U,
           (unsigned)display.drawbuf.lines,
           (unsigned)display.drawbuf.buffers,
           display.drawbuf.in_psram ? 1U : 0U,
           (unsigned)display.drawbuf.te_compose_buffers,
           BENCH_TRANSITION_SNAPSHOT_MODE);
    print_startup_memory("LCD initialized");

    /* Path timing before UI alloc / page loop (PSRAM already up). */
    bench_present_modes_run();

    esp_lcd_touch_handle_t touch = NULL;
    (void)hw_touch_init(&touch);
    print_startup_memory("touch initialized");

    esp_gsp_handle_t ui;
#ifdef GSP_BENCH_DEPLOYABLE
    static esp_gsp_deployable_bundle_t *deployable_bundle;
    ESP_ERROR_CHECK(esp_gsp_deployable_bundle_open(
                        bundle_start, (size_t)(bundle_end - bundle_start), true,
                        &deployable_bundle));
    esp_gsp_config_t app_config;
    ESP_ERROR_CHECK(esp_gsp_deployable_bundle_make_config(
                        deployable_bundle, &app_config));
    printf("bench: bundle directory=deployable\n");
#else
    esp_gsp_config_t app_config = gsp_bundle_config();
    printf("bench: bundle directory=generated\n");
#endif
#ifdef GSP_BENCH_DISABLE_TRANSITION_SNAPSHOTS
    assert(esp_gsp_config_set(
               &app_config, ESP_GSP_FIELD_ENABLE_TRANSITION_SNAPSHOTS, 0) ==
           ESP_GSP_CONFIG_SET_OK);
#endif
    esp_gsp_esp_lcd_config_t esp_config =
        ESP_GSP_ESP_LCD_CONFIG_INIT();
    esp_config.display = display;
    esp_config.touch = touch;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app_config, &esp_config, &ui));
    ESP_ERROR_CHECK(esp_gsp_on_event(ui, benchmark_event, NULL));
    print_startup_memory("GSP started");

    bench_workload_init(&s_bench_workload, GSP_BENCH_RGB888 != 0);
    esp_gsp_timer_create(ui, 1000, drive_tweens, NULL);
    esp_gsp_timer_create(ui, BENCH_STRESS_PERIOD_MS, drive_stress, NULL);
    esp_gsp_timer_create(ui, BENCH_FRAME_PERIOD_MS, drive_frame, NULL);
    esp_gsp_timer_create(ui, 20, page_scheduler, NULL);
    show_page(ui, 0);
}
