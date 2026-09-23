# ESP-GSP 配置项参考

运行时容量和策略在源码组件与预编译组件中都由工程配置。预编译库能力是归档发布的只读上限，不会伪装成可修改的 Kconfig 项。普通场景需求优先由 JSON 与 GSPC 推导。

## 常用选项

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_ENABLE_IMAGE_CACHE` | 按需缓存解码后的图片 | `true` | `bool` / `布尔值` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ENABLE_ASYNC_DECODE` | 在后台解码图片 | `true` | `bool` / `布尔值` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ENABLE_TRANSITION_SNAPSHOTS` | 在可用时使用转场快照 | `true` | `bool` / `布尔值` | 可配置 | 否 |
## 高级设置（可选） / 容量与功能上限 / 核心对象和组件池

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_MAX_SCENES` | 场景容量（0 = 自动） | `0` | `int` / `0..65534` | 可配置 | 是 |
| `CONFIG_ESP_GSP_MAX_TIMERS` | 应用可同时使用的定时器数量 | `8` | `int` / `1..32` | 可配置 | 否 |
| `CONFIG_ESP_GSP_MAX_WIDGETS` | 同时存活的模板控件实例数量 | `16` | `int` / `1..255` | 可配置 | 否 |
| `CONFIG_ESP_GSP_MAX_ANIMATIONS` | 同时运行的运行时动画数量 | `32` | `int` / `0..128` | 可配置 | 否 |
| `CONFIG_ESP_GSP_MAX_LISTS` | 列表与滚轮容量（0 = 自动） | `0` | `int` / `0..256` | 可配置 | 是 |
| `CONFIG_ESP_GSP_LIST_MAX_SLOTS` | 每个列表的可见行数（0 = 自动） | `0` | `int` / `0..65535` | 可配置 | 是 |
| `CONFIG_ESP_GSP_LIST_TEXT_SLOTS` | 每行的动态文字字段数（0 = 自动） | `0` | `int` / `0..65533` | 可配置 | 是 |
| `CONFIG_ESP_GSP_COMPONENT_INSTANCES` | 组件实例容量（0 = 自动） | `0` | `int` / `0..256` | 可配置 | 是 |
| `CONFIG_ESP_GSP_STACK_VIEW_MAX_DEPTH` | 导航栈深度（0 = 自动） | `0` | `int` / `0..64` | 可配置 | 是 |
## 高级设置（可选） / 容量与功能上限 / 渲染器和帧规划

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_COMPONENT_OVERLAY_COMMANDS` | 组件 Overlay 的基础绘制命令容量 | `32` | `int` / `2..128` | 可配置 | 否 |
| `CONFIG_ESP_GSP_DIRTY_RECT_CAPACITY` | 每帧保留的脏矩形数量 | `32` | `int` / `4..128` | 可配置 | 否 |
## 高级设置（可选） / 容量与功能上限 / Canvas、文字、图片和字体池

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_CANVAS_SLOTS` | 外部帧接收目标（Canvas）数量 | `2` | `int` / `1..8` | 可配置 | 否 |
| `CONFIG_ESP_GSP_MAX_ASSET_ANIMS` | 每场景编译动画数（0 = 自动） | `0` | `int` / `0..255` | 可配置 | 是 |
| `CONFIG_ESP_GSP_TEXT_SLOTS` | 动态文字容量（0 = 自动） | `0` | `int` / `0..65533` | 可配置 | 是 |
| `CONFIG_ESP_GSP_DEFAULT_DYNAMIC_IMAGE_SLOTS` | 动态图片容量（0 = 自动） | `0` | `int` / `0..32767` | 可配置 | 是 |
| `CONFIG_ESP_GSP_FREETYPE_CACHE_GLYPHS` | 默认 FreeType 字形缓存条目数 | `32` | `int` / `1..512` | 可配置 | 否 |
| `CONFIG_ESP_GSP_FREETYPE_GLYPH_MAX_PX` | 默认 FreeType 字形最大宽高 | `40` | `int` / `8..256` | 可配置 | 否 |
## 高级设置（可选） / 工程运行时默认值 / 渲染加速

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_ACCEL_MAX_DRIVERS` | 可注册的硬件加速后端数量 | `4` | `int` / `1..16` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ACCEL_MAX_PENDING_BLITS` | 同时在途的异步加速 Blit 数量 | `4` | `int` / `1..16` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ACCEL_SYNC_TIMEOUT_MS` | 等待加速器完成的超时时间（毫秒） | `500` | `int` / `1..10000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ACCEL_MIN_BLIT_PIXELS` | 启用加速复制的最小像素数 | `8192` | `int` / `0..1048576` | 可配置 | 否 |
| `CONFIG_ESP_GSP_PPA_MIN_FILL_PIXELS` | 启用 PPA 填充的最小像素数 | `32768` | `int` / `0..1048576` | 可配置 | 否 |
| `CONFIG_ESP_GSP_PPA_MIN_BLEND_PIXELS` | 启用 PPA 混合或淡变的最小像素数 | `8192` | `int` / `0..1048576` | 可配置 | 否 |
| `CONFIG_ESP_GSP_PPA_RUNTIME_SCALE` | 是否使用 PPA SRM 加速运行时图片缩放 | `false` | `bool` / `布尔值` | 可配置 | 否 |
| `CONFIG_ESP_GSP_PPA_MIN_SCALE_PIXELS` | 启用 PPA 运行时缩放的最小目标像素数 | `32768` | `int` / `0..1048576` | 可配置 | 否 |
| `CONFIG_ESP_GSP_PPA_FILL_MAX_SPAN_PIXELS` | 单次 PPA 填充 Span 的最大像素数 | `524288` | `int` / `1024..16777216` | 可配置 | 否 |
## 高级设置（可选） / 工程运行时默认值 / 运行时池和图片缓存

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_IMAGE_CACHE_ENTRIES` | 默认解码图片缓存条目数 | `16` | `int` / `0..512` | 可配置 | 否 |
| `CONFIG_ESP_GSP_IMAGE_CACHE_SHORTAGE_RETRIES` | 图片缓存分配不足时的重试次数 | `3` | `int` / `0..16` | 可配置 | 否 |
| `CONFIG_ESP_GSP_CONTEXT_DEFAULT_GLYPH_RUNS` | 文字塑形容量（0 = 自动） | `0` | `int` / `0..65535` | 可配置 | 是 |
| `CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES` | 模板实例容量（0 = 自动） | `0` | `int` / `0..65534` | 可配置 | 是 |
| `CONFIG_ESP_GSP_INSTANCE_STATES_PER_SLOT` | 每个模板实例的状态容量（0 = 自动） | `0` | `int` / `0..65535` | 可配置 | 是 |
| `CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MIN_BYTES` | 有 PSRAM 时图片缓存自动预算下限 | `65536` | `int` / `0..16777216` | 可配置 | 否 |
| `CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MAX_BYTES` | 图片缓存自动预算上限 | `4194304` | `int` / `0..67108864` | 可配置 | 否 |
| `CONFIG_ESP_GSP_IMAGE_CACHE_HOST_DEFAULT_BYTES` | 非 ESP 主机的图片缓存默认预算 | `2097152` | `int` / `0..67108864` | 可配置 | 否 |
## 高级设置（可选） / 工程运行时默认值 / 运行时任务 / 渲染任务

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_RENDER_TASK_STACK_PSRAM` | 把渲染任务栈分配到 PSRAM | `false` | `bool` / `布尔值` | 可配置 | 否 |
| `CONFIG_ESP_GSP_RENDER_TASK_STACK_SIZE` | 渲染任务栈大小（字节） | `12288` | `int` / `4096..65536` | 可配置 | 否 |
| `CONFIG_ESP_GSP_RENDER_TASK_STACK_SIZE_FREETYPE` | 启用 FreeType 时的渲染任务栈大小（字节） | `24576` | `int` / `8192..65536` | 可配置 | 否 |
| `CONFIG_ESP_GSP_RENDER_TASK_PRIORITY` | 渲染任务默认优先级 | `4` | `int` / `1..24` | 可配置 | 否 |
## 高级设置（可选） / 工程运行时默认值 / 运行时任务 / 后台解码任务

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_DECODE_TASK_STACK_PSRAM` | 把解码任务栈分配到 PSRAM | `false` | `bool` / `布尔值` | 可配置 | 否 |
| `CONFIG_ESP_GSP_DECODE_TASK_STACK_SIZE` | 解码任务栈大小（字节） | `4096` | `int` / `2048..32768` | 可配置 | 否 |
| `CONFIG_ESP_GSP_DECODE_TASK_PRIORITY` | 独立解码任务默认优先级 | `3` | `int` / `1..24` | 可配置 | 否 |
| `CONFIG_ESP_GSP_DECODE_TASK_IDLE_POLL_MS` | 解码任务空闲轮询间隔（毫秒） | `100` | `int` / `1..10000` | 可配置 | 否 |
## 高级设置（可选） / 工程运行时默认值 / 动画与转场

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_ANIM_FRAME_MEMORY` | 动画帧内存区域偏好 | `auto` | `enum` / `0..2` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ANIM_MAX_FRAME_BYTES` | 单个运行时动画帧最大字节数 | `16777216` | `int` / `1024..67108864` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ANIM_INTERNAL_FRAME_MAX_BYTES` | 优先放在 SRAM 的动画帧大小上限 | `65536` | `int` / `0..16777216` | 可配置 | 否 |
## 高级设置（可选） / 调度与交互 / 运行节奏和命令队列

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_QUEUE_DEPTH` | 平台命令队列深度 | `24` | `int` / `4..128` | 可配置 | 否 |
| `CONFIG_ESP_GSP_ACTIVE_TICK_MS` | 活动渲染循环 Tick 间隔（毫秒） | `10` | `int` / `1..1000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_IDLE_POLL_MS` | 应用空闲轮询默认间隔（毫秒） | `100` | `int` / `1..10000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_POINTER_POLL_MS` | 活动指针轮询间隔（毫秒） | `33` | `int` / `1..1000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_TOUCH_RELEASE_CONFIRM_POLLS` | 确认触摸释放所需的连续空采样次数 | `2` | `int` / `1..8` | 可配置 | 否 |
## 高级设置（可选） / 调度与交互 / 滚动物理

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_SCROLL_VELOCITY_WINDOW_MS` | 计算滚动释放速度的时间窗口（毫秒） | `100` | `int` / `10..1000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCROLL_RELEASE_GRACE_MS` | 滚动释放动作宽限时间（毫秒） | `100` | `int` / `0..1000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCROLL_STOP_SPEED_PX_S` | 惯性滚动停止速度（像素/秒） | `20` | `int` / `1..1000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCROLL_MIN_FLING_SPEED_PX_S` | 启动惯性滚动的最小释放速度（像素/秒） | `160` | `int` / `0..5000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCROLL_MAX_RELEASE_SPEED_PX_S` | 滚动释放速度上限（像素/秒） | `6000` | `int` / `100..50000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCROLL_DECAY_MS` | 滚动速度衰减时间（毫秒） | `325` | `int` / `10..5000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCROLL_MAX_TICK_MS` | 单次滚动积分允许的最大 Tick（毫秒） | `100` | `int` / `1..1000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCROLL_SNAP_RESPONSE_MS` | 列表行吸附响应时间（毫秒） | `40` | `int` / `1..1000` | 可配置 | 否 |
## 高级设置（可选） / 调度与交互 / 滑动和组件运动

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_SWIPE_VELOCITY_WINDOW_MS` | 页面滑动速度窗口（毫秒） | `80` | `int` / `10..1000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_RELEASE_GRACE_MS` | 页面滑动释放动作宽限时间（毫秒） | `350` | `int` / `0..2000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_SETTLE_MIN_MS` | 页面滑动归位最短时间（毫秒） | `80` | `int` / `1..2000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_SETTLE_BASE_MS` | 页面滑动归位基础时间（毫秒） | `110` | `int` / `1..2000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_SETTLE_DISTANCE_MS` | 页面滑动按距离增加的归位时间（毫秒） | `90` | `int` / `0..2000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_SETTLE_MAX_MS` | 页面滑动归位最长时间（毫秒） | `220` | `int` / `1..5000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_SETTLE_SPEED_REDUCTION_MS` | 按速度缩短归位时间的比例（毫秒） | `60` | `int` / `0..2000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_SETTLE_MAX_SPEED_REDUCTION_MS` | 按速度缩短归位时间的最大值（毫秒） | `120` | `int` / `0..2000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_PAGE_FLOW_SETTLE_MS` | 旧版 PageFlow 默认归位时间（毫秒） | `220` | `int` / `1..5000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_STACK_VIEW_SETTLE_MS` | StackView 默认归位时间（毫秒） | `220` | `int` / `1..5000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_DRAWER_SETTLE_MS` | Drawer 默认归位时间（毫秒） | `180` | `int` / `1..5000` | 可配置 | 否 |
| `CONFIG_ESP_GSP_BOOLEAN_TWEEN_MS` | Toggle/Checkbox 滑块过渡时间（毫秒） | `130` | `int` / `1..5000` | 可配置 | 否 |
## 高级设置（可选） / 调度与交互 / 手势阈值

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `CONFIG_ESP_GSP_TAP_SLOP_PX` | 点击判定允许的移动范围（像素） | `16` | `int` / `0..128` | 可配置 | 否 |
| `CONFIG_ESP_GSP_DRAG_START_PX` | 拖动或手势开始阈值（像素） | `12` | `int` / `0..256` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_FLICK_PX` | Flick 提交距离（像素） | `12` | `int` / `0..256` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCENE_SWIPE_MIN_FLING_PX` | Scene、PageFlow 或 Drawer 的最小 Fling 位移（像素） | `24` | `int` / `0..512` | 可配置 | 否 |
| `CONFIG_ESP_GSP_COMPONENT_SWIPE_COMMIT_DIVISOR` | 组件滑动提交距离除数 | `4` | `int` / `2..16` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SCENE_SWIPE_COMMIT_DIVISOR` | 场景滑动提交距离除数 | `5` | `int` / `2..16` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_VELOCITY_NUMERATOR` | 滑动归位速度计算分子 | `3` | `int` / `1..16` | 可配置 | 否 |
| `CONFIG_ESP_GSP_SWIPE_VELOCITY_DENOMINATOR` | 滑动归位速度计算分母 | `5` | `int` / `1..32` | 可配置 | 否 |
| `CONFIG_ESP_GSP_DRAWER_EDGE_SLOP_PX` | Drawer 边缘激活范围（像素） | `24` | `int` / `0..256` | 可配置 | 否 |
## 预编译库能力

