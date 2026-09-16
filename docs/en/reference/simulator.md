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

Multiple browsers may connect simultaneously; each maintains independent
keyframe state and subscriptions.

## Command-line reference

Run `gsp_sim_host --help` for the built-in help.

### Scene and timing

| Parameter | Description |
|---|---|
| `--bundle <PATH>` | Scene bundle file (`.gspb`), required |
| `--dynamic-font <PATH>` | Optional TrueType font file |
| `--fps <N>` | Simulated frame rate, default `60` |
| `--frames <N>` | Frame limit; `0` = run until `quit`; default `300` |

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
| `--wait <FRAMES>` | Wait N frames before the next scripted action; repeatable |

Scripted actions execute in command-line order. The same input commands are
also available through the API channel at runtime.

### Output and connections

| Parameter | Description |
|---|---|
| `--dump <PATH>` | Save the last frame at exit |
| `--dump-format <FMT>` | Screenshot format: `ppm` (default) / `png` / `raw` |
| `--api-enable` | Enable API control channel (defaults to stdio) |
| `--api-listen <URL>` | API endpoint; implies `--api-enable` |
| `--backend-enable` | Enable application backend on a random local port |
| `--backend-listen <URL>` | Backend endpoint; implies `--backend-enable` |
| `--backend-required` | Pause frame progression until backend connects |
| `--backend-idle-timeout <SECS>` | Backend connection wait timeout (`0` = no limit) |
| `--log-level <LEVEL>` | `trace` / `debug` / `info` (default) / `warn` / `error` |
| `--log-file <PATH>` | Write logs to file instead of stderr |

### Diagnostics

| Parameter | Description |
|---|---|
| `--capabilities` | Print the simulator capability set and exit |

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

### Methods

#### Input

| Method | Parameters | Description |
|---|---|---|
| `tap` | `{ "x": N, "y": N }` | Simulate a tap |
| `drag` | `{ "x1": N, "y1": N, "x2": N, "y2": N }` | Simulate a drag gesture |
| `feed_pointer` | `{ "x": N, "y": N, "pressed": bool }` | Low-level pointer input |
| `feed_touch` | `{ "id": N, "x": N, "y": N, "pressed": bool }` | Low-level multi-touch input |

#### Scene and state

| Method | Parameters | Description |
|---|---|---|
| `goto_scene` | `{ "scene": N, "transition": N }` | Switch to a 0-based scene; `transition` is optional |
| `reset` | — | Reload the scene bundle |
| `set_value` | `{ "bind_id": N, "value": N }` | Set an integer binding |
| `set_text` | `{ "bind_id": N, "text": "..." }` | Set a text binding |
| `set_color` | `{ "bind_id": N, "color": N }` | Set a color binding |
| `set_visible` | `{ "bind_id": N, "visible": bool }` | Toggle component visibility |
| `set_component_i32` | `{ "component_key": N, "property_key": N, "value": N }` | Set component property by key |
| `keyboard_attach` | `{ "action_id": N, "text_bind": N }` | Attach virtual keyboard |
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

#### Utility

| Method | Parameters | Description |
|---|---|---|
| `ping` | — | Heartbeat; returns `"pong"` |
| `capabilities` | — | Query display size, scene count, and runtime ABI |
| `frame_info` | — | Get latest frame commit info |
| `wait` | `{ "frames": N }` | Respond after N frames |
| `invalidate` | — | Force full-screen redraw next frame |
| `screenshot` | `{ "path": "...", "format": "png" }` | Save current frame to file |
| `subscribe` | `{ "events": [...] }` | Subscribe to server notifications |
| `unsubscribe` | `{ "events": [...] }` | Unsubscribe from notifications |
| `quit` | — | Request simulator exit |

The API channel remains JSON-only. The application backend additionally has
asynchronous dynamic-data and binary-media extensions described below; the
Browser never receives their data-source requests or binary-upload access.

### Notification events

Events are delivered as JSON-RPC notifications (no `id`). Register with
`subscribe` first; the API channel has no default subscriptions.

| Event | Parameters | Description |
|---|---|---|
| `scene_changed` | `{ "from": N, "to": N }` | Scene transition |
| `callback` | `{ "action_id": N, "arg": N, "scene_id": N, "list": N, "item": N }` | Component callback |
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

### Python client example

```python
import json, socket

def connect(host="127.0.0.1", port=8266):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    return sock

def send_request(sock, method, params=None, request_id=1):
    body = json.dumps({
        "jsonrpc": "2.0",
        "id": request_id,
        "method": method,
        "params": params or {},
    }).encode("utf-8")
    header = f"Content-Length: {len(body)}\r\n\r\n".encode("ascii")
    sock.sendall(header + body)

def read_response(sock):
    buf = b""
    while b"\r\n\r\n" not in buf:
        chunk = sock.recv(4096)
        if not chunk:
            raise ConnectionError("connection closed")
        buf += chunk
    header, _, rest = buf.partition(b"\r\n\r\n")
    length = int(header.split(b":")[1].strip())
    body = rest
    while len(body) < length:
        body += sock.recv(length - len(body))
    return json.loads(body[:length])

sock = connect(port=8266)
send_request(sock, "capabilities", request_id=1)
caps = read_response(sock)
print(f"Display: {caps['result']['width']}x{caps['result']['height']}")

send_request(sock, "tap", {"x": 160, "y": 120}, request_id=2)
print(read_response(sock))

send_request(sock, "wait", {"frames": 5}, request_id=3)
print(read_response(sock))

send_request(sock, "screenshot", {"path": "/tmp/shot.png", "format": "png"}, request_id=4)
print(read_response(sock))
sock.close()
```

