# 配置模型

ESP-GSP 的配置在源码构建和预编译库两种使用方式中都必须有效。每类设置只有一个
归属，并汇入同一优先级链。

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

有效，但只保留应由消费工程解析的设置：运行时策略、交互调优、任务设置和堆上容量。这些值通过工程配置桥接进入运行时，因此密封库也能读取并生效。

会改变库编译布局的值无法在库生成后改变。无源码发布中，这类值不会伪装成可用 Kconfig，而是通过 `ESP_GSP_BUILD_CAP_*` 常量发布能力上限。这样既清除了无效配置，也保留明确的失败边界。

## Slot 与动态实例

Slot 是有界运行时存储，不等于场景对象总数或数据集记录数。普通需求由 GSPC 自动推导。应用动态创建实例时，把复用子树声明为 JSON 模板，用 `max_instances` 表示同时存活最大值。List/Grid 复用因此按可见行/单元格扩展，不按总记录数扩展。

只有额外峰值确实无法写入 JSON 时才使用 `esp_gsp_config_set()`：

```c
esp_gsp_config_t config = gsp_product_config();
ESP_ERROR_CHECK(esp_gsp_config_set(
    &config, ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, 24) ==
    ESP_GSP_CONFIG_SET_OK ? ESP_OK : ESP_FAIL);
```

使用 `idf.py menuconfig` 探索配置，把确定的默认值提交到 `sdkconfig.defaults` 和目标专用 `sdkconfig.defaults.<target>`。修改默认值后执行干净构建，避免旧 `sdkconfig` 被误认为新策略。完整配置路径见[配置参考](../reference/configuration.md)和[Kconfig 使用](../reference/kconfig.md)，全部当前设置、范围和预编译行为见[配置项参考](../reference/configuration-options.md)。

## 可复现配置

- 固件自带的 Bundle 保持 `CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES=0`，由 JSON
  `max_instances` 与编译器需求决定容量。
- 确需覆盖时只使用 `sdkconfig.defaults`、`sdkconfig.defaults.<target>` 或调用方
  CMake 的 `GSP_KCONFIG_DEFAULTS`，不要维护多份等价入口。
- 用 `idf.py reconfigure` 后的活动 `sdkconfig` 作为本次构建证据，并保留
  `gspc compatibility` 结果。
- 运行时 `esp_gsp_config_set()` 仅用于应用在启动前确定的产品策略或外部 Bundle
  容量，不用于掩盖可以重新编译的场景需求。
