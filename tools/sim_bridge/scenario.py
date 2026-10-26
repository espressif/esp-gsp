# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Bounded input and observation scenarios for the existing native runner."""
from __future__ import annotations

import hashlib
import json
import math
import os
from pathlib import Path
import queue
import tempfile
import time

ACTIONS = {
    "tap": (set(), {"name", "x", "y", "callbacks", "max_frames"}),
    "drag": ({"x1", "y1", "x2", "y2"}, set()),
    "expect_property": ({"name", "property", "value"}, set()),
    "wait_property": ({"name", "property", "value"}, {"max_frames"}),
    "expect_visible": ({"name", "visible"}, set()),
    "wait_visible": ({"name", "visible"}, {"max_frames"}),
    "wait_keyboard_text": ({"text"}, {"max_frames"}),
    "wait_component": ({"name"}, {"value", "max_frames", "require_new_event"}),
    "wait_scene": ({"scene"}, {"max_frames"}),
    "wait_callback": ({"name"}, set()),
    "screenshot": ({"file"}, {"reference", "allow_changes"}),
}


def load(path: Path) -> dict:
    plan = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(plan, dict) or set(plan) - {"schema_version", "timeout_seconds", "steps", "checks"}:
        raise ValueError("scenario requires schema_version, steps and optional timeout_seconds/checks")
    if type(plan.get("schema_version")) is not int or plan["schema_version"] != 1:
        raise ValueError("scenario schema_version must be 1")
    timeout = plan.get("timeout_seconds", 30)
    if type(timeout) not in (int, float) or not math.isfinite(timeout) or not 0 < timeout <= 300:
        raise ValueError("scenario timeout_seconds must be in (0, 300]")
    steps = plan.get("steps")
    if not isinstance(steps, list) or not 1 <= len(steps) <= 64:
        raise ValueError("scenario requires 1..64 steps")
    checks = plan.get("checks", [])
    if "checks" in plan and (not isinstance(checks, list) or not 1 <= len(checks) <= 64 or
                             any(not isinstance(name, str) or not name.strip() or len(name) > 160
                                 for name in checks) or len(set(checks)) != len(checks)):
        raise ValueError("scenario checks requires 1..64 unique nonempty names of at most 160 characters")
    screenshots = set()
    has_input = False
    for index, step in enumerate(steps, 1):
        if not isinstance(step, dict) or not isinstance(step.get("action"), str) or step["action"] not in ACTIONS:
            raise ValueError(f"scenario step {index}: unsupported action")
        required, optional = ACTIONS[step["action"]]
        if required - set(step) or set(step) - required - optional - {"action", "check"}:
            raise ValueError(f"scenario step {index}: expected fields {sorted(required)}; optional {sorted(optional)}")
        if "check" in step:
            if not isinstance(step["check"], str) or step["check"] not in checks:
                raise ValueError(f"scenario step {index}: check must name a declared scenario check")
            if (step["action"] == "drag" or step["action"] == "tap" and "callbacks" not in step or
                    step["action"] == "screenshot" and "reference" not in step):
                raise ValueError(f"scenario step {index}: check requires an assertion, not input or capture alone")
        for key in ("name", "property"):
            if key in step and (not isinstance(step[key], str) or not step[key].strip()):
                raise ValueError(f"scenario step {index}: {key} must be a nonempty string")
        if "value" in step and (type(step["value"]) not in (int, bool) or
                                not -(2**31) <= step["value"] <= 2**32 - 1):
            raise ValueError(f"scenario step {index}: value must be a boolean or 32-bit integer")
        if "visible" in step and type(step["visible"]) is not bool:
            raise ValueError(f"scenario step {index}: visible must be a boolean")
        if "require_new_event" in step and type(step["require_new_event"]) is not bool:
            raise ValueError(f"scenario step {index}: require_new_event must be a boolean")
        if step.get("require_new_event") and not has_input:
            raise ValueError(f"scenario step {index}: require_new_event needs a preceding tap or drag")
        has_input |= step["action"] in ("tap", "drag")
        if "max_frames" in step and (type(step["max_frames"]) is not int or not 1 <= step["max_frames"] <= 36000):
            raise ValueError(f"scenario step {index}: max_frames must be in 1..36000")
        if step["action"] == "wait_keyboard_text":
            text = step["text"]
            if not isinstance(text, str) or "\0" in text:
                raise ValueError(f"scenario step {index}: text must be a string without NUL")
            try:
                text.encode("utf-8")
            except UnicodeEncodeError as exc:
                raise ValueError(f"scenario step {index}: text must be valid UTF-8") from exc
        if "scene" in step and (type(step["scene"]) is not int or not 0 <= step["scene"] <= 65535):
            raise ValueError(f"scenario step {index}: scene must be an integer in 0..65535")
        if step["action"] == "wait_component" and "value" in step and (
                type(step["value"]) is not int or not 0 <= step["value"] <= 65535):
            raise ValueError(f"scenario step {index}: component value must be an integer in 0..65535")
        if step["action"] == "tap":
            named = "name" in step
            coordinates = "x" in step or "y" in step
            if named == coordinates or coordinates and not {"x", "y"} <= set(step):
                raise ValueError(f"scenario step {index}: tap requires name or x and y")
            if coordinates and any(type(step[key]) is not int or not -(2**31) <= step[key] < 2**31 for key in ("x", "y")):
                raise ValueError(f"scenario step {index}: tap coordinates must be 32-bit integers")
            if "max_frames" in step and "callbacks" not in step:
                raise ValueError(f"scenario step {index}: tap max_frames requires callbacks")
            if "callbacks" in step:
                expected = step["callbacks"]
                if not isinstance(expected, list) or len(expected) > 64:
                    raise ValueError(f"scenario step {index}: callbacks requires a list of at most 64 expectations")
                for callback in expected:
                    if isinstance(callback, str) and callback.strip():
                        continue
                    if (not isinstance(callback, dict) or
                            not isinstance(callback.get("callback"), str) or not callback["callback"].strip() or
                            set(callback) - {"callback", "arg", "scene_id", "list", "item"} or
                            any(type(value) is not int or not 0 <= value <= 2**32 - 1
                                for key, value in callback.items() if key != "callback")):
                        raise ValueError(f"scenario step {index}: callback expectations require names or typed callback event fields")
        if step["action"] == "drag":
            if any(type(step[k]) is not int or not -(2**31) <= step[k] < 2**31 for k in required):
                raise ValueError(f"scenario step {index}: drag coordinates must be 32-bit integers")
        if step["action"] == "screenshot":
            filename = step["file"]
            if (not isinstance(filename, str) or not filename.endswith(".png") or
                    "/" in filename or "\\" in filename or ":" in filename or
                    filename in screenshots or filename in (".png", "..png")):
                raise ValueError(f"scenario step {index}: file must be a unique PNG filename without directories")
            screenshots.add(filename)
            if "allow_changes" in step and "reference" not in step:
                raise ValueError(f"scenario step {index}: allow_changes requires a reference PNG")
            if "reference" in step:
                reference = step["reference"]
                if not isinstance(reference, str) or not reference.strip():
                    raise ValueError(f"scenario step {index}: reference must name an existing PNG")
                reference = (path.resolve().parent / Path(reference).expanduser()).resolve()
                if not reference.is_file():
                    raise ValueError(f"scenario step {index}: reference PNG does not exist: {reference}")
                with reference.open("rb") as stream:
                    if stream.read(8) != b"\x89PNG\r\n\x1a\n":
                        raise ValueError(f"scenario step {index}: reference is not a PNG")
                step["reference"] = str(reference)
                regions = step.get("allow_changes", [])
                if (not isinstance(regions, list) or len(regions) > 32 or
                        any(not isinstance(region, dict) or set(region) != {"x", "y", "w", "h"} or
                            any(type(value) is not int or not (1 if key in ("w", "h") else 0) <= value <= 2**32 - 1
                                for key, value in region.items()) for region in regions)):
                    raise ValueError(f"scenario step {index}: allow_changes requires up to 32 nonempty integer x/y/w/h regions")
                diff_name = filename[:-4] + ".diff.png"
                if diff_name in screenshots:
                    raise ValueError(f"scenario step {index}: diff filename conflicts with another screenshot")
                screenshots.add(diff_name)
    return plan


