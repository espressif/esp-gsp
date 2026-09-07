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
    def test_protocol_19_results_isolation_and_capacity(self):
        content = '''
I (1) app_init: App version: results-cards
bench: run mode=soak case=P_LOAD64 name="blend load x64" category=capacity dwell_ms=4000
bench: config protocol=19 pressure=full pressure_ms=1 touch=disabled queue_metric=drained hud=results-only bundle_crc=1234abcd logical=240x240
bench: warm-up lap done, measuring
bench: pressure[blend load x64] mode=full period_ms=1 ticks=200 requests=200 accepted=200 rejected=0 errors=0
bench: capacity[blend load x64] objects=64 box=60x56 alpha=128
bench: detail[blend load x64] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: ---- summary ----
bench: blend load x64 25.0 50.0 5.0 15.0 50.0 200.0 4
bench: aggregate wall throughput 25.0 fps
bench: aggregate active throughput 50.0 fps
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000
bench: legacy dwell-weighted busy score 50.0 fps
bench: codecs qoi=0/0 rle16=0/0 png=0/0 failed=0 scale_hw=0 scale_sw=0
bench: dynamic image request=0 queued=0 published=0 failed=0 cancelled=0 probe=0us
bench: measurement end
'''
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            self.assertEqual(compare_logs.parse_log(path).raw_frames, 100)
            for invalid in (
                    content.replace("bench: measurement end", ""),
                    content.replace("queue_metric=drained", "queue_metric=applied"),
                    content.replace("bundle_crc=1234abcd", "bundle_crc=bad"),
                    content.replace("box=60x56", "box=60x57"),
                    content.replace("objects=64", "objects=32"),
                    content.replace("failed=0", "failed=1"),
                    content + "bench: physical input detected; measurement invalid\n",
                    content + "bench: unexpected scene change\n",
                    content + "E (1) gsp_update: Unknown bind ID 3\n",
                    content + "bench: page visibility mismatch case=P_RECTS\n"):
                with self.subTest(invalid=invalid):
                    path.write_text(invalid)
                    with self.assertRaises(ValueError):
                        compare_logs.parse_log(path)
            dropdown = content.replace("P_LOAD64", "P_DROPDOWN").replace(
                "blend load x64", "dropdown select").replace(
                    "bench: capacity[dropdown select] objects=64 box=60x56 alpha=128",
                    "bench: dropdown selections=4 errors=0")
            path.write_text(dropdown)
            self.assertEqual(compare_logs.parse_log(path).raw_frames, 100)
            for invalid in (dropdown.replace("selections=4", "selections=0"),
                            dropdown.replace("selections=4 errors=0", "selections=4 errors=1")):
                path.write_text(invalid)
                with self.assertRaisesRegex(ValueError, "dropdown"):
                    compare_logs.parse_log(path)

    def test_protocol_18_transition_latency_window(self):
        content = """
I (1) app_init: App version: latency-window
bench: run mode=soak case=TRANSITION_LEFT name="slide left" category=transition dwell_ms=4000
bench: config protocol=18 pressure=full pressure_ms=1
bench: warm-up lap done, measuring
bench: pressure[slide left] mode=transition period_ms=1 ticks=200 requests=0 accepted=0 rejected=0 errors=0
bench: detail[slide left] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: transition[slide left] count=20 errors=0 no_visual=0 frames=100 frames_avg=5.0 latency_avg=200000.0us p50=200000us p95=210000us max=220000us snapshot=20 direct=0 inplace=0 streamed=0 path_failures=0 latency_window=latest samples=16
bench: ---- summary ----
bench: slide left 25.0 50.0 5.0 15.0 50.0 200.0 4
bench: aggregate wall throughput 25.0 fps
bench: aggregate active throughput 50.0 fps
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000
bench: legacy dwell-weighted busy score 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
            self.assertEqual(run.transitions["slide left"].latency_samples, 16)
            for invalid in (
                    content.replace("samples=16", "samples=20"),
                    content.replace(" latency_window=latest samples=16", "")):
                path.write_text(invalid)
                with self.assertRaisesRegex(ValueError, "latency window"):
                    compare_logs.parse_log(path)

    def test_protocol_18_requires_consistent_pressure_evidence(self):
        content = """
