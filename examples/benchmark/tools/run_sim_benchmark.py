#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Build, pack and automatically run benchmark scenes through gsp_sim."""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]
SCENE_DIR = ROOT / "examples" / "benchmark" / "scenes"
RUN_SCENE = ROOT / "tools" / "sim" / "run_scene.sh"
CASE_MANIFEST = ROOT / "examples" / "benchmark" / "main" / "bench_cases.inc"
ALL_SIZES = (240, 320, 360, 480, 800, 1024)
SUMMARY_RE = re.compile(
    r"gsp_sim: summary loops=(\d+) presented=(\d+) "
    r"elapsed_us=(\d+) fps=([0-9.]+)"
)


def load_page_binds() -> tuple[str, ...]:
    pattern = re.compile(
        r"^BENCH_CASE\(\s*(P_[A-Z0-9_]+)\s*,\s*"
        r"GSP_BIND_(P_[A-Z0-9_]+)\s*,"
    )
    binds = []
    for line in CASE_MANIFEST.read_text(encoding="utf-8").splitlines():
        match = pattern.match(line)
        if match is None:
            continue
        case_id, bind = match.groups()
        if case_id != bind:
            raise RuntimeError(
                f"{CASE_MANIFEST}: case {case_id} uses mismatched bind {bind}"
            )
        binds.append(bind)
    if not binds or len(binds) != len(set(binds)):
        raise RuntimeError(f"{CASE_MANIFEST}: invalid or duplicate scene cases")
    return tuple(binds)


PAGE_BINDS = load_page_binds()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument("--size", type=int, choices=ALL_SIZES, default=320)
    selection.add_argument("--all", action="store_true")
    parser.add_argument("--frames", type=int)
    parser.add_argument("--gallery", action="store_true",
                        help="save the final frame of every authored page in one run")
    parser.add_argument("--state", choices=("default", "keyboard", "modal", "dropdown"), default="default",
                        help="inspect an open dropdown or a composites tab")
    parser.add_argument("--rgb888", action="store_true",
                        help="preview the native RGB888 scene (800 or 1024 only)")
    parser.add_argument(
        "--case", choices=(*PAGE_BINDS, "RESULTS_OVERLAY"),
        help="render only one stable scene case ID (for example P_STORM)",
    )
    parser.add_argument(
        "--page-frames", type=int, default=60,
        help="frames shown per benchmark page (default: 60)",
    )
    parser.add_argument("--timeout", type=float, default=180.0)
    parser.add_argument(
        "--window",
        action="store_true",
        help="show the SDL window while retaining automatic validation",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=ROOT / "build" / "gsp-sim-benchmark",
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=ROOT / "build" / "gsp-sim",
    )
    return parser.parse_args()