def check_coverage(plan: dict, entries: list[dict]) -> dict:
    """Track declared assertions, without inferring product requirements."""
    states = {entry["index"]: entry["status"] for entry in entries}
    checks = []
    for name in plan["checks"]:
        indices = [index for index, step in enumerate(plan["steps"], 1) if step.get("check") == name]
        if not indices:
            status = "unplanned"
        elif any(states.get(i) == "fail" for i in indices):
            status = "fail"
        elif all(states.get(i) == "pass" for i in indices):
            status = "pass"
        else:
            status = "not_run"
        checks.append({"name": name, "status": status, "steps": indices})
    unverified = [check["name"] for check in checks if check["status"] != "pass"]
    return {"scope": "declared assertions only; not complete product coverage",
            "complete": not unverified, "checks": checks, "unverified": unverified}


def output_directory(manifest: dict, requested: Path | None) -> Path:
    if requested is not None:
        output = requested.resolve()
        output.mkdir(parents=True, exist_ok=False)
        return output
    parent = Path(manifest["bundle"]).parent.parent / "scenario-runs"
    parent.mkdir(parents=True, exist_ok=True)
    return Path(tempfile.mkdtemp(prefix="run-", dir=parent))


class RpcFailure(RuntimeError):
    def __init__(self, method, reply):
        self.reply = reply
        super().__init__(f"{method} failed: {reply}")


