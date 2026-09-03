/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

/* ESP-GSP application API. Prefer generated bundle and component helpers;
 * use this facade for generic runtime operations. Diagnostics and low-level
 * integration helpers live in esp_gsp_debug.h and esp_gsp_advanced.h. */

#pragma once
#include "gsp/gsp_build_caps.h"
#define ESP_GSP_PUBLIC_API_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gsp/gsp_component.h"
#include "gsp/gsp_types.h"
#include "gsp/gsp_version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gsp_ui_core *esp_gsp_handle_t;
typedef struct gsp_font_catalog gsp_font_catalog_t;
typedef int esp_gsp_err_t;

#define ESP_GSP_OK 0
#define ESP_GSP_FAIL -1
#define ESP_GSP_ERR_NO_MEM 0x101
#define ESP_GSP_ERR_INVALID_ARG 0x102
#define ESP_GSP_ERR_INVALID_STATE 0x103
#define ESP_GSP_ERR_NOT_FOUND 0x105
#define ESP_GSP_ERR_NOT_SUPPORTED 0x106
#define ESP_GSP_ERR_TIMEOUT 0x107

/* gspc veneers still spell returns as esp_err_t; accept ESP-IDF's header
 * when present, otherwise alias the portable error type. */
#if defined(__has_include) && __has_include("esp_err.h")
#include "esp_err.h"
#elif !defined(ESP_OK)
typedef esp_gsp_err_t esp_err_t;
#define ESP_OK ESP_GSP_OK
#define ESP_FAIL ESP_GSP_FAIL
#define ESP_ERR_NO_MEM ESP_GSP_ERR_NO_MEM
#define ESP_ERR_INVALID_ARG ESP_GSP_ERR_INVALID_ARG
#define ESP_ERR_INVALID_STATE ESP_GSP_ERR_INVALID_STATE
#define ESP_ERR_NOT_FOUND ESP_GSP_ERR_NOT_FOUND
#define ESP_ERR_NOT_SUPPORTED ESP_GSP_ERR_NOT_SUPPORTED
#define ESP_ERR_TIMEOUT ESP_GSP_ERR_TIMEOUT
#endif
/** Scene-change transitions (values mirror gsp_transition_kind_t;
 *  the facade static-asserts the correspondence). */
typedef enum {
    ESP_GSP_SLIDE_LEFT = 0,         /*!< to-scene enters from the right */
    ESP_GSP_SLIDE_RIGHT,            /*!< to-scene enters from the left */
    ESP_GSP_SLIDE_UP,               /*!< to-scene enters from the bottom */
    ESP_GSP_SLIDE_DOWN,             /*!< to-scene enters from the top */
    ESP_GSP_CROSS_FADE,             /*!< scenes overlap and cross-fade */
    ESP_GSP_FADE = ESP_GSP_CROSS_FADE, /*!< short alias */
    ESP_GSP_FADE_THROUGH_BLACK,     /*!< fade out to black, then fade in */
    ESP_GSP_NO_TRANSITION = 0x100,
} esp_gsp_transition_t;

/** Visual used while horizontally dragging between scenes. */
typedef enum {
    ESP_GSP_SWIPE_SLIDE = 0,       /*!< directional slide (default) */
    ESP_GSP_SWIPE_CROSS_FADE,      /*!< overlap both scene snapshots */
    ESP_GSP_SWIPE_SLIDE_FADE_THROUGH_BLACK,
    /*!< slide under the finger while fading through black */
} esp_gsp_swipe_transition_t;

/** Build-time upper bound for simultaneously active runtime image targets. */
#define ESP_GSP_DYNAMIC_IMAGE_TARGETS_MAX \
    ESP_GSP_BUILD_CAP_MAX_DYNAMIC_IMAGE_TARGETS

typedef struct {
    /** Set to sizeof(esp_gsp_config_t). Use the generated
     *  gsp_<bundle>_config() helper or ESP_GSP_CONFIG_INIT(). */
    size_t struct_size;
    uint32_t schema_version;
    uint16_t abi_version;
    uint16_t reserved_config;
    /** Explicit application overrides. Use the setters below; field ids are
     * stable across compatible schema revisions. */
    esp_gsp_config_overrides_t overrides;

    /* Compiled UI bundle. Platform layers may load this from flash,
     * file systems or network first; the core only consumes bytes. */
    const void *bundle;             /*!< .gspb bytes (64-byte aligned) */
    size_t bundle_size;
    /** Optional Launcher-owned catalog for external FONT resources. */
    const gsp_font_catalog_t *font_catalog;

    /* Component directories (the gsp_obj_directory_<scene> tables
     * gspc emits into <scene>_objects.h), indexed by scene id;
     * entries may be NULL. Registering them once enables the
     * key-addressed component APIs below and descriptor-keyed input
     * routing — no per-call directory plumbing. */
    const gsp_component_directory_t *const *directories;
    uint16_t directory_count;

    /* Dynamic text (chat): optional TTF/OTF blob rasterizing outline
     * glyphs the baked font packs do not carry. FreeType is enabled by
     * gsp_add_bundle(DYNAMIC_FONT ...) or gsp_enable_freetype(). The blob
     * must remain valid until esp_gsp_stop() returns. */
    const void *ttf;
    size_t ttf_size;
    /* Bundle-specific behavior; runtime policies and capacities use
     * esp_gsp_config_set() so every override follows one path. */
    bool disable_swipe;             /*!< horizontal-drag scene
                                         navigation is on by default for
                                         multi-scene bundles; set to
                                         opt out */
    /** Skip CRC verification during runtime creation: the one-time
     *  whole-bundle scan and the nested GRB (resource) and GFB (font)
     *  container CRCs. Each scene's GSB container is still CRC-verified
     *  once while its config is extracted, and structural header,
     *  alignment, member bounds and version validation always remain
     *  enabled. Use only for trusted, build-time assets whose containing
     *  partition is verified separately. */
    bool disable_bundle_crc;
    /** ADVANCED: decoded runtime-image cache budget in bytes. Zero derives a
     *  target default from available heap: half of free PSRAM when present
     *  (clamped to [64 KiB, 4 MiB]), otherwise half of free internal RAM
     *  without a floor (internal-RAM targets can have far less than 128 KiB
     *  free; a raised floor would exceed the reachable heap and turn every
     *  decode into a permanent failure). Increase only when all compiled
     *  startup images and active runtime images cannot fit. */
    size_t image_cache_bytes;
} esp_gsp_config_t;

#if defined(__cplusplus)
static_assert(sizeof(esp_gsp_config_t) <= 256,
              "public configuration ABI exceeds 256 bytes");
#else
_Static_assert(sizeof(esp_gsp_config_t) <= 256,
               "public configuration ABI exceeds 256 bytes");
#endif

static inline esp_gsp_config_t esp_gsp_config_init(void)
{
#ifdef __cplusplus
    esp_gsp_config_t config = {};
#else
    esp_gsp_config_t config = { 0 };
#endif
    config.struct_size = sizeof(config);
    config.schema_version = ESP_GSP_CONFIG_SCHEMA_VERSION;
    config.abi_version = ESP_GSP_CONFIG_ABI_VERSION;
    return config;
}

#define ESP_GSP_CONFIG_INIT() esp_gsp_config_init()

/** Sets one per-instance runtime policy or capacity. Project-wide defaults
 * remain in Kconfig; GSPB scene requirements are applied automatically.
 * Read-only ESP_GSP_BUILD_CAP_* fields are rejected. */
