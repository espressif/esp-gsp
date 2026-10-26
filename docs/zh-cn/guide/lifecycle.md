# 生命周期与线程

## 推荐 API 层级

普通应用使用满足需求的最窄接口：

1. 包含生成的 `<symbol>_gsp.h`。
2. 从 `gsp_<symbol>_config()` 获得 Bundle 配置。
3. 用生成的 `gsp_<scene>_<object>_*()` 操作命名控件。
4. 用 `esp_gsp_on_event()` 配合生成的事件判断函数。
5. 只有生成对象 API 不覆盖数据驱动需求时，才使用通用图片、Canvas、集合或导航 API。

Bind、稳定 Key/Property 和原始 Widget/Template API 用于兼容、编辑器或通用桥接，
不是另一套应用启动方式。独立交付的 UI 包可用 `DEPLOYABLE` 打包，再通过
`esp_gsp_deployable_bundle_open()` 与 `esp_gsp_deployable_bundle_make_config()`
进入同一运行时。

## 启动顺序

1. BSP 初始化面板、Framebuffer、旋转、字节序和可选触控。
2. 应用包含 Bundle 头文件，从 `gsp_<symbol>_config()` 创建配置。
3. 使用 `ESP_GSP_ESP_LCD_CONFIG_INIT()` 创建显示适配配置。
4. 调用 `esp_gsp_esp_lcd_start()`，再注册事件、列表、媒体或 Canvas 回调。
5. 通过命名控件 API 提交初始产品状态。

若绑定、事件或定时器必须在首帧前就绪，使用
`esp_gsp_esp_lcd_start_prepared()` 的准备回调完成初始化。

## 配置生命周期

`gsp_<symbol>_config()` 会写入 `struct_size`、Bundle 和生成目录。手工构造配置时从
`ESP_GSP_CONFIG_INIT()` 开始；LCD 配置从 `ESP_GSP_ESP_LCD_CONFIG_INIT()` 开始。

Bundle 字节、组件目录、可选轮廓字体字节、借用的字体目录及其底层字体数据必须至少存活到
`esp_gsp_stop()` 成功。Deployable 路径还要求调用方拥有的 GSPB 字节和
`esp_gsp_deployable_bundle_t` 在停止前有效；先停止 UI，再关闭 Deployable Handle。

编译后的 Bundle 字节应保持不可变。同一次加载可以复用场景、共享内嵌图片及字体已经通过的
CRC 校验结果，但仍执行结构和边界检查。校验结果不跨 UI 实例保留，外部字体目录
单独校验。

## 提交状态与异步更新

ESP-IDF 加速器由 UI 实例共享。首次有效引用设置策略，后续引用保留该策略并维持
后端寿命；同时运行的实例应使用一致的加速设置。释放所有实例后再改变共享策略，
启动失败不会修改已有实例的策略。

ESP-IDF 上的 setter 把更新提交到渲染任务。返回成功表示更新已接受，不表示屏幕已经
完成提交。普通业务流不要每次 setter 后调用 `esp_gsp_flush()`；仅在测试截图、顺序
切换或停止前等明确同步边界使用。

```text
应用任务 → 命令队列 → 渲染任务 → 状态提交 → 渲染
```

Getter 读取当前场景已经提交的状态；另一个任务刚调用 Setter 后立即读取，仍可能看到
旧值。相关标量必须原子可见时，使用 `esp_gsp_component_set_many()` 或
`esp_gsp_component_set_properties()`，不要依赖多个异步 Setter 的到达顺序。

## 同步栅栏

`esp_gsp_flush(ui, timeout_ms)` 等待调用前已提交的命令被应用，并等待随后一次显示提交
尝试完成。它适用于确定性测试、截图和有序停止，不应放入动画或遥测循环，也不能从
渲染或解码任务回调中调用。超时不会取消已经进入队列的栅栏；渲染错误仍通过
`esp_gsp_render_error_stats()` 获取。

后台图片替换在栅栏完成时仍可能正在解码。需要截图包含新图片时，先等待对应图片完成回调
报告 `GSP_OK`，再用 flush 同步后续渲染尝试；flush 不代替解码完成通知。

## 回调上下文

以下回调运行在框架任务上，必须快速返回：

- UI 事件与定时器；
- List/Grid/Wheel/Message List 数据绑定；
- 图片释放；
- Canvas Direct Draw；
- 指针观察和 pinch 通知。

网络、存储、传感器访问和产品状态转换应通过队列或任务通知交给应用任务。应用完成
处理后，再通过 setter 更新界面。

普通渲染任务回调可以使用 setter，状态可直接应用；Canvas Direct Draw 正处于绘制过程，
不能调用修改 GSP 状态的 API。不要从渲染或解码任务回调中调用 flush 或 stop。

## 媒体所有权

| 模式 | 调用后缓冲区责任 |
|---|---|
| COPY | 框架复制数据，调用返回后应用可释放原缓冲区 |
| BORROW | 保持字节不可变，直到该缓冲区的释放回调执行 |
| TAKE | 仅成功提交后转移所有权；提供释放回调时由回调释放，否则框架调用 `free()` |

立即提交失败不转移 BORROW/TAKE 所有权，也不会随后调用释放或完成回调。
已发布图片会保留编码输入，以便缓存淘汰后重新解码；预算还须包含已接受但仍在队列、
解码或等待发布的替换输入。取消请求后，在途解码也可能暂时保留输入。

