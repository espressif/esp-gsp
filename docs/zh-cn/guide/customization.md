# 应用自定义扩展

优先使用公开回调和生成辅助函数完成定制，不包含私有头文件，也不替换渲染器。
扩展代码与应用位于同一地址空间，必须遵守缓冲区边界、执行上下文和生命周期约定。

## 选择扩展入口

- 业务动作：声明 `callback`，通过 `esp_gsp_on_event()` 处理。
- 控件状态：使用 `esp_gsp_on_component_event()` 观察支持的控件。
- 输入：pointer observer 用于观察，pinch 用于双指手势；input interceptor
  仅用于应用确实负责整段触摸序列的场景。不要只吞掉已经交给控件的触摸序列的释放事件。
  合成输入不一定经过平台输入拦截路径。
- 自定义像素：短时 CPU 绘制使用 Canvas 回调；相机、视频或工作任务生成的内容使用
  Canvas 帧提交。
- 临时前景图形：使用 overlay contributor。该入口只有一个应用提供者，多个业务模块
  应先在应用层组合，避免相互覆盖注册。
- 动态内容：先使用列表／网格数据源、图片 setter 和外部资源服务，再考虑新资源后端。

回调不获得场景私有状态、缓存条目、显示缓冲区所有权或 DMA fence 的控制权。
显示提交仍由框架负责。

## Canvas 绘制约定

声明带名称和 bind 的不透明图像，使用原生尺寸和无损占位资源，并选择目标的
RGB565 或 RGB888 profile。直接绘制要求未缩放的不透明图像，不等同于支持任意
透明度、变换和布局的自定义控件。停止绘制后恢复占位内容。

同一绑定只使用一个生产者。切换到普通图片 setter 前先停止 Canvas 并同步，
不要交错使用两套资源所有者。

当前运行时原地替换资源视图，因此 Canvas 会拒绝与其他图片绑定、静态元素或图片组共享
视图的注册，保持双方不变；已经接受的借用帧会通过释放回调归还。相同占位图片编译后可能
共用视图，要作为独立 Canvas 使用，需要先具备独立可变视图支持；具名辅助函数本身不解除
这项限制。异步 adapter 中 API 成功只表示入队，目标不兼容会在渲染任务中诊断。
flush 是同步栅栏，不是注册结果查询接口。

需要程序化确认时，包含 `esp_gsp_extension.h`，先通过
`esp_gsp_on_extension_event()` 注册实例观察者，再用
`esp_gsp_canvas_set_draw_cb_ex()` 携带应用分配的请求编号。收到
`ESP_GSP_EXTENSION_CANVAS_RESULT` 后，按 `request_id`、`scene_id`、`bind`
和 `operation` 匹配请求，再判断 `status`。提交返回 ESP_GSP 错误码，执行结果使用 GSP
错误码；执行成功也不代表屏幕已经呈现。失败注册不会解除已有绘制上下文。
请求编号只用于关联结果，不固定场景；和已有 bind setter 一样，目标是在执行时的当前场景。
结果中的 `scene_id` 是实际执行场景，因此生产者切换与导航仍需串行安排。

`esp_gsp_canvas_check()` 只检查当前目标是否兼容直接绘制，不修改资源、不占用 Canvas
槽，也不要求有空闲槽。检查成功不是预留；实际注册仍可能因为容量、场景或资源状态变化失败。
结果流报告到达内核并执行的命令，不能当作所有入队请求的恰好一次完成通知；停止时丢弃的
命令或合并掉的帧不逐一产生结果。需要确认结果时，应在活跃 runtime 上完成 flush，再退出。

`esp_gsp_canvas_set_draw_cb()` 注册绘制函数并请求完整重绘。数据改变后使用
`esp_gsp_canvas_invalidate()` 或 `esp_gsp_canvas_invalidate_dirty()`。
dirty 使用 Canvas 局部坐标，采用半开区间 `[x1, x2) × [y1, y2)`，不能超出声明尺寸。

每次绘制回调必须遵循：

1. `pixels` 已指向当前区域的起点，即 Canvas 局部 `x, y`；只写入
   `width × height` 像素，换行使用 `stride_bytes`。
2. 完整初始化所给区域，不能把未指定的目标像素当作保留背景，不能写行尾 padding，
   也不能保留 surface 指针。
3. 保持短时、非阻塞；不能在绘制过程中调用 setter、invalidate、flush、stop
   或其他修改 GSP 状态的 API。