static inline esp_gsp_config_set_result_t esp_gsp_config_set(
    esp_gsp_config_t *config, esp_gsp_config_field_id_t field,
    uint64_t value)
{
    if (config == NULL) {
        return ESP_GSP_CONFIG_SET_INVALID_TABLE;
    }
    return esp_gsp_config_override_set(&config->overrides, field, value);
}

/** UI event kinds delivered by esp_gsp_on_event. */
typedef enum {
    ESP_GSP_EVENT_CALL = 0,         /*!< a `callback` action fired */
    ESP_GSP_EVENT_SCENE_CHANGED,    /*!< navigation settled */
} esp_gsp_event_type_t;

/** UI events delivered by esp_gsp_on_event (render-task context). */
typedef struct esp_gsp_event {
#ifdef __cplusplus
    static constexpr esp_gsp_event_type_t ESP_GSP_EVENT_CALL =
        ::ESP_GSP_EVENT_CALL;
    static constexpr esp_gsp_event_type_t ESP_GSP_EVENT_SCENE_CHANGED =
        ::ESP_GSP_EVENT_SCENE_CHANGED;
#endif
    esp_gsp_event_type_t type;
    uint16_t action_id;             /*!< GSP_ACT_ID_* for CALL */
    uint32_t arg;                   /*!< the action's arg */
    uint16_t scene_id;              /*!< current scene */
    /** CALL from a tapped list row: which list and item; list is
     *  ESP_GSP_LIST_NONE for non-row calls. */
    uint16_t list;
    uint32_t item;
} esp_gsp_event_t;

typedef void (*esp_gsp_event_cb_t)(esp_gsp_handle_t gsp,
                                   const esp_gsp_event_t *event,
                                   void *user_ctx);

typedef void (*esp_gsp_timer_cb_t)(esp_gsp_handle_t gsp, void *user_ctx);

/** Optional pointer observer: receives mapped touch samples on the
 *  render task immediately before gesture routing (single-scene layer
 *  swipes, diagnostics). */
typedef void (*esp_gsp_pointer_observer_cb_t)(esp_gsp_handle_t gsp,
        int32_t x, int32_t y,
        bool pressed, void *user_ctx);

typedef enum {
    ESP_GSP_PINCH_BEGIN = 0,
    ESP_GSP_PINCH_UPDATE,
    ESP_GSP_PINCH_END,
    ESP_GSP_PINCH_CANCEL,
} esp_gsp_pinch_phase_t;

#define ESP_GSP_SCALE_Q16_ONE UINT32_C(65536)

/** Converts an integer percentage to unsigned Q16.16 scale. */
static inline uint32_t esp_gsp_scale_q16_from_percent(uint32_t percent)
{
    uint64_t scale = ((uint64_t)percent * ESP_GSP_SCALE_Q16_ONE + 50U) / 100U;
    return scale > UINT32_MAX ? UINT32_MAX : (uint32_t)scale;
}

/** Combines an image scale and a relative pinch scale, with saturation. */
static inline uint32_t esp_gsp_scale_q16_multiply(uint32_t scale_q16,
        uint32_t relative_q16)
{
    uint64_t scale =
        ((uint64_t)scale_q16 * relative_q16 + (UINT32_C(1) << 15)) >> 16;
    return scale > UINT32_MAX ? UINT32_MAX : (uint32_t)scale;
}

/** Clamps an unsigned Q16.16 scale to an authored or application range. */
static inline uint32_t esp_gsp_scale_q16_clamp(
    uint32_t scale_q16, uint32_t minimum_q16, uint32_t maximum_q16)
{
    if (scale_q16 < minimum_q16) {
        return minimum_q16;
    }
    return scale_q16 > maximum_q16 ? maximum_q16 : scale_q16;
}

/** Application-owned two-contact pinch event. relative_scale_q16 is relative
 * to the distance at BEGIN; the framework does not apply it to any image or
 * other UI object. */
typedef struct {
    esp_gsp_pinch_phase_t phase;
    int32_t center_x;
    int32_t center_y;
    uint32_t relative_scale_q16;
} esp_gsp_pinch_event_t;

/** Return true from BEGIN to accept and capture the pinch. Return values from
 * UPDATE/END/CANCEL are ignored. */
typedef bool (*esp_gsp_pinch_cb_t)(esp_gsp_handle_t gsp,
                                   const esp_gsp_pinch_event_t *event,
                                   void *user_ctx);

/** Opaque builder valid only while an overlay contributor is running. */
typedef struct esp_gsp_overlay_builder esp_gsp_overlay_builder_t;

typedef void (*esp_gsp_overlay_contributor_cb_t)(
    esp_gsp_handle_t gsp, esp_gsp_overlay_builder_t *builder,
    void *user_ctx);

/** Optional top-level input interceptor. Return true to consume the sample
 * before scene hit-testing and gesture routing. */
typedef bool (*esp_gsp_input_interceptor_cb_t)(
    esp_gsp_handle_t gsp, int32_t x, int32_t y, bool pressed,
    void *user_ctx);

/**
 * Stops a platform-created UI instance. Creation belongs to a platform
 * backend, for example the ESP-IDF LCD backend or the SDL
 * adapter on host.
 */
esp_gsp_err_t esp_gsp_stop(esp_gsp_handle_t gsp);

/**
 * Wait until all commands submitted before this call have been applied and
 * the display backend has completed the following render attempt.
 *
 * This is a low-frequency synchronization fence for tests, screenshots and
 * orderly application state changes. Normal UI updates should remain
 * asynchronous. It must not be called from a render-task callback.
 *
 * @param timeout_ms Maximum total wait, including command queue admission.
 *                   Zero performs a non-blocking check/submit.
 *                   A timeout does not cancel an accepted fence.
 */
esp_gsp_err_t esp_gsp_flush(esp_gsp_handle_t gsp, uint32_t timeout_ms);

/** Cumulative render failures and the most recent engine error. The first
 *  frame is strict: esp_gsp_start() returns an error instead of publishing a
 *  handle when it cannot be rendered. Runtime failures remain recoverable and
 *  are counted here rather than silently discarded. */
void esp_gsp_render_error_stats(esp_gsp_handle_t gsp,
                                uint32_t *out_failures,
                                gsp_err_t *out_last_error);

/* --- State updates: callable from ANY task, applied atomically before
 *     the next frame. Bind ids come from the generated *_binds.h. --- */

esp_gsp_err_t esp_gsp_set_value(esp_gsp_handle_t gsp, uint16_t bind,
                                int32_t value);
/** Color in the scene's native pixel format (RGB565 scenes take
 *  16-bit colors, RGB888/ARGB8888 scenes take 24/32-bit). */
esp_gsp_err_t esp_gsp_set_color(esp_gsp_handle_t gsp, uint16_t bind,
                                uint32_t color);
esp_gsp_err_t esp_gsp_set_visible(esp_gsp_handle_t gsp, uint16_t bind,
                                  bool visible);
/** Shapes UTF-8 on the render task; the string is copied (<= 63 bytes
 *  after truncation). */
esp_gsp_err_t esp_gsp_set_text(esp_gsp_handle_t gsp, uint16_t bind,
                               const char *utf8);

/* --- State read-back: the committed value the renderer draws from, on
 *     the CURRENT scene. Callable from any task. A value queued by a
 *     setter above becomes readable once the render task applies it
 *     (before the next frame) — so do not expect a set/get round trip
 *     within the same tick. ESP_ERR_NOT_FOUND for unknown bind ids. --- */

esp_gsp_err_t esp_gsp_get_value(esp_gsp_handle_t gsp, uint16_t bind,
                                int32_t *out_value);