Canvas、列表数据源和回调上下文同样遵守各自的绑定生命周期。成功提交
`esp_gsp_canvas_stop()` 并成功 flush 后才能释放 Direct Draw 上下文；借用帧必须等各自
释放回调后才能复用。停止生产者本身不会归还已经提交的帧。

## List 绑定生命周期

每个逻辑 List 或 Wheel 在一个 UI 实例内只绑定一次，并保存返回的 Handle。绑定会在
场景切换时保留：页面隐藏时行实例被停放，再次可见时重新激活。

不要在每次 Scene Ready 事件中重复调用 `esp_gsp_list_bind_component()`，否则会持续
占用 List Pool，最终返回 `ESP_GSP_LIST_NONE`。数据总数变化时调用生成的
`set_total()`，可见数据变化时调用 `refresh()`。当前 API 没有单独 Unbind；所有绑定
在 `esp_gsp_stop()` 中释放。

## 停止顺序

1. 应用停止产生新帧、新行数据和新异步请求。
2. 解绑或关闭外部生产者，等待其不再进入框架。
3. 从应用任务调用 `esp_gsp_stop()`，不要在框架回调内部停止。
4. 停止完成后再释放借用缓冲、数据源、字体、Bundle 和 BSP 资源。

挂起/恢复只暂停运行行为，不转移资源所有权。需要销毁资源时仍应完成正式停止流程。

`esp_gsp_stop()` 可能返回 `ESP_GSP_ERR_TIMEOUT`；此时 Handle 仍归应用所有，可以重试。
成功后 Handle 和框架拥有的全部回调注册均已失效。

## 暂停和恢复同一个应用

`esp_gsp_esp_lcd_pause()` 暂停当前 UI 并等待 Presenter 栅栏，不销毁应用。
保存 Pause Token，通过 `esp_gsp_esp_lcd_resume_paused()` 恢复原应用；保留 Hub 会话中的
临时应用也使用同一路径。

pause 返回 `ESP_ERR_NOT_FINISHED` 时，表示回滚恢复失败，`out_pause` 会返回可用于
重试 `resume_paused()` 的有效 Token。这不代表显示交接已经就绪：传输／呈现栅栏可能
仍未完成，不能把显示交给另一个生产者。其他 pause 错误不转交 Token。
resume 失败时原应用和 Token 继续保留，供再次恢复；只有成功才消费 Token。
全过程都要保留借用资源。

## 挂起与临时应用

ESP-LCD 适配器可以保留一个 Hub UI 与显示任务，同时在同一显示 Host 上运行临时
Bundle：

```text
Hub → suspend → Session → 启动临时应用 → 停止临时应用并恢复 Hub
```

从应用任务调用 `esp_gsp_esp_lcd_suspend()`。成功后 Session 持有 Hub 的运行时、
场景和组件状态，已解码图片 Surface 会释放，挂起期间不得再通过 Hub Handle 提交工作。

若 suspend 返回 `ESP_ERR_NOT_FINISHED`，表示回滚也未能恢复 Hub，`out_session` 仍会
返回一个没有临时应用的有效 Session。保留它，通过
`esp_gsp_esp_lcd_resume(session, NULL, &hub)` 重试，或销毁 Session。
其他 suspend 错误不会向调用方转交 Session。

`esp_gsp_esp_lcd_start_on_session()` 要求临时 Bundle 的逻辑宽高和像素格式与保留显示
一致。常驻渲染任务不会扩大栈；新应用需求超过原预算时返回 `ESP_ERR_INVALID_SIZE`。
创建 Hub 时用 `task_stack_size` 覆盖最耗栈的临时应用；标准构建中的动态字体或矢量路径
可能至少需要 32 KiB。临时应用配置不能扩大已有任务，详见[栈预算](../reference/configuration.md#内存吃紧时如何调整)。

用返回的临时应用 Handle 调用 `esp_gsp_esp_lcd_resume()`；它会停止临时应用、
恢复 Hub、安排完整首帧，并在成功时消费 Session。没有临时应用时可用
`esp_gsp_esp_lcd_session_destroy()` 销毁挂起的 Hub 和显示 Host。
临时应用应通过 Session 的 resume/replace 接口停止；共享 Hub 运行时的临时 Handle
不能使用 `esp_gsp_stop()`。

临时应用的创建、兼容性或激活失败后，适配器尝试恢复 Hub。恢复成功会消费 Session，
并返回原启动错误；`ESP_ERR_NOT_FINISHED` 表示 Hub 恢复也失败，Session 仍有效且没有
临时应用，`out_gsp` 为 NULL，可使用 `app = NULL` 重试 resume 或销毁。入口指针／会话状态校验拒绝则
保持 Session 不变。替换临时应用的启动失败遵循同一规则。

resume 在暂停临时应用时失败，会保留该应用；临时应用已销毁、但 Hub 恢复失败时，
返回 `ESP_ERR_NOT_FINISHED`，`out_hub` 为 NULL；此后用 `app = NULL` 重试，不能再传已销毁的 Handle。
Session 仅在恢复或销毁成功时消费。销毁失败后只继续重试销毁，不再 start/resume；
借用的素材和显示资源必须保留到销毁成功。

完整函数签名见[函数级 API 参考](../reference/api-functions.md)。
