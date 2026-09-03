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
`goto_scene` 等方法驱动画面更新。后端连接期间场景导航为后端专属：浏览器场景按钮
和 API 的 `goto_scene` 调用会被拒绝。

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
