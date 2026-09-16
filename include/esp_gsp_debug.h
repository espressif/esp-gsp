/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

/* Optional diagnostics and test instrumentation. Ordinary applications use
 * esp_gsp.h or the compiler-generated bundle header. */

#include "esp_gsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** One device-wide, 8-bit-addressable heap class, in bytes. */
typedef struct {
    size_t free_bytes;
    size_t largest_free_block;
} esp_gsp_heap_info_t;

typedef struct {
    esp_gsp_heap_info_t internal;
    esp_gsp_heap_info_t psram;
} esp_gsp_heap_stats_t;

/** Read device-wide internal/PSRAM heaps on demand, including non-GSP users.
 * Does not allocate or change budgets. Call from task context, not an ISR or
 * every frame: heap inspection traverses allocator metadata. The two heaps
 * are sampled separately; concurrent allocations can change the result.
 * Returns false and zeroes the output on hosts without heap capabilities;
 * also returns false for NULL. An absent PSRAM heap has zero values.
 * These values are not DMA guarantees or a complete scene memory budget.
 */
bool esp_gsp_heap_stats(esp_gsp_heap_stats_t *out_stats);

/** Monotonic count of rendered non-idle frames since startup. */
uint32_t esp_gsp_frame_count(esp_gsp_handle_t gsp);

/** Read rendered frames and cumulative time spent producing frames. */
void esp_gsp_render_stats(esp_gsp_handle_t gsp, uint32_t *out_frames,
                          uint64_t *out_busy_us);

/** Cumulative scene-transition path selections. */
typedef struct {
    uint32_t snapshot;
    uint32_t direct;
    uint32_t inplace;
    uint32_t streamed;
    uint32_t failures;
} esp_gsp_transition_stats_t;

void esp_gsp_transition_stats(esp_gsp_handle_t gsp,
                              esp_gsp_transition_stats_t *out_stats);

/** Why a drag snapshot opportunity stayed on live rendering. */
typedef enum {
    ESP_GSP_SNAPSHOT_NONE,
    ESP_GSP_SNAPSHOT_DISABLED,
    ESP_GSP_SNAPSHOT_NOT_VISIBLE,
    ESP_GSP_SNAPSHOT_NO_MOVEMENT,
    ESP_GSP_SNAPSHOT_UNSUPPORTED,
    ESP_GSP_SNAPSHOT_RETRY_BLOCKED,
    ESP_GSP_SNAPSHOT_BACKEND_LIMIT,
    ESP_GSP_SNAPSHOT_BACKEND_ERROR,
    ESP_GSP_SNAPSHOT_REASON_COUNT,
} esp_gsp_snapshot_reason_t;
typedef struct {
    uint32_t attempts, started, reused;
    uint32_t skipped[ESP_GSP_SNAPSHOT_REASON_COUNT];
    esp_gsp_snapshot_reason_t last_reason;
    int32_t last_backend_error;
    bool active;
} esp_gsp_drag_snapshot_stats_t;
/** Cumulative List/Grid/MessageList and viewport drag snapshot accounting.
 * Read from a serialized app callback, like the other debug counters. */
void esp_gsp_drag_snapshot_stats(esp_gsp_handle_t gsp,
                                 esp_gsp_drag_snapshot_stats_t *out_stats);

/** Cumulative dirty-region planner accounting. */
typedef struct {
    uint32_t plans;
    uint32_t input_regions;
    uint32_t output_regions;
    uint32_t merge_candidates;
    uint32_t merges_accepted;
    uint32_t merges_rejected;
    uint32_t full_promotions;
    uint64_t input_pixels;
    uint64_t output_pixels;
    uint64_t estimated_before;
    uint64_t estimated_after;
} esp_gsp_region_stats_t;

void esp_gsp_region_stats(esp_gsp_handle_t gsp,
                          esp_gsp_region_stats_t *out_stats);

/** Cumulative rasterization and presentation time. */
void esp_gsp_render_phases(esp_gsp_handle_t gsp,
                           uint64_t *out_render_us,
                           uint64_t *out_submit_us);

/** Render-task service-loop profile. Values are zero unless the integration
 *  enables the lightweight GSP_PROFILE_SERVICE build instrumentation. */
void esp_gsp_service_stats(esp_gsp_handle_t gsp,
                           uint32_t *out_iterations,
                           uint64_t *out_service_us,
                           uint32_t *out_commands);

/** Cumulative image, animation and canvas pipeline counters. */
typedef struct {
    uint32_t decoded_images;
    uint64_t decode_us;
    uint32_t max_decode_us;
    uint32_t cache_hits;
    uint32_t cache_misses;
    /** Current decoded bytes summed across all live scene caches. */
    size_t cache_bytes;
    uint32_t cache_requests;
    uint32_t cache_coalesced;
    uint32_t cache_evictions;
    uint32_t cache_allocation_failures;
    uint32_t cache_decode_failures;
    uint64_t decoded_bytes;
    uint64_t codec_decode_us;
    uint64_t probe_us;
    uint64_t scale_us;
    uint32_t render_deferred;
    uint32_t jpeg_hw_decodes;
    uint64_t jpeg_hw_us;
    uint32_t jpeg_sw_decodes;
    uint64_t jpeg_sw_us;
    uint32_t jpeg_hw_ineligible;
    uint32_t jpeg_hw_fallbacks;
    uint32_t animation_patches;
    uint64_t animation_work_us;
    uint32_t max_animation_work_us;
    uint64_t animation_patch_pixels;
    uint64_t animation_copy_bytes;
    size_t animation_scratch_peak;
    /** Sum of each live scene cache's lifetime peak. This is a conservative
     *  bound, not a time-correlated application high-water mark. */
    size_t cache_peak_bytes;
    uint64_t queue_wait_us;
    uint32_t max_queue_wait_us;
    uint64_t allocation_us;
    uint32_t max_allocation_us;
    uint64_t publish_us;
    uint32_t max_publish_us;
    uint32_t qoi_decodes;
    uint32_t qoi_failures;
    uint32_t rle16_decodes;
    uint32_t rle16_failures;
    uint32_t png_decodes;
    uint32_t png_failures;
    uint32_t codec_failures;
    uint32_t scale_requests;
    uint32_t scale_hw;
    uint32_t scale_sw;
    uint32_t jpeg_hw_attempts;
    uint32_t jpeg_sw_attempts;
    uint32_t jpeg_sw_failures;
    uint32_t dynamic_image_requests;
    uint32_t dynamic_image_queued;
    uint32_t dynamic_image_published;
    uint32_t dynamic_image_failures;
    uint32_t dynamic_image_cancelled;
    uint64_t dynamic_image_probe_us;
    uint32_t canvas_frames_applied;
    uint32_t canvas_frames_rejected;
    uint32_t canvas_frames_coalesced;
} esp_gsp_media_stats_t;

void esp_gsp_media_stats(esp_gsp_handle_t gsp,
                         esp_gsp_media_stats_t *out_stats);

/** Inject one logical pointer sample. Intended for tests and diagnostics. */
esp_gsp_err_t esp_gsp_inject_touch(esp_gsp_handle_t gsp, int16_t x,
                                   int16_t y, bool pressed);

#ifdef __cplusplus
}
#endif
