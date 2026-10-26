# 安装与集成

本文介绍默认的嵌入式 Bundle 流程：在 ESP-IDF 应用中编写 JSON 场景，构建时自动
编译为 Bundle 并生成 C API，应用通过这些 API 更新界面。

如果你想先体验一下 ESP-GSP 再决定是否集成，可以直接创建示例工程：

<!-- gsp-version:registry-example -->
```sh
idf.py create-project-from-example "espressif/esp-gsp=1.8.0:hello_world"
cd hello_world
python -m pip install -U esp-gsp-tools
```
<!-- /gsp-version:registry-example -->

然后参照示例 `README.md` 选择模拟器预览或匹配的开发板配置。本文其余部分介绍
如何把 ESP-GSP 集成到你的现有应用中。

## 开始之前

请确认以下条件：

- 当前终端已导出 **ESP-IDF 6.0** 或更高版本，ESP32-S31 使用 6.1 或更高版本；
- 板级支持层（BSP）能够创建 `esp_display_present_target_config_t`；
- 已安装 `esp-gsp-tools`，或准备好兼容的 GSPC 可执行文件，通过 `GSPC_EXECUTABLE` 指定；
- （可选）独立模拟器，用于在没有硬件时预览 Bundle。

## 首次启动前：任务栈与 CRC 校验

Bundle 创建和资源校验运行在启动 ESP-GSP 的那个任务上。新工程可以参考 `hello_world`
中的 `CONFIG_ESP_MAIN_TASK_STACK_SIZE=20480`；如果你从自建任务启动，请为那个任务
预留同等空间。

> **这只是起点，不是最低值。** 你应该根据实际使用的字体、解码器和日志级别，用
> `uxTaskGetStackHighWaterMark()` 测量后再做调整。渲染任务的栈配置不会增加调用者
> 任务的栈。

默认保留 Bundle CRC 校验。不要通过关闭任务看门狗来掩盖启动卡顿——只有在其他可信
机制已经校验了静态资源时，才使用 `disable_bundle_crc`。

## 添加组件

选择下面任意一种方式，**同一构建中不要同时引入两份 ESP-GSP**。

### Component Registry 命令

<!-- gsp-version:dependency-command -->
```sh
cd /path/to/your/esp-idf-project
idf.py add-dependency "espressif/esp-gsp^1.8.0"
```
<!-- /gsp-version:dependency-command -->

### `idf_component.yml`

<!-- gsp-version:dependency-manifest -->
```yaml
dependencies:
  espressif/esp-gsp: "^1.8.0"
```
<!-- /gsp-version:dependency-manifest -->

### 本地覆盖（用于框架开发）

<!-- gsp-version:local-override -->
```yaml
dependencies:
  espressif/esp-gsp:
    version: "^1.8.0"
    override_path: /absolute/path/to/esp-gsp
```
<!-- /gsp-version:local-override -->

`override_path` 适用于框架开发调试。正式产品通常应锁定 Registry 版本，确保依赖
可复现。

## 安装 GSPC；按需使用模拟器

先安装工具链管理器。CMake 在构建时会读取组件中的 `.gspc_version` 并调用
`esp-gsp-tools`；首次调用时，管理器会自动下载、校验并缓存对应版本的 GSPC：

```sh
python -m pip install -U esp-gsp-tools
```

组件包里的 `.gspc_version` 记录了发布时推荐的兼容 GSPC 版本。如果你需要为某个
工程指定其他版本，可以在工程根目录创建 `.gspc_version`，工程标记优先于组件标记：

<!-- gsp-version:compiler-pin -->
```sh
echo '0.9.0' > .gspc_version
idf.py build
```
<!-- /gsp-version:compiler-pin -->

> [!TIP]
>
> 如果无法安装管理器或不希望自动下载，可参考 [兼容性](reference/compatibility.md)
> 手动解压到稳定目录，再通过环境变量或 CMake 参数覆盖：
>
> ```shell
> export GSPC_EXECUTABLE=/absolute/path/to/gspc
> # 或在 CMake 中覆盖
> idf.py -D GSPC_EXECUTABLE=/absolute/path/to/gspc build
> ```

`gsp_add_bundle()` 会在 CMake 配置阶段验证 GSPC 兼容性。如果可执行文件缺失、
路径无效，或 Scene/GSPB 版本超出组件支持范围，配置会明确失败。

模拟器**不是**固件构建依赖。`sim` 不会自动检测版本号——你需要手动使用
`idf_component.yml` 中的 `version` 字段（ESP-GSP 组件版本），而不是
`.gspc_version` 中的 GSPC 版本：

