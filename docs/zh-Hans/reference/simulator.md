# 模拟器参考

ESP-GSP 的独立模拟器以预编译可执行文件提供，
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
| 日志面板 | 分标签显示回调事件、宿主日志和运行时诊断 |

启用 Backend 时，场景导航按钮和应用状态写操作会被服务端拒绝，并报
`backend_exclusive` 错误。Browser 仍可发送点击、拖拽等输入并接收实时帧。当
`--input-mode` 为 `api-exclusive` 时，鼠标点击被拒绝并报 `input_busy` 错误。
这些情况均在日志面板中显示。

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
| `--fail-on-error` | 运行时输出 `E (...)` 错误日志时以非 0 退出 |

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
| `goto_scene` | `{ "scene": N, "transition": N }` | 切换到 0-based 索引的场景；`transition` 可省略 |
| `reset` | — | 重新加载场景包 |
| `set_value` | `{ "bind_id": N, "value": N }` | 设置绑定的整数值 |
| `set_text` | `{ "bind_id": N, "text": "..." }` | 设置文本绑定 |
| `set_color` | `{ "bind_id": N, "color": N }` | 设置颜色绑定 |
| `set_visible` | `{ "bind_id": N, "visible": bool }` | 设置组件可见性 |
| `set_component_i32` | `{ "component_key": N, "property_key": N, "value": N }` | 通过键设置组件属性 |
| `keyboard_attach` | `{ "action_id": N, "text_bind": N }` | 关联虚拟键盘 |
| `set_cursor` | `{ "bind_id": N }` | 显示/隐藏文本光标；`65535` 隐藏 |
| `set_swipe_transition` | `{ "transition": N }` | 设置滑动切页视觉效果 |
| `set_swipe_fade_black_point` | `{ "drag_percent": N }` | 设置滑动淡黑阈值（1..99） |
| `drawer_open` / `drawer_close` | `{ "component_key": N, "animated": bool }` | 打开/关闭 Drawer |
| `drawer_is_open` | `{ "component_key": N }` | 查询 Drawer 状态 |
| `page_flow_set_page` | `{ "component_key": N, "page": N, "animated": bool }` | 设置 PageFlow 页面 |
| `list_bind_component` | `{ "component_key": N }` | 绑定固定条目的 List/Wheel，返回 `list` 句柄 |
| `list_snap` | `{ "list": N, "enable": bool }` | 开关 Wheel 行吸附 |
| `list_fling` | `{ "list": N, "velocity_px_s": N }` | 启动列表惯性滑动 |
| `list_scroll_to` | `{ "list": N, "offset_px": N }` | 设置列表绝对滚动偏移 |
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

API 通道保持仅 JSON-RPC。应用 Backend 还提供下文说明的异步动态数据与二进制媒体扩展；
Browser 不会获得这些数据源请求或二进制上传权限。

### 通知事件

事件以 JSON-RPC 通知的形式推送（无 `id` 字段）。需先通过 `subscribe` 注册；
API 通道默认不订阅任何事件。

| 事件 | 参数 | 说明 |
|---|---|---|
| `scene_changed` | `{ "from": N, "to": N }` | 场景切换 |
| `callback` | `{ "action_id": N, "arg": N, "scene_id": N, "list": N, "item": N }` | 组件回调 |
| `frame` | `{ "index": N }` | 每帧推送 |
| `list_bind` | `{ "list": N, "slot": N, "instance": N, "item": N, "resource_slot": N, "text_slot": N }` | Backend 动态 List/Grid 行请求；Grid 成员槽位，65535 表示缺失（List 两者均缺失） |
| `list_bind_overflow` | `{ "dropped": N, "level": "warn", "message": "..." }` | Backend 行请求丢失；原生桥接器要求重启会话 |
| `binary_result` | `{ "transfer_id": "...", "ok": bool, ... }` | Backend 二进制上传接受结果 |
| `backend_state` | `{ "state": "attached"\|"detached" }` | 后端连接/断开 |
| `log` | `{ "level": "...", "message": "..." }` | 宿主日志 |
| `wasi_log` | `{ "message": "..." }` | 运行时诊断（为兼容保留通知名称） |

### 错误码

