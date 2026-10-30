# Native C simulator bridge

`sim_bridge` builds portable C UI/application sources into a native Backend
shared library and an optional isolated Backend executable. It implements a
supported subset of the existing `esp_gsp.h` API. The default runner loads the
`.so`, `.dylib`, or `.dll` directly into sim_host; `--socket-backend` retains
the loopback TCP process boundary for isolation and transport testing.
Application callbacks, their `user_ctx` pointers and timers stay on one native
Backend thread; rendering and animations run in sim_host. Python builds and
supervises the session, but is not an RPC relay.

## Requirements and quick start

Use the installed ESP-GSP component, Python 3.10+, CMake 3.20+, and a native C11 compiler
(GCC/Clang on Linux, Apple Clang on macOS, or MSVC on Windows). On Windows,
run from the compiler's developer terminal. Install the recommended tool manager:

```sh
python -m pip install -U esp-gsp-tools
```

The component includes this bridge and public headers; GSPC and the simulator
are separate precompiled tools. The bridge calls `python -m gsp.execute` using
the same Python interpreter as the runner. The manager downloads, verifies
and caches the requested tools; implementation sources or additional runtime
build toolchains are not needed.

From an ESP-IDF application's root, after Component Manager has installed
`espressif/esp-gsp` (for example, after `idf.py reconfigure`):

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py --project pc
```

This assumes your application has the `pc/CMakeLists.txt` described below.
To try a bundled example without creating an adapter, run from the same root:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project managed_components/espressif__esp-gsp/examples/usage/hello_world/pc
```

Use `examples/performance/benchmark/pc` or `examples/usage/sim_bridge_media/pc`
for the other backends. For an unpacked component, use its `tools/sim_bridge/run.py` path
instead. Run from a writable application/work directory: default output is
`./build/sim_bridge/<project-parent>/<project-name>`, not inside the component.
The native build itself does not require an active ESP-IDF environment.

Tool selection is automatic and version-pinned:

- GSPC: `--gspc` → `GSPC_EXECUTABLE` → `esp-gsp-tools` using `.gspc_version`
  in the application working directory, then the selected component.
- Simulator: `--host` → `GSP_SIM_EXECUTABLE` (legacy alias:
  `GSP_SIM_HOST_EXECUTABLE`) → `esp-gsp-tools` using the selected component's
  `idf_component.yml` version. `--sim-version` / `GSP_SIM_VERSION` can override
  that version when using the manager, but do not override an explicit binary.
- Component: `--component-dir` → `ESP_GSP_COMPONENT_DIR` → the component
  containing this script. Your PC CMake project must honor this selection.

CLI arguments take precedence over the corresponding environment variables.
`GSP_SIM_EXECUTABLE` takes precedence over its legacy alias when both are set.
An application `.gspc_version` contains one exact version number on one line
(for example `0.2.8`), with no YAML key or version-range operator. Usually no
application override is necessary; keep the component's recommended version.

The simulator command is `python -m gsp.execute --version <component-version> sim`;
GSPC uses its separate compiler version, not the component version. No implicit
PATH or development-build fallback selects a different product version.
For offline/custom installations, point the executable overrides to downloaded
files (for example `C:/tools/gsp_sim.exe` on Windows), not command strings.
Relative executable paths are resolved from the invocation working directory,
not from `--project`. For example, with downloaded tools at absolute paths:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py --project pc \
  --gspc /absolute/path/to/gspc --host /absolute/path/to/gsp_sim
```

The manager also supports `GSP_TOOLS_NO_AUTO_DOWNLOAD=1` for cached-only use.
The simulator reports `dynamic_backend_loader_supported` separately from the
Backend ABI version. A build without shared-library loading automatically uses
the Socket Backend in the runner; an ABI version alone does not promise loading.
If tools are missing, install the manager into the runner's Python environment,
or obtain compatible binaries from [release downloads](https://github.com/espressif/esp-gsp/releases).
The simulator must report `dynamic_backend_abi_version: 1` and support
`--ready-file`; update the component and matching simulator together if that
capability is absent. The shared library embeds `GSP_VERSION_STRING`. sim_host
requires an exact match with its embedded runtime before starting application
code; `--sim-version` cannot be used to bypass that check.

### Automation sessions

Start an automation session with explicit loopback ports and API input control:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py --project pc --headless \
  --api-port 8266 --backend-port 8684 --input-mode api-exclusive
```

For a separate interactive preview, omit `--headless`, set `--visual-port`, and
use `--input-mode browser-exclusive`. Give each session distinct nonzero ports
and each concurrent build its own `--build-dir`. Port `0` allocates a free port.
`--visual-port` and `--headless` are mutually exclusive. Omitted options retain
the simulator defaults; `--api-port` enables the API channel and `--backend-port`
selects Socket mode.

The runner loads `api_json` from the build manifest to enable named queries,
inputs and callbacks. GSPC generates the sidecar with its Bundle, and the host
checks their identity. Manifests without the optional field use numeric APIs.

After CMake confirms the selected component, the runner queries simulator
`--capabilities` before compiling the Backend. An unchanged startup selection
needs only one capability query.
`build.py` remains build-only and does not require the simulator.
Invalid responses and timeouts stop startup. Version differences are reported
while allowing explicit tool overrides. The `Tool contract:` JSON log contains
the component version, simulator capabilities and `version_match` result.

The `Session:` JSON log reports endpoints, Bundle and sidecar paths with
`backend_loaded` for a library or `backend_spawned` and its PID for Socket mode. `API:` and
`Preview:` print their respective endpoints. Wait for the desired application
state or callback before capturing a screenshot. Use the application's render
fence to synchronize completed updates; a frame wait advances simulation time.
For bounded runs, `--duration <seconds>` waits for Backend completion and cleans
up the host process tree.

### Repeatable application checks

