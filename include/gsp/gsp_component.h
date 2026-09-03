/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once
#include "gsp/gsp_build_caps.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gsp/gsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Stable component keys resolve to typed properties without the
 * application touching bind ids.
 *
 * The backend is a compiler-generated, flash-resident directory (the
 * table gspc emits into <scene>_objects.h): one entry per retained
 * (named) component, each property mapping to an EXISTING bind — the
 * state slot stays the single runtime truth and the compiled scene
 * bytes never change.
 *
 * Views are stack-only: resolve, use, drop. They must not outlive a
 * scene switch.
 */

/** Stable component key (FNV-1a of the authored object name; the
 *  generated header emits GSP_OBJ_KEY_<NAME> constants). */
typedef uint32_t gsp_component_key_t;

/** Stable semantic property key (FNV-1a of the lower_snake_case property
 *  name). Generated headers emit GSP_PROP_KEY_<NAME>; the common keys below
 *  keep generic code independent from generated scene headers. */
typedef uint32_t gsp_property_key_t;
#define GSP_PROPERTY_KEY_VALUE    UINT32_C(0x425ed3ca)
#define GSP_PROPERTY_KEY_COLOR    UINT32_C(0x3d7e6258)
#define GSP_PROPERTY_KEY_TEXT     UINT32_C(0xbde64e3e)
#define GSP_PROPERTY_KEY_RESOURCE UINT32_C(0x29df7ff5)
#define GSP_PROPERTY_KEY_VISIBLE  UINT32_C(0x54fa99a1)
#define GSP_PROPERTY_KEY_CHECKED  UINT32_C(0x21114ec6)
#define GSP_PROPERTY_KEY_X        UINT32_C(0xfd0c5087)
#define GSP_PROPERTY_KEY_Y        UINT32_C(0xfc0c4ef4)
#define GSP_PROPERTY_KEY_SCALE    UINT32_C(0x82971c71)
#define GSP_PROPERTY_KEY_ENABLED  UINT32_C(0x02f3b39e)
#define GSP_PROPERTY_KEY_SELECTED UINT32_C(0x4e0a1774)

/** Compatibility property kinds for source-compatible generated directories.
 *  New generated descriptors also carry an open stable `key`, explicit value
 *  type, storage, codec and bounds; the component core does not infer their
 *  meaning from this enum or from the component kind. CHECKED is
 *  the semantic on/off of boolean-family components: it reads and
 *  writes the same slot as VALUE (the knob rail) but speaks bool, so
 *  applications never learn the 0..100 rail convention. Append-only
 *  after release. */
typedef enum {
    GSP_COMPONENT_PROP_VALUE = 0,
    GSP_COMPONENT_PROP_COLOR = 1,
    GSP_COMPONENT_PROP_TEXT = 2,
    GSP_COMPONENT_PROP_RESOURCE = 3,
    GSP_COMPONENT_PROP_VISIBLE = 4,
    GSP_COMPONENT_PROP_CHECKED = 5,
    GSP_COMPONENT_PROP_ENABLED = 6,
    GSP_COMPONENT_PROP_SELECTED = 7,
} gsp_component_prop_kind_t;

/** Authoring component kind carried by the descriptor. New keyed properties
 *  do not depend on it; it remains for list drivers and legacy zero-key
 *  property adaptation. GENERIC for kinds without runtime services yet.
 *  Append-only after release. */
typedef enum {
    GSP_COMPONENT_KIND_GENERIC = 0,
    GSP_COMPONENT_KIND_TOGGLE = 1,
    GSP_COMPONENT_KIND_LIST = 2,
    GSP_COMPONENT_KIND_WHEEL = 3,
    GSP_COMPONENT_KIND_RANGE = 4,   /*!< slider/arc/progress */
    GSP_COMPONENT_KIND_PAGE_FLOW = 5,
    GSP_COMPONENT_KIND_STACK_VIEW = 6,
    GSP_COMPONENT_KIND_DRAWER = 7,
    GSP_COMPONENT_KIND_MESSAGE_LIST = 8,
    GSP_COMPONENT_KIND_GRID = 9,
    GSP_COMPONENT_KIND_DROPDOWN = 10,
} gsp_component_kind_t;

