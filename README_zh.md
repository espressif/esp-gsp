# ESP-GSP

[English](README.md) | 简体中文

ESP-GSP（Espressif Graphics Scene Processor）是面向 ESP-IDF 的预编译 UI
框架。它将 JSON 场景和资源转换为可嵌入的 Bundle 与类型化 C API，再通过应用
提供的 ESP-LCD 显示目标完成渲染。

```text
JSON + 资源 -> ESP-IDF 构建 -> 生成 C API -> ESP-GSP -> 显示屏
```

ESP-GSP 适用于界面结构在构建期确定，但文字、数值、图片、数据集合和可见性需要
在运行时变化的产品。有界运行时、确定性资源和生成式 API 有助于控制内存占用，
并让界面源码便于评审。

## 主要特性

| 能力 | 说明 |
|---|---|
| 构建期 UI | Schema 校验、资源编译和确定性 Bundle |
| 生成 API | 为命名元素生成类型化设置函数和事件判断函数 |
| 嵌入式运行时 | 可配置内存用量、RGB565 和 RGB888 渲染 |
| UI 能力 | 控件、布局、List、Grid、模板、导航、媒体和 Canvas |
| 输入 | 点击、拖动、滚动、滑动以及可选的应用层双点手势 |
| 显示路径 | RGB、MIPI-DSI、SPI、QSPI，并保留安全的软件回退 |

## 使用条件

- ESP-IDF 6.0 或更高版本。
- Python 3.10 或更高版本，用于运行 `esp-gsp-tools` 工具链管理器。
- 应用或 BSP 能够初始化一个 `esp_lcd` 显示目标。

## 运行维护示例

直接从 ESP Component Registry 创建最小完整示例：

```sh
idf.py create-project-from-example "espressif/esp-gsp=1.3.1:hello_world"
cd hello_world
python -m pip install -U esp-gsp-tools
```

示例包含 ESP32-C3、ESP32-S3、ESP32-P4 和 ESP32-S31 的配置。请选择与实际
开发板一致的配置，并在烧录前核对面板引脚和时序。预览和构建命令见
[`hello_world` 说明](examples/hello_world/README.md)。

编译器配置使用当前 ESP-IDF Python 环境。PATH 中已有配套 `gspc` 时可离线使用，
否则 CMake 调用工具管理器；源码仓库默认通过 Cargo 构建当前编译器。
版本固定、离线使用和安装错误处理见[编译器选择](docs/zh-Hans/guide/workflow.md#编译器选择)。

## 集成到现有工程

在 ESP-IDF 工程根目录运行：

```sh
idf.py add-dependency "espressif/esp-gsp^1.3.1"
python -m pip install -U esp-gsp-tools
```

在工程根目录的 `scenes/` 中创建场景 JSON。应用需要更新的元素应设置稳定的
`name`；需要进入应用逻辑的动作应设置 `callback`：

```json
{
  "screen": "main", "w": 320, "h": 240,
  "objects": [
    {"type": "progress", "parent": -1, "name": "load",
     "x": 24, "y": 80, "w": 272, "h": 18, "value": 0},
    {"type": "toggle", "parent": -1, "name": "power",
     "callback": "power_changed", "x": 128, "y": 140, "w": 64, "h": 32}
  ]
}
```

在应用组件注册之后添加 Bundle：

```cmake
idf_component_register(SRCS "app_main.c" PRIV_REQUIRES esp-gsp)
gsp_add_bundle(${COMPONENT_LIB})
```

下次构建会校验全部场景、编译资源、嵌入一个 Bundle，并生成 `bundle_gsp.h`。
从 `gsp_bundle_config()` 和 `ESP_GSP_ESP_LCD_CONFIG_INIT()` 开始初始化，再调用
生成的 `gsp_<scene>_<name>_*()` 函数。完整集成和显示启动代码见
[中文快速入门](docs/zh-Hans/getting-started.md)。

## 重要运行约束

- 布局和声明式行为放在 JSON 中；产品状态和阻塞操作放在应用任务中。
- ESP-IDF 上的生成式设置函数为异步提交。仅在明确的同步边界调用
  `esp_gsp_flush()`。
- 先停止外部数据生产者，再从应用任务调用 `esp_gsp_stop()`；框架回调中不要执行
  阻塞操作。
- 一个 Bundle 使用一种逻辑场景分辨率以及 RGB565 或 RGB888 输出格式。面板时序、
  旋转、字节序和触控映射由 BSP 负责。

## 文档

| 任务 | 从这里开始 |
|---|---|
| 安装并运行第一个场景 | [中文快速入门](docs/zh-Hans/getting-started.md) |
| 理解推荐开发流程 | [工程工作流](docs/zh-Hans/guide/workflow.md) |
| 编写 JSON 和选择字段 | [场景指南](docs/zh-Hans/guide/scenes.md)与[字段参考](docs/zh-Hans/reference/authoring.md) |
| 学习某个控件 | [控件库](docs/zh-Hans/components/index.md) |
| 连接应用状态 | [运行时与生成 API](docs/zh-Hans/guide/runtime-api.md) |
| 配置内存和功能 | [配置模型](docs/zh-Hans/guide/configuration.md)与[Kconfig](docs/zh-Hans/reference/kconfig.md) |
| 集成显示链路 | [显示集成](docs/zh-Hans/guide/display.md) |
| 无硬件预览 | [模拟器预览](docs/zh-Hans/guide/simulator-preview.md) |
| 排查问题 | [故障排查](docs/zh-Hans/guide/troubleshooting.md) |
| 查询准确 C 声明 | [公共函数参考](docs/zh-Hans/reference/api-functions.md) |

[文档入口](docs/README.md)包含完整的英文和简体中文文档。仓库还提供用于产品展示
的 [`showcase`](examples/showcase/README.md) 和用于硬件测量的
[`benchmark`](examples/benchmark/README.md)。

## 选择接入方式

- 使用 GSPC 构建场景结构；动态界面使用属性和模板，应用数据使用集合，编码图片使用
  运行时媒体，连续像素流使用 Canvas。
- 双指缩放由应用根据上报的双触点手势计算，再更新目标控件或 Canvas。
- 从右到左与复杂文字可预先制作文字素材，或通过 Canvas 接入应用的文字引擎。

## 许可证

Espressif Modified MIT License，见 [LICENSE](LICENSE)。