def run_case(size: int, args: argparse.Namespace) -> tuple[int, float]:
    scene = SCENE_DIR / f"bench_{'rgb888_' if args.rgb888 else ''}{size}.json"
    alt_scene = SCENE_DIR / f"bench_alt_{size}.json"
    case_dir = args.output_dir.resolve() / str(size)
    case_dir.mkdir(parents=True, exist_ok=True)
    log_path = case_dir / "run.log"
    ppm_path = case_dir / "final.ppm"
    env = os.environ.copy()
    env["GSP_SIM_BUILD_DIR"] = str(args.build_dir.resolve())
    env["GSP_SIM_FRAMES"] = str(args.frames)
    # Hide every other page even when selecting only one: P_RECT1 is visible
    # in the authored startup state.
    binds = (args.case,) + tuple(bind for bind in PAGE_BINDS if bind != args.case) \
        if args.case else args.page_binds
    env["GSP_SIM_CYCLE_BINDS"] = ",".join(binds)
    env["GSP_SIM_CYCLE_FRAMES"] = str(args.frames if args.case else args.page_frames)
    env["GSP_BENCH_RESULTS_PREVIEW"] = "1" if args.case == "RESULTS_OVERLAY" else "0"
    env["GSP_BENCH_VISUAL_PREVIEW"] = "1"
    env["GSP_BENCH_PREVIEW_STATE"] = args.state
    if args.rgb888:
        env["GSP_SIM_PROFILE"] = str(ROOT / "config/profiles/host_rgb888.yaml")
    env["GSP_SIM_APPLICATION_SOURCE"] = ";".join((
        str(ROOT / "examples" / "benchmark" / "tools" /
            "benchmark_sim_workload.c"),
        str(ROOT / "examples" / "benchmark" / "main" /
            "bench_workload.c"),
    ))
    env["GSP_SIM_APPLICATION_INCLUDE_DIR"] = ";".join((
        str(ROOT / "examples" / "benchmark" / "main"),
        str(ROOT / "examples" / "benchmark" / "tools"),
    ))

    command = [
        str(RUN_SCENE),
        str(scene),
        str(alt_scene),
        "--",
        "--invalidate-every-frame",
        "--dump",
        str(ppm_path),
    ]
    if not args.window:
        command.insert(4, "--headless")
    if args.gallery:
        command.extend(("--dump-cycle", str(case_dir / "page")))
    if args.state == "dropdown":
        height = {240: 240, 320: 240, 360: 360, 480: 800, 800: 480, 1024: 600}[size]
        command.extend(("--tap", str(size // 2), str(height // 4 + max(26, height // 12) // 2)))
    print(f"[sim-benchmark] {size}: {args.frames} frames", flush=True)
    try:
        completed = subprocess.run(
            command,
            cwd=ROOT,
            env=env,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            timeout=args.timeout,
            check=False,
        )
    except subprocess.TimeoutExpired as error:
        output = error.stdout or ""
        if isinstance(output, bytes):
            output = output.decode(errors="replace")
        log_path.write_text(output, encoding="utf-8")
        raise RuntimeError(
            f"{size}: timed out after {args.timeout:g}s; see {log_path}"
        ) from error

    log_path.write_text(completed.stdout, encoding="utf-8")
    if completed.returncode != 0:
        raise RuntimeError(
            f"{size}: simulator exited {completed.returncode}; see {log_path}"
        )
    if "benchmark tween workload enabled" not in completed.stdout:
        raise RuntimeError(
            f"{size}: benchmark tween workload was not enabled; "
            f"see {log_path}"
        )
    matches = list(SUMMARY_RE.finditer(completed.stdout))
    if not matches:
        raise RuntimeError(f"{size}: summary missing; see {log_path}")
    loops, presented, _elapsed_us, fps = matches[-1].groups()
    if int(loops) != args.frames or int(presented) != args.frames:
        raise RuntimeError(
            f"{size}: requested {args.frames}, loops={loops}, "
            f"presented={presented}; see {log_path}"
        )
    if not ppm_path.is_file() or ppm_path.stat().st_size == 0:
        raise RuntimeError(f"{size}: final frame missing: {ppm_path}")
    print(
        f"[sim-benchmark] {size}: PASS, {float(fps):.2f} fps, "
        f"log={log_path}",
        flush=True,
    )
    if args.gallery:
        for index, bind in enumerate(args.page_binds):
            source = case_dir / f"page-{index:03d}.ppm"
            if not source.is_file():
                raise RuntimeError(f"{size}: missing gallery frame for {bind}")
            source.replace(case_dir / f"{bind}.ppm")
    return size, float(fps)


def main() -> int:
    args = parse_args()
    args.page_binds = (args.case,) if args.case else PAGE_BINDS
    if args.frames is None:
        args.frames = len(args.page_binds) * args.page_frames
    if args.frames <= 0 or args.page_frames <= 0 or args.timeout <= 0:
        print("frames, page-frames and timeout must be positive", file=sys.stderr)
        return 2
    sizes = ALL_SIZES if args.all else (args.size,)
    if args.rgb888 and any(size not in (800, 1024) for size in sizes):
        print("RGB888 preview supports --size 800 or --size 1024", file=sys.stderr)
        return 2
    state_case = "P_DROPDOWN" if args.state == "dropdown" else "P_COMPOSITES"
    if args.state != "default" and args.case != state_case:
        print(f"--state {args.state} requires --case {state_case}", file=sys.stderr)
        return 2
    results: list[tuple[int, float]] = []
    try:
        for size in sizes:
            results.append(run_case(size, args))
    except RuntimeError as error:
        print(f"[sim-benchmark] FAIL: {error}", file=sys.stderr)
        return 1

    print("[sim-benchmark] all cases passed")
    for size, fps in results:
        print(f"  {size}: {fps:.2f} fps")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
