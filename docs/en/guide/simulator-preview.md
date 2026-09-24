# Simulator Preview and Testing

The standalone simulator host provides three ways to verify UI behavior
before flashing firmware: interactive browser preview, scripted CLI testing,
and API-driven automation. Use the simulator to check scene behavior, then check display and touch on the board.

For the complete command-line, API, and backend reference see the
[Simulator reference](../reference/simulator.md).

## Obtaining the simulator

Install the simulator through the `esp-gsp-tools` toolchain manager:

```sh
python -m pip install -U esp-gsp-tools
python -m gsp.execute --version '<ESP-GSP version>' sim --capabilities
```

`<ESP-GSP version>` is the `version` field from `idf_component.yml`.
Alternatively, set `GSP_SIM_EXECUTABLE` to point to an existing executable.
See [Simulator reference — Obtaining the simulator](../reference/simulator.md#obtaining-the-simulator)
for details.

Commands below use `gsp_sim_host` for the simulator executable. When
invoking through the manager, substitute
`python -m gsp.execute --version '<ESP-GSP version>' sim`.

## Interactive browser preview

Start the simulator with no frame limit to keep the preview running:

```sh
gsp_sim_host --bundle app.gspb --frames 0
```

The host prints the preview URL to stderr. Open it manually in a browser to see:

- a live canvas rendering the simulated display;
- mouse click and drag mapped to pointer input;
- 25 %–400 % zoom;
- scene navigation buttons (◀ ▶ ↺);
- a tabbed log panel showing callback events and runtime output.

Use this mode during authoring to see layout changes, animation, and
navigation in real time without a target build. Closing the browser does not
stop the host; press Ctrl-C or send the control API `quit` method.

## Headless testing and screenshots

For CI or automated checks, run without a browser and capture the final
frame:

```sh
gsp_sim_host --bundle app.gspb --headless \
    --frames 3 --dump final.png --dump-format png
```

Add scripted input to verify interactive paths:

```sh
gsp_sim_host --bundle app.gspb --headless \
    --tap 160 120 --wait 5 --tap 80 200 \
    --dump result.png --dump-format png
```

Scripted `--tap`, `--drag`, and `--wait` actions execute in order. For
complex sequences, prefer the API channel.

## Multiple simulator instances

When running multiple simulators on one machine, assign explicit, distinct
ports instead of relying on automatic allocation. In particular, set
`--visual-listen`, TCP `--api-listen`, and `--backend-listen` ports as needed,
and keep each endpoint associated with its bundle and project.

If a captured simulator image differs greatly from the expected scene, verify
the endpoint and bundle before diagnosing the UI. Another simulator, including
one started by a different project, may still be running and serving the image.

## Reuse C UI business logic

The installed ESP-GSP component includes `tools/sim_bridge`, a native C compatibility
library and CMake integration for building UI `.c` files as a sim_host
Backend. Business sources keep using `esp_gsp.h` and generated component
helpers. Python builds and supervises the processes; the native executable
connects directly over loopback TCP.

Install Python 3.10+, CMake 3.20+, a native C11 compiler and `esp-gsp-tools`.
After your IDF project has resolved its ESP-GSP dependency (for example with
`idf.py reconfigure`), run a packaged example from the application root:

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project managed_components/espressif__esp-gsp/examples/usage/hello_world/pc
```

On Windows, use a compiler developer terminal and `C:/path/to/tool.exe`
paths. The native build does not require an active ESP-IDF environment or
the compiler/simulator implementation sources.
The host must support `--ready-file` and advertise
`capabilities.bridge_version: 1`; older simulators cannot run this workflow.

For your own C UI sources, create a `pc/` project using the
[bridge CMake example](../../../tools/sim_bridge/README.md#add-your-own-application),
then use `--project pc`. The runner selects GSPC through `python -m gsp.execute`
using the application's `.gspc_version`, falling back to the component's
marker. It selects `sim` using the component's `idf_component.yml` version.
The first use downloads/verifies/caches those precompiled tools.
`--gspc` / `GSPC_EXECUTABLE` and `--host` / `GSP_SIM_EXECUTABLE` override binary
paths; `--sim-version` / `GSP_SIM_VERSION` overrides only the managed simulator
version. `--component-dir` / `ESP_GSP_COMPONENT_DIR` selects a different installed
component. Default build output is under the working directory's `build/`,
not `managed_components`. For an unpacked component, adjust only the component
path; no source repository is required.

CMake packs scenes with `--deployable` and generates matching
`bundle_gsp.h` headers in the same build. The runner reads the actual
listener address, starts the Backend, opens the browser, and stops both
owned processes on exit. Add `--headless --duration 3` for a bounded run,
or use `build.py` with the same build arguments to build without launching.
Listener readiness does not mean business initialization is complete;
`--duration` starts after application initialization.

The examples demonstrate the split:

- `hello_world/main/hello_ui.c`: shared device/PC timer-driven UI logic.
- `hello_world/main/app_main.c`: device-only display, touch and GSP startup.
- `hello_world/pc/platform_pc.c`: PC lifecycle adapter.
- `benchmark/pc`: directly compiles the already-separated
  `main/bench_workload.c`, demonstrating reads and animations, not full
  hardware benchmark measurements.
- `sim_bridge_media/pc`: portable List/Grid binders, QOI images and Canvas
  offscreen drawing; select this project with the same runner command.

Isolate drivers, RTOS tasks and hardware services behind an application
HAL/PC mock. Version 1 requires one Backend calling thread; callbacks can
make synchronous supported GSP calls, while workers hand data over through
application queues. The bridge maps common state access, scalar properties,
animations, events, timers and selected navigation APIs. Hosts advertising
`capabilities.bridge_media_version: 1` also support native dynamic List/Grid
binders, COPY PNG/JPEG/QOI images and Canvas push/draw APIs. Binders can make
synchronous row updates. Canvas callbacks instead draw a full local offscreen
surface; small dirty regions use packed wire patches when supported, while
large changes use full frames. Timing and tile partitioning differ from the device,
and GSP mutations inside draw callbacks are rejected. Re-register draw
callbacks on scene entry. `capabilities.bridge_image_version: 1` additionally
supports image BORROW/TAKE helpers, completion/release callbacks and cache
keys. The wire still copies; native sources remain retained until GSP input
release or local shutdown. `canvas_try_push*` admits into an eight-frame local
queue without socket I/O; full queues return TIMEOUT without taking ownership.
Keep accepted buffers immutable until their release callback. Poll or a later
synchronous RPC uploads them. Use upload and completion results to track host processing.
With `capabilities.bridge_fence_version: 1`, `esp_gsp_flush` waits for a host
render attempt, not animations, image decoding or browser presentation. First
poll to finish pending local Canvas work; otherwise flush returns INVALID_STATE.
Timeout does not cancel the fence; later poll/RPC handles its reply.
Unimplemented declarations from the shared headers fail at link time.

See the component's [bridge README](../../../tools/sim_bridge/README.md) for the CMake interface,
lifecycle and limitations, and the [Simulator reference](../reference/simulator.md)
for the protocol.

## API-driven automation

Enable the JSON-RPC control channel for programmatic access:

```sh
gsp_sim_host --bundle app.gspb --frames 0 --api-enable \
    --api-json app_gsp.api.json
```

Then drive the simulator from any JSON-RPC client. A typical verification
flow:

1. `capabilities` — confirm display size, scene count and `named_api`.
2. `list_objects` / `inspect_object` — discover the target in the current scene.
3. `tap_object` / `set_property` — drive named input or state.
4. `wait_component` — wait for PageFlow/Drawer to settle at the expected value;
   check `capabilities.wait_component_version` first. `wait` only counts frames.
5. `screenshot` — capture the result.
6. `quit` — shut down.

The channel supports stdio (default), loopback TCP, and Unix sockets. Set
`--input-mode api-exclusive` when the automation must be the sole input
source and the browser preview is open simultaneously.
The sidecar must come from the same compile as the bundle. Without one, use
coordinate input and numeric bindings.

## Full-stack simulation with a backend

To test application logic alongside the UI, connect an application backend:

```sh
gsp_sim_host --bundle app.gspb --frames 0 \
    --backend-listen tcp://127.0.0.1:8684 \
    --backend-required --api-enable
```

The backend receives `callback` notifications when the user interacts with
the UI and drives the display through `set_text`, `set_value`, `drawer_open`,
`page_flow_set_page`, `list_snap`, `goto_scene`, and other state methods. While
the backend endpoint is enabled, application state writes and scene navigation
are backend-exclusive: browser controls and API calls cannot bypass the
business backend.
The API channel may still perform input, named inspection, waits and screenshots;
state writes and `goto_scene` must be sent by the backend.

To reuse device-side C application logic, keep LCD, touch, GPIO, Wi-Fi, NVS,
and FreeRTOS initialization in a platform layer. Keep timers,
`esp_gsp_on_event()`, and `esp_gsp_*` UI calls in the application layer. A
native `sim_bridge` compatibility library is included in the component and
builds with a native C compiler and the automatically selected precompiled tools.
The native library maps asynchronous `list_bind` notifications and
token-checked `row_publish` to local C binders, and uses COPY binary uploads
for PNG/JPEG/QOI images and full Canvas frames. The host render task never
waits for a Backend callback. For Canvas direct-draw, the PC adaptation runs
the callback offscreen in the native event loop and uploads the result.
Successful upload means acceptance, not completed decoding or rendering;
Canvas push invokes the supplied release callback before returning on success, unlike
device-side deferred release. See the bridge README and simulator reference
for ownership, unsupported APIs and token lifetime rules.

This mode is suitable for verifying data binding, event handling, and
multi-scene flow without target hardware.

## Choosing the right mode

| Goal | Recommended approach |
|---|---|
| Visual authoring and layout iteration | Interactive browser preview |
| Quick render check in CI | Headless with `--dump` |
| Regression test for interactions | Headless with `--tap` / `--drag` scripting |
| Complex automation or AI agent testing | API channel (`--api-enable`) |
| End-to-end application logic | Backend + API channel |

## Run on the board

Use the simulator to check layout, text and interaction before flashing. Check panel
wiring, orientation, byte order, touch mapping and performance on the target board;
see [Display integration](display.md).
