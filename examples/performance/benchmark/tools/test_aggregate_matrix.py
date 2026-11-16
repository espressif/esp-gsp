# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Host-only evidence/aggregation checks; fake device logs never represent scores."""
import csv
import copy
import dataclasses
import hashlib
import json
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace

import aggregate_matrix as matrix
import render_hardware_docs as renderer


@dataclasses.dataclass
class Cpu:
    valid: bool
    busy_bp: int
    cores: int
    windows: int
    window_us: int
    total_us: int
    idle_us: int
    core_idle_us: tuple[int, ...]
    snapshot_us: int
    start_us: int | None = None
    end_us: int | None = None
    tolerance_us: int = 1000
    idle_adjust_us: int = 0
    core_idle_adjust_us: tuple[int, ...] = (0, 0)

    @property
    def busy_percent(self):
        return self.busy_bp / 100


@dataclasses.dataclass
class Transition:
    count: int = 2
    errors: int = 0
    no_visual: int = 0
    frames: int = 8
    frames_avg: float = 4.0
    latency_avg_us: float = 1000.0
    p50_us: int = 900
    p95_us: int = 1100
    max_us: int = 1100
    snapshot: int = 2
    direct: int = 0
    inplace: int = 0
    streamed: int = 0
    path_failures: int = 0
    latency_samples: int = 2


class Parser:
    import re
    ANSI_RE = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")
    DETAIL_RE = re.compile(r"^bench: detail\[(?P<name>.+)\] frames=(?P<frames>\d+) elapsed_us=(?P<elapsed>\d+) busy_us=(?P<busy>\d+) render_us=(?P<render>\d+) submit_us=(?P<submit>\d+) service_steps=(?P<steps>\d+) service_us=(?P<service>\d+) commands=(?P<commands>\d+)")

    @staticmethod
    def parse_log(path):
        text = path.read_text()
        mode = "full" if "MODE=full" in text else "native"
        long = "LONG=1" in text
        period = 3000 if long else 1000
        busy = 501 if "BUSY=501" in text else 500
        bp = 0 if long else 10000
        names = [f"case {index:02}" for index in range(58)]
        if "INCOMPLETE=1" in text:
            names.pop()
        pages = {name: SimpleNamespace(wall_fps=round(10 * 1e6 / period, 1), busy_fps=round(10 * 1e6 / busy, 1),
                 render_ms=0.02, submit_ms=0.03, service_us=None, commands_s=None) for name in names}
        cpus = {name: Cpu(True, bp, 2, 1, period, period, period * 2 if long else 0,
                         (period, period) if long else (0, 0), 1, (index + 1) * 10000, (index + 1) * 10000 + period)
                for index, name in enumerate(names)}
        config = {"protocol": "30", "pressure": mode, "target": "esp32s3", "logical": "320x240",
                  "pixel": "rgb565", "rotation": "0", "cpu_cores": "2", "cpu": "freertos_idle_v2",
                  "cpu_tick_hz": "1000", "cpu_tolerance_us": "1000", "cpu_adjust": "per_core_tick_clamp_v1", "service_profile": "off", "ui_timing": "off", "private": "/home/private/file",
                  "image_cache": "on", "scale_source": "dynamic-qoi"}
        if "LEGACY=1" in text:
            config["protocol"] = "29"
        if "MISMATCH=1" in text:
            config["fb"] = "99"
        return SimpleNamespace(pages=pages, cpu_pages=cpus,
            cpu_aggregate=Cpu(True, bp, 2, 58, period * 58, period * 58,
                             period * 116 if long else 0, (period * 58, period * 58) if long else (0, 0), 58),
            config=tuple(sorted(config.items())), ui_pages={}, transitions={"case 00": Transition()},
            wall_fps=round(10 * 1e6 / period, 1), score_fps=round(10 * 1e6 / busy, 1), raw_frames=580, raw_wall_us=period * 58, raw_busy_us=busy * 58)


class MatrixTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.runs = self.root / "pilot"
        self.board = {"label": "S3_SPI", "chip": "esp32s3", "mac": "12:34:56:78:90:ab",
            "by_id": "/dev/private-port", "connected_at_staging": True,
            "display": {"controller": "ILI9341", "interface": "SPI", "pixel_format": "RGB565",
                        "resolution_px": [320, 240], "private": "/home/private/source"}, "builds": {}}
        self.ready = {}
        for mode in matrix.MODES:
            build = self.root / "builds" / mode
            build.mkdir(parents=True)
            self.board["builds"][mode] = str(build)
            elf, app, config = build / "app.elf", build / "app.bin", build / "sdkconfig"
            elf.write_bytes(("elf-" + mode).encode())
            app.write_bytes(("app-" + mode).encode())
            config.write_text('CONFIG_IDF_TARGET="esp32s3"\nCONFIG_FREERTOS_GENERATE_RUN_TIME_STATS=y\nCONFIG_FREERTOS_RUN_TIME_COUNTER_TYPE_U64=y\nCONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER=y\n# CONFIG_SPIRAM is not set\nCONFIG_ESP_DEFAULT_CPU_FREQ_MHZ=240\n')
            ready = {"chip": "esp32s3", "mode": mode, "elf": str(elf), "elf_sha256": matrix.sha256(elf),
                     "app_sha256": matrix.sha256(app), "sdkconfig_sha256": matrix.sha256(config),
                     "images": [{"file": str(app), "offset": 0x10000, "bytes": app.stat().st_size, "sha256": matrix.sha256(app)}]}
            (build / "deployment-ready.json").write_text(json.dumps(ready))
            self.ready[mode] = ready
        self.status_dir = self.runs / self.board["label"]
        self.status_dir.mkdir(parents=True)
        self.status = {"board": self.board, "state": "complete", "restored": True, "runs": [],
                       "started_at": "2026-11-12", "error": "/home/private/12:34:56:78:90:ab"}
        for index, mode in enumerate(("native", "full", "full", "native")):
            ready = self.ready[mode]
            long = index >= 2
            period = 3000 if long else 1000
            lines = [f"ELF file SHA256: {ready['elf_sha256'][:12]}", "bench: run mode=full cases=58",
                f"MODE={mode} LONG={int(long)} BOOT={index}",
                "Memory [app_main entry]:", "  internal: free=700 min=650 largest=500 bytes",
                "  psram:    free=0 min=0 largest=0 bytes", "bench: warm-up lap done, measuring"]
            for case in range(58):
                lines.extend([f"bench: page case {case:02}", f"bench: case id=P_CASE_{case:02} category=synthetic dwell_ms=1000"])
                lines.append(f"bench: detail[case {case:02}] frames=10 elapsed_us={period} busy_us=500 render_us=200 submit_us=300 service_steps=0 service_us=0 commands=0 internal_peak=12 psram_peak=0")
            lines.extend(["bench: ---- summary ----", "bench: measurement end"])
            path = self.status_dir / f"run-{index}.log"
            path.write_text("\n".join(lines) + "\n")
            self.status["runs"].append({"index": index, "mode": mode, "log": str(path), "log_sha256": matrix.sha256(path),
                                       "app_sha256": ready["app_sha256"], "elf_sha256": ready["elf_sha256"]})
        self.save_status()

    def save_status(self):
        (self.status_dir / "status.json").write_text(json.dumps(self.status))

    def aggregate(self):
        return matrix.aggregate({"boards": [self.board]}, self.runs, Parser)

    def edit_log(self, index, suffix):
        record = self.status["runs"][index]
        path = Path(record["log"])
        path.write_text(path.read_text() + suffix)
        record["log_sha256"] = matrix.sha256(path)
        self.save_status()

    def retain_firmware(self):
        final = self.status["runs"][-1]
        self.status.update(restored=False, restore_required=False,
            deployment_disposition="retained_by_user", retention_authorized=True,
            retained_firmware_verified=True, retained_app_sha256=final["app_sha256"],
            retained_elf_sha256=final["elf_sha256"], retained_boot_log_sha256=final["log_sha256"])
        self.save_status()

    def test_weighted_cpu_complete_coverage_and_public_export(self):
        result = self.aggregate()
        self.assertTrue(result["complete"])
        for group in result["groups"]:
            self.assertEqual(group["valid_repeats"], 2)
            self.assertEqual(group["pooled"]["cpu"]["busy_percent"], 25)
            self.assertEqual(group["pooled"]["cpu"]["windows"], 116)
            self.assertEqual(group["pooled"]["frames"], 1160)
            self.assertEqual(group["pooled"]["stability"]["cpu_busy_percent"]["range"], 100)
            self.assertEqual(len(group["pooled"]["cases"]), 58)
            run = group["repeats"][0]
            self.assertEqual(run["startup_memory"]["app_entry"]["internal"]["min_bytes"], 650)
            case = run["cases"][0]
            self.assertIsNone(case["service_us"])
            self.assertIsNone(case["ui_step_us"])
            self.assertEqual(case["memory"]["internal_incremental_sampled_peak_bytes"], 12)
            self.assertEqual(case["case_id"], "P_CASE_00")
            self.assertEqual(case["category"], "synthetic")
            self.assertEqual(case["dwell_ms"], 1000)
            self.assertEqual(case["transition"]["p95_us"], 1100)
            self.assertEqual(run["transitions"][0]["latency_samples"], 2)
            self.assertEqual(group["build"]["elf_static_memory"]["state"], "unavailable")
        output = self.root / "public"
        matrix.export(result, output)
        for path in output.iterdir():
            text = path.read_text()
            self.assertNotIn(str(self.root), text)
            self.assertNotIn("12:34:56:78:90:ab", text)
            self.assertNotIn("2026-11-12", text)
            self.assertNotIn("/dev/private-port", text)
        with (output / "cases.csv").open() as stream:
            self.assertEqual(len(list(csv.DictReader(stream))), 232)

    def test_missing_repeat_retains_single_result_without_pooled_placeholder(self):
        self.status["runs"].pop()
        self.save_status()
        result = self.aggregate()
        native = result["groups"][0]
        self.assertFalse(result["complete"])
        self.assertFalse(native["publishable"])
        self.assertEqual(native["valid_repeats"], 1)
        self.assertIsNone(native["pooled"])
        self.assertIn("requires_two_complete_independent_boots", native["reasons"])
        self.assertTrue(result["groups"][1]["publishable"])

    def test_tampered_log_and_firmware_and_restart_rejected(self):
        for kind in ("log_digest", "firmware", "restart", "coverage", "fingerprint", "duplicate"):
            with self.subTest(kind=kind):
                original = json.loads(json.dumps(self.status))
                paths = {record["log"]: Path(record["log"]).read_text() for record in self.status["runs"]}
                if kind == "log_digest":
                    Path(self.status["runs"][0]["log"]).write_text("changed")
                elif kind == "firmware":
                    self.status["runs"][0]["elf_sha256"] = "0" * 64
                elif kind == "restart":
                    self.edit_log(0, "ELF file SHA256: 12345678\n")
                elif kind == "coverage":
                    self.edit_log(0, "INCOMPLETE=1\n")
                elif kind == "fingerprint":
                    self.edit_log(3, "MISMATCH=1\n")
                elif kind == "duplicate":
                    self.status["runs"][3] = dict(self.status["runs"][0])
                self.save_status()
                result = self.aggregate()
                self.assertFalse(result["groups"][0]["publishable"])
                self.assertIsNone(result["groups"][0]["pooled"])
                self.status = original
                for path, content in paths.items():
                    Path(path).write_text(content)
                self.save_status()

    def test_failed_transaction_and_disconnected_board_are_explicit(self):
        self.status["state"] = "failed"
        self.status["restored"] = False
        self.save_status()
        result = self.aggregate()
        self.assertFalse(result["complete"])
        self.assertEqual(result["groups"][0]["state"], "failed")
        self.board["connected_at_staging"] = False
        result = self.aggregate()
        self.assertEqual([group["state"] for group in result["groups"]], ["not_in_this_run"] * 2)
        self.assertEqual(result["failures"], [])

    def test_multiple_batches_and_ambiguous_status(self):
        result = matrix.aggregate({"boards": [self.board]}, [self.root / "matrix", self.runs], Parser)
        self.assertTrue(result["complete"])
        duplicate = self.root / "matrix" / self.board["label"]
        duplicate.mkdir(parents=True)
        (duplicate / "status.json").write_text(json.dumps(self.status))
        result = matrix.aggregate({"boards": [self.board]}, [self.root / "matrix", self.runs], Parser)
        self.assertFalse(result["complete"])
        self.assertIn("runner_status_ambiguous", [failure["reason"] for failure in result["failures"]])

    def test_size_json2_and_normalized_summary_and_unknown_are_distinct(self):
        build = Path(self.board["builds"]["native"])
        raw = {"version": "1.2", "layout": [{"name": "DIRAM", "used": 100}, {"name": "IRAM", "used": 20},
               {"name": "External RAM", "used": 500}, {"name": "HP core RAM", "used": 7}]}
        (build / "size.json").write_text(json.dumps(raw))
        size = matrix.read_size(build, self.ready["native"]["elf_sha256"], True, 10)
        self.assertEqual(size["static_internal_sram_bytes"], 120)
        self.assertEqual(size["linked_external_ram_bytes"], 500)
        self.assertEqual(size["static_auxiliary_ram_bytes"], 7)
        normalized = {"schema": 1, "app_binary_bytes": 10, "static_internal_ram_bytes": 123,
                      "linked_external_ram_bytes": 456, "other_memory_layouts": [{"name": "RTC SLOW", "used": 3}]}
        (build / "size-summary.json").write_text(json.dumps(normalized))
        size = matrix.read_size(build, self.ready["native"]["elf_sha256"], True, 10)
        self.assertEqual(size["static_internal_sram_bytes"], 123)
        self.assertEqual(size["linked_external_ram_bytes"], 456)
        self.assertEqual(matrix.read_size(build, "f" * 64, True, 11)["state"], "invalid")
        (build / "size-summary.json").write_text('{"unsupported":1}')
        size = matrix.read_size(build, "f" * 64, True, 10)
        self.assertEqual(size["state"], "unsupported_format")
        self.assertIsNone(size["linked_external_ram_bytes"])

    def test_expected_logical_rotation_core_count_and_cpu_frequency(self):
        self.board["display"]["resolution_px"] = [240, 320]
        self.board["expected"] = {"logical": "320x240", "rotation": "0", "pixel": "rgb565", "cpu_cores": "2"}
        self.board["cpu_mhz"] = 240
        self.assertTrue(self.aggregate()["complete"])
        for key, value in (("rotation", "270"), ("logical", "240x320"), ("cpu_cores", "1")):
            old = self.board["expected"][key]
            self.board["expected"][key] = value
            result = self.aggregate()
            self.assertFalse(result["complete"])
            self.assertIn("display_or_cpu_configuration_mismatch", result["groups"][0]["reasons"])
            self.board["expected"][key] = old
        self.board["cpu_mhz"] = 360
        result = self.aggregate()
        self.assertFalse(result["complete"])
        self.assertIn("cpu_frequency_configuration_mismatch", result["groups"][0]["reasons"])
        self.board["included"] = False
        self.assertEqual(self.aggregate()["groups"][0]["state"], "not_in_this_run")

    def test_c5_hp_sram_lp_sram_and_unknown_memory_do_not_become_zero(self):
        build = Path(self.board["builds"]["native"])
        path = build / "size.json"
        raw = {"version": "1.2", "layout": [{"name": "Flash", "used": 2376884},
               {"name": "HP SRAM", "used": 123748}, {"name": "LP SRAM", "used": 24}]}
        path.write_text(json.dumps(raw))
        size = matrix.read_size(build, self.ready["native"]["elf_sha256"], False, 10)
        self.assertEqual(size["static_internal_sram_bytes"], 123748)
        self.assertEqual(size["static_auxiliary_ram_bytes"], 24)
        self.assertEqual(size["linked_external_ram_bytes"], 0)
        self.assertEqual(size["state"], "available")
        path.write_text(json.dumps({"version": "1.2", "layout": [{"name": "Unknown SRAM", "used": 99}]}))
        size = matrix.read_size(build, "f" * 64, True, 10)
        self.assertIsNone(size["static_internal_sram_bytes"])
        self.assertIsNone(size["static_auxiliary_ram_bytes"])
        self.assertIsNone(size["linked_external_ram_bytes"])
        self.assertEqual(size["state"], "partial")
        # A recognized region explicitly reporting zero is distinct from absent classification.
        path.write_text(json.dumps({"version": "1.2", "layout": [{"name": "HP SRAM", "used": 0}, {"name": "LP SRAM", "used": 0}]}))
        size = matrix.read_size(build, "f" * 64, False, 10)
        self.assertEqual(size["static_internal_sram_bytes"], 0)
        self.assertEqual(size["static_auxiliary_ram_bytes"], 0)
        (build / "size-summary.json").write_text(json.dumps({"schema": 1, "app_binary_bytes": 10,
            "static_internal_ram_bytes": 123748, "linked_external_ram_bytes": 0,
            "other_memory_layouts": [{"name": "LP SRAM", "used": 24}]}))
        self.assertEqual(matrix.read_size(build, "f" * 64, True, 10)["static_auxiliary_ram_bytes"], 24)

    def test_public_settings_disclose_external_render_stack_frequency_and_buffers(self):
        path = self.root / "settings-sdkconfig"
        path.write_text('CONFIG_ESP_GSP_RENDER_TASK_STACK_PSRAM=y\n'
                        'CONFIG_ESP_GSP_RENDER_TASK_STACK_SIZE=32768\n'
                        'CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM=y\n'
                        'CONFIG_SPIRAM_SPEED=80\n'
                        'CONFIG_ESPTOOLPY_FLASHFREQ="80m"\n'
                        'CONFIG_EXAMPLE_LCD_FRAME_BUFFERS=3\n'
                        'CONFIG_PRIVATE_DATA="/home/private/path"\n')
        _, settings = matrix.read_sdkconfig(path)
        self.assertEqual(settings, {"esp_gsp_render_task_stack_psram": True,
            "esp_gsp_render_task_stack_size": 32768, "freertos_task_create_allow_ext_mem": True,
            "spiram_speed": 80, "esptoolpy_flashfreq": "80m", "example_lcd_frame_buffers": 3})
        matrix.assert_public(settings)
        result = self.aggregate()
        result["groups"][0]["build"]["settings"] = settings
        output = self.root / "settings-public"
        matrix.export(result, output)
        with (output / "groups.csv").open() as stream:
            row = next(csv.DictReader(stream))
        self.assertEqual(row["config_esp_gsp_render_task_stack_psram"], "True")
        self.assertEqual(row["config_esptoolpy_flashfreq"], "80m")
        self.assertEqual(row["config_example_lcd_frame_buffers"], "3")
        path.write_text('# CONFIG_ESP_GSP_RENDER_TASK_STACK_PSRAM is not set\n'
                        '# CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM is not set\n'
                        'CONFIG_ESPTOOLPY_FLASHFREQ="/home/private/path"\n')
        _, settings = matrix.read_sdkconfig(path)
        self.assertFalse(settings["esp_gsp_render_task_stack_psram"])
        self.assertFalse(settings["freertos_task_create_allow_ext_mem"])
        self.assertNotIn("esptoolpy_flashfreq", settings)
        self.assertNotIn("example_lcd_frame_buffers", settings)

    def test_case_identity_and_dwell_changes_invalidate_pooling(self):
        record = self.status["runs"][3]
        path = Path(record["log"])
        original = path.read_text()
        path.write_text(original.replace("P_CASE_00 category=synthetic dwell_ms=1000", "P_CASE_00 category=synthetic dwell_ms=2000"))
        record["log_sha256"] = matrix.sha256(path)
        self.save_status()
        result = self.aggregate()
        self.assertFalse(result["groups"][0]["publishable"])
        self.assertIn("repeat_case_identity_or_dwell_mismatch", result["groups"][0]["reasons"])
        path.write_text(original.replace("bench: case id=P_CASE_00 category=synthetic dwell_ms=1000\n", ""))
        record["log_sha256"] = matrix.sha256(path)
        self.save_status()
        self.assertFalse(self.aggregate()["groups"][0]["publishable"])

    def test_protocol_30_pool_keeps_per_window_idle_adjustment(self):
        first = Cpu(True, 4000, 2, 1, 1000, 1000, 1700, (1500, 200), 1,
                    tolerance_us=1000, idle_adjust_us=500, core_idle_adjust_us=(500, 0))
        second = Cpu(True, 5000, 2, 1, 3000, 3000, 3000, (0, 3000), 1,
                     tolerance_us=1000, idle_adjust_us=0, core_idle_adjust_us=(0, 0))
        pooled = matrix.pooled_cpu([matrix.cpu_dict(first), matrix.cpu_dict(second)])
        self.assertEqual(pooled["busy_percent"], 47.5)
        self.assertEqual(pooled["idle_us"], 4700)
        self.assertEqual(pooled["idle_adjust_us"], 500)
        self.assertEqual(pooled["idle_used_us"], 4200)
        self.assertEqual(pooled["core_idle_adjust_us"], [500, 0])
        self.assertEqual(pooled["busy_adjustment_percentage_points"], 6.25)
        self.assertNotEqual(pooled["busy_percent"], (40 + 50) / 2)
        self.edit_log(0, "LEGACY=1\n")
        result = self.aggregate()
        self.assertFalse(result["groups"][0]["publishable"])
        self.assertIn("run_configuration_mismatch", result["groups"][0]["reasons"])

    def test_case_fps_csv_and_stability_use_raw_ratios_not_one_decimal_summary(self):
        record = self.status["runs"][0]
        path = Path(record["log"])
        path.write_text(path.read_text().replace("busy_us=500", "busy_us=501") + "BUSY=501\n")
        record["log_sha256"] = matrix.sha256(path)
        self.save_status()
        result = self.aggregate()
        group = result["groups"][0]
        first, second = [run["cases"][0] for run in group["repeats"]]
        self.assertEqual(first["active_fps"], 10e6 / 501)
        self.assertEqual(first["reported_active_fps"], 19960.1)
        self.assertNotEqual(first["active_fps"], first["reported_active_fps"])
        self.assertEqual(second["wall_fps"], 10e6 / 3000)
        self.assertEqual(second["reported_wall_fps"], 3333.3)
        self.assertNotEqual(second["wall_fps"], second["reported_wall_fps"])
        case_stability = group["pooled"]["cases"][0]["stability"]
        self.assertEqual(case_stability["wall_fps"]["minimum"], 10e6 / 3000)
        self.assertEqual(case_stability["active_fps"]["mean"], (10e6 / 501 + 20000) / 2)
        output = self.root / "precision-public"
        matrix.export(result, output)
        with (output / "cases.csv").open() as stream:
            rows = list(csv.DictReader(stream))
        row = next(row for row in rows if row["mode"] == "native" and row["repeat"] == "2" and row["case_id"] == "P_CASE_00")
        self.assertEqual(float(row["wall_fps"]), 10e6 / 3000)
        self.assertEqual(float(row["reported_wall_fps"]), 3333.3)

    def test_cpu_raw_ratio_and_firmware_integer_truncation_are_separate(self):
        cpu = Cpu(True, 6666, 1, 1, 3, 3, 1, (1,), 0,
                  tolerance_us=1000, idle_adjust_us=0, core_idle_adjust_us=(0,))
        values = matrix.cpu_dict(cpu)
        self.assertEqual(values["busy_bp"], 6666)
        self.assertEqual(values["reported_busy_percent"], 66.66)
        self.assertAlmostEqual(values["busy_percent"], 200 / 3)
        self.assertEqual(values["raw_busy_percent"], values["busy_percent"])
        self.assertNotEqual(values["raw_busy_percent"], values["reported_busy_percent"])
        pooled = matrix.pooled_cpu([values])
        self.assertAlmostEqual(pooled["raw_busy_percent"], 200 / 3)
        self.assertIsNone(pooled["reported_busy_percent"])
        self.assertIsNone(pooled["busy_bp"])

    def test_missing_status_and_build_and_unsafe_labels(self):
        (self.status_dir / "status.json").unlink()
        (Path(self.board["builds"]["full"]) / "deployment-ready.json").unlink()
        result = self.aggregate()
        self.assertFalse(result["complete"])
        self.assertTrue(result["failures"])
        self.assertEqual(result["groups"][0]["valid_repeats"], 0)
        self.assertIsNone(result["groups"][1]["build"])
        self.board["label"] = "device-2026-11-12"
        with self.assertRaises(matrix.EvidenceError):
            self.aggregate()

    def test_retained_firmware_authorized_abba_transaction_is_publishable(self):
        self.retain_firmware()
        result = self.aggregate()
        self.assertTrue(result["complete"])
        self.assertTrue(all(group["firmware_disposition"] == "retained_by_user" for group in result["groups"]))
        self.assertFalse(self.status["restored"])
        self.assertEqual(result["groups"][0]["repeats"][0]["fingerprint"]["image_cache"], "on")
        self.assertEqual(result["groups"][0]["repeats"][0]["fingerprint"]["scale_source"], "dynamic-qoi")
        output = self.root / "retained-public"
        matrix.export(result, output)
        with (output / "groups.csv").open() as stream:
            self.assertTrue(all(row["firmware_disposition"] == "retained_by_user" for row in csv.DictReader(stream)))
        for path in output.iterdir():
            self.assertNotIn(self.board["mac"], path.read_text())
            self.assertNotIn(str(self.root), path.read_text())

    def test_retained_missing_authorization_or_final_proof_cannot_pass(self):
        self.retain_firmware()
        original = copy.deepcopy(self.status)
        for key, value in (("restore_required", True), ("retention_authorized", False),
                           ("retained_firmware_verified", False), ("restored", True),
                           ("retained_app_sha256", "0" * 64), ("retained_elf_sha256", "0" * 64),
                           ("retained_boot_log_sha256", "0" * 64), ("state", "measuring")):
            with self.subTest(key=key):
                self.status = copy.deepcopy(original)
                self.status[key] = value
                self.save_status()
                result = self.aggregate()
                self.assertFalse(result["complete"])
                self.assertTrue(all(not group["publishable"] for group in result["groups"]))
        self.status = copy.deepcopy(original)
        self.status.pop("retention_authorized")
        self.save_status()
        self.assertFalse(self.aggregate()["complete"])

    def test_retained_requires_four_ordered_runs_and_current_native_identity(self):
        self.retain_firmware()
        original = copy.deepcopy(self.status)
        for operation in ("missing", "reordered", "different_final_build"):
            with self.subTest(operation=operation):
                self.status = copy.deepcopy(original)
                if operation == "missing":
                    self.status["runs"].pop(1)
                elif operation == "reordered":
                    self.status["runs"][1], self.status["runs"][2] = self.status["runs"][2], self.status["runs"][1]
                else:
                    self.status["runs"][-1]["app_sha256"] = "0" * 64
                    self.status["retained_app_sha256"] = "0" * 64
                self.save_status()
                self.assertTrue(all(not group["publishable"] for group in self.aggregate()["groups"]))

    def test_retained_does_not_pool_any_mode_if_one_of_four_logs_fails(self):
        self.retain_firmware()
        self.edit_log(0, "INCOMPLETE=1\n")
        result = self.aggregate()
        self.assertFalse(result["complete"])
        for group in result["groups"]:
            self.assertFalse(group["publishable"])
            self.assertIsNone(group["pooled"])
            self.assertIn("retained_full_transaction_validation_incomplete", group["reasons"])

    def test_status_label_chip_and_mac_must_match_plan_for_both_dispositions(self):
        original = copy.deepcopy(self.status)
        for retained in (False, True):
            for key, value in (("label", "OTHER_BOARD"), ("chip", "esp32c5"), ("mac", "aa:bb:cc:dd:ee:ff")):
                with self.subTest(retained=retained, key=key):
                    self.status = copy.deepcopy(original)
                    if retained:
                        self.retain_firmware()
                    self.status["board"] = {**self.board, key: value}
                    self.save_status()
                    result = self.aggregate()
                    self.assertTrue(all(not group["publishable"] for group in result["groups"]))
                    self.assertIn("status_board_identity_mismatch", [item["reason"] for item in result["failures"]])

    def test_image_cache_settings_use_current_kconfig_names(self):
        path = self.root / "image-cache-sdkconfig"
        path.write_text('CONFIG_ESP_GSP_ENABLE_IMAGE_CACHE=y\n'
                        'CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MIN_BYTES=65536\n'
                        'CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MAX_BYTES=4194304\n'
                        'CONFIG_ESP_GSP_IMAGE_CACHE_ENTRIES=16\n'
                        'CONFIG_ESP_GSP_IMAGE_CACHE_ENABLE=y\n'
                        'CONFIG_ESP_GSP_IMAGE_CACHE_SIZE=123456\n')
        _, settings = matrix.read_sdkconfig(path)
        self.assertEqual(settings, {"esp_gsp_enable_image_cache": True,
            "esp_gsp_image_cache_auto_min_bytes": 65536, "esp_gsp_image_cache_auto_max_bytes": 4194304,
            "esp_gsp_image_cache_entries": 16})


