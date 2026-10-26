# Simulator setup and behavioral verification

## Preview and verify

### Keep agent and user simulator sessions separate

For routine agent-driven testing, prefer a headless simulator. If the user
wants to open the simulator and interact with it, start a separate simulator
for that purpose and give the browser control with `--input-mode
browser-exclusive`. Do not expose the WebUI of the agent's automation/debug
instance unless the user explicitly asks to watch that workflow; otherwise the
automation-owned preview can look interactive while rejecting the user's
input.

When running more than one simulator on the same machine, assign explicit,
distinct visual and API ports, plus Backend ports when using the optional
Socket mode, instead of relying on automatic port allocation. Record which
bundle/project owns each endpoint. If a captured
simulator image differs greatly from the expected scene, check for another
running simulator, including one from a different project, before treating the
image as evidence of a UI regression.

### Choose the preview path

Use the standalone simulator when checking scene layout, declarative actions,
or control behavior without application C logic. It can inspect and operate
named scene objects, but it does not execute the owning ESP-IDF application's
tasks or product state.

When the task depends on application callbacks, timers, collections, or live
state implemented in C, prefer the selected component's native `sim_bridge`
when the project has a PC adapter:

```sh
python <selected-esp-gsp-component>/tools/sim_bridge/run.py \
    --project <application-pc-project>
```

The runner builds the deployable Bundle and generated headers, builds both a
shared library and a Socket executable, and loads the library by default.
Use `--socket-backend` for process isolation; `--backend-port` also selects
Socket mode. Library ESP-GSP and Backend ABI versions must match sim_host.
For automation, use `--headless --api-port <port> --input-mode api-exclusive`;
a user preview can use `--visual-port <port> --input-mode browser-exclusive`.
Use distinct ports and build directories. The runner loads the API sidecar and
reports tool identity and endpoints. After `backend_loaded` or `backend_spawned`,
observe the application and capture its rendered state. Prefer an existing
project adapter or the component's
`examples/usage/hello_world/pc` and `examples/usage/sim_bridge_media/pc`
examples. For a project without a PC adapter, inspect
`tools/sim_bridge/README.md` before deciding whether its hardware dependencies
can be isolated behind a small PC HAL/mock.

The bridge implements a supported subset of `esp_gsp.h` and runs application
callbacks on one native Backend thread. The default shared library is trusted
code inside sim_host and can crash or block it; the Socket mode isolates that
failure. Use either mode to check application behavior, not ESP-IDF task
scheduling, peripheral behavior, panel timing, or device performance. Build
the owning ESP-IDF application and use the board for those checks.

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

When previewing repository examples, check that their media is present before
invoking GSPC directly. For missing media, follow the selected checkout's
`examples/README.md` asset-retrieval and source-preview instructions, including
the pinned archive verification and offline cache path. Preserve edited media
before restoring assets; direct GSPC and preview scripts do not fetch them.

Use the Bundle and matching API sidecar from the owning build or compilation
with the selected context. Keep its scene set, pixel format and resource
options; cross-scene navigation needs the registered scene set. The examples
below use `build/app.gspb` and `build/app_gsp.api.json` as placeholders for those
existing artifacts. Substitute their actual paths; no additional compile is
needed for unchanged inputs. Open the browser preview:

```sh
gsp_sim_host --bundle build/app.gspb --api-json build/app_gsp.api.json \
    --frames 0 --input-mode browser-exclusive
```

Open the URL printed by the host in a browser. The page provides real-time rendering, pointer input,
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

For repeatable application input/state checks, the native runner also accepts
`--scenario <check.json>` and an optional `--report-dir <new-directory>`.
Read its supported steps in `tools/sim_bridge/README.md`; reuse this bounded
headless path for taps, drags, scalar/keyboard/callback checks and screenshots. It keeps
writes in the application's C Backend and emits a report with actual results.
Inspect the runner's `--help` before using it with an older component.
After a navigating input, use its `wait_scene` step when advertised before
checking destination controls. Custom RPC clients should observe
`scene_changed` or `capabilities.current_scene`; `wait_scene` is not an RPC method.
For Drawer/PageFlow motion, the scenario's `wait_component` step uses the existing
RPC wait: supply the component name, expected value and a bounded `max_frames`.
It confirms the value and idle state after processing queued input; a scene ID
or scalar match alone does not establish that animation has settled.
For a failed scenario, inspect its `failure_evidence` screenshot and scene before
replaying it. Capture is best-effort and preserves the original failure; object
bounds still describe compiled layout.

