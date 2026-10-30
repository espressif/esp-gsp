/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once

#include "esp_gsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_gsp_update_receipt esp_gsp_update_receipt_t;

#define ESP_GSP_UPDATE_API_VERSION 1U

typedef enum {
    ESP_GSP_UPDATE_ACCEPTED = 1,
    ESP_GSP_UPDATE_APPLIED,
    ESP_GSP_UPDATE_RENDERED,
    ESP_GSP_UPDATE_TRANSFERRED,
    ESP_GSP_UPDATE_PRESENTED,
} esp_gsp_update_stage_t;

typedef enum {
    ESP_GSP_UPDATE_PENDING = 0,
    ESP_GSP_UPDATE_COMPLETE,
    ESP_GSP_UPDATE_FAILED,
    ESP_GSP_UPDATE_SUPERSEDED,
    ESP_GSP_UPDATE_CANCELLED,
    ESP_GSP_UPDATE_TIMED_OUT,
} esp_gsp_update_result_t;

/** Evidence reached by a status snapshot, or available from capabilities.
 * FRAMEBUFFER follows the backend's correlated hardware refresh events;
 * optical output is checked separately. */
typedef enum {
    ESP_GSP_UPDATE_CONFIRM_NONE = 0,
    ESP_GSP_UPDATE_CONFIRM_RENDERED,
    ESP_GSP_UPDATE_CONFIRM_TRANSFER,
    ESP_GSP_UPDATE_CONFIRM_FRAMEBUFFER,
} esp_gsp_update_confirmation_t;

typedef enum {
    ESP_GSP_UPDATE_PROPERTY = 0,
    ESP_GSP_UPDATE_VALUE,
    ESP_GSP_UPDATE_COLOR,
    ESP_GSP_UPDATE_VISIBLE,
    ESP_GSP_UPDATE_TEXT,
    ESP_GSP_UPDATE_IMAGE,
    ESP_GSP_UPDATE_SCENE,
} esp_gsp_update_kind_t;

/** One required operation. Binds are generated bind IDs. PROPERTY instead
 * addresses a component and semantic property key. SCENE changes the context
 * used by subsequent operations and must be the first command. Repeated
 * commands with the same kind and bind, or the same component/property for
 * PROPERTY, require the last value. Images use set_image_ex's
 * COPY/BORROW/TAKE lifetime rules, with acceptance referring to the group. */
typedef struct {
    esp_gsp_update_kind_t kind;
    uint16_t bind;
    union {
        gsp_component_property_update_t property;
        int32_t value;
        uint32_t color;
        bool visible;
        const char *text;
        struct {
            const void *data;
            size_t size;
            esp_gsp_image_options_t options;
        } image;
        struct {
            uint16_t id;
            esp_gsp_transition_t transition;
        } scene;
    };
} esp_gsp_update_command_t;

typedef struct {
    uint64_t id;
    uint64_t frame_id;           /**< Zero until a matching frame is rendered. */
    esp_gsp_update_stage_t stage;
    esp_gsp_update_result_t result;
    esp_gsp_update_confirmation_t confirmation;
    gsp_err_t error;             /**< Terminal error; GSP_OK while pending. */
    uint32_t command_count;
    uint32_t applied_count;      /**< Required operations applied at this snapshot. */
    uint32_t failed_command;     /**< UINT32_MAX for a group-wide failure. */
    uint32_t scene_generation;
    uint32_t frame_generation;   /**< Backend binding epoch paired with frame_id. */
    uint16_t scene_id;
    uint16_t failed_stage;       /**< Stage that could not be established; zero except FAILED/TIMED_OUT. */
} esp_gsp_update_status_t;

/** Terminal notification on the UI/render task, or the caller of stop when
 * shutdown cancels outstanding work. Keep it short; do not flush, wait for
 * this UI or stop it here. ctx remains alive until notification,
 * including when the caller releases its receipt reference early. */
typedef void (*esp_gsp_update_complete_cb_t)(
    const esp_gsp_update_status_t *status, void *ctx);

typedef struct {
    esp_gsp_update_stage_t complete_at; /**< Zero selects RENDERED. */
    esp_gsp_update_complete_cb_t on_complete;
    void *user_ctx;
    uint32_t timeout_ms;         /**< Acceptance-to-evidence deadline; zero disables it. */
} esp_gsp_update_options_t;

typedef struct {
    esp_gsp_update_stage_t max_stage;
    esp_gsp_update_confirmation_t confirmation;
    uint32_t max_commands;
    uint32_t max_pending;
} esp_gsp_update_capabilities_t;

esp_gsp_err_t esp_gsp_update_capabilities(
    esp_gsp_handle_t gsp, esp_gsp_update_capabilities_t *out);

/** Submit one required group without waiting for queue space. Commands and
 * UTF-8 strings are copied; COPY image inputs are copied once. On an immediate
 * error out_receipt is NULL and BORROW/TAKE ownership remains with the caller.
 * Unsupported completion stages are rejected before acceptance.
 * Receipt IDs are not reused; exhaustion rejects admission with INVALID_STATE.
 * out_receipt is installed before an accepted command can notify, including
 * when the render task runs before this function returns.
 *
 * Pure PROPERTY groups retain the existing atomic property transaction.
 * Mixed groups execute in order and report partial failure without rollback.
 * APPLIED requires every operation, including image publication, to succeed.
 * A target overwritten before its matching frame is SUPERSEDED.
 * An unchanged target may reuse a matching, previously committed frame.
 * A failed render attempt terminates the receipt without stopping ordinary
 * rendering. timeout_ms includes queueing and pauses; expiry is observed on
 * the next UI service, without undoing state or releasing in-flight data.
 */
esp_gsp_err_t esp_gsp_update_submit(
    esp_gsp_handle_t gsp, const esp_gsp_update_command_t *commands, size_t count,
    const esp_gsp_update_options_t *options,
    esp_gsp_update_receipt_t **out_receipt);

/** Query while owning a receipt reference, including after its UI has stopped.
 * A contended snapshot returns false without changing out; retry later. */
bool esp_gsp_update_query(const esp_gsp_update_receipt_t *receipt,
                          esp_gsp_update_status_t *out);

/** Request cancellation. Terminal cancellation is observed through query or
 * notification; it does not undo previously applied state or reclaim data
 * still referenced by a decoder, transfer or display buffer. */
void esp_gsp_update_cancel(esp_gsp_update_receipt_t *receipt);

/** Release the caller's reference. This does not cancel accepted work. */
void esp_gsp_update_release(esp_gsp_update_receipt_t *receipt);

#ifdef __cplusplus
}
#endif