esp_gsp_err_t esp_gsp_get_color(esp_gsp_handle_t gsp, uint16_t bind,
                                uint32_t *out_color);
esp_gsp_err_t esp_gsp_get_visible(esp_gsp_handle_t gsp, uint16_t bind,
                                  bool *out_visible);
/** Logical state of a toggle's value bind (the knob tweens 0..100;
 *  >= 50 reads as ON so a mid-animation read reports the target
 *  side). Checkbox/radio store plain 0/1 — read those with
 *  esp_gsp_get_value. */
esp_gsp_err_t esp_gsp_get_toggle(esp_gsp_handle_t gsp, uint16_t bind,
                                 bool *out_on);

/**
 * Routes a compiled keyboard's key events into a framework-owned edit
 * buffer that drives @p text_bind: characters append, DEL deletes,
 * and only OK reaches the application callback (the field already
 * holds the final text; read it back with esp_gsp_keyboard_text).
 * @p action_id is the keyboard's generated GSP_ACT_ID_<NAME>_KEY;
 * ESP_GSP_KEYBOARD_NONE detaches. The buffer starts empty.
 */
#define ESP_GSP_KEYBOARD_NONE UINT16_MAX
esp_gsp_err_t esp_gsp_keyboard_attach(esp_gsp_handle_t gsp,
                                      uint16_t action_id,
                                      uint16_t text_bind);
/** Copies the attached keyboard's current text (NUL terminated). */
esp_gsp_err_t esp_gsp_keyboard_text(esp_gsp_handle_t gsp, char *out_text,
                                    size_t capacity);

/* --- Component API: address properties by the stable GSP_OBJ_KEY_*
 *     keys from <scene>_objects.h. Requires config.directories.
 *     Resolution happens against the CURRENT scene's directory. Get
 *     reads the committed state (any task, same staleness rule as
 *     the bind getters); set queues through the render task.
 *     ESP_ERR_INVALID_STATE without a registered directory.
 *     VALUE speaks the authored units: a slider/arc/progress with an
 *     authored `min`/`max` gets and sets in that range (the bind-id
 *     setters above stay on the raw 0..100 rail). --- */

esp_gsp_err_t esp_gsp_component_get(esp_gsp_handle_t gsp,
                                    gsp_component_key_t key,
                                    gsp_component_prop_kind_t prop,
                                    gsp_value_t *out_value);
esp_gsp_err_t esp_gsp_component_set(esp_gsp_handle_t gsp,
                                    gsp_component_key_t key,
                                    gsp_component_prop_kind_t prop,
                                    const gsp_value_t *value);

/** Open semantic-key API for component-local properties. Generated typed
 *  wrappers hide the key and tagged value in ordinary application code. */
esp_gsp_err_t esp_gsp_component_get_property(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    gsp_property_key_t property, gsp_value_t *out_value);
esp_gsp_err_t esp_gsp_component_get_property_info(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    gsp_property_key_t property, gsp_property_info_t *out_info);
esp_gsp_err_t esp_gsp_component_get_property_info_at(
    esp_gsp_handle_t gsp, gsp_component_key_t component, size_t index,
    gsp_property_info_t *out_info);
esp_gsp_err_t esp_gsp_component_set_property(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    gsp_property_key_t property, const gsp_value_t *value);

/** Reads or updates a named scalable image's unsigned Q16.16 scale. */
esp_gsp_err_t esp_gsp_image_get_scale(
    esp_gsp_handle_t gsp, gsp_component_key_t image,
    uint32_t *out_scale_q16);
esp_gsp_err_t esp_gsp_image_set_scale(
    esp_gsp_handle_t gsp, gsp_component_key_t image,
    uint32_t scale_q16);

/** Maximum scalar component updates accepted by one atomic batch. */
#define ESP_GSP_COMPONENT_BATCH_MAX ESP_GSP_BUILD_CAP_COMPONENT_BATCH_MAX

/** Queues up to ESP_GSP_COMPONENT_BATCH_MAX typed updates as one atomic
 *  render-task transaction. The array is copied before return and may be
 *  stack allocated. Every entry is validated before the batch is queued;
 *  repeated component/property pairs are allowed and the last value wins. */
esp_gsp_err_t esp_gsp_component_set_many(
    esp_gsp_handle_t gsp,
    const gsp_component_update_t *updates,
    size_t count);

/** Open-key atomic batch; same validation/copy/lifetime rules as set_many. */
esp_gsp_err_t esp_gsp_component_set_properties(
    esp_gsp_handle_t gsp,
    const gsp_component_property_update_t *updates,
    size_t count);

/** Atomically moves a compiled static component subtree. X/Y are authored
 *  scene pixels stored in runtime SRAM; compiled commands remain read-only. */
esp_gsp_err_t esp_gsp_component_set_position(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    int32_t x, int32_t y);
/** Reads the last committed authored position. */
esp_gsp_err_t esp_gsp_component_get_position(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    int32_t *out_x, int32_t *out_y);
/** Queues a relative move from the last committed authored position. */
esp_gsp_err_t esp_gsp_component_translate(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    int32_t dx, int32_t dy);

/** Component capabilities returned by esp_gsp_component_get_info(). */
#define ESP_GSP_COMPONENT_CAP_VALUE    (1U << GSP_COMPONENT_PROP_VALUE)
#define ESP_GSP_COMPONENT_CAP_COLOR    (1U << GSP_COMPONENT_PROP_COLOR)
#define ESP_GSP_COMPONENT_CAP_TEXT     (1U << GSP_COMPONENT_PROP_TEXT)
#define ESP_GSP_COMPONENT_CAP_RESOURCE (1U << GSP_COMPONENT_PROP_RESOURCE)
#define ESP_GSP_COMPONENT_CAP_VISIBLE  (1U << GSP_COMPONENT_PROP_VISIBLE)
#define ESP_GSP_COMPONENT_CAP_CHECKED  (1U << GSP_COMPONENT_PROP_CHECKED)
#define ESP_GSP_COMPONENT_CAP_ENABLED  (1U << GSP_COMPONENT_PROP_ENABLED)
#define ESP_GSP_COMPONENT_CAP_SELECTED (1U << GSP_COMPONENT_PROP_SELECTED)

/** Read-only description of one generated component directory entry. */
typedef struct {
    gsp_component_key_t key;
    gsp_component_kind_t kind;
    uint32_t capabilities;       /*!< ESP_GSP_COMPONENT_CAP_* mask */
    size_t property_count;       /*!< enumerable semantic properties */
} esp_gsp_component_info_t;

/** Queries kind and available properties without reading component state. */
esp_gsp_err_t esp_gsp_component_get_info(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        esp_gsp_component_info_t *out_info);

/** Strongly typed convenience APIs over esp_gsp_component_get/set(). */
esp_gsp_err_t esp_gsp_component_get_value(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        int32_t *out_value);
esp_gsp_err_t esp_gsp_component_set_value(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        int32_t value);
esp_gsp_err_t esp_gsp_component_get_color(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        uint32_t *out_rgb888);
esp_gsp_err_t esp_gsp_component_set_color(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        uint32_t rgb888);
esp_gsp_err_t esp_gsp_component_get_visible(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool *out_visible);
esp_gsp_err_t esp_gsp_component_set_visible(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool visible);
/** Structured content setters resolve the generated property bind by key. */
esp_gsp_err_t esp_gsp_component_set_text(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        const char *text);
esp_gsp_err_t esp_gsp_component_set_image(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        const void *data, size_t size);

/** Toggle on/off by key — CHECKED semantics, no bind ids, no 0..100
 *  rail convention. */
