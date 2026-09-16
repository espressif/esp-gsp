# 配置参考

从生成的 `gsp_<symbol>_config()` 和 `ESP_GSP_ESP_LCD_CONFIG_INIT()` 开始。
普通场景不需要在 C 中复制工程配置：JSON 表达场景需求，Kconfig 提供固件级
默认值，只有单个 Bundle 不同于工程默认值时才调用 `esp_gsp_config_set()`。

```text
工程 Kconfig → GSPB 场景需求 → 单实例 Override → 预编译库能力上限
```

完整配置项、默认值、范围和是否支持 JSON 自适应见
[配置项参考](configuration-options.md)。

## Bundle 配置

`esp_gsp_config_t` 描述一个已编译 UI。生成配置已经提供 Bundle 字节、控件目录
和 JSON 推导的最低需求。

| 字段 | 默认行为 | 何时设置 |
|---|---|---|
| `struct_size` | 初始化器填写当前结构大小 | 不要手工改写 |
| `schema_version` | 初始化器选择当前 Schema | 不要手工改写；不匹配会失败 |
| `abi_version` | 初始化器选择当前 ABI | 不要手工改写；旧 Bundle 应重新编译 |
| `reserved_config` | 保持为 0 | 保留字段 |
| `overrides` | 八个空的内联条目 | 通常只通过 `esp_gsp_config_set()` 修改 |
| `bundle`、`bundle_size` | 由生成配置提供 | 仅高级加载器自行构造配置时设置 |
| `font_catalog` | 不使用外部共享字体目录 | Launcher 打开了外部字体目录时设置 |
| `directories`、`directory_count` | 由生成配置提供 | 仅高级配置自行注册控件目录时设置 |
| `ttf`、`ttf_size` | 不启用运行时轮廓字体回退 | 使用 `DYNAMIC_FONT` 或调用者持有字体 Blob 时设置 |
| `disable_swipe` | 多场景默认允许横向滑动 | 整个 UI 必须禁止场景滑动时设置 |
| `disable_bundle_crc` | 校验 Bundle、资源和字体 CRC | 仅可信构建资产已由外层分区完整性保护时设置 |
| `image_cache_bytes` | 根据目标可达 Heap 推导 | 根据同时显示的图片和可用内存设置 |

### 单实例覆盖

```c
esp_gsp_config_t config = gsp_product_config();

esp_gsp_config_set_result_t result = esp_gsp_config_set(
    &config, ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, 24);
assert(result == ESP_GSP_CONFIG_SET_OK);
```

Override 在调用时校验字段归属和范围。库布局字段是只读能力，尝试覆盖会返回
`ESP_GSP_CONFIG_SET_INVALID_FIELD`；应用应读取对应的
`ESP_GSP_BUILD_CAP_*` 常量。

`CONTEXT_DEFAULT_INSTANCES` 统计同时存活的模板实例，包括可见 List/Grid
条目以及应用创建的 Widget。GSPC 已把 JSON 中的 `max_instances` 和可见集合需求
写入 Bundle；只有完全由应用代码额外创建的峰值才需要 Override。

八个以上的单实例 Override 使用调用者持有的只读扩展表：

```c
static const esp_gsp_config_override_entry_t extra[] = {
    { .field_id = ESP_GSP_FIELD_SCROLL_DECAY_MS, .value = 400 },
    { .field_id = ESP_GSP_FIELD_SWIPE_SETTLE_MAX_MS, .value = 240 },
};

esp_gsp_config_t config = gsp_product_config();
assert(esp_gsp_config_override_bind_external(
           &config.overrides, extra,
           sizeof(extra) / sizeof(extra[0])) == ESP_GSP_CONFIG_SET_OK);
```

扩展表在应用创建完成前必须保持有效且不可修改。字段 ID 不能在内联表与扩展表中
重复。

## ESP-LCD 配置

`esp_gsp_esp_lcd_config_t` 把 UI 连接到显示目标和可选触摸设备。