Pass `--scenario pc/check.json` to build and run one headless acceptance sequence:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project pc --scenario pc/check.json
```

For an application whose `increment` callback raises `level.value` from 0 to 10,
`pc/check.json` can contain:

```json
{
  "schema_version": 1,
  "timeout_seconds": 30,
  "steps": [
    {"action": "wait_property", "name": "level", "property": "value", "value": 0, "max_frames": 120},
    {"action": "screenshot", "file": "before.png"},
    {"action": "tap", "name": "increment", "callbacks": ["increment"]},
    {"action": "wait_property", "name": "level", "property": "value", "value": 10, "max_frames": 120},
    {"action": "screenshot", "file": "after.png"}
  ]
}
```

Choose names, property keys and expected values from your application's matching
`api_json`. Supported actions are `tap` by name or coordinates, coordinate `drag` with
`x1/y1/x2/y2`, `expect_property` for an immediate typed scalar assertion,
`wait_property`, `expect_visible` and `wait_visible` (to assert an object's
runtime `effective_visible` state across parent visibility, Drawer/PageFlow/StackView
viewports, and dynamic bounds when `effective_visible_version: 1`),
`wait_keyboard_text`, `wait_component`, `wait_scene`, `wait_callback`, and `screenshot`. Property values support
integer, boolean and native color scalars. A named tap uses runtime input
geometry when the matching host and sidecar support it; older artifacts can
fall back to compiled initial bounds. Inspect `target_verified` in the input
result, then verify its callback and resulting application state.
Current hosts try the initial input candidates first, then propose points from
observed runtime clip boundaries. `query_geometry.search` reports the point
count, up to 64 additional point probes and 64 boundary queries, and whether a
budget was exhausted. Every candidate still needs runtime hit verification;
`no_verified_point` does not establish that the entire object is covered.

`wait_callback` observes callbacks after the most recent scenario `tap` or
`drag`, including callbacks received before that input's response. Before each
input, a frame barrier drains earlier callbacks; the report records them in
`callback_boundary` (or `callbacks.before` for an exact tap probe). With no prior
input, the wait observes callbacks since subscription. This is a time boundary:
concurrent application timers can still emit the same callback. Pair the event
with the expected application state, or use a tap's exact `callbacks` assertion.

For component motion that must emit a new event after an input, add
`"require_new_event": true` to its `wait_component` step:

```json
{"action":"wait_component","name":"panel","value":1,"require_new_event":true,"max_frames":120}
```

Place it after the relevant `tap` or `drag` and before the next input. The runner
captures that component's existing event cursor immediately before the input,
then waits for both a newer matching event and the requested settled state.
This preserves synchronous events that arrive during input dispatch. An unchanged
initial value, an expired cursor, reset or scene change cannot satisfy the new
event requirement. It requires `component_event_cursor_version: 1`; without the
option, existing state-only waits remain valid. The report distinguishes
`observation.kind: "state_match"` from `"state_and_new_event"` and records the
input step and prior observation. These are temporal observations; concurrent
application work can also emit component events.
The component must exist in the scene before that input. A navigation tap cannot
capture the destination component's cursor: first use `wait_scene`, then operate
on that scene's component and verify its new event. A scene change between the
capture and wait cancels the observation.

For a form, `{"action":"wait_keyboard_text","text":"你好A","max_frames":120}`
waits for the attached keyboard's complete text. Empty text is valid; comparison
is exact UTF-8 text, without trimming or normalization. Each attempt advances a
frame before reading, so queued input can take effect. It uses the existing
read-only `keyboard_text` RPC and requires `bridge_keyboard_text_version: 1`
before any scenario input. A timeout keeps the last text, expected text and frame
count in the report. Nonzero result codes, including a truncated read, fail.
Attachment and writes remain in application C. This observes the keyboard edit
buffer; it does not read arbitrary labels or prove that Confirm saved the value.
Pair it with application state checks for confirm, cancel and return behavior.

To track intended acceptance checks, optionally declare them in the same file
and associate assertions with `check`:

```json
{
  "schema_version": 1,
  "checks": ["increment updates state"],
  "steps": [
    {"action":"tap","name":"increment","callbacks":["increment"]},
    {"action":"wait_property","name":"level","property":"value","value":10,
     "check":"increment updates state"}
  ]
}
```

Choose checks from the requested behavior and affected states. Names are unique,
nonempty strings of up to 160 characters; 1–64 checks are supported. A check passes
only when every associated assertion passes. The report's `check_coverage` lists
`pass`, `fail`, `not_run` (assertions not completed) and `unplanned` (no assertions
associated), with step indices and `unverified` names. An unverified declared
check makes the run fail even when all executed steps passed. An earlier error
or interruption also retains the remaining checks. Adding a cancellation check
without testing it therefore cannot silently report full acceptance.

Only assertions can carry `check`: property/text/component/scene/callback checks,
taps with exact `callbacks`, or screenshots with a `reference`. Input submission
and plain image capture alone do not establish acceptance. Declare only checks
that this run can verify; review aesthetic and device requirements separately.
These labels track declared assertions, not inferred requirements or exhaustive
product coverage. No labels are required for existing scenarios, and they add
no runtime observations. Older runners reject unsupported fields before building;
inspect the selected runner's help/reference before using them.

For overlap and click-through checks, tap the actual point and verify the
complete callback sequence:

```json
{"action":"tap","x":60,"y":40,"callbacks":["front"],"max_frames":6}
```

`callbacks: []` checks that no CALL event occurs in the observation window.
Each expectation can also match event fields, for example
`{"callback":"choose","arg":2,"item":5}`. Names are nonempty strings;
`arg`, `scene_id`, `list` and `item` are unsigned 32-bit integers. The observed
sequence must have exactly the expected length and order. Unexpected,
duplicate or unnamed callbacks fail the step. Without `callbacks`, existing
tap behavior is retained; `max_frames` on a tap requires `callbacks`.

A frame barrier separates earlier queued callbacks, which are retained as
`callbacks.before`. The check then observes the full `max_frames` window
(default 6, range 1–36000), even after an expected callback arrives. Up to 64
expectations/events are supported; event loss or overflow fails verification.
The report records the input, expected/observed callbacks and original event
fields. Runtime timers can also produce callbacks during this window.

Use screenshots and geometry to choose overlap centers, exposed edges, modal
backgrounds and clipped areas. After a real show/hide, move or close action,
test the changed points again. Read application properties and active scene as
well: a control can change state through declarative actions without a CALL.
These bounded observations do not prove the absence of arbitrarily delayed
application work. Coordinate taps with an empty callback expectation also work
without named objects. Named callback expectations require a matching API sidecar.

Scenario mode selects API-exclusive input, headless rendering and an available
API port. It observes state written by the real C Backend. Tool versions and
required capabilities must match before input is accepted. Invalid action names,
fields and screenshot paths fail before the build. Scenarios contain 1–64 steps;
`timeout_seconds` bounds the complete execution after connection (default 30,
maximum 300), while `--startup-timeout` still covers tool startup. Property waits
accept `max_frames` from 1 to 36000. `--duration` cannot be combined with a scenario.

After input that navigates, use `{"action":"wait_scene","scene":1}` before
resolving destination controls. This step advances frames and observes the
runtime's active scene; it never changes scenes itself. Its optional `max_frames`
defaults to 300 (1–36000), and a timeout retains the last observed scene ID.
With `runtime_input_query_version: 1`, it also waits for `capabilities.input_ready`,
so a destination is not reported ready while a transition owns input. Older
modules without this capability confirm the scene ID alone.
For Drawer/PageFlow motion, use
`{"action":"wait_component","name":"panel","value":1,"max_frames":120}`.
It advances an input barrier frame, then uses the existing read-only motion wait;
success requires an idle component and, when supplied, the expected value.
Values are integers in 0–65535. `value` is optional for an idle-only check;
after input, specify the intended value as well. The host must advertise
`wait_component_version: 1`; timeout/scene-change errors retain runtime evidence.
`wait_scene` is a scenario step, not an RPC method. Custom clients can observe
`scene_changed` or `capabilities.current_scene`, then check `capabilities.input_ready`. `run.py --help` lists supported
scenario steps for the selected component.
Scene waits, coordinate drags and screenshots also work for scenes containing
only anonymous objects; add names only when the application or a named step needs them.

For a local design edit, a screenshot can optionally compare against a known-good
opaque PNG from the same profile, scene and application state:

```json
{"action":"screenshot","file":"after.png","reference":"baseline.png",
 "allow_changes":[{"x":16,"y":80,"w":208,"h":20}]}
