# Native C simulator bridge

`sim_bridge` builds portable C UI/application sources into a native Backend
executable. It implements a supported subset of the existing `esp_gsp.h` API
over sim_host's loopback TCP Backend channel. Application callbacks, their
`user_ctx` pointers and timers stay in the native process; rendering and
animations run in sim_host. Python builds and supervises processes, but is
not an RPC relay.

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
  --project managed_components/espressif__esp-gsp/examples/hello_world/pc
```

Replace `hello_world` with `benchmark` or `sim_bridge_media` for the other
examples. For an unpacked component, use its `tools/sim_bridge/run.py` path
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
If tools are missing, install the manager into the runner's Python environment,
or obtain compatible binaries from [release downloads](https://github.com/espressif/esp-gsp/releases).
The simulator must support `--ready-file` and `bridge_version: 1`; update the
component and matching simulator together if that capability is absent.

### Dynamic font at startup

When portable UI text needs a font supplied at run time, pass the host a font
before the UI exists. This is the bridge equivalent of loading a font with
`esp_gsp_font_file_open()` and applying it to `esp_gsp_config_t` before
`esp_gsp_start()` on a device:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project pc --dynamic-font assets/NotoSansSC-Regular.otf
```

`--dynamic-font` takes precedence over `GSP_SIM_DYNAMIC_FONT`. The runner
resolves the path and requires a regular file before it builds or launches the
host, then passes its absolute path to sim_host's `--dynamic-font`. The font is
therefore installed while sim_host loads the bundle, before the native Backend
gets its initialization callback. This option is intentionally a launch-time
configuration: it cannot replace a font in an already running bridge UI.

The runner compiles a deployable bundle and its C headers together, builds
the native program, starts sim_host, reads its actual listener address, and
starts the Backend. It opens the preview URL unless `--headless` or
`--no-browser` is used. Ctrl-C, Backend exit, host failure or startup timeout
stops the processes it owns, including the manager's simulator child. POSIX
termination permits application cleanup; Windows process termination may be
immediate. Native Backend cleanup has up to six seconds before forced
termination; cleanup callbacks are not guaranteed after a crash or forced stop.
`--startup-timeout` defaults to 180 seconds and includes initial
simulator download; increase it on slow connections. The browser tab is not closed
by the runner. `--duration` starts counting after application initialization.

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

Only one application-facing bundle header is generated per target. Reuse
that same generation for all its business sources; do not mix old device
headers with a freshly compiled PC bundle. Packing runs on every build to
pick up referenced asset changes as well as JSON changes.

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
The default native `main` opens the Backend connection, checks protocol
compatibility, invokes init, then pumps callbacks and timers until exit.
Initialization must return; a blocking application loop would prevent event
dispatch. For a custom entry point, link `esp_gsp::sim_bridge` directly and use
`gsp_sim_bridge_open/poll/close` instead of `gsp_add_backend`'s default main.
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

CMake writes `<target>-<configuration>.json` with executable/bundle paths;
this works with both single- and multi-configuration generators. For multiple
backends in one CMake project, pass `--target` to the Python scripts.

## Execution and compatibility contract

There is one Backend connection and **one calling thread** within that
Backend. Call all bridge/GSP functions on this thread. UI callbacks and
periodic timers execute serially when `gsp_sim_bridge_poll()` runs. A worker
thread may perform mock I/O, but must hand results to application-owned
queues for the UI thread to consume. This is narrower than the device API's
“callable from any task” guarantee.

Synchronous RPC waits keep receiving responses and queue UI notifications;
they never recursively invoke business callbacks. Callbacks may therefore
call supported GSP APIs synchronously. Keep them short. Timers use monotonic
host time and coalesce missed ticks rather than replaying a burst. Host
headless Backend runs are paced to wall clock as well.

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
| Bind/component value/color animations; scalar property animations and `*_to`; bounded property play/stop | Executed by GSP inside sim_host |
| Press-feedback policy and generated effective-visibility queries | Supported with `bridge_api_version: 2`; visibility descriptors are copied over the Backend boundary |
| Events and timer create/delete | Native callback/context storage and event loop |
| Scene navigation, swipe policy, keyboard attach/cursor, Drawer, PageFlow set/read/drag-state | Existing RPC methods and scalar bridge queries |
| Pointer observer | Receives host pointer samples during `gsp_sim_bridge_poll()`; see the callback timing note below |
| Dynamic List/Grid binders and row text/value/color | Native callbacks, asynchronous host row requests and token-checked publication; requires media version 1 |
| Bind/component/row/Grid encoded images | COPY upload of PNG/JPEG/QOI; requires media version 1 |
| Runtime template widgets | Create/destroy, value/color/text, visibility/position, value animation and encoded RESOURCE images; requires widget version 1 |
| External `esp_gsp_assets` package images | Single-frame PNG/JPEG/QOI package members for Image, Row and Widget targets; see the limitations below |
| Bind/row/Grid image EX advanced options and Widget borrowed/owned helpers | Ownership, lifecycle callbacks and cache keys; requires image version 1 |
| Canvas push/push-dirty, draw callback, invalidate/invalidate-dirty, stop | Full-frame upload; draw callbacks use the PC offscreen adaptation below; requires media version 1 |
| Canvas try-push/try-push-dirty | Eight-frame local queue, deferred upload and release; requires media version 1 |
| Flush | Host render-attempt fence after local Canvas work has been polled; requires fence version 1 |
| Generic enum-based component get/set, batch/position APIs, input/render observers, device startup | Not implemented by this library |

`bridge_api_version: 2` is required for the RGB888 helpers,
`esp_gsp_component_play_animation()` / `stop_animation()`, press-feedback
policy, and `esp_gsp_query_visibility()`. Older version-1 hosts continue to
run the prior subset; these newer calls return `ESP_GSP_ERR_NOT_SUPPORTED`
without sending an unknown RPC.

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
type. In the [media scene](../../examples/sim_bridge_media/scenes/media.json),
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

`canvas_push` / `canvas_push_dirty` synchronously upload a full frame, even
when a dirty rectangle is supplied. Height must match the target, stride
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
bounds are checked during upload. Even dirty pushes need the entire frame.
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

Flush's finite timeout instead covers its socket writes and reply wait;
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
  an offscreen Canvas draw callback; see its [README](../../examples/sim_bridge_media/README.md).
- `showcase` and widget examples provide scene JSON previews rather than native
  business backends. Use `hello_world`, `benchmark` or `sim_bridge_media` as a
  starting point when adapting C application logic to the bridge.

Run a bounded smoke test of the published media example from your application root:

```sh
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project managed_components/espressif__esp-gsp/examples/sim_bridge_media/pc \
  --headless --duration 3
```

Check that the application exits successfully and its counters show row,
image and Canvas activity without application errors. For your own project,
also test navigation, buffer lifetimes and clean shutdown. Use the published
[simulator API](../../docs/en/reference/simulator.md) for input injection and
screenshot assertions. Validate on each deployment host platform; native
preview is not a substitute for device timing and hardware validation.
