#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Build a native C backend and its matching scene bundle (Python 3.10+)."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def executable(value: str | None, name: str) -> str:
    if value:
        found = shutil.which(value)
        if found:
            return str(Path(found).resolve())
        path = Path(value).resolve()
        if path.is_file():
            return str(path)
        raise RuntimeError(f"{name} not found: {value}")
    found = shutil.which(name)
    if found:
        return str(Path(found).resolve())
    raise RuntimeError(f"Executable not found: {name}")


def add_build_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--project", type=Path, required=True, help="CMake project containing gsp_add_backend")
    parser.add_argument("--build-dir", type=Path)
    parser.add_argument("--target", help="gsp_add_backend target; inferred when there is only one")
    parser.add_argument("--config", default="Release", help="CMake build configuration")
    parser.add_argument("--generator", help="CMake generator, e.g. Ninja or Visual Studio 17 2022")
    parser.add_argument("--gspc", default=os.environ.get("GSPC_EXECUTABLE"), help="Override GSPC executable; otherwise use esp-gsp-tools")
    parser.add_argument("--component-dir", type=Path, default=os.environ.get("ESP_GSP_COMPONENT_DIR"),
                        help="ESP-GSP component used by the PC project (default: this script's component)")


def build_project(args: argparse.Namespace) -> dict:
    project = args.project.resolve()
    if not (project / "CMakeLists.txt").is_file():
        raise RuntimeError(f"No CMakeLists.txt in {project}")
    # Keep generated output outside managed_components, even for bundled examples.
    build = (args.build_dir or Path.cwd() / "build" / "sim_bridge" / project.parent.name / project.name).resolve()
    cmake = executable(None, "cmake")
    component = Path(args.component_dir or ROOT).resolve()
    if not (component / "tools/sim_bridge/CMakeLists.txt").is_file():
        raise RuntimeError(f"ESP-GSP component does not contain sim_bridge: {component}")
    gspc = executable(args.gspc, "gspc") if args.gspc else ""
    build.mkdir(parents=True, exist_ok=True)
    # Prevent concurrent bundle generation and native builds in the same tree.
    # A crashed process leaves this marker: inspect before removing it manually.
    lock = build / ".sim_bridge_build.lock"
    try:
        fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600)
    except FileExistsError as exc:
        raise RuntimeError(f"Build directory is locked: {lock}; use another --build-dir or inspect a stale lock") from exc
    try:
        with os.fdopen(fd, "w") as stream:
            stream.write(str(os.getpid()))
        command = [cmake, "-S", str(project), "-B", str(build),
                   f"-DCMAKE_BUILD_TYPE={args.config}", f"-DGSPC_EXECUTABLE={gspc}",
                   f"-DESP_GSP_COMPONENT_DIR={component.as_posix()}",
                   f"-DESP_GSP_PYTHON_EXECUTABLE={Path(sys.executable).as_posix()}",
                   f"-DPROJECT_DIR={Path.cwd().as_posix()}"]
        if args.generator:
            command += ["-G", args.generator]
        subprocess.run(command, check=True)
        pattern = f"{args.target or '*'}-{args.config}.json"
        manifests = list(build.glob(pattern))
        if len(manifests) != 1:
            raise RuntimeError(f"Expected one backend manifest matching {pattern}; specify --target")
        manifest = json.loads(manifests[0].read_text(encoding="utf-8"))
        if manifest.get("version") != 1:
            raise RuntimeError("Unsupported backend manifest")
        if Path(manifest.get("component_root", "")).resolve() != component:
            raise RuntimeError("PC project uses a different component; honor ESP_GSP_COMPONENT_DIR in its CMakeLists.txt")
        target = manifests[0].name.removesuffix(f"-{args.config}.json")
        subprocess.run([cmake, "--build", str(build), "--config", args.config,
                        "--target", target, "--parallel"], check=True)
        for key in ("executable", "bundle"):
            if not Path(manifest[key]).is_file():
                raise RuntimeError(f"Build did not produce {key}: {manifest[key]}")
        return manifest
    finally:
        lock.unlink()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    add_build_arguments(parser)
    args = parser.parse_args()
    try:
        print(json.dumps(build_project(args), indent=2))
        return 0
    except (RuntimeError, OSError, ValueError, subprocess.CalledProcessError) as exc:
        parser.exit(1, f"sim_bridge: {exc}\n")


if __name__ == "__main__":
    raise SystemExit(main())