typedef enum {
    GSP_GESTURE_AXIS_HORIZONTAL = 0,
    GSP_GESTURE_AXIS_VERTICAL = 1,
} gsp_gesture_axis_t;

/** Compiler-owned geometry for a standard PageFlow gesture component. */
typedef struct {
    uint16_t group_id;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t page_count;
    uint16_t selected;
    uint8_t axis;               /*!< gsp_gesture_axis_t */
    uint8_t cyclic;
    /* Nearest ancestor visibility state plus one (zero = ungated).
     * A PageFlow on a hidden authored page must not capture a scene swipe
     * through that page. The +1 encoding keeps zero-initialized configs
     * backward compatible. */
    uint16_t vis_slot_ref;
    uint8_t stop_anywhere;
    uint8_t reserved;
    uint16_t page_extent;
} gsp_page_flow_config_t;

#define GSP_STACK_VIEW_MAX_DEPTH 64U

typedef enum {
    GSP_STACK_VIEW_EASE_LINEAR = 0,
    GSP_STACK_VIEW_EASE_OUT = 1,
    GSP_STACK_VIEW_EASE_IN_OUT = 2,
} gsp_stack_view_ease_t;

/** Compiler-owned geometry for a fixed-capacity in-scene page stack. */
typedef struct {
    uint16_t group_id;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t page_count;
    uint16_t initial_page;
    uint8_t axis;               /*!< gsp_gesture_axis_t */
    uint8_t capacity;           /*!< <= GSP_STACK_VIEW_MAX_DEPTH */
    uint16_t transition_ms;     /*!< 0 keeps the legacy 220 ms default */
    uint8_t transition_easing;  /*!< gsp_stack_view_ease_t */
    uint8_t reserved;
} gsp_stack_view_config_t;

/** Compiler-owned geometry for an overlay drawer transform group. */
typedef struct {
    uint16_t group_id;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t edge;               /*!< gsp_drawer_edge_t */
    uint8_t initially_open;
} gsp_drawer_config_t;

typedef enum {
    GSP_DRAWER_EDGE_TOP = 0,
    GSP_DRAWER_EDGE_BOTTOM = 1,
    GSP_DRAWER_EDGE_LEFT = 2,
    GSP_DRAWER_EDGE_RIGHT = 3,
} gsp_drawer_edge_t;

/** True when the drawer travels along Y, so callers know which axis of
 *  the transform group carries its offset. */
static inline bool gsp_drawer_axis_is_y(const gsp_drawer_config_t *config)
{
    return config->edge == GSP_DRAWER_EDGE_TOP ||
           config->edge == GSP_DRAWER_EDGE_BOTTOM;
}

/**
 * Transform-group offset that parks the drawer off-screen against its
 * edge; open is always offset zero.
 *
 * The compiler bakes this same value into the scene's initial state and
 * the gesture layer measures drag progress against it, so it has to be
 * one definition — a driver and an input layer that disagree about
 * where "closed" is produce a drawer that snaps to the wrong end.
 */
static inline int32_t gsp_drawer_closed_offset(
    const gsp_drawer_config_t *config)
{
    switch (config->edge) {
    case GSP_DRAWER_EDGE_TOP:
        return -(int32_t)config->height;
    case GSP_DRAWER_EDGE_BOTTOM:
        return config->height;
    case GSP_DRAWER_EDGE_LEFT:
        return -(int32_t)config->width;
    default:
        return config->width;
    }
}

/** Typed config of RANGE entries with an authored `min`/`max`: the
 *  VALUE property speaks authored units and the core maps them onto
 *  the internal 0..100 state rail. A NULL config means the rail IS
 *  the authored range. */