| 错误码 | 含义 | 说明 |
|---|---|---|
| `-32600` | Invalid Request | 缺少 `jsonrpc: "2.0"` |
| `-32601` | Method Not Found | 未知方法名 |
| `-32601` | Method Not Allowed | 当前通道调用了不允许的方法 |
| `-32601` | Backend Exclusive | Backend 启用后，非 Backend 通道调用了应用状态写方法 |
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

启用 Backend 后：

- 应用状态写操作以及 `goto_scene`、`reset` 变为 Backend 专属方法；API 调用收到
  `backend_exclusive`，Browser 调用始终收到 `method not allowed`。
- 后端自动订阅 `callback`、`scene_changed`、`list_bind`、`list_bind_overflow`、`binary_result`、`image_complete` 和 `image_release`。
- 同一时间只允许一个后端连接。

使用 `--backend-required` 可在后端连接前暂停帧推进，`--backend-idle-timeout <SECS>`
设置等待超时。

Backend 可调用上方列出的状态和应用逻辑方法，包括光标/滑动策略、Drawer、PageFlow
和固定条目 List/Wheel。还可用 `list_bind_remote` / `grid_bind_remote` 绑定动态集合、
用 `list_set_total` 设置总数，并对 `list_bind` 通知调用带令牌校验的 `row_publish`。
业务处理期间行可能被回收；行更新返回非零结果码表示令牌已失效，必须丢弃。

Backend TCP/Unix 连接还支持 COPY 型 Content-Length 二进制上传：`X-GSP-Kind: image`
和 `row-image` 接受 PNG、JPEG、QOI；`canvas` 接受携带 stride、height（可选脏区）的完整
原始帧。Host 用 `binary_result` 确认接受结果；Direct Draw 回调仍在渲染任务内，远程
生产者应推帧而不是等待回调，`canvas_stop` 负责停止。上述扩展只对 Backend 开放；
原生 C 生产者建议使用组件中的 [bridge 库](../../../tools/sim_bridge/README.md)
处理帧传输和缓冲生命周期。模拟器能力位图以
`ESP_GSP_SIM_CAP_QOI`（bit 7，值 `128`）声明 QOI 解码支持。

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

## 原生 C Backend 工程

`tools/sim_bridge` 提供 `gsp_add_backend(target SOURCES ... SCENES ...)`，
将业务 C 源码与原生兼容库链接，自动生成 deployable bundle 和配套头文件。
`INCLUDE_DIRECTORIES` 可添加业务头文件目录，`PROFILE` 可覆盖默认 RGB565
profile。PC 适配文件实现 `gsp_bridge_app_init(ui)` 与
`gsp_bridge_app_deinit(ui)`；不用在 PC 端执行 ESP-LCD 启动或链接完整 GSP 渲染库。