I (1) app_init: App version: pressure-test
bench: run mode=soak case=P_RECT1 name="rect x1" category=primitives dwell_ms=4000
bench: config protocol=18 pressure=full pressure_ms=1
bench: warm-up lap done, measuring
bench: pressure[rect x1] mode=full period_ms=1 ticks=200 requests=200 accepted=190 rejected=10 errors=0
bench: detail[rect x1] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: ---- summary ----
bench: rect x1 25.0 50.0 5.0 15.0 50.0 200.0 4
bench: aggregate wall throughput 25.0 fps
bench: aggregate active throughput 50.0 fps
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000
bench: legacy dwell-weighted busy score 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            self.assertEqual(compare_logs.parse_log(path).raw_frames, 100)
            invalid_samples = (
                content.replace('mode=soak case=P_RECT1 name="rect x1" category=primitives dwell_ms=4000',
                                'mode=full cases=44'),
                content.replace("case=P_RECT1", "case=P_UNKNOWN"),
                content.replace("accepted=190", "accepted=191"),
                content.replace("ticks=200", "ticks=0"),
                content.replace("rejected=10 errors=0", "rejected=9 errors=1"),
                content.replace("mode=full period", "mode=native period"),
                content.replace("accepted=190 rejected=10", "accepted=0 rejected=200"),
                "\n".join(line for line in content.splitlines()
                          if not line.startswith("bench: pressure[")),
            )
            for invalid in invalid_samples:
                with self.subTest(invalid=invalid):
                    path.write_text(invalid)
                    with self.assertRaises(ValueError):
                        compare_logs.parse_log(path)
            native = content.replace("pressure=full", "pressure=native").replace(
                "mode=full period_ms=1 ticks=200 requests=200 accepted=190 rejected=10",
                "mode=native period_ms=1 ticks=200 requests=0 accepted=0 rejected=0")
            path.write_text(native)
            self.assertEqual(compare_logs.parse_log(path).raw_frames, 100)

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

    def test_protocol_15_validates_completed_keyboard_sequence(self):
        content = """
I (1) app_init: App version:      keyboard-verified
bench: config protocol=15 diagnostics=off
bench: warm-up lap done, measuring
bench: image rotation updates=1 commands=2 errors=0
bench: detail[composites] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: keyboard presses=30 completed=30 text_updates=30 backspaces=2 overflows=0 mismatches=0 errors=0 capacity=63 final="gsp benchmarkgsp benchmark" expected="gsp benchmarkgsp benchmark"
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: composites         25.0      50.0     5.0    15.0    50.0   200.0     4
bench: aggregate wall throughput 25.0 fps
bench: aggregate active throughput 50.0 fps
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000 utilization=50.0%
bench: legacy dwell-weighted busy score 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
        self.assertEqual(run.pages["composites"].wall_fps, 25.0)

    def test_protocol_15_rejects_keyboard_mismatch(self):
        content = """
I (1) app_init: App version:      keyboard-mismatch
bench: config protocol=15 diagnostics=off
bench: warm-up lap done, measuring
bench: image rotation updates=1 commands=2 errors=0
bench: detail[composites] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: keyboard presses=30 completed=30 text_updates=29 backspaces=2 overflows=0 mismatches=1 errors=0 capacity=63 final="gsp benchmar" expected="gsp benchmark"
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: composites         25.0      50.0     5.0    15.0    50.0   200.0     4
bench: aggregate wall throughput 25.0 fps
bench: aggregate active throughput 50.0 fps
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000 utilization=50.0%
bench: legacy dwell-weighted busy score 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                    ValueError, "invalid keyboard correctness"):
                compare_logs.parse_log(path)

    def test_protocol_16_requires_long_keyboard_text(self):
        content = """
I (1) app_init: App version:      keyboard-long-path
bench: config protocol=16 keyboard_ms=4 keyboard_capacity=255 diagnostics=off
bench: warm-up lap done, measuring
bench: image rotation updates=1 commands=2 errors=0
bench: detail[composites] frames=100 elapsed_us=4000000 busy_us=2000000 render_us=500000 submit_us=1500000 service_steps=200 service_us=10000 commands=800 busy=20.000ms internal_peak=0 psram_peak=0
bench: keyboard presses=64 completed=64 text_updates=64 backspaces=1 overflows=0 mismatches=0 errors=0 capacity=255 final="gsp benchmark" expected="gsp benchmark"
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: composites         25.0      50.0     5.0    15.0    50.0   200.0     4
bench: aggregate wall throughput 25.0 fps
bench: aggregate active throughput 50.0 fps
bench: aggregate raw frames=100 wall_us=4000000 busy_us=2000000 utilization=50.0%
bench: legacy dwell-weighted busy score 50.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                    ValueError, "long-text path was not exercised"):
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

    def test_image_rotation_command_error_is_rejected(self):
        content = """
