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

Carousel 回调在选中索引提交后触发。请通过生成的 `get_selected()` 或
`get_value()` 读取索引；其 CALL 事件的 `event->arg` 仍是声明的参数，
不是选中索引。

## 事件与任务上下文

事件、列表绑定、图片释放、定时器和 Canvas 回调运行在框架任务中。不要在其中阻塞、访问慢速存储或执行网络操作；只发送队列/任务通知并尽快返回。

应用负责的返回行为使用 `callback`，StackView 返回使用 `stack_pop`。JSON 的
`back` 动作为保留能力，当前公共运行时不会分发。

```text
UI 事件 → 短回调 → 应用任务 → 产品状态改变
                                  → 生成 setter → 渲染提交
```

### 定时器生命周期

`esp_gsp_timer_cb_t` 在渲染任务中执行。接入 ESP-IDF runtime 后，应用任务
可以创建或删除定时器（ISR 中不可以）；调用期间必须保持应用和 runtime 存活。
删除只会停止后续调度，不等待已经选中执行的回调，因此 `user_ctx` 必须保持有效，
直到回调返回。使用同一个持续接入的 runtime 时，外部任务只能在删除成功后，再成功
调用 `esp_gsp_flush()` 来等待正在执行的回调；只有此时才能释放 `user_ctx`。删除失败或
flush 失败/超时都不能确认回调已经退出。不要在定时器回调中调用 flush，等待时也不要
持有回调所依赖的锁。如果其他定时器或异步任务共享该 `user_ctx`，释放前还需等待这些
使用者结束。未接入 ESP-IDF adapter
时，定时器操作必须与 app stepping 串行化。定时器删除后不得复用其句柄；内部定时器槽位
可能会复用。

## 不再轮询组件状态

通过 `esp_gsp_on_component_event()` 订阅命名 PageFlow、Drawer 的变化，使用生成的
`GSP_OBJ_KEY_*` 匹配 `event->key`。这是独立回调，不改变原有 `esp_gsp_on_event()`
及其事件结构。`VALUE_CHANGED` 表示已提交页码或打开状态变化；`MOTION_FINISHED`
也覆盖回弹原位；`MOTION_CHANGED` 表示目标、拖动或缓动状态变化，不逐像素通知。
这些标志可以同时出现。

`event->state.value` 是已提交值，`target` 是目标值；Drawer 使用 0/1 表示关闭/打开。
单独读到 `is_open()==false` 不能判断打开动画是否结束。需要初始状态时使用
`esp_gsp_component_get_motion()`。旧 getter 语义不变；stop-anywhere PageFlow
的选中页可能在惯性滑行期间改变，不等于运动已完成。

通知在组件和变换更新后的安全阶段派发，同一组件同一 UI step 内合并，不是每条命令
的历史记录，也不代表 LCD 已显示。注册不发送初始事件，场景销毁会丢弃待发通知。
回调可以调用 setter，但不能 flush、阻塞或销毁应用。外部任务解绑回调并成功同步后，
才能释放仍可能被回调使用的上下文。

## 首帧前初始化

使用 `esp_gsp_esp_lcd_start_prepared()`，在渲染任务首帧前完成绑定、注册事件和创建
定时器；它复用 session 的 prepare 回调类型，传 NULL 保持普通 start 行为。
回调前已同步时钟。prepare 没有错误返回通道，应在
回调中检查 setter 结果，并通过上下文记录业务初始化错误；不能在其中调用 flush、
stop 或 session 生命周期接口。

## 可见性、输入与抽屉边缘入口

可见性控制显示，声明的 `enabled` 控制是否接受新输入，不禁止程序化切页或开关抽屉。
需要动态禁止交互的 PageFlow/Drawer 应显式声明 `enabled:true`，结构手势候选会检查
该属性，包括关闭 Drawer 的边缘拉出。可见的打开/关闭动画中的 Drawer 即使 disabled，
仍保留模态点击阻挡；hidden Drawer 不再抢边缘滚动或吞底层点击。

关闭但启用的 Drawer 仍可从边缘拉开，这是保留行为。禁止该入口只需禁用 Drawer，
不必为此同时隐藏；这不会取消已经获得所有权的手势。普通 hit 仍按最近显式 enabled
声明继承，子组件显式启用可覆盖父组件；结构手势检查自身 enabled，不应依赖父属性禁用。

`block_scene_swipe` 仍只控制场景导航，绘制遮罩也不自动拦截全部手势。业务模态层显示
期间应显式禁用底层 PageFlow，并通过完成事件恢复。应用接管顶层输入时，可复用现有
`esp_gsp_set_input_interceptor()` 在命中和手势识别前拦截；应处理完整触摸序列，
而不是只吞下按下样本。

## 动态内容

| 内容 | 推荐 API 路径 |
|---|---|
| 命名数值、文字、可见性、选择状态 | 生成对象辅助函数 |
| 应用持有的行或单元格 | List/Grid/Wheel/Message List 适配器 |
| 偶尔替换的编码图片 | 生成图片 setter 或 `esp_gsp_set_image*()` |
| 相机、视频、连续像素 | Canvas 帧或 Direct Draw 回调 |
| 页面与堆栈导航 | 生成导航/组件辅助函数 |

COPY/BORROW/TAKE 缓冲区释放、回调上下文和停止顺序遵循[生命周期](lifecycle.md#媒体所有权)中的约定。
BORROW 缓冲区收到释放回调后才能释放或复用；停止生产者本身不会归还已经提交的缓冲区。

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
