/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "bundle_gsp.h"
#include "esp_gsp_debug.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "watch_ui.h"

/* Runs on the UI task. Timing a setter here includes its synchronous viewport
 * capture, the interval during which that task cannot sample touch again. */
static void *probe_timer;
static unsigned sample;
static uint32_t previous_frames;
static uint64_t previous_busy;
static int64_t previous_time;

static void probe(esp_gsp_handle_t ui, void *ctx)
{
    (void)ctx;
    uint32_t frames, errors;
    uint64_t busy;
    gsp_err_t last_error;
    esp_gsp_render_stats(ui, &frames, &busy);
    esp_gsp_render_error_stats(ui, &errors, &last_error);
    int64_t now = esp_timer_get_time();
    uint32_t count = frames - previous_frames;
    ESP_LOGI("watch_probe", "interval_page=%u frames=%lu elapsed_us=%lld busy_us_per_frame=%llu render_errors=%lu last_error=%d",
             sample % WATCH_PAGE_COUNT, (unsigned long)count,
             (long long)(now - previous_time),
             (unsigned long long)(count ? (busy - previous_busy) / count : 0),
             (unsigned long)errors, last_error);
    previous_frames = frames;
    previous_busy = busy;
    previous_time = now;
    unsigned target = (sample + 1) % WATCH_PAGE_COUNT;
    int64_t begin = esp_timer_get_time();
    esp_gsp_err_t result = esp_gsp_page_flow_set_page(
                               ui, GSP_WATCH_OBJ_KEY_PAGES, target, true);
    int64_t elapsed = esp_timer_get_time() - begin;
    esp_gsp_drag_snapshot_stats_t snapshots;
    esp_gsp_drag_snapshot_stats(ui, &snapshots);
    esp_gsp_media_stats_t media;
    esp_gsp_media_stats(ui, &media);
    esp_gsp_heap_stats_t heap;
    bool heap_available = esp_gsp_heap_stats(&heap);
    ESP_LOGI("watch_probe", "page=%u capture_us=%lld result=%d snapshots=%lu/%lu cache_bytes=%zu heap_available=%d internal_free=%zu psram_free=%zu",
             target, (long long)elapsed, result,
             (unsigned long)snapshots.started, (unsigned long)snapshots.attempts,
             media.cache_bytes, heap_available,
             heap_available ? heap.internal.free_bytes : 0,
             heap_available ? heap.psram.free_bytes : 0);
    if (++sample == WATCH_PAGE_COUNT * 2) {
        (void)esp_gsp_timer_delete(ui, probe_timer);
        ESP_LOGI("watch_probe", "complete");
    }
}

void watch_probe_start(esp_gsp_handle_t ui)
{
    esp_gsp_render_stats(ui, &previous_frames, &previous_busy);
    previous_time = esp_timer_get_time();
    probe_timer = esp_gsp_timer_create(ui, 1500, probe, NULL);
    if (!probe_timer) {
        ESP_LOGE("watch_probe", "cannot create probe timer");
    }
}