| 字段 | 默认行为 | 何时设置 |
|---|---|---|
| `struct_size` | `ESP_GSP_ESP_LCD_CONFIG_INIT()` 填写 | 不要手工改写 |
| `display` | 没有有效默认值 | 使用 BSP 返回的显示目标 |
| `presenter` | 根据 `display` 新建 Presenter | 高级切换需要复用借用的 Presenter 时设置；设置后忽略 `display` |
| `touch` | 不启用输入 | BSP 提供 `esp_lcd_touch_handle_t` 时设置 |
| `touch_input_mode` | 有中断则使用中断，否则轮询 | 强制轮询或必须使用中断时设置 |
| `touch_wake_from_isr` | 不通知应用 | 应用需要从相同触摸中断唤醒自身任务时设置 |
| `touch_wake_user_ctx` | 空上下文 | Wake 回调需要应用上下文时设置 |
| `task_stack_size` | 12288；使用动态 TTF 时为 24576 | 根据栈高水位测量调整 |
| `task_stack_size_freetype` | 动态 TTF 路径为 24576 | 根据 FreeType 路径的栈使用量调整 |
| `task_priority` | 4 | 根据应用调度需求调整 |
| `task_stack_psram` | 跟随工程渲染栈配置 | 回调满足 PSRAM 栈的 Flash/Cache 约束时设置 |
| `perf_log` | 关闭 | 测量时需要每五秒输出 FPS 时启用 |
| `render_alignment` | 不扩展脏区 | 显示或加速后端要求区域对齐时设置 |

AUTO 触摸模式在中断注册不可用时记录警告并回退轮询。强制 INTERRUPT 模式则
要求有效 INT，并在注册失败时明确失败。ISR 回调只能做 ISR-safe 通知，不能访问
I2C 或调用 ESP-GSP API。

## 内存吃紧时如何调整

先区分内部 RAM、PSRAM 和连续块不足，再调整对应项。`image_cache_bytes`
是解码图片缓存预算，不是整个 GSP 的 RAM 配额；增大它不会增加实际可用内存。
`0` 表示自动推导，而不是关闭缓存。

按需读取设备堆信息，不需要 UI 句柄，初始化失败后也能调用：

```c
#include "esp_gsp_debug.h"

esp_gsp_heap_stats_t heap;
if (esp_gsp_heap_stats(&heap)) {
    // Inspect heap.internal and heap.psram:
    // free_bytes and largest_free_block, both in bytes.
}
```

该查询包含应用和驱动的堆使用，不是 GSP 独占用量；两个堆分别采样，结果会随
其他任务分配而变化。无 PSRAM 时对应数值为零；PC 不提供这些能力时返回
`false` 并清零输出。仅在任务上下文按需调用，不放入每帧回调或 ISR。
最大连续块也不保证满足 DMA、对齐或多个同时分配的要求。

| 观察到的问题 | 优先处理 | 代价或条件 |
|---|---|---|
| 内部 RAM 少，PSRAM 充足 | 检查渲染／解码栈及动画帧是否适合放 PSRAM | PSRAM 栈必须满足 Flash/Cache 约束；动画的 Prefer PSRAM 仍允许回退内部 RAM |
| 图片缓存没有可淘汰空间 | 为活动图片和待替换图片预留同时驻留的预算 | 仅在堆有余量时提高 `image_cache_bytes`，否则减少同时驻留的解码图片 |
| 图片分配失败 | 比较请求字节数与两类堆的最大连续块，检查显示缓冲和解码临时内存 | 预算充足不代表堆能分配；降低缓存或关闭快照可能增加绘制／解码开销 |
| 总内存紧张 | 检查显示缓冲配置、缓存预算、转场快照和后台解码 | 关闭后台解码会将工作转移到渲染任务；显示缓冲需由 BSP／Presenter 配合调整 |

每个图片缓存首次堆分配失败时额外输出两类堆的信息；后续失败保留请求大小，
避免反复扫描堆。需要最新堆状态时调用上述按需查询。

`esp_gsp_media_stats()` 提供缓存占用、峰值、分配失败和解码失败计数。
这些计数不涵盖所有预算拒绝；结合具体失败日志判断，不要把每个
`LIMIT_EXCEEDED` 都解释成整机堆耗尽。读取媒体计数时沿用其 UI 序列化要求。

