/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once

#include "esp_gsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Canvas commands that have reached the UI core. CHECK validates direct
 * drawing without changing the target. A result does not certify presentation. */
typedef enum {
    ESP_GSP_CANVAS_CHECK = 0,
    ESP_GSP_CANVAS_SET_DRAW,
    ESP_GSP_CANVAS_PUSH_FRAME,
    ESP_GSP_CANVAS_INVALIDATE,
    ESP_GSP_CANVAS_STOP,
} esp_gsp_canvas_operation_t;

typedef struct {
    size_t struct_size; /*!< Runtime payload size; fields may be appended. */
    uint32_t request_id; /*!< Caller tag for check/set_draw_cb_ex; otherwise 0. */
    uint16_t scene_id;
    uint16_t bind;
    esp_gsp_canvas_operation_t operation;
    gsp_err_t status; /*!< GSP_OK or a GSP_ERR_* code, distinct from queue errors. */
} esp_gsp_canvas_result_t;

typedef enum {
    ESP_GSP_EXTENSION_APP_CREATED = 0,
    ESP_GSP_EXTENSION_APP_DESTROYING,
    ESP_GSP_EXTENSION_SCENE_CHANGED,
    ESP_GSP_EXTENSION_ACTION,
    ESP_GSP_EXTENSION_CANVAS_RESULT,
    ESP_GSP_EXTENSION_RENDER_ERROR,
} esp_gsp_extension_kind_t;

/* Explicit masks avoid shifting caller-supplied event numbers. ALL contains
 * the kinds/operations known to this header, not reserved future bits. */
#define ESP_GSP_EXTENSION_MASK_APP_CREATED (UINT32_C(1) << ESP_GSP_EXTENSION_APP_CREATED)
#define ESP_GSP_EXTENSION_MASK_APP_DESTROYING (UINT32_C(1) << ESP_GSP_EXTENSION_APP_DESTROYING)
#define ESP_GSP_EXTENSION_MASK_SCENE_CHANGED (UINT32_C(1) << ESP_GSP_EXTENSION_SCENE_CHANGED)
#define ESP_GSP_EXTENSION_MASK_ACTION (UINT32_C(1) << ESP_GSP_EXTENSION_ACTION)
#define ESP_GSP_EXTENSION_MASK_CANVAS_RESULT (UINT32_C(1) << ESP_GSP_EXTENSION_CANVAS_RESULT)
#define ESP_GSP_EXTENSION_MASK_RENDER_ERROR (UINT32_C(1) << ESP_GSP_EXTENSION_RENDER_ERROR)
#define ESP_GSP_EXTENSION_MASK_ALL (ESP_GSP_EXTENSION_MASK_APP_CREATED | \
    ESP_GSP_EXTENSION_MASK_APP_DESTROYING | ESP_GSP_EXTENSION_MASK_SCENE_CHANGED | \
    ESP_GSP_EXTENSION_MASK_ACTION | ESP_GSP_EXTENSION_MASK_CANVAS_RESULT | \
    ESP_GSP_EXTENSION_MASK_RENDER_ERROR)

#define ESP_GSP_CANVAS_MASK_CHECK (UINT32_C(1) << ESP_GSP_CANVAS_CHECK)
#define ESP_GSP_CANVAS_MASK_SET_DRAW (UINT32_C(1) << ESP_GSP_CANVAS_SET_DRAW)
#define ESP_GSP_CANVAS_MASK_PUSH_FRAME (UINT32_C(1) << ESP_GSP_CANVAS_PUSH_FRAME)
#define ESP_GSP_CANVAS_MASK_INVALIDATE (UINT32_C(1) << ESP_GSP_CANVAS_INVALIDATE)
#define ESP_GSP_CANVAS_MASK_STOP (UINT32_C(1) << ESP_GSP_CANVAS_STOP)
#define ESP_GSP_CANVAS_MASK_ALL (ESP_GSP_CANVAS_MASK_CHECK | ESP_GSP_CANVAS_MASK_SET_DRAW | \
    ESP_GSP_CANVAS_MASK_PUSH_FRAME | ESP_GSP_CANVAS_MASK_INVALIDATE | ESP_GSP_CANVAS_MASK_STOP)

/** Borrowed notification, valid only during its callback. Never retain it.
 * Check struct_size before reading fields added by a newer header/runtime. */
typedef struct {
    size_t struct_size;
    esp_gsp_extension_kind_t kind;
    union {
        struct {
            uint16_t previous;
            uint16_t current;
        } scene;
        esp_gsp_event_t action;
        esp_gsp_canvas_result_t canvas;
        gsp_err_t render_error;
    } data;
} esp_gsp_extension_event_t;

/** Observation only: do not mutate GSP state, block, flush, or stop from this
 * callback. Send a notification to an application worker for follow-up work.
 * Scene/action/Canvas/runtime-render-error events execute on the UI service
 * task (or the caller driving a portable core). Lifecycle notifications use
 * the create/destroy caller's task, which need not be the render task.
 * Notifications are synchronous, not a cross-thread event queue. Different
 * instances can invoke global hooks concurrently; synchronize shared state. */
typedef void (*esp_gsp_extension_cb_t)(esp_gsp_handle_t gsp,
                                       const esp_gsp_extension_event_t *event, void *user_ctx);