typedef struct {
    int32_t min;
    int32_t max;                /*!< always above min */
} gsp_range_config_t;

/** gsp_list_config_t.vis_slot: the list has no visibility gate. */
#define GSP_LIST_VIS_SLOT_NONE UINT16_MAX

/** Typed config of LIST/WHEEL entries (entry->config): the compiler's
 *  row-template association, so applications never re-pair template
 *  ids, viewports and row geometry by hand. */
typedef struct {
    uint16_t template_id;
    int16_t x;                  /*!< viewport origin (scene space) */
    int16_t y;
    uint16_t width;             /*!< viewport size */
    uint16_t height;
    uint16_t row_height;
    uint32_t item_count;        /*!< authored item count */
    /* Nearest ancestor visibility state slot (the authored tree
     * collapsed to one scalar, same chain the hit table uses): a
     * hidden page's list neither swallows drags nor needs manual
     * parking. GSP_LIST_VIS_SLOT_NONE when ungated. */
    uint16_t vis_slot;
    /* Authored item texts (item_count entries, flash) — the default
     * row source for fixed lists; NULL when the list is dynamic. */
    const char *const *items;
    uint16_t transform_group_ref; /*!< parent transform group + 1; 0 for root */
    bool scroll_snapshot;      /*!< opt-in cached viewport scrolling */
} gsp_list_config_t;

/** Compiler-owned geometry for a vertically scrolling fixed-column grid. */
typedef struct {
    gsp_list_config_t list;
    uint16_t columns;
    uint16_t cell_width;
    uint16_t cell_height;
    uint16_t column_gap;
    uint16_t row_gap;
    /** First convenience slot, UINT16_MAX when the cell has none. */
    uint16_t resource_slot;
    uint16_t text_slot;
} gsp_grid_config_t;

/** Compiler-owned message facade over a variable-height List template.
 *  Scrolling uses the List/PageFlow viewport-snapshot path when
 *  list.scroll_snapshot is true (the message_list compiler default) and
 *  falls back to live replay if capture is unavailable. Set
 *  scroll_snapshot false to keep live replay when snapshot RAM is
 *  insufficient. */
typedef struct {
    gsp_list_config_t list;
    uint16_t bubble_x_slot;
    uint16_t bubble_w_slot;
    uint16_t bubble_h_slot;
    uint16_t bubble_color_slot;
    uint16_t text_slot;
    uint16_t text_color_slot;
    uint16_t padding_x;
    uint16_t padding_y;
    uint16_t gap;
    uint16_t side_margin;
    uint16_t max_bubble_width;
    uint16_t max_message_height;
    uint32_t incoming_color;
    uint32_t outgoing_color;
    uint32_t incoming_text_color;
    uint32_t outgoing_text_color;
} gsp_message_list_config_t;

/** Compiler-owned option mapping for a retained Dropdown selection. */
typedef struct {
    uint16_t text_bind;
    uint16_t option_count;
    const char *const *options;
} gsp_dropdown_config_t;

/** Typed property value (bounded tagged union; structured content
 *  keeps component-specific typed APIs). */
typedef enum {
    GSP_VALUE_BOOL = 0,
    GSP_VALUE_I32 = 1,
    GSP_VALUE_U32 = 2,
    GSP_VALUE_COLOR = 3,
} gsp_value_type_t;

typedef struct {
    gsp_value_type_t type;
    union {
        bool boolean;
        int32_t i32;
        uint32_t u32;
        uint32_t color;
    } data;
} gsp_value_t;

/** Closed state-slot storage families. Semantic property keys remain open;
 *  a new property normally reuses one of these storage/codec combinations. */
typedef enum {
    GSP_PROPERTY_STORAGE_I32 = 0,
    GSP_PROPERTY_STORAGE_U32 = 1,
    GSP_PROPERTY_STORAGE_COLOR = 2,
    GSP_PROPERTY_STORAGE_TEXT = 3,
    GSP_PROPERTY_STORAGE_RESOURCE = 4,
    GSP_PROPERTY_STORAGE_VISIBLE = 5,
} gsp_property_storage_t;