```

Reference paths are relative to the scenario file. Regions come from the intended
edit or explicitly excluded dynamic areas, rather than the observed differences.
Outside those regions, pixels must match exactly; omitting regions checks the
whole image. The host advertises `screenshot_compare_version: 1` and uses its
existing PNG codec, without a Python imaging dependency. The report includes
changed/unexpected pixel counts, unexpected bounds, the reference hash and a
`after.diff.png` image: green means allowed changes, red means unexpected changes.
Both images come from one captured frame. Unexpected changes fail the scenario.
The reference is preserved and comparison outputs must be new. Inputs are limited
to static opaque PNGs of matching dimensions, at most 32 MiB and 4,194,304 pixels;
up to 32 in-bounds regions are supported, and a mask covering the whole image is
rejected.

Each run prints its `report.json` path under a unique `scenario-runs` directory
in the build tree. The report records the tool contract, generated context and
Bundle identity when present, actual step results and screenshot hashes. A
failed assertion stops the sequence, preserves observed values/RPC errors, writes
a failing report and exits nonzero. Ctrl-C records an interrupted execution.
Build/startup errors use the runner's usual diagnostics; execution reports begin
once the API session connects. Use `--report-dir <new-directory>` to select a
location explicitly; existing reports and screenshots are preserved. Screenshot
filenames must be unique PNG basenames. A pass establishes the recorded host
behavior; visual quality and device acceptance remain separate checks.

After an execution failure, `failure_evidence` records a best-effort screenshot,
current scene and named-object metadata before the session closes. These
read-only RPCs share an additional two-second budget and do not request frame
steps or inputs. Timers may continue while evidence is collected. Screenshot
names are allocated uniquely, including when the plan already uses `failure.png`.
An unavailable host or capture error is recorded without replacing the original
failure or nonzero exit. Object bounds remain compiled-layout metadata. Ctrl-C
skips this capture and proceeds to shutdown.

### Keyboard text readback

The bridge implements `esp_gsp_keyboard_text_size()` and
`esp_gsp_keyboard_text()` for reading the keyboard's current UTF-8 text, and
`esp_gsp_keyboard_attach_ex()` for selecting its byte capacity. Readback preserves
the device contract: required size includes NUL, and a small buffer returns
`ESP_GSP_ERR_INVALID_SIZE` with a complete UTF-8 prefix. The simulator's
`bridge_keyboard_text_version` capability identifies this support; an older
simulator returns `ESP_GSP_ERR_NOT_SUPPORTED` through the bridge.

### Dynamic font at startup

Changing text within the compiled character set uses GFB glyphs. Supply a raw
font only when additional characters need FreeType fallback; a CFF source
already baked into GFB does not require runtime CFF support for those glyphs.

Pass the font to the host before UI creation. On devices, load it with
`esp_gsp_font_file_open()` and apply it to `esp_gsp_config_t` before
`esp_gsp_start()`:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project pc --dynamic-font assets/NotoSansSC-Regular.ttf
```

`--dynamic-font` takes precedence over `GSP_SIM_DYNAMIC_FONT`. The runner
resolves the path and requires a regular file before it builds or launches the
host, then passes its absolute path to sim_host's `--dynamic-font`. The font is
therefore installed while sim_host loads the bundle, before the native Backend
gets its initialization callback. This option is intentionally a launch-time
configuration: it cannot replace a font in an already running bridge UI.

Use a scalable TrueType or CFF font in a `.ttf` or `.otf` container, or as the
first face of a `.ttc` collection. Empty files are rejected before the native
build and checked again by sim_host before runtime startup. The 1.8.0 WASI
runtime includes CFF; CLI/RPC `font_outline_drivers` reports registered TrueType/CFF drivers.
Older runtimes may lack CFF or report `null` when driver introspection is
unavailable. The extension alone does not identify the outline type. Face setup
failures report the detected container and FreeType error code; a missing
CFF driver is reported explicitly. Failed optional font setup leaves compiled
GFB glyphs available and disables the dynamic fallback.

For a `gspc font-link` catalog, pass `--font-catalog /path/to/fonts.gspb` or
set `GSP_FONT_CATALOG`; the CLI option wins. It is independent of
`--dynamic-font`, so both may be supplied. The runner validates the path before
building and the host checks the catalog and CRC before creating the UI.
Use a host whose capabilities report `font_catalog_version: 1`; rebuilding
the Backend alone does not update its embedded simulator runtime. Catalogs
resolve missing GFB assets by ID. They do not add codepoint fallback to an
unlinked bundle; resolved GFB glyphs take priority over dynamic outline-font fallback.
The Backend's UI is already open, so `esp_gsp_font_file_open/apply/close` are
not bridge APIs. Configure both font sources at host startup instead.