### Plan acceptance paths

Derive checks from the user's requested behavior and existing product rules.
For the affected flow, cover normal and boundary input, confirm versus cancel,
return/reenter state, disabled/modal behavior and relevant empty/loading/error
states through actual input and application C.

- **Assertion coverage (`checks`):** When advertised by the runner, declare
  root `checks` and attach `check` labels to assertion steps (`expect_property`,
  `wait_property`, `expect_visible`, `wait_visible`, `wait_keyboard_text`,
  `wait_component`, `wait_scene`, `wait_callback`, `tap` with `callbacks`, or
  `screenshot` with `reference`). Inspect `check_coverage` to confirm all
  declared checks pass (`unplanned` or `not_run` checks fail the scenario).
- **Keyboard forms (`wait_keyboard_text`):** Check
  `bridge_keyboard_text_version: 1` and ensure application C attaches the
  keyboard. Test key taps, deletion and byte limits with `wait_keyboard_text`,
  then verify Confirm, Cancel and return behavior via application properties or
  scene navigation.

### Check overlaps and click-through

For a created or changed UI, inspect screenshots for obscured text and controls.
Use optional compiler inspection and `gspc diagnose --layout-check` to select
candidate points, then verify runtime hit behavior in `--scenario`:

- Use coordinate `tap` with `callbacks` to assert the exact observed CALL
  sequence (for example `{"action":"tap","x":60,"y":40,"callbacks":["front"]}`,
  or `{"callback":"choose","arg":2}` for shared callbacks).
- Probe overlap centers, exposed edges, modal blank areas (`callbacks: []`) and
  action buttons, and repeat after showing/hiding, closing, moving or scrolling
  content.
- Pair callback checks with runtime property or scene assertions (`max_frames`
  defaults to 6 frames for callback collection).

Drive the simulator through JSON-RPC 2.0 over TCP with Content-Length
framing. The typical agent verification sequence:

1. `capabilities` — confirm display size, scene count, `named_api`, and the
   advertised component motion/event versions.
2. `goto_scene` — navigate to the target scene when no application backend is connected.
3. `list_objects` / `inspect_object` — inspect compiled names, initial bounds,
   and (when `effective_visible_version: 1`) runtime `effective_visible` across
   parent visibility and transform viewports. With `runtime_input_query_version: 1`,
   `query_geometry` and `hit_test` use current input geometry.
4. `tap_object` with `require_verified: true` / `set_property` with `name` when `named_api` is true; otherwise
   `tap` / `drag` at known coordinates or numeric `bind_id`.
5. `wait_component` — verify the requested PageFlow/Drawer value and settled
   idle state (`name`, optional `value`, `max_frames`). To require a new event,
   save `component_get_motion.event_cursor` before the action and pass it as
   `after_event` (`component_event_cursor_version: 1`). Initial equality alone
   does not prove the action worked. In a native scenario, use
   `require_new_event: true` on `wait_component` to capture this cursor before
   the preceding input. The component must already exist in that scene; after
   navigation, use `wait_scene` before operating on destination components.
6. `get_property` / `wait_property` — read or wait for a named scalar property
   (`named_property_read_version` / `wait_property_version`), then capture the
   frame.
7. `screenshot` — capture the result for comparison.
8. `quit` — shut down the simulator and check its exit status.

Use `--input-mode api-exclusive` when the automation must be the sole input
source. Subscribe to `callback` and `scene_changed` events to observe UI
responses.

Use `gsp_sim_host --record-input <path>` or `--replay-input <path>` to reproduce
touch sequences. For static layout advice, use `gspc diagnose` with
`--layout-check` and, for round displays, `--round-safe-area`; these are compiler
diagnostic flags, not simulator options.

When a project already provides a separate compatible Backend, it can be
connected directly:

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
For application C sources, prefer `sim_bridge` above so the existing business
logic can run in the Backend instead of implementing a second state model.
Named `set_*` parameters accept `{ "name": "<object>", ... }` in addition to
`bind_id`; `set_property` uses the sidecar's property type and range. Numeric
methods remain valid when no api.json is loaded. Compiled bounds are initial
layout metadata, so re-check screenshots after runtime movement or animation.