```sh
python -m gsp.execute --version '<ESP-GSP version>' sim --bundle product.gspb
```

也可以从 [ESP-GSP Releases](https://github.com/espressif/esp-gsp/releases)
下载匹配的模拟器资产，设置 `GSP_SIM_EXECUTABLE` 后直接调用。

能力查询、CLI 参数和使用说明见 [模拟器参考](reference/simulator.md)。

> [!WARNING]
> **保持版本匹配：** 不要从无关的源码目录随意复制 GSPC。应使用组件发布时标注的
> 兼容版本和对应的发布资产。`--version` 运行成功并不代表 Scene/GSPB 兼容——
> `gspc compatibility` 和组件配置阶段的检查才是最终判定。

## 创建工程目录

默认约定是在工程根目录下使用 `scenes/`：

```text
product-ui/
├── CMakeLists.txt
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   ├── idf_component.yml
│   └── app_main.c
└── scenes/
    ├── main.json
    ├── settings.json
    ├── fonts/
    └── images/
```

`scenes` 这个名字明确表示"场景内容"，不会与 ESP-GSP 组件名或泛化的 `ui` 源码
目录混淆。图片、字体等资源路径都相对于所在的 JSON 文件解析。

### JSON 文件已经散落在其他目录？

默认目录只是约定，不是硬性限制。已有工程可以在 CMake 中列出准确路径：

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES
        "../features/home/home.json"
        "../features/settings/settings.json"
        "../shared/dialogs/confirm.json"
    SYMBOL product
    PIXEL_FORMAT rgb888)
```

显式 `SCENES` 是唯一的替代发现机制，同时决定顺序。不同的 `SYMBOL` 可创建多个
独立 Bundle，不需要再维护第二份配置清单。

## 编写第一个场景

创建 `scenes/main.json`：

```json
{
  "screen": "main",
  "w": 320,
  "h": 240,
  "screen_bg": "#101820",
  "objects": [
    {
      "type": "progress",
      "parent": -1,
      "name": "load",
      "x": 40,
      "y": 96,
      "w": 240,
      "h": 24,
      "value": 30,
      "fg_color": "#4CC9F0"
    },
    {
      "type": "toggle",
      "parent": -1,
      "name": "power",
      "x": 128,
      "y": 164,
      "w": 64,
      "h": 32,
      "fg_color": "#4361EE",
      "callback": "power_changed"
    }
  ]
}
```

这个场景只用了 Progress 和 Toggle，不依赖字体或图片。添加文字时请按
[字体说明](guide/media-and-data.md#静态图片与字体)配置字体。

**命名规则：**

- 需要从应用代码读写的对象，给它设置 `name`——GSPC 会生成对应的类型化函数。
- 需要产品逻辑响应的操作，给它设置 `callback`。
- 纯装饰对象无需命名，也不会生成多余 API。

## 在工程构建中编译场景

在 `idf_component_register()` 之后调用 `gsp_add_bundle()`。

### 使用默认 `scenes/` 目录

```cmake
idf_component_register(
    SRCS "app_main.c"
    PRIV_REQUIRES esp-gsp)

gsp_add_bundle(${COMPONENT_LIB})
```

### 指定场景路径

```cmake
idf_component_register(
    SRCS "app_main.c"
    PRIV_REQUIRES esp-gsp)

gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../features/home/home.json"
    SYMBOL product
    PIXEL_FORMAT rgb565)
```

`SYMBOL product` 对应 `product_gsp.h` 和 `gsp_product_config()`。后面的启动代码
使用默认的 `bundle` 符号，对应 `bundle_gsp.h` 和 `gsp_bundle_config()`。

默认形式会发现并按字典序排列 `PROJECT_DIR/scenes/*.json`，使用 RGB565，Bundle
名称为 `bundle`。`idf.py build` 期间自动完成以下工作：

1. 校验全部场景和引用的资源；
2. 推导场景需求，转换图片和字体；
3. 打包为可嵌入的 GSPB；
4. 生成 `bundle_gsp.h` 和各场景的类型化头文件；
5. 把 JSON 与资源注册为构建依赖——修改后自动重编。

生成的文件不需要提交，也不应手动修改。

## 启动 ESP-GSP

BSP 负责面板初始化、Framebuffer 分配、旋转、字节序和可选的触控映射。ESP-GSP
接收 BSP 提供的显示目标：

```c
#include "esp_gsp_esp_lcd.h"
#include "bundle_gsp.h"

static void on_ui_event(esp_gsp_handle_t ui,
                        const esp_gsp_event_t *event,
                        void *user_ctx)
{
    (void)ui;
    (void)user_ctx;
    if (gsp_main_event_is_power_changed(event)) {
        /* 通知应用任务，不要在回调中阻塞。 */
    }
}