启动方式与示例拆分见[复用 C UI 业务代码](../guide/simulator-preview.md#复用-c-ui-业务代码)。
已安装组件中的 [bridge README](../../../tools/sim_bridge/README.md) 列出完整支持范围；未实现的 C 函数会链接失败，
不会静默模拟成功。此版本不支持多线程直接调用、自动重连或 reset 后的状态重放。

`capabilities.bridge_media_version: 1` 还支持原生动态 List/Grid binder、行字段、
PNG/JPEG/QOI COPY 图片和 Canvas push/draw/invalidate/stop。
`examples/sim_bridge_media` 提供可运行示例。List 需要编译出的 runtime row
template；Grid 模板图片需 `dynamic_image: true`，回调会收到实际资源/文本槽位。
行令牌在回收失效前可重复使用；行请求溢出会使原生桥接会话失败，需重启而非自动重放。

Canvas draw 在 poll 时绘制完整本地离屏缓冲，再上传全帧；脏区失效也如此，不复现
设备端渲染任务/分块时序。draw 内仅允许只读 GSP 查询；场景切换后需重新注册回调。
同步 push 会停用该目标的 draw callback，成功时在返回前调用一次 release，失败时
所有权仍归调用方。上传成功不代表渲染完成。

`canvas_try_push*` 不执行网络 I/O，而是将借用指针放入本地 8 帧 FIFO；成功后立即
停用目标的本地 draw callback。队列满返回 TIMEOUT，不转移所有权。整个帧缓冲应
保持不可改写直到释放。poll 上传帧；后续同步 GSP RPC 也先排空此前帧以保持顺序。
目标/步长/脏区校验或场景不符可在入队后拒绝帧并打印诊断；无论上传、拒绝还是
本地关闭，已接收帧均恰好释放一次，release 不表示宿主接受/呈现。排队帧的 release
可在同步 RPC 前触发，不得写 GSP 状态、poll 或 close。默认入口在 app deinit 前
取消待上传帧；自定义入口保持上下文到 bridge close 返回。强制终止不保证回调。

`bridge_fence_version: 1` 新增仅限 Backend 的 `render_fence`（参数 `{}`，随后
一次宿主 step/渲染尝试后返回 `{"result_code":0}`），由 `esp_gsp_flush` 使用。
API/Browser 不开放此方法，API 的帧数等待仍用 `wait`。先 poll 完成本地 Canvas
上传/重绘，否则 flush 返回 INVALID_STATE。flush 不派发业务回调，不等待动画、
图片发布或浏览器呈现，空闲 step 无画面变化也能完成栅栏。有限超时覆盖其网络 I/O；
UINT32_MAX 无限等待。零超时只本地排队并返回 TIMEOUT。超时保留栅栏和半包，
后续 poll/RPC 继续处理，不取消请求或使连接失效。原生库最多保留 16 个未完成
栅栏，满时返回 TIMEOUT 且不入队。poll 自身的 timeout 只是空闲等待，不是上传
时限。生产者暂停/poll/flush 用法见 `tools/sim_bridge/README.md`。

`bridge_image_version: 1` 新增图片 EX COPY/BORROW/TAKE、borrowed/owned 辅助函数、
完成/释放回调和缓存键。传输仍为 COPY，原生 borrowed/owned 输入保留到 GSP 实际
释放。`image_complete` 报告运行时发布/失败/取消；`image_release` 结束输入保留。
回调在 poll 中执行，不在宿主渲染任务。最多保留 128 个跟踪请求；立即拒绝不转移
所有权、不触发回调。本地退出时，尚未收到完成结果的请求按主动关闭报告 CANCELLED，
按传输失败报告 IO，不据此断言宿主图片的最终状态。默认入口在 app deinit 前结清
回调；自定义入口须保持上下文到 bridge close 返回。完整语义见桥接库 README。

宿主补充两个仅限 Backend 的方法：

- `bridge_call`：固定宽度标量查询、类型化写入、动画、Canvas 尺寸/格式/场景信息及
  component RESOURCE bind 查询；不接受任何原生指针。
- `component_set_text`：通过组件键写文本。

原有 `set_value`、`set_text`、`set_component_i32` 等仍直接复用。
`capabilities` 返回 `bridge_version: 1`、`bridge_media_version: 1`、`bridge_image_version: 1` 和 `current_scene`
用于初始协商。
另有 `bridge_fence_version: 1` 表示渲染栅栏支持。应用通常直接使用 C 包装接口，
无需自行编码这些操作。标量操作编号随组件的 `tools/sim_bridge/protocol.h` 提供。

启动器默认使用脚本所属组件，通常位于 `managed_components/espressif__esp-gsp`，
构建输出在应用工作目录下。GSPC 版本由 `.gspc_version` 选择；模拟器版本由所选
组件的 `idf_component.yml` 选择，均调用已安装的 `esp-gsp-tools`。
`GSPC_EXECUTABLE` / `GSP_SIM_EXECUTABLE` 可覆盖工具路径；`GSP_SIM_VERSION`
覆盖管理器使用的模拟器版本；`ESP_GSP_COMPONENT_DIR` 覆盖组件路径。
对应命令行参数优先，不会隐式选择 PATH 或开发构建中的其他工具版本。
直接 CMake 和解压组件的用法见 bridge README。

启动器使用 `--ready-file <PATH>` 获取 JSON：
`version`、`pid`、`backend`、`browser`、`bridge_version`。
未启用的端点为 null。PID 属于模拟器而非 Python 管理器父进程。
调用方须使用每次启动独有的新路径，校验进程归属，容忍文件写入
过程并在结束时清理；就绪文件不表示业务初始化完成，也不是进程存活检查。
Backend-only headless 模式同样按墙钟节奏推进，便于与原生定时器配合。

## 验收边界

模拟器渲染可以证明场景的可移植行为，不能证明产品屏幕的接线、字节序、旋转、
传输时序、撕裂、触控映射或视觉验收。主机预览、目标构建、板上运行和人工视觉
验收应分别记录。
