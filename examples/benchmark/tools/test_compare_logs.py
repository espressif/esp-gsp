# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path


MODULE_PATH = Path(__file__).with_name("compare_logs.py")
SPEC = importlib.util.spec_from_file_location("compare_logs", MODULE_PATH)
assert SPEC is not None and SPEC.loader is not None
compare_logs = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = compare_logs
SPEC.loader.exec_module(compare_logs)


class ParseLogTest(unittest.TestCase):
    def test_last_complete_summary_is_parsed(self):
        content = """
\x1b[0;32mI (1) app_init: App version:      baseline\x1b[0m\r
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms   sec
bench: idle               1.0      10.0     3.0     7.0     3
bench: labels             2.0      20.0     2.0     8.0     5
bench: weighted average 18.5 busy fps (idle excluded)
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
        self.assertEqual(run.version, "baseline")
        self.assertEqual(run.score_fps, 18.5)
        self.assertEqual(run.metric, "legacy_weighted_busy_score")
        self.assertEqual(run.pages["labels"].submit_ms, 8.0)
        self.assertIsNone(run.pages["labels"].service_us)

    def test_enriched_service_columns_are_parsed(self):
        content = """
I (1) app_init: App version:      enriched
I (2) present_target: target: mode=TRIPLE_PARTIAL panel=RGB gram=0 rotation=0 te=0 fb=3 strip=0x0 stream=0x0
bench: transition[slide left] count=7 errors=0 no_visual=0 frames=154 frames_avg=22.0 latency_avg=1234.5us p50=1200us p95=1500us max=1500us snapshot=7 direct=0 inplace=0 streamed=0 path_failures=0
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: widgets           20.0      25.0    10.0    30.0    42.5    18.0     5
bench: config protocol=2 target=esp32s31 logical=800x480 pixel=rgb888
bench: aggregate wall throughput 16.0 fps (observed; idle excluded)
bench: aggregate active throughput 20.0 fps (engine capacity; idle excluded)
bench: aggregate raw frames=100 wall_us=6250000 busy_us=5000000 utilization=80.0%
bench: legacy dwell-weighted busy score 25.0 fps (idle excluded)
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
        self.assertEqual(run.pages["widgets"].service_us, 42.5)
        self.assertEqual(run.pages["widgets"].commands_s, 18.0)
        self.assertEqual(run.score_fps, 20.0)
        self.assertEqual(run.wall_fps, 16.0)
        self.assertEqual(run.metric, "aggregate_active_fps")
        self.assertEqual(run.legacy_fps, 25.0)
        self.assertEqual(run.raw_frames, 100)
        self.assertEqual(run.transitions["slide left"].count, 7)
        self.assertEqual(run.transitions["slide left"].p95_us, 1500)
        self.assertEqual(run.transitions["slide left"].frames_avg, 22.0)
        self.assertEqual(run.transitions["slide left"].snapshot, 7)
        self.assertIn(("target", "esp32s31"), run.config)
        self.assertIn(("resolved_mode", "TRIPLE_PARTIAL"), run.config)
        self.assertIn(("resolved_fb", "3"), run.config)

    def test_underrun_sample_is_rejected(self):
        content = """
I (1) app_init: App version:      unstable
E (2) lcd.rgb: LCD underrun
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms   sec
bench: labels            10.0      20.0     2.0     8.0     5
bench: aggregate active throughput 20.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(ValueError, "invalid runtime sample"):
                compare_logs.parse_log(path)

    def test_repeater_capacity_failure_is_rejected(self):
        content = """
I (1) app_init: App version:      invisible-list
E (2) gsp_repeater: Repeater slots insufficient: slots=16 required=20
W (3) esp_gsp: list 0: repeater init failed
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms   sec
bench: messages           0.2      50.0     2.0     8.0     6
bench: aggregate active throughput 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(ValueError, "invalid runtime sample"):
                compare_logs.parse_log(path)

    def test_snapshot_free_fade_fallback_is_validated(self):
        content = """
I (1) app_init: App version:      direct-fade
bench: config protocol=4 transition_snapshots=off
bench: transition[fade through black] count=2 errors=0 no_visual=2 frames=0 frames_avg=0.0 latency_avg=100.0us p50=90us p95=110us max=110us snapshot=0 direct=0 inplace=0 streamed=0 path_failures=2
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: fade                0.0       0.0     0.0     0.0     1.0     1.0     5
bench: aggregate wall throughput 1.0 fps
bench: aggregate active throughput 1.0 fps
bench: legacy dwell-weighted busy score 1.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
        self.assertEqual(
            run.transitions["fade through black"].path_failures, 2)
        self.assertEqual(
            run.transitions["fade through black"].no_visual, 2)

    def test_runtime_snapshot_failure_and_drag_cancel_are_validated(self):
        content = """
