---
name: esp-gsp-ui
description: Create, modify, review, diagnose, preview, or integrate ESP-GSP JSON scenes in ESP-IDF projects. Use when a coding agent needs to turn UI requirements or reference images into scenes/*.json, select ESP-GSP widgets and assets, validate scenes with gspc, integrate generated bundle APIs, connect dynamic data or events, or troubleshoot scene, build, simulator, and display behavior.
---

# ESP-GSP UI

Build UI through the public ESP-GSP authoring and generated-API contracts. Keep
the result compatible with the exact component and GSPC selected by the target
application.

## Choose the task path

Start from the user's desired behavior and reuse facts already present in the
project or conversation. Choose the smallest complete path:

- **Create a screen or use a reference image:** find the bundle registration,
  choose native widgets from matching examples, author the scene, diagnose,
  generate and preview, then connect application behavior when requested.
- **Change an existing screen:** inspect the affected scene and its application
  callers, preserve its names and registration, and validate the changed layout
  or interaction. Use cards and the widget page first; add schema or inventory
  when a field or version detail is unclear.
- **Connect live data:** identify the data source, update rate, maximum live
  items and ownership; generate the appropriate setters or collection binders,
  integrate them, and build the owning application.
- **Draw application content:** inspect the existing Canvas contract in
  `guide/media-and-data.md` and `examples/sim_bridge_media`. Choose direct draw
  or frame submission according to target geometry and producer ownership.
  Use the selected version's actual image/bind syntax and public APIs.
- **Reproduce a problem:** record the actual component/tool versions and input,
  reproduce the reported steps, and isolate the failing scene, generated API,
  runtime, or display path before changing it. Use the simulator for layout and
  interaction, and the board for panel, timing, and device-only failures.

For an existing application, discover display and build settings from files;
do not make the user fill out a configuration questionnaire. Ask only for
missing product choices needed by the current task. Continue independent work
while awaiting an answer. Keep validation proportional to the change and honor
the user's requested validation scope.

## Establish the project contract

Identify the application root, active ESP-GSP component, GSPC executable,
bundle registration and affected scenes. Read logical resolution, pixel format
and required interactions for scene work; inspect `IDF_TARGET`, BSP display
path and PSRAM when integrating or diagnosing the device.

Use `gspc doctor <project> --build <build-dir>` when an ESP-IDF build exists; it
reads `build/project_description.json` and reports the observed Target, build
revision, selected `esp-gsp` component path, sdkconfig capabilities and relevant
CMake options. Use that contract to identify the component selected by the
build. If no build exists, inspect the project dependency and CMake
configuration without assuming that a nearby ESP-GSP checkout is active. Resolve
GSPC from the CMake cache or `GSPC_EXECUTABLE`. Otherwise read `.gspc_version`
from the application root first and the selected component second, then use
the installed `esp-gsp-tools` implicit resolver:

```sh
python -m gsp.execute --version <version> gspc compatibility
```

When developing the ESP-GSP framework itself, `ci/gspc-dev` remains available
for CI and source validation. Use the selected component's version metadata to
configure the project marker or `GSPC_EXECUTABLE`. Ask for a version or executable
only when it cannot be determined from the project. Then run:

```sh
gspc compatibility
```

Treat this output as the compiler side of the contract; the owning CMake
configure or build performs the final component/compiler compatibility check.
Do not continue with a known incompatibility. Ask only when a missing choice
changes product behavior, such as resolution, rotation, pixel format, dynamic
data ownership, navigation, or asset treatment.

Commands below use `gspc` for readability. Invoke the resolved absolute
executable when it is not available through `PATH`.

## Use current sources of truth

Read documentation from the ESP-GSP component selected by the application
build. Start with the matching language under `docs/en/` or `docs/zh-Hans/`,
then load only the pages needed for the task:

- `guide/workflow.md` for JSON, application, and BSP ownership;
- `guide/scenes.md` for hierarchy, naming, actions, and templates;
- `guide/runtime-api.md` and `guide/media-and-data.md` for generated APIs,
  callbacks, collections, Canvas, and buffer ownership;
- `reference/configuration.md` for `gsp_add_bundle()` and runtime configuration;
- `reference/simulator.md` for simulator CLI, automation API, and evidence limits;
- `guide/simulator-preview.md` for preview workflows and testing strategies;
- `components/<widget>.md` and `examples/widgets/<widget>/<widget>.json` for a
  concrete control.

Query GSPC instead of copying field or version tables into the project or this
skill. Start with the widget card, its component page, and the checked example;
add schema, docs, or inventory when the task needs field ranges or the full
registry:

```sh
mkdir -p build/gsp-agent-cache
gspc cards slider
gspc schema --authoring -o build/gsp-agent-cache/scene.schema.json
gspc docs -o build/gsp-agent-cache/authoring-reference.md
gspc inventory --format json -o build/gsp-agent-cache/widget-inventory.json
```

After compile or pack, read the sidecar `*.api.json` next to generated headers
for object names, typed operations, helpers, bind/component keys and callbacks.
Use the bundle-level sidecar produced by the same compile as the preview bundle;
the simulator rejects mismatched bundle length/CRC metadata. Those mappings
come from the compiler sidecar, not from inspecting GSPB bytes.
For `gsp_add_bundle`, use the sidecar path reported by the build next to
`<symbol>_gsp.h`. For direct GSPC use, pass `--api-header`; its extension is
replaced with `.api.json`.
`gspc doctor [project]` reports the compiler contract, sidecar commands and
scene files; when a build description is available it also reports target
facts. An unavailable build contract is unknown, not a host-profile fallback.

Keep these disposable exports below the ignored build tree.
The installed compiler schema, selected component documentation, public
headers, generated headers, and checked widget examples are authoritative.

## Translate the requirement into ESP-GSP

For prose, sketches, or screenshots, determine screen states, hierarchy,
dynamic data, actions, assets, resolution, and acceptance criteria. Treat a
screenshot as a visual reference unless the user identifies it as a product
asset. Reconstruct text and controls as native scene objects; do not flatten a
complete UI into one image.

Put each requirement in one owner:

| Requirement | Owner |
|---|---|
| Layout, style, static text, local assets, declarative actions | Scene JSON |
| Named values, visibility, selection, navigation, application events | Generated C API |
| Product state, storage, networking, sensors, blocking work | Application task |
| Dynamic rows, runtime images, camera/video pixels | Public collection, media, or Canvas API |
| Panel timing, framebuffer, byte order, rotation, TE, touch mapping | BSP/display target |

Prefer the application convention `scenes/*.json` and
`gsp_add_bundle(${COMPONENT_LIB})`. Preserve an existing explicit `SCENES` list,
bundle `SYMBOL`, or pixel format unless the requirement changes it. Use stable
`name` values only for application-controlled elements and stable `callback`
values only for actions crossing into product code.

Use declarative navigation for ordinary scene changes and callbacks for
application-owned behavior, including Back history; use `stack_pop` for a
StackView. For a Slider or Arc value-change callback, check the selected
version's value/release action contract and use generated event helpers. Keep
networking, storage and other blocking work in an application task.

Choose List, Grid, Wheel, or Message List for application-backed collections;
use Canvas for continuously produced pixels. Let GSPC derive recycled
List/Grid/Wheel row instances and row/text slot demand from viewport and
template geometry. Set `max_instances` only on an explicit template that the
application creates directly, using its maximum simultaneously live instance
count. Do not replace JSON-derived demand with duplicate Kconfig or application
configuration. Distinguish recycled row slots, per-row dynamic text/resource
slots, and simultaneously bound collection quota when diagnosing limits.

## Connect data and assets with predictable behavior

Use the selected version's widget page and generated header to confirm these
patterns before applying them:

- **Wheel data:** use `dynamic_items: true` when items change at runtime so the
  generated collection API remains available for empty, short and long lists.
  Bind once; use compiled text through the documented default binding or a row
  callback for application data. Selection uses logical item indexes. Let the
  compiler manage row slots. Use `cyclic` when continuous scrolling is wanted.
- **Chart data:** declare the series, value range and point capacity in JSON;
  use generated series setters or append helpers for updates. Keep sampling in
  the application and choose an update rate appropriate to the display.
- **Dynamic text:** declare the characters needed by future values, size the
  text area using the selected font metrics, and check the documented multiline
  alignment/overflow combinations before integration. Preview long, empty and
  representative translated values when relevant.
- **SVG and images:** distinguish runtime vector Image capabilities from
  bitmap assets used by carousel/effects. Inspect compiler diagnostics for the
  source element and cause. If raster export is needed, preserve the requested
  appearance and confirm that it still supports the required interaction or
  animation; do not silently discard those behaviors.
- **Canvas drawing:** direct callbacks may run once per partition. Use
  object-local coordinates and `stride_bytes`, initialize every supplied
  region, and keep one consistent state across the repaint. Advance animation
  or consume samples before drawing. Follow the selected version's target,
  callback and shutdown contracts; the PC bridge's offscreen preview does not
  verify device partition timing or memory use.
- **Image memory:** inspect the bundle's resource report and target decoder/
  cache configuration. Separate stored asset size from visible decoded memory;
  check initial display and page transitions before changing the budget.

When a requested control or style is absent, check the current public contract
and offer the closest composition with its concrete behavior differences.
Implement a composition when it satisfies the request; ask when the choice
would change the intended user experience.

## Validate before integrating C

After every meaningful JSON change, run structured diagnostics against the
actual scene files. Pass target capabilities only when they are confirmed by
the project or user:

```sh
gspc diagnose scenes/main.json --format json
```

Fix diagnostics at their reported JSON paths. Apply `suggestions` when present.
Use the properties defined by the installed ESP-GSP Schema, `gspc cards`, and
widget reference.

Generate the bundle and headers through `gsp_add_bundle()` or the matching GSPC
before using new application APIs. Inspect compiler diagnostics, execution
requirements, `<symbol>_gsp.h`, and per-scene headers. Use the declared typed
setters, event predicates, collection binders, and template helpers. Validate
application changes through the owning ESP-IDF build.

Fix source JSON or compiler inputs and regenerate outputs. Prefer generated
helpers for named controls; use public generic APIs with generated keys for
batch updates or data-driven operations.

Keep callbacks non-blocking and follow COPY, BORROW, and TAKE ownership. Treat
setters as asynchronous submissions on ESP-IDF. Use `esp_gsp_flush()` only for
an explicit synchronization boundary.

## Preview and verify

### Locate the simulator

Resolve `gsp_sim_host` before attempting preview. Try the following in order
and use the first that succeeds:

1. **`GSP_SIM_EXECUTABLE` environment variable** — if set, use it directly.
2. **`esp-gsp-tools` manager** — read the ESP-GSP component version from
   `idf_component.yml` and invoke through the manager:
   ```sh
   python -m gsp.execute --version '<ESP-GSP version>' sim --capabilities
   ```
   If the manager returns a working executable, use `python -m gsp.execute
   --version '<ESP-GSP version>' sim` as the simulator command prefix.
3. **Framework source checkout** — when working inside the ESP-GSP repository
   itself, the CI-built binary is at
   `tools/sim_host/target/release/gsp_sim_host`. Verify it exists and is
   executable before use.

If preview is requested and a framework source checkout is available, follow
its simulator build instructions to obtain the executable. If the environment
prevents that build or execution, report the specific blocker and continue
independent firmware work. Firmware builds can proceed without a simulator.

Commands below use `gsp_sim_host` for readability. Substitute the resolved
path or manager prefix when `gsp_sim_host` is not on `PATH`.

### Interactive preview

Compile the same scene set, pixel format and resource options as the
application. Prefer its existing generated bundle when available. The following
example is for a single scene; include the registered scenes for cross-scene
navigation checks. Open the browser preview:

```sh
gspc pack scenes/main.json --deployable -o build/app.gspb \
    --api-header build/app_gsp.h --symbol app
gsp_sim_host --bundle build/app.gspb --frames 0
```

The host opens a browser page with real-time rendering, pointer input,
25 %–400 % zoom, scene navigation (◀ ▶ ↺), and a log panel showing
callbacks and runtime output.

### Headless screenshot

For quick render checks without a browser:

```sh
gsp_sim_host --bundle build/app.gspb --headless \
    --frames 3 --dump build/preview.png --dump-format png
```

Add scripted input to verify interactive paths:

```sh
gsp_sim_host --bundle build/app.gspb --headless \
    --tap 160 120 --wait 5 --dump build/after-tap.png --dump-format png
```

### API-driven automation

Enable the JSON-RPC API channel for programmatic control. This is the
preferred approach for automated verification:

```sh
gsp_sim_host --bundle build/app.gspb --frames 0 \
    --api-json build/app_gsp.api.json \
    --api-listen tcp://127.0.0.1:8266
```

Drive the simulator through JSON-RPC 2.0 over TCP with Content-Length
framing. The typical agent verification sequence:

1. `capabilities` — confirm display size, scene count, and `named_api`.
2. `goto_scene` — navigate to the target scene when no application backend is connected.
3. `list_objects` / `inspect_object` / `hit_test` to confirm compiled names and bounds.
4. `tap_object` / `set_property` with `name` when `named_api` is true; otherwise
   `tap` / `drag` at known coordinates or numeric `bind_id`.
5. `wait` — let animation settle (e.g. 3–5 frames).
6. `screenshot` — capture the result for comparison.
7. `quit` — shut down the simulator.

Use `--input-mode api-exclusive` when the automation must be the sole input
source. Subscribe to `callback` and `scene_changed` events to observe UI
responses.

When testing application logic alongside the UI, add a backend channel:

```sh
gsp_sim_host --bundle build/app.gspb --frames 0 \
    --backend-listen tcp://127.0.0.1:8684 --backend-required \
    --api-listen tcp://127.0.0.1:8266
```

The backend responds to `callback` notifications (optional `callback` name
when `--api-json` is loaded) and drives UI updates through `set_text`,
`set_value`, `goto_scene`, etc., mirroring the firmware C API behavior.
When a backend is enabled, it owns state writes and scene navigation; use the
API channel for named input, inspection, waits and screenshots.
Named `set_*` parameters accept `{ "name": "<object>", ... }` in addition to
`bind_id`; `set_property` uses the sidecar's property type and range. Numeric
methods remain valid when no api.json is loaded. Compiled bounds are initial
layout metadata, so re-check screenshots after runtime movement or animation.

For a source-checkout release gate, run
`python3 tools/sim_host/tests/agent_loop_smoke.py --host <sim-host> --gspc <gspc>`.
It covers a deterministic diagnose-and-repair case before exercising the
matching Bundle/API sidecar in the simulator. It is a host/simulator test, not
a substitute for the owning ESP-IDF build or board acceptance.

### Visual regression

For a reported visual regression, hold GSPC, simulator, target profile,
logical resolution, pixel format, scene, and frame count constant. Compare
against a known-good bundle or commit, or make one controlled property change
for A/B output. A scene that compiles is contract-valid but not visually
accepted.

### Evidence layers

Report verification as separate layers:

1. Scene diagnostics or compilation;
2. generated API compilation;
3. target configuration, compilation, and linking;
4. simulator or host behavior;
5. named-board execution;
6. human visual and touch acceptance.

Do not present simulator output as panel proof, device logs as visual proof, or
an unrun layer as passed. Report missing fonts, assets, target facts, simulator
availability, and framework capability gaps explicitly. Do not hide a missing
public framework capability in private-header coupling or an application-side
workaround.

## Keep implementation documentation product-facing

When the user asks for an implementation document, write it as a durable
product or engineering artifact. Describe the implemented behavior, public
interfaces and usage, configuration and dependencies, ownership and lifecycle,
validation evidence, and any genuine product or framework constraints that
affect integration or acceptance.

Keep implementation documents product-facing. Describe public behavior, actual
dependencies, verified evidence and genuine product or framework constraints;
omit authoring-process details, temporary workspace state and internal decision
rationale. Do not turn an unrun check or an unavailable observation into a
product limitation.

If work is incomplete, record the status in the validation or handoff section
with the exact missing check and evidence. State a limitation in the
implementation document only when it is established by the selected public
contract, source behavior, compiler diagnostic, or executed test; distinguish
not implemented, not supported by the selected version, not validated on the
target, and environment-blocked cases. Prefer affirmative, user-actionable
wording and keep internal optimization notes in the agent response or change
log rather than in implementation documentation.

## Deliver a usable result

Summarize the changed source files, how to build or preview them, and the checks
actually completed. Explain any required application hookup or asset choice
in the user's language. Include a screenshot for visual work when captured,
and identify any remaining blocker precisely. Keep generated artifacts in the
project's normal build location and leave unrelated project settings intact.
