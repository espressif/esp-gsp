# 场景 JSON

## 从需求或参考图开始

先锁定逻辑宽高、像素格式、主要交互、静态资源、运行时数据来源和 BSP 显示方向。
将截图或设计图作为视觉参考：

1. 将可见元素拆成容器、标签、控件、形状和可复用图片；截图资源不能提供这些对象的状态和交互。
2. 参考图与屏幕分辨率不同时换算几何尺寸。
3. 记录缺失字体、精确颜色和交互状态的假设。

截图资源不能定义按压／禁用状态、滚动、导航、加载或错误行为；这些行为需要在场景和应用中声明。

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

顶层必须包含 `screen`、`w`、`h` 和非空 `objects`，`w/h` 必须大于 0。
`default_font_size`（如果声明）必须在 1–255。build 与 `gspc diagnose` 使用同一套字段类型、
枚举、范围和语义检查，这些输入错误会在资源生成前报告。文档中的矩形别名保留旧版颜色
字段；注册控件的未知字段会报错。对象数组顺序同时影响绘制与命中
层级：后面的对象显示并命中在前面对象之上。普通父引用可指向数组前方或后方；
隐藏状态继承和模板归属不依赖父子排列顺序。编译器不会为此重排绘制顺序。

## 外观

下面的卡片组合了圆角背景、外轮廓、硬阴影和纵向布局；内部两条色块是普通子矩形：

```json
{
  "screen": "styled_card", "w": 240, "h": 160, "screen_bg": "#101827",
  "objects": [
    {"type":"container","name":"card","x":20,"y":20,"w":200,"h":104,
     "bg_color":"#24334A","radius":12,"outline_color":"#5AA9E6","outline_width":2,
     "shadow_color":"#000000","shadow_offset_y":6,"shadow_opacity":96,
     "layout":"column","padding":16,"gap":12,"align_cross":"stretch"},
    {"type":"rect","parent_name":"card","w":168,"h":16,"bg_color":"#5AA9E6"},
    {"type":"rect","parent_name":"card","w":168,"h":16,"bg_color":"#5AA9E6","bg_opacity":96}
  ]
}
```

保存为 `styled_card.json` 后运行 `gspc pack styled_card.json --deployable -o styled_card.gspb`。
再按下面的字段说明调整外观和布局。

内容控件和结构控件的 `bg_opacity`（0–255，默认 255）只影响背景。
最终 alpha 为 `round(round(opacity * bg_opacity / 255) * color_alpha / 255)`。
`border_opacity` 独立调节显式边框；非 255 的 `bg_opacity` 要求 `opacity` 为字面量。

支持这些字段的控件可用 `outline_color`、`outline_width`、`outline_pad` 和
`outline_opacity` 绘制外部圆角轮廓；轮廓需要颜色和正宽度。
内容控件和结构控件的 `border_side` 支持 `all`（默认）、`none`、`top`、`bottom`、
`left`、`right`、`horizontal`、`vertical`。局部边框绘制在原始元素框内，要求静态
`w`、`h` 和 `radius: 0`；宽度、颜色和透明度共用。厚的相对两边会拆分绘制，避免同一像素
重复混合。`all` 保留圆角边框行为，`none` 只关闭边框。

`shadow_offset_x/y`、`shadow_spread`、`shadow_radius`、`shadow_color` 和
`shadow_opacity` 会启用硬的填充圆角阴影。默认黑色、透明度 96、偏移和 spread 为 0，
圆角沿用元素半径；spread 同时扩大绘制边界和阴影圆角。装饰按元素矩形计算，不按图片
alpha 轮廓或子节点并集计算。装饰会扩大绘制和脏区，不改变布局尺寸和触摸区域；祖先裁剪
仍然生效。每个启用的轮廓或阴影在优化前最多增加一条图元；有效透明度为 0 时不输出。
局部边框按选中边各增加一条、最多两条；透明像素和更大的脏区都有绘制成本。

场景位置组和整个模板实例可以带着装饰移动。外轮廓和阴影要求静态 `w/h/radius`。
模板的局部边框成员要求 `x/y` 为字面量，
边框颜色为字面量或静态主题；带轮廓或阴影的模板成员也要求 `x/y` 和装饰颜色静态。

## 布局

