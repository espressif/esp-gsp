#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Execute the device benchmark's callbacks with a fake clock and counters."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

MAIN = Path(__file__).resolve().parents[1] / "main/app_main.c"


def function(source, signature):
    start = source.index(signature)
    opening = source.index("{", start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end]


class BenchmarkLifecycleTests(unittest.TestCase):
    def test_prepare_is_unscored_and_tweens_follow_page_clock(self):
        compiler = shutil.which("cc")
        if compiler is None:
            self.skipTest("C compiler unavailable")
        source = MAIN.read_text()
        callbacks = "\n".join(function(source, signature) for signature in (
            "static void start_page_measurement(", "static void drive_stress(",
            "static void drive_pressure(", "static void drive_eyes("))
        prelude = r'''
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef void *esp_gsp_handle_t;
typedef int esp_gsp_err_t;
typedef struct { bool timing_available; uint32_t steps; uint64_t step_us; } esp_gsp_property_stats_t;
#define ESP_OK 0
#define ESP_ERR_INVALID_STATE 1
#define ESP_GSP_OK 0
#define ESP_GSP_ERR_TIMEOUT 2
#define ESP_ERROR_CHECK(value) assert((value) == ESP_OK)
#define GSP_RENDERER_PROFILE_OPCODES 1
#define MALLOC_CAP_INTERNAL 1
#define MALLOC_CAP_SPIRAM 2
#define MALLOC_CAP_8BIT 4
#define GSP_BENCH_FULL_REPAINT 0
#define GSP_BIND_P_STATIC_MOVE 1
#define GSP_BIND_P_MOVE 2
#define GSP_BIND_P_MESSAGES 3
#define GSP_BIND_PRESSURE_BG 4
static struct { uint16_t bind; } s_pages[1];
static unsigned s_page, tween_calls, work_calls, tween_phase;
static bool s_page_preparing, s_results_active, s_page_waiting_for_scene;
static int64_t now_us, s_page_start_us, s_next_tween_us;
static uint32_t frames, iterations, commands;
static uint64_t busy_us, render_us, submit_us, service_us;
static esp_gsp_property_stats_t properties, s_page_start_properties;
static uint32_t s_page_start_frames, s_page_start_service_iterations, s_page_start_service_commands;
static uint64_t s_page_start_busy_us, s_page_start_render_us, s_page_start_submit_us, s_page_start_service_us;
static uint64_t s_page_start_ops[1];
static unsigned s_page_region_start, s_page_start_transition_stats, s_page_start_viewports;
static size_t s_page_internal_start, s_page_psram_start, s_page_internal_min, s_page_psram_min;
static uint32_t s_pressure_ticks, s_pressure_requests, s_pressure_accepted, s_pressure_rejected, s_pressure_errors;
static struct { uint32_t tween_tick; } s_bench_workload;
static int64_t esp_timer_get_time(void) { return now_us; }
static bool esp_gsp_property_stats(void *ui, esp_gsp_property_stats_t *out, size_t size) {
    (void)ui; (void)size; *out = properties; return true;
}
static void esp_gsp_region_stats(void *ui, unsigned *out) { (void)ui; *out = 0; }
static void esp_gsp_drag_snapshot_stats(void *ui, unsigned *out) { (void)ui; *out = 0; }
static void esp_gsp_transition_stats(void *ui, unsigned *out) { (void)ui; *out = 0; }
static void esp_gsp_render_stats(void *ui, uint32_t *f, uint64_t *b) { (void)ui; *f=frames; *b=busy_us; }
static void esp_gsp_render_phases(void *ui, uint64_t *r, uint64_t *s) { (void)ui; *r=render_us; *s=submit_us; }
static void esp_gsp_service_stats(void *ui, uint32_t *i, uint64_t *s, uint32_t *c) {
    (void)ui; *i=iterations; *s=service_us; *c=commands;
}
static void gsp_renderer_profile_snapshot(uint64_t *ops, void *unused, unsigned n) { (void)ops; (void)unused; (void)n; }
static size_t heap_caps_get_free_size(unsigned caps) { (void)caps; return 123; }
static void bench_workload_drive_tweens(void *ui, void *workload) {
    (void)ui; (void)workload; tween_phase=s_bench_workload.tween_tick++; ++tween_calls;
}
static void drive_fx(void *ui, void *ctx) { (void)ui; (void)ctx; ++work_calls; }
static void drive_text(void *ui, void *ctx) { (void)ui; (void)ctx; }
static void drive_scroll(void *ui, void *ctx) { (void)ui; (void)ctx; }
static void drive_drag(void *ui, void *ctx) { (void)ui; (void)ctx; }
static void drive_messages(void *ui) { (void)ui; ++work_calls; }
static void bench_workload_drive_eyes(void *ui, void *ctx) { (void)ui; (void)ctx; ++work_calls; }
static bool benchmark_transition_page(unsigned bind) { (void)bind; return false; }
static uint32_t bench_native_color(uint32_t a, uint32_t b) { (void)b; return a; }
static int esp_gsp_set_color(void *ui, unsigned bind, unsigned color) { (void)ui; (void)bind; (void)color; return 0; }
'''
        assertions = r'''
int main(void) {
    for (unsigned lap=0; lap<2; ++lap) {
        s_page_preparing=true;
        s_bench_workload.tween_tick=0;
        unsigned prior_calls=work_calls, prior_tweens=tween_calls;
        now_us=10000000LL + lap*20000000LL;
        drive_stress(NULL,NULL); drive_pressure(NULL,NULL); drive_eyes(NULL,NULL);
        assert(work_calls==prior_calls && tween_calls==prior_tweens);
        /* Setup costs, including an earlier summary print, are already complete. */
        frames=400+lap*100; busy_us=2000000+lap*1000000;
        properties=(esp_gsp_property_stats_t){true,100+lap*100,900000+lap*500000};
        start_page_measurement(NULL);
        assert(!s_page_preparing && s_page_start_frames==frames && s_page_start_busy_us==busy_us);
        assert(s_page_start_properties.steps==properties.steps && s_page_start_properties.step_us==properties.step_us);
        assert(tween_phase==0 && tween_calls==prior_tweens+1);
        assert(s_next_tween_us==now_us+1000000);
        now_us=s_page_start_us+999999; drive_stress(NULL,NULL);
        assert(tween_calls==prior_tweens+1);
        now_us=s_page_start_us+1000000; drive_stress(NULL,NULL);
        assert(tween_phase==1 && tween_calls==prior_tweens+2);
        now_us=s_page_start_us+3250000; drive_stress(NULL,NULL);
        assert(tween_phase==3 && s_next_tween_us==s_page_start_us+4000000);
        properties.step_us+=700; busy_us+=1300; frames+=2;
        assert(properties.step_us-s_page_start_properties.step_us==700);
        assert(busy_us-s_page_start_busy_us==1300 && frames-s_page_start_frames==2);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "lifecycle.c"
            path.write_text(prelude + callbacks + assertions)
            binary = Path(directory) / "lifecycle"
            subprocess.run([compiler, "-std=c11", "-Wall", "-Wextra", "-Werror", str(path), "-o", str(binary)], check=True)
            subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    unittest.main()
