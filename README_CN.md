# ESP-GSP

[English](README.md) | 中文

ESP-GSP（Espressif Graphics Scene Processor）面向 ESP-IDF，将 JSON 场景和
资源编译成内嵌 Bundle 与类型化 C API。设备端通过 ESP-LCD 渲染有界运行时
状态，不需要维护通用 UI 对象树。

```text
JSON + 资源 -> ESP-IDF 构建 -> 生成 C API -> ESP-GSP -> 显示屏
```

## 主要特性

| 能力 | 说明 |
|---|---|
| 构建期 UI | Schema 校验、资源编译和确定性 Bundle |
| 生成 API | 为命名元素生成类型化 setter 与事件判断函数 |
| 嵌入式运行时 | 可配置固定资源池、脏区渲染、RGB565/RGB888 |
| UI 能力 | 控件、布局、List/Grid、模板、导航、媒体与 Canvas |
| 输入 | 点击、拖动、滚动、滑动以及可选的应用层双点 pinch |
| 显示路径 | RGB、MIPI-DSI、SPI、QSPI，并保留安全的软件回退 |

ESP-GSP 适用于界面结构在构建期确定，而文字、数值、媒体、集合和可见性需要在
运行时变化的产品。

## 要求与安装

- ESP-IDF 6.0 或更高版本。
- 安装 `esp-gsp-tools` Python 包。它会根据组件中的 `.gspc_version` 自动下载并
  调用对应版本的 GSPC。IDF 工程根目录下的 `.gspc_version` 优先于组件中的标记。
- 应用或 BSP 能初始化一个 `esp_lcd` 显示目标。

```sh
idf.py add-dependency "espressif/esp-gsp^1.1.0"
python -m pip install -U esp-gsp-tools
```

托管组件包含密封运行时、公共头文件、CMake 集成和维护中的发布例程。GSPC 与
模拟器作为独立可执行文件发布；组件不会下载编译器/模拟器源码，也不包含仓库
`tools/` 目录。

## 快速使用

需要由应用控制的元素设置稳定 `name`，需要交给应用处理的动作设置
`callback`：

```json
{
  "screen": "main", "w": 320, "h": 240,
  "objects": [
    {"type": "progress", "parent": -1, "name": "load",
     "x": 24, "y": 80, "w": 272, "h": 18,
     "value": 0, "fg_color": "#4CC9F0"},
    {"type": "toggle", "parent": -1, "name": "power",
     "callback": "power_changed",
     "x": 128, "y": 140, "w": 64, "h": 32,
     "fg_color": "#4361EE"}
  ]
}
```

在应用组件中注册场景：

```cmake
idf_component_register(SRCS "app_main.c" PRIV_REQUIRES esp-gsp)
gsp_add_bundle(${COMPONENT_LIB})
```

默认情况下，构建会编译工程 `scenes/` 目录中的全部 JSON，内嵌一个 Bundle
并生成 `bundle_gsp.h`。多 Bundle 工程或散落 JSON 仍可显式传入 `SCENES`。
使用 `gsp_bundle_config()`、
`ESP_GSP_ESP_LCD_CONFIG_INIT()` 和生成的 `gsp_<scene>_<name>_*()` API。
完整可运行集成见[中文快速入门](docs/zh-Hans/getting-started.md)。

配置只通过一条优先级链解析：

```text
工程 Kconfig -> JSON 推导的 GSPB 需求 -> 单实例 Override -> 预编译库能力上限
```

GSPC 自动推导场景需求，包括 JSON 模板上的 `max_instances`。密封 Lib 场景中，
Kconfig 仍然控制固件级运行策略和堆上容量；只有已经固化进库的能力通过只读
`ESP_GSP_BUILD_CAP_*` 常量公开。

## 推荐用法

1. 先锁定芯片、逻辑分辨率、像素格式和 BSP 显示路径。
2. 布局与声明式行为留在 JSON，产品状态和阻塞工作留在应用任务。
3. 保持 Kconfig 默认值，直到编译器输出或测量结果证明需要产品级修改。
4. 重新构建后再使用生成 API，不要修改生成头文件。
5. 先预览，再分别记录目标构建、板级运行和视觉验收结果。

ESP-IDF 上的 setter 为异步提交。仅在明确同步边界使用 `esp_gsp_flush()`，并从
应用任务调用 `esp_gsp_stop()`。

## 文档

| 任务 | 文档 |
|---|---|
| 安装并运行第一个场景 | [中文快速入门](docs/zh-Hans/getting-started.md) |
| 使用 Codex 创建或修改 UI | [Agent 辅助 UI 开发](docs/zh-Hans/guide/agent-assisted-ui.md) |
| 学习全部控件 | [控件库](docs/zh-Hans/components/index.md) |
| 编写 JSON | [场景结构与命名](docs/zh-Hans/guide/scenes.md) |
| 连接应用状态 | [运行时与生成 API](docs/zh-Hans/guide/runtime-api.md) |
| 查询公共 C 函数 | [公共函数参考](docs/zh-Hans/reference/api-functions.md) |
| 配置产品 | [统一配置模型](docs/zh-Hans/guide/configuration.md) |
| 集成与排错 | [工程工作流](docs/zh-Hans/guide/workflow.md)与[故障排查](docs/zh-Hans/guide/troubleshooting.md) |
| 预览 Deployable Bundle | [独立模拟器](docs/zh-Hans/reference/simulator.md) |

[文档语言入口](docs/README.md)清晰区分
`docs/en/` 与 `docs/zh-Hans/`。控件页、字段元数据和准确 C 签名来自 GSPC 与真实
`examples/widgets` 场景；本地交互预览使用推荐 GSPC 编译场景，并通过发布版
ESP-GSP 模拟器打开。

项目仓库中的示例：
[`hello_world`](examples/hello_world)
展示最小集成，
[`showcase`](examples/showcase)
展示产品 UI，
[`benchmark`](examples/benchmark)
用于可复现测量。

## 当前限制

- 场景结构在构建期固定；动态内容使用属性、模板、集合、运行时媒体或 Canvas。
- 双点 pinch 只上报触点，不会自动缩放控件。
- 一个 Bundle 使用一种逻辑场景分辨率和一种输出像素格式。
- 不支持从右到左布局和复杂文字塑形。

## 许可证

Espressif Modified MIT License，见 [LICENSE](LICENSE)。
