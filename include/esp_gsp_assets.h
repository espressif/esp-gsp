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

/** ESP-IDF filesystem asset service. One reader task per open package. */
typedef struct esp_gsp_assets esp_gsp_assets_t;
/** Generated asset reference; do not construct offsets or frame counts by hand. */
typedef struct {
    uint32_t name_hash;
    uint16_t first_member;
    uint16_t member_count;
} esp_gsp_asset_ref_t;

typedef struct {
    /** Total encoded bytes retained by this service, including displayed images.
     * Zero derives a budget from available RAM. Decoded cache/display buffers
     * are separate. Increase only with measured heap headroom. */
    size_t read_budget_bytes;
    /** Zero uses 4096 bytes. Internal stack: this task performs filesystem IO. */
    uint32_t task_stack_size;
    /** Zero uses priority 3. */
    uint32_t task_priority;
} esp_gsp_assets_config_t;

/** Target references preserve recycled-row and widget generations. Prefer the
 * generated setters; constructors are useful for status/stop queries. */
typedef enum {
    ESP_GSP_ASSET_IMAGE, ESP_GSP_ASSET_ROW, ESP_GSP_ASSET_WIDGET,
} esp_gsp_asset_target_kind_t;
typedef struct {
    esp_gsp_asset_target_kind_t kind;
    uint16_t bind;
    uint16_t resource_slot;
    esp_gsp_row_t row;
} esp_gsp_asset_target_t;

static inline esp_gsp_asset_target_t esp_gsp_asset_image_target(uint16_t bind)
{
    esp_gsp_asset_target_t target = {ESP_GSP_ASSET_IMAGE, 0, 0, {0, 0, 0, 0}}; target.kind = ESP_GSP_ASSET_IMAGE; target.bind = bind; return target;
}
static inline esp_gsp_asset_target_t esp_gsp_asset_row_target(esp_gsp_row_t row, uint16_t slot)
{
    esp_gsp_asset_target_t target = {ESP_GSP_ASSET_IMAGE, 0, 0, {0, 0, 0, 0}}; target.kind = ESP_GSP_ASSET_ROW; target.bind = row.list;
    target.resource_slot = slot; target.row = row; return target;
}
static inline esp_gsp_asset_target_t esp_gsp_asset_widget_target(esp_gsp_widget_t widget, uint16_t slot)
{
    esp_gsp_asset_target_t target = {ESP_GSP_ASSET_IMAGE, 0, 0, {0, 0, 0, 0}}; target.kind = ESP_GSP_ASSET_WIDGET; target.bind = widget;
    target.resource_slot = slot; return target;
}

typedef enum {
    ESP_GSP_ASSET_QUEUED, ESP_GSP_ASSET_READING, ESP_GSP_ASSET_DECODING,
    ESP_GSP_ASSET_READY, ESP_GSP_ASSET_FAILED, ESP_GSP_ASSET_STOPPING,
    ESP_GSP_ASSET_STOPPED, ESP_GSP_ASSET_FINISHED,
} esp_gsp_asset_state_t;
typedef struct {
    esp_gsp_asset_state_t state;
    uint32_t request_id;
    gsp_err_t last_error;
    /** Valid for FAILED: distinguishes file loading from UI decode/publication. */
    esp_gsp_asset_state_t failed_stage;
    int system_errno;
    /** Encoded-member allocation requirement; UI decode failures have a
     * separate decoded working set, reported by cache/heap diagnostics. */
    size_t required_bytes;
    bool pending;
} esp_gsp_asset_status_t;
typedef struct {
    size_t encoded_bytes, encoded_peak_bytes, encoded_budget_bytes;
    size_t decoded_bytes, decoded_peak_bytes;
    size_t index_bytes;
    /** File bytes delivered; load time includes lookup, CRC and allocation. */
    uint64_t read_bytes, load_us;
    uint32_t submitted_frames, queue_retries;
} esp_gsp_assets_stats_t;

/** Open a mounted SD/NAND filesystem package using the generated open helper.
 * index_crc comes from its generated header. A matching index takes the fast
 * path; a replacement package (or index_crc=0) is fully CRC-checked in bounded
 * chunks at open. Each requested member is CRC-checked when read. No whole-pack
 * RAM copy. Call from an application task; open performs IO.
 * File contents must stay immutable until close. Close before unmount/update.
 * NULL config selects defaults. On failure *out_assets is NULL.
 */
esp_gsp_err_t esp_gsp_assets_open(const char *path, uint32_t index_crc,
                                  const esp_gsp_assets_config_t *config, esp_gsp_assets_t **out_assets);

