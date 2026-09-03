# ESP-IDF Kconfig 使用

Kconfig 在源码组件和密封预编译组件中都有效。消费工程解析运行时策略、交互参数、
任务设置和堆容量，并通过工程配置桥接传给预编译运行时。已经固化进库布局的能力
不伪装成可修改项，而是通过 `ESP_GSP_BUILD_CAP_*` 只读常量公开。

全部配置项、默认值和范围见[配置项参考](configuration-options.md)。

## 交互配置

在应用工程根目录执行：

```sh
idf.py menuconfig
```

打开 `Component config -> ESP-GSP`，保存后重新构建：

```sh
idf.py build
```

`menuconfig` 修改的是当前构建实际使用的 `sdkconfig`。已有工程中，修改
`sdkconfig.defaults` 不会覆盖已经写入 `sdkconfig` 的旧值。确认实际配置：

```sh
rg '^CONFIG_ESP_GSP_' sdkconfig
```

如果工程设置了 CMake `SDKCONFIG`，应检查其指定文件。默认构建生成的编译期
头文件位于 `build/config/sdkconfig.h`。

## 可复现默认值

用 `menuconfig` 探索，用 `sdkconfig.defaults` 提交已经确认的产品默认值：

```ini
CONFIG_ESP_GSP_MAX_LISTS=6
CONFIG_ESP_GSP_LIST_MAX_SLOTS=40
CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES=24
```

以上数值只是语法示例，不是推荐值。容量按最大活动场景中的同时存活峰值配置，
不按数据集总记录数配置。可以使用 `idf.py save-defconfig` 导出非默认选项，但应
审查完整 Diff，因为它包含整个工程的配置变化。

### 目标专用默认值

只属于某个芯片的设置放在 `sdkconfig.defaults.<IDF_TARGET>`，例如：

```ini
# sdkconfig.defaults.esp32p4
CONFIG_ESP_GSP_RENDER_TASK_STACK_PSRAM=y
CONFIG_ESP_GSP_DECODE_TASK_STACK_PSRAM=y
```

存在 `sdkconfig.defaults` 时，ESP-IDF 会自动追加匹配的目标文件，不要重复列出。
PSRAM 任务栈要求目标启用 PSRAM 和 FreeRTOS 静态分配；回调还必须满足 Flash
Cache 关闭时的访问限制。没有测量依据时保留内部 SRAM 默认值。

需要叠加产品片段时：

```sh
idf.py -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.product' build
```

覆盖 `SDKCONFIG_DEFAULTS` 后，如仍需标准默认值，必须显式保留
`sdkconfig.defaults`。

## 验证新的 Defaults

使用隔离的构建目录和配置文件，避免旧 `sdkconfig` 掩盖新默认值：

```sh
idf.py -B build_esp32p4 \
  -D SDKCONFIG=build_esp32p4/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32p4 build
```

## 配置解析顺序

消费工程的 `sdkconfig` 对源码和预编译消费方式都有效。每个 UI 实例按固定顺序
解析一次不可变配置快照：

```text
工程 Kconfig → GSPB 场景需求 → 单实例 Override → 预编译库能力上限
```

- JSON 推导只会提高标记为 AUTO 的场景容量。
- 非 AUTO 容量低于 Bundle 最低需求时明确失败，不会静默提高或截断。
- Override 不能低于 Bundle 需求，也不能超过发布库能力。
- 旧版、不含版本化需求记录的 Bundle 必须用当前 GSPC 重新构建。

`CONFIG_ESP_GSP_LIST_MAX_SLOTS=0` 表示 AUTO。GSPC 根据视口、行模板和 Grid
列数计算需求，运行时至少分配该需求，最终不能超过库能力 64。多场景 Bundle
取单个活动场景最大值，不把互斥场景相加。

## Slot 的含义

Slot 是一类有界运行时存储，不是统一的对象计数。

| Slot 类型 | 一个 Slot 表示什么 | 配置方式 |
|---|---|---|
| 场景状态 | 一个动态 Bind、可见性、变换、属性或动态主题值 | 编译进 GSB，无工程计数项 |
| `TEXT_SLOTS` | 一条同时塑形的非模板动态文字 | 按活动场景自动推导 |
| `LIST_MAX_SLOTS` | 一个可见/回收的 List、Wheel 行或 Grid 单元格 | 按视口与模板自动推导 |
| `LIST_TEXT_SLOTS` | 回收行中的一个动态文字字段 | 按行模板自动推导 |
| `CONTEXT_DEFAULT_INSTANCES` | 一个同时存活的模板实例 | 按 `max_instances` 和集合需求推导 |
| `INSTANCE_STATES_PER_SLOT` | 每个模板实例可携带的动态状态数 | 编译器对照工程/库限制检查 |
| `CONTEXT_DEFAULT_GLYPH_RUNS` | 一条直接文字、集合文字或内部文字的 Glyph Run | 按活动场景推导 |
| `DEFAULT_DYNAMIC_IMAGE_SLOTS` | 一个同时持有运行时图片的逻辑目标 | 命名图片自动推导，应用创建目标需额外配置 |
| `CANVAS_SLOTS` | 一个同时绑定的外部帧生产者 | 由固件策略配置 |
| Timer、Widget、Animation | 一个活动运行时对象 | 按应用同时使用峰值配置 |

List/Grid 的 Slot 数量与总数据量无关。增加记录数不会增加可见行池；增加视口内
同时可见的行、列、模板字段或应用创建实例才可能需要更大容量。

## 应该在哪一层配置

1. 布局、内容、模板和 `max_instances` 放在 JSON，让 GSPC 自动推导。
2. 任务栈、优先级、缓存策略、脏区容量、输入阈值和应用创建对象等固件级值使用 Kconfig。
3. 只有某个 Bundle 或运行时选择不同于固件默认值时使用 `esp_gsp_config_set()`。

这条路径既保证密封库可配置，也避免一个需求同时存在多套覆盖机制。

## 修改后的验证

1. 检查活动 `sdkconfig`，不要只检查 Defaults 文件。
2. 构建实际 Target，让 Kconfig、场景编译、C 编译和链接全部执行。
3. 运行需求最大的场景和被修改的功能。
4. 缩小任务栈前测量高水位。
5. 触摸、调度、加速和动画手感必须在真实硬件验证。

配置解析、目标构建、板上运行和视觉/触摸验收应分别记录。