4. 同一轮重绘的各区域使用相同视觉状态。回调次数不等于帧数。可以在渲染任务的
   timer 或其他串行应用回调中更新状态，再 invalidate。外部任务生产数据时需要
   快照或所有权协议；仅做局部拷贝不能保证并发写入安全。

[媒体示例](../../../examples/usage/sim_bridge_media/README.md) 展示了 timer 驱动的
状态代次，并处理两种原生像素格式。native bridge 使用完整离屏画面，设备端可能按区域
调用；不能用 bridge 的行为推断设备性能和调度。

## 注册与退出

使用 `esp_gsp_on_extension_event_ex()` 可以只订阅需要的事件。比如只观察 Canvas
检查、绘制注册和渲染错误：

```c
esp_gsp_err_t result = esp_gsp_on_extension_event_ex(
    ui, observe, state,
    ESP_GSP_EXTENSION_MASK_CANVAS_RESULT | ESP_GSP_EXTENSION_MASK_RENDER_ERROR,
    ESP_GSP_CANVAS_MASK_CHECK | ESP_GSP_CANVAS_MASK_SET_DRAW);
```

事件掩码和 Canvas 操作掩码同时生效；它们只过滤该实例的观察者，不影响全局钩子。
旧的 `esp_gsp_on_extension_event()` 仍接收运行时支持的全部事件与操作。
未知位立即报参数错误，入队失败也不会改动旧注册；掩码、回调和上下文在同一命令中生效。
零掩码只静音，不解除上下文生命周期责任；释放上下文前仍应清除注册并成功同步。

事件与 pointer observer 的注册和 input interceptor、overlay 一样经过运行时命令路径。
ESP-IDF 外部任务调用成功表示命令已接受，不一定已生效；入队失败保留旧注册。
允许调用这些接口的渲染任务回调中，注册可能立即生效。

每个入口的新注册会替换旧回调。替换完成之前，新旧上下文都必须有效。
从应用任务释放旧的 event／observer／interceptor／overlay 上下文时：

1. 成功清除或替换对应注册。
2. 在同一活跃 runtime 上成功调用 `esp_gsp_flush()`。
3. 确认其他回调和应用工作任务也不再使用该上下文。

注销或 flush 失败时不能释放上下文。不能在回调内等待，也不能在等待期间持有回调需要的锁。
注册和销毁应串行执行；没有任务 adapter 的 portable core，需要应用自行与 app stepping 串行化。

native sim_bridge 是独立客户端实现：注册和回调分发与应用事件循环及 `poll()` 串行执行，
不使用设备命令队列。参见 [bridge 线程约定](../../../tools/sim_bridge/README.md)。

