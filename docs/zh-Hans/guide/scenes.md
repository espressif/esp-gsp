# 场景 JSON

## 从需求或参考图开始

先锁定逻辑宽高、像素格式、主要交互、静态资源、运行时数据来源和 BSP 显示方向。
参考图只能描述视觉目标；对象层级、动态属性、所有权和容量仍需按 GSP 契约明确表达。

## 最小场景与对象

每个场景声明稳定的 `screen`、逻辑 `w`、`h` 和 `objects` 数组。`parent: -1` 表示屏幕根节点；层级需要保持可读时，优先使用 `parent_name`，避免依赖容易随排序变化的数字索引。

```json
{
  "screen": "settings",
  "w": 480,
  "h": 320,
  "objects": [
    {"type":"container","parent":-1,"name":"panel","x":20,"y":20,"w":440,"h":280},
    {"type":"toggle","parent_name":"panel","name":"wifi","x":24,"y":24,"w":64,"h":32,"callback":"wifi_changed"}
  ]
}
```

顶层必须包含 `screen`、`w`、`h` 和非空 `objects`。对象数组顺序同时影响绘制与命中
层级：后面的对象显示并命中在前面对象之上。普通父引用可指向数组前方或后方；
隐藏状态继承和模板归属不依赖父子排列顺序。编译器不会为此重排绘制顺序。

## name、bind 与 callback

- `name` 创建稳定组件键和类型化对象辅助函数。
- `bind` 导出共享场景状态，供声明式动作或通用接口更新。
- `callback` 创建动作 ID 和生成式事件判断函数。
- `events` 把输入连接到声明式动作。

普通应用集成优先使用 `name` 和生成对象函数；Raw ID 只适用于确实需要数据驱动的高级场景。

## 字段决策

| 目的 | 字段 | 规则 |
|---|---|---|
| 类型与层级 | `type`、`parent` 或 `parent_name` | 两种 Parent 选一种；优先可读名称 |
| 几何 | `x`、`y`、`w`、`h` | 使用逻辑像素；父对象通常先出现 |
| 应用 API | `name` | 给应用控制的对象添加，生成类型化函数 |
| 应用事件 | `callback` | 需要产品代码响应时添加，回调必须非阻塞 |
| 声明式事件 | `events` | 用于场景内显隐、数值和导航动作 |
| 外观 | 颜色、边框、半径、透明度 | 仅使用该控件注册的字段 |
| 布局 | `layout`、`padding`、`gap`、`margin`、`grow` | 布局控制主轴，避免与手工坐标冲突 |
| 编译期复用 | `styles`、`themes`、`components` | 复用定义，不增加运行时实例 |
| 运行时复用 | `template`、`max_instances` | 创建或回收有界实例 |

## 选择正确结构

| 需求 | 起点 |
|---|---|
| 固定内容 | Label、Image、Shape、Container |
| 数值与开关 | Progress、Slider、Arc、Toggle、Checkbox |
| 少量固定选项 | Dropdown、Wheel、TabView |
| 动态序列/图库/会话 | List / Grid / Message List 与模板 |
| 相机、视频像素 | Image 目标加 Canvas API |
| 导航 | PageFlow、StackView、Drawer、Layer 或场景 |

## 模板与动态容量

List/Grid 的重复行，以及应用动态创建的组件树，应编写为模板。`max_instances` 表示“同时存活”的最大数量，不是数据集总记录数。GSPC 会把需求写入 GSPB，普通用户不需要手工计算 Slot。

## 资源与文字

路径相对于 JSON 文件。静态文本会自动贡献字形；运行时文字需要声明 `font_charset`，或使用动态字体。图片可选择 `raw`、`lossless`、`jpeg` 或面向目标能力的 `auto`；构建后才由应用提供的内容应使用运行时图片 API。

## 验证与可接受字段

[控件页面](../components/index.md)直接展示 GSPC 注册表中的全部字段、类型、必填/默认、范围、选项和动态属性。[中文完整编写参考](../reference/authoring.md)和共享的
[JSON Schema](../../en/reference/scene.schema.json)来自同一份编译器契约。

优先执行所属 ESP-IDF 工程的 `idf.py build`，因为它会应用真实 Bundle 选项、Target
Profile 和容量。构建后在模拟器中检查布局和交互，再在目标板上检查显示、触控和动画效果。


## 动态属性范围

字段表中的“可运行时更新”表示该字段可以生成运行时属性，具体由声明方式决定：

