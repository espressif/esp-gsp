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
ALL_SIZES = (240, 320, 360, 480, 800, 1024)
SUMMARY_RE = re.compile(
    r"gsp_sim: summary loops=(\d+) presented=(\d+) "
    r"elapsed_us=(\d+) fps=([0-9.]+)"
)
PAGE_BINDS = (
    "P_RECT1", "P_RECTS", "P_CARDS", "P_SHAPES", "P_CLOCK", "P_GRAD",
    "P_OPA", "P_OPAL", "P_OVER", "P_TEXT", "P_BIGTEXT", "P_SCROLL",
    "P_ARCS", "P_IMGRGB", "P_IMGARGB", "P_QOI", "P_SCALE", "P_STREAM",
    "P_WALL", "P_STATIC_MOVE", "P_MOVE", "P_WIDGETS", "P_FLOW", "P_STACK",
    "P_COMPOSITES", "P_DRAWER", "P_WHEEL", "P_ANIM", "P_MOTION",
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument("--size", type=int, choices=ALL_SIZES, default=320)
    selection.add_argument("--all", action="store_true")
    parser.add_argument("--frames", type=int, default=len(PAGE_BINDS) * 60)
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
    scene = SCENE_DIR / f"bench_{size}.json"
    alt_scene = SCENE_DIR / f"bench_alt_{size}.json"
    case_dir = args.output_dir.resolve() / str(size)
    case_dir.mkdir(parents=True, exist_ok=True)
    log_path = case_dir / "run.log"
    ppm_path = case_dir / "final.ppm"
    env = os.environ.copy()
    env["GSP_SIM_BUILD_DIR"] = str(args.build_dir.resolve())
    env["GSP_SIM_FRAMES"] = str(args.frames)
    env["GSP_SIM_CYCLE_BINDS"] = ",".join(PAGE_BINDS)
    env["GSP_SIM_CYCLE_FRAMES"] = str(args.page_frames)
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
    return size, float(fps)


def main() -> int:
    args = parse_args()
    if args.frames <= 0 or args.page_frames <= 0 or args.timeout <= 0:
        print("frames, page-frames and timeout must be positive", file=sys.stderr)
        return 2
    sizes = ALL_SIZES if args.all else (args.size,)
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