def capture_failure(peer, output: Path, step: dict | None) -> dict:
    """Best-effort read-only evidence with a separate, shared two-second budget.

    Do not step frames here: the failure state belongs to the application.
    Evidence errors must not replace the scenario's original failure.
    """
    deadline = time.monotonic() + 2
    evidence = {"budget_seconds": 2, "errors": {}}

    def observe(method, params=None):
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError("failure evidence deadline exceeded")
        reply = peer.call(method, params, timeout=remaining)
        if "error" in reply or reply.get("result") is None:
            raise RpcFailure(method, reply)
        return reply["result"]

    try:
        caps = observe("capabilities")
        evidence["current_scene"] = caps.get("current_scene")
    except Exception as exc:
        evidence["errors"]["scene"] = str(exc) or type(exc).__name__
    path = None
    try:
        fd, filename = tempfile.mkstemp(prefix="failure-", suffix=".png", dir=output)
        os.close(fd)
        path = Path(filename)
        observe("screenshot", {"path": str(path), "format": "png"})
        content = path.read_bytes()
        if not content.startswith(b"\x89PNG\r\n\x1a\n"):
            raise RuntimeError("failure screenshot is not a PNG")
        evidence["screenshot"] = {"path": str(path), "sha256": hashlib.sha256(content).hexdigest()}
    except Exception as exc:
        evidence["errors"]["screenshot"] = str(exc) or type(exc).__name__
    finally:
        if path is not None and "screenshot" not in evidence:
            try:
                path.unlink(missing_ok=True)
            except OSError as exc:
                evidence["errors"]["cleanup"] = str(exc)
    if step and "name" in step:
        try:
            evidence["object"] = observe("inspect_object", {"name": step["name"]})
        except Exception as exc:
            evidence["errors"]["object"] = str(exc) or type(exc).__name__
    evidence["status"] = "captured" if not evidence["errors"] else "partial" if "screenshot" in evidence else "unavailable"
    return evidence