行列布局中，`margin_left`、`margin_right`、`margin_top`、`margin_bottom` 分别覆盖
`margin` 的对应方向。`align_main` 支持 `start`、`center`、`end`、`space_between`；
`align_cross` 支持 `start`、`center`、`end`、`stretch`。省略 `align_cross` 时保留字面量的
交叉轴坐标。布局在构建期求值。

`min_width`、`max_width`、`min_height`、`max_height`（0–65535 像素）约束静态尺寸，
也作用于行列布局的 grow 和交叉轴 stretch。最小值不能大于最大值，且同一尺寸不能同时
声明有界动态值。grow 先保留最小尺寸，再按权重分配剩余空间；达到最大值的子控件会把
空间让给其他 grow 子控件，全部达到最大值后的剩余空间按 `align_main` 放置。最小尺寸总和
放不下时保留最小值并溢出。这些约束不引入运行时重排。带有显式对齐或尺寸约束的布局，
如果会覆盖子控件的有界动态位置或 grow 尺寸，会拒绝该组合；省略这些字段时保留旧布局的
宽松行为。需要整体移动编译好的排列时，移动外层位置组。

## 文字

静态场景文字支持 `text_line_space`（行间额外像素，0–4096）和
`text_vertical_align`（`auto`、`top`、`center`、`bottom`）。垂直对齐作用于整个可见文字块，
包括空行；`auto` 保留单行居中和多行顶部对齐。属性在编译期应用，不增加运行时文字排版工作。

非默认的行间距或垂直对齐要求场景文字无文字绑定、`input` 关闭且 `w/h` 为字面量。
文字绑定、输入框、模板文字和有界动态 `w/h` 会拒绝这些非默认值；颜色和可见性绑定仍可使用。

## 图片

`image` 控件的 `image_opacity`（0–255，默认 255）只影响图片像素，不改变控件背景和边框；
普通模板图片 blit 也支持它。固定、非动画栅格图片还可将图片透明度与 fit、缩放、旋转组合：
透明度在编译期烘焙进资源，变换在运行时执行。要求 `opacity` 保持静态 255（默认值），
codec 未设置（省略 `codec`、`auto` 或 `default`）或为 `store`，且 `store_scale` 为 1。
仅设置旧的 `compress: true`、未指定 codec 时，会选用无损容器，不能用于上述烘焙。
动态图片源、模板 `dynamic_image`、动画资源和 SVG 不使用透明度烘焙；需要烘焙
的组合会产生编译错误。

进行烘焙时，编译器将 `image_opacity` 按 0–255 比例乘入源 alpha，生成无损资源变体并以
`store` 输出，不套用 profile 的自动压缩策略。
alpha 平面和多个透明度变体可能增加资源占用；相同编码结果会去重。运行时变换直接使用烘焙后的
alpha，不增加额外透明度操作。

对于有效透明度小于 255 的栅格图片，字面量 `rotation: 0` 会规范化为不生成运行时旋转属性，
并产生警告。不透明图片在零角度仍保留原有旋转属性；有界动态旋转即使默认值为 0 也仍是运行时属性。

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

“字面量”是直接写在 JSON 中的数字或字符串；“有界动态值”是带默认值和上下界的
声明，允许应用在该范围内更新属性。两者的具体写法见下文。

字段表中的“可运行时更新”表示该字段可以生成运行时属性，具体由声明方式决定：

- `name` 提供稳定对象标识，本身不会把所有静态字段变成动态字段。
- 对于有界几何和透明度字段，普通标量字面量保持静态。需要暴露运行时属性时，使用 `{"default":128,"min":0,"max":255,"property":"alpha"}`；`property` 可省略，默认使用字段名。
- 位图使用动态 `x`/`y`、`rotation` 和 `scalable` 调整位置、旋转和缩放；`w`/`h`/`opacity`/`radius` 使用静态值。场景 SVG 还可使用动态 `w`/`h`，模板图片的这四个字段使用静态值。
- 动态 `x`/`y` 通过变换组移动对象及其子树；动态 `w`/`h`/`radius`/`opacity` 是对象自身绘制的效果，不会重新布局子节点，也不提供整棵子树的整体透明度。
- Layer 的动态 `w`/`h`/`radius`/`opacity` 作用于自身背景填充，使用时需声明背景颜色。
- 控件的 `value`、文字、颜色、可见性以及模板实例属性各自有声明和 API。

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
