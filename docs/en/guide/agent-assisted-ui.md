# Agent-assisted UI development

ESP-GSP provides an installable coding-agent Skill for turning product requirements,
reference images, or existing designs into validated scene JSON and generated
C API integration. The Skill uses the GSPC and ESP-GSP version selected by the
application and reads its schema, API reference and examples.

## Install the Skill

The canonical Skill is `skills/esp-gsp-ui` in the
[ESP-GSP repository](https://github.com/espressif/esp-gsp). Install it from the
same tag or commit as the ESP-GSP version used by the project. For Codex,
`$skill-installer` can install that path.

```text
Use $skill-installer to install skills/esp-gsp-ui from espressif/esp-gsp at
the ref used by this project.
```

From an existing source checkout, copy or link it into the skills directory
used by your coding agent. For example, with Codex:

```sh
mkdir -p ~/.codex/skills
cp -R skills/esp-gsp-ui ~/.codex/skills/
```

Invoke it as `$esp-gsp-ui` on the next turn. The Skill belongs to the GitHub
development experience and is not included in the ESP-IDF
component archive. A project using the managed component still has the
bilingual documentation, widget examples, and generated references that the
Skill reads. Without installing the Skill, follow this guide and use
`gspc doctor`, `gspc cards`, and `gspc diagnose` directly.

## Give it a useful requirement

Provide the project path and describe the desired page behavior. The Skill
reads existing target, display, bundle and tool settings from the project.
For a new project, include the display size or orientation when it is relevant.
Existing project configuration is used when available, and product decisions
that are not represented in the project can be supplied in the request.

Useful details include page states, dynamic data, navigation, available assets,
and the interactions you want checked. You do not need to know generated API
names or resource slot rules.

For assets that should stay outside the firmware, follow the
[external-assets workflow](external-assets.md) and point the agent at
`examples/usage/external_assets`. Keep the manifest, target Image placeholders,
and the UI profile compatible, and request a fallback and status check for
missing or replaced files. For an SD/NAND font, use the
[font-file workflow](external-assets.md#fonts-on-sd): apply the loaded font
before UI startup with an explicit size limit, then close it after the UI has
stopped. This keeps storage ownership and font lifetime in the application
while the generated UI API remains unchanged.

A concise creation request can be:

```text
Use $esp-gsp-ui in this ESP-IDF project. Create scenes/settings.json for a
480x320 RGB888 display with a brightness slider, Wi-Fi toggle, language
dropdown, and Back action. Integrate the generated API, build the project, and
preview the page in the simulator.
```

For a reference image, attach it and state whether it is a visual reference or
an asset that must ship. A static image does not define pressed, disabled,
scrolling, loading, error, or navigation behavior; include those states in the
request when they matter.

For an existing UI, name the behavior rather than asking for a blind rewrite:

```text
Use $esp-gsp-ui to add an application-backed Wheel to scenes/setup.json.
The available options come from the application and can change after loading.
Keep the current selection when possible, support an empty list, and preview
scrolling and selection.
```

For a reported problem, provide the steps and expected result:

```text
Use $esp-gsp-ui to investigate this: tapping Enter switches to the clock page
and also triggers the card at the same position. Reproduce it in the simulator,
find the cause, and fix it so one touch only activates the starting page.
```

## What the Skill does

The Skill follows the same ownership model as the
[recommended workflow](workflow.md):

1. identifies the selected component, target, GSPC, bundle, scene, and BSP;
2. loads the relevant widget card, checked example and documentation as needed;
   `gspc cards --list` helps discover controls when their type is not yet known;
3. keeps layout and declarative behavior in JSON, product state in the
   application, and panel behavior in the BSP;
4. validates the edited scene through the target-configured compile/preview or
   owning build, using its diagnostics and `suggestions`;
   `gspc diagnose` provides compilation checks when no artifacts are needed.
   Daemon `diagnostics/pull` defaults to schema checks; use `level: "compile"`
   when advertised by `doctor/get`;
5. uses bundle headers and `*.api.json` from that compile, integrates their
   APIs, and builds application changes;
6. checks changed visuals and interactions in the simulator, resolving or building
   it as needed; loads the
   `--api-json` from the same compile, uses its names and initial layout metadata
   for inspection, and verifies input routing and state through current runtime
   queries; with an application backend,
   state writes and navigation stay backend-owned;
7. summarizes changes, build results and preview checks.

For overlapping controls, popups and changing geometry, include coordinate
probes at intersections, exposed edges, modal backgrounds and clipped areas.
The native runner's `tap` can assert an exact callback sequence, including
`callbacks: []`; pair it with application properties and scene checks. Repeat
after show/hide, close or move actions, and review the captured images for visual
collisions. See the [native scenario reference](../../../tools/sim_bridge/README.md#repeatable-application-checks).

Review compile-level `GSPC-TEXT-CLIPPED` information for initial static text;
intentional clipping remains valid. Use the runner's `wait_component` for
Drawer/PageFlow value and idle-state checks. For a local edit with a known-good
PNG, optional screenshot `reference`/`allow_changes` checks report changes outside
the intended region. Keep the profile and application state fixed for comparison.

Select acceptance paths from the requested behavior: relevant input boundaries,
confirm/cancel, return/reenter and empty/error states. The runner's optional
`checks`/`check` labels report failed, unexecuted and unplanned assertions; a run
cannot pass with unverified declared checks. `wait_keyboard_text` verifies the
attached keyboard's input, including empty and UTF-8 text; verify application
state separately for saves and cancellation.

For unchanged inputs, scene set and resolved compiler settings, reuse diagnostics
from the successful build or preview already being performed.

CLI diagnosis requires actual scene paths or a generated context supplying
the registered scene set. With the daemon, use
`cards/list` for widget summaries and `cards/get` for fields. `doctor/get`
includes the same compiler compatibility manifest as CLI doctor.

For `compile/preview`, check the result's `success`, `stale`, `inputIdentity`
and `artifactIdentity`; the response's `ok` only indicates request handling.
Identities record the source JSON CRC32 and byte length, absolute root/path,
compiler-options CRC32 and pixel format. Source identity covers JSON, so also
verify the matching Bundle/API sidecar for asset changes.
An old Bundle is only returned for the same root, stable scene path and resolved
compiler options, and remains a display fallback after a failed compile.
Anonymous and binary-streaming previews do not reuse cached Bundles.
`compile/previewProject` requires an opened or updated project, reports success
and identities per screen, and leaves failed screens dirty. Changes to resolved
compiler settings or referenced assets/components also invalidate
the previous preview. An empty result for an open project means no screens were dirty.

When the compiler advertises `compiler_features.scene_inspection: 1`, add
`inspect: {"name":"title"}` to the existing daemon preview request for optional
source, style/component and layout-owner explanations. `inspect: true` lists
nodes with short text previews; `nodeId` selects an anonymous node. Check source
snapshot CRCs before editing; inspection describes initial layout, not runtime
input hits. CLI
`compile`/`pack --inspect --api-header ...` adds the same optional facts to the
application API sidecar.

When an ESP-IDF build already exists, run `gspc doctor <project> --build
<build-dir>` first. Use its observed target and component contract for the
agent workflow; an unavailable build description must remain an unknown rather
than being replaced with a host assumption.

Source checkout directory names may vary. Read `build_contract.component.selected_names`
for the actual CMake component identifiers and `component.paths` for their roots.

`build_contract.status` distinguishes `unavailable` (no description), `invalid`
(malformed description), `incomplete` (required facts missing), `stale` (project
or target mismatch), and `observed` (build metadata read). Use `missing` and
`stale_reasons` to resolve reported issues. An explicit `--build` selects only
that build directory. The command's exit status indicates whether the report
was generated; the owning ESP-IDF configure/build validates the configuration
and tool compatibility.

The selected compiler may advertise `compiler_features.compile_context: 1`.
After application Bundle generation, `doctor.contexts` lists the generated
`*.context.json` for each registered Bundle. Select the affected Bundle's
`observed` context and run:

```sh
gspc diagnose --context path/to/bundle_gsp.context.json
```

This reuses the actual registered scenes, effective target Profile, codec/cache
choices and slot/project limits, including checks across shared resources.
Explicit scene paths select a scoped check. Daemon compile methods and compile
diagnostics accept the same `context` path. An invalid context fails explicitly;
it does not select a host default. `pending` contexts need the owning Bundle
built. Contexts are generated snapshots; reconfigure/build after changing CMake,
sdkconfig, Profile or toolchain settings. Older compatible compilers keep the
existing explicit-argument workflow.

Use [Scene JSON](scenes.md) to understand authored structure,
[Runtime and generated API](runtime-api.md) for application integration, and
the [Widget library](../components/index.md) for every supported control.
Each card's `example` is a fragment: use its `example_path` and documentation
paths for the complete example and field reference. For PageFlow/Drawer
automation, check `capabilities` for component-motion support before using
`component_get_motion`, `wait_component(name, optional value, max_frames)`, or
`component_event`; a successful wait requires idle and, if supplied, the requested value,
and timeout is a JSON-RPC error. Compiled bounds are initial metadata, not
runtime hit-test proof.

## Choose the preview path

Use the standalone simulator from [Simulator preview and testing](../configuration/simulator-preview.md)
for scene layout, declarative actions, and control behavior. This checks the
scene without running the owning application's C tasks or product state logic.

> **For coding agents:** Prefer headless mode for routine automated testing. If
> the user wants to open the simulator and interact with it, start a separate
> simulator and assign browser control with `--input-mode browser-exclusive`.
> Do not expose the WebUI of the agent's automation/debug instance unless the
> user explicitly asks to watch that workflow; an automation-owned preview may
> look interactive while rejecting user input.

When acceptance depends on application C callbacks, timers, dynamic collections,
or live state, prefer the component's `sim_bridge`. It runs portable application
C code on the PC and sends UI rendering to the simulator. After the ESP-GSP
component is installed in the project (for example, after `idf.py reconfigure`),
run this from the application root:

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py --project pc
```

The component also includes `examples/usage/hello_world/pc` and
`examples/usage/sim_bridge_media/pc`; pass either path as `--project` to try the
flow. From a source checkout, invoke that checkout's
`tools/sim_bridge/run.py`. See the [sim_bridge guide](../../../tools/sim_bridge/README.md)
for requirements, tool selection, PC HAL/mocks, and the supported API subset.

For automation, the runner accepts `--headless --api-port 8266 --backend-port
8684 --input-mode api-exclusive`. It loads the matching API sidecar and prints
actual session addresses plus observed tool versions. Use distinct ports for
separate sessions. After `backend_spawned`, wait for the requested application
state and capture the rendered result. For bounded checks use
`--duration` so the runner supervises Backend completion and host cleanup.

For a repeatable input/state check, pass `--scenario pc/check.json`. This mode
selects headless rendering and API-exclusive input, executes named taps, drags,
typed scalar assertions, callback waits and screenshots, then prints a unique
`report.json` path in the build tree. An assertion failure preserves the actual
result and exits nonzero. The [sim_bridge guide](../../../tools/sim_bridge/README.md)
describes the bounded scenario format; check `run.py --help` when using an older
component. The application C Backend owns state changes throughout the check.
After navigation, use the advertised `wait_scene` scenario step before reading
destination controls; it observes the active scene without changing it.

Use sim_bridge to check application behavior. Its supported API subset and
single-thread Backend contract differ from ESP-IDF task scheduling; it does not
verify peripherals, panel timing, or device performance. Build the owning
ESP-IDF application and use the board for those checks.

## Review the result

Review the source JSON and application changes, then rebuild to refresh generated
headers and bundles. Check object names, callback responsiveness, resource ownership
and capacity for simultaneously active objects. Preview layout and interaction in the
simulator, then follow [Display integration](display.md) to check the board.

The Skill reads the selected component's schema, headers and examples and follows
the language used in your request.
