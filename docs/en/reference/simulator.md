# Simulator Reference

The standalone ESP-GSP simulator is distributed as a precompiled executable.
It provides an interactive browser
preview, CLI-scripted input, and a JSON-RPC automation API.

## Obtaining the simulator

The simulator is not a firmware build dependency and must be obtained
separately. The recommended method is the `esp-gsp-tools` toolchain manager:

```sh
python -m pip install -U esp-gsp-tools
python -m gsp.execute --version '<ESP-GSP version>' sim --capabilities
```

`<ESP-GSP version>` is the `version` field from `idf_component.yml` (the
ESP-GSP component version), independent of the GSPC version in
`.gspc_version`. The manager downloads and caches the requested version
automatically.

Alternatively, set the `GSP_SIM_EXECUTABLE` environment variable to point
to an existing simulator executable.

Commands below use `gsp_sim_host` for the simulator executable. When
invoking through the manager, substitute
`python -m gsp.execute --version '<ESP-GSP version>' sim`.

## Quick start

```sh
# Interactive browser preview (runs until quit)
gsp_sim_host --bundle app.gspb --frames 0

# Headless screenshot
gsp_sim_host --bundle app.gspb --headless \
    --tap 120 80 --dump frame.png --dump-format png

# API-driven automation
gsp_sim_host --bundle app.gspb --frames 0 --api-enable

# Full-stack simulation with application backend
gsp_sim_host --bundle app.gspb --frames 0 \
    --backend-listen tcp://127.0.0.1:9001 --api-enable
```

## Browser preview

The host embeds a lightweight HTTP + WebSocket server. When `--headless` is
not specified, the preview starts automatically and the URL is printed to
stderr:

```
gsp_sim_host: browser preview listening at http://127.0.0.1:3222/
```

| Feature | Description |
|---|---|
| Real-time canvas | Renders frame data received over WebSocket |
| Pointer input | Mouse click and drag mapped to simulator input |
| Zoom | 25 %–400 % viewport scaling |
| Scene navigation | Previous (◀), Next (▶), and Reset (↺) buttons |
| Log panel | Tabbed view of callback events, host logs, and runtime diagnostics |

When the backend endpoint is enabled, scene navigation buttons and direct
application-state writes are rejected by the server. When `--input-mode` is `api-exclusive`, mouse clicks are rejected
with an `input_busy` error. Both conditions are reported in the log panel.

Open the printed URL manually; the host does not launch a browser. Closing a
browser does not stop the host; use Ctrl-C or the control API `quit` method.

Multiple browsers may connect simultaneously; each maintains independent
keyframe state and subscriptions.

## Command-line reference

Run `gsp_sim_host --help` for the built-in help.

Use the bundle's compiled GFB glyphs when they cover the required text, including
text changed at runtime. `--dynamic-font` is an optional missing-glyph fallback;
GFB catalogs also use compiled glyphs and do not require a raw font.

### Scene and timing

| Parameter | Description |
|---|---|
| `--bundle <PATH>` | Scene bundle file (`.gspb`); required except for capability or embedded-runtime information queries |
| `--dynamic-font <PATH>` | Optional missing-glyph fallback using a scalable TrueType or CFF font (`.ttf`, `.otf`, or first face of `.ttc`); query `--capabilities` for compiled drivers |
| `--font-catalog <PATH>` | GSPB font catalog from `gspc font-link`, loaded before UI startup |
| `--record-input <PATH>` | Record consumed touch samples to a new JSONL file; existing files are rejected |
| `--replay-input <PATH>` | Replay a matching JSONL trace; conflicts with recording and CLI tap/drag/wait |
| `--fps <N>` | Simulated frame rate, default `60` |
| `--frames <N>` | Frame limit; default `0` = no limit; script/replay exit rules are described below |

The 1.8.0 WASI runtime includes TrueType and CFF drivers. CLI/RPC capabilities
report registered TrueType/CFF drivers in `font_outline_drivers`, such as
`["truetype", "cff"]`; `null` means the
embedded runtime predates the query or the probe could not run, and `[]` means
neither TrueType nor CFF is enabled. Older simulator releases may lack CFF.
The CFF driver handles CFF/CFF2 outlines, but this API does not expose variable
font axes. The extension alone does not identify the outline type.
Native source builds report the drivers in their linked FreeType.
Collections (`.ttc`) use face 0, with the same driver restrictions; diagnostics
identify that face's outline signature when the collection header allows it.
An empty font file is rejected before the runtime starts.
Bitmap-only faces are rejected during setup; dynamic fallback requires scalable outlines.

