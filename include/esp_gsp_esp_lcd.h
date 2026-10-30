/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

/* ESP-IDF convenience glue: create the portable GSP UI core, attach
 * the ESP LCD display backend, then run it from a FreeRTOS task.
 *
 * Panel classification, framebuffer ownership and anti-tearing mode
 * belong to esp_display_present. This header owns only the ESP-IDF
 * application lifecycle wrapper.
 */

#include "esp_gsp.h"
#include "esp_display_present_config.h"
#include "esp_lcd_touch.h"

struct esp_display_presenter;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_GSP_TOUCH_INPUT_AUTO = 0,
    ESP_GSP_TOUCH_INPUT_POLLING,
    ESP_GSP_TOUCH_INPUT_INTERRUPT,
} esp_gsp_touch_input_mode_t;

/** ISR-safe notification invoked when the borrowed touch controller asserts
 * its interrupt. The callback must not perform I2C or GSP operations. */
typedef void (*esp_gsp_esp_lcd_touch_wake_from_isr_cb_t)(void *user_ctx);

typedef struct {
    /** Set to sizeof(esp_gsp_esp_lcd_config_t). Initialize new configurations
     *  with ESP_GSP_ESP_LCD_CONFIG_INIT(). */
    size_t struct_size;
    esp_display_present_target_config_t display;
    /** Optional borrowed presenter. When set, display target is ignored. */
    struct esp_display_presenter *presenter;
    esp_lcd_touch_handle_t touch;       /*!< optional input adapter */
    esp_gsp_touch_input_mode_t touch_input_mode; /*!< auto by default */
    esp_gsp_esp_lcd_touch_wake_from_isr_cb_t touch_wake_from_isr;
    void *touch_wake_user_ctx;
    uint32_t task_stack_size;       /*!< default 12288, or 32768 with active dynamic text */
    uint32_t task_stack_size_freetype; /*!< default 32768; runtime enforces the linked provider minimum */
    uint32_t task_priority;         /*!< default 4 */
    bool task_stack_psram;          /*!< allocate render stack from PSRAM */
    bool perf_log;                  /*!< log rendered fps every 5 s */
    /** Producer raster-area alignment; zero fields mean no expansion. */
    esp_display_present_render_alignment_t render_alignment;
} esp_gsp_esp_lcd_config_t;

static inline esp_gsp_esp_lcd_config_t esp_gsp_esp_lcd_config_init(void)
{
#ifdef __cplusplus
    esp_gsp_esp_lcd_config_t config = {};
#else
    esp_gsp_esp_lcd_config_t config = { 0 };
#endif
    config.struct_size = sizeof(config);
    return config;
}

#define ESP_GSP_ESP_LCD_CONFIG_INIT() esp_gsp_esp_lcd_config_init()

/** Opaque lease for a retained hub and its resident LCD render task. */
typedef struct esp_gsp_esp_lcd_session esp_gsp_esp_lcd_session_t;

/** Opaque pause token used while another display producer owns the presenter. */
typedef struct esp_gsp_esp_lcd_pause esp_gsp_esp_lcd_pause_t;

typedef enum {
    ESP_GSP_ESP_LCD_STAGE_NONE,
    ESP_GSP_ESP_LCD_STAGE_VALIDATE,
    ESP_GSP_ESP_LCD_STAGE_PAUSE_APP,
    ESP_GSP_ESP_LCD_STAGE_CREATE_APP,
    ESP_GSP_ESP_LCD_STAGE_ACTIVATE_APP,
    ESP_GSP_ESP_LCD_STAGE_RESUME_HUB,
} esp_gsp_esp_lcd_session_stage_t;

typedef enum {
    ESP_GSP_ESP_LCD_SESSION_UNCHANGED,    /*!< No input ownership was changed. */
    ESP_GSP_ESP_LCD_SESSION_APP_RETAINED, /*!< Session and previous App still owned; not a running/quiescence guarantee. */
    ESP_GSP_ESP_LCD_SESSION_APP_STARTED,  /*!< Session retained, new App is in app. */
    ESP_GSP_ESP_LCD_SESSION_EMPTY,        /*!< Session retained without a temporary App. */
    ESP_GSP_ESP_LCD_SESSION_HUB_RESUMED,  /*!< Session consumed, original Hub is in hub. */
} esp_gsp_esp_lcd_session_outcome_t;

