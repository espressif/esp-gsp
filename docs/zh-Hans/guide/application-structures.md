# 应用结构与所有权

普通应用优先使用 Bundle 头文件中的类型化辅助函数。只有集成层需要直接接触公共结构。

## 启动配置

```c
esp_gsp_config_t app = gsp_bundle_config();

esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd.display = display_from_bsp;
lcd.touch = touch_from_bsp; /* 可选 */

ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
```

| 结构 | 用途 | 规则 |
|---|---|---|
| `esp_gsp_config_t` | Bundle、目录、字体与实例配置 | 从 `gsp_<symbol>_config()` 开始，仅覆盖确有差异的策略/容量 |
| `esp_gsp_esp_lcd_config_t` | 显示目标、触控与平台生命周期 | 从 `ESP_GSP_ESP_LCD_CONFIG_INIT()` 开始 |
| `esp_display_present_target_config_t` | 面板、Framebuffer、GRAM、TE 与提交能力 | 由 BSP 构造并保持到停止完成 |

不要自行替换 Bundle 指针、目录表、Schema/ABI 字段，也不要绕过
`esp_gsp_config_set()` 修改受控配置。

两个 ESP-GSP 配置结构都包含 `struct_size`，对应初始化器会写入正确值。所有字段、
默认值和取舍见[配置参考](../reference/configuration.md)。

## 事件与输入

| 结构 | 关键内容 | 生命周期 |
|---|---|---|
| `esp_gsp_event_t` | action、参数、scene、list/item | 仅在事件回调期间借用 |
| `esp_gsp_pinch_event_t` | 两触点与手势状态 | 仅在 pinch 回调期间借用 |
| `esp_gsp_row_t` / `esp_gsp_grid_cell_t` | 当前复用行/单元 | 仅在绑定回调期间有效 |
| `esp_gsp_canvas_surface_t` | 像素地址、Stride、格式、脏区 | 按 Canvas 所有权约定有效 |
| `esp_gsp_image_options_t` | 格式、所有权与解码选项 | 调用时读取，缓冲区按 COPY/BORROW/TAKE 管理 |

应用不得保存这些临时视图中的内部指针后异步使用。需要跨任务处理时复制必要的标量和
产品数据，不复制框架内部对象。

## 运行时图片所有权

`esp_gsp_image_options_t` 控制一次编码图片提交：

| 字段 | 规则 |
|---|---|
| `ownership` | COPY 复制输入；BORROW 借用到释放回调；TAKE 只在成功提交后转移所有权 |
| `on_release`、`release_ctx` | BORROW 必须提供；TAKE 可在释放时获得通知 |
| `on_complete`、`complete_ctx` | 可选，用于接收已接受请求的最终结果 |
| `cache_key` | 非零值标识内容不变的资源；字节变化时必须更换 |

空选项表示 COPY。释放和完成回调可能运行在渲染或解码任务，必须快速返回。API 立即
返回错误时，缓冲区仍归调用方所有，也不会再触发释放回调。

## 应用数据集合

| 结构 | 关键内容 | 规则 |
|---|---|---|
| `esp_gsp_message_t` | `text`、稳定 `id`、变化的 `revision`、`direction` | 应用填充；`text` 必须保持有效到下一次 `get()` 调用 |
| `esp_gsp_message_source_t` | `struct_size`、`count`、`get`、可选 `decorate`、`user_ctx`、`flags` | 清零初始化并将 `struct_size` 设为结构大小；回调运行在渲染任务，`flags = 0` 保留文本 hash 校验 |
| `esp_gsp_row_t` | `list`、`slot`、`instance`、`item` | 框架提供的复用行令牌，原样传给行 setter |
| `esp_gsp_grid_cell_t` | 行令牌以及解析后的资源/文字槽 | 传给生成或公共单元格 setter，不要自行构造 |

在 Binder 回调中提交 Row 与 Grid 更新。异步图片 API 捕获行令牌，并在发布前校验
实例和所属数据项；行被复用后，旧图片更新会被取消。复用 `slot` 不是数据集中的永久标识。

## Canvas 与生成描述符

| 结构 | 关键内容 | 规则 |
|---|---|---|
| `esp_gsp_canvas_surface_t` | `pixels`、Stride、区域、像素格式 | 临时绘制目标；填满所给区域且不保存像素指针 |
| `esp_gsp_template_desc_t` | 生成的 `id`、尺寸、`max_instances` | 供 `esp_gsp_widget_create_template()` 使用的只读元数据 |
| `esp_gsp_component_info_t` | `key`、`kind`、能力和属性数 | 高级通用检查；普通业务代码优先使用类型化辅助函数 |

Canvas 绘制回调运行在渲染任务，可能分区调用。不得在其中调用修改 GSP 状态或等待
完成的 API。

## 所有权速查

- 配置由应用从生成/BSP 初始化器创建；其引用的 Bundle、字体和显示资源按文档保持有效。
- Event、Pinch、Row/Cell 令牌和 Canvas Surface 是框架拥有的回调视图。
- 图片选项与 Message Source 上下文由应用拥有，必须覆盖已注册回调的生命周期。
- 模板 ID、属性 ID、场景 ID 和内部 `__...` 对象都不应手写；只使用当前构建产物公开的符号。

函数签名与完整头文件契约见[函数级 API 参考](../reference/api-functions.md)。
