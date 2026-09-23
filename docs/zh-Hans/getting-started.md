# 安装与集成

## 初始化任务栈与校验

GSP 创建和资源校验同步运行在调用者任务中。新工程可先采用 `hello_world` 的 `CONFIG_ESP_MAIN_TASK_STACK_SIZE=20480`；如果从自建任务启动，应配置那个任务的栈。20 KiB 是接入起点，不是固定最低需求：结合字体、解码器和日志负载，用 `uxTaskGetStackHighWaterMark()` 验证后再调整。渲染任务的 `task_stack_size`/Kconfig 栈配置不会增加初始化调用者的栈。

默认保留 CRC 校验。大包扫描分段给调度器运行机会；不要通过关闭任务看门狗解决初始化卡顿。`disable_bundle_crc` 仅适用于由其他可信机制验证的静态资源。


添加预编译运行时组件，指定兼容的 GSPC 可执行文件，在统一目录编写场景，
并把生成 Bundle 启动到产品显示目标。

如果希望先体验 ESP-GSP，再集成到现有工程，可以直接创建维护示例：

<!-- gsp-version:registry-example -->
```sh
idf.py create-project-from-example "espressif/esp-gsp=1.5.0:hello_world"
cd hello_world
python -m pip install -U esp-gsp-tools
```
<!-- /gsp-version:registry-example -->

随后根据示例 `README.md` 选择模拟器预览或匹配的开发板配置。本文其余部分介绍
如何把 ESP-GSP 集成到现有应用。

## 开始之前

需要准备：

- 已在当前终端导出的 ESP-IDF 6.0 或更高版本；
- 能创建 `esp_display_present_target_config_t` 的板级支持层；
- ESP-GSP 托管组件；
- 已安装 `esp-gsp-tools`，用于根据组件中的 `.gspc_version` 解析并缓存 GSPC；
- 可选的独立模拟器，用于预览 Deployable Bundle。

## 添加组件

选择一种方式即可，同一构建中不要同时放入两份 ESP-GSP。

### Component Registry 命令

<!-- gsp-version:dependency-command -->
```sh
cd /path/to/your/esp-idf-project
idf.py add-dependency "espressif/esp-gsp^1.5.0"
```
<!-- /gsp-version:dependency-command -->

### `idf_component.yml`

<!-- gsp-version:dependency-manifest -->
```yaml
dependencies:
  espressif/esp-gsp: "^1.5.0"
```
<!-- /gsp-version:dependency-manifest -->

### 本地覆盖

<!-- gsp-version:local-override -->
```yaml
dependencies:
  espressif/esp-gsp:
    version: "^1.5.0"
    override_path: /absolute/path/to/esp-gsp
```
<!-- /gsp-version:local-override -->

`override_path` 用于框架开发。正式产品通常应锁定 Registry 版本，使依赖可复现。

## 安装 GSPC；按需使用模拟器

安装工具链管理器。CMake 会读取组件中的 `.gspc_version` 并调用
`esp-gsp-tools`；第一次调用时，管理器会自动下载、校验并缓存对应版本的 GSPC：

```sh
python -m pip install -U esp-gsp-tools
```

组件包中的 `.gspc_version` 记录发布时推荐的兼容 GSPC 版本。如需为某个
ESP-IDF 工程指定其他版本，可在工程根目录创建 `.gspc_version`；工程标记优先于
组件标记：

<!-- gsp-version:compiler-pin -->
```sh
echo '0.6.0' > .gspc_version
idf.py build
```
<!-- /gsp-version:compiler-pin -->

> [!TIP]
>
> 如果无法安装管理器或不希望自动下载，可参考 [兼容性](reference/compatibility.md)
> ，解压到稳定工具目录，再通过环境变量或 CMake 参数指定覆盖：
>
> ```shell
> export GSPC_EXECUTABLE=/absolute/path/to/gspc
> # CMake 中也可以通过以下方式覆盖
> idf.py -D GSPC_EXECUTABLE=/absolute/path/to/gspc build
> ```

`gsp_add_bundle()` 在 CMake 配置阶段验证 GSPC 兼容性。可执行文件缺失、路径
无效，或者 Scene/GSPB 版本超出组件支持范围时，配置会明确失败。

模拟器不是固件构建依赖。`sim` 不自动检测版本号，请手动使用
`idf_component.yml` 中的 `version` 字段（ESP-GSP 组件版本），不要使用
`.gspc_version` 中的 GSPC 版本：

```sh
python -m gsp.execute --version '<ESP-GSP version>' sim --bundle product.gspb
```

也可以从 [ESP-GSP Releases](https://github.com/espressif/esp-gsp/releases) 下载匹配主机和版本的
模拟器资产，设置 `GSP_SIM_EXECUTABLE` 后直接调用。

能力查询、CLI 参数和使用说明见[模拟器参考](reference/simulator.md)。

> [!WARNING]
> **保持版本匹配：**不要从无关源码目录随意复制 GSPC。应使用组件发布的兼容
> 信息和与之匹配的发布资产。仅 `--version` 成功不代表 Scene/GSPB 兼容，
> `gspc compatibility` 与组件配置阶段检查才是最终判定。

## 创建工程目录

默认约定是在工程根目录使用 `scenes/`：

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

`scenes` 明确表示“场景内容”，不会与 ESP-GSP 组件名或泛化的 `ui` 源码目录混淆。图片、字体等资源路径都相对于所在 JSON 文件解析。

### JSON 已经散落在其他目录

默认目录只是友好约定，不是限制。已有工程可在 CMake 中列出准确路径：

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES
        "../features/home/home.json"
        "../features/settings/settings.json"
        "../shared/dialogs/confirm.json"
    SYMBOL product
    PIXEL_FORMAT rgb888)