void app_main(void)
{
    esp_display_present_target_config_t display;
    ESP_ERROR_CHECK(board_display_init(&display));

    esp_lcd_touch_handle_t touch = NULL;
    (void)board_touch_init(&touch); /* 可选。 */

    esp_gsp_config_t app = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
    lcd.display = display;
    lcd.touch = touch;

    esp_gsp_handle_t ui;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
    ESP_ERROR_CHECK(esp_gsp_on_event(ui, on_ui_event, NULL));
    ESP_ERROR_CHECK(gsp_main_load_set_value(ui, 60));
}
```

上面的 `board_display_init()`、`board_touch_init()` 是产品 BSP 函数，不是
ESP-GSP API。完整的显示配置请参考维护例程。

## 构建、烧录并查看 API

### 构建

```sh
idf.py set-target esp32p4
idf.py build
```

ESP32-H4、ESP32-S31 等预览目标使用
`idf.py --preview set-target <target>` 选择芯片后再构建。

### 烧录与日志

```sh
idf.py -p PORT flash monitor
```

把 `PORT` 替换为你本机实际连接的串口。

### 全新配置

```sh
idf.py fullclean
idf.py -D GSPC_EXECUTABLE=/absolute/path/to/gspc build
```

构建成功后，对 `bundle_gsp.h` 使用编辑器补全。生成的接口命名大致是
`gsp_<scene>_<name>_<operation>()`，但具体操作由控件类型和动态字段决定——
请以生成头文件中的声明为准。

## 烧录前预览

先安装 `esp-gsp-tools`，编译 Deployable Bundle，再通过 Python 入口打开：

```sh
python -m gsp.execute --version '<GSPC version>' gspc pack scenes/main.json \
  --deployable -o product.gspb
python -m gsp.execute --version '<ESP-GSP version>' sim --bundle product.gspb
```

`sim` 不会自动检测版本号。`<ESP-GSP version>` 对应 `idf_component.yml` 中的
`version` 字段，`<GSPC version>` 对应组件包中 CI 生成的 `.gspc_version`。两者
版本相互独立。

也可以从 [ESP-GSP Releases](https://github.com/espressif/esp-gsp/releases)
下载匹配的 GSPC 和模拟器资产，设置 `GSPC_EXECUTABLE`、`GSP_SIM_EXECUTABLE` 后
直接调用。

无窗口的 CI 或远程终端可以输出静态帧：

```sh
python -m gsp.execute --version '<ESP-GSP version>' sim --bundle product.gspb \
  --headless --frames 3 --dump product.ppm
```

[控件库](components/index.md)为每个控件提供了可在本地运行模拟器的命令。
烧录后还需在目标板上检查布局、颜色、撕裂、触控手感和性能。

## 选择正确的配置层

| 你要配置什么 | 推荐机制 |
|---|---|
| 场景层级、对象、模板和自动推导的需求 | JSON |
| 固件级运行策略和堆容量默认值 | ESP-IDF Kconfig / `sdkconfig.defaults` |
| 某个 Bundle 或实例与工程默认值不同 | `esp_gsp_config_set()` |
| 场景位置和 Bundle 像素格式 | `gsp_add_bundle()` 参数 |
| 面板、触控、旋转、Presenter 和缓冲策略 | BSP 显示目标 |

它们形成一条优先级链，而不是多个互相竞争的配置系统：

```text
工程 Kconfig → JSON 推导的 GSPB 需求 → 单实例覆盖 → 预编译库能力上限
```

普通场景需求由 JSON 自动推导。如果应用需要动态创建模板实例，在 JSON 模板上设置
`max_instances`，GSPC 会把峰值计入 Bundle。只有无法写入场景的实例差异，才需要
用单实例覆盖。

## 下一步

- 按 [工程工作流](guide/workflow.md) 完成日常开发。
- 学习 [场景结构与命名](guide/scenes.md)。
- 用 [运行时与生成 API](guide/runtime-api.md) 连接产品状态。
- 理解统一的 [配置模型](configuration/index.md)。
- 从 [控件库](components/index.md) 复制并预览完整示例。

> [!TIP]
> **从维护例程开始：** `examples/usage/hello_world` 是最小的完整板级集成；
> `examples/scenarios` 提供贴合目标分辨率的产品界面示例；`examples/usage`
> 展示控件、媒体、导航和数据绑定的用法。
> 例程中的引脚与时序只是示例，烧录前务必与实际硬件保持一致。