The runner compiles a deployable bundle and its C headers together, builds the
shared library plus Socket executable, and starts sim_host with the library by
default. Pass `--socket-backend` to start the executable and connect it to the
host's loopback endpoint instead. It opens the preview URL unless `--headless`
or `--no-browser` is used. A normal simulator exit (including API `quit`) ends
the Socket session successfully and cleans up its Backend. Application failures
while the simulator remains running and nonzero simulator exits still fail.
Ctrl-C, Backend exit, host failure or startup timeout
stops the processes it owns, including the manager's simulator child. POSIX
termination permits application cleanup; Windows process termination may be
immediate. Cleanup callbacks are not guaranteed after a crash or forced stop.
`--startup-timeout` defaults to 180 seconds and includes initial simulator
download; increase it on slow connections. The browser tab is not closed by
the runner. In shared-library mode, `--duration` starts after host readiness;
readiness does not guarantee that application initialization has completed.

Use `--build-dir` to isolate concurrent builds, `--config Debug` to change
configuration, and `--generator Ninja` to select a CMake generator. A build
lock guards each directory; after a crash, inspect its PID before manually
removing a stale `.sim_bridge_build.lock`. Do not rebuild a running backend's
assets in place: use a separate build directory for that preview.

## Add your own application

Keep display/touch initialization, FreeRTOS tasks, drivers and device
services outside portable UI sources. Replace those dependencies through a
small application HAL/PC mock. Do not link the device GSP runtime or include
its private headers in the Backend.

For example, place this in your PC adapter project's `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_ui_backend LANGUAGES C)
if(NOT ESP_GSP_COMPONENT_DIR)
    if(DEFINED ENV{ESP_GSP_COMPONENT_DIR})
        set(ESP_GSP_COMPONENT_DIR "$ENV{ESP_GSP_COMPONENT_DIR}")
    else()
        set(ESP_GSP_COMPONENT_DIR
            "${CMAKE_CURRENT_LIST_DIR}/../managed_components/espressif__esp-gsp")
    endif()
endif()
add_subdirectory("${ESP_GSP_COMPONENT_DIR}/tools/sim_bridge" sim_bridge)
gsp_add_backend(my_backend
    SOURCES ../main/ui_logic.c platform_pc.c
    INCLUDE_DIRECTORIES ../main
    SCENES ../scenes/main.json)
```

`SCENES` order defines scene IDs. The optional `PROFILE` selects a gspc YAML
profile; the default is the bridge's included `profiles/rgb565.yaml`. Keep native color
values consistent with that profile. Generated `bundle_gsp.h` and scene
headers are on the target include path. `gsp_add_backend` always packs with
`--deployable`: sim_host needs the component directories inside the bundle,
not just the headers compiled into your Backend.

For shared project components, add `COMPONENTS_DIR ../ui/components` to the call
and use the same directory in the device's `gsp_add_bundle()`. Paths resolve
relative to the calling `CMakeLists.txt`; `Card.json` defines the component
`Card`. Definition edits and directory membership changes rebuild the Bundle,
and its generated compilation context retains these inputs. Scene-level inline
definitions remain available. See [reusable project components](../../docs/en/guide/scenes.md#reusable-project-components)
for substitution, naming and asset-path rules.

Only one application-facing bundle header is generated per target. Reuse
that same generation for all its business sources; do not mix old device
headers with a freshly compiled PC bundle. Incremental builds track JSON,
Profile and referenced assets, regenerate missing required headers, and reuse
unchanged outputs.
With Ninja, ordinary color, text and layout edits rebuild the Bundle without
rerunning project configuration. Scene renames still refresh generated-header
rules. Other generators retain their existing configure dependencies.

Implement the PC lifecycle adapter:

```c
#include "gsp_sim_bridge.h"
#include "ui_logic.h"

esp_gsp_err_t gsp_bridge_app_init(esp_gsp_handle_t ui)
{
    /* Device app_main calls the same ui_logic_init after hardware startup. */
    return ui_logic_init(ui);
}

void gsp_bridge_app_deinit(esp_gsp_handle_t ui)
{
    ui_logic_deinit(ui); /* Also safe if init only partially succeeded. */
}
```

Do not call `gsp_bundle_config()` or start an ESP-LCD renderer in this adapter.
Both supplied entry points (the shared library and the Socket executable)
initialize the application, then pump callbacks and timers until exit.
`gsp_bridge_app_init()` must return; a blocking application loop inside it
would prevent event dispatch. Applications using these entry points do not
need their own polling loop. For a custom entry point, link
`esp_gsp::sim_bridge` directly and use `gsp_sim_bridge_open/poll/close` instead;
the custom entry point takes ownership of the event loop described below.
Call close only after leaving callbacks/poll, never from inside a callback.

Build without launching, or invoke CMake directly:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/build.py --project pc

cmake -S pc -B build/hello-pc -DCMAKE_BUILD_TYPE=Release
cmake --build build/hello-pc --config Release
```

Direct CMake accepts `GSPC_EXECUTABLE` as a CMake variable or environment
variable, otherwise uses the same tool-manager resolver. It selects
`.gspc_version` from `PROJECT_DIR` (if set), otherwise the CMake source
directory, then the component. Use `-DPROJECT_DIR=/path/to/application` when
you want the same application-root override as the Python runner.
Set `-DESP_GSP_PYTHON_EXECUTABLE=/path/to/python` if CMake should use a
particular Python environment. Do not edit files in `managed_components`.

CMake produces `<target>_backend.dll`, `lib<target>_backend.dylib`, or
`lib<target>_backend.so` together with the existing executable. It writes a
version-2 `<target>-<configuration>.json` manifest containing
`backend_library`, `executable`, and `bundle`; this works with both single- and
multi-configuration generators. For multiple backends in one CMake project,
pass `--target` to the Python scripts.

## Custom entry point: keep the event loop running

After opening the bridge and initializing the application, call
`gsp_sim_bridge_poll()` repeatedly on the same Backend thread, **even when
there is no user input, no new command, or no visible screen change**.
`gsp_sim_bridge_open()` does not create a background event dispatcher.
Unlike the device's GSP task, a custom PC entry point depends on its caller
to keep callbacks and timers running. The default entry points already do this.

Do not block that thread in `fgets()`, a blocking socket/queue read, or a long
sleep while waiting for control input. Check input without blocking and process
only a bounded batch before polling again. A partially received line must not
block the loop while waiting for its newline. Alternatively, let a worker read
input and enqueue commands; only the Backend thread may execute their GSP calls.

For example, the loop after successful bridge/app initialization can be:

