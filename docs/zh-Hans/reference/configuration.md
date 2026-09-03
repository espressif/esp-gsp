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
| `image_cache_bytes` | 根据目标可达 Heap 推导 | 测量证明默认图片预算不合适时设置 |

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
| `task_stack_size` | 12288；使用动态 TTF 时为 24576 | 栈高水位测量证明需要调整时设置 |
| `task_stack_size_freetype` | 动态 TTF 路径为 24576 | FreeType 路径测量证明需要调整时设置 |
| `task_priority` | 4 | 调度分析证明需要调整时设置 |
| `task_stack_psram` | 跟随工程渲染栈配置 | 回调满足 PSRAM 栈的 Flash/Cache 约束时设置 |
| `perf_log` | 关闭 | 测量时需要每五秒输出 FPS 时启用 |
| `render_alignment` | 不扩展脏区 | 显示或加速后端要求区域对齐时设置 |

AUTO 触摸模式在中断注册不可用时记录警告并回退轮询。强制 INTERRUPT 模式则
要求有效 INT，并在注册失败时明确失败。ISR 回调只能做 ISR-safe 通知，不能访问
I2C 或调用 ESP-GSP API。

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

零 `image_cache_bytes` 会根据可达 Heap、最大可分配块和场景需求推导预算。活动
场景的编译图片会在显示前准备；离开场景后可保留解码结果，超过预算时优先释放
最久未使用的非活动场景。动画转场需要新旧场景同时驻留，不能承担峰值的产品应
使用 `ESP_GSP_NO_TRANSITION`。

关闭 `ESP_GSP_FIELD_ENABLE_IMAGE_CACHE` 后，QOI/RLE 等资源按区域解码；运行时
发布的 PNG/JPEG 需要解码图片缓存，在关闭状态下会被拒绝。

## 相关文档

- [配置模型](../guide/configuration.md)：JSON、Kconfig 与 Override 的职责。
- [Kconfig 使用](kconfig.md)：`menuconfig`、默认值和目标专用配置。
- [配置项参考](configuration-options.md)：全部当前配置与只读能力。
- [应用结构](../guide/application-structures.md)：配置结构和所有权。
