#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Compare the final measured benchmark summary from IDF monitor logs."""

from __future__ import annotations

import argparse
import dataclasses
import re
from pathlib import Path


ANSI_RE = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")
VERSION_RE = re.compile(r"app_init: App version:\s+(\S+)")
CONFIG_RE = re.compile(r"^bench: config\s+(?P<values>.+)$")
RUN_RE = re.compile(
    r"^bench: run mode=(?P<mode>[a-z]+)"
    r"(?: cases=\d+| case=(?P<case>\S+).* dwell_ms=(?P<dwell>\d+))?$"
)
PRESSURE_RE = re.compile(
    r"^bench: pressure\[(?P<name>.+)\] mode=(?P<mode>full|native|transition)"
    r" period_ms=(?P<period>\d+) ticks=(?P<ticks>\d+)"
    r" requests=(?P<requests>\d+) accepted=(?P<accepted>\d+)"
    r" rejected=(?P<rejected>\d+) errors=(?P<errors>\d+)$"
)
RESOLVED_CONFIG_RE = re.compile(
    r"(?:^|\s)present_target: target:\s+(?P<values>.+)$"
)
INVALID_RUNTIME_RE = re.compile(
    r"(?:lcd\.rgb: LCD underrun|Guru Meditation Error|"
    r"Task watchdog got triggered|Interrupt wdt timeout|"
    r"bench: physical input detected|bench: unexpected scene change|"
    r"bench: page visibility mismatch|"
    r"gsp_update: Unknown bind ID|Display render failed:|"
    r"assert failed:|abort\(\) was called|"
    r"gsp_repeater: Repeater slots insufficient|"
    r"gsp_repeater: Repeater window exceeds slots|"
    r"gsp_repeater: Row instance create failed|"
    r"esp_gsp: list \d+: repeater init failed|"
    r"esp_gsp: list \d+: initial reconcile failed)"
)
ROW_RE = re.compile(
    r"^bench:\s+(?P<name>.+?)\s+"
    r"(?P<wall>\d+\.\d+)\s+(?P<busy>\d+\.\d+)\s+"
    r"(?P<render>\d+\.\d+)\s+(?P<submit>\d+\.\d+)\s+"
    r"(?:(?P<service>\d+\.\d+)\s+(?P<commands>\d+\.\d+)\s+)?"
    r"(?P<seconds>\d+)\s*$"
)
AGGREGATE_ACTIVE_RE = re.compile(
    r"^bench: aggregate active throughput\s+(?P<fps>\d+\.\d+) fps"
)
HISTORICAL_AGGREGATE_BUSY_RE = re.compile(
    r"^bench: aggregate busy throughput\s+(?P<fps>\d+\.\d+) fps"
)
AGGREGATE_WALL_RE = re.compile(
    r"^bench: aggregate wall throughput\s+(?P<fps>\d+\.\d+) fps"
)
AGGREGATE_RAW_RE = re.compile(
    r"^bench: aggregate raw frames=(?P<frames>\d+)"
    r"\s+wall_us=(?P<wall_us>\d+)\s+busy_us=(?P<busy_us>\d+)"
)
LEGACY_RE = re.compile(
    r"^bench: legacy dwell-weighted busy score\s+"
    r"(?P<fps>\d+\.\d+) fps"
)
HISTORICAL_WEIGHTED_RE = re.compile(
    r"^bench: weighted average\s+(?P<fps>\d+\.\d+) busy fps"
)
TRANSITION_RE = re.compile(
    r"^bench: transition\[(?P<name>.+)\]"
    r"\s+count=(?P<count>\d+)\s+errors=(?P<errors>\d+)"
    r"\s+no_visual=(?P<no_visual>\d+)\s+frames=(?P<frames>\d+)"
    r"\s+frames_avg=(?P<frames_avg>\d+\.\d+)"
    r"\s+latency_avg=(?P<latency_avg>\d+\.\d+)us"
    r"\s+p50=(?P<p50>\d+)us\s+p95=(?P<p95>\d+)us"
    r"\s+max=(?P<maximum>\d+)us"
    r"\s+snapshot=(?P<snapshot>\d+)\s+direct=(?P<direct>\d+)"
    r"\s+inplace=(?P<inplace>\d+)\s+streamed=(?P<streamed>\d+)"
    r"\s+path_failures=(?P<path_failures>\d+)"
    r"(?: latency_window=latest samples=(?P<samples>\d+))?$"
)
DRAWER_RE = re.compile(
    r"^bench: drawer opens=(?P<opens>\d+)"
    r"\s+closes=(?P<closes>\d+)\s+errors=(?P<errors>\d+)$"
)
DETAIL_RE = re.compile(
    r"^bench: detail\[(?P<name>.+)\]"
    r"\s+frames=(?P<frames>\d+)\s+elapsed_us=(?P<elapsed>\d+)"
    r"\s+busy_us=(?P<busy>\d+)\s+render_us=(?P<render>\d+)"
    r"\s+submit_us=(?P<submit>\d+)\s+service_steps=(?P<steps>\d+)"
    r"\s+service_us=(?P<service>\d+)\s+commands=(?P<commands>\d+)"
)
ERROR_COUNTER_RE = re.compile(
    r"^bench: (?:components commands=\d+|dynamic qoi accepted=\d+|"
    r"image scale updates=\d+ commands=\d+|"
    r"static_move .* commands=\d+|move .* commands=\d+|"
    r"clock .* updates=\d+ commands=\d+)"
    r".*errors=(?P<errors>\d+)"
)
ROTATION_RESULT_RE = re.compile(
    r"^bench: image rotation updates=(?P<updates>\d+)"
    r"\s+commands=(?P<commands>\d+)\s+errors=(?P<errors>\d+)$"
)
STORM_RESULT_RE = re.compile(
    r"^bench: storm period_ms=(?P<period>\d+)"
    r"\s+target_hz=(?P<hz>\d+\.\d+)\s+updates=(?P<updates>\d+)"
    r"\s+commands=(?P<commands>\d+)\s+rejected=(?P<rejected>\d+)"
    r"(?: errors=(?P<errors>\d+))?$"
)
WHEEL_ERROR_RE = re.compile(
    r"^bench: wheel command_errors=(?P<errors>\d+)$"
)
KEYBOARD_RE = re.compile(
    r'^bench: keyboard presses=(?P<presses>\d+)'
    r'(?:\s+completed=(?P<completed>\d+))?'
    r'\s+text_updates=(?P<updates>\d+)\s+backspaces=(?P<backspaces>\d+)'
    r'(?:\s+overflows=(?P<overflows>\d+)'
    r'\s+mismatches=(?P<mismatches>\d+)\s+errors=(?P<errors>\d+)'
    r'\s+capacity=(?P<capacity>\d+))?'
    r'\s+final="(?P<final>[^"]*)"'
    r'(?:\s+expected="(?P<expected>[^"]*)")?$'
)
GRID_RE = re.compile(
    r"^bench: grid binds=(?P<binds>\d+)"
    r"\s+drags=(?P<drags>\d+)\s+errors=(?P<errors>\d+)$"
)
CAPACITY_RE = re.compile(
    r"^bench: capacity\[(?P<name>.+)\] objects=(?P<objects>\d+)"
    r" box=(?P<w>\d+)x(?P<h>\d+) alpha=(?P<alpha>\d+)$")