/** Register one per-instance extension observer through the command path.
 * NULL clears it; replacement follows esp_gsp_on_event() lifetime rules.
 * Success confirms queue admission. Keep old contexts alive until a successful
 * unregister/replacement followed by flush on the same active runtime, or
 * successful stop. No replay: APP_CREATED precedes observer registration.
 * This observer is independent of the ordinary business event callback.
 * It runs after the corresponding global hook. Native sim_bridge returns
 * ESP_GSP_ERR_NOT_SUPPORTED for these core extension APIs. */
esp_gsp_err_t esp_gsp_on_extension_event(esp_gsp_handle_t gsp,
        esp_gsp_extension_cb_t callback, void *user_ctx);

/** Filtered variant of esp_gsp_on_extension_event(). event_mask selects
 * ESP_GSP_EXTENSION_MASK_* kinds; canvas_operation_mask further selects
 * ESP_GSP_CANVAS_MASK_* operations within CANVAS_RESULT. Zero masks mute those
 * notifications without freeing the registered context. Unknown bits return
 * ESP_GSP_ERR_INVALID_ARG and leave the old registration intact.
 * Masks and callback/context are copied in one registration command. These
 * filters apply only to this instance observer; global hooks are independent.
 * The ordinary registration function continues to subscribe to all events and
 * Canvas operations known to the runtime. Lifetime/thread rules are identical.
 * Native sim_bridge returns ESP_GSP_ERR_NOT_SUPPORTED. */
esp_gsp_err_t esp_gsp_on_extension_event_ex(esp_gsp_handle_t gsp,
        esp_gsp_extension_cb_t callback, void *user_ctx,
        uint32_t event_mask, uint32_t canvas_operation_mask);

/** Queue a non-mutating check of direct-draw target compatibility. The tagged
 * result is delivered to the extension observer/global Canvas hook when the
 * command executes. A successful check is a snapshot, not a reservation.
 * This is an observation stream: commands discarded at shutdown and coalesced
 * frame submissions do not each produce results. Stop producers and flush
 * while the runtime is active when results must be observed. */
esp_gsp_err_t esp_gsp_canvas_check(esp_gsp_handle_t gsp, uint16_t bind,
                                   uint32_t request_id);

/** Tagged variant of esp_gsp_canvas_set_draw_cb(). Submission returns an
 * ESP_GSP_ERR_* queue result; the extension Canvas result carries GSP_OK or
 * GSP_ERR_* from application on the UI task. The tag is copied, not allocated.
 * Install an observer first (or supply the global hook). No callback/context
 * ownership transfers on submission. A rejected registration leaves the
 * previous drawing intact; its context must remain alive. Like existing bind
 * setters, it addresses the current scene when executed, not a pinned scene
 * at submission. The result reports that actual scene; serialize navigation
 * and producer changes when scene identity matters. */
esp_gsp_err_t esp_gsp_canvas_set_draw_cb_ex(esp_gsp_handle_t gsp, uint16_t bind,
        esp_gsp_canvas_draw_cb_t draw_cb, void *user_ctx, uint32_t request_id);

/** Global lifecycle hook after successful core initialization, before platform
 * attachment. A later platform-start failure still destroys this core. Default
 * no-op; observer-only rules above apply. Do not use this notification to
 * start a worker accessing GSP before the public platform start succeeds.
 * On ELF GCC/Clang, override with an
 * ordinary strong definition including this header. Definitions in a static
 * archive need a separately named strong link anchor in the same object or
 * explicit direct/whole-archive linking; -u on the hook alone is insufficient
 * because the default implementation already defines it. All hooks use C
 * linkage. Other toolchains retain defaults and can use instance observers. */
void esp_gsp_hook_app_created(esp_gsp_handle_t gsp);

/** Global hook once before a successfully initialized core is destroyed.
 * The handle is borrowed for this call; do not store it for later use. Partial
 * initialization failures that never emitted APP_CREATED emit neither hook.
 * Resource release callbacks may still follow; free shared application state
 * only after the whole stop/destroy operation has completed. */
void esp_gsp_hook_app_destroying(esp_gsp_handle_t gsp);

/** Global hook after the scene is committed and before its business event.
 * The change cannot be vetoed; it does not certify that pixels were presented. */
void esp_gsp_hook_scene_changed(esp_gsp_handle_t gsp,
                                uint16_t previous_scene, uint16_t current_scene);

/** Global observer of authored CALL actions reaching business dispatch,
 * before the ordinary application handler (also when none is installed).
 * Internally consumed keyboard edits do not reach this hook. It cannot
 * consume or change the borrowed event. */
void esp_gsp_hook_action(esp_gsp_handle_t gsp, const esp_gsp_event_t *event);

/** Global observer of Canvas commands processed by the UI core. Not an
 * exactly-once completion API for all queued frames; see canvas_check(). */
void esp_gsp_hook_canvas_result(esp_gsp_handle_t gsp,
                                const esp_gsp_canvas_result_t *result);

/** Global hook for errors reported by the managed ESP-IDF render loop or the
 * managed simulator session render entry. Direct low-level renderer calls are outside
 * this contract. Reporting supplements existing counters and return values. */
void esp_gsp_hook_render_error(esp_gsp_handle_t gsp, gsp_err_t error);

#ifdef __cplusplus
}
#endif