## Application backend

The backend channel allows application business logic to drive the UI during
simulation, mirroring the firmware C API. Enable with
`--backend-listen <URL>` (TCP or Unix socket; stdio is not supported).

When the backend endpoint is enabled:

- Application state writes (`set_*`, component, Drawer, PageFlow and List
  controls) plus `goto_scene` and `reset` become backend-exclusive. API calls
  receive `backend_exclusive`; Browser calls are always `method not allowed`.
- The backend automatically subscribes to `callback`, `scene_changed`, `list_bind`, `list_bind_overflow`, `binary_result`, `image_complete`, and `image_release`.
- Only one backend connection is allowed at a time.

Use `--backend-required` to pause frame progression until the backend
connects. Use `--backend-idle-timeout <SECS>` to set a connection timeout.

The backend may call the state and application-logic methods listed above,
including cursor/swipe policy, Drawer and PageFlow controls, and fixed-item
List/Wheel operations. It may also bind dynamic collections with
`list_bind_remote` / `grid_bind_remote`, set their size with `list_set_total`,
and answer `list_bind` notifications with token-checked `row_publish` updates.
Rows can be recycled while a backend is working: a nonzero row result code
means the token is stale and must be discarded.

Backend TCP/Unix connections accept COPY-only Content-Length binary uploads:
`X-GSP-Kind: image` and `row-image` accept PNG, JPEG, or QOI; `canvas` accepts
a complete raw frame with stride and height headers (optional dirty rectangle).
The host replies with `binary_result`; direct-draw callbacks stay local to the
render task, so a remote producer pushes frames rather than waiting for a
callback. `canvas_stop` stops a producer. These extensions are Backend-only;
native C producers should use the component's
[bridge library](../../../tools/sim_bridge/README.md) to handle framing and
buffer lifetimes. The simulator
capability bitmap advertises this decoder support as `ESP_GSP_SIM_CAP_QOI`
(bit 7, value `128`).

### Backend Python example

```python
import json, socket

class SimBackend:
    def __init__(self, host="127.0.0.1", port=8684):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
        self.next_id = 1

    def call(self, method, params=None):
        rid = self.next_id
        self.next_id += 1
        body = json.dumps({
            "jsonrpc": "2.0", "id": rid,
            "method": method, "params": params or {},
        }).encode("utf-8")
        header = f"Content-Length: {len(body)}\r\n\r\n".encode("ascii")
        self.sock.sendall(header + body)
        while True:
            msg = self._read()
            if msg.get("id") == rid:
                return msg.get("result")
            self._on_notification(msg)

    def _read(self):
        buf = b""
        while b"\r\n\r\n" not in buf:
            chunk = self.sock.recv(4096)
            if not chunk:
                raise ConnectionError("closed")
            buf += chunk
        header, _, rest = buf.partition(b"\r\n\r\n")
        length = int(header.split(b":")[1].strip())
        body = rest
        while len(body) < length:
            body += self.sock.recv(length - len(body))
        return json.loads(body[:length])

    def _on_notification(self, msg):
        method = msg.get("method", "")
        params = msg.get("params", {})
        if method == "callback":
            print(f"callback: action_id={params.get('action_id')}")

backend = SimBackend(port=8684)
caps = backend.call("capabilities")
print(f"Scenes: {caps['scene_count']}")

backend.call("set_text", {"bind_id": 1, "text": "Ready"})
backend.call("set_value", {"bind_id": 2, "value": 75})

while True:
    msg = backend._read()
    if msg.get("method"):
        backend._on_notification(msg)
```

## Native C Backend projects

`tools/sim_bridge` provides `gsp_add_backend(target SOURCES ... SCENES ...)`
to link business C sources with the native compatibility library and
generate a matching deployable bundle/header pair. Use
`INCLUDE_DIRECTORIES` for business headers and `PROFILE` to override the
default RGB565 profile. A PC adapter implements `gsp_bridge_app_init(ui)`
and `gsp_bridge_app_deinit(ui)`; it does not start ESP-LCD or link the full
GSP renderer.