DROPDOWN_RE = re.compile(r"^bench: dropdown selections=(\d+) errors=(\d+)$")


@dataclasses.dataclass(frozen=True)
class Page:
    wall_fps: float
    busy_fps: float
    render_ms: float
    submit_ms: float
    service_us: float | None
    commands_s: float | None
    seconds: int


@dataclasses.dataclass(frozen=True)
class RawPage:
    frames: int
    elapsed_us: int
    busy_us: int
    render_us: int
    submit_us: int
    service_steps: int
    service_us: int
    commands: int


@dataclasses.dataclass(frozen=True)
class Transition:
    count: int
    errors: int
    no_visual: int
    frames: int
    frames_avg: float
    latency_avg_us: float
    p50_us: int
    p95_us: int
    max_us: int
    snapshot: int
    direct: int
    inplace: int
    streamed: int
    path_failures: int
    latency_samples: int | None


@dataclasses.dataclass(frozen=True)
class Run:
    version: str
    score_fps: float
    metric: str
    wall_fps: float | None
    legacy_fps: float | None
    pages: dict[str, Page]
    transitions: dict[str, Transition]
    config: tuple[tuple[str, str], ...] | None
    raw_frames: int | None
    raw_wall_us: int | None
    raw_busy_us: int | None

    @property
    def weighted_fps(self) -> float:
        """Compatibility accessor for callers consuming historical logs."""
        return self.score_fps