/** Public lifetime result, valid independently of a possibly consumed Token.
 * For replace/resume, APP_RETAINED requires keeping the previous App's callback
 * contexts alive. APP_STARTED, EMPTY or HUB_RESUMED means a supplied previous
 * temporary GSP App was destroyed; external producer cleanup remains the caller's.
 * UNCHANGED leaves all input lifetimes as they were; do not interpret NULL app
 * in that outcome as evidence that a previously valid App was destroyed.
 * stage identifies the primary failure; recovery_error is the raw automatic
 * Hub-recovery result after a start/replace failure (ESP_OK when not attempted).
 */
typedef struct {
    uint32_t struct_size;
    esp_gsp_esp_lcd_session_stage_t stage;
    esp_gsp_esp_lcd_session_outcome_t outcome;
    esp_err_t operation_error;
    esp_err_t recovery_error;
    esp_gsp_handle_t app;
    esp_gsp_handle_t hub;
} esp_gsp_esp_lcd_session_result_t;

static inline esp_gsp_esp_lcd_session_result_t esp_gsp_esp_lcd_session_result_init(void)
{
#ifdef __cplusplus
    esp_gsp_esp_lcd_session_result_t result = {};
#else
    esp_gsp_esp_lcd_session_result_t result = {0};
#endif
    result.struct_size = sizeof(result);
    return result;
}

#define ESP_GSP_ESP_LCD_SESSION_RESULT_INIT() esp_gsp_esp_lcd_session_result_init()

/** Configure an activated UI on its render task before its first frame.
 * The UI clock is synchronized before invocation. Setters execute on that
 * task; keep the callback short and check their return values. Do not call
 * flush, stop, pause, suspend or replace from this callback. It has no error
 * return and cannot cancel startup; application-owned registrations and their
 * contexts must remain valid until startup completes or fails.
 */
typedef void (*esp_gsp_esp_lcd_prepare_cb_t)(esp_gsp_handle_t gsp,
        void *user_ctx);

/**
 * Quiesce the active GSP render task without destroying its UI state.
 *
 * On success no new GSP frame can be submitted until
 * esp_gsp_esp_lcd_resume_paused() is called. The function also waits for the
 * shared presenter transfer/present fences to retire.
 * Decoded image caches are released while the scene and source ownership are
 * retained. Resume prepares the current scene again before rendering; a paused
 * UI is not a prepared image-cache snapshot for direct renderer calls.
 * ESP_ERR_NOT_FINISHED means pause rollback could not restore the original
 * app: *out_pause contains a valid token for retry with resume_paused(). This
 * does not certify quiescence; do not hand the display to another producer.
 * Other failures do not transfer a pause token. A non-NULL out_pause is cleared
 * before argument validation; any previously stored token is not freed.
 */
esp_err_t esp_gsp_esp_lcd_pause(
    esp_gsp_handle_t gsp,
    uint32_t timeout_ms,
    esp_gsp_esp_lcd_pause_t **out_pause);

/**
 * Resume the exact UI paused by esp_gsp_esp_lcd_pause(), including a temporary
 * app running on a retained hub session. The pause token is consumed on
 * success and the first resumed frame is forced to redraw in full.
 * On failure the token retains the original app for another resume attempt;
 * do not free its borrowed resources or discard the token.
 */
esp_err_t esp_gsp_esp_lcd_resume_paused(
    esp_gsp_esp_lcd_pause_t *pause,
    esp_gsp_handle_t *out_gsp);

/**
 * Pause a UI while retaining its scenes and component state. Its render task
 * and LCD host remain resident; decoded image surfaces are released.
 * ESP_ERR_NOT_FINISHED means rollback to the hub also failed: *out_session
 * contains a valid session with no temporary app. Retain it and retry with
 * esp_gsp_esp_lcd_resume(session, NULL, ...) or destroy it. Other failures do
 * not transfer a session to the caller.
 */
esp_err_t esp_gsp_esp_lcd_suspend(
    esp_gsp_handle_t hub,
    esp_gsp_esp_lcd_session_t **out_session);