/** Public value <-> state-slot conversion. */
typedef enum {
    GSP_PROPERTY_CODEC_DIRECT = 0,
    GSP_PROPERTY_CODEC_BOOL_0_1 = 1,
    GSP_PROPERTY_CODEC_BOOL_0_100 = 2,
    GSP_PROPERTY_CODEC_RANGE_0_100 = 3,
} gsp_property_codec_t;

/** gsp_component_prop_t.property_ref: no direct GSB 1.1 property.
 *  Zero is deliberately the compatibility sentinel so legacy generated
 *  headers that omit the appended field remain valid. */
#define GSP_COMPONENT_PROPERTY_REF_NONE UINT16_C(0)
#define GSP_COMPONENT_BIND_NONE UINT16_MAX

/** One property of one component. `bind` is the stable compatibility
 *  route. `property_ref` is property_index + 1 for GSB 1.1 and lets the
 *  component core update state and damage dependencies without searching
 *  the bind table. */
typedef struct {
    uint8_t prop;               /*!< gsp_component_prop_kind_t */
    uint8_t reserved;
    uint16_t bind;
    uint16_t property_ref;      /*!< property index + 1, or zero */
    uint8_t value_type;         /*!< gsp_value_type_t */
    uint8_t storage;            /*!< gsp_property_storage_t */
    uint8_t codec;              /*!< gsp_property_codec_t */
    uint8_t flags;              /*!< reserved; zero */
    uint16_t reserved2;
    gsp_property_key_t key;     /*!< zero in legacy generated headers */
    uint32_t min_value;         /*!< raw bound; signed for I32 values */
    uint32_t max_value;         /*!< raw bound; signed for I32 values */
} gsp_component_prop_t;

/** Stable, allocation-free property metadata for dynamic tooling and generic
 *  application code. Generated typed APIs normally make this unnecessary. */
typedef struct {
    gsp_property_key_t key;
    gsp_value_type_t value_type;
    gsp_property_storage_t storage;
    gsp_property_codec_t codec;
    int64_t min_value;
    int64_t max_value;
} gsp_property_info_t;

/** One retained component (generated, flash-resident, key-sorted). */
typedef struct {
    gsp_component_key_t key;
    uint8_t kind;               /*!< gsp_component_kind_t */
    uint8_t prop_count;
    uint16_t reserved;
    const gsp_component_prop_t *props;
    /* Kind-typed compile-time config (LIST/WHEEL: gsp_list_config_t);
     * NULL for kinds without one. */
    const void *config;
} gsp_component_entry_t;

typedef struct {
    const gsp_component_entry_t *entries;   /*!< sorted by key */
    uint16_t count;
    /** Compiler-generated bind -> entry index map, sorted by bind.
     *  Optional for source compatibility with legacy generated headers. */
    const struct gsp_component_route *routes;
    uint16_t route_count;
    /** GSB 1.1 property_ref -> entry index map, sorted by property_ref. */
    const struct gsp_component_property_route *property_routes;
    uint16_t property_route_count;
} gsp_component_directory_t;

typedef struct gsp_component_route {
    uint16_t bind;
    uint16_t entry;
} gsp_component_route_t;

typedef struct gsp_component_property_route {
    uint16_t property_ref;      /*!< property index + 1 */
    uint16_t entry;
} gsp_component_property_route_t;

/** One entry for gsp_component_set_many(): stable key + typed property. */
typedef struct {
    gsp_component_key_t key;
    gsp_component_prop_kind_t prop;
    gsp_value_t value;
} gsp_component_update_t;

/** Open-key batch entry for component-local/custom properties. */
typedef struct {
    gsp_component_key_t component;
    gsp_property_key_t property;
    gsp_value_t value;
} gsp_component_property_update_t;

#ifdef __cplusplus
}
#endif
