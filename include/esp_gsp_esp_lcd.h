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
    uint32_t task_stack_size;       /*!< default 12288, or 24576 with TTF */
    uint32_t task_stack_size_freetype; /*!< default 24576 with TTF */
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

/** Configure an activated UI on its render task before its first frame. */
typedef void (*esp_gsp_esp_lcd_prepare_cb_t)(esp_gsp_handle_t gsp,
        void *user_ctx);

/**
 * Quiesce the active GSP render task without destroying its UI state.
 *
 * On success no new GSP frame can be submitted until
 * esp_gsp_esp_lcd_resume_paused() is called. The function also waits for the
 * shared presenter transfer/present fences to retire.
 */
esp_err_t esp_gsp_esp_lcd_pause(
    esp_gsp_handle_t gsp,
    uint32_t timeout_ms,
    esp_gsp_esp_lcd_pause_t **out_pause);

/**
 * Resume the exact UI paused by esp_gsp_esp_lcd_pause(), including a temporary
 * app running on a retained hub session. The pause token is consumed on
 * success and the first resumed frame is forced to redraw in full.
 */
esp_err_t esp_gsp_esp_lcd_resume_paused(
    esp_gsp_esp_lcd_pause_t *pause,
    esp_gsp_handle_t *out_gsp);

/**
 * Pause a UI while retaining its scenes and component state. Its render task
 * and LCD host remain resident; decoded image surfaces are released.
 */
esp_err_t esp_gsp_esp_lcd_suspend(
    esp_gsp_handle_t hub,
    esp_gsp_esp_lcd_session_t **out_session);

/** Start an app bundle on a suspended hub's LCD host. The bundle must match
 *  the display width, height and pixel format. A startup failure resumes the
 *  hub and consumes the session. */
esp_err_t esp_gsp_esp_lcd_start_on_session(
    esp_gsp_esp_lcd_session_t *session,
    const esp_gsp_config_t *app_config,
    esp_gsp_handle_t *out_gsp);

/**
 * Start an app and invoke @p prepare on the resident render task before the
 * first frame. This is intended for installing generic runtime contributors,
 * event handlers and timers without racing the initial render.
 */
esp_err_t esp_gsp_esp_lcd_start_on_session_prepared(
    esp_gsp_esp_lcd_session_t *session,
    const esp_gsp_config_t *app_config,
    esp_gsp_esp_lcd_prepare_cb_t prepare,
    void *prepare_ctx,
    esp_gsp_handle_t *out_gsp);

/** Replace the active temporary app while retaining the suspended hub.
 * Unlike esp_gsp_esp_lcd_resume(), this never reactivates or presents the
 * hub between the two Apps. On startup failure the hub is resumed and the
 * session is consumed. */
esp_err_t esp_gsp_esp_lcd_replace_on_session_prepared(
    esp_gsp_esp_lcd_session_t *session,
    esp_gsp_handle_t current_app,
    const esp_gsp_config_t *next_config,
    esp_gsp_esp_lcd_prepare_cb_t prepare,
    void *prepare_ctx,
    esp_gsp_handle_t *out_gsp);

/**
 * Stop the temporary app, then resume the retained hub and force its first
 * frame to redraw. The session is consumed on success.
 */
esp_err_t esp_gsp_esp_lcd_resume(
    esp_gsp_esp_lcd_session_t *session,
    esp_gsp_handle_t app,
    esp_gsp_handle_t *out_hub);

/** Destroy a suspended hub and its LCD host when no temporary app is active. */
esp_err_t esp_gsp_esp_lcd_session_destroy(
    esp_gsp_esp_lcd_session_t *session);

/** Creation and resource validation use the caller's stack before the render
 * task starts. See getting-started: the example starts with a 20 KiB main stack;
 * measure the caller's high-water mark with the actual asset/font workload. */
esp_err_t esp_gsp_esp_lcd_start(const esp_gsp_config_t *app_config,
                                const esp_gsp_esp_lcd_config_t *esp_config,
                                esp_gsp_handle_t *out_gsp);

#ifdef __cplusplus
}
#endif