esp_gsp_err_t esp_gsp_component_get_checked(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool *out_checked);
esp_gsp_err_t esp_gsp_component_set_checked(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool checked);
/** Enables or disables a component that opted into authored `enabled`. */
esp_gsp_err_t esp_gsp_component_get_enabled(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool *out_enabled);
esp_gsp_err_t esp_gsp_component_set_enabled(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool enabled);

/** Standard PageFlow navigation. Tab clicks and pointer settle use the same
 *  driver state machine as this API. */
esp_gsp_err_t esp_gsp_page_flow_set_page(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        uint16_t page, bool animated);
esp_gsp_err_t esp_gsp_page_flow_get_page(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        uint16_t *out_page);
/** Current transform offset in pixels along the PageFlow axis. Unlike the
 * selected page, this remains precise while dragging and for stop-anywhere. */
esp_gsp_err_t esp_gsp_page_flow_get_offset(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        int32_t *out_offset_px);
esp_gsp_err_t esp_gsp_page_flow_is_dragging(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool *out_dragging);

/** In-scene StackView navigation. Push/pop requests are rejected while an
 * animation is active; the root page cannot be popped. */
esp_gsp_err_t esp_gsp_stack_view_push(esp_gsp_handle_t gsp,
                                      gsp_component_key_t key,
                                      uint16_t page, bool animated);
esp_gsp_err_t esp_gsp_stack_view_pop(esp_gsp_handle_t gsp,
                                     gsp_component_key_t key,
                                     bool animated);
esp_gsp_err_t esp_gsp_stack_view_get_top(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        uint16_t *out_page);
esp_gsp_err_t esp_gsp_stack_view_is_animating(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        bool *out_animating);

/** Overlay Drawer state. Gesture and Close-button actions use this same
 * settle state machine. */
esp_gsp_err_t esp_gsp_drawer_open(esp_gsp_handle_t gsp,
                                  gsp_component_key_t key, bool animated);
esp_gsp_err_t esp_gsp_drawer_close(esp_gsp_handle_t gsp,
                                   gsp_component_key_t key, bool animated);
esp_gsp_err_t esp_gsp_drawer_is_open(esp_gsp_handle_t gsp,
                                     gsp_component_key_t key,
                                     bool *out_open);

/**
 * Shows a runtime-provided encoded image (JPEG/PNG/QOI, e.g. a network
 * download) on an image bind. The container is sniffed from the data —
 * no metadata needed. The encoded payload is copied before this function
 * returns and decoded by a background worker, so the caller may immediately
 * release or reuse @p data.
 */
esp_gsp_err_t esp_gsp_set_image(esp_gsp_handle_t gsp, uint16_t bind,
                                const void *data, size_t size);

/**
 * Returns a borrowed encoded-image payload to its producer. The callback runs
 * exactly once after the framework no longer references the payload. It may
 * run from the caller (submission failure), render task or decode task and
 * must not block.
 */
typedef void (*esp_gsp_image_release_cb_t)(void *release_ctx);

/** Encoded-input ownership used by esp_gsp_set_image_ex(). */
typedef enum {
    ESP_GSP_IMAGE_COPY = 0,   /*!< framework copies before returning */
    ESP_GSP_IMAGE_BORROW,     /*!< caller retains until on_release */
    ESP_GSP_IMAGE_TAKE,       /*!< framework owns after successful submit */
} esp_gsp_image_ownership_t;

/**
 * Reports the terminal result of one accepted runtime-image request. The
 * callback runs exactly once from the render or decode task. GSP_OK means the
 * new image was published; GSP_ERR_CANCELLED means a newer request or shutdown
 * superseded it. Keep the callback short and non-blocking. In particular, do
 * not call esp_gsp_stop() from this callback; defer shutdown to the application
 * task.
 */
typedef void (*esp_gsp_image_complete_cb_t)(
    esp_gsp_handle_t gsp, uint16_t bind, gsp_err_t status,
    void *complete_ctx);

typedef struct {
    esp_gsp_image_ownership_t ownership;
    /** Required for BORROW, invalid for COPY. For TAKE, NULL uses free(data). */
    esp_gsp_image_release_cb_t on_release;
    void *release_ctx;
    esp_gsp_image_complete_cb_t on_complete;
    void *complete_ctx;
    /** Reuses decoded pixels for identical immutable content. Change the
     *  nonzero key when the encoded content changes; zero keeps legacy
     *  per-submit caching. */
    uint32_t cache_key;
} esp_gsp_image_options_t;

/**
 * Complete runtime-image submission contract. NULL options select COPY.
 * BORROW returns the immutable source through on_release. TAKE transfers the
 * source only when this function returns ESP_GSP_OK; on immediate submission
 * failure the caller still owns it. on_complete is called only for accepted
 * submissions.
 */
esp_gsp_err_t esp_gsp_set_image_ex(
    esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size,
    const esp_gsp_image_options_t *options);

/**
 * Borrowing variant of esp_gsp_set_image(). The encoded bytes are never
 * copied; @p data must remain immutable until @p on_release is called.
 * Replacement, cancellation, decode failure and shutdown all release the
 * payload exactly once. @p on_release is required.
 *
 * Note that a published image holds its payload for as long as it is the one
 * on screen, not just until it decodes: the decode cache may drop the pixels
 * under memory pressure and re-decode from these bytes. So a borrowed buffer
 * comes back when the next image for that bind is adopted, or at shutdown —
 * budget for one outstanding buffer per image bind.
 *
 * This removes only the encoded-input copy. Decode and framebuffer composition
 * still follow the target image/cache policy.
 */
esp_gsp_err_t esp_gsp_set_image_borrowed(
    esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size,
    esp_gsp_image_release_cb_t on_release, void *release_ctx);

/**
 * Transfers malloc-compatible encoded storage without copying. Ownership is
 * transferred only when ESP_GSP_OK is returned; the framework calls free()
 * once the image is replaced, cancelled or shut down — same lifetime as the
 * borrowed variant, without the callback.
 */
esp_gsp_err_t esp_gsp_set_image_owned(
    esp_gsp_handle_t gsp, uint16_t bind, void *data, size_t size);

/** Shows a blinking caret at the end of a text bind's current content
 *  (soft-keyboard input fields); ESP_GSP_NO_CURSOR hides it. Track the
 *  content with esp_gsp_set_text as usual. */
#define ESP_GSP_NO_CURSOR UINT16_MAX
esp_gsp_err_t esp_gsp_set_cursor(esp_gsp_handle_t gsp, uint16_t bind);

/** Switches scenes (multi-scene bundles) with an optional transition.
 *  Use ESP_GSP_CROSS_FADE to blend overlapping scenes, or
 *  ESP_GSP_FADE_THROUGH_BLACK to dim the old scene fully to black
 *  before revealing the new scene. Strip displays can render
 *  fade-through-black incrementally without full-screen snapshots;
 *  cross-fade still requires both scene snapshots. If no visual path
 *  is available, the scene still switches safely. Safe from any task;
 *  also invoked
 *  automatically by `goto` actions and swipe navigation. */
esp_gsp_err_t esp_gsp_goto_scene(esp_gsp_handle_t gsp, uint16_t scene_id,
                                 esp_gsp_transition_t transition);

/** Enables or disables horizontal swipe scene navigation at runtime.
 *  Multi-scene bundles enable swipes by default (@c disable_swipe=false
 *  in esp_gsp_config_t); this mirrors that flag. The default classifier locks
 *  only when horizontal travel clearly dominates vertical travel. Release
 *  commits after a one-fifth-screen pull, or after a shorter deliberate fling;
 *  tiny fast movements remain below the fling-distance guard. Safe from any
 *  task. */