```c
/* app_* are application helpers, not bridge APIs. Both must return promptly. */
while (rc == ESP_GSP_OK && !app_should_stop()) {
    app_drain_commands_nonblocking(ui, 16); /* At most 16 ready commands. */
    rc = gsp_sim_bridge_poll(ui, 20);
}
/* Then perform teardown on this thread; preserve contexts through close. */
```

The timeout is an idle wait, shortened for due timers or queued work; it is not
a fixed sleep or a limit on callback execution time. Keep initialization,
callbacks and command handlers short. Blocking a UI callback can stall progress
on the device too. Do not add a second polling thread while leaving GSP calls
on the first thread: that violates the bridge's single-thread contract.

If a control responds only after another click or screenshot request, inspect
the Backend loop first. The host may keep rendering animations while C callbacks
remain undelivered. An input RPC acknowledgment or a running host is not proof
that application callbacks have executed. Extra screenshots or repeated clicks
are not a substitute for polling. Regression tests should send a single tap,
then observe its application result without sending another input, wait or
capture command to wake the Backend. Also check timer progress during idle input
and while a command is only partially received.

## Execution and compatibility contract

There is one Backend connection and **one calling thread** within that
Backend. Call all bridge/GSP functions on this thread. UI callbacks and
periodic timers execute serially when `gsp_sim_bridge_poll()` runs. A worker
thread may perform mock I/O, but must hand results to application-owned
queues for the UI thread to consume. This is narrower than the device API's
“callable from any task” guarantee.

Synchronous bridge calls keep receiving responses and queue UI notifications;
they never recursively invoke business callbacks. In shared-library mode the
same command contract uses in-process queues; in Socket mode it uses framed
JSON-RPC over loopback TCP. The host services commands while waiting for the
next frame, so accepted state changes are not quantized to one RPC per frame.
Callbacks may call supported GSP APIs synchronously. Keep them short. Timers
use monotonic host time and coalesce missed ticks rather than replaying a burst.
Host headless Backend runs are paced to wall clock as well.

The shared library is trusted native code. sim_host validates the query symbol,
ABI number, exact descriptor size, embedded GSP version, reserved fields, and
run entry before registering it as the Backend. Those checks defend against
stale or accidentally incompatible build products; they cannot sandbox a
malicious library or recover from its memory corruption, crash, or deadlock.
Use `--socket-backend` when process isolation is more important than local-call
latency or when diagnosing transport behavior.

`gsp_sim_bridge_current_scene()` exposes the scene from the initial handshake
and subsequent notifications. No synthetic initial scene-change callback is
sent: initialize the first scene explicitly. Host readiness means listeners
are bound, not that business initialization has completed. The host may
advance frames as soon as the Backend connects.

| C API group | Version 1 |
| --- | --- |
| Bind value/color/visibility/text writes | Existing RPC methods |
| Bind value/color/visibility/toggle reads | Host scalar extension |
| Component info; value/color/visible/checked/enabled reads and writes | Supported; integer writes reuse `set_component_i32` |
| Component text; scalar property get/set; RGB888 color helpers | Supported; original tagged scalar types retained |
| Atomic component position setter | Supported with `bridge_api_version: 3` |
| Property metadata by semantic key or index | Supported with `bridge_api_version: 4`; signed 64-bit bounds preserved |
| Named image scale get/set | Uses the existing unsigned Q16.16 scalar property operations |
| Bind/component value/color animations; scalar property animations and `*_to`; bounded property play/stop | Executed by GSP inside sim_host |
| Press-feedback policy and generated effective-visibility queries | Supported with `bridge_api_version: 2`; visibility descriptors are copied over the Backend boundary |
| Events and timer create/delete | Native callback/context storage and event loop |
| Scene navigation, swipe policy, keyboard attach/cursor, Drawer, PageFlow set/read/drag-state | Existing RPC methods and scalar bridge queries |
| Input interceptor | Non-null registration returns `ESP_GSP_ERR_NOT_SUPPORTED`; `bridge_input_interceptor_version: 0` |
| Pointer observer | Receives host pointer samples during `gsp_sim_bridge_poll()`; see the callback timing note below |
| Dynamic List/Grid binders and row text/value/color | Native callbacks, asynchronous host row requests and token-checked publication; requires media version 1 |
| Bind/component/row/Grid encoded images | COPY upload of PNG/JPEG/QOI; requires media version 1 |
| Runtime template widgets | Create/destroy, value/color/text, visibility/position, value animation and encoded RESOURCE images; requires widget version 1 |
| External `esp_gsp_assets` package images | Single-frame PNG/JPEG/QOI package members for Image, Row and Widget targets; see the limitations below |
| Bind/row/Grid image EX advanced options and Widget borrowed/owned helpers | Ownership, lifecycle callbacks and cache keys; requires image version 1 |
| Canvas push/push-dirty, draw callback, invalidate/invalidate-dirty, stop | Full-frame upload; draw callbacks use the PC offscreen adaptation below; requires media version 1 |
| Canvas try-push/try-push-dirty | Eight-frame local queue, deferred upload and release; requires media version 1 |
| Flush | Host render-attempt fence after local Canvas work has been polled; requires fence version 1 |
| Generic enum-based component get/set, arbitrary batches, position reads/relative moves, input/render observers, device startup | Not implemented by this library |

`bridge_api_version: 2` is required for the RGB888 helpers,
`esp_gsp_component_play_animation()` / `stop_animation()`, press-feedback
policy, and `esp_gsp_query_visibility()`. Older version-1 hosts continue to
run the prior subset; these newer calls return `ESP_GSP_ERR_NOT_SUPPORTED`
without sending an unknown RPC.

`esp_gsp_component_set_position()` requires `bridge_api_version: 3`. Both
coordinates are validated and submitted as one core operation. Older hosts
return `ESP_GSP_ERR_NOT_SUPPORTED` without sending an unknown operation.

`esp_gsp_component_get_property_info()` and `get_property_info_at()` require
`bridge_api_version: 4`. Older hosts return `ESP_GSP_ERR_NOT_SUPPORTED` and
leave the output unchanged. `esp_gsp_image_get_scale()` and `set_scale()` use
the original property operations, so they also work with older hosts that
expose the image's scale property.

Only exported implementations are linkable. Unsupported public C functions
produce a link error, not a success stub. The shared headers describe the
full device API, **not** a promise that every declaration is implemented by
this bridge. Extend the mapping when your application's next API requires
it through the published compatibility API; native pointers cannot be sent
to another process.

