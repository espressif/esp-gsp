# 配置模型

无论你使用源码构建还是预编译库，配置入口和优先级都是一样的。

## 先用默认值就好

一般接入不需要手动填写各种容量参数。只需要：

1. 在 ESP-IDF/BSP 中选好开发板和 PSRAM 配置；
2. 编写场景 JSON；
3. 执行构建。

GSPC 会自动推导场景中声明的列表、文字、图片和模板实例需求。

在 `menuconfig → ESP-GSP` 中，默认只展示图片缓存、后台解码和转场快照三个功能
选项，其他配置都收在 **Advanced settings (optional)** 里。已保存的 `sdkconfig`
和实例覆盖会继续生效。

**关于 "0 = 自动" 字段**：可由内容推导的容量字段根据 Bundle 需求配置；缓存预算、
任务默认值等其他自动设置遵循各自字段说明。高级设置涉及应用并发、任务调优和产品容量等进阶配置。
编译器会在出现容量冲突时给出对应的配置项名称。预编译库的能力上限可通过
`ESP_GSP_BUILD_CAP_*` 常量查询。

应用可以在运行时动态提供图片、文字、定时器和模板实例。但已知峰值应在场景或模板中
声明，额外的应用容量用实例覆盖来配置。

队列和同步等待的超时会按最终固件的 FreeRTOS tick rate 换算，源码和预编译构建遵循
同一规则。更新预编译组件时，请使用配套的组件文件并重新构建应用。

## 看懂构建输出

构建日志会依次显示：

1. 自动推导的场景需求；
2. 资源编码后的体积；
3. 动画帧工作集大小；
4. 图片缓存策略。

当缓存预算显示 "automatic runtime budget" 时，表示预算由运行时动态确定，并不是
关闭了缓存。

日志还会显示 Profile 指定的内部 RAM / PSRAM 分配偏好。但规划整机内存时，还需要
另外计入以下开销：任务栈、控制池、DMA 临时缓冲、显示缓冲和应用自身的分配。请以
目标设备的实际堆容量为准。不同类型的缓冲区有不同的分配和回退策略，详见
[媒体内存策略](../guide/media-and-data.md)以及下方的显示缓冲预算说明。

构建期会检查已知的资源需求；运行时则在分配失败时报告真实的堆信息。结合这些诊断
和 BSP 的缓冲区大小来调整应用预算。

## 按应用自动裁剪运行时

在应用的 `main/CMakeLists.txt` 中，保留现有 Bundle 定义，再声明编译资源之外需要的能力：

```cmake
gsp_add_bundle(${COMPONENT_LIB} SCENES "../scenes/home.json")
gsp_specialize_runtime(DYNAMIC_FEATURES NONE)
```

`NONE` 表示不额外使用下表中的能力。文字、数值、主题、列表、模板实例、定时器和
已编译动画仍可动态更新。构建分析全部已注册的 Bundle 和资源包，自动保留页面、
隐藏内容、模板和动画需要的模块，不改变资源编码或画面效果。

如果还会接收网络图片或播放独立 EAF 文件，声明这些额外能力即可：

```cmake
gsp_specialize_runtime(DYNAMIC_FEATURES jpeg png eaf)
```

| 动态能力 | 使用场景 |
|---|---|
| `jpeg` | 运行时 JPEG/JPEG_A8 图片；编译资源中的 JPEG 需求自动保留 |
| `png` | 运行时 PNG 图片 |
| `eaf` | `esp_gsp_play()` 播放独立 EAF；编译后的动画独立推导依赖 |
| `reference` | 额外使用 RGB565/RGB888 之外的场景，如 ARGB8888 叠加层 |
| `vector` | 编译清单之外的 vector 资源 |
| `procedural` | 编译清单之外带 procedural 命令的场景 |
| `anim_deflate` | 编译清单之外包含分组 DEFLATE 动画的资源包，包括 GRB 1.7 和 1.8 |
| `anim_spans` | 编译清单之外包含编译期 span 动画的 GRB 1.8 资源包；帧使用分组 DEFLATE 时还须声明 `anim_deflate` |

每个工程调用一次，放在 `idf_component_register()` 之后。不填写 `DYNAMIC_FEATURES`
等同于 `AUTO`，保守保留所有手动启用的模块；`ALL` 保留全部已启用能力。
未调用该函数的工程保持原有链接方式。手动关闭的 Kconfig 功能仍是能力上限；
资源或明确声明的动态能力与之冲突时，构建指出需要修改的配置项。

`gsp_add_bundle()`、`gsp_add_assets()` 的全部产物都会参与分析，包括其他组件稍后注册的产物。
预先准备好的额外包可通过 `EXTRA_BUNDLES "path/to/media.gspb"` 传入。
未来 UI 包的功能范围尚未确定时，使用 `DYNAMIC_FEATURES ALL`，或继续使用默认完整运行时。