esp_gsp_err_t esp_gsp_set_swipe_enabled(esp_gsp_handle_t gsp, bool enabled);

/** Selects the visual that follows horizontal scene-swipe progress.
 *  The default is ESP_GSP_SWIPE_SLIDE. The slide-fade option moves the
 *  pages directly from gesture distance; the transition layer derives
 *  brightness independently from that same raw distance, so no
 *  effect-specific calculation lives in the touch path. Safe from any
 *  task. */
esp_gsp_err_t esp_gsp_set_swipe_transition(
    esp_gsp_handle_t gsp, esp_gsp_swipe_transition_t transition);

/** Sets where a fade-through-black swipe reaches full black.
 *  drag_percent is the horizontal drag distance as a percentage of
 *  screen width (1..99). The default is 50. A smaller value dims the
 *  current scene faster; the target scene still reaches full brightness
 *  at 100%. This changes only brightness response, never page position,
 *  gesture recognition or commit thresholds. Safe from any task. */
esp_gsp_err_t esp_gsp_set_swipe_fade_black_point(
    esp_gsp_handle_t gsp, uint8_t drag_percent);

/** Registers the application event callback (render-task context;
 *  keep it short, use esp_gsp_set_* freely inside). */
esp_gsp_err_t esp_gsp_on_event(esp_gsp_handle_t gsp, esp_gsp_event_cb_t cb,
                               void *user_ctx);

/** Registers the independent pinch observer through the render-task queue.
 *
 * BEGIN is emitted once with scale 1.0. Return true to accept the pinch and
 * cancel the current single-pointer route; false leaves that touch sequence on
 * the single-pointer route. Later accepted samples emit UPDATE. END means the
 * contacts were released normally and CANCEL means recognition was
 * interrupted. Passing NULL unregisters the observer. The callback runs in
 * render-task context and must not block.
 */
esp_gsp_err_t esp_gsp_on_pinch(esp_gsp_handle_t gsp,
                               esp_gsp_pinch_cb_t cb, void *user_ctx);

/** Registers a pointer observer (render-task context). Pass @c NULL to
 *  clear. */
esp_gsp_err_t esp_gsp_set_pointer_observer(
    esp_gsp_handle_t gsp, esp_gsp_pointer_observer_cb_t cb, void *user_ctx);

/** Registers one application-owned overlay contributor through the
 *  render-task queue (like esp_gsp_on_pinch): the registration and the
 *  resulting overlay rebuild are applied on the render task, never inline
 *  in the calling task. Passing NULL clears it. The callback runs on the
 *  render task during overlay rebuild. */
esp_gsp_err_t esp_gsp_set_overlay_contributor(
    esp_gsp_handle_t gsp, esp_gsp_overlay_contributor_cb_t cb,
    void *user_ctx);

/** Registers one top-level input interceptor through the render-task
 *  queue. Passing NULL clears it. The callback runs in render-task
 *  context and must not block. */
esp_gsp_err_t esp_gsp_set_input_interceptor(
    esp_gsp_handle_t gsp, esp_gsp_input_interceptor_cb_t cb,
    void *user_ctx);

esp_gsp_err_t esp_gsp_overlay_builder_round_rect(
    esp_gsp_overlay_builder_t *builder, int32_t x, int32_t y,
    uint16_t width, uint16_t height, uint32_t color, uint16_t radius,
    uint8_t opacity);
/** The @p a8 bitmap is borrowed, not copied: it must stay alive until
 *  the next overlay rebuild (any state change that repaints chrome, or
 *  the next esp_gsp_set_overlay_contributor call). */
esp_gsp_err_t esp_gsp_overlay_builder_glyph_a8(
    esp_gsp_overlay_builder_t *builder, int32_t x, int32_t y,
    uint16_t width, uint16_t height, uint32_t color,
    const uint8_t *a8, size_t a8_size, size_t stride_bytes);

/* --- Declarative scrolling lists (LVGL-style): the compiler emits
 *     the row template + viewport constants; the framework owns the
 *     repeater, the drag gesture, clamping, and threading. The app
 *     only fills row data. --- */

typedef uint16_t esp_gsp_list_t;
#define ESP_GSP_LIST_NONE UINT16_MAX
typedef esp_gsp_list_t esp_gsp_grid_t;
#define ESP_GSP_GRID_NONE ((esp_gsp_grid_t)UINT16_MAX)

typedef enum {
    ESP_GSP_MESSAGE_INCOMING = 0,
    ESP_GSP_MESSAGE_OUTGOING = 1,
} esp_gsp_message_direction_t;

/** Borrowed message view; text only needs to remain valid during get().
 * Keep id stable and increment revision when content changes. The framework
 * also checks a text hash, so stale revision values remain functionally safe. */
typedef struct {
    const char *text;
    uint64_t id;
    uint32_t revision;
    esp_gsp_message_direction_t direction;
} esp_gsp_message_t;

typedef uint32_t (*esp_gsp_message_count_cb_t)(void *user_ctx);
typedef bool (*esp_gsp_message_get_cb_t)(
    void *user_ctx, uint32_t index, esp_gsp_message_t *out_message);

/** Expands a generated template prefix to the bind_list geometry
 *  arguments: esp_gsp_bind_list(ui, ESP_GSP_LIST(GSP_TEMPLATE_ROW),
 *  total, bind_row, ctx). */
