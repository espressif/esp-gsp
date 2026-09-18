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

/** Component update accounting, in two units. Populated only by a library
 *  built with the GSP_PROFILE_SERVICE instrumentation; see
 *  esp_gsp_property_stats() for how an ordinary build answers.
 *
 *  A *batch* is one atomic component update transaction: one
 *  esp_gsp_component_set_properties() or esp_gsp_component_set_many() call,
 *  including the single-value convenience setters
 *  (esp_gsp_component_set_property(), _set_position(), _set_value(),
 *  _set_color_rgb888(), esp_gsp_image_set_scale(), ...) that resolve to one
 *  of them. An *entry* is one component+property+value item inside a batch,
 *  so a single-value setter is one batch with one entry and
 *  esp_gsp_component_set_position() is one batch with two.
 *
 *  Each accepted batch is processed twice by design: the caller validates it
 *  so an invalid update fails immediately, and a second pass applies it
 *  inside its own transaction. The validate_* fields account for the first
 *  pass, the apply_* fields for the second. Both are counted even where a
 *  single task performs both.
 *
 *  Each timing field is measured around the work it names. Which one
 *  contains which depends on where the apply pass ran, and a single build
 *  mixes several such paths, so never add them together:
 *   - validate_us is always the calling task, around the validation pass
 *     only.
 *   - apply_us is measured wherever the batch is actually applied, which is
 *     one of three places:
 *       * Queued drain. The caller is an ordinary task, so the batch is
 *         handed to the platform adapter's queue and applied when the render
 *         task drains it. That drain is part of what service_us of
 *         esp_gsp_service_stats() measures, so this apply_us falls inside
 *         service_us.
 *       * Render-task inline. The caller already is the render task — an
 *         application timer, input or decode callback invoked from the frame
 *         loop — and the adapter applies the batch inline rather than
 *         queueing it. This apply_us falls inside whichever render-task
 *         window the callback ran in: step_us for a callback under
 *         esp_gsp_ui_step(), service_us for one reached from the command
 *         drain or the pointer poll.
 *       * No adapter. The update is always applied inline on the calling
 *         task, so it falls inside no render-task window unless that caller
 *         was itself running under esp_gsp_ui_step().
 *     apply_us is one total over all three, so it can only be treated as a
 *     breakdown of service_us or of step_us by an integration that is known
 *     to use a single path.
 *   - step_us is the esp_gsp_ui_step() body. It may therefore contain
 *     apply_us spans, and is disjoint from the render_us/submit_us of
 *     esp_gsp_render_phases().
 *
 *  Neither coalescing nor no-op suppression exists on this path, and the
 *  update transaction does not report whether a committed value differed
 *  from the previous one, so no coalesced or no-op count is offered.
 */
typedef struct {
    /** Bytes of this structure the library filled; always the first field. */
    uint16_t struct_size;
    /** Non-zero when this build carries the counter instrumentation. Zero
     *  means every count below is a zeroed placeholder. */
    uint8_t counters_available;
    /** Non-zero when the port has a microsecond clock, so the _us and max_*
     *  fields carry real measurements. Ports without one (the host and
     *  simulator builds, which stay bit-exact across runs) report zero here
     *  and leave those fields at 0 while the counts remain valid. */
    uint8_t timing_available;
    /** Batches that ran caller-side validation, and their entries. */
    uint32_t validated_batches;
    uint32_t validated_entries;
    /** Batches rejected by caller-side validation; nothing was queued. */
    uint32_t validate_failures;
    /** Batches that passed validation but could not be queued. */
    uint32_t submit_failures;
    /** Batches committed by the render task, and their entries. */
    uint32_t applied_batches;
    uint32_t applied_entries;
    /** Batches whose apply-time transaction failed; nothing was committed. */
    uint32_t apply_failures;
    /** Batches the render task skipped before opening a transaction, in
     *  practice because their scene was no longer current on arrival. */
    uint32_t apply_dropped_batches;
    /** Batches whose entry count exceeded the inline update scratch, counted
     *  once per pass, so the pass had to stage its transaction on the heap. */
    uint32_t heap_scratch_batches;
    /** Completed UI-core state advances, one per esp_gsp_ui_step() body.
     *  gsp_app_step() resolves a pending scene transition before entering
     *  that body, so the transition and its adapter calls are outside both
     *  this count and step_us. */
    uint32_t steps;
    uint32_t max_validate_us;
    uint32_t max_apply_us;
    uint32_t max_step_us;
    uint64_t validate_us;
    uint64_t apply_us;
    uint64_t step_us;
} esp_gsp_property_stats_t;

/** Snapshot the component update counters. Pass
 *  sizeof(esp_gsp_property_stats_t) as @p stats_size; the library fills the
 *  common prefix and reports it in struct_size, so a caller built against a
 *  different header revision still reads the fields it knows.
 *
 *  Returns true only when this library carries the GSP_PROFILE_SERVICE
 *  instrumentation and @p gsp is valid. An ordinary build has no counters at
 *  all: the call then writes @p stats_size zeroed bytes, sets struct_size,
 *  leaves counters_available at 0 and returns false, so a zeroed reading is
 *  never mistaken for measured traffic.
 *
 *  Two cases write nothing at all and return false: @p out_stats is NULL, or
 *  @p stats_size is under sizeof(uint32_t) and so cannot hold even the
 *  struct_size and availability prefix. In both the caller's buffer is left
 *  exactly as it was. Every other @p stats_size is fully written.
 *
 *  Counters are cumulative and wrap; take two snapshots and subtract. Every
 *  field is read with a single atomic load, so snapshots do not race with
 *  counter updates or report torn values. Fields are sampled one after
 *  another and may belong to slightly different moments; this is not an
 *  atomic snapshot of the entire pipeline.
 *
 *  Atomic operations use the target toolchain's implementation. On targets
 *  without native support, including 64-bit totals on supported 32-bit SoCs,
 *  ESP-IDF helpers use brief interrupt-disabled critical sections or spinlocks.
 *  Reads and updates can therefore delay another task; the overhead depends
 *  on the target and contention. These helpers do not acquire UI/state
 *  mutexes, and this cost only exists in an instrumented build. */
bool esp_gsp_property_stats(esp_gsp_handle_t gsp,
                            esp_gsp_property_stats_t *out_stats,
                            size_t stats_size);

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