/**
 * Opt-in release of the inactive viewport snapshot atlas on a suspended Hub.
 * Call before start_on_session(), while the session has no temporary app.
 * Scenes, component state, borrowed sources, fonts and display buffers remain
 * owned as before. Ordinary pause/suspend retain their existing cache policy.
 *
 * Runs on the resident render task after Presenter transfer/present and
 * accelerator fences. timeout_ms bounds the Presenter wait; task dispatch and
 * accelerator synchronization can take longer. Call from an application task,
 * serializing all lifecycle calls on the session/runtime. Zero timeout is invalid.
 *
 * Optional out_released_bytes counts atlas payload, excluding allocator overhead;
 * it is set to zero before validation and on failure. Repeated successful calls
 * release zero bytes. Every result retains the session/token and UI ownership;
 * retry or resume after failure. ESP_ERR_NOT_FINISHED means accelerator reads
 * could not be confirmed retired; no atlas is freed. Success invalidates cached
 * pixels and forces a complete resumed frame; later gestures recapture normally.
 */
esp_err_t esp_gsp_esp_lcd_trim_session(
    esp_gsp_esp_lcd_session_t *session, uint32_t timeout_ms,
    size_t *out_released_bytes);

/** Same atlas-only operation for the exact UI retained by pause(). The pause
 *  token is never consumed; resume_paused() is still required. A token returned
 *  with ESP_ERR_NOT_FINISHED is accepted only if its runtime is actually paused
 *  and all fences can now retire. See trim_session() for context and timeouts. */
esp_err_t esp_gsp_esp_lcd_trim_paused(
    esp_gsp_esp_lcd_pause_t *pause, uint32_t timeout_ms,
    size_t *out_released_bytes);

/** Start an app bundle on a suspended hub's LCD host. The bundle must match
 *  the display width, height and pixel format. A startup failure attempts to
 *  resume the hub. Successful recovery consumes the session and returns the
 *  startup error; ESP_ERR_NOT_FINISHED means recovery also failed and the
 *  session remains owned by the caller with no temporary app. Retry with
 *  esp_gsp_esp_lcd_resume(session, NULL, ...) or destroy the session.
 *  ESP_ERR_INVALID_SIZE means the resident task stack cannot accommodate the
 *  new app; create a larger session. Calls rejected by initial argument or
 *  session-state validation do not consume the session. */
esp_err_t esp_gsp_esp_lcd_start_on_session(
    esp_gsp_esp_lcd_session_t *session,
    const esp_gsp_config_t *app_config,
    esp_gsp_handle_t *out_gsp);

/**
 * Start an app and invoke @p prepare on the resident render task before the
 * first frame. This is intended for installing generic runtime contributors,
 * event handlers and timers without racing the initial render.
 * Session ownership and failure recovery follow start_on_session().
 */
esp_err_t esp_gsp_esp_lcd_start_on_session_prepared(
    esp_gsp_esp_lcd_session_t *session,
    const esp_gsp_config_t *app_config,
    esp_gsp_esp_lcd_prepare_cb_t prepare,
    void *prepare_ctx,
    esp_gsp_handle_t *out_gsp);

/** Replace the active temporary app while retaining the suspended hub.
 * Unlike esp_gsp_esp_lcd_resume(), this never reactivates or presents the
 * hub between the two Apps. Startup failure follows start_on_session():
 * successful hub recovery consumes the session; ESP_ERR_NOT_FINISHED retains
 * an empty session for resume(session, NULL, ...) or destruction. Failure to
 * suspend the current app leaves that app and session intact. */
esp_err_t esp_gsp_esp_lcd_replace_on_session_prepared(
    esp_gsp_esp_lcd_session_t *session,
    esp_gsp_handle_t current_app,
    const esp_gsp_config_t *next_config,
    esp_gsp_esp_lcd_prepare_cb_t prepare,
    void *prepare_ctx,
    esp_gsp_handle_t *out_gsp);

/**
 * Stop the temporary app, then resume the retained hub and force its first
 * frame to redraw. The session is consumed only on success. Pass NULL for app
 * when no temporary app is active, including recovery after
 * ESP_ERR_NOT_FINISHED. A pause failure leaves the temporary app alive and
 * returns that error. ESP_ERR_NOT_FINISHED means the temporary app is gone
 * but hub recovery failed; retain the session and retry with app=NULL.
 */
