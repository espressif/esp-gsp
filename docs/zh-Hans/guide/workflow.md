# 工程工作流

## 先锁定产品约束

编辑 JSON 前记录 IDF target、逻辑分辨率、RGB565/RGB888 Bundle 格式、面板接口、旋转、触控方向、PSRAM 条件和负责 Bundle 的应用组件。SoC 型号本身不能决定这些显示属性。

## 把修改放到正确层级

| 修改 | 负责层 |
|---|---|
| 布局、样式、静态文字与资源 | 场景 JSON |
| 数值、可见性、选择和导航 | 生成式 C API |
| 网络、存储、传感器和业务状态 | 应用任务 |
| 相机、视频、实时像素 | Canvas API |
| 面板时序、缓冲、旋转、触控映射 | BSP/显示目标 |

## 优先默认集成路径

固件自带 UI 时，在应用组件中使用：

```cmake
gsp_add_bundle(${COMPONENT_LIB})
```

它会按名称顺序编译 `scenes/*.json`，默认使用 RGB565。只有产品偏离约定时才显式提供
`SCENES`、`SYMBOL` 或 `PIXEL_FORMAT`。`DYNAMIC_FONT`、`IMAGE_CACHE_BYTES`、
`PROFILE` 和 `DEPLOYABLE` 也只在对应需求存在时添加。

`DEPLOYABLE` 只提供独立 Bundle 加载机制，不定义存储、传输、认证、回滚或分区策略；
这些仍归产品更新层。显示提交默认保持 `ESP_DISPLAY_PRESENT_MODE_AUTO`，除非目标板
测量支持经过评审的覆盖。

## 为生成 API 编写场景

应用需要更新的对象使用稳定 `name`，需要应用响应的动作使用 `callback`，纯装饰对象
保持未命名。固定结构写入场景，复用结构使用 Template，大集合使用 List/Grid/Wheel/
Message List，连续像素使用 Canvas。所有字段都以[场景编写参考](../reference/authoring.md)
为准，不从其他 UI 框架猜测字段。

## 先构建，再写集成代码

先编译场景，再编写集成代码。生成的 `<symbol>_gsp.h` 是当前 Bundle 的准确应用接口。不要修改生成文件、猜 setter，也不要提交 GSPB 或生成头文件。

- 场景的 `name`、`callback`、动态属性、Template 或列表变化后重新构建。
- 普通业务优先生成的类型化函数；通用公共 API 只处理数据驱动需求。
- Raw ID 是高级兼容面；只有真实的通用集成需要时才定义 `GSP_BUNDLE_ENABLE_RAW_IDS`。

## 连接产品状态

应用是产品状态的唯一事实源。UI 事件只请求操作，应用完成业务状态变化后再通过生成
Setter 写回 UI：

```text
UI 回调 → 短消息/通知 → 应用任务
应用状态变化 → 生成 Setter → 渲染任务提交
```

Setter 在 ESP-IDF 上异步执行，成功表示已接受。事件、Timer、集合 Binder、图片释放和
Canvas 回调不得阻塞；存储、网络、应用解码和停止操作都交给应用任务。仅在测试、截图
或有序同步边界使用 `esp_gsp_flush()`。

## 选择数据路径

| 数据形态 | 路径 |
|---|---|
| 构建期图片或字体 | 场景相对资源 |
| 偶尔替换的编码图片 | 命名 Image Setter 或 `esp_gsp_set_image*()` |
| 相机、视频或连续像素 | Canvas Frame 或 Direct Draw |
| 大型应用集合 | List/Grid/Wheel Binder 与复用行 |
| 动态会话数据 | Message List |
| 固定重复结构 | 场景 Template |

选择所有权或缓存配置前阅读[媒体与应用数据](media-and-data.md)。

## 编译器辅助开发

```sh
gspc schema --authoring -o scene.schema.json
gspc diagnose scenes/*.json --format json -o diagnostics.json
gspc docs -o authoring-reference.md
```

这些机器可读产物让编辑器、脚本和 AI 助手都跟随同一编译器注册表。

## 使用 Agent 编写 JSON（Vibe Coding）

GSP 场景适合由 Agent 辅助生成，但 Agent 是场景作者，不是第二套编译器：

安装方式和示例需求见[Agent 辅助 UI 开发](agent-assisted-ui.md)。

1. 明确提供目标分辨率、像素格式、产品行为、可用资源，以及
   `examples/widgets/` 中最接近的例子。
2. 同时提供当前版本的 `scene.schema.json`、字段参考和相关控件页；
   不允许凭其他 UI 框架或旧版本记忆猜字段。
3. 只让 Agent 修改源 JSON 和资源。生成头文件、GSPB、对象 ID、容量计算和
   编译器产物始终由工具生成。
4. 每次有意义的修改后运行 `gspc diagnose` 和真实 ESP-IDF 构建，把准确 JSON
   路径与诊断反馈给 Agent 继续修正。
5. 人工检查 JSON diff，再把本地模拟器预览与目标硬件验收作为两个独立层级。

高质量提示应包含仓库版本、target/profile、场景路径、交互要求和必须通过的
命令。这样可以保留 Vibe Coding 的速度，同时仍受同一份确定性编译契约约束。

## 分层验证

1. 场景生成接受 JSON、资源、Profile 和容量。
2. 应用代码通过生成头文件编译。
3. 指定 IDF target 配置、编译、链接通过。
4. 模拟器/主机测试覆盖渲染和输入语义。
5. 固件启动，真实屏幕和触控链路工作。
6. 人工确认布局、颜色、动画、裁剪、撕裂和触控手感。

主机预览不是硬件证明，干净日志也不是视觉验收。

## 完成检查

- 分辨率和像素格式匹配显示目标。
- 应用控制对象具有稳定名称。
- 回调保持短小，阻塞工作交给应用任务。
- 生成 API 通过重建更新，没有手写复制。
- 动态集合使用模板，并按同时存活峰值配置。
- 模拟器、目标构建、硬件运行和视觉验收分别汇报。
