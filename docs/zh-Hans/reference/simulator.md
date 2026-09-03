# 模拟器参考

`gsp_sim_host` 是 ESP-GSP 的独立模拟器。它在开发主机上运行编译后的 WASI 运行时，
提供交互式浏览器预览、CLI 脚本化输入和 JSON-RPC 自动化 API。

## 获取模拟器

模拟器不是固件构建依赖，需要单独获取。推荐通过 `esp-gsp-tools` 工具链管理器
安装和调用：

```sh
python -m pip install -U esp-gsp-tools
python -m gsp.execute --version '<ESP-GSP version>' sim --capabilities
```

`<ESP-GSP version>` 使用 `idf_component.yml` 中的 `version` 字段（ESP-GSP
组件版本），与 `.gspc_version` 中的 GSPC 版本相互独立。管理器会自动下载并缓存
指定版本。

也可以手动设置 `GSP_SIM_EXECUTABLE` 环境变量指向已有的模拟器可执行文件。

下文命令使用 `gsp_sim_host` 代表模拟器可执行文件。通过管理器调用时，替换为
`python -m gsp.execute --version '<ESP-GSP version>' sim`。

## 快速开始

```sh
# 交互式浏览器预览（持续运行）
gsp_sim_host --bundle app.gspb --frames 0

# Headless 截图
gsp_sim_host --bundle app.gspb --headless \
    --tap 120 80 --dump frame.png --dump-format png

# API 驱动自动化
gsp_sim_host --bundle app.gspb --frames 0 --api-enable

# 带应用后端的全栈模拟
gsp_sim_host --bundle app.gspb --frames 0 \
    --backend-listen tcp://127.0.0.1:9001 --api-enable
```

## 浏览器预览

宿主内嵌了轻量级 HTTP + WebSocket 服务器。未指定 `--headless` 时预览自动启动，
URL 输出到 stderr：

```
gsp_sim_host: browser preview listening at http://127.0.0.1:3222/
```

| 功能 | 说明 |
|---|---|
| 实时画面 | Canvas 渲染通过 WebSocket 接收的帧数据 |
| 指针输入 | 鼠标点击和拖拽映射为模拟器输入 |
| 缩放控制 | 支持 25 %–400 % 视口缩放 |
| 场景导航 | 上一场景（◀）、下一场景（▶）、重置（↺）按钮 |
| 日志面板 | 分标签显示回调事件、宿主日志和 C/WASI stderr 输出 |

启用后端时，场景导航按钮的操作会被服务端拒绝。当 `--input-mode` 为 `api-exclusive`
时，鼠标点击被拒绝并报 `input_busy` 错误。两种情况均在日志面板中显示。

支持多个浏览器同时连接；每个连接独立维护关键帧状态和订阅列表。

## 命令行参数参考

运行 `gsp_sim_host --help` 可获取最新内置帮助。

### 场景与时序

| 参数 | 说明 |
|---|---|
| `--bundle <PATH>` | 场景包文件（`.gspb`），必需 |
| `--dynamic-font <PATH>` | 可选 TrueType 字体文件 |
| `--fps <N>` | 模拟帧率，默认 `60` |
| `--frames <N>` | 运行帧数上限；`0` 表示无限运行直到 `quit`；默认 `300` |

### 预览与输入

| 参数 | 说明 |
|---|---|
| `--headless` | 不启动浏览器预览 |
| `--visual-listen <ADDR>` | 预览绑定地址；与 `--headless` 互斥 |
| `--input-mode <MODE>` | 输入归属：`shared`（默认）/ `api-exclusive` / `browser-exclusive` |
| `--fail-on-error` | WASI 输出 `E (...)` 错误日志时以非 0 退出 |

### 脚本化操作

| 参数 | 说明 |
|---|---|
| `--tap <X> <Y>` | 在坐标 (X, Y) 点按，可重复 |
| `--drag <X1> <Y1> <X2> <Y2>` | 从 (X1,Y1) 拖拽到 (X2,Y2)，可重复 |
| `--wait <FRAMES>` | 在下一个脚本动作前等待 N 帧，可重复 |

脚本操作按在命令行中出现的顺序依次执行。同样的输入命令也可以在运行时通过 API 通道
发送。

### 输出与连接