See [Reuse C UI business logic](../guide/simulator-preview.md#reuse-c-ui-business-logic)
for examples. The installed component's [bridge README](../../../tools/sim_bridge/README.md) lists supported
C functions. Unimplemented functions fail at link time instead of silently
succeeding. This version does not support concurrent calling threads,
automatic reconnection or state replay after reset.

With `capabilities.bridge_media_version: 1`, the library also maps dynamic
List/Grid binders, row fields, COPY PNG/JPEG/QOI images, and Canvas
push/draw/invalidate/stop. See `examples/sim_bridge_media` for a runnable
project. A List must have a compiled runtime row template; Grid template
images need `dynamic_image: true`. Grid callbacks receive actual resource/text
slots. Row tokens are reusable until invalidated by recycling; overflow is
fatal, not silently replayed.

Canvas draw runs on a full native offscreen buffer at poll time, followed by
full-frame upload, including for dirty invalidation. It is not the device's
render-task/tile callback contract. Only read-only GSP queries are permitted
inside draw; scene changes disable callbacks until re-registered. Explicit
sync push disables that target's callback and invokes release once before
returning on success; failure retains caller ownership. Upload success is
not render completion.

`canvas_try_push*` instead admits a borrowed pointer into a local eight-frame
FIFO with no socket I/O; success immediately disables the target's local draw
callback. Queue-full returns TIMEOUT without ownership transfer. Keep the
entire frame immutable until release. Poll uploads frames; synchronous GSP
RPC calls also drain earlier frames to preserve order. Target/stride/dirty
validation and scene mismatch can reject an already admitted frame, with a
diagnostic. Release runs exactly once after upload, rejection or local shutdown,
not as confirmation of host acceptance/display. Queued releases must not
mutate GSP, poll or close; they may run before a synchronous RPC. The default
main cancels queued frames before app deinit; custom mains keep their contexts
alive through bridge close. Forced termination cannot guarantee callbacks.

`bridge_fence_version: 1` adds Backend-only `render_fence` (`{}` →
`{"result_code":0}` after a subsequent host step/render attempt), used by
`esp_gsp_flush`. API/Browser cannot invoke it; API frame waits remain `wait`.
First poll to finish local Canvas uploads/redraws: pending work makes flush
return INVALID_STATE. Flush does not execute business callbacks, wait for
animations/image publication/browser presentation, or require an idle frame
to change. A finite timeout covers its socket I/O; UINT32_MAX waits indefinitely.
Zero queues locally and returns TIMEOUT. Timeout retains the fence and partial
traffic for later poll/RPC, without cancelling it or invalidating the session.
The native table holds at most 16 outstanding fences; full returns TIMEOUT
without admission. Poll's own timeout is only an idle wait, not an upload budget.
See `tools/sim_bridge/README.md` for the producer-pause/poll/flush pattern.

`bridge_image_version: 1` adds image EX COPY/BORROW/TAKE, borrowed/owned helpers,
completion/release callbacks and cache keys. Transport remains COPY; the
native borrowed/owned source is retained until actual GSP input release.
`image_complete` reports runtime publication/failure/cancellation;
`image_release` ends source retention. Callbacks execute at poll time, not in
the host render task. Only 128 tracked images may remain retained; immediate
rejection transfers no ownership and invokes no callbacks. On local shutdown,
unobserved completion becomes CANCELLED (orderly close) or IO (transport
failure), not an assertion about the host's final image. Default main settles
these callbacks before app deinit. Custom applications keep contexts alive
through bridge close. Full details are in `tools/sim_bridge/README.md`.

The host adds two Backend-only methods:

- `bridge_call`: fixed-width scalar queries, typed writes, animations,
  Canvas dimensions/format/scene and component RESOURCE-bind lookup;
  no native pointers are accepted.
- `component_set_text`: text updates addressed by component key.

Existing methods such as `set_value`, `set_text` and `set_component_i32`
are reused directly. `capabilities` reports `bridge_version: 1`,
`bridge_media_version: 1`, `bridge_image_version: 1` and
`bridge_fence_version: 1` and `current_scene` for initial negotiation.
Applications normally use the C wrappers rather than encode these operations.
The component includes `tools/sim_bridge/protocol.h` for the scalar operation IDs.

The runner defaults to the component containing the script, normally
`managed_components/espressif__esp-gsp`, and writes builds under the application's
working directory. GSPC uses `.gspc_version`; the simulator uses the selected
component's `idf_component.yml` version. Both are invoked through the installed
`esp-gsp-tools` manager. `GSPC_EXECUTABLE` / `GSP_SIM_EXECUTABLE` override tool
paths; `GSP_SIM_VERSION` overrides the managed simulator version;
`ESP_GSP_COMPONENT_DIR` overrides the component path. CLI equivalents take
precedence. No product binary is selected implicitly from PATH or a development
build. See the bridge README for direct CMake and unpacked-component use.

The runner uses `--ready-file <PATH>` to obtain JSON fields `version`,
`pid`, `backend`, `browser` and `bridge_version`; disabled endpoints
are null. The PID belongs to the simulator, not its Python tool-manager parent.
Callers must use a fresh per-launch path, verify process ownership, tolerate
a partially written file, and clean it up afterwards. Readiness means bound
listeners, not completed application initialization or process liveness.
Backend-only headless runs are wall-clock paced to match native timers.

## Run on the board

Use the simulator to check layout, text and interaction before flashing. Check panel
wiring, orientation, byte order, touch mapping and performance on the target board;
see [Display integration](../guide/display.md).