| 配置或能力 | 作用 | 默认值 | 类型 / 范围 | 预编译组件 | JSON 自适应 |
|---|---|---:|---|---|---|
| `ESP_GSP_BUILD_CAP_COMPONENT_BATCH_MAX` | 一次原子组件批处理允许的最大更新数 | `64` | `int` / `1..64` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_TRANSACTION_UPDATE_CAPACITY` | 输入与组件事务的内部更新容量 | `64` | `int` / `1..64` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_RENDER_CLIP_STACK_DEPTH` | 渲染器最大嵌套裁剪深度 | `128` | `int` / `4..128` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_RENDER_TILE_SPAN_CAPACITY` | Tile Index 快速路径的 Span 容量 | `256` | `int` / `1..256` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_TEXT_CAPACITY` | 动态文字命令的内联字节数 | `63` | `int` / `1..1024` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_MAX_DYNAMIC_IMAGE_TARGETS` | 运行时图片目标的库能力上限 | `32767` | `int` / `1..32767` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_MAX_FONTS_PER_SCENE` | 单个场景引用字体包的数量上限 | `32` | `int` / `1..32` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_ANIM_PATCH_RECTS` | 每个编译动画帧保留的脏块数量上限 | `32` | `int` / `1..32` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_ANIM_REFERENCE_COMMANDS` | 动画可见性引用命令容量 | `32` | `int` / `1..32` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_SCROLL_PHYS_SAMPLES` | 滚动速度采样容量 | `32` | `int` / `2..32` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_SWIPE_KINETICS_SAMPLES` | 页面滑动速度采样容量 | `32` | `int` / `2..32` | 固定上限 | 否 |
| `ESP_GSP_BUILD_CAP_MAX_TOUCH_POINTS` | 每次采样处理的触点数量上限 | `2` | `int` / `1..2` | 固定上限 | 否 |

`AUTO` 项的零值表示使用 Bundle 需求或目标默认值；非零值是明确约束。容量按对应资源的运行时生命周期配置：可回收池取同时存活峰值，保留型资源覆盖整个 UI 实例。配置层级、Slot 与动态实例的选择方法见[配置模型](../guide/configuration.md)。