def execute(peer, plan: dict, output: Path, manifest: dict, contract: dict) -> None:
    """Only drive input and observe runtime state; application C owns writes."""
    started = time.monotonic()
    deadline = started + plan.get("timeout_seconds", 30)
    report = {"schema_version": 1, "status": "running", "tool_contract": contract,
              "manifest": manifest, "scenario": plan, "steps": [],
              "evidence_scope": "native application and simulator; visual quality and device acceptance are not measured"}

    def remaining():
        seconds = deadline - time.monotonic()
        if seconds <= 0:
            raise TimeoutError("scenario deadline exceeded")
        return seconds

    def call(method, params=None):
        reply = peer.call(method, params, timeout=remaining())
        if "error" in reply:
            raise RpcFailure(method, reply)
        result = reply.get("result")
        if result is None or (isinstance(result, dict) and
                              (result.get("result_code", 0) != 0 or result.get("ok") is False)):
            raise RpcFailure(method, reply)
        return result

    try:
        if manifest.get("context"):
            report["compilation_context"] = json.loads(Path(manifest["context"]).read_text(encoding="utf-8"))
        if manifest.get("api_json"):
            sidecar = json.loads(Path(manifest["api_json"]).read_text(encoding="utf-8"))
            report["artifact"] = sidecar.get("artifact")
        caps = call("capabilities")
        report["capabilities"] = caps
        actions = {step["action"] for step in plan["steps"]}
        named_actions = {"expect_property", "wait_property", "expect_visible", "wait_visible", "wait_component", "wait_callback"}
        named_taps = any(step["action"] == "tap" and ("name" in step or step.get("callbacks")) for step in plan["steps"])
        if (actions & named_actions or named_taps) and not caps.get("named_api"):
            raise RuntimeError("named scenario steps require the matching Bundle api_json; rebuild with --api-header")
        for action, capability in (("expect_property", "named_property_read_version"),
                                   ("wait_property", "wait_property_version"),
                                   ("expect_visible", "effective_visible_version"),
                                   ("wait_visible", "effective_visible_version"),
                                   ("wait_keyboard_text", "bridge_keyboard_text_version"),
                                   ("wait_component", "wait_component_version")):
            if action in actions and caps.get(capability) != 1:
                raise RuntimeError(f"simulator does not support {action}; update the matching simulator")
        probes = any(step["action"] == "tap" and "callbacks" in step for step in plan["steps"])
        if any(step.get("require_new_event") for step in plan["steps"]) and caps.get("component_event_cursor_version") != 1:
            raise RuntimeError("simulator does not support new component event observation; update the matching simulator")
        if any("reference" in step for step in plan["steps"]) and caps.get("screenshot_compare_version") != 1:
            raise RuntimeError("simulator does not support screenshot comparison; update the matching simulator")
        call("subscribe", {"events": ["callback", "log"] if probes else ["callback"]})
        component_observations = {}

        def before_input(index):
            component_observations.clear()
            for following in plan["steps"][index:]:
                if following["action"] in ("tap", "drag"):
                    break
                if following["action"] != "wait_component" or not following.get("require_new_event"):
                    continue
                name = following["name"]
                if name in component_observations:
                    continue
                try:
                    observed = call("component_get_motion", {"name": name})
                except RpcFailure as exc:
                    raise RuntimeError(
                        f"require_new_event needs {name!r} in the scene before input; "
                        f"after navigation, use wait_scene before operating on the destination component: {exc}"
                    ) from exc
                cursor = observed.get("event_cursor")
                if (not isinstance(cursor, dict) or set(cursor) != {"epoch", "sequence"} or
                        any(type(value) is not int or value < 0 for value in cursor.values()) or
                        type(observed.get("scene_id")) is not int):
                    raise RuntimeError("component observation requires a valid runtime event cursor and scene")
                component_observations[name] = {"after_input_step": index, "before": observed}

        for index, step in enumerate(plan["steps"], 1):
            entry = {"index": index, "action": step["action"], "status": "running"}
            if "check" in step:
                entry["check"] = step["check"]
            report["steps"].append(entry)
            params = {key: value for key, value in step.items() if key not in ("action", "check")}
            action = step["action"]
            if (action in ("tap", "drag") and "wait_callback" in actions and
                    not (action == "tap" and "callbacks" in params)):
                # Flush the host's earlier events through the same response
                # stream before starting a new input. Clearing the local queue
                # alone would leave callbacks still waiting in the transport.
                call("wait", {"frames": 1})
                earlier = [note["params"] for note in peer.take_notifications()
                           if note.get("method") == "callback"]
                entry["callback_boundary"] = {"before": earlier[:64],
                                              "discarded_callbacks": len(earlier)}
            if action == "tap":
                checked = "callbacks" in params
                expected = params.pop("callbacks", [])
                frames = params.pop("max_frames", 6)
                if checked:
                    evidence = {"expected": expected, "observed": [], "events": [], "frames_observed": 0}
                    entry["callbacks"] = evidence

                    def collect(baseline=False):
                        notes = peer.take_notifications()
                        events = [note["params"] for note in notes if note.get("method") == "callback"]
                        overflow = [note["params"] for note in notes if note.get("method") == "log"
                                    and type(note["params"].get("dropped")) is int and note["params"]["dropped"] > 0]
                        if baseline:
                            evidence["before"] = events[:64]
                            total = len(events)
                        else:
                            total = len(evidence["events"]) + len(events)
                            evidence["events"].extend(events[:max(0, 64 - len(evidence["events"]))])
                            evidence["observed"] = [event.get("callback") for event in evidence["events"]]
                        if overflow:
                            evidence["overflow"] = overflow
                            raise RuntimeError("event loss/overflow; click result cannot be verified")
                        if total > 64:
                            raise RuntimeError("callback evidence limit exceeded")

                    # Separate earlier callbacks from this input. Each frame
                    # response follows event publication on the same stream.
                    call("wait", {"frames": 1})
                    collect(baseline=True)
                method = "tap_object" if "name" in params else "tap"
                entry["input"] = {"method": method, "params": params}
                before_input(index)
                entry["result"] = call(method, params)
                if checked:
                    for frame in range(1, frames + 1):
                        call("wait", {"frames": 1})
                        evidence["frames_observed"] = frame
                        collect()
                    matches = len(expected) == len(evidence["events"]) and all(
                        all(type(event.get(key)) is type(value) and event[key] == value for key, value in
                            ({"callback": want} if isinstance(want, str) else want).items())
                        for want, event in zip(expected, evidence["events"]))
                    if not matches:
                        raise RuntimeError(f"tap callbacks: expected {expected!r}, observed {evidence['events']!r}")
            elif action == "drag":
                before_input(index)
                entry["result"] = call("drag", params)
            elif action in ("expect_property", "wait_property"):
                if action == "wait_property":
                    result = call("wait_property", params)
                else:
                    result = call("get_property", {"name": params["name"], "property": params["property"]})
                entry["result"] = result
                if type(result.get("value")) is not type(params["value"]) or result["value"] != params["value"]:
                    raise RuntimeError(f"{params['name']}.{params['property']}: expected {params['value']!r}, observed {result.get('value')!r}")
            elif action in ("expect_visible", "wait_visible"):
                if action == "expect_visible":
                    observed = call("inspect_object", {"name": params["name"]})
                    visible = observed.get("effective_visible")
                    entry["result"] = {"name": params["name"], "effective_visible": visible,
                                       "expected_visible": params["visible"], "source": "runtime"}
                    if type(visible) is not bool:
                        raise RuntimeError(f"{params['name']}: object has no runtime visibility descriptor in api_json")
                    if visible != params["visible"]:
                        raise RuntimeError(f"{params['name']}.effective_visible: expected {params['visible']!r}, observed {visible!r}")
                else:
                    for frame in range(1, params.get("max_frames", 300) + 1):
                        call("wait", {"frames": 1})
                        observed = call("inspect_object", {"name": params["name"]})
                        visible = observed.get("effective_visible")
                        entry["result"] = {"name": params["name"], "effective_visible": visible,
                                           "expected_visible": params["visible"],
                                           "frames_waited": frame, "source": "runtime"}
                        if type(visible) is not bool:
                            raise RuntimeError(f"{params['name']}: object has no runtime visibility descriptor in api_json")
                        if visible == params["visible"]:
                            break
                    else:
                        raise RuntimeError(f"{params['name']}.effective_visible wait timed out: expected {params['visible']!r}, observed {visible!r}")
            elif action == "wait_keyboard_text":
                for frame in range(1, params.get("max_frames", 300) + 1):
                    call("wait", {"frames": 1})
                    observed = call("keyboard_text")
                    entry["result"] = {**observed, "expected_text": params["text"],
                                       "frames_waited": frame, "source": "runtime"}
                    if type(observed.get("text")) is not str:
                        raise RuntimeError("keyboard text observation did not return a string")
                    if observed["text"] == params["text"]:
                        break
                else:
                    raise RuntimeError(f"keyboard text wait timed out: expected {params['text']!r}, observed {observed['text']!r}")
            elif action == "wait_component":
                require_event = params.pop("require_new_event", False)
                entry["observation"] = {"kind": "state_match"}
                if require_event:
                    observation = component_observations.get(params["name"])
                    if observation is None:
                        raise RuntimeError("require_new_event needs an observation before the preceding input")
                    params["after_event"] = observation["before"]["event_cursor"]
                    params["scene"] = observation["before"]["scene_id"]
                    entry["observation"] = {"kind": "state_and_new_event", **observation}
                # Apply queued input before querying an already-idle component.
                call("wait", {"frames": 1})
                result = call("wait_component", params)
                entry["result"] = result
                state = result.get("state", {})
                if (result.get("status") != "settled" or not isinstance(state, dict) or state.get("dragging") is not False or
                        state.get("settling") is not False or
                        require_event and result.get("event_observed") is not True or
                        "value" in params and (type(state.get("value")) is not int or
                                              state["value"] != params["value"])):
                    raise RuntimeError(f"component did not settle at the expected value: {result}")
            elif action == "wait_scene":
                count = caps.get("scene_count")
                if type(count) is not int or not 0 <= params["scene"] < count:
                    raise ValueError(f"scene {params['scene']} is outside the loaded scene count {count}")
                for frame in range(1, params.get("max_frames", 300) + 1):
                    call("wait", {"frames": 1})
                    status = call("capabilities")
                    observed = status.get("current_scene")
                    entry["result"] = {"scene_id": observed, "expected_scene": params["scene"],
                                       "frames_waited": frame, "source": "runtime"}
                    if "input_ready" in status:
                        entry["result"]["input_ready"] = status["input_ready"]
                    if observed == params["scene"] and status.get("input_ready") is not False:
                        break
                else:
                    raise RuntimeError(f"scene wait timed out: expected {params['scene']}, observed {observed}")
            elif action == "wait_callback":
                while True:
                    event = peer.notification("callback", timeout=remaining())
                    if event.get("callback") == params["name"]:
                        entry["result"] = event
                        break
            elif action == "screenshot":
                # The observation channel advances a frame through wait;
                # render_fence belongs to the application Backend channel.
                call("wait", {"frames": 1})
                path = output / params["file"]
                request = {"path": str(path), "format": "png"}
                if "reference" in params:
                    request.update(reference=params["reference"], allow_changes=params.get("allow_changes", []))
                entry["result"] = call("screenshot", request)
                content = path.read_bytes()
                if not content.startswith(b"\x89PNG\r\n\x1a\n"):
                    raise RuntimeError(f"simulator did not write a PNG: {path}")
                entry["artifact"] = {"path": str(path), "sha256": hashlib.sha256(content).hexdigest()}
                if "reference" in params:
                    comparison = entry["result"].get("comparison", {})
                    entry["comparison"] = comparison
                    if comparison.get("matched") is not True:
                        raise RuntimeError(f"screenshot preservation failed: {comparison.get('changed_outside_allowed')} unexpected pixels; diff: {comparison.get('diff_path')}")
            entry["status"] = "pass"
        if "checks" in plan:
            coverage = check_coverage(plan, report["steps"])
            if not coverage["complete"]:
                raise RuntimeError(f"scenario checks not verified: {coverage['unverified']!r}")
        report["status"] = "pass"
    except BaseException as exc:
        report["status"] = "interrupted" if isinstance(exc, KeyboardInterrupt) else "fail"
        error = "RPC deadline exceeded" if isinstance(exc, queue.Empty) else str(exc)
        report["error"] = error
        if report["steps"] and report["steps"][-1]["status"] == "running":
            report["steps"][-1].update(status="fail", error=error)
            if isinstance(exc, RpcFailure):
                report["steps"][-1]["rpc_error"] = exc.reply
        if isinstance(exc, (KeyboardInterrupt, SystemExit)):
            raise
        try:
            report["failure_evidence"] = capture_failure(
                peer, output, plan["steps"][len(report["steps"]) - 1] if report["steps"] else None,
            )
        except Exception as capture_error:
            report["failure_evidence"] = {"status": "unavailable", "error": str(capture_error)}
        raise RuntimeError(f"scenario failed: {error}; report: {output / 'report.json'}") from exc
    finally:
        if "checks" in plan:
            report["check_coverage"] = check_coverage(plan, report["steps"])
        report["elapsed_seconds"] = round(time.monotonic() - started, 3)
        with (output / "report.json").open("x", encoding="utf-8") as stream:
            json.dump(report, stream, ensure_ascii=False, indent=2)
            stream.write("\n")
