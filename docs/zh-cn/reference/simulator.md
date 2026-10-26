# 模拟器参考

ESP-GSP 的独立模拟器以预编译可执行文件提供，支持交互式浏览器预览、CLI 脚本化
输入和 JSON-RPC 自动化 API。

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

宿主内嵌了轻量的 HTTP + WebSocket 服务器。未指定 `--headless` 时预览自动启动，
URL 输出到 stderr：

```
gsp_sim_host: browser preview listening at http://127.0.0.1:3222/
```

| 功能 | 说明 |
|---|---|
| 实时画面 | Canvas 渲染通过 WebSocket 接收的帧数据 |
| 指针输入 | 鼠标点击和拖拽映射为模拟器输入 |
| 缩放控制 | 支持 25%–400% 视口缩放 |
| 场景导航 | 上一场景（◀）、下一场景（▶）、重置（↺）按钮 |
| 日志面板 | 分标签显示回调事件、宿主日志和运行时诊断 |

启用 Backend 时，场景导航按钮会被服务端以 `backend_exclusive` 拒绝。
Browser 的应用状态写操作始终被以 `not_allowed` 拒绝。当 `--input-mode` 为 `api-exclusive` 时，鼠标点击被
拒绝并报 `input_busy` 错误。这些情况都会在日志面板中显示。

Browser 仍可发送点击、拖拽等输入并接收实时帧。支持多个浏览器同时连接，每个连接
独立维护关键帧状态和订阅列表。

宿主只打印 URL，**不会自动打开浏览器**。手动打开该地址即可。关闭浏览器不会停止
宿主——用 Ctrl-C 或控制 API 的 `quit` 退出。

## 命令行参数参考

运行 `gsp_sim_host --help` 可获取最新的内置帮助。

bundle 的预编译 GFB 字形能覆盖所需文本时，可直接使用，包括运行时修改的文本。
`--dynamic-font` 是可选的缺字回退；GFB 目录同样使用预编译字形，无需原始字体。

### 场景与时序

| 参数 | 说明 |
|---|---|
| `--bundle <PATH>` | 场景包文件（`.gspb`）；能力或内嵌运行时信息查询以外必需 |
| `--dynamic-font <PATH>` | 可选的缺字回退字体，使用 TrueType 或 CFF 可缩放轮廓（`.ttf`、`.otf` 或 `.ttc` 的首个 face）；使用 `--capabilities` 查询编入的 driver |
| `--font-catalog <PATH>` | 启动前加载的 GSPB 字体目录（`gspc font-link`） |
| `--record-input <PATH>` | 将实际消费的触摸样本录制到新 JSONL 文件；已有文件会被拒绝 |
| `--replay-input <PATH>` | 回放匹配的 JSONL；与录制及 CLI tap/drag/wait 互斥 |
| `--fps <N>` | 模拟帧率，默认 `60` |
| `--frames <N>` | 运行帧数上限，默认 `0` 无限制；脚本及回放退出规则见下文 |

1.8.0 的 WASI 运行时包含 TrueType 和 CFF driver。CLI/RPC capabilities 的
`font_outline_drivers` 报告注册的 TrueType/CFF driver，例如 `["truetype", "cff"]`；
`null` 表示旧运行时没有该查询或探测失败，`[]` 表示未启用这两类 driver。
旧版模拟器可能不含 CFF。CFF driver 可读取 CFF/CFF2 轮廓，但该接口不提供
可变字体轴控制。扩展名不能判断轮廓类型。原生源码构建报告所链接 FreeType
实际启用的 driver。
集合字体（`.ttc`）使用 face 0，适用相同的 driver 限制；在集合头部可解析时，
诊断会标明首个 face 的轮廓签名。空字体文件会在运行时启动前被拒绝。
仅含位图的字体会在初始化时被拒绝，动态回退需要可缩放轮廓。

可选字体初始化失败时，预编译 GFB 字形仍然可用，动态回退被关闭。字体解析失败的
警告会包含检测到的字体容器、字节数和 FreeType 错误码；缺少 CFF driver 时会明确
提示不支持 CFF/CFF2。其他解析失败不能直接证明字体文件损坏，需结合错误码与
该 FreeType 构建启用的 driver 判断。
字号初始化失败会报告具体 FreeType 操作、场景/字体索引、请求的像素字号与
FreeType 错误码。

字形缓存尺寸是缓存上限，不是可绘制字号上限。超限回退位图使用一个可复用的堆
工作区，后续解析会重新光栅化。模拟器直接驱动可移植运行时，不创建 FreeRTOS
渲染任务；设备任务栈配置不会调整模拟器的 WASM 栈。