/** Queue an image or animation for an authored Image bind. Asset names come
 * from the generated header. Submission returns without file IO or decoding.
 * once plays one animation cycle; false follows the authored loop count.
 * Existing content stays visible until a complete replacement is ready.
 * Each target has at most one in-flight frame. New requests coalesce per bind.
 * Animation memory follows ANIM_FRAME_MEMORY / ANIM_MAX_FRAME_BYTES and is
 * separate from the encoded read budget. JPEG animations also work with the
 * image cache disabled; measure playback performance on the target device.
 * The target placeholder must match the exported pixel/alpha form and native
 * dimensions (opaque image-fit binds can vary dimensions). Export to the same
 * RGB565/RGB888 profile as the UI.
 * Show targets the active scene. Switching scenes cancels subsequent frames;
 * resubmit when returning to that scene.
 * Hiding a page/group does not stop file IO or decoding. Stop its targets
 * when hidden and show them again when visible; show restarts playback.
 * gsp must remain alive until this service is closed. Use one service owner
 * per target; do not concurrently replace it with another image producer.
 */
esp_gsp_err_t esp_gsp_assets_show(esp_gsp_assets_t *assets,
                                  esp_gsp_handle_t gsp, uint16_t bind, esp_gsp_asset_ref_t asset, bool once);

/** Resolve an asset by its manifest name, including newly added assets.
 * Names are copied as a stable key; the caller may release name on return.
 * Same asynchronous result and lifetime contract as esp_gsp_assets_show(). */
esp_gsp_err_t esp_gsp_assets_show_name(esp_gsp_assets_t *assets,
                                       esp_gsp_handle_t gsp, uint16_t bind, const char *name, bool once);

/** Same ownership and scene rules as show; a recycled row token is checked
 * again on the UI task, so delayed data cannot replace a newly assigned row. */
esp_gsp_err_t esp_gsp_assets_show_row(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                      esp_gsp_row_t row, uint16_t slot, esp_gsp_asset_ref_t asset, bool once);
esp_gsp_err_t esp_gsp_assets_show_widget(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_widget_t widget, uint16_t slot, esp_gsp_asset_ref_t asset, bool once);
/** Generic target forms for Image, Row and Widget destinations. */
esp_gsp_err_t esp_gsp_assets_show_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_asset_target_t target, esp_gsp_asset_ref_t asset, bool once);
esp_gsp_err_t esp_gsp_assets_show_target_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_asset_target_t target, const char *name, bool once);
esp_gsp_err_t esp_gsp_assets_get_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                        esp_gsp_asset_target_t target, esp_gsp_asset_status_t *out_status);
esp_gsp_err_t esp_gsp_assets_stop_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_asset_target_t target);
/** On-demand overview: encoded bytes belong to this package; optional gsp
 * contributes all live scene decoded caches, including non-external images.
 * Decoded counters also include this service's animation canvases/patch scratch.
 * Peaks are conservative sums, not simultaneous samples. Display buffers and
 * codec-internal scratch are not included. */
esp_gsp_err_t esp_gsp_assets_get_stats(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                       esp_gsp_assets_stats_t *out_stats);

/** Last load/decode result for this target; out_pending reports unfinished work.
 * Returns GSP_ERR_NOT_FOUND before the target has been submitted. */
gsp_err_t esp_gsp_assets_status(esp_gsp_assets_t *assets,
                                esp_gsp_handle_t gsp, uint16_t bind, bool *out_pending);

/** Stop scheduling this target; its last published image remains visible.
 * An already accepted frame may finish. Call show again to restart/replace. */
esp_gsp_err_t esp_gsp_assets_stop(esp_gsp_assets_t *assets,
                                  esp_gsp_handle_t gsp, uint16_t bind);

/** Stop new work and wait at most 5000 ms. On ESP_GSP_ERR_TIMEOUT the handle
 * remains valid and closing; resume UI/IO progress and retry close. Never
 * unmount storage or destroy the UI until close returns ESP_GSP_OK. */
esp_gsp_err_t esp_gsp_assets_close(esp_gsp_assets_t *assets);
/** Explicit wait budget; zero polls. Does not forcibly cancel a driver read.
 * On timeout status/stats and another close are allowed; show is rejected.
 * Call from an application task, not a render/decode callback. Other callers
 * must be quiescent before successful close frees the handle. Published source
 * buffers survive close until UI replacement/shutdown. NULL succeeds. */
esp_gsp_err_t esp_gsp_assets_close_wait(esp_gsp_assets_t *assets, uint32_t timeout_ms);
#ifdef __cplusplus
}
#endif