#define ESP_GSP_LIST(prefix) \
    (prefix), (prefix##_VIEWPORT_X), (prefix##_VIEWPORT_Y), \
    (prefix##_VIEWPORT_W), (prefix##_VIEWPORT_H)

/**
 * Creates a framework-owned scrolling list from a compiled row
 * template (asynchronous). @p bind_item runs on the render task for
 * every row entering the viewport — fill it with gsp_instance_set_* /
 * gsp_widget calls. Returns ESP_GSP_LIST_NONE when the shared
 * List/Grid/Wheel quota (default 5) is exhausted. Bind each logical list once and
 * retain its handle: bindings
 * persist across scene changes, parking while their scene/page is hidden and
 * reactivating when it becomes visible. Rebinding on every scene-ready event
 * consumes another quota slot; update an existing binding with
 * esp_gsp_list_set_total() and esp_gsp_list_refresh().
 */
/** Opaque row reference handed to the binder; feed it to the
 *  esp_gsp_row_* setters. */
typedef struct {
    uint16_t list;
    uint16_t slot;
    gsp_instance_handle_t instance;
    /** Data item currently assigned to this recycled slot. */
    uint32_t item;
} esp_gsp_row_t;

/** One recycled Grid cell. The row token remains available for advanced
 * composite-cell setters; ordinary image/text grids use the convenience
 * functions below and never handle template slots. */
typedef struct {
    esp_gsp_row_t row;
    uint16_t resource_slot;
    uint16_t text_slot;
} esp_gsp_grid_cell_t;

typedef gsp_err_t (*esp_gsp_message_decorate_cb_t)(
    esp_gsp_handle_t gsp, esp_gsp_row_t row,
    const esp_gsp_message_t *message, void *user_ctx);

/** Application-owned data source copied by bind; callbacks run on render task. */
typedef struct {
    size_t struct_size;
    esp_gsp_message_count_cb_t count;
    esp_gsp_message_get_cb_t get;
    esp_gsp_message_decorate_cb_t decorate;
    void *user_ctx;
} esp_gsp_message_source_t;

/** Row binder (render-task context): fill the row with the
 *  esp_gsp_row_* setters — buffers, glyph handles, and slot lookup
 *  are framework-owned. */
typedef gsp_err_t (*esp_gsp_list_bind_cb_t)(
    esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t item_index,
    void *user_ctx);

/** Grid binder (render-task context), called with the logical cell item. */
typedef gsp_err_t (*esp_gsp_grid_bind_cb_t)(
    esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, uint32_t item_index,
    void *user_ctx);

/** Binds a generated Grid component. The framework owns row/column mapping,
 * recycling, tail cells and scrolling; the callback only fills one item. */
esp_gsp_grid_t esp_gsp_grid_bind_component(
    esp_gsp_handle_t gsp, gsp_component_key_t key,
    esp_gsp_grid_bind_cb_t bind_item, void *user_ctx);

/** Slot-free convenience setters for the Grid cell's first dynamic image or
 * text field. They return ESP_GSP_ERR_NOT_SUPPORTED when the template has no
 * matching dynamic field. */
esp_gsp_err_t esp_gsp_grid_cell_set_image(
    esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell,
    const void *data, size_t size);
esp_gsp_err_t esp_gsp_grid_cell_set_image_ex(
    esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell,
    const void *data, size_t size,
    const esp_gsp_image_options_t *options);
esp_gsp_err_t esp_gsp_grid_cell_set_image_borrowed(
    esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell,
    const void *data, size_t size,
    esp_gsp_image_release_cb_t on_release, void *release_ctx);
esp_gsp_err_t esp_gsp_grid_cell_set_image_owned(
    esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell,
    void *data, size_t size);
esp_gsp_err_t esp_gsp_grid_cell_set_text(
    esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const char *utf8);

/** Grid-named aliases keep application code at the cell abstraction. */
esp_gsp_err_t esp_gsp_grid_set_total(
    esp_gsp_handle_t gsp, esp_gsp_grid_t grid, uint32_t total);
esp_gsp_err_t esp_gsp_grid_refresh(
    esp_gsp_handle_t gsp, esp_gsp_grid_t grid);

/** Shapes UTF-8 into the row's TEXT slot (FreeType fallback and
 *  multi-line wrap apply as for esp_gsp_set_text). */
esp_gsp_err_t esp_gsp_row_text(esp_gsp_handle_t gsp, esp_gsp_row_t row,
                               const char *utf8);
/** Shapes UTF-8 into an explicit TEXT slot of a composite row. */
esp_gsp_err_t esp_gsp_row_set_text(esp_gsp_handle_t gsp,
                                   esp_gsp_row_t row, uint16_t text_slot,
                                   const char *utf8);
/** Drives the row's first VALUE slot (0..100). */
esp_gsp_err_t esp_gsp_row_value(esp_gsp_handle_t gsp, esp_gsp_row_t row,
                                uint32_t value);
/** Drives an explicit VALUE slot of a composite row. */
esp_gsp_err_t esp_gsp_row_set_value(esp_gsp_handle_t gsp,
                                    esp_gsp_row_t row, uint16_t value_slot,
                                    uint32_t value);
/** Drives the row's first COLOR slot (native format). */
esp_gsp_err_t esp_gsp_row_color(esp_gsp_handle_t gsp, esp_gsp_row_t row,
                                uint32_t color);
/** Drives an explicit COLOR slot of a composite row. */
esp_gsp_err_t esp_gsp_row_set_color(esp_gsp_handle_t gsp,
                                    esp_gsp_row_t row, uint16_t color_slot,
                                    uint32_t color);

/**
 * Replaces one RESOURCE slot in a recycled row from encoded JPEG/PNG/QOI
 * bytes. The input is copied before return. A stale row token cancels the
 * accepted request before publication, so an asynchronous loader cannot
 * publish into a slot that has since been recycled for another item.
 */
esp_gsp_err_t esp_gsp_row_set_image(
    esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot,
    const void *data, size_t size);
esp_gsp_err_t esp_gsp_row_set_image_ex(
    esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot,
    const void *data, size_t size,
    const esp_gsp_image_options_t *options);

/** Borrowed/owned variants of esp_gsp_row_set_image(). */
esp_gsp_err_t esp_gsp_row_set_image_borrowed(
    esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot,
    const void *data, size_t size,
    esp_gsp_image_release_cb_t on_release, void *release_ctx);
esp_gsp_err_t esp_gsp_row_set_image_owned(
    esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot,
    void *data, size_t size);

/**
 * Creates the list/wheel by its stable component key: template,
 * viewport and initial total (the authored item count) come from the
 * generated directory — nothing to re-pair by hand. Requires
 * config.directories. A NULL @p bind_item serves the authored item
 * texts directly (fixed lists: zero application assembly). Adjust
 * the count later with esp_gsp_list_set_total when data is dynamic. The
 * returned binding has UI-instance lifetime; there is currently no unbind
 * operation.
 */
esp_gsp_list_t esp_gsp_list_bind_component(esp_gsp_handle_t gsp,
        gsp_component_key_t key,
        esp_gsp_list_bind_cb_t bind_item,
        void *user_ctx);

/** ADVANCED: manual template/viewport pairing (prefer the
 *  component-key form above). */
esp_gsp_list_t esp_gsp_bind_list(esp_gsp_handle_t gsp,
                                 uint16_t template_id, int16_t x,
                                 int16_t y, uint16_t width,
                                 uint16_t height, uint32_t total,
                                 esp_gsp_list_bind_cb_t bind_item,
                                 void *user_ctx);
esp_gsp_err_t esp_gsp_list_set_total(esp_gsp_handle_t gsp,
                                     esp_gsp_list_t list, uint32_t total);
/** Re-binds visible rows after the backing data changed. */
esp_gsp_err_t esp_gsp_list_refresh(esp_gsp_handle_t gsp,
                                   esp_gsp_list_t list);

/**
 * Enables variable-height rows and atomically replaces the complete height
 * index. There is no logical item limit other than memory and the signed
 * 31-bit pixel extent. Existing fixed-height lists do not allocate this
 * index and keep their arithmetic fast path.
 *
 * @p prepended_count maps the old first visible item onto its new index when
 * older records were inserted at the front. If @p stick_to_end is true, a
 * viewport already at the end remains attached to the end (new live messages
 * do not pull a user who is browsing older history).
 */
esp_gsp_err_t esp_gsp_list_set_item_heights(
    esp_gsp_handle_t gsp, esp_gsp_list_t list, const uint16_t *heights,
    uint32_t count, uint32_t prepended_count, bool stick_to_end);

/** Binds a generated message_list component and loads its current source.
 * The source is application-owned; its callbacks execute on the render task. */
esp_gsp_list_t esp_gsp_message_list_bind_component(
    esp_gsp_handle_t gsp, gsp_component_key_t key,
    const esp_gsp_message_source_t *source);

/**
 * Re-reads count/messages and updates layout. Pass the number inserted before
 * the previous item zero to preserve the visible history anchor. Appends keep
 * a viewport already at the end attached to the new end, without pulling a
 * user who is browsing older history.
 */
esp_gsp_err_t esp_gsp_message_list_changed(
    esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t prepended_count);

/** Wheel-style behavior: released drags settle on a row boundary. */
esp_gsp_err_t esp_gsp_list_snap(esp_gsp_handle_t gsp, esp_gsp_list_t list,
                                bool enable);

/** Programmatic momentum: starts coasting at @p velocity_px_s (signed;
 *  positive scrolls toward higher items). Friction and row snapping
 *  behave exactly as a released drag. */
esp_gsp_err_t esp_gsp_list_fling(esp_gsp_handle_t gsp, esp_gsp_list_t list,
                                 int32_t velocity_px_s);

/** Jumps to an absolute scroll offset in pixels (clamped to content;
 *  cancels any coasting). Row N sits at offset N * row_height. */
esp_gsp_err_t esp_gsp_list_scroll_to(esp_gsp_handle_t gsp,
                                     esp_gsp_list_t list,
                                     int32_t offset_px);

/** Roller-style fade: translucent @p native_color bands dim the rows
 *  toward the viewport's top and bottom edges (LVGL-roller look).
 *  Pass the wheel's background color. The bands render above the row
 *  instances, which disables the scroll-blit shortcut over this
 *  viewport — scrolling repaints the viewport instead. */
esp_gsp_err_t esp_gsp_list_fade(esp_gsp_handle_t gsp, esp_gsp_list_t list,
                                uint32_t native_color, bool enable);

/** Vertical drag deltas inside @p region (render-task context) —
 *  feed them to gsp_repeater_set_scroll or your own logic. Drags in a
 *  scroll region take priority over scene swipes; taps still work. */
typedef void (*esp_gsp_scroll_cb_t)(esp_gsp_handle_t gsp,
                                    int32_t delta_y, void *user_ctx);
esp_gsp_err_t esp_gsp_on_scroll(esp_gsp_handle_t gsp, gsp_rect_t region,
                                esp_gsp_scroll_cb_t cb, void *user_ctx);

/* --- Canvas: external frame sink (video / camera / any user
 *     stream). The scene authors a placeholder image with a bind; the
 *     producer pushes profile-native opaque RGB565/RGB888 frames of the
 *     SAME size and format. stride_bytes must be at least width * 2 for
 *     RGB565 or width * 3 for RGB888, and the supplied storage must cover
 *     stride_bytes * authored height bytes. Frames
 *     remain borrowed (submission does not clone them): on_release returns
 *     the frame once the machine stopped referencing it (the frame
 *     that replaced it has rendered, or the canvas stopped). Pushing
 *     faster than the render loop drops the never-shown frame
 *     immediately. A continuous zero-copy stream therefore needs at
 *     least two producer frames; one remains live while the next is
 *     submitted. Release callbacks run in render-task context and must
 *     not block. Every successfully accepted frame gets exactly one
 *     release callback; after any immediate API error, ownership remains
 *     with the caller and no callback follows. Any-task safe (not ISR
 *     safe). --- */
typedef esp_gsp_image_release_cb_t esp_gsp_canvas_release_cb_t;

/** Temporary destination passed to a Canvas draw callback. @p pixels points
 *  at Canvas-local (@p x, @p y); write exactly width x height pixels using
 *  stride_bytes and do not retain the pointer after the callback returns. */
typedef struct {
    void *pixels;
    size_t stride_bytes;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    /** ESP_GSP_CANVAS_PIXEL_RGB565 or ESP_GSP_CANVAS_PIXEL_RGB888. */
    uint8_t pixel_format;
} esp_gsp_canvas_surface_t;

#define ESP_GSP_CANVAS_PIXEL_RGB565 0U
#define ESP_GSP_CANVAS_PIXEL_RGB888 1U

/** Draws Canvas pixels directly into the renderer-owned destination. The
 *  callback runs in render-task context and may receive partitioned regions.
 *  It must fully initialize every supplied region, must not block, and must
 *  not call a GSP API that changes state or waits for the render queue; such
 *  calls return ESP_GSP_ERR_INVALID_STATE. Read-only queries are allowed. */
typedef void (*esp_gsp_canvas_draw_cb_t)(
    const esp_gsp_canvas_surface_t *surface, void *user_ctx);

/** Registers a direct draw callback and requests the first full repaint.
 *  The authored bind must target an unscaled profile-native opaque image.
 *  Existing esp_gsp_canvas_push() users are unaffected; pushing a frame to
 *  the same bind replaces the callback. user_ctx must remain valid until the
 *  callback is replaced or Canvas stop/destruction has completed; use
 *  esp_gsp_flush() before releasing it after esp_gsp_canvas_stop(). A repaint
 *  may invoke the callback more than once; render one coherent generation for
 *  all regions belonging to that repaint. */
esp_gsp_err_t esp_gsp_canvas_set_draw_cb(
    esp_gsp_handle_t gsp, uint16_t bind,
    esp_gsp_canvas_draw_cb_t draw_cb, void *user_ctx);

/** Requests a full repaint of a callback-backed Canvas. */
esp_gsp_err_t esp_gsp_canvas_invalidate(
    esp_gsp_handle_t gsp, uint16_t bind);

/** Requests repaint of a non-empty Canvas-local rectangle. */
esp_gsp_err_t esp_gsp_canvas_invalidate_dirty(
    esp_gsp_handle_t gsp, uint16_t bind, gsp_rect_t dirty);

/** Publishes a borrowed frame. Eligible renderer copies automatically use the
 *  available PPA/DMA2D backend and otherwise fall back to software. */
esp_gsp_err_t esp_gsp_canvas_push(esp_gsp_handle_t gsp, uint16_t bind,
                                  const void *pixels, size_t stride_bytes,
                                  esp_gsp_canvas_release_cb_t on_release,
                                  void *release_ctx);

/** Non-blocking variant of esp_gsp_canvas_push(). Returns
 *  ESP_GSP_ERR_TIMEOUT when the render queue cannot accept the frame
 *  immediately; ownership remains with the caller in that case. */
esp_gsp_err_t esp_gsp_canvas_try_push(
    esp_gsp_handle_t gsp, uint16_t bind, const void *pixels,
    size_t stride_bytes, esp_gsp_canvas_release_cb_t on_release,
    void *release_ctx);

/** Publishes a borrowed Canvas frame while repainting only @p dirty, in
 *  resource-local pixels with an exclusive x2/y2 edge. Pixels outside dirty
 *  must match the previously displayed frame. The dirty rectangle must be
 *  non-empty and fit the authored placeholder. Ownership and release ordering
 *  are identical to esp_gsp_canvas_push(). */
esp_gsp_err_t esp_gsp_canvas_push_dirty(
    esp_gsp_handle_t gsp, uint16_t bind, const void *pixels,
    size_t stride_bytes, gsp_rect_t dirty,
    esp_gsp_canvas_release_cb_t on_release, void *release_ctx);

/** Non-blocking variant of esp_gsp_canvas_push_dirty(). */
esp_gsp_err_t esp_gsp_canvas_try_push_dirty(
    esp_gsp_handle_t gsp, uint16_t bind, const void *pixels,
    size_t stride_bytes, gsp_rect_t dirty,
    esp_gsp_canvas_release_cb_t on_release, void *release_ctx);

/** Queues restoration of the placeholder image. The release callback is the
 *  completion signal for each borrowed frame; do not free accepted frames
 *  immediately after this function returns. */
esp_gsp_err_t esp_gsp_canvas_stop(esp_gsp_handle_t gsp, uint16_t bind);

/** Periodic callback in render-task context (lv_timer equivalent).
 *  Returns a handle usable with esp_gsp_timer_delete; NULL on error. */
void *esp_gsp_timer_create(esp_gsp_handle_t gsp, uint32_t period_ms,
                           esp_gsp_timer_cb_t cb, void *user_ctx);
esp_gsp_err_t esp_gsp_timer_delete(esp_gsp_handle_t gsp, void *timer);

/* --- Template widgets: runtime instances of gspc-declared templates
 *     (GSP_TEMPLATE_* constants from the generated *_templates.h).
 *     Same threading rules as the setters: callable from any task,
 *     applied on the render task before the next frame. --- */

typedef uint16_t esp_gsp_widget_t;
#define ESP_GSP_WIDGET_NONE UINT16_MAX

/** Generated-template identity and authored bounds. Applications normally
 * receive one named constant per template from `<scene>_templates.h`. */
typedef struct {
    uint16_t id;
    uint16_t width;
    uint16_t height;
    uint16_t max_instances;
} esp_gsp_template_desc_t;

/** Instantiates @p template_id at (@p x, @p y) in the CURRENT scene.
 *  Creation is asynchronous; the handle is immediately usable with the
 *  widget setters. Returns ESP_GSP_WIDGET_NONE when the widget quota
 *  is used up. */
esp_gsp_widget_t esp_gsp_widget_create(esp_gsp_handle_t gsp,
                                       uint16_t template_id,
                                       int16_t x, int16_t y);

/** Named-descriptor form; avoids exposing the compiled template id. */
static inline esp_gsp_widget_t esp_gsp_widget_create_template(
    esp_gsp_handle_t gsp, const esp_gsp_template_desc_t *template_desc,
    int16_t x, int16_t y)
{
    return template_desc != NULL ?
           esp_gsp_widget_create(gsp, template_desc->id, x, y) :
           ESP_GSP_WIDGET_NONE;
}
esp_gsp_err_t esp_gsp_widget_destroy(esp_gsp_handle_t gsp,
                                     esp_gsp_widget_t widget);

/** Drives a VALUE slot (progress members), value in 0..100. */
esp_gsp_err_t esp_gsp_widget_set_value(esp_gsp_handle_t gsp,
                                       esp_gsp_widget_t widget,
                                       uint16_t slot, uint32_t value);
/** Drives a COLOR slot, native pixel format (see esp_gsp_set_color). */
esp_gsp_err_t esp_gsp_widget_set_color(esp_gsp_handle_t gsp,
                                       esp_gsp_widget_t widget,
                                       uint16_t slot, uint32_t color);
/** Shapes UTF-8 into a TEXT slot (copied, <= 63 bytes). */
esp_gsp_err_t esp_gsp_widget_set_text(esp_gsp_handle_t gsp,
                                      esp_gsp_widget_t widget,
                                      uint16_t slot, const char *utf8);
/** Replaces a template RESOURCE slot from copied encoded image bytes. */
esp_gsp_err_t esp_gsp_widget_set_image(
    esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot,
    const void *data, size_t size);
esp_gsp_err_t esp_gsp_widget_set_image_borrowed(
    esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot,
    const void *data, size_t size,
    esp_gsp_image_release_cb_t on_release, void *release_ctx);
esp_gsp_err_t esp_gsp_widget_set_image_owned(
    esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot,
    void *data, size_t size);

/* Convenience setters targeting the widget's FIRST slot of each kind
 * (single-purpose templates need no *_SLOT constants; multi-slot
 * templates keep the explicit forms above). */
esp_gsp_err_t esp_gsp_widget_value(esp_gsp_handle_t gsp,
                                   esp_gsp_widget_t widget, uint32_t value);
esp_gsp_err_t esp_gsp_widget_color(esp_gsp_handle_t gsp,
                                   esp_gsp_widget_t widget, uint32_t color);
esp_gsp_err_t esp_gsp_widget_text(esp_gsp_handle_t gsp,
                                  esp_gsp_widget_t widget,
                                  const char *utf8);
esp_gsp_err_t esp_gsp_widget_set_visible(esp_gsp_handle_t gsp,
        esp_gsp_widget_t widget,
        bool visible);
esp_gsp_err_t esp_gsp_widget_set_position(esp_gsp_handle_t gsp,
        esp_gsp_widget_t widget,
        int16_t x, int16_t y);

/**
 * Plays an EAF frame animation into a profile-native RGB565/RGB888
 * placeholder image resource. The framework ticks it on the render task —
 * no app-side loop. @p eaf is NOT copied. once = stop on the last frame.
 */
esp_gsp_err_t esp_gsp_play(esp_gsp_handle_t gsp, const void *eaf,
                           size_t size, bool once);

/* --- Value animations (lv_anim equivalent): the framework tweens
 *     a bind or widget VALUE slot on the render task. --- */

typedef enum {
    ESP_GSP_EASE_LINEAR = 0,
    ESP_GSP_EASE_OUT,               /*!< cubic ease-out */
    ESP_GSP_EASE_IN_OUT,            /*!< cubic ease-in-out */
} esp_gsp_ease_t;

/** Pass as @p from to start a tween from the target's CURRENT value. */
#define ESP_GSP_ANIM_CURRENT INT32_MIN

/** Animates a value bind from @p from to @p to (0..100 domain) over
 *  @p duration_ms. A new animation on the same bind replaces the
 *  running one. */
esp_gsp_err_t esp_gsp_animate(esp_gsp_handle_t gsp, uint16_t bind,
                              int32_t from, int32_t to,
                              uint32_t duration_ms, esp_gsp_ease_t ease);

/** Tweens a color bind between two native colors (per-channel
 *  interpolation, RGB565 scenes). */
esp_gsp_err_t esp_gsp_animate_color(esp_gsp_handle_t gsp, uint16_t bind,
                                    uint32_t from, uint32_t to,
                                    uint32_t duration_ms,
                                    esp_gsp_ease_t ease);

/** Component-key animations use semantic VALUE/COLOR properties and the
 *  same authored-unit conversion as component get/set. */
esp_gsp_err_t esp_gsp_component_animate_value(
    esp_gsp_handle_t gsp, gsp_component_key_t key,
    int32_t from, int32_t to, uint32_t duration_ms,
    esp_gsp_ease_t ease);
esp_gsp_err_t esp_gsp_component_animate_value_to(
    esp_gsp_handle_t gsp, gsp_component_key_t key,
    int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
esp_gsp_err_t esp_gsp_component_animate_color(
    esp_gsp_handle_t gsp, gsp_component_key_t key,
    uint32_t from, uint32_t to, uint32_t duration_ms,
    esp_gsp_ease_t ease);
esp_gsp_err_t esp_gsp_component_animate_color_to(
    esp_gsp_handle_t gsp, gsp_component_key_t key,
    uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);

/** Open-key animation for any scalar semantic property. */
esp_gsp_err_t esp_gsp_component_animate_property(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    gsp_property_key_t property, const gsp_value_t *from,
    const gsp_value_t *to, uint32_t duration_ms, esp_gsp_ease_t ease);
esp_gsp_err_t esp_gsp_component_animate_property_to(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    gsp_property_key_t property, const gsp_value_t *to,
    uint32_t duration_ms, esp_gsp_ease_t ease);

/** Starts one synchronized x/y tween from the committed position. Both
 *  axes are committed together on every step, so no frame shows the
 *  component at a position between the two. Animating x and y as two
 *  separate properties does not give that guarantee. */
esp_gsp_err_t esp_gsp_component_animate_position_to(
    esp_gsp_handle_t gsp, gsp_component_key_t component,
    int32_t x, int32_t y, uint32_t duration_ms, esp_gsp_ease_t ease);
/** Stops the component's synchronized position tween, if active. The
 *  component stays where the last committed step put it. */
esp_gsp_err_t esp_gsp_component_stop_position_animation(
    esp_gsp_handle_t gsp, gsp_component_key_t component);

/** Same, driving a widget's first VALUE slot. */
esp_gsp_err_t esp_gsp_widget_animate(esp_gsp_handle_t gsp,
                                     esp_gsp_widget_t widget, int32_t from,
                                     int32_t to, uint32_t duration_ms,
                                     esp_gsp_ease_t ease);

#ifdef __cplusplus
}
#endif
