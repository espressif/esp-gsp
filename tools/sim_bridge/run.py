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
from build import add_build_arguments, build_project, executable, validate_backend_manifest
from rpc_client import connect
import scenario


def component_version(component: Path) -> str:
    text = (component / "idf_component.yml").read_text(encoding="utf-8")
    match = re.search(r'''^version:\s*["']?([0-9]+\.[0-9]+\.[0-9]+(?:[-+][0-9A-Za-z.+-]+)?)["']?\s*(?:#.*)?$''', text, re.MULTILINE)
    if not match:
        raise RuntimeError("Cannot read ESP-GSP version from idf_component.yml")
    return match.group(1)


def simulator_command(override: str | None, version: str | None, component: Path) -> list[str]:
    if override:
        return [executable(override, "sim")]
    if not version:
        version = component_version(component)
    if not re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+(?:[-+][0-9A-Za-z.+-]+)?", version):
        raise RuntimeError("Simulator version must be an exact version, e.g. 1.1.0")
    return [sys.executable, "-m", "gsp.execute", "--version", version, "sim"]


def dynamic_font_path(value: str | Path | None) -> Path | None:
    """Return a host-safe font path, failing before the simulator is started."""
    return font_asset_path(value, "Dynamic font")


def font_asset_path(value: str | Path | None, label: str) -> Path | None:
    if value is None:
        return None
    path = Path(value).expanduser()
    try:
        path = path.resolve(strict=True)
    except OSError as exc:
        raise RuntimeError(f"{label} not found: {path}") from exc
    if not path.is_file():
        raise RuntimeError(f"{label} is not a regular file: {path}")
    if path.stat().st_size == 0:
        raise RuntimeError(f"{label} file is empty: {path}")
    return path


def host_command(host: list[str], manifest: dict, args: argparse.Namespace, ready: Path,
                 *, loader_supported: bool = True) -> list[str]:
    """Build the sim_host command before it creates the UI/runtime instance."""
    validate_backend_manifest(manifest)
    command = [*host, "--bundle", manifest["bundle"], "--frames", "0",
               "--backend-required", "--ready-file", str(ready)]
    backend_port = getattr(args, "backend_port", None)
    socket_backend = getattr(args, "socket_backend", False) or backend_port is not None or not loader_supported
    if socket_backend:
        command += ["--backend-idle-timeout", "30"]
        if backend_port is None:
            command.append("--backend-enable")
        else:
            command += ["--backend-listen", f"tcp://127.0.0.1:{backend_port}"]
    else:
        library = Path(manifest["backend_library"]).resolve(strict=True)
        if not library.is_file():
            raise RuntimeError(f"Backend library is not a regular file: {library}")
        command += ["--backend-library", str(library)]
    for field, option, prefix in (("api_port", "--api-listen", "tcp://127.0.0.1:"),
                                  ("visual_port", "--visual-listen", "127.0.0.1:")):
        port = getattr(args, field, None)
        if port is not None:
            command += [option, f"{prefix}{port}"]
    if getattr(args, "input_mode", None) is not None:
        command += ["--input-mode", args.input_mode]
    if manifest.get("api_json"):
        sidecar = Path(manifest["api_json"]).resolve(strict=True)
        if not sidecar.is_file():
            raise RuntimeError(f"API sidecar is not a regular file: {sidecar}")
        command += ["--api-json", str(sidecar)]
    font = dynamic_font_path(args.dynamic_font)
    if font is not None:
        # sim_host reads this before load_with_config(), which mirrors applying
        # a device font-file configuration before esp_gsp_start().
        command += ["--dynamic-font", str(font)]
    catalog = font_asset_path(getattr(args, "font_catalog", None), "Font catalog")
    if catalog is not None:
        command += ["--font-catalog", str(catalog)]
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
        if not isinstance(info, dict):
            raise RuntimeError("Incompatible sim_host readiness metadata")
        socket_ready = (info.get("backend_mode", "socket") == "socket" and
                        isinstance(info.get("backend"), str) and
                        info["backend"].startswith("tcp://127."))
        library_ready = (info.get("backend_mode") == "library" and
                         info.get("dynamic_backend_abi_version") == 1 and
                         isinstance(info.get("backend_library"), str) and
                         bool(info["backend_library"]))
        if (info.get("version") != 1 or info.get("bridge_version") != 1 or
                type(info.get("pid")) is not int or info["pid"] <= 0 or
                (not managed and info["pid"] != proc.pid) or not (socket_ready or library_ready)):
            raise RuntimeError("Incompatible sim_host readiness metadata")
        # gsp.execute waits for a child simulator, so its PID is not the PID in
        # ready.json. On POSIX both must belong to the session we just created.
        if managed and not owns_simulator(proc.pid, info["pid"]):
            raise RuntimeError("Simulator readiness came from outside the owned process tree")
        return info
    raise RuntimeError("Timed out waiting for sim_host; use a release with --ready-file and bridge_version=1")