Canvas 则先成功提交 `esp_gsp_canvas_stop()`，再 flush，之后才能释放绘制状态。
整个应用成功 stop 也是生命周期边界。外部生产者应先停止；已经提交的借用帧必须等释放回调，
停止生产者本身不会归还帧。参见[媒体所有权](lifecycle.md#媒体所有权)。

## 兼容边界

这些回调和 Canvas 沿用编译场景模型，不直接增加任意 JSON 元素类型、布局引擎、
自定义 opcode 或运行时格式解析器。优先组合现有控件和 Canvas。
私有源码头文件中的字体、解码、加速或存储 provider 不是应用可依赖的扩展契约。

## 带长度的借用帧

相机或工作任务提供像素时，可以使用 `esp_gsp_canvas_submit_frame()` 或非阻塞的
`esp_gsp_canvas_try_submit_frame()`。它们沿用 Canvas 的原生像素格式、尺寸、单生产者
和借用规则，并检查提供的内存长度：

```c
esp_gsp_canvas_frame_t frame = ESP_GSP_CANVAS_FRAME_INIT();
frame.pixels = producer_pixels;
frame.size_bytes = producer_bytes;
frame.stride_bytes = producer_stride;
frame.on_release = return_to_pool;
frame.release_ctx = producer_frame;
/* frame.dirty 默认为 NULL；也可指向 Canvas 局部脏矩形。 */
esp_gsp_err_t result = esp_gsp_canvas_try_submit_frame(ui, bind, &frame);
if (result != ESP_GSP_OK) {
    return_to_pool(producer_frame); /* 未转移所有权，框架不会调用释放回调。 */
}
```

描述符和脏矩形的值在提交时复制，描述符本身可以放在栈上；像素和释放上下文必须保持有效，
直到释放回调。缓冲区要覆盖 `stride_bytes × 声明高度`，包括最后一行 padding；
stride 还必须能用运行时的 32 位字段表示。不会自动转换像素格式。

ESP-IDF 提交成功只表示入队；长度不足或目标不兼容可能在渲染任务执行时被拒绝，
此时已有画面保持不变，已接受的帧通过释放回调归还一次。队列满或其他立即错误不会触发释放。
释放回调可能早于提交函数返回，不要在成功提交后继续访问或复用生产者对象。
长度必须真实，接口不能识别悬空指针或调用者虚报的分配大小。

native bridge 的非阻塞入口不发起尺寸查询 RPC；在 poll/后续 drain 上传之前才检查长度。
它也会在成功上传前保留已有绘制回调，失败归还已接受帧。bridge 成功上传表示已有副本，
不等于设备呈现。旧的 push/try_push 接口仍可使用，但不提供缓冲区长度校验；
这些接口在运行时接纳 Canvas 时也必须遵守独占资源视图限制。

## 生成的 Canvas 辅助函数

对具有公开 resource bind 的命名 Image，GSPC 生成
`gsp_<scene>_<name>_canvas_set_draw_cb`、`canvas_set_draw_cb_ex`、`canvas_check`、
`canvas_submit_frame`、`canvas_try_submit_frame`、`canvas_invalidate`、
`canvas_invalidate_dirty`、`canvas_push`、`canvas_try_push`、`canvas_push_dirty`、
`canvas_try_push_dirty` 和 `canvas_stop`。
它们仅代入声明的 bind ID，沿用公开 API 的所有权、同步和目标限制。
具体名称和条件可查生成的 `*.api.json`；有辅助函数不等于该目标一定能直接绘制。

媒体示例的注册可以写成：

```c
esp_gsp_err_t result = gsp_media_surface_canvas_set_draw_cb(ui, draw, state);
```

## 可选弱引用业务处理函数

ELF 目标使用 GCC 或 Clang 时，可以在包含生成的 bundle 头文件之前定义
`GSP_GENERATE_WEAK_EVENT_HANDLERS`，启用可选场景分发器。
不定义该宏时，现有事件判断函数仍然可用。假设场景名为 `home`，声明了 `save` 回调：

```c
#define GSP_GENERATE_WEAK_EVENT_HANDLERS
#include "bundle_gsp.h"

void gsp_home_on_save(esp_gsp_handle_t ui,
                      const gsp_home_call_event_t *event, void *user_ctx)
{
    /* 处理动作或通知应用工作任务，不在这里阻塞。 */
    (void)ui;
    (void)event;
    (void)user_ctx;
}

static void app_event(esp_gsp_handle_t ui,
                      const esp_gsp_event_t *event, void *user_ctx)
{
    if (gsp_home_dispatch_call(ui, event, user_ctx)) {
        return;
    }
    /* 在这里处理场景变化、未实现的动作或其他场景。 */
}
```

通过 `esp_gsp_on_event()` 显式注册 `app_event` 并检查返回值；不会自动注册，也不增加
第二条事件通知。分发器仅在匹配且已经实现的处理函数被调用后返回 true；空事件、其他场景、
非 CALL 事件、未知动作或缺少处理函数均返回 false。类型化事件仅在此次调用中有效，不能保留
其指针。普通事件回调的执行上下文和生命周期规则仍然适用。

用户处理函数采用普通 C linkage 声明，只有内部别名引用是弱引用，因此用户定义仍然是强符号，
同一编译单元中在头文件后定义也一样。可以在头文件前定义
`GSP_HOME_EVENT_HANDLER_PREFIX my_app_home`，将处理函数改名为 `my_app_home_on_save`。
分发器与处理函数所在编译单元必须使用相同前缀。前缀管理全局符号名，多实例业务状态使用
`user_ctx`。

**静态归档需要显式保留处理函数。** 弱引用不会单独拉入归档成员。可以直接链接处理函数的
目标文件，或者给最终链接添加 `-Wl,-u,gsp_home_on_save`；自定义前缀时使用实际名称，
并检查 link map。未保留的处理函数视为不存在，分发返回 false，不会表现为注册失败。
其他工具链可以继续使用普通事件判断函数，不启用弱处理宏。

## 内核通知钩子与实例观察者

`esp_gsp_extension.h` 提供六个默认空实现的内核钩子：

- `esp_gsp_hook_app_created`：内核成功初始化，平台显示可能尚未接入。
- `esp_gsp_hook_app_destroying`：该内核开始销毁，资源释放回调可能仍会随后执行。
- `esp_gsp_hook_scene_changed`：场景已提交；不能否决切换，不保证画面已显示。
- `esp_gsp_hook_action`：声明的 CALL 动作，先于普通业务事件回调，不消费事件。
- `esp_gsp_hook_canvas_result`：内核处理 Canvas 命令的结果。
- `esp_gsp_hook_render_error`：受管理的 ESP-IDF 渲染循环或模拟器 session 报告的渲染错误。

ELF GCC/Clang 构建可以在应用中提供同名普通强函数。函数声明不带 weak 属性，C++ 也使用
C linkage。钩子用于记录诊断、更新应用自己的统计或通知工作任务；不能阻塞、修改 GSP 状态、
flush 或 stop，不接管 framebuffer、资源校验和 DMA。普通业务回调仍可按原契约使用 setter。
这些内核钩子不需要 `GSP_GENERATE_WEAK_EVENT_HANDLERS`，该宏只控制生成的业务分发器。
每个全局钩子只能有一个强实现；多个模块应在该实现中显式分发。不同实例可能并发调用全局
钩子，共享状态必须自行同步，示例使用原子计数。

同一事件先调用全局钩子，再调用 `esp_gsp_on_extension_event()` 注册的实例观察者。
实例观察者适合用 `user_ctx` 区分应用实例；各入口不重放历史事件，因此注册后不会补发
APP_CREATED。同一实例中递归发出的扩展通知会被抑制；不能依赖递归通知组织业务流程。

创建／销毁钩子运行在调用 create／destroy 的任务中，其他通知运行在 UI service 上下文；portable core 和 native simulator session
由驱动 core 的调用者线程执行。
创建通知不代表平台 start 已成功，不能立即启动访问 GSP 的工作任务。销毁通知也不是资源已归还
的证明；共享状态应在整个 stop/destroy 成功后释放。事件指针只在回调期间有效。

推荐使用项目辅助函数接入内核钩子源码：

```cmake
# 在应用组件目标建立后调用；这些文件不要再列入 idf_component_register 的 SRCS。
gsp_add_hooks(${COMPONENT_LIB} SOURCES "app_hooks.c" "extra_hooks.cpp")
```

`gsp_add_hooks()` 为每个指定源文件生成一个带独立强锚点的编译包装文件，放入应用原目标中
编译，因此沿用该目标的编译选项、条件依赖和生成头文件顺序。包装文件退出 unity 合并，
保留钩子时不会顺带拉入其他 unity 源码；不要求把整个应用库强制链接。
普通 CMake 项目可以 include 组件的 `cmake/gsp_hooks.cmake`；目标必须是非 imported 的
STATIC library 或 executable，使用 ELF 目标和 GNU 兼容链接器。重复源文件会在配置阶段报错，
重复强函数由链接器报错。
无需手写锚点，用户函数仍应使用普通强定义。设备示例已使用这个入口。

手工接入时，**内核弱定义的静态库保留方式与上方生成弱引用不同。** 默认库已经定义了钩子，单独对钩子使用
`-Wl,-u,esp_gsp_hook_action` 不保证拉入用户覆盖。可以直接链接用户目标文件，或在覆盖所在
文件中定义独立的强锚点，并保留该锚点：

```c
#include "esp_gsp_extension.h"

void my_hooks_link_anchor(void) {}

void esp_gsp_hook_action(esp_gsp_handle_t ui, const esp_gsp_event_t *event)
{
    /* 只做短时观察或通知应用任务。 */
    (void)ui;
    (void)event;
}
```

```cmake
target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,-u,my_hooks_link_anchor")
```

覆盖文件使用 C++ 时，锚点本身也要使用 `extern "C"`，使实际符号名与链接选项一致。

非 ELF 工具链保留默认钩子，使用实例观察者扩展。native sim_bridge 的内核观察、检查和标记绘制注册 API 返回
`ESP_GSP_ERR_NOT_SUPPORTED`，因为客户端回调不能替换另一个进程中的内核钩子；已有 Canvas
和事件接口继续适用。设备侧完整接入示例见[自定义绘制](../../../examples/usage/custom_draw/README.md)。