| 参数 | 说明 |
|---|---|
| `--dump <PATH>` | 运行结束时保存最后一帧画面 |
| `--dump-format <FMT>` | 截图格式：`ppm`（默认）/ `png` / `raw` |
| `--api-enable` | 启用 API 控制通道（默认 stdio） |
| `--api-listen <URL>` | API 端点；隐含 `--api-enable` |
| `--backend-enable` | 在随机本地端口启用应用后端 |
| `--backend-listen <URL>` | 应用后端端点；隐含 `--backend-enable` |
| `--backend-required` | 在后端连接之前暂停帧推进 |
| `--backend-idle-timeout <SECS>` | 等待后端连接的超时秒数（`0` = 无限等待） |
| `--log-level <LEVEL>` | `trace` / `debug` / `info`（默认）/ `warn` / `error` |
| `--log-file <PATH>` | 将日志写入文件而非 stderr |

### 诊断

| 参数 | 说明 |
|---|---|
| `--capabilities` | 输出模拟器支持的功能集并退出 |

### 高级调优

| 参数 | 说明 |
|---|---|
| `--invalidate-every-frame` | 每帧强制全屏重绘 |
| `--instance-slots <N>` | 覆盖场景包中的组件实例上限（`0` = 使用包内值） |
| `--dynamic-image-slots <N>` | 覆盖场景包中的运行时图片上限（`0` = 使用包内值） |
| `--idle-poll-ms <N>` | 空闲轮询间隔毫秒数（`0` = 使用默认值） |

## 输入模式

当 API 通道和浏览器同时连接时，`--input-mode` 决定哪一端可以注入指针输入
（`tap`、`drag`、`feed_pointer`、`feed_touch`）：

| 模式 | 浏览器 | API 通道 | 说明 |
|---|---|---|---|
| `shared` | ✓ | ✓ | 先到先服务（默认） |
| `api-exclusive` | ✗ `input_busy` | ✓ | 浏览器点击被拒绝 |
| `browser-exclusive` | ✓ | ✗ `input_busy` | API 输入请求被拒绝 |

后端通道始终不能发送输入命令，不受此模式影响。

## 端口分配

自动分配端口（未指定端口或端口为 `0`）时，宿主在以下优选范围内顺序尝试：

| 通道 | 优选端口范围 | 手动指定方式 |
|---|---|---|
| 浏览器预览 | 3222–3231 | `--visual-listen 127.0.0.1:PORT` |
| API 控制 | 8266–8275 | `--api-listen tcp://127.0.0.1:PORT` |
| 应用后端 | 8684–8693 | `--backend-listen tcp://127.0.0.1:PORT` |

优选范围内所有端口均被占用时，宿主报错退出。

## 自动化 API

API 控制通道支持脚本驱动和 AI agent 自动化，使用 JSON-RPC 2.0 协议。
通过 `--api-enable`（默认 stdio）或 `--api-listen <URL>` 启用。
支持的传输方式：stdio、回环 TCP（`tcp://127.0.0.1:PORT`）、Unix 套接字
（`unix:///path/to/sim.sock`）。

### 方法

#### 输入

| 方法 | 参数 | 说明 |
|---|---|---|
| `tap` | `{ "x": N, "y": N }` | 模拟点按 |
| `drag` | `{ "x1": N, "y1": N, "x2": N, "y2": N }` | 模拟拖拽手势 |
| `feed_pointer` | `{ "x": N, "y": N, "pressed": bool }` | 低级指针输入 |
| `feed_touch` | `{ "id": N, "x": N, "y": N, "pressed": bool }` | 低级多点触控输入 |

#### 场景与状态

| 方法 | 参数 | 说明 |
|---|---|---|
| `goto_scene` | `{ "scene": N }` | 切换到 0-based 索引的场景 |
| `reset` | — | 重新加载场景包 |
| `set_value` | `{ "bind_id": N, "value": N }` | 设置绑定的整数值 |
| `set_text` | `{ "bind_id": N, "text": "..." }` | 设置文本绑定 |
| `set_color` | `{ "bind_id": N, "color": N }` | 设置颜色绑定 |
| `set_visible` | `{ "bind_id": N, "visible": bool }` | 设置组件可见性 |
| `set_component_i32` | `{ "component_key": N, "property_key": N, "value": N }` | 通过键设置组件属性 |
| `keyboard_attach` | `{ "action_id": N, "text_bind": N }` | 关联虚拟键盘 |
| `set_swipe_enabled` | `{ "enabled": bool }` | 启用或禁用滑动切页 |
| `fling_messages` | `{ "velocity_px_s": N }` | 模拟列表快速滑动 |