class RendererIdentityTest(unittest.TestCase):
    def test_representative_cases_are_required_with_58_unique_ids(self):
        ids = ["P_SCROLL", "P_STATIC_MOVE", "P_GRID", "P_ANIM", "P_ANIM_SPARSE",
               "DRAG_COMMIT", "P_DRAWER", "P_DROPDOWN"]
        ids += [f"P_OTHER_{index}" for index in range(50)]
        sample = {"cases": [{"case_id": key} for key in ids]}
        self.assertIn("P_ANIM_SPARSE", renderer.case_map(sample))
        sample["cases"][4]["case_id"] = "P_UNKNOWN"
        with self.assertRaisesRegex(renderer.IncompleteMatrix, "Representative case IDs are missing"):
            renderer.case_map(sample)

    def setUp(self):
        # Identity-only fixtures exercise no measurement or publication scores.
        build = {"board": "HostFixture", "mode": "native", "app_sha256": "a" * 64,
                 "elf_sha256": "b" * 64, "sdkconfig_sha256": "c" * 64}
        self.groups = {("HostFixture", "native"): {"build": dict(build)}}
        self.measurement = {"schema": "gsp-hardware-benchmark-measurement-v1", "protocol": 30,
                            "cases_per_measured_boot": 58, "independent_boots_per_mode": 2,
                            "gsp": {"version": "1.10.3"}, "gspc": {"version": "0.11.2"},
                            "sdk": {"base_commit": "d" * 40}, "builds": [build]}

    def test_versions_and_downloads_follow_matched_measurement_identity(self):
        identity = renderer.validate_measurement(self.measurement, self.groups)
        self.assertEqual(identity, {"gsp": "1.10.3", "gspc": "0.11.2", "idf": "d" * 40})
        for language in ("en", "zh-cn"):
            text = renderer.render(language, None, identity)
            self.assertIn("ESP-GSP 1.10.3", text)
            self.assertIn("GSPC 0.11.2", text)
            self.assertIn("`" + "d" * 11 + "`", text)
            self.assertIn("assets/performance/gsp-1.10.3-benchmark/cases.csv", text)
            self.assertNotIn("1.10.1", text)

    def test_mismatched_app_elf_or_configuration_cannot_supply_a_version(self):
        for field in ("app_sha256", "elf_sha256", "sdkconfig_sha256"):
            with self.subTest(field=field):
                metadata = copy.deepcopy(self.measurement)
                metadata["builds"][0][field] = "e" * 64
                with self.assertRaisesRegex(renderer.IncompleteMatrix, "Measurement artifact differs"):
                    renderer.validate_measurement(metadata, self.groups)

    def test_missing_duplicate_or_different_builds_are_rejected(self):
        for builds in ([], self.measurement["builds"] * 2,
                       [{**self.measurement["builds"][0], "mode": "full"}]):
            with self.subTest(builds=builds):
                metadata = {**self.measurement, "builds": builds}
                with self.assertRaisesRegex(renderer.IncompleteMatrix, "Measurement build set differs"):
                    renderer.validate_measurement(metadata, self.groups)

    def test_invalid_version_and_measurement_contract_are_rejected(self):
        for key, value in (("protocol", 29), ("cases_per_measured_boot", 57),
                           ("independent_boots_per_mode", 1)):
            with self.subTest(key=key):
                with self.assertRaisesRegex(renderer.IncompleteMatrix, "Measurement contract differs"):
                    renderer.validate_measurement({**self.measurement, key: value}, self.groups)
        metadata = copy.deepcopy(self.measurement)
        metadata["gsp"]["version"] = "1.10.3/../private"
        with self.assertRaisesRegex(renderer.IncompleteMatrix, "Invalid measured version identity"):
            renderer.validate_measurement(metadata, self.groups)
        with self.assertRaisesRegex(renderer.IncompleteMatrix, "Measured version identity is required"):
            renderer.render("en", self.groups)


class ToolCliTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.tools = Path(__file__).resolve().parent

    def run_tool(self, name, *arguments):
        return subprocess.run([sys.executable, str(self.tools / name), *map(str, arguments)],
                              cwd=self.root, capture_output=True, text=True,
                              env={**os.environ, "PYTHONDONTWRITEBYTECODE": "1"})

    def test_aggregate_defaults_to_adjacent_parser_from_another_directory(self):
        plan = self.root / "plan.json"
        # This explicitly excluded fixture checks CLI wiring, not measured scores.
        plan.write_text(json.dumps({"boards": [{"label": "HostFixture", "chip": "esp32s3", "included": False}]}))
        output = self.root / "public"
        result = self.run_tool("aggregate_matrix.py", "--plan", plan, "--runs-dir", self.root / "runs",
                               "--output-dir", output)
        self.assertEqual(result.returncode, 1, result.stderr)
        data = json.loads((output / "matrix.json").read_text())
        self.assertFalse(data["complete"])
        self.assertEqual(data["failures"], [])
        self.assertEqual([group["state"] for group in data["groups"]], ["not_in_this_run"] * 2)

    def test_both_tools_refuse_outputs_in_tool_source_tree(self):
        output = self.tools / "generated-host-fixture"
        for tool, arguments in (("aggregate_matrix.py", ("--plan", self.root / "absent.json", "--runs-dir", self.root)),
                                ("render_hardware_docs.py", ("--outline",))):
            with self.subTest(tool=tool):
                result = self.run_tool(tool, *arguments, "--output-dir", output)
                self.assertEqual(result.returncode, 2, result.stderr)
                self.assertIn("must be outside the tools source directory", result.stderr)
        self.assertFalse(output.exists())

    def test_renderer_defaults_to_twelve_profiles_and_keeps_explicit_compatibility(self):
        for options, count in (((), 12), (("--include-mosaico",), 12), (("--exclude-mosaico",), 11)):
            with self.subTest(options=options):
                output = self.root / ("drafts-" + str(count) + str(len(options)))
                result = self.run_tool("render_hardware_docs.py", "--outline", *options, "--output-dir", output)
                self.assertEqual(result.returncode, 0, result.stderr)
                for language in ("en", "zh-cn"):
                    page = output / language / "guide" / "hardware-benchmark.outline.md"
                    text = page.read_text()
                    self.assertIn(f"{count} hardware configurations" if language == "en" else f"{count} 项硬件配置", text)
                    self.assertNotIn("1.10.1", text)
                    self.assertNotIn("assets/performance/", text)
                    self.assertFalse(page.with_name("hardware-benchmark.md").exists())

    def test_measurement_requires_a_matrix_instead_of_labelling_an_outline(self):
        output = self.root / "drafts"
        result = self.run_tool("render_hardware_docs.py", "--outline", "--measurement", self.root / "identity.json",
                               "--output-dir", output)
        self.assertEqual(result.returncode, 2, result.stderr)
        self.assertIn("--measurement requires --matrix", result.stderr)
        self.assertFalse(output.exists())

    def test_incomplete_matrix_cannot_write_publication_pages(self):
        source = self.root / "incomplete.json"
        source.write_text(json.dumps({"schema": "gsp-benchmark-public-v1", "complete": False}))
        output = self.root / "publication"
        result = self.run_tool("render_hardware_docs.py", "--matrix", source, "--output-dir", output)
        self.assertEqual(result.returncode, 1, result.stderr)
        self.assertIn("Matrix is incomplete", result.stderr)
        self.assertFalse(output.exists())


if __name__ == "__main__":
    unittest.main()
