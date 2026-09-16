#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Build and run the native backend with sim_host; stop both on exit."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import sys
import tempfile
import time
import webbrowser

# Importing the sibling helper must not add cache files to managed_components.
sys.dont_write_bytecode = True
from build import add_build_arguments, build_project, executable


def simulator_command(override: str | None, version: str | None, component: Path) -> list[str]:
    if override:
        return [executable(override, "sim")]
    if not version:
        text = (component / "idf_component.yml").read_text(encoding="utf-8")
        match = re.search(r'''^version:\s*["']?([0-9]+\.[0-9]+\.[0-9]+(?:[-+][0-9A-Za-z.+-]+)?)["']?\s*(?:#.*)?$''', text, re.MULTILINE)
        if not match:
            raise RuntimeError("Cannot read ESP-GSP version from idf_component.yml; use --sim-version or --host")
        version = match.group(1)
    if not re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+(?:[-+][0-9A-Za-z.+-]+)?", version):
        raise RuntimeError("Simulator version must be an exact version, e.g. 1.1.0")
    return [sys.executable, "-m", "gsp.execute", "--version", version, "sim"]


def dynamic_font_path(value: str | Path | None) -> Path | None:
    """Return a host-safe font path, failing before the simulator is started."""
    if value is None:
        return None
    path = Path(value).expanduser()
    try:
        path = path.resolve(strict=True)
    except OSError as exc:
        raise RuntimeError(f"Dynamic font not found: {path}") from exc
    if not path.is_file():
        raise RuntimeError(f"Dynamic font is not a regular file: {path}")
    return path


def host_command(host: list[str], manifest: dict, args: argparse.Namespace, ready: Path) -> list[str]:
    """Build the sim_host command before it creates the UI/runtime instance."""
    command = [*host, "--bundle", manifest["bundle"], "--frames", "0",
               "--backend-enable", "--backend-required",
               "--backend-idle-timeout", "30", "--ready-file", str(ready)]
    font = dynamic_font_path(args.dynamic_font)
    if font is not None:
        # sim_host reads this before load_with_config(), which mirrors applying
        # a device font-file configuration before esp_gsp_start().
        command += ["--dynamic-font", str(font)]
    if args.headless:
        command.append("--headless")
    return command


def owns_simulator(parent_pid: int, pid: int) -> bool:
    if os.name != "nt":
        return os.getpgid(pid) == parent_pid
    # Use the OS process tree rather than trusting an arbitrary PID from JSON.
    # No extra Python package is needed on Windows.
    import ctypes
    from ctypes import wintypes

    class ProcessEntry(ctypes.Structure):
        _fields_ = [("size", wintypes.DWORD), ("usage", wintypes.DWORD),
                    ("pid", wintypes.DWORD), ("heap", ctypes.c_size_t),
                    ("module", wintypes.DWORD), ("threads", wintypes.DWORD),
                    ("parent", wintypes.DWORD), ("priority", wintypes.LONG),
                    ("flags", wintypes.DWORD), ("exe", wintypes.WCHAR * 260)]

    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel.CreateToolhelp32Snapshot.argtypes = [wintypes.DWORD, wintypes.DWORD]
    kernel.CreateToolhelp32Snapshot.restype = wintypes.HANDLE
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    kernel.CloseHandle.restype = wintypes.BOOL
    for name in ("Process32FirstW", "Process32NextW"):
        function = getattr(kernel, name)
        function.argtypes = [wintypes.HANDLE, ctypes.POINTER(ProcessEntry)]
        function.restype = wintypes.BOOL
    snapshot = kernel.CreateToolhelp32Snapshot(2, 0)  # TH32CS_SNAPPROCESS
    if snapshot == ctypes.c_void_p(-1).value:
        raise ctypes.WinError(ctypes.get_last_error())
    parents = {}
    try:
        entry = ProcessEntry()
        entry.size = ctypes.sizeof(entry)
        present = kernel.Process32FirstW(snapshot, ctypes.byref(entry))
        while present:
            parents[entry.pid] = entry.parent
            present = kernel.Process32NextW(snapshot, ctypes.byref(entry))
    finally:
        kernel.CloseHandle(snapshot)
    visited = set()
    while pid and pid not in visited:
        if pid == parent_pid:
            return True
        visited.add(pid)
        pid = parents.get(pid, 0)
    return False


