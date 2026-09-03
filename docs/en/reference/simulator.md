# Simulator Reference

`gsp_sim_host` is the standalone simulator for ESP-GSP. It runs the compiled
WASI runtime on the development host and provides an interactive browser
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
| Log panel | Tabbed view of callback events, host logs, and C/WASI stderr output |

When a backend is attached, scene navigation buttons are rejected by the
server. When `--input-mode` is `api-exclusive`, mouse clicks are rejected
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
| `--fail-on-error` | Exit non-zero when WASI emits `E (...)` error log |

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
| `goto_scene` | `{ "scene": N }` | Switch to scene by 0-based index |
| `reset` | — | Reload the scene bundle |
| `set_value` | `{ "bind_id": N, "value": N }` | Set an integer binding |
| `set_text` | `{ "bind_id": N, "text": "..." }` | Set a text binding |
| `set_color` | `{ "bind_id": N, "color": N }` | Set a color binding |
| `set_visible` | `{ "bind_id": N, "visible": bool }` | Toggle component visibility |
| `set_component_i32` | `{ "component_key": N, "property_key": N, "value": N }` | Set component property by key |
| `keyboard_attach` | `{ "action_id": N, "text_bind": N }` | Attach virtual keyboard |
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

### Notification events

Events are delivered as JSON-RPC notifications (no `id`). Register with
`subscribe` first; the API channel has no default subscriptions.

| Event | Parameters | Description |
|---|---|---|
| `scene_changed` | `{ "from": N, "to": N }` | Scene transition |
| `callback` | `{ "action_id": N, "arg": N, "scene_id": N, "list": N, "item": N }` | Component callback |
| `frame` | `{ "index": N }` | Per-frame tick |
| `backend_state` | `{ "state": "attached"\|"detached" }` | Backend connection change |
| `log` | `{ "level": "...", "message": "..." }` | Host log message |
| `wasi_log` | `{ "message": "..." }` | C/WASI stderr output |

### Error codes

| Code | Meaning | Description |
|---|---|---|
| `-32600` | Invalid Request | Missing `jsonrpc: "2.0"` |
| `-32601` | Method Not Found | Unknown method |
| `-32601` | Method Not Allowed | Backend channel called a disallowed method |
| `-32601` | Backend Exclusive | Non-backend channel called `goto_scene` or `reset` while backend is active |
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

When a backend is attached:

- `goto_scene` and `reset` become backend-exclusive; other channels receive
  `backend_exclusive` errors.
- The backend automatically subscribes to `callback` and `scene_changed`.
- Only one backend connection is allowed at a time.

Use `--backend-required` to pause frame progression until the backend
connects. Use `--backend-idle-timeout <SECS>` to set a connection timeout.

The backend may call: `ping`, `capabilities`, `set_value`, `set_color`,
`set_visible`, `set_text`, `set_component_i32`, `keyboard_attach`,
`set_swipe_enabled`, `goto_scene`, `reset`, `subscribe`, `unsubscribe`.
Other methods (e.g. `tap`, `screenshot`, `quit`) return `method not allowed`.

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

## Validation boundary

A simulator render proves portable scene behavior. It does not prove panel
wiring, byte order, rotation, transfer timing, tearing behavior, touch
mapping, or visual acceptance on the product display. Report host preview,
target build, board execution, and human visual acceptance separately.