任务栈开关位于 `ESP-GSP → Advanced settings (optional) → Project runtime defaults
→ Runtime tasks`。默认渲染栈为 12 KiB，动态 FreeType 为 24 KiB；后台解码栈为
4 KiB。启用 PSRAM 栈前核对任务及回调中的 Flash、NVS、文件系统操作，必要时
转交内部栈任务或 Flash dispatcher。初始化仍使用调用者栈，修改渲染栈不改变
`app_main()` 栈。缩栈前必须测量所有启用路径的历史最低剩余空间。

启动日志的 `memory` 来自实际栈地址，因此也反映 IDF 全局外部栈配置及其回退。
创建失败时只报告 `requested_memory`，因为此时不存在可检查的栈。
这些诊断不会自动改变预算、搬移任务栈或关闭功能。

## `gsp_add_bundle()` 参数

```cmake
gsp_add_bundle(<component-target>
    [SCENES <scene0.json> [scene1.json ...]]
    [PIXEL_FORMAT rgb565|rgb888]
    [IMAGE_CACHE_BYTES <bytes>]
    [DYNAMIC_FONT <font.ttf>]
    [SYMBOL <c_identifier>]
    [PROFILE <expert-profile.yaml>]
    [DEPLOYABLE])
```

| 参数 | 作用 |
|---|---|
| `SCENES` | 显式列出散落 JSON；未设置时按名称排序编译 `PROJECT_DIR/scenes/*.json` |
| `PIXEL_FORMAT` | 选择 `rgb565` 或 `rgb888`；未提供 Profile 时默认 `rgb565` |
| `IMAGE_CACHE_BYTES` | 同时设置编译期和运行时解码图片预算 |
| `DYNAMIC_FONT` | 嵌入运行时 TTF/OTF Blob 并启用 FreeType 路径 |
| `SYMBOL` | 设置 Bundle 和生成头文件使用的 C 标识符 |
| `PROFILE` | 高级编译 Profile；普通工程优先使用目标默认能力 |
| `DEPLOYABLE` | 增加自描述 GMD 元数据，使 Bundle 可脱离生成目录加载 |

没有 `SCENES` 时，工程必须至少包含一个 `scenes/*.json`。不同 Bundle 必须使用
不同 `SYMBOL`。`DEPLOYABLE` 不负责分区、传输、升级、回滚或签名策略。

## 图片缓存与场景切换

零 `image_cache_bytes` 会根据可达 Heap、最大可分配块和场景需求推导预算。首帧前，
运行时先准备当前可见命令使用的编译图片，再用剩余预算预取隐藏页面的图片；即使资源
注册顺序从隐藏内容开始，也会优先准备首屏图片。返回保留的场景时可复用解码结果，超过
预算则优先释放最久未使用的非活动场景 Surface。

预算应容纳可见图片的解码工作集。容纳不下时，启动日志会报告
`Visible scene images exceed cache budget`；已准备的可见图片保持驻留，跳过的图片走
占位和按需解码路径。需要首帧完整就绪时，应增大 `image_cache_bytes` 或减小图片解码
尺寸。动画转场还要求新旧场景同时驻留；峰值预算不足时使用
`ESP_GSP_NO_TRANSITION`。

关闭 `ESP_GSP_FIELD_ENABLE_IMAGE_CACHE` 后，QOI/RLE 等资源按区域解码；运行时
发布的 PNG/JPEG 需要解码图片缓存，在关闭状态下会被拒绝。

即使关闭图片缓存，矢量绘制仍需要额外内存。应使用应用的实际素材和转场测量峰值；
相关内存由框架自动管理。

## 相关文档

- [配置模型](../guide/configuration.md)：JSON、Kconfig 与 Override 的职责。
- [Kconfig 使用](kconfig.md)：`menuconfig`、默认值和目标专用配置。
- [配置项参考](configuration-options.md)：全部当前配置与只读能力。
- [应用结构](../guide/application-structures.md)：配置结构和所有权。