- `name` 提供稳定对象标识，本身不会把所有静态字段变成动态字段。
- 对于有界几何和透明度字段，普通标量字面量保持静态。需要暴露运行时属性时，使用 `{"default":128,"min":0,"max":255,"property":"alpha"}`；`property` 可省略，默认使用字段名。
- 位图使用动态 `x`/`y`、`rotation` 和 `scalable` 调整位置、旋转和缩放；`w`/`h`/`opacity`/`radius` 使用静态值。场景 SVG 还可使用动态 `w`/`h`，模板图片的这四个字段使用静态值。
- 动态 `x`/`y` 通过变换组移动对象及其子树；动态 `w`/`h`/`radius`/`opacity` 是对象自身绘制的效果，不会重新布局子节点，也不提供整棵子树的整体透明度。
- Layer 的动态 `w`/`h`/`radius`/`opacity` 作用于自身背景填充，使用时需声明背景颜色。
- 控件的 `value`、文字、颜色、可见性，以及模板实例属性有各自的声明与 API，不能仅凭字段表中的“是”互相类推。查看对应控件示例和生成头文件。


字段表分别说明场景对象和模板成员的动态能力。“自身填充”表示属性控制对象背景，
文字、图片和子节点保持各自外观；“图片”要求配置图片源，“SVG”适用于场景中的 SVG
图片。字段表标注 `runtime_style` 时，启用该选项后使用对应接口。

模板 helper 使用以下单位：有界宽、高、圆角和透明度 setter 使用 0..100，
JSON 默认值则按声明单位填写，由编译器换算。模板中的 `property` 应省略或使用字段名，
不能用它重命名槽位。模板槽位按成员名和字段名导出；
场景中的 `property` 名称用于对应的场景属性 API。

## 声明运行时控制

需要从 C 显隐普通可见对象时，声明
`bind: "eye_visible", bind_target: "visible"`；需要改文字时声明文字 Bind。数值控件、
有界动态属性和模板槽直接生成对应 API，无需额外声明 Bind。静态对象可以省略绑定。

命名对象的 `hidden: true` 会保留初始隐藏状态和显隐 API，所需字形也会保留；未命名、
未被显隐动作引用且没有可见性绑定的静态隐藏子树会在编译期移除。模板内静态隐藏成员
不会被绘制。

顶层 Bundle 头默认导出 `GSP_<SCENE>_OBJ_KEY_<NAME>`，例如 `GSP_HOME_OBJ_KEY_EYE`。
这些稳定对象键无需打开 LEGACY 或 RAW_IDS 开关；JSON 改名后，旧符号会在应用编译时
报错。优先使用生成的类型化 helper；确需通用组件 API 时使用上述键，不要手写 FNV 数值。
不带场景前缀的旧名称仍需显式启用，避免不同页面重名。

## 按压事件与默认反馈

普通可点击控件已有默认按压遮罩，无需 pointer observer。JSON 的 `press`、`release`、
`long`、`click` 绑定 `action: "call"` 后，均以 `ESP_GSP_EVENT_CALL` 携带各自 callback ID
交给应用。

普通控件按住 500 ms 且未拖动时，`long` 触发一次，释放后不再触发 `click`；取消或
滚动接管不触发长按。使用不同 callback 名称区分各阶段，回调中不要阻塞。
取消输入不投递 `release`；需要自动恢复按压外观时使用控件内建反馈。
按住期间隐藏或禁用控件会取消本次交互。手指尚未松开时，即使恢复控件，
也不会产生新的按压；松手后可再次操作。
Slider/Arc 的值变化和释放提交遵循工作流中的范围控件契约。

## 容器边界与裁剪

Container、Layer 用于分组并提供子对象的坐标原点。设置 `clip_children: true` 后，
子对象的绘制与触摸区域会裁剪到容器矩形边界。嵌套裁剪取交集，并随父容器移动。
裁剪视口使用固定 `w/h`；`radius` 设置背景和边框圆角，`overflow` 控制文字溢出。

List、PageFlow、StackView、Drawer 提供各自的矩形视口。子对象视口采用矩形裁剪；
静态圆形或路径遮罩效果可预先合并进图片的透明通道。

## 公共 UI

将公共标题栏、导航栏或键盘放在同一场景的 PageFlow/StackView 页面根节点之外，
切换页面时即可保留这些控件及其状态。多个独立场景可通过组件定义复用相同 UI；
每个场景有各自的控件实例。

需要应用自行绘制叠加内容时，注册 `esp_gsp_set_overlay_contributor()`。它支持圆角矩形
和应用持有的 A8 字形位图，切场景后会在活动场景上重新绘制。自定义叠加内容的输入
通过顶层 input interceptor 处理；布局、焦点和控件命中使用场景控件提供的接口。

Keyboard 的页面和按键在构建时生成。活动页面负责显示和输入，资源包保存所有页面。
让多个页面共享一个键盘，可以同时减少 JSON 重复和编译资源占用。
