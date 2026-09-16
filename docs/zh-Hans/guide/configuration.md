# 配置模型

源码构建和预编译库使用相同的配置入口与优先级。

## 先使用默认值

普通接入无需逐项填写容量：选择 IDF/BSP 的板卡与 PSRAM 配置，编写场景并构建即可。GSPC 推导已声明的场景、列表、文字、图片目标和模板实例需求。

`menuconfig → ESP-GSP` 默认只展示图片缓存、后台解码、转场快照三个功能选项，其余配置集中在 **Advanced settings (optional)**。已保存的 `sdkconfig` 和实例覆盖继续生效。

标注“0 = 自动”的字段按场景需求配置；其他字段按各自说明填写。高级设置用于应用并发、任务调优和产品容量配置。编译器会一起报告已知的容量冲突和对应配置项；预编译库的能力上限由 `ESP_GSP_BUILD_CAP_*` 常量给出。

应用可在运行时提供图片、文字、定时器和动态实例。在场景/模板中声明已知峰值，使用实例覆盖配置额外的应用容量。

## 阅读构建结果

构建日志先列出自动推导的业务需求，再显示资源体积、动画帧工作集和图片缓存策略。缓存预算显示“automatic runtime budget”时表示运行时确定预算，不是关闭缓存。

日志显示 profile 指定的 internal/PSRAM 分配偏好。规划整机内存时，还需单独计入任务栈、控制池、DMA 临时区、显示缓冲和应用分配。应检查目标设备的实际堆容量；PSRAM 优先分配可能回退到 internal RAM。

构建时检查已知资源需求，运行时报告真实分配失败及相关堆信息。结合这些诊断和 BSP 缓冲区大小调整应用预算。

## 三个输入层，一个最终结果

| 层级 | 用于 | 不用于 |
|---|---|---|
| JSON | 场景结构、模板和 GSPC 可推导需求 | 固件调度策略 |
| Kconfig | 固件级运行策略、交互参数和堆容量默认值 | 单个场景外观 |
| `esp_gsp_config_set()` | 某 Bundle/实例不同于固件默认值 | 在 C 中重复全部工程默认值 |

```text
工程 Kconfig → GSPB 中的 JSON 需求 → 单实例覆盖 → 预编译库能力上限
```

运行时会提高 AUTO 容量以覆盖 Bundle 需求；超过预编译能力上限时明确失败，不会静默截断内容。

## 使用预编译库时 Kconfig 是否有效

有效，但只保留应由消费工程解析的设置：运行时策略、交互调优、任务设置和堆上容量。这些值通过工程配置桥接进入运行时，因此预编译库也能读取并生效。

影响库编译布局的参数随预编译库固定，并通过 `ESP_GSP_BUILD_CAP_*` 常量提供查询；任务策略和堆上容量等设置由消费工程配置。

## Slot 与动态实例

Slot 是有界运行时存储，不等于场景对象总数或数据集记录数。普通需求由 GSPC 自动推导。应用动态创建实例时，把复用子树声明为 JSON 模板，用 `max_instances` 表示同时存活最大值。List/Grid 复用因此按可见行/单元格扩展，不按总记录数扩展。

需要为单个 Bundle 设置容量时，可在启动前使用 `esp_gsp_config_set()`：

```c
esp_gsp_config_t config = gsp_product_config();
ESP_ERROR_CHECK(esp_gsp_config_set(
    &config, ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, 24) ==
    ESP_GSP_CONFIG_SET_OK ? ESP_OK : ESP_FAIL);
```

使用 `idf.py menuconfig` 探索配置，把确定的默认值提交到 `sdkconfig.defaults` 和目标专用 `sdkconfig.defaults.<target>`。修改默认值后执行干净构建，避免旧 `sdkconfig` 被误认为新策略。完整配置路径见[配置参考](../reference/configuration.md)和[Kconfig 使用](../reference/kconfig.md)，全部当前设置、范围和预编译行为见[配置项参考](../reference/configuration-options.md)。

## 保存工程配置

- 在 `sdkconfig.defaults`、`sdkconfig.defaults.<target>` 中保存工程默认值；额外片段通过
  调用方 CMake 的 `GSP_KCONFIG_DEFAULTS` 加载。
- `idf.py reconfigure` 后查看活动 `sdkconfig`，确认设置已生效。
- Bundle 的实例数可采用 AUTO，由 JSON `max_instances` 推导；应用在启动前确定的
  容量和产品策略可通过 `esp_gsp_config_set()` 设置。
