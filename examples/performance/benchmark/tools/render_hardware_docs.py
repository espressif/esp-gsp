#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Render bilingual hardware benchmark drafts from a complete public matrix.

Requires an explicit --output-dir outside this tool's source directory. --outline
writes visibly unpublished structure drafts without measurements. Publication
Markdown requires the complete declared matrix; there is no incomplete-data bypass.
"""
from __future__ import annotations

import argparse
import csv
import io
import json
import math
import re
import sys
from pathlib import Path
from typing import Any

SHA = re.compile(r"[a-f0-9]{64}\Z")
VERSION = re.compile(r"(?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*)(?:[-+][A-Za-z0-9.-]+)?\Z")
REVISION = re.compile(r"[a-f0-9]{40}(?:[a-f0-9]{24})?\Z")
MODES = ("native", "full")
CONTENT_CASES = (
    ("P_SCROLL", "Text updates", "文本更新"),
    ("P_STATIC_MOVE", "Static move", "静态移动"),
    ("P_GRID", "Grid scroll", "网格滚动"),
)
ANIMATION_CASES = (
    ("P_ANIM", "GIF page", "GIF 页面"),
    ("P_ANIM_SPARSE", "Sparse GIF page", "稀疏 GIF 页面"),
)
INTERACTION_CASES = (
    ("DRAG_COMMIT", "Page drag", "全屏拖拽"),
    ("P_DRAWER", "Drawer", "下拉面板"),
    ("P_DROPDOWN", "Dropdown", "下拉选项"),
)
# Stable display order groups by hardware family, never by measured throughput.
# Evidence key, display label, target, interface, logical, native, rotation, pixel, CPU, cores
# Display labels name the chip, interface and logical resolution; RGB888 is the only non-RGB565 profile.
CONFIGS = (
    ("C3_SPI", "ESP32-C3 SPI · 240×240", "esp32c3", "SPI", "240x240", "240x240", "0", "rgb565", 160, 1),
    ("C5_CO5300", "ESP32-C5 QSPI · 466×466", "esp32c5", "QSPI", "466x466", "466x466", "0", "rgb565", 240, 1),
    ("S3_SPI", "ESP32-S3 SPI · 320×240", "esp32s3", "SPI", "320x240", "320x240", "0", "rgb565", 240, 2),
    ("S3_QSPI", "ESP32-S3 QSPI · 360×360", "esp32s3", "QSPI", "360x360", "360x360", "0", "rgb565", 240, 2),
    ("S3_RGB", "ESP32-S3 RGB · 800×480", "esp32s3", "RGB", "800x480", "800x480", "0", "rgb565", 240, 2),
    ("S31_RGB480", "ESP32-S31 RGB · 480×480", "esp32s31", "RGB", "480x480", "480x480", "0", "rgb565", 320, 2),
    ("S31_RGB565", "ESP32-S31 RGB · 480×800", "esp32s31", "RGB", "480x800", "800x480", "270", "rgb565", 320, 2),
    ("S31_RGB888", "ESP32-S31 RGB888 · 800×480", "esp32s31", "RGB", "800x480", "800x480", "0", "rgb888", 320, 2),
    ("P4_EYE_SPI", "ESP32-P4 SPI · 240×240", "esp32p4", "SPI", "240x240", "240x240", "0", "rgb565", 400, 2),
    ("P4_V1_DSI", "ESP32-P4 DSI 360 MHz · 1024×600", "esp32p4", "MIPI-DSI", "1024x600", "1024x600", "0", "rgb565", 360, 2),
    ("P4_V3_DSI", "ESP32-P4 DSI 400 MHz · 1024×600", "esp32p4", "MIPI-DSI", "1024x600", "1024x600", "0", "rgb565", 400, 2),
)


BASE_CONFIGS = CONFIGS
MOSAICO_CONFIG = ("S31_CO5300", "ESP32-S31 QSPI · 480×480", "esp32s31", "QSPI", "480x480", "480x480", "0", "rgb565", 320, 2)
CONFIGS = BASE_CONFIGS[:5] + (MOSAICO_CONFIG,) + BASE_CONFIGS[5:]

class IncompleteMatrix(ValueError):
    pass


def require(condition: Any, message: str) -> None:
    if not condition:
        raise IncompleteMatrix(message)


def counter(value: Any, *, positive: bool = False) -> int:
    require(type(value) is int and value >= (1 if positive else 0), "Invalid raw counter")
    return value


def close(actual: Any, expected: float) -> bool:
    return type(actual) in (int, float) and math.isfinite(actual) and math.isclose(
        actual, expected, rel_tol=1e-9, abs_tol=1e-7)


def cpu_percent(cpu: dict[str, Any]) -> float:
    cores = counter(cpu["cores"], positive=True)
    total = counter(cpu["total_us"], positive=True)
    capacity = total * cores
    idle = counter(cpu["idle_us"])
    adjustment = counter(cpu["idle_adjust_us"])
    require(cpu.get("valid", True) is True, "Invalid CPU sample")
    require(cpu["capacity_us"] == capacity, "CPU capacity mismatch")
    require(0 <= idle - adjustment <= capacity, "CPU adjusted idle outside capacity")
    require(len(cpu["core_idle_us"]) == len(cpu["core_idle_adjust_us"]) == cores,
            "CPU core record mismatch")
    require(sum(cpu["core_idle_us"]) == idle and sum(cpu["core_idle_adjust_us"]) == adjustment,
            "CPU raw/adjusted core sums mismatch")
    windows = counter(cpu["windows"], positive=True)
    tolerance = counter(cpu["tolerance_us"], positive=True)
    require(all(0 <= value <= tolerance * windows for value in cpu["core_idle_adjust_us"]),
            "CPU endpoint adjustment exceeds its declared bound")
    value = 100.0 * (capacity - idle + adjustment) / capacity
    require(close(cpu["busy_percent"], value), "CPU percentage differs from raw weighted counters")
    return value


def fps(sample: dict[str, Any]) -> float:
    frames = counter(sample["frames"])
    wall = counter(sample["wall_us"], positive=True)
    value = frames * 1e6 / wall
    require(close(sample["wall_fps"], value), "FPS differs from raw frame/wall counters")
    return value


def case_map(sample: dict[str, Any]) -> dict[str, dict[str, Any]]:
    cases = sample["cases"]
    require(isinstance(cases, list) and len(cases) == 58, "58 complete cases are required")
    result = {case["case_id"]: case for case in cases}
    require(len(result) == 58, "Case IDs must be unique")
    representative = CONTENT_CASES + ANIMATION_CASES + INTERACTION_CASES
    require(all(case_id in result for case_id, *_ in representative), "Representative case IDs are missing")
    return result


def startup_deltas(group: dict[str, Any], heap: str) -> list[int]:
    result = []
    for run in group["repeats"]:
        memory = run["startup_memory"]
        entry = counter(memory["app_entry"][heap]["free_bytes"])
        started = counter(memory["gsp_started"][heap]["free_bytes"])
        result.append(entry - started)
    return result


def verify_pool(pooled: dict[str, Any], samples: list[dict[str, Any]]) -> None:
    for key in ("frames", "wall_us", "render_busy_us"):
        require(pooled[key] == sum(item[key] for item in samples), "Pooled frame/time sums mismatch")
    for key in ("total_us", "capacity_us", "idle_us", "idle_adjust_us", "windows"):
        require(pooled["cpu"][key] == sum(item["cpu"][key] for item in samples), "Pooled CPU sums mismatch")
    fps(pooled)
    cpu_percent(pooled["cpu"])


def validate_matrix(data: dict[str, Any]) -> dict[tuple[str, str], dict[str, Any]]:
    require(data.get("schema") == "gsp-benchmark-public-v1", "Unsupported public matrix schema")
    require(data.get("complete") is True, "Matrix is incomplete; publishing-shaped drafts were not written")
    require(data.get("required_protocol") == 30 and data.get("required_case_count") == 58 and
            data.get("required_independent_boots_per_mode") == 2, "Unexpected measurement contract")
    require(data.get("included_board_count") == len(CONFIGS) and data.get("not_in_this_run_board_count") == 0,
            "This page requires every declared profile, including both RGB formats")
    require(not data.get("failures"), "Public matrix still contains validation failures")
    require(not any(data.get(key) for key in ("synthetic", "test_fixture", "is_fixture")),
            "Fixture data cannot produce publication drafts")
    boards = {board["board"]: board for board in data["boards"]}
    require(len(boards) == len(data["boards"]) == len(CONFIGS) and set(boards) == {row[0] for row in CONFIGS},
            "The declared board set is incomplete or unexpected")
    groups = {(group["board"], group["mode"]): group for group in data["groups"]}
    require(len(groups) == len(data["groups"]) == len(CONFIGS) * len(MODES) and
            set(groups) == {(row[0], mode) for row in CONFIGS for mode in MODES},
            "Both modes are required for every profile")
    all_case_ids: set[str] | None = None
    for label, _, chip, interface, logical, native, rotation, pixel, mhz, cores in CONFIGS:
        board = boards[label]
        display = board["display"]
        require(board["chip"] == chip and display["interface"] == interface and
                display["pixel_format"].lower() == pixel and
                display["resolution_px"] == list(map(int, native.split("x"))) and board["cpu_mhz"] == mhz,
                f"Unexpected board configuration: {label}")
        for mode in MODES:
            group = groups[label, mode]
            require(group["state"] == "valid" and group["publishable"] is True and
                    not group["reasons"] and group["valid_repeats"] == group["required_repeats"] == 2,
                    f"Two validated independent boots are required: {label}/{mode}")
            runs = group["repeats"]
            require(len(runs) == 2 and len({run["run_index"] for run in runs}) == 2 and
                    len({run["log_sha256"] for run in runs}) == 2 and
                    all(SHA.fullmatch(run["log_sha256"]) for run in runs), "Independent boot identities are missing")
            build = group["build"]
            counter(build["app_binary_bytes"], positive=True)
            counter(build["elf_static_memory"]["static_internal_sram_bytes"], positive=True)
            require(build["settings"]["esp_default_cpu_freq_mhz"] == mhz, "CPU build frequency mismatch")
            maps = []
            for run in runs:
                require(run["state"] == "validated_repeat" and run["case_count"] == 58,
                        "A repeat lacks validated 58-case evidence")
                fingerprint = run["fingerprint"]
                expected = {"protocol": "30", "target": chip, "logical": logical, "rotation": rotation,
                            "pixel": pixel, "pressure": mode, "frame_ms": "16", "cpu_cores": str(cores),
                            "cpu_adjust": "per_core_tick_clamp_v1", "cpu_clock": "esp_timer", "cpu_bits": "64"}
                require(all(fingerprint.get(key) == value for key, value in expected.items()),
                        f"Unexpected run fingerprint: {label}/{mode}")
                require(run["cpu"]["cores"] == cores, "Measured CPU core count mismatch")
                # A parsed per-repeat percentage is rounded to basis points. The
                # published values below are derived from unrounded pooled sums.
                counter(run["frames"])
                counter(run["wall_us"], positive=True)
                maps.append(case_map(run))
            require(runs[0]["fingerprint"] == runs[1]["fingerprint"], "Repeat fingerprint mismatch")
            pooled = group["pooled"]
            pooled_map = case_map(pooled)
            ids = set(pooled_map)
            require(all(set(cases) == ids for cases in maps), "Repeat case sets differ")
            require(all_case_ids is None or all_case_ids == ids, "Case sets differ across hardware or modes")
            all_case_ids = ids
            verify_pool(pooled, runs)
            for case_id, case in pooled_map.items():
                verify_pool(case, [cases[case_id] for cases in maps])
            startup_deltas(group, "internal")
            startup_deltas(group, "psram")
    return groups


def validate_measurement(data: dict[str, Any], groups: dict) -> dict[str, str]:
    require(isinstance(data, dict) and data.get("schema") == "gsp-hardware-benchmark-measurement-v1",
            "Unsupported measurement identity schema")
    require(data.get("protocol") == 30 and data.get("cases_per_measured_boot") == 58 and
            data.get("independent_boots_per_mode") == 2, "Measurement contract differs from the matrix")
    gsp, gspc, idf = data["gsp"]["version"], data["gspc"]["version"], data["sdk"]["base_commit"]
    require(isinstance(gsp, str) and VERSION.fullmatch(gsp) and
            isinstance(gspc, str) and VERSION.fullmatch(gspc) and
            isinstance(idf, str) and REVISION.fullmatch(idf), "Invalid measured version identity")
    builds = {(build["board"], build["mode"]): build for build in data["builds"]}
    require(len(builds) == len(data["builds"]) and set(builds) == set(groups),
            "Measurement build set differs from the matrix")
    for key, group in groups.items():
        for field in ("app_sha256", "elf_sha256", "sdkconfig_sha256"):
            value = builds[key].get(field)
            require(isinstance(value, str) and SHA.fullmatch(value) and value == group["build"].get(field),
                    f"Measurement artifact differs from the matrix: {key[0]}/{key[1]}/{field}")
    return {"gsp": gsp, "gspc": gspc, "idf": idf}


def table(headers: list[str], rows: list[list[str]], *, text_columns: int = 1) -> str:
    def line(values: list[str]) -> str:
        return "| " + " | ".join(values) + " |"
    return "\n".join(["<div class=\"gsp-statistics\">", "", line(headers),
                       line(["---"] * text_columns + ["---:"] * (len(headers) - text_columns)),
                       *(line(row) for row in rows), "", "</div>"])


def number(value: float) -> str:
    require(math.isfinite(value), "Cannot render a non-finite number")
    return f"{value:.2f}"


def memory_range(values: list[int], divisor: int) -> str:
    low, high = min(values) / divisor, max(values) / divisor
    return number(low) if number(low) == number(high) else f"{number(low)}–{number(high)}"


def configuration_table(language: str) -> str:
    zh = language == "zh-cn"
    headers = ["配置", "CPU MHz", "接口", "逻辑 → 面板", "旋转", "格式"] if zh else [
        "Configuration", "CPU MHz", "Interface", "Logical → panel", "Rotation", "Format"]
    return table(headers, [[name, str(mhz), interface, logical.replace("x", "×") + " → " + native.replace("x", "×"),
                           rotation + "°", pixel.upper()]
                          for _, name, _, interface, logical, native, rotation, pixel, mhz, _ in CONFIGS])


def overview_points(groups: dict, language: str) -> str:
    zh = language == "zh-cn"
    native = [(config, case_map(groups[config[0], "native"]["pooled"])) for config in CONFIGS]
    count = len(CONFIGS)

    gif_fps = [fps(cases["P_ANIM"]) for _, cases in native]
    gif_cpu = [(cpu_percent(cases["P_ANIM"]["cpu"]), config) for config, cases in native]
    peak_cpu, peak_config = max(gif_cpu)
    dual_cpu = max(value for value, config in gif_cpu if config[9] > 1)
    paced = min(gif_fps) >= 19.5
    move = [(fps(cases["P_STATIC_MOVE"]), config[1]) for config, cases in native]
    (move_low, _), (move_high, move_name) = min(move), max(move)
    drag = [fps(cases["DRAG_COMMIT"]) for _, cases in native]
    pressure = [fps(groups[config[0], "full"]["pooled"]) for config in CONFIGS]

    gif_range = f"{number(min(gif_fps))}–{number(max(gif_fps))} FPS"
    single_peak = peak_config[9] == 1
    if zh:
        points = [
            f"**动图：** {count} 项配置的 4 视图 GIF 页面均为 {gif_range}" +
            ("，跟上 20 FPS 源节奏" if paced else "") +
            (f"；双核芯片 CPU 不超过 {number(dual_cpu)}%，单核芯片最高为 {number(peak_cpu)}%（{peak_config[1]}）。"
             if single_peak else f"；CPU 不超过 {number(peak_cpu)}%。"),
            f"**内容移动：** 全部配置不低于 {number(move_low)} FPS，{move_name} 最高为 {number(move_high)} FPS。",
            f"**全屏拖拽：** {number(min(drag))}–{number(max(drag))} FPS，含输入回放中的动作间隔。",
            f"**全屏重绘压力：** 此模式下 58 项汇总平均为 {number(min(pressure))}–{number(max(pressure))} FPS。",
        ]
    else:
        points = [
            f"**Animation:** four-view GIF pages run at {gif_range} on all {count} configurations" +
            (", keeping pace with the 20 FPS source" if paced else "") +
            (f". Dual-core chips use no more than {number(dual_cpu)}% CPU; single-core chips peak at {number(peak_cpu)}% ({peak_config[1]})."
             if single_peak else f", using no more than {number(peak_cpu)}% CPU."),
            f"**Moving content:** every configuration reaches at least {number(move_low)} FPS, up to {number(move_high)} FPS on {move_name}.",
            f"**Full-screen drag:** {number(min(drag))}–{number(max(drag))} FPS, including pauses in input replay.",
            f"**Full-repaint pressure:** the combined averages across 58 cases in this mode range from {number(min(pressure))} to {number(max(pressure))} FPS.",
        ]
    return "\n".join("- " + point for point in points)


def selected_table(groups: dict, language: str, cases_to_show: tuple) -> str:
    zh = language == "zh-cn"
    headers = (["配置"] if zh else ["Configuration"]) + [case[2 if zh else 1] for case in cases_to_show]
    rows = []
    for label, name, *_ in CONFIGS:
        cases = case_map(groups[label, "native"]["pooled"])
        rows.append([name] + [number(fps(cases[key])) + " / " + number(cpu_percent(cases[key]["cpu"])) + "%"
                              for key, *_ in cases_to_show])
    return table(headers, rows)


def pressure_table(groups: dict, language: str) -> str:
    headers = ["配置", "58 项平均 FPS", "CPU %"] if language == "zh-cn" else [
        "Configuration", "58-case average FPS", "CPU %"]
    return table(headers, [[name, number(fps(groups[label, "full"]["pooled"])),
                            number(cpu_percent(groups[label, "full"]["pooled"]["cpu"]))]
                           for label, name, *_ in CONFIGS])


def full_case_tables(groups: dict | None, language: str) -> str:
    zh = language == "zh-cn"
    sections = []
    for mode in MODES:
        label = (("正常更新" if mode == "native" else "全屏重绘压力") if zh else
                 ("Normal updates" if mode == "native" else "Full-repaint pressure"))
        if groups is None:
            content = "_完整矩阵载入后生成。_" if zh else "_Generated after the complete matrix is loaded._"
        else:
            samples = [case_map(groups[row[0], mode]["pooled"]) for row in CONFIGS]
            rows = [[case["case"]] + [number(fps(sample[case_id])) for sample in samples]
                    for case_id, case in samples[0].items()]
            content = table(["案例" if zh else "Case"] + [row[1] for row in CONFIGS], rows)
        sections.append("<details>\n<summary>" + label + ("：全部 58 项 FPS" if zh else ": all 58 cases, FPS") +
                        "</summary>\n\n" + content + "\n\n</details>")
    note = ("拖拽取消可能不提交画面，需结合动作结果判断交互是否完成。" if zh else
            "A cancelled drag may submit no image; interaction completion must also be checked through its functional outcome.")
    return "\n\n".join(sections + [note])


def repeat_summary(groups: dict, language: str) -> str:
    fps_ranges, cpu_ranges = [], []
    for group in groups.values():
        frame_rates = [fps(run) for run in group["repeats"]]
        cpu_rates = [cpu_percent(run["cpu"]) for run in group["repeats"]]
        mean = sum(frame_rates) / len(frame_rates)
        fps_ranges.append(100 * (max(frame_rates) - min(frame_rates)) / mean if mean else 0)
        cpu_ranges.append(max(cpu_rates) - min(cpu_rates))
    if language == "zh-cn":
        return (f"各配置与模式的整组加权结果中，两次运行 FPS 的最大相对极差为 {number(max(fps_ranges))}%，"
                f"CPU 最大极差为 {number(max(cpu_ranges))} 个百分点。逐案例范围见 CSV；这些观测范围不是置信区间。")
    return (f"Across configuration/mode pairs, the largest relative range in whole-suite weighted FPS between runs is {number(max(fps_ranges))}%, "
            f"and the largest CPU range is {number(max(cpu_ranges))} percentage points. Per-case ranges are in the CSV; "
            "these observed ranges are not confidence intervals.")


def resource_table(groups: dict, language: str, mode: str = "native") -> str:
    zh = language == "zh-cn"
    headers = ["配置", "Flash 应用 MiB", "静态内部 RAM KiB", "启动 SRAM 增量 KiB", "启动 PSRAM 增量 MiB"] if zh else [
        "Configuration", "App in Flash MiB", "Static internal RAM KiB", "Startup SRAM increase KiB", "Startup PSRAM increase MiB"]
    rows = []
    for label, name, *_ in CONFIGS:
        group = groups[label, mode]
        build = group["build"]
        rows.append([name, number(build["app_binary_bytes"] / 1048576),
                     number(build["elf_static_memory"]["static_internal_sram_bytes"] / 1024),
                     memory_range(startup_deltas(group, "internal"), 1024),
                     memory_range(startup_deltas(group, "psram"), 1048576)])
    return table(headers, rows)


def download_csvs(groups: dict) -> dict[str, str]:
    cases, resources = [], []
    for label, config_name, chip, interface, logical, native, rotation, pixel, mhz, cores in CONFIGS:
        for mode in MODES:
            group = groups[label, mode]
            samples = [case_map(run) for run in group["repeats"]]
            for case_id, case in case_map(group["pooled"]).items():
                repeats = [sample[case_id] for sample in samples]
                frame_rates = [fps(sample) for sample in repeats]
                cpu_rates = [cpu_percent(sample["cpu"]) for sample in repeats]
                row = {"configuration": config_name, "mode": mode, "case_id": case_id, "case": case["case"],
                       "frames": case["frames"], "wall_us": case["wall_us"], "fps": fps(case),
                       "fps_min": min(frame_rates), "fps_max": max(frame_rates),
                       "cpu_percent": cpu_percent(case["cpu"]),
                       "cpu_percent_min": min(cpu_rates), "cpu_percent_max": max(cpu_rates)}
                for heap in ("internal", "psram"):
                    values = [sample.get("memory", {}).get(heap + "_incremental_sampled_peak_bytes")
                              for sample in repeats]
                    row[heap + "_sampled_peak_increase_bytes"] = (
                        max(values) if all(type(value) is int for value in values) else None)
                cases.append(row)
            build = group["build"]
            row = {"configuration": config_name, "mode": mode, "chip": chip, "interface": interface,
                   "logical_resolution": logical, "panel_resolution": native,
                   "rotation": rotation, "pixel_format": pixel,
                   "cpu_mhz": mhz, "cpu_cores": cores,
                   "present_mode": group["repeats"][0]["fingerprint"].get("resolved_mode"),
                   "framebuffers": build["settings"].get("example_lcd_frame_buffers"),
                   "psram_xip": build["settings"].get("spiram_xip_from_psram"),
                   "render_stack_psram": build["settings"].get("esp_gsp_render_task_stack_psram"),
                   "app_flash_bytes": build["app_binary_bytes"],
                   "static_internal_sram_bytes": build["elf_static_memory"]["static_internal_sram_bytes"],
                   "linked_external_ram_bytes": build["elf_static_memory"]["linked_external_ram_bytes"]}
            for heap in ("internal", "psram"):
                values = startup_deltas(group, heap)
                row.update({f"{heap}_startup_increase_min_bytes": min(values),
                            f"{heap}_startup_increase_max_bytes": max(values)})
                free = [run["startup_memory"]["gsp_started"][heap]["free_bytes"]
                        for run in group["repeats"]]
                row.update({f"{heap}_initialized_free_min_bytes": min(free),
                            f"{heap}_initialized_free_max_bytes": max(free)})
            largest = [run["startup_memory"]["gsp_started"]["internal"]["largest_bytes"]
                       for run in group["repeats"]]
            row.update(internal_initialized_largest_block_min_bytes=min(largest),
                       internal_initialized_largest_block_max_bytes=max(largest))
            resources.append(row)
    result = {}
    for name, rows in (("cases.csv", cases), ("resources.csv", resources)):
        stream = io.StringIO(newline="")
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
        result[name] = stream.getvalue()
    return result


def memory_headroom(groups: dict | None, language: str) -> str:
    zh = language == "zh-cn"
    heading = "初始化完成时的内存余量" if zh else "Free memory after initialization"
    if groups is None:
        content = "_完整矩阵载入后生成。_" if zh else "_Generated after the complete matrix is loaded._"
    else:
        headers = (["配置 / 模式", "SRAM 可用 KiB", "SRAM 最大块 KiB", "PSRAM 可用 MiB"] if zh else
                   ["Configuration / mode", "Free SRAM KiB", "Largest SRAM block KiB", "Free PSRAM MiB"])
        modes = ({"native": "正常更新", "full": "全屏压力"} if zh else
                 {"native": "normal", "full": "full repaint"})
        rows = []
        for label, name, *_ in CONFIGS:
            for mode in MODES:
                stages = [run["startup_memory"]["gsp_started"] for run in groups[label, mode]["repeats"]]
                rows.append([f"{name} / {modes[mode]}",
                    memory_range([counter(stage["internal"]["free_bytes"]) for stage in stages], 1024),
                    memory_range([counter(stage["internal"]["largest_bytes"]) for stage in stages], 1024),
                    memory_range([counter(stage["psram"]["free_bytes"]) for stage in stages], 1048576)])
        content = table(headers, rows)
    note = ("余量在显示与 UI 初始化完成后测量，不是完整回放期间的最低余量。" if zh else
            "Measured after display and UI initialization, rather than the minimum across the full replay.")
    return "<details>\n<summary>" + heading + "</summary>\n\n" + content + "\n\n" + note + "\n\n</details>"


def render(language: str, groups: dict | None, identity: dict[str, str] | None = None) -> str:
    require(groups is None or identity is not None, "Measured version identity is required")
    gsp_label = f"ESP-GSP {identity['gsp']}" if identity is not None else "ESP-GSP"
    zh = language == "zh-cn"
    if zh:
        title = "# 硬件性能统计"
        intro = f"{gsp_label} 的完整 58 项基准，覆盖 **5 类芯片、4 种显示接口、{len(CONFIGS)} 项硬件配置**，从单核 160 MHz 的 ESP32-C3 到双核 ESP32-P4。正常更新与全屏重绘压力分别统计帧率、CPU 和内存。与 LVGL 的对照见 [GSP / LVGL 性能统计](performance.md)。"
        overview = "## 数据概览\n\n**读表方式：** 每格为 FPS / CPU。CPU 按全部核心平均，双核约 50% 即一个核满载。各配置的分辨率与像素格式不同，结果用于方案选型，不用于芯片排名。"
        normal = "## 正常更新\n\n各场景保留自身的刷新节奏。"
        content_heading = "### 文字、图片与内容移动"
        animation_heading = "### 动图刷新"
        animation_source = "两种 GIF 源均为 120×120、20 FPS；GIF 页面含 4 个同步视图，稀疏 GIF 页面为 1 个视图。接近 20 FPS 即跟上源节奏。"
        animation_note = "单张 GIF 与 LVGL 的播放对照见[单动图统计](performance.md#动图刷新)。"
        interaction_heading = "### 拖拽与下拉交互"
        fps_note = "交互 FPS 包含输入回放中的动作间隔：下拉选项约 4 FPS 是整段回放的提交率，不是控件的刷新上限。"
        pressure = "## 全屏重绘压力\n\n48 个普通页面每 1 ms 请求一次整屏背景更新，10 个转场与拖拽案例保持正常节奏。下表为 58 项总帧数除以总采样时间，与正常更新分开阅读。"
        resources = "## Flash 与内存\n\n下表为正常更新固件。Flash 为完整 58 项测试程序及内嵌资源，不是 GSP 库本身的大小；启动增量包含显示帧缓冲与 UI 初始化分配。"
        pressure_resources = "全屏压力固件的 Flash 与内存"
        full_data = "## 完整 58 项数据\n\n展开查看各案例帧率；逐案例 CPU、内存与两次运行的观测范围见 CSV。"
        config = "## 硬件配置\n\n结果包含渲染与显示输出的共同影响。"
        config_summary = "显示与处理器配置"
        config_notes = "GSP 按默认策略选择加速后端。MIPI-DSI 像素时钟为 48 MHz；ESP32-C5 使用 4 MiB PSRAM 并关闭 XIP。呈现模式与帧缓冲数量见资源 CSV。"
        method_heading = "## 测试方法"
        method_summary = "采样细节与重复性"
        method = (f"测试使用 GSP {identity['gsp']}、GSPC {identity['gspc']} 和 ESP-IDF `{identity['idf'][:11]}`。" if identity else
                  "GSP、GSPC 和 ESP-IDF 身份将在载入测量后列出。")
        method += ('每种配置、每种模式独立运行两次，预热后测量全部 58 项。\n\n'
                   '- **FPS**：总帧数除以总采样时间，不是屏幕光学刷新率。常规驱动周期为 16 ms，文字、特效、滚动与静态移动为 4 ms；文本更新逐次改写 8 行文字来模拟滚动。\n'
                   '- **CPU**：所有核的非空闲运行时间占总容量的比例。\n'
                   '- **动图**：逻辑宽度小于 320 时每个视图为 60×60，其余为 120×120。\n'
                   '- **全屏压力**：整屏更新请求不一定逐次形成一帧。C3 没有快照时，cross-fade 回退为经黑场淡入淡出。\n'
                   '- **内存**：启动增量统计应用入口至 UI 启动的 heap 变化；逐案例增量为采样值，不是精确峰值。ELF 外部 RAM 载荷与 heap 增量不能直接相加。\n\n'
                   'KiB = 1,024 字节，MiB = 1,048,576 字节。')
        download_help = table(["文件", "内容"], [
            ["cases.csv", f"{len(CONFIGS)} 配置 × 2 模式 × 58 案例：帧数、耗时、FPS、CPU、两次观测范围和内存采样增量。"],
            ["resources.csv", f"{len(CONFIGS)} 配置 × 2 模式：分辨率、频率、呈现策略、Flash 与 RAM；内存字段单位为字节。"],
        ], text_columns=2)
        links = [("逐案例结果 CSV", "cases.csv"), ("资源占用 CSV", "resources.csv")]
        pending = "结构草稿：尚未载入完整有效矩阵；不包含测量数字，不能作为发布稿。"
        placeholder = "_此表由完整矩阵中两次独立启动的原始计数生成。_"
    else:
        title = "# Hardware performance statistics"
        intro = f"The complete 58-case {gsp_label} benchmark covers **five chip families, four display interfaces and {len(CONFIGS)} hardware configurations**, from the single-core 160 MHz ESP32-C3 to the dual-core ESP32-P4. Normal updates and full-repaint pressure report FPS, CPU and memory separately. See [GSP / LVGL performance statistics](performance.md) for the LVGL comparison."
        overview = "## Results at a glance\n\n**How to read the tables:** cells show FPS / CPU. CPU is averaged across all cores, so about 50% on a dual-core chip means one core is fully busy. Resolutions and pixel formats differ, so results support hardware planning rather than ranking chips."
        normal = "## Normal updates\n\nEach scene retains its own update cadence."
        content_heading = "### Text, images and moving content"
        animation_heading = "### Animation updates"
        animation_source = "Both GIF sources are 120×120 at 20 FPS. The GIF page has four synchronized views and the sparse GIF page has one. Approaching 20 FPS means playback keeps pace with the source."
        animation_note = "See [single-GIF statistics](performance.md#animation-updates) for the playback comparison with LVGL."
        interaction_heading = "### Dragging and dropdown interaction"
        fps_note = "Interaction FPS includes pauses in input replay: about 4 FPS for dropdown selection is the submission rate across the whole replay, not the control's refresh limit."
        pressure = "## Full-repaint pressure\n\nThe 48 ordinary pages request a full-screen background update every 1 ms, while the ten transition and drag cases keep their normal cadence. The table divides total frames across all 58 cases by total sampled time and is read separately from normal updates."
        resources = "## Flash and memory\n\nThe table describes the normal-update firmware. Flash covers the complete 58-case test application and embedded assets, rather than the GSP library alone. Startup increases include display framebuffers and UI initialization."
        pressure_resources = "Flash and memory for full-repaint firmware"
        full_data = "## All 58 cases\n\nExpand the tables for per-case FPS. The CSV files retain per-case CPU, memory and observed ranges across the two runs."
        config = "## Hardware configurations\n\nResults include both rendering and display output costs."
        config_summary = "Display and processor configurations"
        config_notes = "GSP selects accelerators using its default policy. MIPI-DSI uses a 48 MHz pixel clock. ESP32-C5 has 4 MiB PSRAM with XIP disabled. Presentation mode and framebuffer count are listed in the resource CSV."
        method_heading = "## Measurement method"
        method_summary = "Sampling details and repeatability"
        method = (f"Tests use GSP {identity['gsp']}, GSPC {identity['gspc']} and ESP-IDF `{identity['idf'][:11]}`. " if identity else
                  "GSP, GSPC and ESP-IDF identities will be listed after measurements are loaded. ")
        method += ('Each configuration and mode is measured in two independent runs, with warm-up before all 58 cases.\n\n'
                   '- **FPS**: total frames divided by total sampled time, rather than optical screen refresh. The ordinary driver uses 16 ms; text, effect, scroll and static-move callbacks use 4 ms. Text updates simulate scrolling by rewriting eight rows.\n'
                   '- **CPU**: non-idle runtime divided by total capacity across all configured cores.\n'
                   '- **Animation**: each view is 60×60 when the logical width is below 320, and 120×120 otherwise.\n'
                   '- **Full-repaint pressure**: each full-screen request need not produce a frame. On C3, cross-fade falls back to fade-through-black when snapshots are unavailable.\n'
                   '- **Memory**: startup increases track heap changes from application entry to UI startup; per-case increases are sampled values, rather than exact peaks. ELF external-RAM payload and heap increases must not simply be added.\n\n'
                   'KiB = 1,024 bytes; MiB = 1,048,576 bytes.')
        download_help = table(["File", "Contents"], [
            ["cases.csv", f"{len(CONFIGS)} configurations × 2 modes × 58 cases: frames, time, FPS, CPU, two-run ranges and sampled memory increases."],
            ["resources.csv", f"{len(CONFIGS)} configurations × 2 modes: resolution, frequency, presentation, Flash and RAM. Memory fields use bytes."],
        ], text_columns=2)
        links = [("Per-case results CSV", "cases.csv"), ("Resource costs CSV", "resources.csv")]
        pending = "Structure draft: the complete validated matrix has not been loaded. Contains no measured values and is not a publication draft."
        placeholder = "_Generated from the complete matrix's raw counters from two independent boots._"

    def fold(summary: str, body: str) -> str:
        return "<details>\n<summary>" + summary + "</summary>\n\n" + body + "\n\n</details>"

    parts = [title]
    if groups is None:
        parts.append("> " + pending)
        intro = intro.replace(" 的完整", " 计划记录的完整", 1).replace("The complete 58-case", "The planned complete 58-case")
        method = method.replace("Each configuration and mode is measured in two independent runs", "Publication requires two independent runs per configuration/mode").replace("每种配置、每种模式独立运行两次", "每种配置、每种模式需独立运行两次")
    else:
        method += "\n\n" + repeat_summary(groups, language)
    parts.append(intro)
    if identity is not None:
        assets = f"../../assets/performance/gsp-{identity['gsp']}-benchmark"
        parts.append(("数据下载：" if zh else "Data: ") + " · ".join(f"[{label}]({assets}/{filename})" for label, filename in links))
        parts.append(fold("CSV 包含什么" if zh else "What the CSV files contain", download_help))
    parts += [overview,
              overview_points(groups, language) if groups is not None else placeholder,
              normal, content_heading,
              selected_table(groups, language, CONTENT_CASES) if groups is not None else placeholder,
              animation_heading, animation_source,
              selected_table(groups, language, ANIMATION_CASES) if groups is not None else placeholder,
              animation_note,
              interaction_heading,
              selected_table(groups, language, INTERACTION_CASES) if groups is not None else placeholder, fps_note,
              pressure, pressure_table(groups, language) if groups is not None else placeholder,
              resources, resource_table(groups, language) if groups is not None else placeholder,
              fold(pressure_resources, resource_table(groups, language, "full") if groups is not None else placeholder),
              memory_headroom(groups, language),
              full_data, full_case_tables(groups, language),
              config, fold(config_summary, configuration_table(language) + "\n\n" + config_notes),
              method_heading, fold(method_summary, method)]
    return "\n\n".join(parts) + "\n"

def main() -> int:
    global CONFIGS
    parser = argparse.ArgumentParser(description=__doc__)
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--matrix", type=Path, help="aggregate_matrix.py's saved matrix.json")
    source.add_argument("--outline", action="store_true", help="Structure only; never writes publication filenames")
    parser.add_argument("--measurement", type=Path,
                        help="Measured build/version identity; defaults to measurement.json beside --matrix")
    profiles = parser.add_mutually_exclusive_group()
    profiles.add_argument("--include-mosaico", dest="include_mosaico", action="store_true",
                          help="Require all 12 profiles (the default; retained for compatibility)")
    profiles.add_argument("--exclude-mosaico", dest="include_mosaico", action="store_false",
                          help="Require the 11-profile publication matrix without Mosaico")
    parser.set_defaults(include_mosaico=True)
    parser.add_argument("--output-dir", type=Path, required=True,
                        help="Write bilingual pages and compact CSV downloads beneath this directory")
    args = parser.parse_args()
    if args.outline and args.measurement is not None:
        parser.error("--measurement requires --matrix")
    if args.output_dir.resolve().is_relative_to(Path(__file__).resolve().parent):
        parser.error("--output-dir must be outside the tools source directory")
    CONFIGS = BASE_CONFIGS[:5] + (MOSAICO_CONFIG,) + BASE_CONFIGS[5:] if args.include_mosaico else BASE_CONFIGS
    try:
        groups, identity = None, None
        if args.matrix is not None:
            data = json.loads(args.matrix.read_text())
            groups = validate_matrix(data)
            measurement = args.measurement or args.matrix.with_name("measurement.json")
            identity = validate_measurement(json.loads(measurement.read_text()), groups)
        # Prepare both languages before creating or replacing any output.
        pages = {language: render(language, groups, identity) for language in ("en", "zh-cn")}
        downloads = download_csvs(groups) if groups is not None else {}
        suffix = ".outline" if args.outline else ""
        for language, content in pages.items():
            target = args.output_dir / language / "guide" / f"hardware-benchmark{suffix}.md"
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(content)
        for name, content in downloads.items():
            target = args.output_dir / "assets" / "performance" / f"gsp-{identity['gsp']}-benchmark" / name
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(content, newline="")
        print("Wrote bilingual structure outlines" if args.outline else "Wrote bilingual pages and compact CSV downloads")
        return 0
    except (IncompleteMatrix, KeyError, TypeError, ValueError, OSError) as error:
        print(f"Could not complete publication drafts: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