If optional font setup fails, compiled GFB glyphs remain available and the
dynamic fallback is disabled. Face setup warnings include the detected font
container, byte count and FreeType error code; a missing CFF driver is reported
explicitly. Other parse failures do not by themselves prove that a font is
damaged: inspect the error code and the drivers enabled in that FreeType build.
Size setup failures report the FreeType operation, scene/font index, requested
pixel size and FreeType error code.

The glyph-cache dimension is a caching bound, not a rendered font-size limit.
Larger fallback bitmaps use one reusable heap workspace and are rasterized
again on later resolves. The simulator drives the portable runtime without
FreeRTOS render tasks; device task-stack settings do not size its WASM stack.

Use `--font-catalog fonts.gspb` (or `GSP_FONT_CATALOG`) for a catalog from
`gspc font-link`; an explicit CLI path wins. Both this option and `--dynamic-font`
may be supplied. The catalog is CRC-checked before UI startup and preserved on
reset. `font_catalog_version: 1` in CLI/RPC capabilities identifies support in
the embedded runtime; older simulator releases need an update. Catalogs resolve
missing GFB assets by ID, not missing codepoints. See [external fonts](../guide/external-assets.md#simulator-and-pc-bridge)
for resolution priority and PC bridge limits.

### Preview and input

| Parameter | Description |
|---|---|
| `--headless` | Disable browser preview |
| `--visual-listen <ADDR>` | Bind preview to a specific address; mutually exclusive with `--headless` |
| `--input-mode <MODE>` | Input ownership: `shared` (default) / `api-exclusive` / `browser-exclusive` |
| `--fail-on-error` | Exit non-zero when the runtime emits an `E (...)` error log |

### Input scripting

| Parameter | Description |
|---|---|
| `--tap <X> <Y>` | Tap at coordinates (X, Y); repeatable |
| `--drag <X1> <Y1> <X2> <Y2>` | Drag from (X1, Y1) to (X2, Y2); repeatable |
| `--wait <FRAMES>` | Wait N frames before the next scripted action (N > 0); repeatable |

Scripted actions execute in command-line order. A completed script exits when
no API channel is enabled; with an API channel, use `quit` or a frame limit.
The same input commands are also available through the API channel at runtime.

### Touch recording and replay

```sh
gsp_sim_host --bundle app.gspb --record-input touch.jsonl --frames 600
gsp_sim_host --bundle app.gspb --headless --replay-input touch.jsonl \
    --dump replay.png --dump-format png
```

Recording captures consumed pointer/touch samples, including expanded tap/drag
samples. Replay uses simulation time and must match the bundle, dimensions,
frame step, and optional dynamic-font/catalog hashes in the trace. It exits at
the end record; a frame limit or `quit` that cuts it short returns an error.
Reset is rejected during recording or replay, and replay rejects live input.
The trace contains touch input, not property writes, business state or Backend
`pointer` notifications. Reproduce those separately when the application needs
them. Use a fresh recording filename; a truncated trace is not a valid replay.

### Output and connections

| Parameter | Description |
|---|---|
| `--dump <PATH>` | Save the last frame at exit |
| `--dump-format <FMT>` | Screenshot format: `ppm` (default) / `png` / `raw` |
| `--api-enable` | Enable API control channel (defaults to stdio) |
| `--api-listen <URL>` | API endpoint; implies `--api-enable` |
| `--api-json <PATH>` | Load a GSPC `*.api.json` for named tap/set, inspect/hit_test, and callback names |
| `--backend-enable` | Enable application backend on an automatically selected local port |
| `--backend-listen <URL>` | Backend endpoint; implies `--backend-enable` |
| `--backend-library <PATH>` | Load a native Backend shared library; mutually exclusive with Socket Backend options |
| `--backend-required` | Pause frame progression until backend connects |
| `--backend-idle-timeout <SECS>` | With `--backend-required`, timeout for initial connection or reconnection; default `0` = no limit |
| `--ready-file <PATH>` | Write listener addresses and process identity after startup; use a fresh path and remove it after exit |
| `--log-level <LEVEL>` | `trace` / `debug` / `info` (default) / `warn` / `error`; a valid `RUST_LOG` takes precedence |
| `--log-file <PATH>` | Append host tracing logs to a file; WASI stderr and the preview URL remain on stderr |

### Diagnostics

| Parameter | Description |
|---|---|
| `--capabilities` | Print the simulator capability set and exit without requiring a bundle |
| `--print-wasm-info` | Hidden diagnostic option: print embedded WASM `staged`, `bytes`, `sha256` and `source`, then exit without a bundle |

### Advanced tuning

| Parameter | Description |
|---|---|
| `--invalidate-every-frame` | Force full-screen redraw every frame |
| `--instance-slots <N>` | Override bundle component instance limit (`0` = use bundle value) |
| `--dynamic-image-slots <N>` | Override bundle runtime image limit (`0` = use bundle value) |
| `--idle-poll-ms <N>` | Idle poll interval in milliseconds (`0` = use default) |

## Input modes

When both the API channel and browser are connected, `--input-mode` decides
which endpoint may inject pointer input (`tap`, `drag`, `feed_pointer`,
`feed_touch`):

| Mode | Browser | API channel | Note |
|---|---|---|---|
| `shared` | ✓ | ✓ | First-come first-served (default) |
| `api-exclusive` | ✗ `input_busy` | ✓ | Browser clicks are rejected |
| `browser-exclusive` | ✓ | ✗ `input_busy` | API input requests are rejected |

The backend channel can never send input commands regardless of mode.

## Port allocation

When auto-assigning ports (no port specified or port `0`), the host tries
sequentially within these preferred ranges:

| Channel | Preferred range | Manual override |
|---|---|---|
| Browser preview | 3222–3231 | `--visual-listen 127.0.0.1:PORT` |
| API control | 8266–8275 | `--api-listen tcp://127.0.0.1:PORT` |
| Application backend | 8684–8693 | `--backend-listen tcp://127.0.0.1:PORT` |

If all preferred ports are occupied, the host exits with an error.

## Automation API

The API control channel enables scripted and AI-agent-driven automation using
JSON-RPC 2.0. Enable it with `--api-enable` (defaults to stdio) or
`--api-listen <URL>`. Supported transports: stdio, loopback TCP
(`tcp://127.0.0.1:PORT`), and Unix socket (`unix:///path/to/sim.sock`).

For stable named automation, compile with `--api-header` and load the resulting
bundle sidecar:

```sh
gsp_sim_host --bundle build/app.gspb --api-json build/app_gsp.api.json \
    --api-listen tcp://127.0.0.1:8266 --frames 0
```

`capabilities.named_api` confirms that the map is active. The sidecar must come
from the same compile as the bundle; mismatched schema, byte length, or CRC is
rejected at startup. Named `bounds` describe the compiled initial layout;
`query_geometry` and `inspect_object.runtime_geometry` inspect current input geometry.

### Methods

#### Input

| Method | Parameters | Description |
|---|---|---|
| `tap` | `{ "x": N, "y": N }` | Simulate a tap |
| `tap_object` | `{ "name": "...", "scene": N }` | Tap a verified runtime input point; optional `require_verified` rejects legacy fallback |
| `drag` | `{ "x1": N, "y1": N, "x2": N, "y2": N }` | Simulate a drag gesture |
| `feed_pointer` | `{ "x": N, "y": N, "pressed": bool }` | Low-level pointer input |
| `feed_touch` | `{ "id": N, "x": N, "y": N, "pressed": bool }` | Low-level multi-touch input |

#### Scene and state

| Method | Parameters | Description |
|---|---|---|
| `goto_scene` | `{ "scene": N, "transition": N }` | Switch to a 0-based scene; `transition` is optional |
| `reset` | — | Reload the scene bundle |
| `set_property` | `{ "name": "...", "property": "...", "value": ... }` | Set a named property using sidecar type/range metadata |
| `get_property` | `{ "name": "...", "property": "...", "scene": N }` | Read the active scene's named scalar property from committed runtime state |
| `wait_property` | `{ "name": "...", "property": "...", "value": ..., "max_frames": N }` | Wait for the typed expected value after a frame step |
| `set_value` | `{ "bind_id": N, "value": N }` | Set an integer binding |
| `set_text` | `{ "bind_id": N, "text": "..." }` | Set a text binding |
| `set_color` | `{ "bind_id": N, "color": N }` | Set a native-format color binding |
| `set_visible` | `{ "bind_id": N, "visible": bool }` | Toggle component visibility |
| `set_component_i32` | `{ "component_key": N, "property_key": N, "value": N }` | Set component property by key |
| `keyboard_attach` | `{ "action_id": N, "text_bind": N }` | Attach virtual keyboard |
| `keyboard_attach_ex` | `{ "action_id": N, "text_bind": N, "max_bytes": N }` | Attach a virtual keyboard with an explicit UTF-8 byte limit; `max_bytes` excludes the trailing NUL |
| `keyboard_text_size` | — | Read the required keyboard-text capacity, including the trailing NUL; API/Backend/Browser read-only |
| `keyboard_text` | `{ "capacity": N }` (optional) | Read current keyboard text; insufficient capacity returns `ESP_GSP_ERR_INVALID_SIZE` with a valid UTF-8 prefix; API/Backend/Browser read-only |
| `set_cursor` | `{ "bind_id": N }` | Show/hide text cursor; `65535` hides it |
| `set_swipe_transition` | `{ "transition": N }` | Select swipe transition visual |
| `set_swipe_fade_black_point` | `{ "drag_percent": N }` | Set swipe fade black point (1..99) |
| `drawer_open` / `drawer_close` | `{ "component_key": N, "animated": bool }` | Open/close a Drawer |
| `drawer_is_open` | `{ "component_key": N }` | Read Drawer state |
| `page_flow_set_page` | `{ "component_key": N, "page": N, "animated": bool }` | Select a PageFlow page |
| `list_bind_component` | `{ "component_key": N }` | Bind a fixed-item List/Wheel; returns a `list` handle |
| `list_snap` | `{ "list": N, "enable": bool }` | Enable/disable Wheel row snapping |
| `list_fling` | `{ "list": N, "velocity_px_s": N }` | Start list momentum scrolling |
| `list_scroll_to` | `{ "list": N, "offset_px": N }` | Set an absolute list offset |
| `set_swipe_enabled` | `{ "enabled": bool }` | Enable or disable swipe navigation |
| `fling_messages` | `{ "velocity_px_s": N }` | Simulate list fling gesture |

`set_property` accepts booleans and numbers in their authored units. Retained
components use the same property codecs as the C API, including `enabled`,
Toggle `checked`, and custom Slider/Arc/Progress ranges. Plain bundles without a
component directory retain the corresponding binding conversions. `set_value`
continues to write the raw integer binding; it does not convert semantic values.
With a retained directory, scalar writes validate the runtime property key and
type, including color and visibility. Invalid metadata cannot fall back to a
raw binding write.

#### Utility

| Method | Parameters | Description |
|---|---|---|
| `ping` | — | Heartbeat; returns `"pong"` |
| `capabilities` | — | Query display size, scene count, runtime ABI, and `named_api` |
| `list_objects` | `{ "scene": N }` | List named objects; `scene` is optional |
| `inspect_object` | `{ "name": "...", "scene": N }` | Inspect named operations, properties, and compiled bounds |
| `hit_test` | `{ "x": N, "y": N, "scene": N }` | Query actual runtime input routing; older modules return compiled bounds |
| `query_geometry` | `{ "name": "...", "scene": N, "instance": N }` | Current input bounds, clipped regions and a verified tap point |
| `resource_memory` | `{ "scene": N }` | On-demand simulator allocation attribution |
| `frame_info` | — | Get latest frame commit info |
| `wait` | `{ "frames": N }` | Respond after N frames |
| `component_get_motion` | `{ "name": "...", "scene": N }` | Query PageFlow/Drawer state; alternatively use `component_key`; `scene` is optional |
| `wait_component` | `{ "name": "...", "value": N, "max_frames": N }` | Wait for rest and optional value; see conditions below |
| `invalidate` | — | Force full-screen redraw next frame |
| `screenshot` | `{ "path": "...", "format": "png" }` | Save current frame to file |
| `subscribe` | `{ "events": [...] }` | Subscribe to server notifications |
| `unsubscribe` | `{ "events": [...] }` | Unsubscribe from notifications |
| `quit` | — | Request simulator exit |

`capabilities.input_ready` is true when no transition, custom interceptor, active
contact, pressed pointer or gesture owns input. A changed scene ID alone does not
mean the scene is ready for a tap.

With `runtime_input_query_version: 1` and a matching new sidecar, `query_geometry`
resolves current transforms, clips and enabled state. Candidate points are checked
through runtime hit routing and Drawer gating. `no_verified_point` means no sampled
point was verified; it does not prove the object is completely occluded. Queries
refuse transition-owned input and custom interceptors. Existing contacts or an
active gesture return `input_busy`; rejection preserves the existing contacts.
Non-interactive objects have no input target.

After the initial points for each hit region, the query uses observed occluder
boundaries to find narrow exposed areas. Each request allows up to 64 additional
point checks and 64 boundary queries; every point still requires a runtime hit
match. The optional `search` object reports these counts and `budget_exhausted`.

`tap_object` uses this query and returns `target_verified: true` when it verifies
the target before dispatch. It does not verify application results: inspect callbacks,
state and screenshots afterward. Pass `require_verified: true` to reject older
modules/sidecars, whose compatible fallback is the compiled center with
`target_verified: false`. A known blocked target never falls back.

For a list row, obtain `instance`, `runtime_epoch`, `list` and `item` from `hit_test`
and pass all four to `tap_object` with the list name. Instance generations alone
cannot detect a row rebound to another item. The host checks owner, item, generation
and runtime epoch again before dispatch; reset invalidates saved row identities.

With `resource_memory_version: 1`, `resource_memory` reports simultaneous tracked
image-payload peaks and current cache, animation and host-presentation allocations.
Cache bytes overlap decoded payloads; individual peaks must not be summed. The
response states exclusions and leaves the complete application peak unknown.
These host readings do not establish device heap or DMA availability.
`observed_at` associates the reading with the validated Bundle identity, active
scene/content ID, frame sequence, runtime epoch/version and display size. Bundle
identity is null without a validated sidecar; the target Profile remains unknown.
Compare readings from the intended artifact and state before attributing a change
to an asset or interaction.

The API channel remains JSON-only. The application backend additionally has
asynchronous dynamic-data and binary-media extensions described below; the
Browser never receives their data-source requests or binary-upload access.

### Named scalar properties

Named scalar observation requires `named_api`, plus
`capabilities.named_property_read_version: 1` / `wait_property_version: 1`.
It uses the matching API sidecar's typed binds and supports ordinary and
deployable Bundles. Results report `source: "runtime"`; text properties are
outside this scalar API. Reads/waits are available while a Backend owns state
writes. A getter does not advance frames. A wait evaluates after at least one
successful frame step, even if the value already matches, so a preceding write
can be painted before a subsequent screenshot. `max_frames` defaults to 300
and accepts 1–36000. The expected value must match the declared type and range.
Timeouts and runtime query failures are JSON-RPC errors; reset and scene change
cancel pending waits. A matched value proves the observed state, while actual
input/callback and screenshot checks establish interaction and visual behavior.
`set_property` color writes accept RGB888; color reads/waits use the active pixel format's
native values, such as `0xF800` for red in RGB565. Use screenshot comparisons
with a baseline for the same pixel format when checking rendered colors. When a
component directory is present, scalar writes validate runtime property keys and
types, including color and visibility; invalid metadata cannot fall back to raw bind writes.

### Component motion and conditional waiting

Before using these methods, check `capabilities.component_motion_version`,
`component_events_version`, and `wait_component_version`. A value of `1`
means supported; `0` means the loaded WASM does not include the feature.

**Querying motion state.**
`component_get_motion` accepts `{"name":"pages"}` or a numeric
`component_key`, with an optional `scene`. The response contains
`result_code` and a `state` object:

- `value` — committed page index or open/close state.
- `target` — destination value during a transition.
- `dragging` / `settling` — whether the user is still dragging or the
  component is animating toward the target.

A failed query may return `state: null`; always check `result_code`.

**Waiting for a component to settle.**
`wait_component` takes the same target identifier plus optional `value`
(the expected final value; omit to wait for any settled state) and
`max_frames` (default 300, range 1–36 000). When the component stops
moving and the value matches, the response contains
`status: "settled"`, the final `state`, and `frames_waited`. The budget
counts simulation frames, not wall-clock time. On timeout the method
returns error `-32020` with `data.reason: "timeout"` and the last
observed state. A scene change or `reset` cancels any pending wait.
These observation methods remain available even when Backend owns
state writes.

To require evidence of a new action, check `component_event_cursor_version: 1`,
read `component_get_motion.event_cursor` **before** the action, then pass it as
`wait_component.after_event`. Success requires a newer event for that component
(and expected value) as well as settled state, returning `event_observed: true`.
An unchanged initial state cannot pass. Reset or exhausted event history returns
`event_cursor_expired`; concurrent writes still require application-level checks.
Omitting `after_event` preserves ordinary predicate-wait semantics.

**Component event notifications.**
Subscribe to `component_event` to receive per-step notifications with
`component_key`, `scene_id`, `kind`, `flags`, `state`, and `event_cursor`. The `flags`
field is a bitmask: value changed (`1`), settled (`2`), motion
changed (`4`). Events are coalesced per UI step and do not record every
intermediate request. For deterministic assertions, use the control API
(`wait_component` or `component_get_motion`); Browser WebSocket
notifications share a droppable frame queue. Note that `wait` only
counts frames and `render_fence` only marks a render boundary — neither
guarantees that an animation or business operation has completed.

### Notification events

Events are delivered as JSON-RPC notifications (no `id`). Register with
`subscribe` first; the API channel has no default subscriptions.

| Event | Parameters | Description |
|---|---|---|
| `scene_changed` | `{ "from": N, "to": N }` | Scene transition |
| `component_event` | `{ "component_key": N, "scene_id": N, "kind": N, "flags": N, "state": {...} }` | Coalesced PageFlow/Drawer state notification; explicitly subscribe |
| `callback` | `{ "action_id": N, "arg": N, "scene_id": N, "list": N, "item": N, "callback"?: "name" }` | Component callback; includes the compiled callback name when `--api-json` is loaded |
| `frame` | `{ "index": N }` | Per-frame tick |
| `list_bind` | `{ "list": N, "slot": N, "instance": N, "item": N, "resource_slot": N, "text_slot": N }` | Backend dynamic List/Grid row request; Grid member slots, 65535 means absent (both absent for List) |
| `list_bind_overflow` | `{ "dropped": N, "level": "warn", "message": "..." }` | Backend row-request loss; native bridge requires a session restart |
| `binary_result` | `{ "transfer_id": "...", "ok": bool, ... }` | Backend binary-upload acceptance |
| `backend_state` | `{ "state": "attached"\|"detached" }` | Backend connection change |
| `log` | `{ "level": "...", "message": "..." }` | Host log message |
| `wasi_log` | `{ "message": "..." }` | Runtime diagnostics (notification name retained for compatibility) |

### Error codes

| Code | Meaning | Description |
|---|---|---|
| `-32600` | Invalid Request | Missing `jsonrpc: "2.0"` |
| `-32601` | Method Not Found | Unknown method |
| `-32601` | Method Not Allowed | The channel called a disallowed method |
| `-32601` | Backend Exclusive | A non-backend channel called an application-state method while the backend is enabled |
| `-32602` | Invalid Params | Missing or wrong parameter type |
| `-32603` | Internal Error | Host internal error |
| `-32010` | Input Busy | `--input-mode` blocked input from this channel |
| `-32020` | Observation / verified input failed | Inspect `data.reason`, including timeout, cancellation, expired cursor, unsupported capability or unverified target |

`not_allowed` and `backend_exclusive` are documentation labels, both with code
`-32601`. Their messages are `method not allowed for this channel: ...` and
`method reserved for backend while backend is enabled: ...`, respectively.
`widget_*`, `bridge_call`, `query_visibility`, `render_fence` and remote data/media
extensions are always Backend-only; API/Browser calls return `not_allowed`
regardless of whether Backend is enabled.

### Python client example

Examples share [rpc_client.py](../../../tools/sim_bridge/rpc_client.py). From the repository root run
`PYTHONPATH=tools/sim_bridge python3 your_script.py` (use the component path for
an installed package). The client preserves consecutive frames and interleaved
notifications and raises on EOF. Use one caller per connection; `call()` returns
the complete response, including `error`. Check errors and `result_code`.

```python
from rpc_client import connect

with connect(port=8266) as api:
    caps = api.call("capabilities")
    print(caps)
    print(api.call("tap", {"x": 160, "y": 120}))
    print(api.call("wait", {"frames": 5}))
    print(api.call("screenshot", {"path": "/tmp/shot.png", "format": "png"}))
```

## Application backend

The backend channel allows application business logic to drive the UI during
simulation, mirroring the firmware C API. Enable with
`--backend-listen <URL>` (TCP or Unix socket; stdio is not supported), or load
a matching native C Backend with `--backend-library <PATH>`.

When the backend endpoint is enabled:

- Application state writes (`set_*`, component, Drawer, PageFlow and List
  controls) plus `goto_scene` and `reset` become backend-exclusive. API calls
  receive `backend_exclusive`. Browser state writes return `not_allowed`, while
  `goto_scene` and `reset` return `backend_exclusive` when Backend is enabled.
- The backend automatically subscribes to `callback`, `scene_changed`, `pointer`, `list_bind`, `list_bind_overflow`, `binary_result`, `image_complete`, and `image_release`.
- Only one backend connection is allowed at a time.

Use `--backend-required` to pause frame progression until the backend
connects. Use `--backend-idle-timeout <SECS>` with it to limit initial connection or reconnect waits (default `0`, unlimited).

The backend may call the state and application-logic methods listed above,
including cursor/swipe policy, Drawer and PageFlow controls, and fixed-item
List/Wheel operations. It may also bind dynamic collections with
`list_bind_remote` / `grid_bind_remote`, set their size with `list_set_total`,
and answer `list_bind` notifications with token-checked `row_publish` updates.
Rows can be recycled while a backend is working: a nonzero row result code
can mean a stale token or another failure, such as an invalid field slot.
Inspect the result code; never retry a token known to be stale.

Backend TCP/Unix connections also support binary uploads using COPY
semantics with `Content-Length` framing:

- `X-GSP-Kind: image` and `row-image` — accept PNG, JPEG, or QOI data.
- `X-GSP-Kind: canvas` — accepts a complete raw frame with stride and
  height headers (and an optional dirty rectangle).

The host confirms each upload with a `binary_result` notification.
Direct-draw callbacks run inside the render task, so a remote
(socket-based) producer should push frames instead of waiting for a
draw callback; call `canvas_stop` to stop a remote producer. These
binary extensions are Backend-only. Native C producers should use the
component's [bridge library](../../../tools/sim_bridge/README.md) for
framing and buffer lifetime management. The simulator capability bitmap
reports QOI decoder support as `ESP_GSP_SIM_CAP_QOI` (bit 7,
value `128`).

### Backend Python example

```python
from rpc_client import connect

with connect(port=8684) as backend:
    print(backend.call("capabilities"))
    # Use bind IDs from your generated scene.
    print(backend.call("set_text", {"bind_id": 1, "text": "Ready"}))
    print(backend.call("set_value", {"bind_id": 2, "value": 75}))
    while True:
        event = backend.notification(timeout=None)
        print(event["method"], event["params"])
        # Handle callback/scene_changed here; backend.call() retains notifications.
```

## Native C Backend projects

`tools/sim_bridge` provides `gsp_add_backend(target SOURCES ... SCENES ...)`
to link business C sources with the native compatibility library and generate
a matching deployable bundle/header pair. The build produces both a shared
library (the default runner mode) and an isolated Socket executable. Use
`INCLUDE_DIRECTORIES` for business headers and `PROFILE` to override the
default RGB565 profile. A PC adapter implements `gsp_bridge_app_init(ui)`
and `gsp_bridge_app_deinit(ui)`; it does not start ESP-LCD or link the full
GSP renderer.

See [Reuse C UI business logic](../configuration/simulator-preview.md#reuse-c-ui-business-logic)
for examples. The installed component's [bridge README](../../../tools/sim_bridge/README.md) lists supported
C functions. Unimplemented functions fail at link time instead of silently
succeeding. This version does not support concurrent calling threads,
automatic reconnection or state replay after reset.

The supplied entry points own event dispatch after app initialization returns.
A custom entry point must repeatedly call `gsp_sim_bridge_poll()` on its Backend
thread, including while idle; host rendering and input acknowledgments do not
drive C callbacks or timers for it. Never block this thread waiting for input.
See [custom entry-point requirements](../../../tools/sim_bridge/README.md#custom-entry-point-keep-the-event-loop-running)
for command queues, partial input and an event-loop example.

The shared library contains the exact ESP-GSP version used to build it.
sim_host rejects it before application startup unless its Backend ABI,
descriptor size, reserved fields, run entry, and GSP version all match. This
guards against stale build products, not hostile native code: a loaded library
shares the simulator process and can crash or block it. Pass
`tools/sim_bridge/run.py --socket-backend` when process isolation is required.

### Dynamic media

When `capabilities.bridge_media_version` is `1`, the bridge library
supports dynamic List/Grid binders, row fields, COPY-mode PNG/JPEG/QOI
images, and Canvas push/draw/invalidate/stop. See
`examples/usage/sim_bridge_media` for a runnable project.

- A List must have a compiled runtime row template.
- Grid template images require `dynamic_image: true`; Grid callbacks
  receive the actual resource and text slots.
- Row tokens can be reused until the row is recycled. Overflow is fatal
  and requires a session restart — it is not silently replayed.

### Canvas rendering

The bridge draws into a full native offscreen buffer during `poll`.
When the host reports `bridge_canvas_patch_version: 1`, incremental
dirty regions are transferred as packed row patches after the initial
keyframe; large or out-of-sync updates fall back to a full frame. This
is not the same as the device's render-task/tile callback model.

Inside a draw callback, only read-only GSP queries are allowed. Scene
changes disable the callback until it is re-registered.

**Synchronous push** (`canvas_push`) disables the target's draw callback
and, on success, invokes release once before returning. On failure the
caller retains ownership. Upload success does not imply render completion.

**Asynchronous push** (`canvas_try_push*`) places a borrowed pointer
into a local 8-frame FIFO without any socket I/O. On success the
target's local draw callback is disabled immediately. If the queue is
full, the call returns `TIMEOUT` without transferring ownership. The
frame buffer must remain immutable until released.

`poll` uploads queued frames; synchronous GSP RPC calls also drain
earlier frames to preserve ordering. Validation failures (wrong
target, stride, dirty region, or scene mismatch) can reject an
already-queued frame — a diagnostic is printed in that case. Each
admitted frame is released exactly once, whether after upload,
rejection, or local shutdown. Release does not confirm host
acceptance or display. Release callbacks from queued frames must not
mutate GSP state, call `poll`, or close the bridge; they may fire
before a synchronous RPC returns. The default entry point cancels
queued frames before app deinit; custom entry points must keep their
contexts alive until bridge close returns. Forced process termination
cannot guarantee that release callbacks run.

### Render fence

When `capabilities.bridge_fence_version` is `1`, the Backend-only
method `render_fence` is available (C wrapper: `esp_gsp_flush`). It
takes no parameters (`{}`) and responds with `{"result_code":0}` after
the host completes a subsequent step/render attempt. API and Browser
channels cannot call this method; use `wait` for frame-based delays on
the API channel.

Before calling flush, `poll` must finish all local Canvas
uploads/redraws; pending local work causes flush to return
`INVALID_STATE`. Flush does not execute business callbacks, wait for
animations, image publication, or browser presentation. An idle frame
with no visible changes can still satisfy the fence.

Timeout behavior:

- A finite timeout covers transport submission and the reply wait.
  `UINT32_MAX` waits indefinitely.
- A timeout of zero queues the fence locally and returns `TIMEOUT`
  immediately.
- On timeout, the fence and any partial traffic are retained for later
  `poll`/RPC processing — the request is not cancelled and the session
  remains valid.
- The native table holds at most 16 outstanding fences; a full table
  returns `TIMEOUT` without queuing.
- The timeout parameter on `poll` itself is only an idle wait, not an
  upload budget.

See `tools/sim_bridge/README.md` for the producer-pause / poll / flush
pattern.

### Image lifecycle

When `capabilities.bridge_image_version` is `1`, the bridge supports
extended image operations: COPY, BORROW, and TAKE semantics, plus
borrowed/owned helper functions, completion/release callbacks, and
cache keys. Regardless of source semantics, transport to the host
always uses COPY; a native borrowed or owned source is retained until
GSP actually releases the input.

- `image_complete` notifies the application of runtime publication,
  failure, or cancellation.
- `image_release` signals that the source data is no longer needed.
- Callbacks execute during `poll`, not inside the host render task.

At most 128 images may be tracked simultaneously. Exceeding this limit
causes immediate rejection — no ownership is transferred and no
callbacks are invoked. On local shutdown, images whose completion was
not yet observed are reported as `CANCELLED` (orderly close) or `IO`
(transport failure); these codes describe the bridge shutdown, not the
host's final treatment of the image. The default entry point settles
all pending callbacks before app deinit. Custom entry points must keep
their contexts alive until bridge close returns. Full details are in
`tools/sim_bridge/README.md`.

### Backend-only methods

The host exposes two additional Backend-only methods:

- `bridge_call` — fixed-width scalar queries, typed writes, animations,
  Canvas dimensions/format/scene queries, and component RESOURCE-bind
  lookup. No native pointers are accepted.
- `component_set_text` — write text addressed by component key.

Standard methods such as `set_value`, `set_text`, and `set_component_i32`
work unchanged on the Backend channel. The `capabilities` response
reports `bridge_version`, `bridge_media_version`, `bridge_image_version`,
`bridge_fence_version`, `bridge_keyboard_text_version`,
`bridge_canvas_patch_version`, and `current_scene` for initial negotiation.
When `bridge_keyboard_text_version` is `1`, the native bridge exposes the
same keyboard text readback contract: required capacity includes the trailing
NUL, and a short buffer returns a complete UTF-8 prefix with
`ESP_GSP_ERR_INVALID_SIZE`.

Applications typically use the C wrapper functions rather than encoding
these operations directly. Scalar operation IDs are defined in
`tools/sim_bridge/protocol.h`.

### Runner configuration

The runner script defaults to the component directory containing it
(normally `managed_components/espressif__esp-gsp`) and writes build
artifacts under the application's working directory. The GSPC version
comes from `.gspc_version`; the simulator version comes from the
component's `idf_component.yml`. Both tools are invoked through the
installed `esp-gsp-tools` manager.

Environment variable overrides:

| Variable | Purpose |
|---|---|
| `GSPC_EXECUTABLE` / `GSP_SIM_EXECUTABLE` | Override tool paths |
| `GSP_SIM_VERSION` | Override the managed simulator version |
| `ESP_GSP_COMPONENT_DIR` | Override the component path |

CLI equivalents take precedence over environment variables. The runner
never selects a binary implicitly from `PATH` or a development build
directory. See the bridge README for direct CMake and
unpacked-component use.

`sim_host --capabilities` reports `keyboard_text_version` for the loaded WASM
and `dynamic_backend_loader_supported` for the host process. When the loader
value is `false`, `sim_bridge/run.py` automatically selects the Socket Backend
instead of attempting to load a native shared library.

### Ready file

The runner passes `--ready-file <PATH>` to the simulator, which writes
a JSON file containing `version`, `pid`, `backend_mode`, `backend`,
`backend_library`, `api`, `browser`, `bridge_version`, and
`dynamic_backend_abi_version`. Disabled endpoints and the `api` field for stdio API appear as `null`.
The PID belongs to the simulator process, not its Python tool-manager
parent.

Callers should:

- Use a fresh, per-launch file path.
- Verify process ownership of the PID.
- Tolerate a partially written file during startup.
- Clean up the file after the simulator exits.

The ready file signals that listeners are bound, not that application
initialization is complete or that the process is still alive.
Backend-only headless runs advance at wall-clock pace to match native
timers.

## Run on the board

Use the simulator to check layout, text and interaction before flashing. Check panel
wiring, orientation, byte order, touch mapping and performance on the target board;
see [Display integration](../guide/display.md).
