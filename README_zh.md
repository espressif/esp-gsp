# ESP-GSP

[English](README.md) | 简体中文

ESP-GSP（Espressif Graphics Scene Processor）是乐鑫为 ESP-IDF 打造的预编译 UI
框架。你用 JSON 描述界面，框架在编译期把 JSON 和资源打包成 Bundle，同时生成类型
安全的 C API。运行时，应用通过 ESP-LCD 将 Bundle 渲染到屏幕上。

```text
JSON + 资源 -> ESP-IDF 构建 -> 生成 C API -> ESP-GSP -> 显示屏
```

ESP-GSP 适合这样的产品：界面结构在编译时就已确定，但文字、数值、图片和可见性等
内容需要在运行时变化。由于资源在编译期确定，内存用量可预测，界面代码也方便评审。

## 主要特性

| 能力 | 说明 |
|---|---|
| 构建期 UI | 用 JSON Schema 校验场景，编译资源，生成确定性的 Bundle |
| 生成 API | 为带 `name` 的元素自动生成类型化的读写函数和事件判断函数 |
| 嵌入式运行时 | 内存用量可配置，支持 RGB565 和 RGB888 渲染 |
| UI 能力 | 控件、布局、List、Grid、模板、导航、媒体和 Canvas |
| 输入 | 点击、拖动、滚动、滑动，以及可选的应用层双点手势 |
| 显示路径 | 支持 RGB、MIPI-DSI、SPI、QSPI，并保留软件回退路径 |

## 使用条件

- **ESP-IDF 6.0** 或更高版本。
- **Python 3.10** 或更高版本，用于运行 `esp-gsp-tools` 工具链管理器。
- 应用或 BSP 能够初始化一个 `esp_lcd` 显示目标。

## 快速体验

直接从 ESP Component Registry 创建一个最小完整示例：

<!-- gsp-version:registry-example -->
```sh
idf.py create-project-from-example "espressif/esp-gsp=1.7.0:hello_world"
cd hello_world
python -m pip install -U esp-gsp-tools
```
<!-- /gsp-version:registry-example -->

支持的芯片包括 ESP32、ESP32-C2/C3/C5/C6/H4、ESP32-S3、ESP32-P4 和 ESP32-S31。
其中 H4 与 S31 需要 ESP-IDF 的 `--preview` target 参数。

示例自带 ESP32-C3、ESP32-S3、ESP32-P4 和 ESP32-S31 的配置文件。请选择与你的
开发板一致的配置，烧录前务必核对面板引脚和时序。预览和构建命令见
[`hello_world` 说明](examples/usage/hello_world/README.md)。

编译器使用当前 ESP-IDF Python 环境。如果 PATH 里已经有配套的 `gspc`，可以离线
使用；否则 CMake 会通过工具管理器自动下载。从源码仓库构建时默认通过 Cargo 编译
当前编译器。版本固定、离线使用和安装异常处理见
[编译器选择](docs/zh-cn/guide/workflow.md#编译器选择)。

## 集成到现有工程

在你的 ESP-IDF 工程根目录运行：

<!-- gsp-version:dependency-command -->
```sh
idf.py add-dependency "espressif/esp-gsp^1.7.0"
python -m pip install -U esp-gsp-tools
```
<!-- /gsp-version:dependency-command -->

然后在工程根目录的 `scenes/` 中编写场景 JSON。需要从应用代码读写的元素，记得给
它加上稳定的 `name`；需要回传到应用逻辑的操作，给它设置 `callback`：

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

在应用组件的 CMake 中注册 Bundle：

```cmake
idf_component_register(SRCS "app_main.c" PRIV_REQUIRES esp-gsp)
gsp_add_bundle(${COMPONENT_LIB})
```

下次构建时，框架会自动完成以下工作：

1. 校验所有场景和引用的资源；
2. 编译资源，打包成一个可嵌入的 Bundle；
3. 生成 `bundle_gsp.h` 和各场景的类型化头文件；
4. 把 JSON 和资源注册为构建依赖——修改后自动重编。

生成的文件不需要提交到版本控制，也不应手动修改。

从 `gsp_bundle_config()` 和 `ESP_GSP_ESP_LCD_CONFIG_INIT()` 开始初始化，
然后调用生成的 `gsp_<scene>_<name>_*()` 函数即可。完整的集成步骤和显示启动代码见
[中文快速入门](docs/zh-cn/getting-started.md)。

## 重要运行约束

- **界面放 JSON，业务放应用**：布局和声明式行为写在 JSON 中，产品状态和阻塞操作
  放在应用任务中处理。
- **异步提交**：ESP-IDF 上的生成式设置函数是异步的。只在明确需要同步的地方调用
  `esp_gsp_flush()`。
- **先停生产者，再停框架**：调用 `esp_gsp_stop()` 前，先停掉外部数据生产者。
  框架回调中不要做阻塞操作。
- **一个 Bundle 对应一种配置**：使用统一的逻辑场景分辨率和固定的输出格式
  （RGB565 或 RGB888）。面板时序、旋转、字节序和触控映射由 BSP 负责。

## 文档

| 你想做什么 | 从这里开始 |
|---|---|
| 安装并运行第一个场景 | [中文快速入门](docs/zh-cn/getting-started.md) |
| 理解推荐的开发流程 | [工程工作流](docs/zh-cn/guide/workflow.md) |
| 用 AI Agent 编写和验证界面 | [Agent 辅助 UI 开发](docs/zh-cn/guide/agent-assisted-ui.md) |
| 编写 JSON、选择字段 | [场景指南](docs/zh-cn/guide/scenes.md) 与 [字段参考](docs/zh-cn/reference/authoring.md) |
| 了解某个控件的用法 | [控件库](docs/zh-cn/components/index.md) |
| 连接应用状态 | [运行时与生成 API](docs/zh-cn/guide/runtime-api.md) |
| 配置内存和功能 | [配置模型](docs/zh-cn/configuration/index.md) 与 [Kconfig](docs/zh-cn/reference/kconfig.md) |
| 集成显示链路 | [显示集成](docs/zh-cn/guide/display.md) |
| 不用硬件就预览 | [模拟器预览](docs/zh-cn/configuration/simulator-preview.md) |
| 排查问题 | [故障排查](docs/zh-cn/configuration/troubleshooting.md) |
| 查询 C 函数声明 | [公共函数参考](docs/zh-cn/reference/api-functions.md) |

[文档入口](docs/README.md)汇总了完整的英文和中文文档。仓库还提供用于产品展示的
[场景示例](examples/scenarios/README.md)和用于性能测量的
[`benchmark`](examples/performance/benchmark/README.md)。

## 接入方式选择

- 用 GSPC 构建界面结构。需要动态界面时使用属性和模板，需要展示应用数据时使用集合
  控件，编码图片使用运行时媒体接口，连续像素流使用 Canvas。
- 双指缩放需要应用自行处理：从双触点手势中计算缩放比例，再更新目标控件或 Canvas。
- 从右到左（RTL）书写和复杂文字排版，可以预先把文字做成图片素材，或通过 Canvas
  接入应用自己的文字引擎。

## 许可证

Espressif Modified MIT License，见 [LICENSE](LICENSE)。