构建会列出保留和移除的模块，详细原因及输入身份写入 `build/gsp_runtime/runtime.json`。
修改页面或资源后执行正常增量构建即可。源码和预编译组件使用相同入口；GSPC 和库需要
支持配套的 runtime-link ABI，版本不匹配会在配置时给出更新提示。

特化固件打开 Bundle 时检查能力需求，超出能力的新 UI 包会在分配 UI 实例前被拒绝。
应用应先成功打开新包，再替换当前 UI。运行时提交被裁剪的图片格式时，仍通过已有的
完成回调报告 `GSP_ERR_UNSUPPORTED`。Flash 收益取决于实际需求，不意味着缓存、
显示缓冲或任务栈可以按同一比例缩小。

## 显示提交缓冲预算

ESP-GSP 依赖 `esp_display_present ^1.1.2`，其缓冲策略与 GSP 场景容量是独立的。
Present 1.1.0 引入了自动 FB repair 和 TE 策略；当前帧同步和渲染器缓冲修复至少
需要 1.1.2。

在多核 FreeRTOS 和内存策略允许的情况下，这些模式可能会选择两块分区绘制缓冲。
符合条件的双缓冲路径还可能创建一个复制 worker（占 4 KiB 任务栈加上任务/队列开销）。
worker 在首次使用时才创建；分配失败会退回同步复制。额外缓冲和已创建的 worker 在
该 presenter endpoint 存续期间一直保留，不是每帧的临时开销。

如果你需要固定为单块绘制缓冲，可以在 BSP 初始化后、传给 `esp_gsp_esp_lcd_start()`
之前设置：

```c
/* hw_lcd_init(&display) 之后，赋给 esp_config.display 之前 */
display.drawbuf.buffers = 1;
```

这只限制分区绘制缓冲，不影响面板 framebuffer、TE 全帧 compose buffer 和 GSP
缓存。PSRAM 优先分配仍可能回退到内部 RAM。请在实际板卡上检查最终的模式和运行时
堆用量——升级依赖不意味着 SRAM 占用不变，也不保证所有场景都能提升 FPS。

## 三个输入层，一个最终结果

| 配置层 | 适合放什么 | 不适合放什么 |
|---|---|---|
| JSON | 场景结构、模板、GSPC 可自动推导的需求 | 固件级调度策略 |
| Kconfig | 固件级运行策略、交互参数、堆容量默认值 | 单个场景的外观 |
| `esp_gsp_config_set()` | 某个 Bundle/实例与工程默认值不同的配置 | 在 C 代码里重复全部工程默认值 |

三者形成一条优先级链，而不是互相竞争：

```text
工程 Kconfig → GSPB 中的 JSON 需求 → 单实例覆盖 → 预编译库能力上限
```

运行时会自动提高 AUTO 容量以满足 Bundle 需求。如果超出预编译能力上限，会明确报错，
不会静默截断内容。

## 预编译库下 Kconfig 是否生效

**生效**。保留下来的都是应由消费工程来决定的设置：运行时策略、交互调优、任务配置
和堆上容量。这些值通过工程配置桥接传入运行时，预编译库也能读取。

影响库编译布局的参数已随预编译包固定，可通过 `ESP_GSP_BUILD_CAP_*` 常量查询；
任务策略和堆上容量则由消费工程自行配置。

## Slot 与动态实例

Slot 是有界的运行时存储单元，不等于场景中的对象总数，也不等于数据集的记录数。
普通需求由 GSPC 自动推导。

当应用需要在运行时动态创建实例时，把可复用的子树声明为 JSON 模板，并用
`max_instances` 标明同时存活的最大数量。这样，List/Grid 的复用是按可见行/单元格
来扩展的，而不是按总记录数。

如果需要为某个 Bundle 单独调整容量，可以在启动前调用 `esp_gsp_config_set()`：

```c
esp_gsp_config_t config = gsp_product_config();
ESP_ERROR_CHECK(esp_gsp_config_set(
    &config, ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, 24) ==
    ESP_GSP_CONFIG_SET_OK ? ESP_OK : ESP_FAIL);
```

## 保存工程配置

- 用 `idf.py menuconfig` 探索配置，把确定的默认值提交到 `sdkconfig.defaults`
  和目标专用的 `sdkconfig.defaults.<target>` 中。
- 修改默认值后执行干净构建，避免旧 `sdkconfig` 被误认为新策略。
- 组件需要额外的默认值片段时，通过 CMake 的 `GSP_KCONFIG_DEFAULTS` 变量加载。
- 用 `idf.py reconfigure` 查看当前活动的 `sdkconfig`，确认设置已生效。
- Bundle 的实例数可以用 AUTO，由 JSON 的 `max_instances` 自动推导；应用在启动前
  需要确定的容量和产品策略，通过 `esp_gsp_config_set()` 设置。

完整的配置路径见 [配置参考](../reference/configuration.md) 和
[Kconfig 使用](../reference/kconfig.md)，全部设置项、范围和预编译行为见
[配置项参考](../reference/configuration-options.md)。