#### 辅助

| 方法 | 参数 | 说明 |
|---|---|---|
| `ping` | — | 心跳检测；返回 `"pong"` |
| `capabilities` | — | 查询画面尺寸、场景数和运行时 ABI |
| `frame_info` | — | 获取最近一帧的提交信息 |
| `wait` | `{ "frames": N }` | 延迟 N 帧后响应 |
| `invalidate` | — | 强制下一帧全屏重绘 |
| `screenshot` | `{ "path": "...", "format": "png" }` | 保存当前画面到文件 |
| `subscribe` | `{ "events": [...] }` | 订阅服务端通知 |
| `unsubscribe` | `{ "events": [...] }` | 取消订阅 |
| `quit` | — | 请求模拟器退出 |

### 通知事件

事件以 JSON-RPC 通知的形式推送（无 `id` 字段）。需先通过 `subscribe` 注册；
API 通道默认不订阅任何事件。

| 事件 | 参数 | 说明 |
|---|---|---|
| `scene_changed` | `{ "from": N, "to": N }` | 场景切换 |
| `callback` | `{ "action_id": N, "arg": N, "scene_id": N, "list": N, "item": N }` | 组件回调 |
| `frame` | `{ "index": N }` | 每帧推送 |
| `backend_state` | `{ "state": "attached"\|"detached" }` | 后端连接/断开 |
| `log` | `{ "level": "...", "message": "..." }` | 宿主日志 |
| `wasi_log` | `{ "message": "..." }` | C/WASI stderr 输出 |

### 错误码

| 错误码 | 含义 | 说明 |
|---|---|---|
| `-32600` | Invalid Request | 缺少 `jsonrpc: "2.0"` |
| `-32601` | Method Not Found | 未知方法名 |
| `-32601` | Method Not Allowed | 后端调用了不在允许列表中的方法 |
| `-32601` | Backend Exclusive | 后端启用时，非后端通道调用了 `goto_scene` 或 `reset` |
| `-32602` | Invalid Params | 参数缺失或类型错误 |
| `-32603` | Internal Error | 宿主内部错误 |
| `-32010` | Input Busy | `--input-mode` 阻止了当前通道发送输入 |

### Python 客户端示例

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
print(f"画面尺寸: {caps['result']['width']}x{caps['result']['height']}")

send_request(sock, "tap", {"x": 160, "y": 120}, request_id=2)
print(read_response(sock))

send_request(sock, "wait", {"frames": 5}, request_id=3)
print(read_response(sock))

send_request(sock, "screenshot", {"path": "/tmp/shot.png", "format": "png"}, request_id=4)
print(read_response(sock))
sock.close()
```

## 应用后端

后端通道允许应用业务逻辑在模拟过程中驱动 UI，复现固件 C API 的交互行为。
通过 `--backend-listen <URL>` 启用（TCP 或 Unix 套接字；不支持 stdio）。

后端连接后：

- `goto_scene` 和 `reset` 变为后端专属方法，其他通道调用收到
  `backend_exclusive` 错误。
- 后端自动订阅 `callback` 和 `scene_changed`。
- 同一时间只允许一个后端连接。

使用 `--backend-required` 可在后端连接前暂停帧推进，`--backend-idle-timeout <SECS>`
设置等待超时。

后端可调用的方法：`ping`、`capabilities`、`set_value`、`set_color`、
`set_visible`、`set_text`、`set_component_i32`、`keyboard_attach`、
`set_swipe_enabled`、`goto_scene`、`reset`、`subscribe`、`unsubscribe`。
调用其他方法（如 `tap`、`screenshot`、`quit`）将返回 `method not allowed`。

### 后端 Python 示例

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
            print(f"回调: action_id={params.get('action_id')}")

backend = SimBackend(port=8684)
caps = backend.call("capabilities")
print(f"场景数: {caps['scene_count']}")

backend.call("set_text", {"bind_id": 1, "text": "Ready"})
backend.call("set_value", {"bind_id": 2, "value": 75})

while True:
    msg = backend._read()
    if msg.get("method"):
        backend._on_notification(msg)
```

## 验收边界

模拟器渲染可以证明场景的可移植行为，不能证明产品屏幕的接线、字节序、旋转、
传输时序、撕裂、触控映射或视觉验收。主机预览、目标构建、板上运行和人工视觉
验收应分别记录。