`gspc font-link` 的目录使用 `--font-catalog fonts.gspb` 或 `GSP_FONT_CATALOG`，
显式 CLI 路径优先，可与 `--dynamic-font` 同时传入。目录在 UI 启动前校验 CRC，
reset 时保留。CLI/RPC capabilities 的 `font_catalog_version: 1` 表示内嵌运行时
支持该入口，旧发行版需更新。目录按 ID 补齐缺失 GFB 资产，不按码点提供缺字回退。
解析优先级及 PC bridge 边界见[外置字体](../guide/external-assets.md#模拟器与-pc-bridge)。

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
| `--drag <X1> <Y1> <X2> <Y2>` | 从 (X1, Y1) 拖拽到 (X2, Y2)，可重复 |
| `--wait <FRAMES>` | 在下一个脚本动作前等待 N 帧（N > 0），可重复 |

脚本操作按在命令行中出现的顺序依次执行。未启用 API 通道时，脚本完成会退出；
启用 API 后应使用 `quit` 或帧数上限结束。同样的输入命令也可在运行时通过 API 通道发送。

### 触摸录制与回放

```sh
gsp_sim_host --bundle app.gspb --record-input touch.jsonl --frames 600
gsp_sim_host --bundle app.gspb --headless --replay-input touch.jsonl \
    --dump replay.png --dump-format png
```

录制保存实际消费的 pointer/touch 样本，包括 tap/drag 展开的样本。回放使用模拟时间，
必须匹配轨迹头中的 bundle、尺寸、帧步长以及可选动态字体/字体目录 hash。
到达结束记录时自动退出；帧数上限或 `quit` 提前截断会报错。录制/回放期间拒绝 reset，
回放期间拒绝现场输入。轨迹不包含属性写入、业务状态或 Backend 的 `pointer` 通知；
依赖这些输入的应用需另外重现它们。使用新的录制文件名；截断文件不能作为完整回放。

### 输出与连接

| 参数 | 说明 |
|---|---|
| `--dump <PATH>` | 运行结束时保存最后一帧画面 |
| `--dump-format <FMT>` | 截图格式：`ppm`（默认）/ `png` / `raw` |
| `--api-enable` | 启用 API 控制通道（默认 stdio） |
| `--api-listen <URL>` | API 端点；隐含 `--api-enable` |
| `--api-json <PATH>` | 加载 GSPC `*.api.json`，支持按名点按/设值/检查包围盒并补全回调名 |
| `--backend-enable` | 在自动选择的本地端口启用应用后端 |
| `--backend-listen <URL>` | 应用后端端点；隐含 `--backend-enable` |
| `--backend-library <PATH>` | 加载本机 Backend 动态库；与 Socket Backend 参数互斥 |
| `--backend-required` | 在后端连接之前暂停帧推进 |
| `--backend-idle-timeout <SECS>` | 配合 `--backend-required` 限制首次连接或重新连接的等待；默认 `0` 无限等待 |
| `--ready-file <PATH>` | 启动后写入监听端点与进程身份；使用新路径，退出后由调用方清理 |
| `--log-level <LEVEL>` | `trace` / `debug` / `info`（默认）/ `warn` / `error`；有效的 `RUST_LOG` 优先 |
| `--log-file <PATH>` | 将宿主 tracing 日志追加到文件；WASI stderr 和预览 URL 仍输出到 stderr |

### 诊断

| 参数 | 说明 |
|---|---|
| `--capabilities` | 无需 bundle，输出模拟器支持的功能集并退出 |
| `--print-wasm-info` | 隐藏诊断参数；无需 bundle，输出内嵌 WASM 的 `staged`、`bytes`、`sha256`、`source` 后退出 |

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
支持的传输方式：stdio、回环 TCP（`tcp://127.0.0.1:PORT`）和 Unix 套接字
（`unix:///path/to/sim.sock`）。

需要按名自动化时，通过 `--api-header` 编译并加载对应的 bundle sidecar：

```sh
gsp_sim_host --bundle build/app.gspb --api-json build/app_gsp.api.json \
    --api-listen tcp://127.0.0.1:8266 --frames 0
```

`capabilities.named_api` 用于确认映射已启用。sidecar 必须与 bundle 来自同一次
编译——schema、字节数或 CRC 不匹配时宿主会拒绝启动。按名包围盒表示编译初始布局，
`query_geometry` 和 `inspect_object.runtime_geometry` 用于查询当前输入几何。

### 方法

#### 输入

| 方法 | 参数 | 说明 |
|---|---|---|
| `tap` | `{ "x": N, "y": N }` | 模拟点按 |
| `tap_object` | `{ "name": "...", "scene": N }` | 点击已核验的运行时输入点；`require_verified` 可拒绝旧版回退 |
| `drag` | `{ "x1": N, "y1": N, "x2": N, "y2": N }` | 模拟拖拽手势 |
| `feed_pointer` | `{ "x": N, "y": N, "pressed": bool }` | 低级指针输入 |
| `feed_touch` | `{ "id": N, "x": N, "y": N, "pressed": bool }` | 低级多点触控输入 |

#### 场景与状态

| 方法 | 参数 | 说明 |
|---|---|---|
| `goto_scene` | `{ "scene": N, "transition": N }` | 切换到 0-based 索引的场景；`transition` 可省略 |
| `reset` | — | 重新加载场景包 |
| `set_property` | `{ "name": "...", "property": "...", "value": ... }` | 按 sidecar 类型和范围设置命名属性 |
| `get_property` | `{ "name": "...", "property": "...", "scene": N }` | 从已提交的运行时状态读取当前场景的命名标量属性 |
| `wait_property` | `{ "name": "...", "property": "...", "value": ..., "max_frames": N }` | 在帧推进后等待属性达到指定类型和值 |
| `set_value` | `{ "bind_id": N, "value": N }` | 设置绑定的整数值 |
| `set_text` | `{ "bind_id": N, "text": "..." }` | 设置文本绑定 |
| `set_color` | `{ "bind_id": N, "color": N }` | 按原生像素格式设置颜色绑定 |
| `set_visible` | `{ "bind_id": N, "visible": bool }` | 设置组件可见性 |
| `set_component_i32` | `{ "component_key": N, "property_key": N, "value": N }` | 通过键设置组件属性 |
| `keyboard_attach` | `{ "action_id": N, "text_bind": N }` | 关联虚拟键盘 |
| `keyboard_attach_ex` | `{ "action_id": N, "text_bind": N, "max_bytes": N }` | 按指定 UTF-8 字节容量关联虚拟键盘；`max_bytes` 不含结尾 NUL |
| `keyboard_text_size` | — | 读取当前键盘文字所需容量（含结尾 NUL）；API/Backend/Browser 只读 |
| `keyboard_text` | `{ "capacity": N }`（可选） | 读取当前键盘文字；容量不足返回 `ESP_GSP_ERR_INVALID_SIZE` 和完整 UTF-8 前缀；API/Backend/Browser 只读 |
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

`set_property` 接收布尔值和声明范围内的数值。有组件目录时，属性转换与 C API
一致，包括 `enabled`、Toggle 的 `checked` 和 Slider/Arc/Progress 的自定义范围。
不带组件目录的普通 bundle 仍兼容对应绑定转换。`set_value` 继续直接写入整数绑定，
不转换语义属性值。

#### 辅助

| 方法 | 参数 | 说明 |
|---|---|---|
| `ping` | — | 心跳检测；返回 `"pong"` |
| `capabilities` | — | 查询画面尺寸、场景数、运行时 ABI 和 `named_api` |
| `list_objects` | `{ "scene": N }` | 列出命名对象；`scene` 可省略 |
| `inspect_object` | `{ "name": "...", "scene": N }` | 查看命名操作、属性和编译包围盒 |
| `hit_test` | `{ "x": N, "y": N, "scene": N }` | 查询实际运行时命中；旧模块返回编译包围盒 |
| `query_geometry` | `{ "name": "...", "scene": N, "instance": N }` | 当前输入包围盒、裁剪区域与已核验的点击点 |
| `resource_memory` | `{ "scene": N }` | 按需读取模拟器内存归因 |
| `frame_info` | — | 获取最近一帧的提交信息 |
| `wait` | `{ "frames": N }` | 延迟 N 帧后响应 |
| `component_get_motion` | `{ "name": "...", "scene": N }` | 查询 PageFlow/Drawer 状态；也可用 `component_key`；`scene` 可选 |
| `wait_component` | `{ "name": "...", "value": N, "max_frames": N }` | 等待静止及可选目标值；见下方条件说明 |
| `invalidate` | — | 强制下一帧全屏重绘 |
| `screenshot` | `{ "path": "...", "format": "png" }` | 保存当前画面到文件 |
| `subscribe` | `{ "events": [...] }` | 订阅服务端通知 |
| `unsubscribe` | `{ "events": [...] }` | 取消订阅 |
| `quit` | — | 请求模拟器退出 |

`capabilities.input_ready` 为 true 表示转场、自定义拦截器、活动触点、按下的指针和手势均未占用输入；场景编号改变不代表已经可点击。

`runtime_input_query_version: 1` 配合匹配的新 sidecar，支持按当前变换、裁剪和启用状态
查询输入区域，并通过真实命中与 Drawer 路由核验候选点。`no_verified_point` 表示采样点
均未核验通过，不等于对象完全被遮挡。转场占用输入或存在自定义拦截器时拒绝确认；
已有触点或活动手势时返回 `input_busy`，拒绝操作会保留原触点；没有交互行为的对象返回 `no_input_target`。

初始候选点未命中时，查询会利用已观察到的遮挡边界寻找狭窄可见区域。每次请求最多追加
64 次点命中检查和 64 次边界查询，所有候选点仍须通过真实命中验证。可选的 `search`
字段记录实际查询次数及 `budget_exhausted`，用于识别搜索预算耗尽的情况。

`tap_object` 核验后返回 `target_verified: true`，业务结果仍需通过回调、属性和截图确认。
建议自动操作传入 `require_verified: true`。旧模块或旧 sidecar 的兼容回退使用编译中心，
返回 `target_verified: false`；已知被遮挡、禁用等目标不会回退。

列表行从 `hit_test` 取得 `instance`、`runtime_epoch`、`list`、`item`，点击时将四者与
列表名称一并传给 `tap_object`。行重新绑定时实例代次可能不变，因此还需校验所属列表和
业务索引；reset 后旧轮次失效，避免误点复用后的另一行。

`resource_memory_version: 1` 支持按需查询图片缓冲的同时峰值，以及缓存、动画和宿主
转场缓冲的当前占用。缓存与已解码图片统计重叠，不能相加各项峰值。响应列明统计排除项，
完整应用峰值保持未知；Host 数值不能证明设备堆或 DMA 可用空间。
`observed_at` 将读数关联到已验证的 Bundle 身份、当前场景及内容 ID、帧序号、运行轮次与
版本、显示尺寸。没有通过验证的 sidecar 时 Bundle 身份为 null，目标 Profile 仍保持未知。
比较资源或交互前后占用时，应核对对应产物和状态。

API 通道保持仅 JSON-RPC。应用 Backend 还提供下文说明的异步动态数据与二进制媒体
扩展；Browser 不会获得这些数据源请求或二进制上传权限。

### 命名标量属性

按名观察标量属性需要 `named_api`，以及
`capabilities.named_property_read_version: 1` / `wait_property_version: 1`。
它通过已匹配 API sidecar 中的类型化 bind 读取状态，支持普通和 deployable Bundle，
结果标明 `source: "runtime"`；该标量接口不读取文本。Backend 拥有状态写权限时，
API 仍可读取或等待属性。getter 不推进帧；wait 至少在一次成功帧推进后检查条件，
即使当前值已经匹配，也会给前一次状态更新留下绘制机会，再供后续截图使用。
`max_frames` 默认为 300，范围为 1–36000；预期值必须符合属性类型和声明范围。
超时和运行时查询失败返回 JSON-RPC 错误，reset 和场景切换取消尚未完成的等待。
值匹配证明观察到的状态，实际输入、回调和截图共同用于验证交互与画面。
`set_property` 颜色写入接受 RGB888；读取和等待使用当前像素格式的原生色值，例如
RGB565 红色为 `0xF800`。检查实际颜色时，宜使用对应像素格式的截图基线。有组件目录时，标量写入
会校验运行时属性键和类型，包括颜色与显隐；无效元数据不能退回原始绑定写入。

### 组件运动状态与条件等待

使用以下方法前，先检查 `capabilities` 中的 `component_motion_version`、
`component_events_version` 和 `wait_component_version`。值为 `1` 表示支持，
`0` 表示当前加载的 WASM 不包含该功能。

**查询运动状态：** `component_get_motion` 接受 `{"name":"pages"}` 形式的名称或
数值 `component_key`，以及可选的 `scene`。返回中包含 `result_code` 和
`state` 对象：

- `value` — 已提交的页码或开/关状态。
- `target` — 转场进行中的目标值。
- `dragging` / `settling` — 用户是否仍在拖拽 / 组件是否正在动画过渡。

查询失败时 `state` 可能为 `null`，务必检查 `result_code`。

**等待组件静止：** `wait_component` 接受相同的目标标识，以及以下可选字段：

- `value` — 期望的最终值（省略则仅等待静止）。
- `max_frames` — 帧预算，默认 300，范围 1–36,000。
- `after_event` — 操作前从 `component_get_motion.event_cursor` 取得的游标。

当组件停止运动且值匹配时，返回 `status: "settled"`、最终 `state` 和
`frames_waited`。帧预算按成功推进的模拟帧计数，而非实际时间。超时返回错误
`-32020`，`data.reason: "timeout"` 附带最后观测到的状态。场景切换或 `reset`
会取消正在进行的等待。即使 Backend 独占状态写入，这些观察方法仍然可用。

要求新操作证据时，先检查 `component_event_cursor_version: 1`，在操作前读取游标，
再传入 `after_event` 等待。成功同时要求组件出现游标之后的新事件、期望值匹配及静止，
返回 `event_observed: true`；初始值本来相同不会通过。reset 或事件历史不足时返回
`event_cursor_expired`。并发业务写入仍需应用层校验；省略该字段保留普通条件等待语义。

**组件事件通知：** 订阅 `component_event` 后，每个 UI step 会收到包含
`component_key`、`scene_id`、`kind`、`flags`、`state` 和 `event_cursor` 的通知。`flags` 为
位掩码：值变化 (`1`)、已静止 (`2`)、运动变化 (`4`)。通知按 UI step 合并，不记录
每次中间请求。

> **注意：** 要做确定性断言，请使用控制 API（`wait_component` 或
> `component_get_motion`）。Browser WebSocket 通知与画面共用可丢弃队列，不适合
> 作为测试断言。`wait` 仅按帧计数，`render_fence` 仅标记渲染同步边界——二者都
> 不保证动画或业务操作已完成。

### 通知事件

事件以 JSON-RPC 通知的形式推送（无 `id` 字段）。需先通过 `subscribe` 注册；
API 通道默认不订阅任何事件。

| 事件 | 参数 | 说明 |
|---|---|---|
| `scene_changed` | `{ "from": N, "to": N }` | 场景切换 |
| `component_event` | `{ "component_key": N, "scene_id": N, "kind": N, "flags": N, "state": {...} }` | 合并后的 PageFlow/Drawer 状态通知；需显式订阅 |
| `callback` | `{ "action_id": N, "arg": N, "scene_id": N, "list": N, "item": N, "callback"?: "name" }` | 组件回调；加载 `--api-json` 后附带编译得到的回调名 |
| `frame` | `{ "index": N }` | 每帧推送 |
| `list_bind` | `{ "list": N, "slot": N, "instance": N, "item": N, "resource_slot": N, "text_slot": N }` | Backend 动态 List/Grid 行请求；Grid 成员槽位，65535 表示缺失（List 两者均缺失） |
| `list_bind_overflow` | `{ "dropped": N, "level": "warn", "message": "..." }` | Backend 行请求丢失；原生桥接器要求重启会话 |
| `binary_result` | `{ "transfer_id": "...", "ok": bool, ... }` | Backend 二进制上传结果 |
| `backend_state` | `{ "state": "attached"\|"detached" }` | 后端连接/断开 |
| `log` | `{ "level": "...", "message": "..." }` | 宿主日志 |
| `wasi_log` | `{ "message": "..." }` | 运行时诊断（通知名称为兼容保留） |

### 错误码

| 错误码 | 含义 | 说明 |
|---|---|---|
| `-32600` | Invalid Request | 缺少 `jsonrpc: "2.0"` |
| `-32601` | Method Not Found | 未知方法名 |
| `-32601` | Method Not Allowed | 当前通道不允许调用该方法 |
| `-32601` | Backend Exclusive | Backend 启用后，非 Backend 通道调用了应用状态写方法 |
| `-32602` | Invalid Params | 参数缺失或类型错误 |
| `-32603` | Internal Error | 宿主内部错误 |
| `-32010` | Input Busy | `--input-mode` 阻止了当前通道发送输入 |
| `-32020` | Observation / verified input failed | 检查 `data.reason`：如超时、取消、游标过期、不支持或目标无法核验 |

`not_allowed` 与 `backend_exclusive` 是文档中的分类名，错误码均为 `-32601`。
消息分别为 `method not allowed for this channel: ...` 和
`method reserved for backend while backend is enabled: ...`。
`widget_*`、`bridge_call`、`query_visibility`、`render_fence` 及远程数据/媒体
扩展始终只允许 Backend，API/Browser 调用返回 `not_allowed`，不取决于 Backend
是否启用。

### Python 客户端示例

示例复用 [rpc_client.py](../../../tools/sim_bridge/rpc_client.py)，不另行实现
分帧。从仓库根目录以
`PYTHONPATH=tools/sim_bridge python3 your_script.py` 运行（安装包使用对应组件
路径）。客户端保留连续帧及交错通知，EOF 会报错。每个连接仅支持单调用者，
`call()` 返回完整响应（包括 `error`），业务层须检查错误和 `result_code`。

```python
from rpc_client import connect

with connect(port=8266) as api:
    caps = api.call("capabilities")
    print(caps)
    print(api.call("tap", {"x": 160, "y": 120}))
    print(api.call("wait", {"frames": 5}))
    print(api.call("screenshot", {"path": "/tmp/shot.png", "format": "png"}))
```

## 应用后端

后端通道让应用的业务逻辑在模拟过程中驱动 UI，复现固件 C API 的交互行为。
可通过 `--backend-listen <URL>` 启用 TCP/Unix Socket（不支持 stdio），或用
`--backend-library <PATH>` 加载匹配的本机 C Backend。

启用 Backend 后：

- 应用状态写操作以及 `goto_scene`、`reset` 变为 Backend 专属方法。API 调用收到
  `backend_exclusive`。Browser 普通状态写入收到 `not_allowed`；`goto_scene` 和
  `reset` 在 Backend 启用时收到 `backend_exclusive`。
- 后端自动订阅 `callback`、`scene_changed`、`pointer`、`list_bind`、`list_bind_overflow`、
  `binary_result`、`image_complete` 和 `image_release`。
- 同一时间只允许一个后端连接。

使用 `--backend-required` 可在后端连接前暂停帧推进；
配合 `--backend-idle-timeout <SECS>` 限制首次连接或断开后的等待（默认 `0`，无限等待）。

Backend 可调用上方列出的状态和应用逻辑方法，包括光标/滑动策略、Drawer、PageFlow
和固定条目 List/Wheel。还可用 `list_bind_remote` / `grid_bind_remote` 绑定动态
集合、用 `list_set_total` 设置总数，并对 `list_bind` 通知调用带令牌校验的
`row_publish`。

业务处理期间行可能被回收。非零结果码可能表示令牌过期或字段 slot 错误，不能全部
视为令牌过期。**检查具体结果码，不要重试已确认失效的令牌。**

Socket Backend 的 TCP/Unix 连接还支持基于 `Content-Length` 的 COPY 模式二进制
上传：

- `X-GSP-Kind: image` 和 `row-image` — 接受 PNG、JPEG 或 QOI 数据。
- `X-GSP-Kind: canvas` — 接受携带 stride 和 height 头部（可选脏区）的完整原始帧。

宿主通过 `binary_result` 通知确认每次上传的结果。Direct Draw 回调在渲染任务内部
运行——远程（Socket）生产者应主动推帧，而非等待 draw 回调。调用 `canvas_stop` 可
停止远程生产者。这些二进制扩展仅对 Backend 开放。原生 C 生产者建议使用组件中的
[bridge 库](../../../tools/sim_bridge/README.md) 处理帧传输和缓冲区生命周期。
模拟器能力位图以 `ESP_GSP_SIM_CAP_QOI`（bit 7，值 `128`）报告 QOI 解码器支持。

### 后端 Python 示例

```python
from rpc_client import connect

with connect(port=8684) as backend:
    print(backend.call("capabilities"))
    # 使用你生成场景中的 bind ID。
    print(backend.call("set_text", {"bind_id": 1, "text": "Ready"}))
    print(backend.call("set_value", {"bind_id": 2, "value": 75}))
    while True:
        event = backend.notification(timeout=None)
        print(event["method"], event["params"])
        # 在这里处理 callback/scene_changed；backend.call() 会保留通知。
```

## 原生 C Backend 工程

`tools/sim_bridge` 提供 `gsp_add_backend(target SOURCES ... SCENES ...)`，将
业务 C 源码与原生兼容库链接，自动生成 deployable bundle 和配套头文件。构建同时
产出默认加载的动态库和用于隔离的 Socket 可执行文件。

`INCLUDE_DIRECTORIES` 可添加业务头文件目录，`PROFILE` 可覆盖默认 RGB565
profile。PC 适配文件实现 `gsp_bridge_app_init(ui)` 与
`gsp_bridge_app_deinit(ui)`——不需要在 PC 端执行 ESP-LCD 启动或链接完整 GSP
渲染库。

启动方式与示例拆分见
[复用 C UI 业务代码](../configuration/simulator-preview.md#复用-c-ui-业务代码)。
已安装组件中的 [bridge README](../../../tools/sim_bridge/README.md) 列出完整
支持范围。未实现的 C 函数会链接失败，不会静默模拟成功。此版本不支持多线程直接
调用、自动重连或 reset 后的状态重放。

默认入口在应用初始化返回后负责事件派发。自定义入口必须在 Backend 线程中反复调用
`gsp_sim_bridge_poll()`——空闲时也不例外。宿主绘图和输入请求的应答不会代替它
执行 C 回调与定时器。不要让该线程阻塞等待输入。命令队列、分段输入和事件循环示例
见 [自定义入口要求](../../../tools/sim_bridge/README.md#custom-entry-point-keep-the-event-loop-running)。

动态库内含构建它的精确 ESP-GSP 版本。sim_host 在应用启动前校验 Backend ABI、
描述符大小、保留字段、运行入口和 GSP 版本——任一不匹配即拒绝加载。该校验用于
发现过期或误选的产物，不用于隔离不可信的本机代码。动态库崩溃或阻塞会直接影响
模拟器。需要进程隔离时使用 `tools/sim_bridge/run.py --socket-backend`。

### 动态媒体

当 `capabilities.bridge_media_version` 为 `1` 时，桥接库支持动态 List/Grid
binder、行字段、COPY 模式 PNG/JPEG/QOI 图片，以及 Canvas push/draw/
invalidate/stop。可运行示例见 `examples/usage/sim_bridge_media`。

- List 必须有编译生成的 runtime row template。
- Grid 模板图片需设置 `dynamic_image: true`；Grid 回调会收到实际的资源和文本
  槽位。
- 行令牌在行被回收之前可重复使用。行请求溢出是致命错误，需要重启会话，不会自动
  重放。

### Canvas 渲染

桥接库在 `poll` 期间将画面绘制到完整的本地离屏缓冲区。当宿主报告
`bridge_canvas_patch_version: 1` 时，首帧之后的小脏区以紧凑的逐行 patch 传输；
大面积更新或基准失配时回退到全帧传输。此机制与设备端的渲染任务/分块回调模型不同。

在 draw 回调内只允许只读 GSP 查询。场景切换后回调被禁用，需要重新注册。

**同步推送**（`canvas_push`）会停用目标的 draw callback。成功时在返回前调用一次
release，失败时所有权仍归调用方。上传成功不等于渲染完成。

**异步推送**（`canvas_try_push*`）将借用指针放入本地 8 帧 FIFO，不执行任何
网络 I/O。成功后立即停用目标的本地 draw callback。队列满时返回 `TIMEOUT`，不
转移所有权。帧缓冲区在释放前必须保持不可改写。

`poll` 负责上传排队的帧；后续同步 GSP RPC 也会先排空之前的帧以保持顺序。
目标/步长/脏区校验或场景不匹配可能在入队后拒绝帧并打印诊断信息。无论上传、拒绝
还是本地关闭，每个已接收的帧恰好释放一次。release 不表示宿主已接受或显示。排队
帧的 release 回调可能在同步 RPC 返回前触发——回调中不得修改 GSP 状态、调用 poll
或 close。默认入口在 app deinit 前取消待上传帧；自定义入口须保持上下文存活直到
bridge close 返回。强制终止进程无法保证回调执行。

### 渲染栅栏

当 `capabilities.bridge_fence_version` 为 `1` 时，可使用仅限 Backend 的方法
`render_fence`（C 包装函数：`esp_gsp_flush`）。该方法不需要参数（`{}`），在宿主
完成一次后续 step/渲染尝试后返回 `{"result_code":0}`。API 和 Browser 通道不能
调用此方法；API 的帧延迟仍使用 `wait`。

调用 flush 前，`poll` 必须先完成所有本地 Canvas 上传/重绘，否则 flush 返回
`INVALID_STATE`。flush 不会派发业务回调，也不会等待动画、图片发布或浏览器呈现
完成。即使空闲帧没有画面变化也能满足栅栏条件。

**超时行为：**

- 有限超时覆盖传输提交和应答等待。`UINT32_MAX` 表示无限等待。
- 零超时仅在本地排队并立即返回 `TIMEOUT`。
- 超时后栅栏和未完成的数据包会保留，由后续 `poll`/RPC 继续处理。请求不被取消，
  会话仍然有效。
- 原生库最多保留 16 个未完成栅栏；满时返回 `TIMEOUT` 且不入队。
- `poll` 自身的 timeout 参数仅用于空闲等待，不是上传时限。

生产者暂停 / poll / flush 用法见 `tools/sim_bridge/README.md`。

### 图片生命周期

当 `capabilities.bridge_image_version` 为 `1` 时，桥接库支持扩展图片操作：
COPY、BORROW 和 TAKE 语义，borrowed/owned 辅助函数，完成/释放回调，以及缓存键。
无论源端使用何种语义，传输到宿主时始终使用 COPY；原生的 borrowed 或 owned 输入
会保留到 GSP 实际释放为止。

- `image_complete` — 通知应用运行时的发布、失败或取消。
- `image_release` — 表示源数据不再需要保留。
- 回调在 `poll` 期间执行，不在宿主渲染任务中。

最多可同时跟踪 128 个图片请求。超出此限制的请求会被立即拒绝，不转移所有权，也不
触发回调。本地退出时，尚未收到完成结果的请求按主动关闭报告 `CANCELLED`，按传输
失败报告 `IO`——这些状态码描述的是桥接层的关闭，不代表宿主对图片的最终处理结果。
默认入口在 app deinit 前结清所有待处理回调。自定义入口须保持上下文存活直到
bridge close 返回。完整语义见桥接库 README。

### 仅限 Backend 的方法

宿主额外提供两个仅限 Backend 的方法：

- **`bridge_call`** — 固定宽度标量查询、类型化写入、动画、Canvas 尺寸/格式/场景
  查询，以及 component RESOURCE bind 查询。不接受任何原生指针。
- **`component_set_text`** — 通过组件键写入文本。

标准方法如 `set_value`、`set_text`、`set_component_i32` 在 Backend 通道上同样
可用，行为不变。`capabilities` 返回 `bridge_version`、`bridge_media_version`、
`bridge_image_version`、`bridge_fence_version`、
`bridge_keyboard_text_version`、`bridge_canvas_patch_version` 和
`current_scene`，用于初始协商。当 `bridge_keyboard_text_version` 为 `1` 时，
原生桥接器提供相同的键盘文字读取契约：所需容量包含结尾 NUL，容量不足时返回
`ESP_GSP_ERR_INVALID_SIZE` 和完整 UTF-8 前缀。

应用通常直接使用 C 包装函数，无需自行编码这些操作。标量操作编号定义在
`tools/sim_bridge/protocol.h` 中。

### 启动器配置

启动器脚本默认使用其所在的组件目录（通常为
`managed_components/espressif__esp-gsp`），构建产物输出到应用的工作目录下。
GSPC 版本由 `.gspc_version` 决定；模拟器版本由组件的 `idf_component.yml` 决定。
两者均通过已安装的 `esp-gsp-tools` 管理器调用。

**环境变量覆盖：**

| 变量 | 用途 |
|---|---|
| `GSPC_EXECUTABLE` / `GSP_SIM_EXECUTABLE` | 覆盖工具路径 |
| `GSP_SIM_VERSION` | 覆盖管理器使用的模拟器版本 |
| `ESP_GSP_COMPONENT_DIR` | 覆盖组件路径 |

对应的命令行参数优先于环境变量。启动器不会隐式选择 `PATH` 或开发构建目录中的
工具版本。直接 CMake 和解压组件的用法见 bridge README。

`sim_host --capabilities` 会报告已加载 WASM 的 `keyboard_text_version` 和宿主
进程的 `dynamic_backend_loader_supported`。当后者为 `false` 时，
`sim_bridge/run.py` 会自动选择 Socket Backend，不会尝试加载本机动态库。

### 就绪文件

启动器向模拟器传递 `--ready-file <PATH>`，模拟器会写入包含 `version`、`pid`、
`backend_mode`、`backend`、`backend_library`、`api`、`browser`、`bridge_version` 和
`dynamic_backend_abi_version` 的 JSON 文件。未启用的端点及 stdio API 的 `api` 字段显示为 `null`。PID 属于
模拟器进程，而非 Python 管理器父进程。

调用方应：

- 每次启动使用新的、独立的文件路径。
- 校验 PID 的进程归属。
- 容忍启动期间文件尚未写完的情况。
- 模拟器退出后清理该文件。

就绪文件表示监听端口已绑定，不代表业务初始化完成或进程仍然存活。Backend-only
headless 模式同样按实际时间推进，以配合原生定时器。

## 在开发板上运行

模拟器适合在烧录前检查布局、文字和交互。烧录后还需要在目标板上检查面板接线、
显示方向、字节序、触控映射和性能。具体步骤见 [显示集成](../guide/display.md)。