Pointer-observer callbacks are delivered by the native bridge event loop after
the host has accepted a pointer sample. They are suitable for PC preview
diagnostics and input-driven application logic, but do not reproduce the
device's synchronous "immediately before gesture routing" timing. Registering
a non-NULL observer requires `capabilities.bridge_pointer_version: 1`.

Successful state/animation calls mean GSP accepted the operation, not that
the next frame has rendered. Readbacks query committed runtime state, so a
write/read in the same frame need not return the new value. Existing RPCs
may collapse runtime errors to `-1`; new scalar operations preserve GSP
error codes. Transport failures return failure and invalidate the connection;
RPC method errors are reported on stderr and do not themselves disconnect.
Ordinary RPC calls have a 5-second reply deadline, a 1 MiB incoming JSON limit, and a
256-event queue. Overflow is fatal rather than silently dropping UI events.
No automatic reconnect/reset replay is provided; restart the runner to
recreate native state and the matching simulator session.

For named observation from an automation client, load the manifest's
`api_json` and check the host's `named_property_read_version` and
`wait_property_version`. `get_property` reads committed scalar state;
`wait_property` waits for the expected value after a frame step. Both remain
available when the Backend owns writes. Use actual input and application C
callbacks to produce behavior, then read/wait and capture the resulting view.
Named properties use authored ranges and boolean values rather than internal
state rails. Current API sidecars carry their conversion codec. Older sidecars
can use the deployable runtime's property directory; if an ordinary Bundle's
encoding is ambiguous, regenerate its sidecar with the selected current GSPC.

With a compiler advertising `compiler_features.compile_context: 1`, the build
manifest and runner Session also report `context`. This generated snapshot
records the selected scenes, effective Profile and capacities; reuse it with
`gspc diagnose --context <path>`. The owning CMake build tracks scene, Profile,
font and image dependencies through GSPC's depfile, so unchanged builds reuse
the Bundle and generated headers. Compatible older tools omit the optional
context field.

### Required update groups

Include `esp_gsp_updates.h` to submit a group with an explicit completion target.
Keep polling, then query the receipt or consume its terminal callback; only
`ESP_GSP_UPDATE_COMPLETE` confirms the requested goal. Release the caller's
receipt reference after use. Simulator receipts reach `RENDERED`; query
capabilities before choosing a stage. Hardware display confirmation is provided
by device backends. See [target-frame receipts](../../docs/en/guide/update-receipts.md)
for deadlines, cancellation and input ownership. `esp_gsp_flush()` remains a
queue/render-attempt barrier.

### Dynamic data and images

These mappings additionally require `capabilities.bridge_media_version: 1`.
A host with only `bridge_version: 1` can still run scalar-only applications;
media operations are rejected as unsupported (binders return the NONE
handle). Use matching host and bridge builds for the media example.
If the selected simulator lacks a required capability, use a matching released
component/simulator pair. The application build does not rebuild those tools.

List and Grid bind callbacks run on the Backend calling thread during poll,
not on the host render task. They may synchronously call row/image APIs;
incoming notifications are queued until the callback returns. Bind each
component once, then use `*_set_total` / `*_refresh` as data changes. Handles
persist across scene navigation, but row tokens may expire when recycled.
A valid token can publish multiple fields or multiple updates; it is not
consumed by the first update. Do not retry an expired token. Binder failures
are logged; they cannot be returned to the render task retroactively.

Row publication and List total/refresh operations preserve the corresponding
ESP-GSP API error codes. Row text/value/color setters can return
`ESP_GSP_ERR_INVALID_ARG` for stale tokens or invalid arguments; automatic slot
lookup can return `ESP_GSP_ERR_NOT_FOUND`. Check the returned code instead of
treating every failure as a recycled row. Image upload acceptance remains separate from
`image_complete`, which reports the result of applying the queued image.

Author a List/Wheel that actually has a runtime row template: a short static
list that fits its viewport may compile without one and cannot be bound as a
dynamic list. Grid template images need `"dynamic_image": true`, and text
needs a text binding. `esp_gsp_grid_cell_t` receives the host's actual
`resource_slot` / `text_slot` (65535 means absent). `esp_gsp_row_text/value/color`
select the first matching template slot; explicit `row_set_*` uses its slot.
The media example shows both authoring requirements.

Host row-request overflow (`list_bind_overflow`) or a full local 256-row
queue fails the session; restart rather than silently losing data requests.
The bridge does not auto-rebind or replay requests to recover from overflow.

Encoded image uploads are limited to 16 MiB each. Successful submission means
the host accepted its copy, not that decoding or rendering has completed.
Component-key image updates require a compiled
component directory entry with a RESOURCE property; use the authored bind
when the compiler does not retain such a component.

`esp_gsp_assets_open()` is also available for PC adapters. It validates the
external GSPB and its nested GRB CRCs, then `show` / `show_name` unwrap one
single-frame PNG, JPEG or QOI resource and sends it through the normal COPY
image channel. The operation completes synchronously in the bridge, so status
is immediately READY or FAILED and `stop` only records STOPPED while retaining
the host's last picture. The reader does not implement package animations,
delta patches, JPEG+A8, or raw/RLE/vector resources; each is
reported as `ESP_GSP_ERR_NOT_SUPPORTED` rather than being treated as a
successful image upload. `close` is immediate because no source buffer is
retained after the host copies it.

### Image ownership and terminal results

`capabilities.bridge_image_version: 1` adds `set_image_ex`,
`row_set_image_ex`, `grid_cell_set_image_ex` with all three ownership modes,
release/completion callbacks and `cache_key`. The bind/row/Grid
`*_image_borrowed` and `*_image_owned` helpers are also implemented. Without
this extension, ordinary COPY calls still work; advanced options return
`ESP_GSP_ERR_NOT_SUPPORTED` before submission. Invalid options return
`ESP_GSP_ERR_INVALID_ARG`: BORROW requires `on_release`; COPY forbids it.
Plain COPY through `*_image_ex` also works with media version 1 alone:
options may be NULL, or COPY with no callbacks and `cache_key = 0`.

The transport always copies bytes into the host process. BORROW/TAKE are
native **lifetime compatibility**, not cross-process zero-copy: after
successful submission, the native library retains the source until the
host's actual GSP image-input release notification, including cache/source
retention after publication. BORROW calls `on_release` once. TAKE uses that
callback if provided, otherwise `free(data)`; use malloc-compatible storage.
Immediate submission failure transfers no ownership and fires neither
callback. COPY lets the caller reuse data when submission returns.