def wait_ready(proc: subprocess.Popen, path: Path, timeout: float, *, managed: bool = False) -> dict:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if proc.poll() is not None:
            raise RuntimeError(f"sim_host exited before becoming ready ({proc.returncode})")
        try:
            info = json.loads(path.read_text(encoding="utf-8"))
        except (FileNotFoundError, json.JSONDecodeError):
            time.sleep(0.05)
            continue
        if (not isinstance(info, dict) or info.get("version") != 1 or info.get("bridge_version") != 1 or
                type(info.get("pid")) is not int or info["pid"] <= 0 or
                (not managed and info["pid"] != proc.pid) or not isinstance(info.get("backend"), str) or
                not info["backend"].startswith("tcp://127.")):
            raise RuntimeError("Incompatible sim_host readiness metadata")
        # gsp.execute waits for a child simulator, so its PID is not the PID in
        # ready.json. On POSIX both must belong to the session we just created.
        if managed and not owns_simulator(proc.pid, info["pid"]):
            raise RuntimeError("Simulator readiness came from outside the owned process tree")
        return info
    raise RuntimeError("Timed out waiting for sim_host; use a release with --ready-file and bridge_version=1")


def start(command: list[str]) -> subprocess.Popen:
    if os.name == "nt":
        return subprocess.Popen(command, creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)
    return subprocess.Popen(command, start_new_session=True)


def stop(proc: subprocess.Popen | None, *, tree: bool = False, child_pid: int | None = None) -> None:
    if proc is None:
        return
    if tree and os.name == "nt":
        # Stop the manager and its child together, not just the Python wrapper.
        # A successfully read private readiness file also identifies our host
        # if the manager unexpectedly exited first.
        pid = proc.pid if proc.poll() is None else child_pid
        if pid:
            subprocess.run(["taskkill", "/PID", str(pid), "/T", "/F"],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
    elif tree:
        try:
            os.killpg(proc.pid, signal.SIGTERM)
        except ProcessLookupError:
            pass
    elif proc.poll() is None:
        proc.terminate()
    try:
        proc.wait(timeout=6)
    except subprocess.TimeoutExpired:
        if tree and os.name != "nt":
            os.killpg(proc.pid, signal.SIGKILL)
        else:
            proc.kill()
        proc.wait()
    if tree and os.name != "nt":
        # A wrapper may exit before a child that ignores SIGTERM. Do not leak
        # that process merely because Popen.wait() only waited for the wrapper.
        try:
            os.killpg(proc.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass


def run_backend(args: argparse.Namespace, manifest: dict) -> int:
    host = simulator_command(args.host, args.sim_version, Path(manifest["component_root"]))
    sim = backend = None
    info = {}
    with tempfile.TemporaryDirectory(prefix="gsp-sim-bridge-") as temporary:
        ready = Path(temporary) / "ready.json"
        command = host_command(host, manifest, args, ready)
        try:
            sim = start(command)
            info = wait_ready(sim, ready, args.startup_timeout, managed=not args.host)
            backend_command = [manifest["executable"], "--endpoint", info["backend"]]
            if args.duration:
                backend_command += ["--duration-ms", str(max(1, round(args.duration * 1000)))]
            backend = start(backend_command)
            if info.get("browser"):
                print(f"Preview: {info['browser']}", flush=True)
                if not args.no_browser:
                    webbrowser.open(info["browser"])
            while backend.poll() is None:
                if sim.poll() is not None:
                    raise RuntimeError(f"sim_host exited while the backend was running ({sim.returncode})")
                time.sleep(0.05)
            return backend.returncode
        finally:
            stop(backend, tree=True)
            stop(sim, tree=True, child_pid=info.get("pid"))


def add_run_arguments(parser: argparse.ArgumentParser) -> None:
    add_build_arguments(parser)
    parser.add_argument("--host", default=os.environ.get("GSP_SIM_EXECUTABLE") or
                        os.environ.get("GSP_SIM_HOST_EXECUTABLE"))
    parser.add_argument("--sim-version", default=os.environ.get("GSP_SIM_VERSION"),
                        help="Override simulator version; default is the linked ESP-GSP component version")
    parser.add_argument("--dynamic-font", default=os.environ.get("GSP_SIM_DYNAMIC_FONT"),
                        help="TrueType/OpenType font loaded by sim_host before UI startup")
    parser.add_argument("--headless", action="store_true")
    parser.add_argument("--no-browser", action="store_true")
    parser.add_argument("--duration", type=float, default=0, help="Backend running time in seconds; 0 runs until interrupted")
    parser.add_argument("--startup-timeout", type=float, default=180,
                        help="Seconds for simulator download/startup (default: 180)")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    add_run_arguments(parser)
    args = parser.parse_args()
    if args.duration < 0 or args.startup_timeout <= 0:
        parser.error("duration must be nonnegative and startup-timeout must be positive")
    try:
        # Do this before an often-expensive native build.  host_command()
        # repeats the normalization for callers that use run_backend() directly.
        args.dynamic_font = dynamic_font_path(args.dynamic_font)
        return run_backend(args, build_project(args))
    except KeyboardInterrupt:
        return 130
    except (RuntimeError, OSError, ValueError, subprocess.CalledProcessError) as exc:
        parser.exit(1, f"sim_bridge: {exc}\n")


if __name__ == "__main__":
    raise SystemExit(main())
