/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once
#include "gsp/gsp_build_caps.h"

/* Deprecated transition API for 0.2.x source integrations. New simulator
 * consumers must use gsp/sim/esp_gsp_simulator.h; this header intentionally
 * remains available while existing source-tree tools migrate. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gsp/gsp_component.h"
#include "gsp/gsp_platform.h"
#include "gsp/gsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gsp_app gsp_app_t;
typedef struct gsp_context gsp_context_t;
typedef struct gsp_font_catalog gsp_font_catalog_t;

/** Mirrors gsp_transition_kind_t without pulling private compose headers. */
typedef enum {
    GSP_APP_TRANSITION_SLIDE_LEFT = 0,
    GSP_APP_TRANSITION_SLIDE_RIGHT,
    GSP_APP_TRANSITION_SLIDE_UP,
    GSP_APP_TRANSITION_SLIDE_DOWN,
    GSP_APP_TRANSITION_FADE,
    GSP_APP_TRANSITION_FADE_THROUGH_BLACK,
    GSP_APP_TRANSITION_SLIDE_LEFT_FADE_THROUGH_BLACK,
    GSP_APP_TRANSITION_SLIDE_RIGHT_FADE_THROUGH_BLACK,
} gsp_app_transition_kind_t;

/** Default idle polling intervals for embedders. */
#define GSP_APP_IDLE_POLL_MS 100U
#define GSP_APP_POINTER_POLL_MS 10U

typedef enum {
    GSP_APP_CMD_SET_VALUE = 0,
    GSP_APP_CMD_SET_COLOR,
    GSP_APP_CMD_SET_VISIBLE,
    GSP_APP_CMD_SET_TEXT,
    GSP_APP_CMD_GOTO_SCENE,
    GSP_APP_CMD_SET_SWIPE_ENABLED,
} gsp_app_command_type_t;

typedef struct {
    gsp_app_command_type_t type;
    uint16_t target;
    uint32_t value;
    const char *text;
} gsp_app_command_t;

typedef struct {
    esp_gsp_config_overrides_t overrides;
    const void *bundle;
    size_t bundle_size;
    const gsp_component_directory_t *const *directories;
    uint16_t directory_count;
    const void *ttf;
    size_t ttf_size;
    size_t freetype_cache_glyphs;
    uint16_t freetype_glyph_max_px;
    /** Shared per-scene template-instance pool. Recycled List/Grid rows and
     *  directly created widgets each consume one simultaneously active slot. */
    size_t instance_slots;
    /** Total runtime glyph-run capacity, including caller and List handles. */
    size_t glyph_run_slots;
    /** Dynamic-text shaping slots: one per bind or template text widget
     *  that can hold shaped text at the same time. Zero keeps the default
     *  (32) — enough for ~19 concurrent dynamic texts plus headroom. Each
     *  slot costs ~386 bytes of heap, so on no-PSRAM targets size this to
     *  the scene's actual peak concurrent text count instead of raising
     *  the default. Values above ESP_GSP_TEXT_SLOTS reserve a larger
     *  glyph-run handle space (text slots occupy the low handles). */
    size_t text_slots;
    /** Logical runtime image targets; the facade reserves two generations
     *  internally for each target. */
    size_t dynamic_image_slots;
    size_t image_cache_bytes;
    const gsp_font_catalog_t *font_catalog;
    bool disable_image_cache;
    size_t dirty_capacity;
    bool disable_swipe;
    bool disable_bundle_crc;
    /** Select POINTER_POLL_MS instead of IDLE_POLL_MS from the resolved
     * project/application policy. Platform touch runners set this. */
    bool use_pointer_poll;
    uint32_t idle_poll_ms;      /*!< default GSP_APP_IDLE_POLL_MS */
} gsp_app_config_t;

typedef struct {
    bool active;
    uint16_t from_scene;
    uint16_t to_scene;
    gsp_app_transition_kind_t kind;
    uint16_t progress; /*!< raw horizontal gesture distance in pixels */
    uint8_t fade_black_point_percent;
} gsp_app_transition_t;

gsp_err_t gsp_app_create(const gsp_app_config_t *config,
                         gsp_app_t **out_app);
const esp_gsp_effective_config_t *gsp_app_effective_config(
    const gsp_app_t *app);
void gsp_app_destroy(gsp_app_t *app);

gsp_err_t gsp_app_apply(gsp_app_t *app,
                        const gsp_app_command_t *command);
void gsp_app_step(gsp_app_t *app, int64_t now_us);
int64_t gsp_app_next_deadline_us(const gsp_app_t *app);
/** Scripted pointer injection (tests, simulators, demos): feeds the
 *  classifier directly, bypassing the input interceptor and observer
 *  that platform runners route through. */
void gsp_app_feed_pointer(gsp_app_t *app, int32_t x, int32_t y,
                          bool pressed);
/** Feeds one contact from a multi-touch source. The portable app keeps the
 * active contact set and routes its centroid through the pointer classifier;
 * platform pinch-aware builds may consume the same contact stream directly. */
void gsp_app_feed_touch(gsp_app_t *app, int32_t id, int32_t x, int32_t y,
                        bool pressed);

uint16_t gsp_app_current_scene(const gsp_app_t *app);
uint16_t gsp_app_scene_count(const gsp_app_t *app);
gsp_context_t *gsp_app_current_context(gsp_app_t *app);
gsp_context_t *gsp_app_scene_context(gsp_app_t *app, uint16_t scene);
bool gsp_app_transition(const gsp_app_t *app,
                        gsp_app_transition_t *out_transition);

#ifdef __cplusplus
}
#endif