### Visual regression

Before implementing a UI change, state its expected layout, font/glyphs,
colors, opacity, stacking and motion at relevant states in concrete terms.
After observing the requested runtime state, compare the rendered result to
those targets; a successful RPC, property match or changed screenshot alone
does not establish the intended appearance. Use independently authored pixel
or geometry assertions where practical, and keep expected colors consistent
with the selected format's quantization and screenshot expansion. Review
fonts and animation states explicitly when fixed-pixel assertions do not cover
them, and report any acceptance dimensions that remain unverified.

For text, preserve the intended font file and sizes and declare known future
characters with `font_charset` or `font_charset_file`. Review unintended
`GSPC-RS-FONT-CHARSET`, `GSPC-RS-FONT-MISSING` and `GSPC-RS-TEXT-MISSING`
warnings: compilation can succeed with missing or undeclared glyphs. Check
representative translated and longest runtime values in preview. For corpus,
budget and optional dynamic-font choices, follow the selected component's
`guide/media-and-data.md#raster-images-and-fonts` (Chinese: `#位图与字体`).

For images, check fit/crop, transparency and rotation at the intended box size;
image rotation is clipped to its box and differs from BSP display orientation.
Use `gspc svg-info` for vector source bounds, then follow the selected component's
`guide/media-and-data.md#compiled-svg-images` and `#explicit-static-svg-baking`
(Chinese: `#编译式-svg-图片` and `#显式静态-svg-烘焙`). SVG input on an Image defaults
to compiled vectors; explicit `svg_mode: "raster"` bakes supported static artwork
at fixed dimensions and changes which runtime operations remain available.
Check the execution plan's actual resource dimensions and decode estimates
alongside appearance; encoded bytes and initial visibility estimates do not
establish peak RAM use. See `guide/media-and-data.md#transparent-rasters-previews-and-resource-budgets`
(Chinese: `#透明位图预览与资源预算`).

For local edits with a known-good opaque PNG, the native scenario's `screenshot`
can take `reference` and `allow_changes` rectangles (`x/y/w/h`). Read the runner
reference for its negotiated capability and limits. The comparison records
unexpected pixel changes and a red/green diff image in the same report; the
reference stays unchanged. Fix profile, scene and application state, and allow
only the requested edit and known dynamic areas. Do not derive the allowed mask
from the actual diff or require a reference for a new design. Do not use a new
capture from the implementation under test as its own expected image. For
automated visual checks, include a negative control such as presenting the old
frame as the requested new state and confirm that the same assertion fails.

Compile-level `GSPC-TEXT-CLIPPED` information uses actual static glyph layout and
omitted lines. Review it before accepting a screenshot; keep intentional clipping
and ellipsis. It covers the initial text's own box, not future bound values,
parent clipping or other objects covering it. Unambiguous source objects receive
a range; generated text retains its compiled name for optional inspection.
Use [element explanations](project-context.md#explain-an-existing-element-when-needed)
to find the controlling layout/source before editing. Inspection geometry is
`initial_unclipped` or `template_definition`; it does not prove the final visible
or touchable region. Recheck moved, clipped and overlapping content at runtime.

For a reported visual regression, hold GSPC, simulator, target profile,
logical resolution, pixel format, scene, and frame count constant. Compare
against a known-good bundle or commit, or make one controlled property change
for A/B output.

### Verification scope

Report the checks performed for the change: compiler diagnostics, generated C
and application builds, simulator behavior, named-board execution, and physical
visual/touch observations. Identify missing assets, target facts or unavailable
capabilities that affect the result. Device timing and panel appearance require
their corresponding board measurements and observations.

## Assert behavior through runtime observation

```json
{"method":"wait_property","params":{"name":"brightness","property":"value","value":70,"max_frames":120}}
```

The expected value must match the sidecar type and range. A timeout, query
failure, reset or scene change fails/cancels the wait. The result identifies
`source: "runtime"`; text properties are outside this scalar API.

For application behavior, drive pointer input and observe the callback's
resulting state through the shared C Backend. Use named reads/waits, callback
notifications and screenshots together. Setting the desired UI value directly
does not prove that an application callback produced it.