I (1) app_init: App version:      constrained
bench: config protocol=4 transition_snapshots=on
bench: warm-up lap done, measuring
bench: transition[fade through black] count=2 errors=0 no_visual=2 frames=0 frames_avg=0.0 latency_avg=100.0us p50=90us p95=110us max=110us snapshot=0 direct=0 inplace=0 streamed=0 path_failures=2
bench: transition[drag cancel] count=2 errors=0 no_visual=2 frames=0 frames_avg=0.0 latency_avg=5.0us p50=5us p95=5us max=5us snapshot=0 direct=0 inplace=0 streamed=0 path_failures=0
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: fade                0.0       0.0     0.0     0.0     1.0     1.0     5
bench: aggregate wall throughput 1.0 fps
bench: aggregate active throughput 1.0 fps
bench: legacy dwell-weighted busy score 1.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
        self.assertEqual(
            run.transitions["fade through black"].path_failures, 2)
        self.assertEqual(run.transitions["drag cancel"].no_visual, 2)

    def test_partial_next_lap_does_not_replace_measured_transitions(self):
        content = """
I (1) app_init: App version:      repeated
bench: warm-up lap done, measuring
bench: transition[fade through black] count=2 errors=0 no_visual=0 frames=20 frames_avg=10.0 latency_avg=100.0us p50=90us p95=110us max=110us snapshot=2 direct=0 inplace=0 streamed=0 path_failures=0
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: fade                1.0       2.0     3.0     4.0     5.0     6.0     5
bench: aggregate wall throughput 1.0 fps
bench: aggregate active throughput 2.0 fps
bench: legacy dwell-weighted busy score 2.0 fps
bench: page idle
bench: transition[fade through black] count=1 errors=0 no_visual=1 frames=0 frames_avg=0.0 latency_avg=1.0us p50=1us p95=1us max=1us snapshot=0 direct=0 inplace=0 streamed=0 path_failures=1
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
        self.assertEqual(run.transitions["fade through black"].count, 2)
        self.assertEqual(run.transitions["fade through black"].frames, 20)

    def test_failed_drawer_cycle_is_rejected(self):
        content = """
I (1) app_init: App version:      drawer-stuck
bench: warm-up lap done, measuring
bench: drawer opens=1 closes=0 errors=1
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: drawer              1.0       2.0     3.0     4.0     5.0     6.0     5
bench: aggregate wall throughput 1.0 fps
bench: aggregate active throughput 2.0 fps
bench: legacy dwell-weighted busy score 2.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(ValueError, "invalid drawer cycle"):
                compare_logs.parse_log(path)

    def test_composites_without_real_keyboard_activity_is_rejected(self):
        content = """
I (1) app_init: App version:      keyboard-missed
bench: warm-up lap done, measuring
bench: keyboard presses=0 text_updates=0 backspaces=0 final=""
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: composites          1.0       2.0     3.0     4.0     5.0     6.0     6
bench: aggregate wall throughput 1.0 fps
bench: aggregate active throughput 2.0 fps
bench: legacy dwell-weighted busy score 2.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                ValueError, "invalid keyboard interaction"):
                compare_logs.parse_log(path)

    def test_grid_without_real_drag_activity_is_rejected(self):
        content = """
I (1) app_init: App version:      grid-static
bench: warm-up lap done, measuring
bench: grid binds=12 drags=0 errors=0
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: grid album          1.0       2.0     3.0     4.0     5.0     6.0     5
bench: aggregate wall throughput 1.0 fps
bench: aggregate active throughput 2.0 fps
bench: legacy dwell-weighted busy score 2.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                    ValueError, "invalid grid interaction"):
                compare_logs.parse_log(path)

    def test_grid_interaction_result_is_required(self):
        content = """
I (1) app_init: App version:      grid-missed
bench: warm-up lap done, measuring
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: grid album          1.0       2.0     3.0     4.0     5.0     6.0     5
bench: aggregate wall throughput 1.0 fps
bench: aggregate active throughput 2.0 fps
bench: legacy dwell-weighted busy score 2.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                    ValueError, "missing grid interaction result"):
                compare_logs.parse_log(path)

    def test_clock_command_error_is_rejected(self):
        content = """
I (1) app_init: App version:      clock-error
bench: warm-up lap done, measuring
bench: clock period_ms=16 target_hz=62.5 updates=187 commands=748 errors=1
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: clock needles      60.0      80.0     5.0     7.0    10.0   187.0     3
bench: aggregate wall throughput 60.0 fps
bench: aggregate active throughput 80.0 fps
bench: legacy dwell-weighted busy score 80.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                ValueError, "workload reported command errors"):
                compare_logs.parse_log(path)

    def test_image_scale_command_error_is_rejected(self):
        content = """
I (1) app_init: App version:      scale-error
bench: config protocol=12 diagnostics=off
bench: warm-up lap done, measuring
bench: image scale updates=187 commands=748 errors=1
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: image scale        60.0      80.0     5.0     7.0    10.0   187.0     5
bench: aggregate wall throughput 60.0 fps
bench: aggregate active throughput 80.0 fps
bench: legacy dwell-weighted busy score 80.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                    ValueError, "workload reported command errors"):
                compare_logs.parse_log(path)

    def test_protocol_9_recomputes_every_row_and_aggregate(self):
        content = """
I (1) app_init: App version:      exact
bench: config protocol=9 diagnostics=off
bench: warm-up lap done, measuring
bench: detail[rect x1] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: rect x1            25.0      50.0     5.0    15.0    50.0   200.0     4
bench: aggregate wall throughput 25.0 fps (observed)
bench: aggregate active throughput 50.0 fps (engine capacity)
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000 utilization=50.0%
bench: legacy dwell-weighted busy score 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
        self.assertEqual(run.raw_frames, 100)

    def test_protocol_9_rejects_rounded_row_not_backed_by_raw_data(self):
        content = """
I (1) app_init: App version:      false-row
bench: config protocol=9 diagnostics=off
bench: warm-up lap done, measuring
bench: detail[rect x1] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: rect x1            25.0      99.0     5.0    15.0    50.0   200.0     4
bench: aggregate wall throughput 25.0 fps
bench: aggregate active throughput 50.0 fps
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000 utilization=50.0%
bench: legacy dwell-weighted busy score 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(ValueError, "summary row"):
                compare_logs.parse_log(path)


if __name__ == "__main__":
    unittest.main()