`binary_result` only acknowledges acceptance. The separate `image_complete`
notification carries the GSP runtime's terminal status: `GSP_OK` means image
publication, not display presentation; errors and `GSP_ERR_CANCELLED` are
reported too. `image_release` marks the end of encoded-input retention, not
the end of decoding. Nonzero cache keys are forwarded to GSP unchanged:
reuse a key only for identical immutable encoded content and change it when
content changes. They do not suppress transport uploads.

External-package images expose the same `esp_gsp_assets_get_publication()`
query as the device. `has_frame` becomes true after a successful
`image_complete`, and a replacement request, stop or scene change clears it.
`request_id` identifies the current request while an older image may remain
visible. Closing a package safely detaches uploads whose callbacks are still
pending. Hosts without `bridge_image_version: 1` retain package uploads;
the publication query returns `ESP_GSP_ERR_NOT_SUPPORTED` with a zeroed output.

Callbacks stay on the calling thread and are dispatched by poll, never
recursively while waiting for a binary reply. Completion callbacks may call
supported GSP APIs; keep them and release callbacks short. At most 128 tracked
images may remain retained (published images count too). Admission is rejected
at the limit, with no ownership transfer. Continue polling to drain releases;
do not spin-retry submissions inside one callback. Lost lifecycle events are
fatal rather than silently losing a release.

The native completion callback precedes the release callback for the same
request. Keep `complete_ctx` alive through completion and `release_ctx`
through release; if they share one object, retain it until release finishes.
COPY with only a completion callback needs its context only through that
callback, not until the host later releases its independent input. The options
structure itself is copied by submission and may be stack-local.

On orderly local shutdown, an accepted request without an observed completion
receives local `GSP_ERR_CANCELLED`; after transport failure it receives
`GSP_ERR_IO`, since the host's final outcome is unknown. Already received
terminal results are preserved. Retained native inputs are then released
once; the host keeps its independent copy. These local shutdown results do
not assert that the host rolled back an image it may already have published.
The default main settles callbacks **before** `gsp_bridge_app_deinit`, with
GSP mutations disabled during that settlement and advanced image submission
disabled thereafter. For a custom entry point, keep callback contexts alive
through `gsp_sim_bridge_close`, then free application state. Never call close
inside a callback. Calls from poll/draw are ignored with a diagnostic;
recursive close during shutdown is also ignored.
Forced process termination cannot guarantee callbacks or orderly cleanup.

### Canvas offscreen adaptation

`esp_gsp_canvas_set_draw_cb` allocates a complete native RGB565/RGB888 buffer
matching the opaque authored target. At the next poll, it calls the draw
callback with `(x, y) = (0, 0)` and the full width/height, then uploads that
buffer. Invalidation requests coalesce. `invalidate_dirty` validates the
rectangle but still redraws and uploads the full frame. Callbacks must draw
the full supplied surface; device render-task timing and tile partitioning
are not reproduced. This is a UI logic preview, not a Canvas performance test.

> **Compatibility limitation:**
> Do not use this PC path to validate device Canvas tiled Direct Draw behavior
> or performance.

The target is an image resource with an authored bind, not a new JSON widget
type. In the [media scene](../../examples/usage/sim_bridge_media/scenes/media.json),
the `surface` image uses `"codec": "lossless"` and `"bind": "surface"` with an
opaque source. The build profile chooses RGB565/RGB888; the generated header
provides `GSP_BIND_SURFACE` for registration. The callback's dimensions are
the runtime resource dimensions, which need not equal a scaled widget's
layout rectangle. Alpha/non-native-format resources are rejected. The same
scene also shows the Grid text `bind_target: "text"` and `dynamic_image` syntax.

Draw callbacks may query supported read-only GSP state but cannot mutate it:
writes, invalidation, producer replacement and timer changes are rejected
(`ESP_GSP_ERR_INVALID_STATE`, or NULL/NONE for handle-returning APIs).
Keep the callback short and do not close the bridge inside any callback.
Scene-change notifications disable all local draw callbacks; register them
again when entering a scene. A successful explicit push also disables the
draw callback for that target. Successful `canvas_stop` unregisters the local
callback immediately; restoring the authored image on the host is queued.

`canvas_push` / `canvas_push_dirty` synchronously consume a full native source
frame. With `bridge_canvas_patch_version: 1`, the first update and large dirty
regions upload a full keyframe; later dirty regions covering at most roughly
75% of the frame are packed and transferred row-by-row. The host reconstructs
the complete backing frame, and the bridge automatically falls back to a
keyframe after reset, scene changes, stop, or cache mismatch. Height must match the target, stride
must cover a full row, and dirty bounds must stay inside the canvas. A scene
tag rejects uploads prepared for another scene, and the queued host command
checks scene and buffer size again before use. Total payload is at most
16 MiB. On success, the supplied release callback runs **once before the
push function returns**, because the host owns a copy. On failure it is not
called; ownership stays with the caller. This is earlier than device-side
frame release and does not imply presentation.

### Nonblocking Canvas producers and synchronization

`canvas_try_push` / `canvas_try_push_dirty` perform **no socket I/O**. Success
means admission into an eight-frame local FIFO, not host acceptance. The
library retains the supplied pointer without copying; keep its full-frame
buffer immutable and valid until release. A NULL release callback is allowed,
but gives no per-frame lifetime signal: use it only for storage kept alive
and immutable through bridge close, or supply a callback for reusable buffers.
Queue-full returns
`ESP_GSP_ERR_TIMEOUT` without taking ownership or calling release. Basic
argument errors are rejected immediately; target dimensions, stride and dirty
bounds are checked during upload. Dirty pushes still require the entire native
source frame to remain valid, although their wire payload can be much smaller.
An accepted push disables the target's local draw callback immediately.

Poll uploads queued frames; synchronous RPC-backed GSP calls first drain
earlier frames to preserve ordering (for example, push before stop). Upload
rejection, including a scene mismatch, is logged and still releases an
accepted frame once. There is no asynchronous per-frame result callback;
transport failure also makes poll fail. Frames carry the locally observed
scene ID: after navigation, wait for that scene's notification before queuing
its frames. A release callback means only that the native source is no longer
needed, not that a frame was accepted or displayed.