def start(command: list[str], **kwargs) -> subprocess.Popen:
    if os.name == "nt":
        return subprocess.Popen(command, creationflags=subprocess.CREATE_NEW_PROCESS_GROUP, **kwargs)
    return subprocess.Popen(command, start_new_session=True, **kwargs)


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


def probe_simulator(command: list[str], timeout: float) -> subprocess.CompletedProcess:
    proc = start(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    try:
        stdout, stderr = proc.communicate(timeout=timeout)
        return subprocess.CompletedProcess(command, proc.returncode, stdout, stderr)
    except subprocess.TimeoutExpired as exc:
        raise RuntimeError("Simulator preflight timed out; check tool installation and startup") from exc
    finally:
        stop(proc, tree=True)
        if proc.stdout:
            proc.stdout.close()
        if proc.stderr:
            proc.stderr.close()


def simulator_preflight(host: list[str], component: Path, timeout: float) -> dict:
    """Observe tool identity; equal versions do not prove API compatibility."""
    result = probe_simulator([*host, "--capabilities"], timeout)
    if result.returncode:
        raise RuntimeError(f"Simulator preflight failed ({result.returncode}): {result.stderr.strip()}")
    try:
        capabilities = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError("Simulator preflight did not return capability JSON") from exc
    if not isinstance(capabilities, dict) or not isinstance(capabilities.get("runtime_version"), str):
        raise RuntimeError("Simulator preflight is missing runtime_version")
    expected = component_version(component)
    if "dynamic_backend_loader_supported" in capabilities and not isinstance(capabilities["dynamic_backend_loader_supported"], bool):
        raise RuntimeError("Simulator returned an invalid shared-library loader capability")
    observed = capabilities["runtime_version"]
    if observed != expected:
        print(f"sim_bridge: component {expected}, simulator runtime {observed}; versions differ. "
              "Use matching tools before accepting application behavior.", file=sys.stderr)
    return {"component_version": expected, "simulator": capabilities,
            "version_match": observed == expected}


def port_number(value: str) -> int:
    try:
        port = int(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("port must be an integer from 0 to 65535") from exc
    if not 0 <= port <= 65535:
        raise argparse.ArgumentTypeError("port must be from 0 to 65535")
    return port


def validate_session(args: argparse.Namespace) -> None:
    ports = [getattr(args, name, None) for name in ("api_port", "backend_port", "visual_port")]
    fixed = [port for port in ports if port not in (None, 0)]
    if len(set(fixed)) != len(fixed):
        raise RuntimeError("API, backend and visual ports must be distinct (0 allocates a free port)")
    if args.headless and getattr(args, "visual_port", None) is not None:
        raise RuntimeError("--visual-port cannot be used with --headless")


def prepare_scenario(args: argparse.Namespace) -> None:
    if args.scenario is None:
        if args.report_dir is not None:
            raise ValueError("--report-dir requires --scenario")
        return
    args.scenario_plan = scenario.load(args.scenario)
    if args.duration or args.input_mode not in (None, "api-exclusive"):
        raise ValueError("--scenario requires API input ownership and cannot be combined with --duration")
    if args.report_dir is not None and args.report_dir.exists():
        raise ValueError("--report-dir must be a new directory; previous reports are preserved")
    args.api_port = 0 if args.api_port is None else args.api_port
    args.input_mode = "api-exclusive"
    args.headless = args.no_browser = True


def simulator_state(host: list[str], component: Path):
    """Identify this invocation's selection without retaining a tool cache."""
    try:
        status = Path(host[0]).stat()
        return (component.resolve(), component_version(component), tuple(host),
                status.st_dev, status.st_ino, status.st_size, status.st_mtime_ns, status.st_ctime_ns)
    except OSError:
        return None


def prepare_simulator(args: argparse.Namespace, component: Path, prepared=None):
    host = simulator_command(args.host, args.sim_version, component)
    state = simulator_state(host, component)
    if prepared is not None and state is not None and state == prepared[2]:
        contract = prepared[1]
    else:
        print("Checking simulator...", flush=True)
        contract = simulator_preflight(host, component, args.startup_timeout)
    plan = getattr(args, "scenario_plan", None)
    if plan is not None and not contract["version_match"]:
        raise RuntimeError("scenario acceptance requires matching component and simulator versions")
    return host, contract, state


def run_backend(args: argparse.Namespace, manifest: dict, *, prepared=None) -> int:
    validate_session(args)
    host, contract, _ = prepare_simulator(args, Path(manifest["component_root"]), prepared)
    plan = getattr(args, "scenario_plan", None)
    print("Tool contract: " + json.dumps(contract, sort_keys=True), flush=True)
    sim = backend = None
    info = {}
    with tempfile.TemporaryDirectory(prefix="gsp-sim-bridge-") as temporary:
        ready = Path(temporary) / "ready.json"
        loader_supported = contract["simulator"].get("dynamic_backend_loader_supported", True)
        if not loader_supported and not getattr(args, "socket_backend", False):
            print("sim_bridge: simulator uses Socket Backend because shared-library loading is unavailable", flush=True)
        command = host_command(host, manifest, args, ready, loader_supported=loader_supported)
        try:
            sim = start(command)
            info = wait_ready(sim, ready, args.startup_timeout, managed=not args.host)
            if info.get("backend_mode", "socket") == "socket":
                backend_command = [manifest["executable"], "--endpoint", info["backend"]]
                if args.duration:
                    backend_command += ["--duration-ms", str(max(1, round(args.duration * 1000)))]
                backend = start(backend_command)
            session = {"phase": "backend_spawned" if backend else "backend_loaded",
                       "listeners": info, "bundle": manifest["bundle"],
                       "api_json": manifest.get("api_json")}
            if manifest.get("context"):
                session["context"] = manifest["context"]
            if backend is not None:
                session["backend_pid"] = backend.pid
            print("Session: " + json.dumps(session, sort_keys=True), flush=True)
            if plan is not None:
                endpoint = info.get("api", "")
                match = re.fullmatch(r"tcp://127\.0\.0\.1:([0-9]+)", endpoint)
                if not match:
                    raise RuntimeError("scenario requires a loopback TCP API endpoint from sim_host")
                output = scenario.output_directory(manifest, args.report_dir)
                print(f"Scenario report: {output / 'report.json'}", flush=True)
                with connect(port=int(match[1])) as peer:
                    scenario.execute(peer, plan, output, manifest, contract)
                print(f"Scenario passed: {output / 'report.json'}", flush=True)
                return 0
            if info.get("api"):
                print(f"API: {info['api']}", flush=True)
            if info.get("browser"):
                print(f"Preview: {info['browser']}", flush=True)
                if not args.no_browser:
                    webbrowser.open(info["browser"])
            if backend is not None:
                while sim.poll() is None:
                    status = backend.poll()
                    if status is not None:
                        if status == 0:
                            return 0
                        # A clean simulator shutdown can close the Backend's
                        # transport before its process has finished exiting.
                        # Preserve an application failure if the host stays up.
                        try:
                            sim.wait(timeout=1)
                        except subprocess.TimeoutExpired:
                            return status
                        break
                    time.sleep(0.05)
                if sim.returncode == 0:
                    print("sim_bridge: simulator closed normally", flush=True)
                return sim.returncode
            deadline = time.monotonic() + args.duration if args.duration else None
            while sim.poll() is None and (deadline is None or time.monotonic() < deadline):
                time.sleep(0.05)
            return 0 if sim.poll() is None else sim.returncode
        finally:
            stop(backend, tree=True)
            stop(sim, tree=True, child_pid=info.get("pid"))


def add_run_arguments(parser: argparse.ArgumentParser) -> None:
    add_build_arguments(parser)
    parser.add_argument("--host", default=os.environ.get("GSP_SIM_EXECUTABLE") or
                        os.environ.get("GSP_SIM_HOST_EXECUTABLE"))
    parser.add_argument("--sim-version", default=os.environ.get("GSP_SIM_VERSION"),
                        help="Override simulator version; default is the linked ESP-GSP component version")
    parser.add_argument("--dynamic-font", default=os.environ.get("GSP_SIM_DYNAMIC_FONT") or None,
                        help="Optional missing-glyph fallback font with TrueType or CFF outlines "
                             "(.ttf, .otf, or first face of .ttc), loaded before UI startup; "
                             "see the host's --capabilities for compiled drivers")
    parser.add_argument("--font-catalog", default=os.environ.get("GSP_FONT_CATALOG") or None,
                        help="GSPB catalog from gspc font-link, loaded before UI startup")
    for channel in ("api", "backend", "visual"):
        parser.add_argument(f"--{channel}-port", type=port_number,
                            help=f"Loopback {channel} port; 0 allocates a free port" +
                            ("; selects Socket mode" if channel == "backend" else ""))
    parser.add_argument("--input-mode", choices=("shared", "api-exclusive", "browser-exclusive"),
                        help="Input ownership; omitted preserves the simulator default")
    parser.add_argument("--headless", action="store_true")
    parser.add_argument("--no-browser", action="store_true")
    parser.add_argument("--socket-backend", action="store_true",
                        help="Run the isolated executable Backend instead of the default shared library")
    parser.add_argument("--duration", type=float, default=0, help="Backend running time in seconds; 0 runs until interrupted")
    parser.add_argument("--startup-timeout", type=float, default=180,
                        help="Seconds for simulator download/startup (default: 180)")
    parser.add_argument("--scenario", type=Path,
                        help="Run a bounded headless JSON scenario, then exit; optional checks/check labels track acceptance; steps: " + ", ".join(scenario.ACTIONS))
    parser.add_argument("--report-dir", type=Path,
                        help="New directory for scenario report and PNGs; default: a unique directory in the build tree")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    add_run_arguments(parser)
    args = parser.parse_args()
    if args.duration < 0 or args.startup_timeout <= 0:
        parser.error("duration must be nonnegative and startup-timeout must be positive")
    try:
        # Do this before an often-expensive native build.  host_command()
        # repeats the normalization for callers that use run_backend() directly.
        prepare_scenario(args)
        validate_session(args)
        args.dynamic_font = dynamic_font_path(args.dynamic_font)
        args.font_catalog = font_asset_path(args.font_catalog, "Font catalog")
        prepared = None

        def before_build(manifest):
            nonlocal prepared
            prepared = prepare_simulator(args, Path(manifest["component_root"]))

        manifest = build_project(args, before_build=before_build)
        return run_backend(args, manifest, prepared=prepared)
    except KeyboardInterrupt:
        return 130
    except (RuntimeError, OSError, ValueError, subprocess.SubprocessError) as exc:
        parser.exit(1, f"sim_bridge: {exc}\n")


if __name__ == "__main__":
    raise SystemExit(main())