I (1) app_init: App version:      rotation-error
bench: config protocol=14 diagnostics=off
bench: warm-up lap done, measuring
bench: image rotation updates=312 commands=624 errors=1
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: image rotation     60.0      80.0     5.0     7.0    10.0   124.8     5
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

    def test_image_rotation_no_op_workload_is_rejected(self):
        content = """
I (1) app_init: App version:      rotation-no-op
bench: config protocol=14 diagnostics=off
bench: warm-up lap done, measuring
bench: image rotation updates=0 commands=0 errors=0
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: image rotation     60.0      80.0     5.0     7.0    10.0     0.0     5
bench: aggregate wall throughput 60.0 fps
bench: aggregate active throughput 80.0 fps
bench: legacy dwell-weighted busy score 80.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                    ValueError, "invalid image rotation workload result"):
                compare_logs.parse_log(path)

    def test_protocol_17_accepts_measured_render_storm(self):
        content = """
I (1) app_init: App version:      storm
bench: run mode=soak case=P_STORM name="render storm" category=saturation dwell_ms=8000
bench: config protocol=17 diagnostics=off
bench: warm-up lap done, measuring
bench: detail[render storm] frames=100 elapsed_us=8000000 busy_us=4000000 render_us=1000000 submit_us=3000000 service_steps=200 service_us=10000 commands=900 busy=40.000ms internal_peak=0 psram_peak=0
bench: storm period_ms=4 target_hz=250.0 updates=2000 commands=18000 rejected=100
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: render storm       12.5      25.0    10.0    30.0    50.0   112.5     8
bench: aggregate wall throughput 12.5 fps
bench: aggregate active throughput 25.0 fps
bench: aggregate raw frames=100 wall_us=8000000 busy_us=4000000 utilization=50.0%
bench: legacy dwell-weighted busy score 25.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            run = compare_logs.parse_log(path)
            current = content.replace(
                "protocol=17 diagnostics=off",
                "protocol=18 diagnostics=off pressure=full pressure_ms=1").replace(
                    "bench: detail[render storm]",
                    "bench: pressure[render storm] mode=full period_ms=1 ticks=200 "
                    "requests=200 accepted=200 rejected=0 errors=0\n"
                    "bench: detail[render storm]").replace(
                        "commands=18000 rejected=100",
                        "commands=18000 rejected=100 errors=0")
            path.write_text(current)
            self.assertEqual(compare_logs.parse_log(path).raw_frames, 100)
            for invalid in (
                    current.replace("rejected=100 errors=0", "rejected=100 errors=1"),
                    current.replace("rejected=100 errors=0", "rejected=100")):
                path.write_text(invalid)
                with self.assertRaisesRegex(ValueError, "render storm"):
                    compare_logs.parse_log(path)
        self.assertEqual(run.pages["render storm"].commands_s, 112.5)
        self.assertIn(("run_case", "P_STORM"), run.config)

    def test_protocol_17_rejects_non_rendering_storm(self):
        content = """
I (1) app_init: App version:      storm-rejected
bench: config protocol=17 diagnostics=off
bench: warm-up lap done, measuring
bench: detail[render storm] frames=100 elapsed_us=8000000 busy_us=4000000 render_us=1000000 submit_us=3000000 service_steps=200 service_us=10000 commands=900 busy=40.000ms internal_peak=0 psram_peak=0
bench: storm period_ms=4 target_hz=250.0 updates=2000 commands=18000 rejected=18000
bench: ---- summary ----
bench: page          wall fps  busy fps rndr ms subm ms  svc us   cmd/s   sec
bench: render storm       12.5      25.0    10.0    30.0    50.0   112.5     8
bench: aggregate wall throughput 12.5 fps
bench: aggregate active throughput 25.0 fps
bench: aggregate raw frames=100 wall_us=8000000 busy_us=4000000 utilization=50.0%
bench: legacy dwell-weighted busy score 25.0 fps
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "monitor.log"
            path.write_text(content)
            with self.assertRaisesRegex(
                    ValueError, "invalid render storm workload result"):
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