esp_err_t esp_gsp_esp_lcd_resume(
    esp_gsp_esp_lcd_session_t *session,
    esp_gsp_handle_t app,
    esp_gsp_handle_t *out_hub);

/** Ownership-aware alternatives to the legacy session operations.
 * *inout_session is set to NULL exactly when the SDK consumes the Token;
 * otherwise it remains owned by the caller. Optional result must be initialized
 * with ESP_GSP_ESP_LCD_SESSION_RESULT_INIT() and the exact struct_size.
 * A size mismatch rejects the call before any effects and leaves result intact.
 * With a valid size every return resets/populates result, including validation
 * failure. A NULL result keeps the same error codes and pointer ownership update.
 *
 * Error codes alone are insufficient for legacy replace(): pause failure and
 * startup failure followed by successful Hub recovery can return the same code.
 * Use outcome and the public App/Hub handles, never probe a consumed Token.
 * On EMPTY after replace/resume clear the destroyed old App Handle before retry.
 * All lifecycle calls on the runtime still require application-task serialization.
 */
esp_err_t esp_gsp_esp_lcd_start_on_session_ex(
    esp_gsp_esp_lcd_session_t **inout_session,
    const esp_gsp_config_t *app_config, esp_gsp_handle_t *out_gsp,
    esp_gsp_esp_lcd_session_result_t *result);

/** Start an App with a prepare callback before its first frame and report Session ownership. */
esp_err_t esp_gsp_esp_lcd_start_on_session_prepared_ex(
    esp_gsp_esp_lcd_session_t **inout_session,
    const esp_gsp_config_t *app_config,
    esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx,
    esp_gsp_handle_t *out_gsp, esp_gsp_esp_lcd_session_result_t *result);

/** Replace the temporary App, prepare the replacement before its first frame and report Session ownership. */
esp_err_t esp_gsp_esp_lcd_replace_on_session_prepared_ex(
    esp_gsp_esp_lcd_session_t **inout_session, esp_gsp_handle_t current_app,
    const esp_gsp_config_t *next_config,
    esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx,
    esp_gsp_handle_t *out_gsp, esp_gsp_esp_lcd_session_result_t *result);

/** Stop the temporary App, resume the Hub and report Session ownership. */
esp_err_t esp_gsp_esp_lcd_resume_ex(
    esp_gsp_esp_lcd_session_t **inout_session, esp_gsp_handle_t app,
    esp_gsp_handle_t *out_hub, esp_gsp_esp_lcd_session_result_t *result);

/** Destroy a suspended hub and its LCD host when no temporary app is active.
 *  Failure retains the session and its resources for destruction retry;
 *  after requesting destruction, do not start or resume an app on it.
 *  Release borrowed bundle/font/display data only after ESP_OK. */
esp_err_t esp_gsp_esp_lcd_session_destroy(
    esp_gsp_esp_lcd_session_t *session);

/** Creation and resource validation use the caller's stack before the render
 * task starts. See getting-started: the example starts with a 20 KiB main stack;
 * measure the caller's high-water mark with the actual asset/font workload. */
esp_err_t esp_gsp_esp_lcd_start(const esp_gsp_config_t *app_config,
                                const esp_gsp_esp_lcd_config_t *esp_config,
                                esp_gsp_handle_t *out_gsp);

/** Like start(), with an optional callback on the render task before its first
 * frame. Use it to install contributors, event handlers and timers without
 * racing the initial render. A NULL callback is equivalent to start().
 * If invoked, the callback completes before this function returns, including
 * first-frame failure. Its void return cannot report preparation failures;
 * the returned error covers framework startup and the first render attempt.
 * A context retained by a registered callback must outlive that registration.
 */
esp_err_t esp_gsp_esp_lcd_start_prepared(
    const esp_gsp_config_t *app_config,
    const esp_gsp_esp_lcd_config_t *esp_config,
    esp_gsp_esp_lcd_prepare_cb_t prepare,
    void *prepare_ctx,
    esp_gsp_handle_t *out_gsp);

#ifdef __cplusplus
}
#endif
