#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Validate runner evidence and export a public benchmark matrix (standard library only).

Inputs: --plan, repeatable --runs-dir, --output-dir; --parser-dir defaults here.
Two independent, complete 58-case boots per board/mode are required for pooled
results. Individual validated repeats remain visible when a group is incomplete.
No dates, device identities, ports, private paths or raw exception text are copied.
Optional build size-summary.json schema:
  {"schema":"gsp-benchmark-size-v1", "elf_sha256":"...",
   "static_internal_sram_bytes":123, "linked_external_ram_bytes":0,
   "static_auxiliary_ram_bytes":456}
The complete public contract and metric definitions are included in matrix.json.
"""
from __future__ import annotations

import argparse
import csv
import dataclasses
import hashlib
import importlib
import json
import math
import re
import sys
from pathlib import Path
from typing import Any

MODES = ("native", "full")
CASES = 58
REPEATS = 2
SHA = re.compile(r"[a-f0-9]{64}\Z")
TOKEN = re.compile(r"[A-Za-z][A-Za-z0-9_+ .-]{0,63}\Z")
PRIVATE = re.compile(r"(?:/home/|/dev/|/tmp/|[A-Za-z]:\\|\b\d{4}-\d{2}-\d{2}\b|\b20\d{6}\b|(?:[0-9a-f]{2}:){5}[0-9a-f]{2})", re.I)
MEMORY = re.compile(
    r"Memory \[(?P<stage>[^\]]+)\]:\n"
    r"\s+internal:\s+free=(?P<ifree>\d+) min=(?P<imin>\d+) largest=(?P<ilargest>\d+) bytes\n"
    r"\s+psram:\s+free=(?P<pfree>\d+) min=(?P<pmin>\d+) largest=(?P<plargest>\d+) bytes")
STAGES = {"app_main entry": "app_entry", "LCD initialized": "lcd_initialized",
          "touch initialized": "touch_initialized", "GSP started": "gsp_started"}
FINGERPRINT_KEYS = {
    "resolved_mode", "resolved_panel", "resolved_rotation", "resolved_te", "resolved_fb", "resolved_gram", "resolved_drawbuf", "console", "console_txbuf",
    "protocol", "frame_ms", "target_hz", "stress_ms", "stress_hz", "keyboard_ms",
    "keyboard_window_ms", "keyboard_capacity", "diagnostics", "target", "logical",
    "pixel", "panel", "mode", "fb", "rotation", "te", "drawbuf", "tebuf",
    "transition_snapshots", "pressure", "pressure_ms", "touch", "queue_metric",
    "hud", "reset", "bundle_crc", "cpu", "cpu_clock", "cpu_bits", "cpu_cores",
    "cpu_tasks", "cpu_scope", "cpu_adjust", "cpu_tick_hz", "cpu_tolerance_us", "ui_timing", "service_profile",
    "image_cache", "scale_source"}
CONFIG_KEYS = (
    "CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ", "CONFIG_FREERTOS_NUMBER_OF_CORES",
    "CONFIG_SPIRAM", "CONFIG_SPIRAM_SPEED_MHZ", "CONFIG_SPIRAM_XIP_FROM_PSRAM",
    "CONFIG_SPIRAM_SPEED", "CONFIG_ESPTOOLPY_FLASHFREQ", "CONFIG_EXAMPLE_LCD_FRAME_BUFFERS",
    "CONFIG_ESP_GSP_RENDER_TASK_STACK_PSRAM", "CONFIG_ESP_GSP_RENDER_TASK_STACK_SIZE",
    "CONFIG_ESP_GSP_RENDER_TASK_STACK_SIZE_FREETYPE", "CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM",
    "CONFIG_COMPILER_OPTIMIZATION_PERF", "CONFIG_FREERTOS_HZ",
    "CONFIG_ESP_GSP_ENABLE_IMAGE_CACHE", "CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MIN_BYTES",
    "CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MAX_BYTES", "CONFIG_ESP_GSP_IMAGE_CACHE_ENTRIES",
    "CONFIG_ESP_GSP_USE_PPA", "CONFIG_ESP_GSP_USE_PIE",
    "CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS", "CONFIG_FREERTOS_RUN_TIME_COUNTER_TYPE_U64",
    "CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER")


class EvidenceError(Exception):
    """Only predefined public reason codes may be carried by this exception."""


def require(condition: Any, code: str) -> None:
    if not condition:
        raise EvidenceError(code)


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text())
    except (OSError, ValueError) as error:
        raise EvidenceError("evidence_missing_or_invalid_json") from error


def sha256(path: Path) -> str:
    try:
        digest = hashlib.sha256()
        with path.open("rb") as stream:
            for block in iter(lambda: stream.read(1024 * 1024), b""):
                digest.update(block)
        return digest.hexdigest()
    except OSError as error:
        raise EvidenceError("artifact_missing") from error


def token(value: Any) -> str:
    require(isinstance(value, str) and TOKEN.fullmatch(value) and not PRIVATE.search(value), "unsafe_public_label")
    return value


def integer(value: Any, code: str = "invalid_numeric_evidence") -> int:
    require(type(value) is int and value >= 0, code)
    return value


def public_config(values: dict[str, str]) -> dict[str, str]:
    # Fixed keys and constrained values; never export arbitrary log configuration.
    return {key: value for key, value in values.items() if key in FINGERPRINT_KEYS
            and re.fullmatch(r"[A-Za-z0-9_./%-]{1,64}", value) and not PRIVATE.search(value)
            and ("/" not in value or re.fullmatch(r"\d+(?:/\d+)+", value))}


def read_sdkconfig(path: Path) -> tuple[dict[str, str], dict[str, Any]]:
    try:
        text = path.read_text()
    except OSError as error:
        raise EvidenceError("sdkconfig_missing") from error
    options = dict(re.findall(r"^(CONFIG_\w+)=(.+)$", text, re.M))
    disabled = re.findall(r"^# (CONFIG_\w+) is not set$", text, re.M)
    options.update({name: "n" for name in disabled})
    public: dict[str, Any] = {}
    for key in CONFIG_KEYS:
        value = options.get(key)
        if value in ("y", "n"):
            public[key.removeprefix("CONFIG_").lower()] = value == "y"
        elif value and value.isdecimal():
            public[key.removeprefix("CONFIG_").lower()] = int(value)
        elif key == "CONFIG_ESPTOOLPY_FLASHFREQ" and value and re.fullmatch(r'"\d+[mM]"', value):
            public[key.removeprefix("CONFIG_").lower()] = value.strip('"')
    return options, public


def read_size(build: Path, elf_hash: str, spiram: bool, app_bytes: int) -> dict[str, Any]:
    result = {"state": "unavailable", "source": None, "static_internal_sram_bytes": None,
              "linked_external_ram_bytes": None, "static_auxiliary_ram_bytes": None}
    normalized = build / "size-summary.json"
    raw = build / "size.json"
    path = normalized if normalized.is_file() else raw
    if not path.is_file():
        return result
    try:
        data = load_json(path)
        if data.get("schema") == 1:
            require(data.get("app_binary_bytes") == app_bytes, "size_application_bytes_mismatch")
            result["static_internal_sram_bytes"] = integer(data["static_internal_ram_bytes"])
            result["linked_external_ram_bytes"] = integer(data["linked_external_ram_bytes"])
            other = data.get("other_memory_layouts", [])
            rtc = [integer(region["used"]) for region in other
                   if region.get("name") in {"RTC FAST", "RTC SLOW", "HP core RAM", "LP RAM", "LP SRAM"}]
            result["static_auxiliary_ram_bytes"] = sum(rtc) if rtc else None
            result["source"] = "idf_size_normalized_summary"
        elif data.get("schema") == "gsp-benchmark-size-v1":
            require(data.get("elf_sha256") == elf_hash, "size_elf_mismatch")
            for key in ("static_internal_sram_bytes", "linked_external_ram_bytes", "static_auxiliary_ram_bytes"):
                if data.get(key) is not None:
                    result[key] = integer(data[key])
            result["source"] = "elf_size_summary"
        elif isinstance(data.get("layout"), list):
            # ESP-IDF size JSON/JSON2 format 1.1/1.2. Memory types are physical regions;
            # DIRAM is counted once, rather than separately as instruction/data aliases.
            internal, external, rtc = [], [], []
            for region in data["layout"]:
                name, used = region["name"], integer(region["used"])
                if name in {"IRAM", "DRAM", "DIRAM", "HP SRAM"}:
                    internal.append(used)
                elif name in {"External RAM", "PSRAM"}:
                    external.append(used)
                elif name in {"RTC FAST", "RTC SLOW", "HP core RAM", "LP RAM", "LP SRAM"}:
                    rtc.append(used)
            result.update(source="idf_size_layout", static_internal_sram_bytes=sum(internal) if internal else None,
                          linked_external_ram_bytes=sum(external) if external else (0 if not spiram else None),
                          static_auxiliary_ram_bytes=sum(rtc) if rtc else None)
        else:
            result["state"] = "unsupported_format"
            return result
        result["state"] = "available" if all(result[key] is not None for key in
            ("static_internal_sram_bytes", "linked_external_ram_bytes")) else "partial"
    except (EvidenceError, KeyError, TypeError, ValueError):
        result["state"] = "invalid"
        result["source"] = None
        for key in ("static_internal_sram_bytes", "linked_external_ram_bytes", "static_auxiliary_ram_bytes"):
            result[key] = None
    return result


def read_build(board: dict[str, Any], mode: str) -> dict[str, Any]:
    build = Path(board["builds"][mode])
    ready = load_json(build / "deployment-ready.json")
    require(ready["chip"] == board["chip"] and ready["mode"] == mode, "deployment_target_or_mode_mismatch")
    for key in ("elf_sha256", "app_sha256", "sdkconfig_sha256"):
        require(isinstance(ready.get(key), str) and SHA.fullmatch(ready[key]), "invalid_artifact_digest")
    require(sha256(Path(ready["elf"])) == ready["elf_sha256"], "elf_digest_mismatch")
    require(sha256(build / "sdkconfig") == ready["sdkconfig_sha256"], "sdkconfig_digest_mismatch")
    options, settings = read_sdkconfig(build / "sdkconfig")
    require(options.get("CONFIG_IDF_TARGET", "").strip('"') == board["chip"], "sdkconfig_target_mismatch")
    require(all(options.get(name) == "y" for name in (
        "CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS", "CONFIG_FREERTOS_RUN_TIME_COUNTER_TYPE_U64",
        "CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER")), "cpu_sdkconfig_mismatch")
    if board.get("cpu_mhz") is not None:
        require(options.get("CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ") == str(board["cpu_mhz"]), "cpu_frequency_configuration_mismatch")
    images = ready["images"]
    require(isinstance(images, list) and images, "deployment_images_missing")
    app_images = []
    payload = 0
    for image in images:
        size = integer(image["bytes"])
        require(Path(image["file"]).stat().st_size == size and sha256(Path(image["file"])) == image["sha256"], "image_digest_or_size_mismatch")
        payload += size
        if image["sha256"] == ready["app_sha256"]:
            app_images.append(size)
    require(len(app_images) == 1, "application_image_ambiguous")
    return {"ready": ready, "public": {
        "app_sha256": ready["app_sha256"], "elf_sha256": ready["elf_sha256"],
        "sdkconfig_sha256": ready["sdkconfig_sha256"], "app_binary_bytes": app_images[0],
        "flash_payload_bytes": payload, "elf_file_bytes": Path(ready["elf"]).stat().st_size,
        "settings": settings, "elf_static_memory": read_size(build, ready["elf_sha256"], options.get("CONFIG_SPIRAM") == "y", app_images[0])}}


def transaction_gate(status: dict[str, Any], board: dict[str, Any]) -> tuple[bool, str]:
    """Keep the restored gate; retained firmware needs explicit final evidence.

    Full per-run parsing and artifact validation still happen in aggregate().
    This checks the transaction-level authorization and native-3 identity.
    """
    if status.get("deployment_disposition") == "retained_by_user":
        if status.get("state") != "complete":
            return False, "retained_by_user"
        require(status.get("restore_required") is False and status.get("retention_authorized") is True,
                "retained_firmware_authorization_missing")
        require(status.get("restored") is not True, "retained_firmware_must_not_claim_restoration")
        records = status.get("runs", [])
        require(isinstance(records, list) and len(records) == 4 and
                all(isinstance(record, dict) and type(record.get("index")) is int for record in records) and
                [(record.get("index"), record.get("mode")) for record in records] ==
                [(0, "native"), (1, "full"), (2, "full"), (3, "native")],
                "retained_transaction_requires_four_abba_boots")
        require(status.get("retained_firmware_verified") is True, "retained_final_firmware_verification_missing")
        native = load_json(Path(board["builds"]["native"]) / "deployment-ready.json")
        final = records[-1]
        keys = {"retained_app_sha256": "app_sha256", "retained_elf_sha256": "elf_sha256",
                "retained_boot_log_sha256": "log_sha256"}
        require(all(isinstance(status.get(key), str) and SHA.fullmatch(status[key]) and
                    status[key] == final.get(source) for key, source in keys.items()) and
                final.get("app_sha256") == native.get("app_sha256") and
                final.get("elf_sha256") == native.get("elf_sha256"),
                "retained_final_firmware_identity_mismatch")
        return True, "retained_by_user"
    if status.get("state") == "complete" and status.get("restored") is True:
        return True, "restored_original"
    return False, "pending"


def cpu_dict(cpu: Any) -> dict[str, Any]:
    value = dataclasses.asdict(cpu)
    value["core_idle_us"] = list(cpu.core_idle_us)
    value["reported_busy_percent"] = cpu.busy_bp / 100.0
    value["capacity_us"] = cpu.total_us * cpu.cores
    value["idle_used_us"] = cpu.idle_us - cpu.idle_adjust_us
    value["raw_busy_percent"] = 100 * (value["capacity_us"] - value["idle_used_us"]) / value["capacity_us"]
    value["busy_percent"] = value["raw_busy_percent"]
    value["busy_adjustment_percentage_points"] = 100 * cpu.idle_adjust_us / value["capacity_us"]
    value["endpoint_adjustment_bound_percentage_points"] = 100 * cpu.tolerance_us * cpu.windows * cpu.cores / value["capacity_us"]
    return value


def measured_text(text: str) -> str:
    marker = "bench: ---- summary ----"
    end = text.rfind(marker)
    require(end >= 0, "measured_summary_missing")
    start = max(text.rfind(marker, 0, end), text.rfind("bench: warm-up lap done, measuring", 0, end), 0)
    return text[start:end]


def read_repeat(record: dict[str, Any], build: dict[str, Any], board: dict[str, Any], mode: str, parser: Any) -> dict[str, Any]:
    ready = build["ready"]
    require(record.get("app_sha256") == ready["app_sha256"] and record.get("elf_sha256") == ready["elf_sha256"], "run_firmware_mismatch")
    log = Path(record["log"])
    digest = sha256(log)
    require(digest == record.get("log_sha256"), "log_digest_mismatch")
    text = parser.ANSI_RE.sub("", log.read_text(errors="replace")).replace("\r", "")
    boots = re.findall(r"ELF file SHA256:\s*([a-fA-F0-9]+)", text)
    require(len(boots) == 1 and len(boots[0]) >= 8 and ready["elf_sha256"].startswith(boots[0].lower()), "independent_boot_identity_invalid")
    require(text.count("bench: measurement end") == 1 and "bench: run mode=full cases=58" in text, "full_run_completion_missing")
    try:
        run = parser.parse_log(log)
    except (ValueError, KeyError, TypeError) as error:
        raise EvidenceError("benchmark_parser_rejected") from error
    config = dict(run.config or ())
    require(len(run.pages) == CASES and len(run.cpu_pages) == CASES and run.cpu_aggregate is not None,
            "complete_58_case_cpu_set_missing")
    require(int(config.get("protocol", 0)) == 30 and config.get("pressure") == mode and
            config.get("target") == board["chip"] and config.get("run_mode", "full") == "full", "run_configuration_mismatch")
    display = board.get("display", {})
    dimensions = display.get("resolution_px")
    expected = {"logical": "x".join(map(str, dimensions)), "pixel": display.get("pixel_format", "").lower()}
    expected.update(board.get("expected", {}))
    require(all(config.get(key) == str(value) for key, value in expected.items()), "display_or_cpu_configuration_mismatch")
    metadata: dict[str, dict[str, Any]] = {}
    page_name = None
    for line in measured_text(text).splitlines():
        if line.startswith("bench: page "):
            page_name = line.removeprefix("bench: page ")
        match = re.fullmatch(r"bench: case id=(\w+) category=(\w+) dwell_ms=(\d+)", line)
        if match:
            require(page_name is not None and page_name not in metadata, "case_identity_metadata_invalid")
            metadata[page_name] = {"case_id": token(match[1]), "category": token(match[2]), "dwell_ms": int(match[3])}
    require(set(metadata) == set(run.pages) and len({item["case_id"] for item in metadata.values()}) == CASES,
            "complete_case_identity_metadata_missing")
    if getattr(parser, "__file__", None):
        manifest = Path(parser.__file__).resolve().parents[1] / "main" / "bench_cases.inc"
        expected_cases = {name: (case_id, category) for case_id, name, category in re.findall(
            r'^BENCH_CASE\((\w+),\s*\w+,\s*"([^\"]+)",\s*"([^\"]+)"', manifest.read_text(), re.M)}
        require(len(expected_cases) == CASES and set(expected_cases) == set(metadata) and
                all((value["case_id"], value["category"]) == expected_cases[name] for name, value in metadata.items()),
                "case_manifest_identity_or_category_mismatch")
    details: dict[str, dict[str, Any]] = {}
    timing = config.get("service_profile") != "off"
    for line in measured_text(text).splitlines():
        match = parser.DETAIL_RE.match(line)
        if not match:
            continue
        name = match["name"]
        require(name not in details, "duplicate_case_raw_counters")
        raw = {key: int(value) for key, value in match.groupdict().items() if key != "name"}
        peak = re.search(r" internal_peak=(\d+) psram_peak=(\d+)(?:\s|$)", line)
        raw["memory"] = {"internal_incremental_sampled_peak_bytes": int(peak[1]) if peak else None,
                         "psram_incremental_sampled_peak_bytes": int(peak[2]) if peak else None}
        details[name] = raw
    require(set(details) == set(run.pages), "case_raw_set_missing")
    cases = []
    for name, page in run.pages.items():
        raw = details[name]
        ui = run.ui_pages.get(name)
        cases.append({"case": token(name), **metadata[name], "frames": raw["frames"], "wall_us": raw["elapsed"],
            "render_busy_us": raw["busy"], "render_us": raw["render"], "submit_us": raw["submit"],
            "wall_fps": raw["frames"] * 1e6 / raw["elapsed"],
            "active_fps": raw["frames"] * 1e6 / raw["busy"],
            "reported_wall_fps": page.wall_fps, "reported_active_fps": page.busy_fps,
            "render_ms_per_frame": page.render_ms, "submit_ms_per_frame": page.submit_ms,
            "service_mean_us": page.service_us, "queue_commands_per_s": page.commands_s,
            "service_iterations": raw["steps"] if timing else None,
            "service_us": raw["service"] if timing else None,
            "queue_commands": raw["commands"] if timing else None,
            "ui_steps": ui.steps if ui else None, "ui_step_us": ui.step_us if ui else None,
            "memory": raw["memory"], "cpu": cpu_dict(run.cpu_pages[name]),
            "transition": dataclasses.asdict(run.transitions[name]) if name in run.transitions else None})
    memory = {}
    for match in MEMORY.finditer(text):
        stage = STAGES.get(match["stage"])
        if stage:
            require(stage not in memory, "duplicate_startup_memory_stage")
            memory[stage] = {kind: {metric + "_bytes": int(match[prefix + metric])
                            for metric in ("free", "min", "largest")}
                            for kind, prefix in (("internal", "i"), ("psram", "p"))}
    frames, wall, busy = run.raw_frames, run.raw_wall_us, run.raw_busy_us
    require(frames and wall and busy, "aggregate_raw_missing")
    return {"state": "validated_repeat", "run_index": integer(record["index"]), "log_sha256": digest,
            "fingerprint": public_config(config), "_fingerprint": tuple(sorted(config.items())),
            "case_count": CASES, "frames": frames, "wall_us": wall, "render_busy_us": busy,
            "wall_fps": frames * 1e6 / wall, "active_fps": frames * 1e6 / busy,
            "reported_wall_fps": run.wall_fps, "reported_active_fps": run.score_fps,
            "render_busy_ratio_percent": busy * 100.0 / wall,
            "cpu": cpu_dict(run.cpu_aggregate), "startup_memory": memory, "cases": cases,
            "transitions": [{"case": token(name), **metadata[name], **dataclasses.asdict(value)}
                            for name, value in run.transitions.items()]}


def stability(values: list[float], *, percent: bool = True) -> dict[str, float | None]:
    mean = sum(values) / len(values)
    return {"minimum": min(values), "maximum": max(values), "mean": mean,
            "range": max(values) - min(values),
            "relative_range_percent": (max(values) - min(values)) * 100.0 / mean if mean and percent else None}


def pooled_cpu(samples: list[dict[str, Any]]) -> dict[str, Any]:
    cores = samples[0]["cores"]
    require(all(item["cores"] == cores for item in samples), "repeat_cpu_core_count_mismatch")
    require(all(item["tolerance_us"] == samples[0]["tolerance_us"] for item in samples), "repeat_cpu_tolerance_mismatch")
    result = {key: sum(item[key] for item in samples) for key in
              ("windows", "window_us", "total_us", "idle_us", "idle_adjust_us", "snapshot_us", "capacity_us")}
    result.update(cores=cores, tolerance_us=samples[0]["tolerance_us"],
                  core_idle_us=[sum(item["core_idle_us"][core] for item in samples) for core in range(cores)],
                  core_idle_adjust_us=[sum(item["core_idle_adjust_us"][core] for item in samples) for core in range(cores)])
    result["idle_used_us"] = result["idle_us"] - result["idle_adjust_us"]
    result["busy_percent"] = 100 * (result["capacity_us"] - result["idle_used_us"]) / result["capacity_us"]
    result["raw_busy_percent"] = result["busy_percent"]
    # No firmware printed a pooled statistic spanning separate boots.
    result["busy_bp"] = None
    result["reported_busy_percent"] = None
    result["busy_adjustment_percentage_points"] = 100 * result["idle_adjust_us"] / result["capacity_us"]
    result["endpoint_adjustment_bound_percentage_points"] = 100 * result["tolerance_us"] * result["windows"] * cores / result["capacity_us"]
    return result


def pool_repeats(repeats: list[dict[str, Any]]) -> dict[str, Any]:
    frames, wall, busy = (sum(run[key] for run in repeats) for key in ("frames", "wall_us", "render_busy_us"))
    per_case = []
    for name in [case["case"] for case in repeats[0]["cases"]]:
        samples = [next(case for case in run["cases"] if case["case"] == name) for run in repeats]
        totals = {key: sum(case[key] for case in samples) for key in ("frames", "wall_us", "render_busy_us")}
        require(all((sample["case_id"], sample["category"], sample["dwell_ms"]) ==
                    (samples[0]["case_id"], samples[0]["category"], samples[0]["dwell_ms"]) for sample in samples),
                "repeat_case_identity_or_dwell_mismatch")
        per_case.append({"case": name, **{key: samples[0][key] for key in ("case_id", "category", "dwell_ms")}, **totals,
                         "wall_fps": totals["frames"] * 1e6 / totals["wall_us"],
                         "active_fps": totals["frames"] * 1e6 / totals["render_busy_us"],
                         "cpu": pooled_cpu([case["cpu"] for case in samples]),
                         "stability": {"wall_fps": stability([case["wall_fps"] for case in samples]),
                                       "active_fps": stability([case["active_fps"] for case in samples]),
                                       "cpu_busy_percent": stability([case["cpu"]["busy_percent"] for case in samples], percent=False)}})
    return {"frames": frames, "wall_us": wall, "render_busy_us": busy, "wall_fps": frames * 1e6 / wall,
            "active_fps": frames * 1e6 / busy, "cpu": pooled_cpu([run["cpu"] for run in repeats]),
            "stability": {"wall_fps": stability([run["wall_fps"] for run in repeats]),
                          "active_fps": stability([run["active_fps"] for run in repeats]),
                          "cpu_busy_percent": stability([run["cpu"]["busy_percent"] for run in repeats], percent=False)},
            "cases": per_case}


def aggregate(plan: dict[str, Any], runs_dir: Path | list[Path], parser: Any) -> dict[str, Any]:
    boards, groups, failures = [], [], []
    run_roots = [runs_dir] if isinstance(runs_dir, Path) else runs_dir
    seen_labels: set[str] = set()
    for board in plan["boards"]:
        label, chip = token(board["label"]), token(board["chip"])
        require(label not in seen_labels, "duplicate_board_label")
        seen_labels.add(label)
        display = board.get("display", {})
        public_display = {key: token(display[key]) for key in ("controller", "interface", "pixel_format") if key in display}
        public_display["resolution_px"] = [integer(value) for value in display.get("resolution_px", [])]
        if "bus_width_bits" in display:
            public_display["bus_width_bits"] = integer(display["bus_width_bits"])
        boards.append({"board": label, "chip": chip, "display": public_display,
                       "cpu_mhz": integer(board["cpu_mhz"]) if board.get("cpu_mhz") is not None else None,
                       "expected_fingerprint": public_config({key: str(value) for key, value in board.get("expected", {}).items()})})
        if not board.get("included", board.get("connected_at_staging", True)):
            for mode in MODES:
                groups.append({"board": label, "chip": chip, "mode": mode, "state": "not_in_this_run",
                               "publishable": False, "required_repeats": REPEATS, "valid_repeats": 0,
                               "build": None, "repeats": [], "pooled": None, "reasons": ["not_in_this_run"],
                               "firmware_disposition": "not_in_this_run"})
            continue
        try:
            candidates = [root / label / "status.json" for root in run_roots if (root / label / "status.json").is_file()]
            require(len(candidates) == 1, "runner_status_missing" if not candidates else "runner_status_ambiguous")
            status = load_json(candidates[0])
            identity = status.get("board", {})
            require(identity.get("label") == label and identity.get("chip") == chip and
                    isinstance(board.get("mac"), str) and isinstance(identity.get("mac"), str) and
                    identity["mac"].lower() == board["mac"].lower(), "status_board_identity_mismatch")
            board_done, firmware_disposition = transaction_gate(status, board)
            records = status.get("runs", [])
            require(isinstance(records, list), "invalid_runner_runs")
        except EvidenceError as error:
            status, records, board_done, firmware_disposition = {}, [], False, "pending"
            failures.append({"board": label, "mode": None, "run_index": None, "reason": str(error)})
        first_group = len(groups)
        for mode in MODES:
            reasons: list[str] = []
            group = {"board": label, "chip": chip, "mode": mode, "state": "incomplete",
                     "publishable": False, "required_repeats": REPEATS, "valid_repeats": 0,
                     "build": None, "repeats": [], "pooled": None, "reasons": reasons,
                     "firmware_disposition": firmware_disposition}
            groups.append(group)
            try:
                build = read_build(board, mode)
                group["build"] = build["public"]
            except (EvidenceError, OSError, KeyError, TypeError, ValueError) as error:
                code = str(error) if isinstance(error, EvidenceError) else "build_evidence_invalid"
                reasons.append(code)
                group["state"] = "incomplete" if code == "evidence_missing_or_invalid_json" else "failed"
                failures.append({"board": label, "mode": mode, "run_index": None, "reason": code})
                continue
            seen_logs, seen_indexes = set(), set()
            selected = [record for record in records if isinstance(record, dict) and record.get("mode") == mode]
            for number, record in enumerate(sorted(selected, key=lambda value: (0, value["index"]) if type(value.get("index")) is int else (1, 0)), 1):
                try:
                    run = read_repeat(record, build, board, mode, parser)
                    require(run["log_sha256"] not in seen_logs and run["run_index"] not in seen_indexes, "duplicate_boot_evidence")
                    seen_logs.add(run["log_sha256"])
                    seen_indexes.add(run["run_index"])
                    if group["repeats"]:
                        require(run["_fingerprint"] == group["repeats"][0]["_fingerprint"], "repeat_fingerprint_mismatch")
                    run["repeat"] = number
                    group["repeats"].append(run)
                except (EvidenceError, OSError, KeyError, TypeError, ValueError) as error:
                    code = str(error) if isinstance(error, EvidenceError) else "run_evidence_invalid"
                    reasons.append(code)
                    failures.append({"board": label, "mode": mode, "run_index": record.get("index") if type(record.get("index")) is int else None, "reason": code})
            group["valid_repeats"] = len(group["repeats"])
            if len(group["repeats"]) != REPEATS:
                reasons.append("requires_two_complete_independent_boots")
            if not board_done:
                reasons.append("runner_transaction_not_complete_or_not_restored")
            if not reasons:
                try:
                    group.update(state="valid", publishable=True, pooled=pool_repeats(group["repeats"]))
                except EvidenceError as error:
                    reasons.append(str(error))
                    group.update(state="failed", publishable=False, pooled=None)
            elif status.get("state") == "failed" or any(reason not in {
                    "requires_two_complete_independent_boots", "runner_transaction_not_complete_or_not_restored"} for reason in reasons):
                group["state"] = "failed"
            for reason in reasons:
                if not any(item["board"] == label and item["mode"] == mode and item["reason"] == reason for item in failures):
                    failures.append({"board": label, "mode": mode, "run_index": None, "reason": reason})
            for run in group["repeats"]:
                run.pop("_fingerprint", None)
        # Retention authorization applies to one complete four-boot transaction.
        # No mode is pooled when any of those four independently parsed runs fails.
        board_groups = groups[first_group:]
        if board_done and firmware_disposition == "retained_by_user" and any(not group["publishable"] for group in board_groups):
            reason = "retained_full_transaction_validation_incomplete"
            for group in board_groups:
                group.update(state="failed", publishable=False, pooled=None)
                if reason not in group["reasons"]:
                    group["reasons"].append(reason)
                failures.append({"board": label, "mode": group["mode"], "run_index": None, "reason": reason})
    return {"schema": "gsp-benchmark-public-v1", "required_protocol": 30, "required_case_count": CASES,
            "required_independent_boots_per_mode": REPEATS,
            "included_board_count": sum(board.get("included", board.get("connected_at_staging", True)) is True for board in plan["boards"]),
            "not_in_this_run_board_count": sum(not board.get("included", board.get("connected_at_staging", True)) for board in plan["boards"]),
            "complete": any(group["state"] != "not_in_this_run" for group in groups) and
                        all(group["publishable"] or group["state"] == "not_in_this_run" for group in groups),
            "definitions": {
                "wall_fps": "Software-completed submitted engine frames divided by measured wall time; not optical refresh. The ordinary update driver requests every 16 ms, while text/effect/scroll/static-move stress callbacks use 4 ms even in native mode; 62.5 Hz is not a global FPS cap. Media sources such as 20 Hz GIFs can further limit content updates. Low demand-limited FPS does not establish low rendering capacity.",
                "active_fps": "Frames divided by render-path busy time, including presentation waits; not CPU utilization.",
                "cpu_busy_percent": "Whole-system non-idle FreeRTOS runtime normalized over all configured cores; ISR time is attributed to the interrupted task.",
                "pooled_cpu": "100 * (sum(capacity_us) - sum(idle_us) + sum(idle_adjust_us)) / sum(capacity_us), over disjoint measured case windows and independent boots. Raw idle and the explicit one-tick-bounded per-window adjustments are both summed; cumulative counters are not clipped again.",
                "cpu_endpoint_adjustment": "Protocol 30 keeps raw idle deltas. Each core's adjustment is max(raw_idle-window_us,0), permitted only through one configured tick. The exact change due to adjustment and its per-window theoretical bound are reported in percentage points; this does not bound every CPU measurement error.",
                "service_mean_us": "Queued-command/input service time per render-task iteration; unavailable when service profiling is disabled.",
                "ui_step_us": "UI-core work including timers, physics and viewport snapshots; unavailable when instrumentation is disabled.",
                "snapshot_us": "Sum of boundary CPU snapshot capture durations; overhead is not subtracted.",
                "startup_memory": "Observed free/minimum-free/largest-block bytes at named stages; minimum-free is the heap lifetime watermark at that stage.",
                "incremental_sampled_peak_bytes": "Case-start free heap minus lowest sampled free heap in that case; an incremental sampled increase, not total heap usage or exact peak.",
                "static_memory": "ELF linker memory-type allocation; internal SRAM includes instruction/data/bss once. Linked External RAM may include XIP text/rodata and is not dynamic PSRAM heap usage. Excludes runtime heap/frame-buffer allocations.",
                "configuration_settings": "sdkconfig values, including component defaults; asset requirements can raise the render stack beyond its configured default, with the actual allocation reported in startup logs. PSRAM stack placement, external-memory task creation permission, memory clocks and framebuffer count are disclosed when present; missing settings are unavailable, not inferred false.",
                "firmware_disposition": "restored_original: original Flash restored and readback verified. retained_by_user: benchmark firmware left installed with verified final app, ELF and boot-log identities.",
                "arithmetic_precision": "FPS and repeat stability use raw frame/time ratios. reported_wall_fps and reported_active_fps retain the one-decimal firmware summary. CPU busy_percent/raw_busy_percent use raw adjusted counters; reported_busy_percent is the firmware integer busy_bp divided by 100 and is truncated to 0.01 percentage point. A pooled result across boots has no firmware-reported CPU percentage. Extra arithmetic digits do not establish measurement accuracy.",
                "repeat_stability": "Minimum, maximum, arithmetic mean and range across independent boots; FPS relative range is range/mean*100; CPU range is percentage points. No pass threshold is inferred.",
                "missing": "null or an explicit failure reason; missing/failed measurements are never zero-filled.",
                "transition_latency": "Transition completion latency and latest-at-most-16 transition samples from the existing benchmark; not per-frame p95.",
                "evidence_limits": "Serial validation does not establish visual appearance, tearing, color accuracy, power or optical refresh."},
            "boards": boards, "groups": groups, "failures": failures}


def assert_public(value: Any) -> None:
    if isinstance(value, str):
        require(not PRIVATE.search(value), "private_content_in_public_output")
    elif isinstance(value, float):
        require(math.isfinite(value), "non_finite_public_number")
    elif isinstance(value, dict):
        for key, item in value.items():
            assert_public(key)
            assert_public(item)
    elif isinstance(value, (list, tuple)):
        for item in value:
            assert_public(item)


def flat_metrics(value: dict[str, Any]) -> dict[str, Any]:
    cpu = value["cpu"]
    return {key: value.get(key) for key in ("frames", "wall_us", "render_busy_us", "wall_fps", "active_fps",
                                          "reported_wall_fps", "reported_active_fps")} | {
        "cpu_busy_percent": cpu["busy_percent"], "cpu_total_us": cpu["total_us"], "cpu_idle_us": cpu["idle_us"],
        "cpu_raw_busy_percent": cpu["raw_busy_percent"], "cpu_reported_busy_percent": cpu["reported_busy_percent"],
        "cpu_reported_busy_bp": cpu["busy_bp"],
        "cpu_capacity_us": cpu["capacity_us"], "cpu_cores": cpu["cores"], "cpu_window_us": cpu["window_us"],
        "cpu_snapshot_us": cpu["snapshot_us"], "cpu_core_idle_us": ";".join(map(str, cpu["core_idle_us"])),
        "cpu_tolerance_us": cpu["tolerance_us"], "cpu_idle_adjust_us": cpu["idle_adjust_us"],
        "cpu_core_idle_adjust_us": ";".join(map(str, cpu["core_idle_adjust_us"])), "cpu_idle_used_us": cpu["idle_used_us"],
        "cpu_adjustment_percentage_points": cpu["busy_adjustment_percentage_points"],
        "cpu_adjustment_bound_percentage_points": cpu["endpoint_adjustment_bound_percentage_points"]}


def write_csv(path: Path, rows: list[dict[str, Any]], fallback: list[str]) -> None:
    fields = list(dict.fromkeys(key for row in rows for key in row)) or fallback
    with path.open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def export(result: dict[str, Any], output: Path) -> None:
    assert_public(result)
    output.mkdir(parents=True, exist_ok=True)
    (output / "matrix.json").write_text(json.dumps(result, ensure_ascii=False, indent=2, allow_nan=False) + "\n")
    runs, cases, groups, transitions, startup = [], [], [], [], []
    for group in result["groups"]:
        common = {"board": group["board"], "mode": group["mode"], "firmware_disposition": group["firmware_disposition"]}
        build = group["build"] or {}
        memory = build.get("elf_static_memory", {})
        row = common | {"state": group["state"], "publishable": group["publishable"], "valid_repeats": group["valid_repeats"],
                        "reasons": ";".join(group["reasons"]), "app_binary_bytes": build.get("app_binary_bytes"),
                        "flash_payload_bytes": build.get("flash_payload_bytes"), "static_memory_state": memory.get("state"),
                        "static_internal_sram_bytes": memory.get("static_internal_sram_bytes"),
                        "linked_external_ram_bytes": memory.get("linked_external_ram_bytes"), "static_auxiliary_ram_bytes": memory.get("static_auxiliary_ram_bytes")}
        row.update({"config_" + key: value for key, value in build.get("settings", {}).items()})
        if group["pooled"]:
            row.update(flat_metrics(group["pooled"]))
            row["wall_fps_repeat_relative_range_percent"] = group["pooled"]["stability"]["wall_fps"]["relative_range_percent"]
            row["cpu_repeat_range_percentage_points"] = group["pooled"]["stability"]["cpu_busy_percent"]["range"]
        groups.append(row)
        for run in group["repeats"]:
            key = common | {"repeat": run["repeat"], "run_index": run["run_index"], "group_publishable": group["publishable"]}
            runs.append(key | {"case_count": run["case_count"], "log_sha256": run["log_sha256"]} | flat_metrics(run))
            for stage, heaps in run["startup_memory"].items():
                for heap, values in heaps.items():
                    startup.append(key | {"stage": stage, "heap": heap} | values)
            for transition in run["transitions"]:
                transitions.append(key | transition)
            for case in run["cases"]:
                cases.append(key | {field: case[field] for field in ("case", "case_id", "category", "dwell_ms")} | flat_metrics(case) | {
                    field: case.get(field) for field in ("render_us", "submit_us", "service_iterations", "service_us",
                        "queue_commands", "service_mean_us", "queue_commands_per_s", "ui_steps", "ui_step_us")} | case["memory"])
    for filename, rows, fallback in (("runs.csv", runs, ["board", "mode", "repeat"]),
                                    ("cases.csv", cases, ["board", "mode", "repeat", "case"]),
                                    ("groups.csv", groups, ["board", "mode", "state"]),
                                    ("failures.csv", result["failures"], ["board", "mode", "run_index", "reason"]),
                                    ("transitions.csv", transitions, ["board", "mode", "repeat", "case_id"]),
                                    ("startup-memory.csv", startup, ["board", "mode", "repeat", "stage", "heap"])):
        write_csv(output / filename, rows, fallback)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--plan", type=Path, required=True)
    parser.add_argument("--runs-dir", type=Path, action="append", required=True,
                        help="Runner output directory; repeat for separate pilot/matrix batches")
    parser.add_argument("--parser-dir", type=Path, default=Path(__file__).resolve().parent,
                        help="Directory containing compare_logs.py (default: alongside this script)")
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--label", action="append", help="Export only selected public board labels; repeat as needed")
    args = parser.parse_args()
    if args.output_dir.resolve().is_relative_to(Path(__file__).resolve().parent):
        parser.error("--output-dir must be outside the tools source directory")
    sys.path.insert(0, str(args.parser_dir))
    benchmark_parser = importlib.import_module("compare_logs")
    plan = load_json(args.plan)
    if args.label:
        plan["boards"] = [board for board in plan["boards"] if board["label"] in args.label]
        require(len(plan["boards"]) == len(set(args.label)), "unknown_or_duplicate_selected_board")
    result = aggregate(plan, args.runs_dir, benchmark_parser)
    export(result, args.output_dir)
    print(json.dumps({"complete": result["complete"], "valid_groups": sum(group["publishable"] for group in result["groups"]),
                      "total_groups": len(result["groups"]), "failure_count": len(result["failures"])}))
    return 0 if result["complete"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