```

显式 `SCENES` 是唯一的替代发现机制，同时决定顺序；不同 `SYMBOL` 可创建多个独立 Bundle。无需再维护第二份配置清单。

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

这个场景使用 Progress 和 Toggle，不依赖字体或图片；添加文字时按
[字体说明](guide/media-and-data.md#静态图片与字体)配置字体。
为应用需要访问的对象设置 `name`，需要产品逻辑响应的动作设置 `callback`。
装饰对象无需命名，也不会生成多余 API。

## 随工程编译场景

在 `idf_component_register()` 之后调用 `gsp_add_bundle()`：

### 默认 `scenes/` 目录

```cmake
idf_component_register(
    SRCS "app_main.c"
    PRIV_REQUIRES esp-gsp)

gsp_add_bundle(${COMPONENT_LIB})
```

### 显式场景路径

```cmake
idf_component_register(
    SRCS "app_main.c"
    PRIV_REQUIRES esp-gsp)

gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../features/home/home.json"
    SYMBOL product
    PIXEL_FORMAT rgb565)
```

`SYMBOL product` 对应 `product_gsp.h` 和 `gsp_product_config()`；后面的启动代码
使用默认 `bundle` 符号，对应 `bundle_gsp.h` 和 `gsp_bundle_config()`。

默认形式会发现并按字典序排列 `PROJECT_DIR/scenes/*.json`，使用 RGB565，Bundle 名称为 `bundle`。`idf.py build` 期间自动完成：

1. 校验全部场景与引用资源；
2. 推导场景需求，转换图片和字体；
3. 打包可嵌入的 GSPB；
4. 生成 `bundle_gsp.h` 和各场景类型化头文件；
5. 把 JSON 与资源登记为构建依赖，修改后自动重编。

生成文件不应提交，也不应手动修改。

## 启动 ESP-GSP

BSP 负责面板初始化、Framebuffer 暴露、旋转、字节序和可选触控映射；ESP-GSP 接收其显示目标：

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

`board_display_init()`、`board_touch_init()` 是产品 BSP 函数，不是 ESP-GSP API。完整显示配置请参考维护例程。

## 构建、烧录并查看 API

### 构建

```sh
idf.py set-target esp32p4
idf.py build
```

### 烧录与日志

```sh
idf.py -p PORT flash monitor
```

将 `PORT` 替换为本机实际连接设备的串口；串口编号不对应固定芯片或开发板。

### 全新配置

```sh
idf.py fullclean
idf.py -D GSPC_EXECUTABLE=/absolute/path/to/gspc build
```

构建成功后，对 `bundle_gsp.h` 使用编辑器补全。对象接口命名大致为 `gsp_<scene>_<name>_<operation>()`，但具体操作由控件类型和动态字段决定，可在生成头文件中查看具体声明。

## 烧录前预览

先安装 `esp-gsp-tools`，编译 Deployable Bundle，再通过 Python 入口打开：

```sh
python -m gsp.execute --version '<GSPC version>' gspc pack scenes/main.json \
  --deployable -o product.gspb
python -m gsp.execute --version '<ESP-GSP version>' sim --bundle product.gspb
```

`sim` 不自动检测版本号；`<ESP-GSP version>` 使用
`idf_component.yml` 中的 `version` 字段，而 `<GSPC version>` 使用组件包中由 CI
生成的 `.gspc_version`。两者版本相互独立。

也可以从 [ESP-GSP Releases](https://github.com/espressif/esp-gsp/releases) 下载匹配的
GSPC 和模拟器资产，设置 `GSPC_EXECUTABLE`、`GSP_SIM_EXECUTABLE` 后直接调用。

无窗口 CI 或远程终端可输出帧：

```sh
python -m gsp.execute --version '<ESP-GSP version>' sim --bundle product.gspb \
  --headless --frames 3 --dump product.ppm
```

[控件库](components/index.md)为每个控件提供调用同一发布版模拟器的本地命令。
烧录后在目标板上检查布局、颜色、撕裂、触控手感和性能。

## 选择正确的配置层

| 需求 | 推荐机制 |
|---|---|
| 场景层级、对象、模板和可自动推导需求 | JSON |
| 固件级运行策略与堆容量默认值 | ESP-IDF Kconfig / `sdkconfig.defaults` |
| 某一个 Bundle 或实例不同于工程默认值 | `esp_gsp_config_set()` |
| 场景位置和 Bundle 像素格式 | `gsp_add_bundle()` 参数 |
| 面板、触控、旋转、Presenter 与缓冲策略 | BSP 显示目标 |

它们形成单一优先级链，而不是多个互相竞争的配置系统：

```text
工程 Kconfig → JSON 推导的 GSPB 需求 → 单实例覆盖 → 预编译库能力上限
```

普通场景需求由 JSON 自动推导。应用需要动态创建模板实例时，在 JSON 模板上设置
`max_instances`，GSPC 会把峰值计入 Bundle；只有无法写入场景的实例差异才使用
单实例覆盖。

## 下一步

- 按[工程工作流](guide/workflow.md)完成日常开发。
- 学习[场景结构与命名](guide/scenes.md)。
- 使用[运行时与生成 API](guide/runtime-api.md)连接产品状态。
- 理解统一的[配置模型](guide/configuration.md)。
- 从[控件库](components/index.md)复制并预览完整例子。

> [!TIP]
> **从维护例程开始：**`examples/usage/hello_world` 是最小完整板级集成；
> `examples/scenarios` 提供贴合目标硬件分辨率的产品界面；`examples/usage` 展示控件、媒体、导航和数据绑定。
> 例程引脚与时序只是示例，烧录前必须与实际硬件一致。
