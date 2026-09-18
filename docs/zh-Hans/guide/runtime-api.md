# 运行时与生成 API

## 从生成配置启动

`gsp_<symbol>_config()` 提供 Bundle 数据、组件目录和 JSON 推导需求。将它与 `ESP_GSP_ESP_LCD_CONFIG_INIT()`、BSP 创建的显示目标组合，再调用 `esp_gsp_esp_lcd_start()`：

```c
esp_gsp_config_t app = gsp_bundle_config();
esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd.display = display;
lcd.touch = touch;

esp_gsp_handle_t ui;
ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
```

## 生成对象操作

只有控件类型和 JSON 动态属性支持时，GSPC 才生成对应操作。配置 `name` 和 `fg_color` 的 Slider 会生成 `get_value()`、`set_value()`；带文字 `bind` 的命名 Label 会生成文字 setter；`callback` 会生成事件判断函数。应查看生成头文件旁的 `*.api.json`、或各[控件示例页](../components/index.md)展示的真实签名。

ESP-IDF 中 setter 是异步的。成功表示命令已入队；渲染任务随后提交状态，并按变更安排刷新。
`esp_gsp_flush()` 用于确定性测试、截图或明确的同步边界。

Slider 和启用拖动的 Arc 可在 `events` 中声明 `value → call` 和 `release → call`，
分别接收数值变化和释放提交。`event->arg` 转为 `int32_t` 后使用声明的 `min/max` 单位；
重复值不重复通知。只声明 `callback` 时默认在释放时通知。Arc 通过数值
`bind` 启用拖动；配置示例见[工程工作流](workflow.md)。

## 事件与任务上下文

事件、列表绑定、图片释放、定时器和 Canvas 回调运行在框架任务中。不要在其中阻塞、访问慢速存储或执行网络操作；只发送队列/任务通知并尽快返回。

应用负责的返回行为使用 `callback`，StackView 返回使用 `stack_pop`。JSON 的
`back` 动作为保留能力，当前公共运行时不会分发。

```text
UI 事件 → 短回调 → 应用任务 → 产品状态改变
                                  → 生成 setter → 渲染提交
```

## 动态内容

| 内容 | 推荐 API 路径 |
|---|---|
| 命名数值、文字、可见性、选择状态 | 生成对象辅助函数 |
| 应用持有的行或单元格 | List/Grid/Wheel/Message List 适配器 |
| 偶尔替换的编码图片 | 生成图片 setter 或 `esp_gsp_set_image*()` |
| 相机、视频、连续像素 | Canvas 帧或 Direct Draw 回调 |
| 页面与堆栈导航 | 生成导航/组件辅助函数 |

缓冲区必须遵循 COPY、BORROW 或 TAKE 所有权约定。`esp_gsp_stop()` 完成前，先停止外部生产者、解绑回调并释放借用资源。详细规则见[生命周期](lifecycle.md)和
[API 参考](../reference/api.md)。

## 正确停止

从应用任务调用 `esp_gsp_stop()`，不要在事件、列表、图片释放、定时器或 Canvas 回调
中停止。先停止新的媒体帧、网络结果和数据发布，再停止 Canvas 并在需要时执行一次
`esp_gsp_flush()`，最后停止 UI。返回 `ESP_GSP_ERR_TIMEOUT` 时 Handle 仍有效，可以
重试；成功后再释放 Bundle、字体、回调上下文、显示目标和 BSP 资源。

详细所有权规则见[生命周期](lifecycle.md)，完整签名见[函数级 API 参考](../reference/api-functions.md)。

## 公共错误码

生成 setter 和 `esp_gsp_*` 返回已有的 `esp_gsp_err_t`。不要另造错误类型。

| 宏 | 值 | 含义 |
|---|---|---|
| `ESP_GSP_OK` | `0` | 已接受（异步入队成功，不是渲染完成） |
| `ESP_GSP_FAIL` | `-1` | 未分类失败 |
| `ESP_GSP_ERR_NO_MEM` | `0x101` | 分配失败 |
| `ESP_GSP_ERR_INVALID_ARG` | `0x102` | 参数无效 |
| `ESP_GSP_ERR_INVALID_STATE` | `0x103` | 当前状态不允许该调用 |
| `ESP_GSP_ERR_INVALID_SIZE` | `0x104` | 缓冲区或容量不足 |
| `ESP_GSP_ERR_NOT_FOUND` | `0x105` | 对象、绑定或资源不存在 |
| `ESP_GSP_ERR_NOT_SUPPORTED` | `0x106` | 当前控件或配置不支持该操作 |
| `ESP_GSP_ERR_TIMEOUT` | `0x107` | 等待队列或停止超时；句柄仍有效，可重试 |


## 颜色与按下反馈

`esp_gsp_component_set_color()` / `get_color()` 保持原生色值语义：RGB565 场景中红色是 `0xF800`；RGB888 场景是 `0xFF0000`。需要统一使用 `0xRRGGBB` 时，调用 `esp_gsp_component_set_color_rgb888()` / `get_color_rgb888()`。JSON 颜色和生成属性接口的既有语义不变。

`esp_gsp_set_press_feedback_enabled(gsp, false)` 关闭默认按下遮罩，保留点击和控件自身交互。命令在渲染任务处理，关闭时也清除当前遮罩；传入 `true` 恢复默认效果。

## 有效可见性

为命名的静态场景对象重新运行 GSPC，可调用生成的 `gsp_<scene>_<object>_get_effective_visible()`。它使用 `esp_gsp_query_visibility()`，检查活动场景、祖先可见状态、动态宽高以及变换后的布局矩形与屏幕/父视口的交集，因此能判断移出 PageFlow 页面或关闭 Drawer 内的对象。

结果表示布局可见性；逐像素遮挡和交互可用性不在计算范围内，包括透明像素、无关
兄弟对象和模态遮罩。声明的可见状态用 `get_visible()` 查询，模板实例使用实例 API。
与其他状态 getter 一样，请在应用的串行回调中查询。

## 拖拽快照诊断

包含 `esp_gsp_debug.h` 后调用 `esp_gsp_drag_snapshot_stats()`。`attempts` 统计快照机会，`started` 统计成功启用，`reused` 统计列表端点窗口复用；`active` 表示当前是否有拖拽快照。`skipped[]` 区分禁用、不可见、无位移、能力不支持、重试阻塞、后端资源限制和其他后端错误，`last_backend_error` 保留原始错误码。

这些计数与 `esp_gsp_transition_stats()` 的场景转场计数独立。未使用快照时仍正常执行实时渲染；在应用串行回调中读取计数，并比较操作前后的差值。