def parse_log(path: Path) -> Run:
    text = ANSI_RE.sub("", path.read_text(errors="replace")).replace("\r", "")
    invalid = INVALID_RUNTIME_RE.search(text)
    if invalid:
        raise ValueError(
            f"{path}: invalid runtime sample ({invalid.group(0)})")
    versions = VERSION_RE.findall(text)
    if not versions:
        raise ValueError(f"{path}: app version not found")

    summary_marker = "bench: ---- summary ----"
    summary_offset = text.rfind(summary_marker)
    if summary_offset < 0:
        raise ValueError(f"{path}: measured summary not found")
    summary = text[summary_offset + len(summary_marker):]

    # A monitor may be left attached after the measured summary, causing the
    # benchmark to start another lap. Only associate transition samples from
    # the lap immediately preceding the final complete summary.
    previous_summary = text.rfind(summary_marker, 0, summary_offset)
    warmup_marker = "bench: warm-up lap done, measuring"
    measured_start = text.rfind(warmup_marker, 0, summary_offset)
    transition_start = max(previous_summary, measured_start)
    if transition_start < 0:
        transition_start = 0
    transition_text = text[transition_start:summary_offset]

    pages: dict[str, Page] = {}
    transitions: dict[str, Transition] = {}
    active_fps: float | None = None
    wall_fps: float | None = None
    legacy_fps: float | None = None
    raw_frames: int | None = None
    raw_wall_us: int | None = None
    raw_busy_us: int | None = None
    config_values: dict[str, str] = {}
    drawer_result: tuple[int, int, int] | None = None
    keyboard_result: tuple[
        int, int | None, int, int, int | None, int | None,
        int | None, int | None, str, str | None,
    ] | None = None
    grid_result: tuple[int, int, int] | None = None
    rotation_result: tuple[int, int] | None = None
    storm_result: tuple[int, int, int, int | None] | None = None
    raw_pages: dict[str, RawPage] = {}
    pressure_results = {}
    capacity_results = {}
    dropdown_result = None
    for line in transition_text.splitlines():
        match = CAPACITY_RE.match(line)
        if match:
            capacity_results[match["name"]] = match.groupdict()
            continue
        match = DROPDOWN_RE.match(line)
        if match:
            dropdown_result = tuple(map(int, match.groups()))
            continue
        match = PRESSURE_RE.match(line)
        if match:
            pressure_results[match["name"]] = match.groupdict()
            continue
        match = DETAIL_RE.match(line)
        if match:
            values = match.groupdict()
            raw_pages[values["name"]] = RawPage(
                frames=int(values["frames"]),
                elapsed_us=int(values["elapsed"]),
                busy_us=int(values["busy"]),
                render_us=int(values["render"]),
                submit_us=int(values["submit"]),
                service_steps=int(values["steps"]),
                service_us=int(values["service"]),
                commands=int(values["commands"]),
            )
            continue
        match = ROTATION_RESULT_RE.match(line)
        if match:
            if int(match.group("errors")) != 0:
                raise ValueError(
                    f"{path}: benchmark workload reported command errors")
            rotation_result = (
                int(match.group("updates")), int(match.group("commands")))
            continue
        match = STORM_RESULT_RE.match(line)
        if match:
            storm_result = (
                int(match.group("updates")),
                int(match.group("commands")),
                int(match.group("rejected")),
                int(match.group("errors"))
                    if match.group("errors") is not None else None,
            )
            continue
        match = ERROR_COUNTER_RE.match(line) or WHEEL_ERROR_RE.match(line)
        if match and int(match.group("errors")) != 0:
            raise ValueError(
                f"{path}: benchmark workload reported command errors")
        match = TRANSITION_RE.match(line)
        if match:
            values = match.groupdict()
            transitions[values["name"]] = Transition(
                count=int(values["count"]),
                errors=int(values["errors"]),
                no_visual=int(values["no_visual"]),
                frames=int(values["frames"]),
                frames_avg=float(values["frames_avg"]),
                latency_avg_us=float(values["latency_avg"]),
                p50_us=int(values["p50"]),
                p95_us=int(values["p95"]),
                max_us=int(values["maximum"]),
                snapshot=int(values["snapshot"]),
                direct=int(values["direct"]),
                inplace=int(values["inplace"]),
                streamed=int(values["streamed"]),
                path_failures=int(values["path_failures"]),
                latency_samples=int(values["samples"])
                    if values["samples"] is not None else None,
            )
            continue
        match = DRAWER_RE.match(line)
        if match:
            drawer_result = (
                int(match.group("opens")),
                int(match.group("closes")),
                int(match.group("errors")),
            )
            continue
        match = KEYBOARD_RE.match(line)
        if match:
            keyboard_result = (
                int(match.group("presses")),
                int(match.group("completed"))
                    if match.group("completed") is not None else None,
                int(match.group("updates")),
                int(match.group("backspaces")),
                int(match.group("overflows"))
                    if match.group("overflows") is not None else None,
                int(match.group("mismatches"))
                    if match.group("mismatches") is not None else None,
                int(match.group("errors"))
                    if match.group("errors") is not None else None,
                int(match.group("capacity"))
                    if match.group("capacity") is not None else None,
                match.group("final"),
                match.group("expected"),
            )
            continue
        match = GRID_RE.match(line)
        if match:
            grid_result = (
                int(match.group("binds")),
                int(match.group("drags")),
                int(match.group("errors")),
            )
    run_mode = "full"
    for line in text.splitlines():
        match = RUN_RE.match(line)
        if match:
            run_mode = match.group("mode")
            if run_mode != "full":
                config_values["run_mode"] = run_mode
                if match.group("case") is not None:
                    config_values["run_case"] = match.group("case")
                if match.group("dwell") is not None:
                    config_values["run_dwell_ms"] = match.group("dwell")
            continue
        match = CONFIG_RE.match(line)
        if match:
            for item in match.group("values").split():
                if "=" in item:
                    key, value = item.split("=", 1)
                    config_values[key] = value
            continue
        match = RESOLVED_CONFIG_RE.search(line)
        if match:
            for item in match.group("values").split():
                if "=" in item:
                    key, value = item.split("=", 1)
                    config_values[f"resolved_{key}"] = value
    config = tuple(sorted(config_values.items())) if config_values else None
    for line in summary.splitlines():
        match = ROW_RE.match(line)
        if match:
            values = match.groupdict()
            pages[values["name"]] = Page(
                wall_fps=float(values["wall"]),
                busy_fps=float(values["busy"]),
                render_ms=float(values["render"]),
                submit_ms=float(values["submit"]),
                service_us=float(values["service"])
                    if values["service"] is not None else None,
                commands_s=float(values["commands"])
                    if values["commands"] is not None else None,
                seconds=int(values["seconds"]),
            )
            continue
        match = AGGREGATE_ACTIVE_RE.match(line) or \
            HISTORICAL_AGGREGATE_BUSY_RE.match(line)
        if match:
            active_fps = float(match.group("fps"))
            continue
        match = AGGREGATE_WALL_RE.match(line)
        if match:
            wall_fps = float(match.group("fps"))
            continue
        match = AGGREGATE_RAW_RE.match(line)
        if match:
            raw_frames = int(match.group("frames"))
            raw_wall_us = int(match.group("wall_us"))
            raw_busy_us = int(match.group("busy_us"))
            continue
        match = LEGACY_RE.match(line) or HISTORICAL_WEIGHTED_RE.match(line)
        if match:
            legacy_fps = float(match.group("fps"))

    if (active_fps is None and legacy_fps is None) or not pages:
        raise ValueError(f"{path}: incomplete measured summary")
    config_map = dict(config) if config is not None else {}
    # Up to protocol 7 a streamed transition was counted only as streamed;
    # from 8 on streamed is a delivery axis that overlaps the source routes,
    # so only those three account for the samples.
    try:
        protocol = int(config_map.get("protocol", 0))
    except ValueError:
        protocol = 0
    if protocol >= 18:
        manifest = Path(__file__).resolve().parents[1] / "main/bench_cases.inc"
        cases = dict(re.findall(
            r'^BENCH_CASE\((\w+),\s*\w+,\s*"([^"]+)"',
            manifest.read_text(encoding="utf-8"), re.MULTILINE))
        if protocol == 18:
            cases = {key: value for key, value in cases.items()
                     if not key.startswith("P_LOAD") and key != "P_DROPDOWN"}
        if run_mode == "full":
            expected_pages = set(cases.values())
        elif run_mode == "soak" and config_map.get("run_case") in cases:
            expected_pages = {cases[config_map["run_case"]]}
        else:
            raise ValueError(f"{path}: invalid benchmark case selection")
        if set(pages) != expected_pages:
            raise ValueError(f"{path}: incomplete or unexpected case coverage")
        if any(page.frames == 0 for page in raw_pages.values()):
            raise ValueError(f"{path}: case produced no rendered frames")
        if set(pressure_results) != set(pages):
            raise ValueError(f"{path}: missing per-case pressure evidence")
        if config_map.get("pressure") not in {"full", "native"}:
            raise ValueError(f"{path}: missing pressure configuration")
        for name, result in pressure_results.items():
            ticks, requests, accepted, rejected, errors = (
                int(result[key]) for key in
                ("ticks", "requests", "accepted", "rejected", "errors"))
            if ticks == 0 or errors or requests != accepted + rejected:
                raise ValueError(f"{path}: invalid pressure accounting on {name!r}")
            expected_mode = "transition" if name in transitions else config_map["pressure"]
            if result["mode"] != expected_mode or \
                    result["period"] != config_map.get("pressure_ms"):
                raise ValueError(f"{path}: pressure mode mismatch on {name!r}")
            if result["mode"] == "full":
                if requests != ticks or accepted == 0:
                    raise ValueError(f"{path}: missing repaint load on {name!r}")
            elif requests:
                raise ValueError(f"{path}: unexpected repaint load on {name!r}")
    if protocol >= 19:
        if "bench: measurement end" not in summary:
            raise ValueError(f"{path}: incomplete measurement trailer")
        if config_map.get("queue_metric") != "drained" or \
                config_map.get("hud") != "results-only" or \
                config_map.get("touch") not in {"disabled", "diagnostic"} or \
                not re.fullmatch(r"[0-9a-fA-F]{8}", config_map.get("bundle_crc", "")):
            raise ValueError(f"{path}: missing measurement isolation configuration")
        geometry = re.fullmatch(r"(\d+)x(\d+)", config_map.get("logical", ""))
        for count in (1, 8, 32, 64):
            name = f"blend load x{count}"
            if name in pages:
                result = capacity_results.get(name)
                if result is None or int(result["objects"]) != count or \
                        int(result["alpha"]) != 128 or \
                        int(result["w"]) == 0 or int(result["h"]) == 0:
                    raise ValueError(f"{path}: invalid capacity workload on {name!r}")
                if geometry is None:
                    raise ValueError(f"{path}: missing capacity display geometry")
                width, height = map(int, geometry.groups())
                expected_box = (width // 4, max(20, (height - max(50, height // 5) - 20) // 3))
                if (int(result["w"]), int(result["h"])) != expected_box:
                    raise ValueError(f"{path}: inconsistent capacity geometry on {name!r}")
        if "dropdown select" in pages and (dropdown_result is None or
                dropdown_result[0] == 0 or dropdown_result[1] != 0):
            raise ValueError(f"{path}: invalid dropdown interaction")
        if "drawer" in pages and drawer_result is None:
            raise ValueError(f"{path}: missing drawer interaction result")
        for prefix in ("bench: codecs ", "bench: dynamic image "):
            if not any(line.startswith(prefix) for line in summary.splitlines()):
                raise ValueError(f"{path}: missing media measurement trailer")
        for line in summary.splitlines():
            if line.startswith(("bench: codecs ", "bench: dynamic image ")):
                failure = re.search(r" failed=(\d+)", line)
                if failure is None or int(failure[1]) != 0:
                    raise ValueError(f"{path}: media workload failure")
        for name, result in transitions.items():
            if result.path_failures or (name in {"cross fade", "fade through black"}
                                       and result.no_visual):
                raise ValueError(f"{path}: non-rendered transition on {name!r}")
    if protocol >= 14 and "image rotation" in pages:
        if rotation_result is None:
            raise ValueError(f"{path}: missing image rotation workload result")
        updates, commands = rotation_result
        if updates == 0 or commands != updates * 2:
            raise ValueError(
                f"{path}: invalid image rotation workload result"
                f" (updates={updates}, commands={commands})")
    for name, transition in transitions.items():
        if protocol >= 18 and transition.latency_samples != min(transition.count, 16):
            raise ValueError(f"{path}: invalid transition latency window on {name!r}")
        if transition.count == 0 or transition.errors != 0:
            raise ValueError(
                f"{path}: invalid transition sample {name!r}"
                f" (count={transition.count}, errors={transition.errors})")
        selected = transition.snapshot + transition.direct + \
            transition.inplace + transition.path_failures
        if protocol < 8:
            selected += transition.streamed
        is_drag = name in {
            "drag commit", "drag cancel", "drag flick", "drag slide fade"
        }
        if not is_drag and selected != transition.count:
            raise ValueError(
                f"{path}: transition path count mismatch on {name!r}")
        if name in {"cross fade", "fade through black"}:
            # Cross-fade can degrade to a direct fade-through-black when
            # snapshots are unavailable. Only paths that still fail to render
            # are recorded as both a path failure and a no-visual sample.
            expected_no_visual = transition.path_failures
        elif name == "drag cancel":
            # A cancel may either animate back to the current scene or finish
            # immediately when the platform has no visual drag path.
            expected_no_visual = None
        else:
            expected_no_visual = 0
        if expected_no_visual is not None and \
                transition.no_visual != expected_no_visual:
            raise ValueError(
                f"{path}: unexpected no-visual count on {name!r}")
    if drawer_result is not None:
        valid_drawer = drawer_result == (3, 3, 0)
        if run_mode == "soak" or protocol >= 18:
            valid_drawer = drawer_result[0] > 0 and \
                drawer_result[0] == drawer_result[1] and \
                drawer_result[2] == 0
        if not valid_drawer:
            raise ValueError(
                f"{path}: invalid drawer cycle"
                f" (opens={drawer_result[0]}, closes={drawer_result[1]},"
                f" errors={drawer_result[2]})")
    if protocol >= 17 and "render storm" in pages:
        if storm_result is None:
            raise ValueError(f"{path}: missing render storm workload result")
        updates, commands, rejected, errors = storm_result
        if (protocol >= 18 and errors != 0) or errors or \
                updates == 0 or commands != updates * 9 or rejected >= commands:
            raise ValueError(
                f"{path}: invalid render storm workload result"
                f" (updates={updates}, commands={commands},"
                f" rejected={rejected})")
    if "composites" in pages:
        if keyboard_result is None:
            raise ValueError(f"{path}: missing keyboard interaction result")
        presses, completed, updates, backspaces, overflows, mismatches, \
            keyboard_errors, capacity, final_text, expected = keyboard_result
        if presses == 0 or updates == 0 or backspaces == 0 or not final_text:
            raise ValueError(
                f"{path}: invalid keyboard interaction"
                f" (presses={presses}, updates={updates},"
                f" backspaces={backspaces}, final={final_text!r})")
        if protocol >= 15:
            if None in {
                    completed, overflows, mismatches, keyboard_errors,
                    capacity, expected,
            }:
                raise ValueError(
                    f"{path}: incomplete keyboard correctness result")
            if completed != presses or updates != completed or \
                    overflows != 0 or mismatches != 0 or \
                    keyboard_errors != 0 or final_text != expected or \
                    capacity is None or len(final_text.encode()) > capacity:
                raise ValueError(
                    f"{path}: invalid keyboard correctness"
                    f" (presses={presses}, completed={completed},"
                    f" updates={updates}, overflows={overflows},"
                    f" mismatches={mismatches}, errors={keyboard_errors},"
                    f" capacity={capacity}, final={final_text!r},"
                    f" expected={expected!r})")
            if protocol >= 16 and (capacity <= 63 or
                                   len(final_text.encode()) <= 63):
                raise ValueError(
                    f"{path}: keyboard long-text path was not exercised"
                    f" (capacity={capacity}, bytes="
                    f"{len(final_text.encode())})")
    if "grid album" in pages:
        if grid_result is None:
            raise ValueError(f"{path}: missing grid interaction result")
        binds, drags, errors = grid_result
        if binds == 0 or drags == 0 or errors != 0:
            raise ValueError(
                f"{path}: invalid grid interaction"
                f" (binds={binds}, drags={drags}, errors={errors})")
    if protocol >= 9:
        if set(raw_pages) != set(pages):
            raise ValueError(
                f"{path}: per-page raw counter set does not match summary")
        for name, page in pages.items():
            raw = raw_pages[name]
            if raw.elapsed_us == 0 or raw.busy_us == 0 or raw.frames == 0:
                raise ValueError(f"{path}: zero raw counter on {name!r}")
            expected = (
                raw.frames * 1_000_000.0 / raw.elapsed_us,
                raw.frames * 1_000_000.0 / raw.busy_us,
                raw.render_us / 1000.0 / raw.frames,
                raw.submit_us / 1000.0 / raw.frames,
                raw.service_us / raw.service_steps
                    if raw.service_steps else 0.0,
                raw.commands * 1_000_000.0 / raw.elapsed_us,
            )
            reported = (
                page.wall_fps, page.busy_fps, page.render_ms,
                page.submit_ms, page.service_us or 0.0,
                page.commands_s or 0.0,
            )
            if any(abs(a - b) > 0.11 for a, b in zip(expected, reported)):
                raise ValueError(
                    f"{path}: summary row does not match raw data on {name!r}")
    if raw_frames is not None:
        if raw_wall_us == 0 or raw_busy_us == 0:
            raise ValueError(f"{path}: invalid zero aggregate raw time")
        raw_wall_fps = raw_frames * 1000000.0 / raw_wall_us
        raw_active_fps = raw_frames * 1000000.0 / raw_busy_us
        if wall_fps is None or abs(raw_wall_fps - wall_fps) > 0.11:
            raise ValueError(f"{path}: wall aggregate does not match raw data")
        if active_fps is None or abs(raw_active_fps - active_fps) > 0.11:
            raise ValueError(
                f"{path}: active aggregate does not match raw data")
        if protocol >= 9 and (
                sum(item.frames for item in raw_pages.values()) != raw_frames or
                sum(item.elapsed_us for item in raw_pages.values()) !=
                raw_wall_us or
                sum(item.busy_us for item in raw_pages.values()) !=
                raw_busy_us):
            raise ValueError(
                f"{path}: aggregate raw counters do not equal page totals")
    if active_fps is not None:
        return Run(versions[-1], active_fps, "aggregate_active_fps",
                   wall_fps, legacy_fps, pages, transitions, config, raw_frames,
                   raw_wall_us, raw_busy_us)
    return Run(versions[-1], legacy_fps, "legacy_weighted_busy_score",
               None, legacy_fps, pages, transitions, config, None, None, None)


def percent(current: float, baseline: float) -> float:
    return (current / baseline - 1.0) * 100.0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--pair",
        action="append",
        nargs=3,
        metavar=("DEVICE", "BASELINE_LOG", "CURRENT_LOG"),
        required=True,
        help="device label and the two IDF monitor logs; repeat per device",
    )
    parser.add_argument(
        "--details",
        action="store_true",
        help="also print per-page busy-FPS deltas",
    )
    args = parser.parse_args()

    print("| Device | Baseline | Current | Metric | Baseline | Current | Delta |"
          " Wall baseline | Wall current | Wall delta |")
    print("|---|---:|---:|---|---:|---:|---:|---:|---:|---:|")
    parsed: list[tuple[str, Run, Run]] = []
    for device, baseline_path, current_path in args.pair:
        baseline = parse_log(Path(baseline_path))
        current = parse_log(Path(current_path))
        if baseline.metric != current.metric:
            raise ValueError(
                f"{device}: metric mismatch ({baseline.metric} vs "
                f"{current.metric}); do not compare different statistics")
        if set(baseline.pages) != set(current.pages):
            raise ValueError(
                f"{device}: page set mismatch; do not compare"
                " different workloads")
        if set(baseline.transitions) != set(current.transitions):
            raise ValueError(
                f"{device}: transition set mismatch; do not compare"
                " different workloads")
        for name, old in baseline.pages.items():
            if old.seconds != current.pages[name].seconds:
                raise ValueError(
                    f"{device}: dwell mismatch on {name!r}; do not compare"
                    " different workloads")
        if baseline.config is not None and current.config is not None and \
                baseline.config != current.config:
            raise ValueError(
                f"{device}: benchmark configuration mismatch; do not"
                " compare different target/display configurations")
        parsed.append((device, baseline, current))
        delta = percent(current.score_fps, baseline.score_fps)
        if baseline.wall_fps is not None and current.wall_fps is not None:
            wall_delta = percent(current.wall_fps, baseline.wall_fps)
            wall_values = (
                f"{baseline.wall_fps:.1f} | {current.wall_fps:.1f} | "
                f"{wall_delta:+.1f}%")
        else:
            wall_values = "— | — | —"
        print(
            f"| {device} | `{baseline.version}` | `{current.version}` | "
            f"{baseline.metric} | {baseline.score_fps:.1f} | "
            f"{current.score_fps:.1f} | {delta:+.1f}% | {wall_values} |"
        )

    if args.details:
        for device, baseline, current in parsed:
            print(f"\n### {device}\n")
            print("| Page | Baseline busy FPS | Current busy FPS | Delta | "
                  "Baseline service us | Current service us | Delta |")
            print("|---|---:|---:|---:|---:|---:|---:|")
            for name, old in baseline.pages.items():
                new = current.pages.get(name)
                if new is None:
                    print(f"| {name} | {old.busy_fps:.1f} | missing | — | "
                          "— | — | — |")
                    continue
                delta = percent(new.busy_fps, old.busy_fps)
                if old.service_us is None or new.service_us is None:
                    service = "— | — | —"
                else:
                    service_delta = percent(new.service_us, old.service_us) \
                        if old.service_us != 0 else 0.0
                    service = (f"{old.service_us:.1f} | "
                               f"{new.service_us:.1f} | "
                               f"{service_delta:+.1f}%")
                print(
                    f"| {name} | {old.busy_fps:.1f} | {new.busy_fps:.1f} | "
                    f"{delta:+.1f}% | {service} |"
                )
            if baseline.transitions:
                print(f"\n#### {device} transition latency\n")
                print("| Transition | Count/errors | Baseline p95 us | "
                      "Current p95 us | Delta | Baseline frames | "
                      "Current frames | Current paths S/D/I/T/F |")
                print("|---|---:|---:|---:|---:|---:|---:|---:|")
                for name, old in baseline.transitions.items():
                    new = current.transitions[name]
                    latency_delta = percent(new.p95_us, old.p95_us) \
                        if old.p95_us != 0 else 0.0
                    print(
                        f"| {name} | {new.count}/{new.errors} | "
                        f"{old.p95_us} | {new.p95_us} | "
                        f"{latency_delta:+.1f}% | {old.frames_avg:.1f} | "
                        f"{new.frames_avg:.1f} | "
                        f"{new.snapshot}/{new.direct}/{new.inplace}/"
                        f"{new.streamed}/{new.path_failures} |"
                    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
