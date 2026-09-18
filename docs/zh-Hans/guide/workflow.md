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
这些仍归产品更新层。显示提交默认使用 `ESP_DISPLAY_PRESENT_MODE_AUTO`；需要调优时，
结合目标板的性能测量调整。

## 为生成 API 编写场景

应用需要更新的对象使用稳定 `name`，需要应用响应的动作使用 `callback`，纯装饰对象
保持未命名。固定结构写入场景，复用结构使用 Template，大集合使用 List/Grid/Wheel/
Message List，连续像素使用 Canvas。所有字段都以[场景编写参考](../reference/authoring.md)
为准，使用该版本定义的字段。

## 先构建，再写集成代码

先编译场景，再编写集成代码。生成的 `<symbol>_gsp.h` 是当前 Bundle 的准确应用接口。使用生成头文件中的声明和编辑器补全；接口变更通过修改 JSON 后重新构建完成。

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
gspc doctor .
gspc cards slider
gspc schema --authoring -o scene.schema.json
gspc diagnose scenes/*.json --format json -o diagnostics.json
gspc docs -o authoring-reference.md
```

这些机器可读产物让编辑器、脚本和 AI 助手都跟随同一编译器注册表。
优先用 `gspc cards` 查看单个控件；需要字段范围、版本或完整注册表时再导出 Schema、docs 或 inventory。
编译或打包还会在头文件旁写出 `*.api.json`，用于按对象名对照生成 API 与模拟器映射。

## Agent 辅助编写

向 Agent 提供场景路径、目标分辨率、像素格式、交互需求和资源，并附上相关控件示例。
通过 GSPC 生成资源与应用接口，使用构建诊断继续调整源 JSON。
安装和提示示例见[Agent 辅助 UI 开发](agent-assisted-ui.md)。

## 检查应用

1. 执行 `idf.py build`，检查场景、资源和应用代码的构建结果。
2. 在模拟器中检查布局、数据更新和交互。
3. 烧录到目标板，检查颜色、旋转、裁剪、动画、触控和内存使用。

## 完成检查

- 分辨率和像素格式匹配显示目标。
- 应用控制对象具有稳定名称。
- 回调保持短小，阻塞工作交给应用任务。
- 生成 API 通过重建更新，没有手写复制。
- 动态集合使用模板，并按同时存活峰值配置。
- 在目标板上检查显示、触控和资源使用。

## 多场景构建与输入事件

`gsp_add_bundle()` 通过 `compile` 链接多场景，共享图片和字体字符集。CLI 的 `pack`
使用相同链接语义；组装已有二进制成员使用 `bundle`。需要独立生成工程产物时：

```sh
gspc compile scenes/*.json -o gsp-out/app.gspb \
  --api-header gsp-out/app_gsp.h --depfile gsp-out/app.d --symbol app
```

场景 ID 由显式输入顺序决定；CMake 自动发现时按文件名排序。它不是跨不同 Bundle
版本的永久编号。JSON 修改会触发重建，生成的 action 判断函数同时检查场景和局部编号。

Arc 需要显式 `bind`（例如 `bind: "volume"`）才能启用指针拖动；仅 `name` 和数值
setter 不会把仪表变成拖动控件。Slider 默认可拖动。

Slider/Arc 可在 `events` 中声明 `value -> call` 和 `release -> call`，分别处理值变化
与拖动提交。`arg` 转为 `int32_t` 后是声明的 `min/max` 单位；重复值不重复通知。
按下改变数值也会产生 `value`，拖出控件后释放仍产生 `release`；取消不产生提交。
只有 `callback` 而没有 `events` 的 Slider/Arc 默认在释放时回调。应用主动 setter
不会产生这些用户输入回调。回调里将业务工作投递到应用任务。

这两个事件也支持 `set_value`、`add_value`、显隐和导航等声明式动作，按 JSON 顺序执行，
状态写入在事件事务结束时提交。只有 `call` 的 `arg` 会替换成拖动后的实际值；其他动作
保留声明参数（例如 `set_value` 使用目标的 0..100 数值轨道，0 表示其最小值）。
因此，`release` 中先 `set_value: 0` 再 `call` 可以重置控件，并通知应用本次拖动结束值；
回调内 getter 仍可能读到声明式动作提交前的状态。隐藏或禁用捕获中的控件会取消拖动，
恢复显示或启用后，需要抬起再按下。

取色可以使用三个 Slider 表示 H、S、V（建议范围 0..359、0..100、0..100）：`value`
更新预览，`release` 提交灯光或主题设置。颜色空间转换由应用完成；业务保存完整 HSV
值；控件将声明范围划分为 100 个间隔，选择色相范围时应考虑这一精度。

Wheel 的生成绑定默认开启行吸附；高级 List 可用 `esp_gsp_list_snap()` 显式开启。
`cyclic: true` 会在编译期拒绝；`snap_to_item: false` 可关闭吸附，生成头与自描述包行为一致。通过行回调处理选项选择，行吸附控制滚动停止位置。

## 动画与转场

通过应用动画 API 启动属性动画，将启动与生命周期管理放在同一模块，并检查返回值。
使用补间选项配置循环和往返播放。

使用 `esp_gsp_goto_scene()` 切换场景并选择转场，使用
`esp_gsp_set_swipe_transition()` 配置跟手效果。配置导航时，确认显示后端支持所选转场。

源码仓库使用源码构建，预编译组件包使用包内库文件。升级时配套更新编译器和组件包。

## 编译器选择

先激活 ESP-IDF 环境，再安装 `esp-gsp-tools`。CMake 使用该环境的 Python，
安装到另一个 Python 中不会让当前构建找到管理器。

显式 `GSPC_EXECUTABLE` CMake 参数优先于同名环境变量。没有版本标记的源码仓库
使用基于 Cargo 的当前编译器入口。组件包优先读取工程的 `.gspc_version`，否则
读取包内标记：先使用 PATH 中版本完全匹配的 `gspc`，再使用管理器的版本缓存或下载。
不匹配的 PATH 编译器会被提示并跳过；最终选中的编译器仍须通过格式和 ABI 校验。

离线构建可将配套编译器放入 PATH、设置 `GSPC_EXECUTABLE`，或提前联网填充管理器缓存。
失败时按提示中的 Python 安装命令或编译器路径处理。管理器版本与 GSPC 版本相互独立；
升级管理器不能提供尚未发布的编译器产物。

## Dropdown 选择与颜色动作

`gsp_settings_display_mode_set_selected(ui, index)` 使用零基索引，作用于当前场景。
场景前缀区分 C 符号，不会将命令路由到未激活页面；应在对应
`ESP_GSP_EVENT_SCENE_CHANGED` 后恢复保存的选择。setter 更新运行时状态和显示文字，
不改写 JSON 的初始 `selected`。用户点击的新索引读取回调的 `event->arg`；回调内
getter 可能仍读到本次输入提交前的值。ESP-IDF 下 setter 成功表示命令已被接受入队。

使用 `set_bg_color` 从场景事件更新颜色 Bind。例如，声明
`bind: "button_color"`、`bind_target: "color"` 后使用：

```json
"events": [
  {"event":"click", "action":"set_bg_color", "target":"button_color", "arg":"#FF3020"}
]
```

跨目标使用的颜色应写为 `#RRGGBB` 字符串；整数参数按目标原生像素格式解释。
该动作更新颜色，透明度单独配置。可点击控件自带自动按压反馈。

数值动作用整数 `arg` 传参，`value` 为兼容别名。例如，`add_value` 设置
`arg: -5`，会将数值 Bind 在 0..100 范围内减少五步。`call` 也接受有符号
32 位参数，回调中将 `event->arg` 转为 `int32_t` 读取。页面和场景索引使用
无符号 16 位整数。类型错误或超出范围会定位到对应事件字段；显式 `arg`
优先于 `value`。

## 生成入口、事件编号与容量报告

`gsp_add_bundle()` 默认使用 `SYMBOL bundle`，因此生成 `bundle_gsp.h` 和
`gsp_bundle_config()`。指定 `SYMBOL ui`（CLI 为 `--symbol ui`）后，入口是
`gsp_ui_config()`；CMake 头文件也变为 `ui_gsp.h`。生成头文件开头会列出实际入口。

普通回调使用 `gsp_<scene>_event_is_<callback>()` 或 `event_decode_call()`，
它们同时检查场景和动作编号。`action_id` 是场景内局部编号，不是跨场景唯一 ID。
确需访问 `GSP_<SCENE>_ACT_ID_*` 时，在包含 Bundle 头之前定义
`GSP_BUNDLE_ENABLE_RAW_IDS`，并同时判断 `scene_id`；默认不导出这些原始编号。

容量报告中的 `text slots outside lists` 包含普通场景文字和独立模板实例的可更新文字；
`total glyph-run slots` 还包含列表文字和内部余量。
模板容量按 `max_instances × 每实例文字槽数` 推导；JSON 之外的运行时对象由应用配置额外容量。静态文字不占运行时文字槽。

多场景链接共用一组 `(font, size)` 字体包。`--max-font-packs` 默认 32，与 ESP-GSP
字体容量一致；使用自定义运行时时可指定对应上限。共享字体超限按 Bundle 报告一次。