Accepted frames are released exactly once after upload, rejection or local
shutdown. Queued release callbacks may run during poll, before a synchronous
RPC starts, or during cleanup; they must not mutate GSP, poll or close the
bridge. Read-only queries are permitted while connected. The default main
cancels pending frames before app deinit, then rejects further try-pushes.
A custom main must keep buffers and contexts alive through bridge close.
Forced termination cannot guarantee release callbacks.

`esp_gsp_flush` requires `capabilities.bridge_fence_version: 1` (otherwise
`ESP_GSP_ERR_NOT_SUPPORTED`). It sends a Backend-only `render_fence` and waits
until earlier submitted commands have reached a subsequent host step/render
attempt. An idle step also completes the fence. This is **not** an animation,
image-decode/publication or browser-presentation completion signal; use image
completion callbacks for publication. Host render failure cannot report a
successful fence.

First poll to finish local queued Canvas uploads and offscreen redraws. If
either remains pending, flush returns `ESP_GSP_ERR_INVALID_STATE` without
submitting a fence. Flush never runs application callbacks or timers; incoming
notifications wait for poll. It is also forbidden from draw/release callbacks.
Use this explicit PC boundary for tests and captures, not on every update:

```c
/* Called on the business thread, outside draw/release callbacks. */
esp_gsp_err_t rc = esp_gsp_canvas_try_push(ui, bind, pixels, stride, release, ctx);
if (rc == ESP_GSP_OK) {
    rc = gsp_sim_bridge_poll(ui, 0); /* upload; may release pixels here */
    if (rc == ESP_GSP_OK) rc = esp_gsp_flush(ui, 1000);
}
/* On failed try-push, pixels still belong to the caller. A later flush
 * success is not proof that a locally admitted frame passed validation. */
```

Poll can itself invoke producers; if these leave new queued work, poll again
before flushing, or pause the producer for a deterministic boundary. Poll's
timeout is only an idle wait: uploads, RPCs and callbacks can take longer.
There is no separate pending-work query: handle INVALID_STATE at the flush
boundary by stopping producers and polling their remaining work, not by
blindly retrying flush. For a test that needs upload acceptance, use synchronous
`canvas_push` and check its result, then flush and capture through the API
channel using [screenshot](../../docs/en/reference/simulator.md). Assert the
expected screenshot pixels: neither release nor a fence
alone is an assertion about image contents. For encoded images, also wait for
successful image completion before the fence. Browser presentation has no
acknowledgement in this API.

Flush's finite timeout instead covers its transport submission and reply wait;
`UINT32_MAX` waits indefinitely. Zero admits a fence locally without I/O and
returns `ESP_GSP_ERR_TIMEOUT`. A timeout does not cancel the fence or poison
the connection: later poll/RPC/flush resumes partial traffic and consumes late
replies. At most 16 native fences can be outstanding; a full table returns
TIMEOUT without adding another. Continue polling rather than spinning on
flush(0). Closing discards outstanding fences without claiming completion.

## Examples and application validation

- `hello_world/main/hello_ui.c` is shared by ESP-IDF and the PC adapter.
- `benchmark/main/bench_workload.c` was already hardware-independent and is
  compiled unchanged into the PC target. This reproduces its tween workload,
  **not** the full board benchmark, Canvas/media producers, page cycling,
  dynamic list fixtures or timing measurements.
- `sim_bridge_media` provides portable C List/Grid binders, QOI images and
  an offscreen Canvas draw callback; see its [README](../../examples/usage/sim_bridge_media/README.md).
- Widget examples provide scene JSON previews rather than native
  business backends. Use `hello_world`, `benchmark` or `sim_bridge_media` as a
  starting point when adapting C application logic to the bridge.

Run a bounded smoke test of the published media example from your application root:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project managed_components/espressif__esp-gsp/examples/usage/sim_bridge_media/pc \
  --headless --duration 3
```

Check that the application exits successfully and its counters show row,
image and Canvas activity without application errors. For your own project,
also test navigation, buffer lifetimes and clean shutdown. Use the published
[simulator API](../../docs/en/reference/simulator.md) for input injection and
screenshot assertions. Validate on each deployment host platform; native
preview is not a substitute for device timing and hardware validation.

Input interception is not supported by the PC bridge. Registering a non-null
`esp_gsp_set_input_interceptor` callback returns `ESP_GSP_ERR_NOT_SUPPORTED`;
it does not install an observer or suppress samples. Its capabilities report
`bridge_input_interceptor_version: 0`. Pointer observers are asynchronous
notifications after routing. The simulator's control API injects input directly
into the portable core and does not exercise the platform interceptor/observer
path. The host separately forwards injected primary-pointer samples to Backend
observers; receiving these notifications does not prove platform interception.
Validate interception with a source platform adapter or on device.

## Python protocol client and regression checks

[rpc_client.py](rpc_client.py) is a small sequential client shared by the
documentation and live integration tests. It handles Content-Length framing,
partial reads, EOF and interleaved notifications. Run scripts from the repository
root with `PYTHONPATH=tools/sim_bridge python3 your_script.py`; for an installed
component, use that component's `tools/sim_bridge` directory instead.
`connect()` owns the socket and receiver lifetime. `Peer.call()` returns the full
JSON-RPC response, including errors; check `error` and any `result_code`.
Use one calling thread per connection. `notification()` returns the next queued
notification; passing a method returns its params. The default wait is 8 seconds;
`timeout=None` supports an idle backend event loop.

From the repository root:

```sh
python3 -m unittest discover -s tools/sim_bridge/tests -p test_rpc_client.py -v
python3 tools/sim_bridge/tests/font_catalog_integration.py \
  --host /absolute/path/to/gsp_sim_host --gspc /absolute/path/to/gspc
python3 tools/sim_bridge/tests/visibility_integration.py \
  --host /absolute/path/to/gsp_sim_host --gspc /absolute/path/to/gspc
BRIDGE_TEST_GSPC=/absolute/path/to/gspc \
BRIDGE_TEST_SIM=/absolute/path/to/gsp_sim_host \
  python3 -m unittest discover -s tools/sim_bridge/tests -p 'test_*.py'
```

The visibility check compiles a temporary hello scene and tests real WASI
visible/offscreen/wrong-scene/error results, plus API and Browser rejection both
with and without Backend enabled. It leaves the public protocol unchanged.
The live runner suite covers Library and Socket Backends, including UTF-8 input,
deletion, byte limits, confirm/cancel/return state and missing acceptance checks.
Without the two tool variables, live cases are explicitly skipped.
