# 模拟器预览与测试

独立模拟器宿主提供三种在烧录固件前验证 UI 行为的方式：交互式浏览器预览、CLI
脚本化测试和 API 驱动的自动化。每种方式都能验证可移植的场景行为，但不能替代
目标硬件上的面板接线、触控映射和视觉验收。

完整的命令行、API 和后端参考请参阅[模拟器参考](../reference/simulator.md)。

## 准备模拟器

通过 `esp-gsp-tools` 工具链管理器获取模拟器：

```sh
python -m pip install -U esp-gsp-tools
python -m gsp.execute --version '<ESP-GSP version>' sim --capabilities
```

`<ESP-GSP version>` 取自 `idf_component.yml` 的 `version` 字段。也可以设置
`GSP_SIM_EXECUTABLE` 环境变量指向已有的可执行文件。获取方式详见
[模拟器参考 — 获取模拟器](../reference/simulator.md#获取模拟器)。

下文命令使用 `gsp_sim_host` 代表模拟器。通过管理器调用时替换为
`python -m gsp.execute --version '<ESP-GSP version>' sim`。

## 交互式浏览器预览

不限制帧数启动模拟器以保持预览运行：

```sh
gsp_sim_host --bundle app.gspb --frames 0
```

宿主将预览 URL 输出到 stderr 并打开浏览器页面，提供：

- 实时 Canvas 渲染模拟画面；
- 鼠标点击和拖拽映射为指针输入；
- 25 %–400 % 缩放；
- 场景导航按钮（◀ ▶ ↺）；
- 分标签的日志面板，显示回调事件和运行时输出。

在编写场景时使用此模式，可以实时查看布局变化、动画和导航效果，无需 Target 构建。
关闭浏览器或按 Ctrl-C 停止模拟器。

## Headless 测试与截图

用于 CI 或自动化检查时，不启动浏览器，运行结束后保存最后一帧画面：

```sh
gsp_sim_host --bundle app.gspb --headless \
    --frames 3 --dump final.png --dump-format png
```

添加脚本化输入以验证交互路径：

```sh
gsp_sim_host --bundle app.gspb --headless \
    --tap 160 120 --wait 5 --tap 80 200 \
    --dump result.png --dump-format png
```

脚本化的 `--tap`、`--drag` 和 `--wait` 操作按出现顺序执行。更复杂的序列建议使用
API 通道。

## 复用 C UI 业务代码

已安装的 ESP-GSP 组件包含 `tools/sim_bridge`，可把 UI 业务 `.c` 与原生兼容库一起编译为
sim_host 的 Backend。它继续使用 `esp_gsp.h` 和 gspc 生成的组件 API；
Python 只负责构建与启动，编译后的程序直接通过本机 TCP 连接模拟器。

准备 Python 3.10+、CMake 3.20+、本机 C11 编译器和 `esp-gsp-tools`。
IDF 工程解析 ESP-GSP 依赖后（例如执行 `idf.py reconfigure`），在应用根目录运行
组件附带的示例：

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project managed_components/espressif__esp-gsp/examples/hello_world/pc
```

Windows 使用开发者终端和 `C:/path/to/tool.exe` 路径。原生构建不要求激活 ESP-IDF
环境，也不需要编译器或模拟器的实现源码。宿主需支持 `--ready-file` 且报告
`capabilities.bridge_version: 1`；旧版模拟器不能直接使用这条流程。

接入自己的 C UI 代码时，按[bridge CMake 示例](../../../tools/sim_bridge/README.md#add-your-own-application)
创建 `pc/` 工程，再使用 `--project pc`。启动器自动调用 `python -m gsp.execute`：
GSPC 版本取应用根目录的 `.gspc_version`，没有则取组件中的标记；模拟器 `sim`
版本取组件 `idf_component.yml` 的 `version`。首次使用自动下载、校验并缓存预编译工具。
`--gspc` / `GSPC_EXECUTABLE`、`--host` / `GSP_SIM_EXECUTABLE` 可覆盖可执行文件；
`--sim-version` / `GSP_SIM_VERSION` 只覆盖管理器选择的模拟器版本。
`--component-dir` / `ESP_GSP_COMPONENT_DIR` 可指定其他已安装组件。
默认构建输出在当前工作目录的 `build/` 下，不写入 `managed_components`。
使用解压后的组件时调整组件路径即可，不需要源码仓库。

CMake 将场景以 `--deployable` 打包，并在同次构建中生成匹配的
`bundle_gsp.h`。启动器取得实际端口后连接 Backend、打开浏览器，并在退出时
停止它启动的两个进程。加 `--headless --duration 3` 可进行限时无窗口运行；
`build.py` 使用相同构建参数但不启动进程。
端口就绪不表示业务初始化完成；`--duration` 从业务初始化完成后开始计时。

示例的分层如下：

- `hello_world/main/hello_ui.c`：设备与 PC 共用的定时更新逻辑。
- `hello_world/main/app_main.c`：仅设备端的面板、触控与 GSP 启动。
- `hello_world/pc/platform_pc.c`：PC 生命周期入口。
- `benchmark/pc`：直接复用已经拆分的 `main/bench_workload.c`，演示查询和动画，
  不包含完整硬件性能测试。
- `sim_bridge_media/pc`：可移植 List/Grid binder、QOI 图片和 Canvas 离屏绘制；
  使用同一启动命令替换 project 路径即可运行。

移植自己的工程时，先把驱动、RTOS 任务和硬件服务隔离到应用 HAL/PC Mock。
第一版要求所有 GSP 调用在同一个 Backend 业务线程中执行；回调内可以同步调用
已支持 API，工作线程需经应用队列交接数据。支持范围包括常用状态读写、标量属性、
动画、事件、定时器和部分导航。报告 `capabilities.bridge_media_version: 1` 的宿主
还支持原生动态 List/Grid binder、PNG/JPEG/QOI COPY 图片以及 Canvas push/draw。
binder 可同步更新行；Canvas 回调则先画完整本地离屏缓冲再上传，时机与分块方式
不同于设备端，回调内的 GSP 状态写入会被拒绝。进入场景时需重新注册 draw callback。
`capabilities.bridge_image_version: 1` 进一步支持图片 BORROW/TAKE、完成/释放
回调和缓存键。传输仍会复制；原生输入保留到 GSP 释放或本地会话关闭。
`canvas_try_push*` 不执行网络 I/O，立即返回本地 8 帧队列的接收结果；满队列返回
TIMEOUT 且不转移所有权。已接收缓冲应保持不可改写直到 release；poll 或后续同步
RPC 执行上传，release 不表示宿主接受或呈现。`capabilities.bridge_fence_version: 1`
支持 `esp_gsp_flush` 等待宿主渲染尝试，但不等待动画、图片解码或浏览器呈现。
应先 poll 完成本地待上传/待绘制的 Canvas 工作，否则 flush 返回 INVALID_STATE。
超时不取消栅栏，后续 poll/RPC 继续处理应答。
共享头文件中未实现的函数会在链接时失败。

完整 CMake 接入接口、生命周期与限制见组件中的
[bridge README](../../../tools/sim_bridge/README.md)，协议说明见[模拟器参考](../reference/simulator.md)。

## API 驱动自动化

启用 JSON-RPC 控制通道以进行编程控制：

```sh
gsp_sim_host --bundle app.gspb --frames 0 --api-enable
```

然后从任意 JSON-RPC 客户端驱动模拟器。典型验证流程：

1. `capabilities` — 确认画面尺寸和场景数。
2. `tap` / `drag` — 注入指针输入。
3. `wait` — 等待动画完成。
4. `screenshot` — 截图保存结果。
5. `quit` — 关闭模拟器。

通道支持 stdio（默认）、回环 TCP 和 Unix 套接字。当自动化需要独占输入且浏览器
预览同时打开时，设置 `--input-mode api-exclusive`。

## 带后端的全栈模拟

为同时测试应用逻辑和 UI，连接应用后端：

```sh
gsp_sim_host --bundle app.gspb --frames 0 \
    --backend-listen tcp://127.0.0.1:8684 \
    --backend-required --api-enable
```

后端在用户操作 UI 时接收 `callback` 通知，并通过 `set_text`、`set_value`、
`drawer_open`、`page_flow_set_page`、`list_snap` 和 `goto_scene` 等方法驱动画面更新。
启用 Backend 后，应用状态写入和场景导航均由 Backend 专属：浏览器控件和 API 调用
不能绕过业务后端。

如果要复用设备端 C 业务逻辑，建议把 LCD、touch、GPIO、Wi-Fi、NVS 和 FreeRTOS
初始化隔离在平台层。业务层保留 timer、`esp_gsp_on_event()` 和 `esp_gsp_*` UI
调用；组件附带的 `sim_bridge` 使用本机 C 编译器及自动选择的预编译工具构建。
原生库将异步 `list_bind` 和带令牌校验的 `row_publish` 转接为本地 C
binder，PNG/JPEG/QOI 图片与完整 Canvas 帧使用 COPY 型二进制上传；宿主渲染任务
不会等待 Backend 回调。Canvas Direct Draw 在 PC 事件循环中先离屏绘制再上传。
上传成功只表示接受，不代表完成解码或渲染；Canvas push 成功时在返回前调用提供的
释放回调，与设备端延迟释放不同。所有权、未支持 API 和令牌生命周期见桥接库 README
与模拟器参考。

此模式适合验证数据绑定、事件处理和多场景流转，无需目标硬件。

## 选择合适的模式

| 目标 | 推荐方式 |
|---|---|
| 可视化编写和布局迭代 | 交互式浏览器预览 |
| CI 中的快速渲染检查 | Headless + `--dump` |
| 交互回归测试 | Headless + `--tap` / `--drag` 脚本 |
| 复杂自动化或 AI agent 测试 | API 通道（`--api-enable`） |
| 端到端应用逻辑验证 | 后端 + API 通道 |

## 验收边界

模拟器运行可以证明编译后的场景在可移植运行时中能够正确渲染并响应输入，
**不能**证明：

- 面板字节序、旋转或撕裂行为；
- 触控控制器坐标映射；
- PSRAM 时序或 DMA 吞吐量；
- 最终显示色彩精度或字体渲染保真度。

模拟器证据和硬件证据应作为独立层级分别报告。
