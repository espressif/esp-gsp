# 场景 JSON

## 从需求或参考图开始

动手写 JSON 之前，先确认以下信息：逻辑宽高、像素格式、主要交互方式、静态资源
清单、运行时数据来源，以及 BSP 的显示方向。如果有截图或设计图，可以作为视觉
参考：

1. 把可见元素拆分为容器、标签、控件、形状和可复用图片。截图只能提供静态外观，
   不能定义对象的状态和交互。
2. 如果参考图和屏幕分辨率不同，需要换算几何尺寸。
3. 记下缺失的字体、精确颜色和交互状态，这些需要后续确认。

截图无法定义按压/禁用状态、滚动、导航、加载或错误行为——这些需要在场景和应用中
另行声明。

## 最小场景与对象

每个场景必须声明一个稳定的 `screen` 名称、逻辑 `w`/`h`，以及一个
`objects` 数组；仅显示背景的场景可以使用空数组。`parent: -1` 表示挂到屏幕根节点。
层级较深时，优先使用 `parent_name` 而不是数字索引——名称不会因为数组重排而失效。

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

`w`/`h` 必须大于 0。
`default_font_size`（如声明）必须在 1–255。构建和 `gspc diagnose` 使用同一套字段
类型、枚举、范围和语义检查——输入错误会在资源生成前就报出来。文档中的矩形别名
保留旧版颜色字段；注册控件上出现未知字段会报错。

对象数组顺序同时影响绘制和命中层级：后面的对象显示并命中在前面对象之上。普通的
父引用可以指向数组中前方或后方的对象；隐藏状态继承和模板归属不依赖排列顺序。
编译器不会擅自重排绘制顺序。

`gspc diagnose scene.json --layout-check` 可列出静态根级 Button/Label/Toggle 矩形的
交叠候选与建议检查点。报告数量有上限，属于信息提示，允许有意的分层设计。
动态或带父级的几何关系仍需通过预览和真实输入检查。使用原生联调的
[坐标点击与回调断言](../../../tools/sim_bridge/README.md#repeatable-application-checks)，验证实际响应
和下层业务状态。

## 可复用的项目组件

Grid/换行和组件展开能力最初在 GSPC 0.7.0 / ESP-GSP 1.6.0 引入。当前工程应使用
[兼容性契约](../reference/compatibility.md) 中配套的编译器与运行时。
编译器内部的虚拟集合内容可能使用 GSB 2.20 的 `instance_order`
变体——相关场景应用配套工具链重新构建。

### 定义组件

在场景的 `components` 映射中，或通过 `--components-dir` 加载的 JSON 文件里，
可以定义参数化子树。每份定义包含一个 `objects` 数组，以及可选的 `params` 必填
参数名称数组。

**参数替换规则：**

- 完整的 `"$title"` 会被对应实参替换，保留 JSON 类型。
- 不以 `$` 开头的字符串也支持文字替换，例如 `"Reading: $value"`，非字符串参数
  会转换为 JSON 文本。
- 以 `$` 开头的字符串被视为一个完整参数引用，而不是带后缀的模板。
- 参数替换不计算算术表达式。

### 实例化组件

用 `use` 实例化一个组件定义。使用处可以覆盖根对象的 `name`、`x`、`y`，以及
`parent` 或 `parent_name` 中的一项（命名父节点可以在场景中后声明）。其他使用处
字段必须声明为参数，并在定义中显式引用。

定义内子对象的数字 `parent` 是局部对象索引，省略时默认挂到定义的根对象。
`parent_name` 引用场景范围的名称。重复实例中需要暴露的子对象，应通过名称参数等
方式获得各自唯一的名称。

定义可以包含普通控件和内置组合控件（TabView、Dropdown、Keyboard、MsgBox 等），
生成的控件复用直接声明时的事件和弹层机制。**定义内不允许嵌套 `use`**——可在场景
层组合实例或展开共享子树。场景级 `styles` 和根字体默认值仍然有效。素材路径相对
于场景文件解析，不相对于组件定义文件。

[组件模式示例](../../../examples/usage/component_patterns/README.md) 提供了设置
项、图标按钮和角标。这些是可编辑的项目组件，不是新的运行时控件类型。展开后的
对象仍占用命令、状态和资源容量，应与等价手写场景比较。

用 `gspc diagnose --components-dir <目录> <场景.json>` 检查展开及构建语义。
daemon 的 `level: "schema"` 仅检查源码结构，完整验证应使用 `level: "compile"`。

在工程构建中，向 `gsp_add_bundle()` 和 PC 适配工程的 `gsp_add_backend()` 传入
`COMPONENTS_DIR`，即可复用同一份组件定义。目录相对于调用处的 `CMakeLists.txt`：

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    COMPONENTS_DIR ../ui/components
    SCENES ../ui/scenes/home.json ../ui/scenes/settings.json)
```

目录顶层的 JSON 文件名即组件名称，例如 `Card.json` 定义 `Card`。修改、新增和删除
定义都会触发 Bundle 重建；生成的编译上下文会保留该目录，供诊断和预览复用。
共享目录是可选能力，适合工程时也可以继续使用场景内的定义。

编译会在参数替换后，按直接声明控件时相同的字段、类型、范围和动作规则检查组件。
非法字段会阻止生成新的资源和头文件。字段诊断通过 `file`、`path` 和 `range` 指向
实际组件定义、实例参数或共享样式；定义位置不一定具有展开后的 `objectIndex`。
包含标点的键在诊断路径中使用方括号表示。

## 外观

下面的卡片组合了圆角背景、外轮廓、硬阴影和纵向布局。内部两条色块是普通子矩形：

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

保存为 `styled_card.json` 后运行
`gspc pack styled_card.json --deployable -o styled_card.gspb` 即可预览。
下面的字段说明帮助你调整外观和布局。

### 背景透明度

内容控件和结构控件的 `bg_opacity`（0–255，默认 255）只影响背景。最终 alpha 为
`round(round(opacity * bg_opacity / 255) * color_alpha / 255)`。
`border_opacity` 独立调节边框透明度。`bg_opacity` 非 255 时要求 `opacity` 为
字面量。

### 外轮廓

支持的控件可以用 `outline_color`、`outline_width`、`outline_pad` 和
`outline_opacity` 绘制外部圆角轮廓。轮廓需要同时指定颜色和正宽度才会生效。

### 局部边框

`border_side` 支持 `all`（默认）、`none`、`top`、`bottom`、`left`、`right`、
`horizontal`、`vertical`。局部边框绘制在原始元素框内，要求静态 `w`、`h` 和
`radius: 0`，宽度、颜色和透明度共用。厚的对边会拆分绘制以避免同一像素混合两次。
`all` 保留圆角边框行为，`none` 只关闭边框。

### 阴影

`shadow_offset_x/y`、`shadow_spread`、`shadow_radius`、`shadow_color` 和
`shadow_opacity` 启用硬填充圆角阴影。默认黑色、透明度 96、偏移和 spread 为 0，
圆角沿用元素半径。`spread` 同时扩大绘制边界和阴影圆角。

装饰按元素矩形计算，不按图片 alpha 轮廓或子节点并集计算。装饰会扩大绘制和脏区，
但不改变布局尺寸和触摸区域。祖先裁剪仍然生效。每个启用的轮廓或阴影在优化前最多
增加一条图元；有效透明度为 0 时不输出。局部边框按选中边各增加一条、最多两条。
透明像素和更大的脏区都有绘制成本。

场景位置组和整个模板实例可以带着装饰移动。外轮廓和阴影要求静态 `w`/`h`/`radius`。
模板的局部边框成员要求 `x`/`y` 为字面量，边框颜色为字面量或静态主题。带轮廓或
阴影的模板成员也要求 `x`/`y` 和装饰颜色静态。

## 布局

`layout: "grid"` 排列的是显式声明的子对象——它与使用单元模板回收数据项的
[虚拟化 Grid 控件](../components/grid.md)（`type: "grid"`）是不同的能力。

`layout` 在构建期为直接子对象求值。省略 `layout_wrap` 或设为 false 时，`row`
和 `column` 保持原有行为。行列布局中，`margin_left`、`margin_right`、
`margin_top`、`margin_bottom` 可分别覆盖 `margin` 的对应方向。

### 行列布局对齐

- `align_main` 支持 `start`、`center`、`end`、`space_between`。
- `align_cross` 支持 `start`、`center`、`end`、`stretch`。

未换行的行列布局省略 `align_cross` 时，保留子对象字面量的交叉轴坐标。

### 自动换行

设置 `layout_wrap: true` 后，编译器按每个子对象包含外边距的有界字面量主轴尺寸划分
多行。`gap` 同时分隔同一行的子对象和相邻行。超过可用主轴空间的单个子对象仍保留为
一项。每一行独立应用对齐方式，换行行列布局的交叉轴默认是 `start`。每一行的
`grow` 从子对象的有界字面量尺寸开始分配剩余主轴空间，并受最大尺寸限制。

### 网格布局

`layout: "grid"` 要求 `grid_columns` 为 1–65535。网格使用等宽列轨道；内部宽度
不能整除时，余数分配给前面的列。子对象按声明顺序从左到右逐行放置；
`grid_column_span`（默认 1）可以占用 1 到 `grid_columns` 列，当前位置放不下时
移到下一行。

网格行高取该行子对象包含上下外边距的有界字面量高度最大值。网格子对象的宽度由
所占列轨道宽度减去左右外边距得到，再按该子对象的宽度边界限制。

- `align_main`（`start`、`center`、`end`）在单元格内定位受 `max_width` 限制的
  子对象。网格不支持 `space_between`。
- `align_cross`（`start`、`center`、`end`、`stretch`）在行内定位或拉伸子对象。
- 网格子对象不能使用 `grow`。

### 尺寸约束

`min_width`、`max_width`、`min_height`、`max_height`（0–65535 像素）约束静态
尺寸，也作用于行列布局的 grow、交叉轴 stretch 和网格单元尺寸。

- 最小值不能大于最大值，且同一维度不能同时声明有界动态值。
- `grow` 先保留最小尺寸，再按权重分配剩余空间；达到最大值的子控件把空间让给其他
  grow 子控件，全部达到最大值后的剩余空间按 `align_main` 放置。
- 最小尺寸总和放不下时保留最小值并溢出。
- 这些约束不引入运行时重排。

带有显式对齐或尺寸约束的布局，如果会覆盖子控件的有界动态位置或 grow 尺寸，会
拒绝该组合；省略这些字段时保留旧布局的宽松行为。网格和换行行列布局会拒绝直接
子对象的有界动态 `x`/`y`/`w`/`h`，因为这些几何由布局求解器负责。

嵌套布局会先求值父布局，再求值子布局，即使父对象在对象数组中后声明也是如此。
运行时设置不会重新排列兄弟对象。未使用新字段的未换行行列场景保持原有行为。
需要整体移动编译好的排列时，移动外层位置组。

### 复合控件与布局

展开后的复合控件作为 Grid/Wrap 的直接子对象时，如果保持生成根节点的字面量尺寸
不变，仍可由布局移动。如果布局需要改变展开根节点的实际 `w`/`h`，编译器会报错，
因为复合控件展开时内部几何已经确定。

**解决方法：** 把复合控件放在一个容器内，让容器作为 Grid/Wrap 的直接子对象。
容器可以接受布局分配的几何，而复合控件保持自己的内部几何。

复合控件生成的 overlay 和 placeholder 不参与 Grid/Wrap 放置，不会占用单元格。

### 完整示例

下面的场景包含三列网格、跨两列的子对象和一个自动换行的行布局。子对象省略了
`x`/`y`，因为位置由布局提供：

```json
{
  "screen": "grid_wrap",
  "w": 320,
  "h": 240,
  "screen_bg": "#101820",
  "objects": [
    {
      "type": "container",
      "name": "grid",
      "x": 16,
      "y": 16,
      "w": 288,
      "h": 104,
      "layout": "grid",
      "grid_columns": 3,
      "padding": 8,
      "gap": 8,
      "align_main": "center",
      "align_cross": "center",
      "bg_color": "#1B263B"
    },
    {
      "type": "rect",
      "parent_name": "grid",
      "w": 64,
      "h": 24,
      "max_width": 72,
      "bg_color": "#4CC9F0"
    },
    {
      "type": "rect",
      "parent_name": "grid",
      "w": 64,
      "h": 24,
      "max_width": 72,
      "bg_color": "#90BE6D"
    },
    {
      "type": "rect",
      "parent_name": "grid",
      "w": 128,
      "h": 24,
      "grid_column_span": 2,
      "max_width": 152,
      "bg_color": "#F9C74F"
    },
    {
      "type": "container",
      "name": "wrapped",
      "x": 16,
      "y": 136,
      "w": 288,
      "h": 88,
      "layout": "row",
      "layout_wrap": true,
      "padding": 8,
      "gap": 8,
      "align_main": "center",
      "bg_color": "#1B263B"
    },
    {
      "type": "rect",
      "parent_name": "wrapped",
      "w": 100,
      "h": 20,
      "bg_color": "#F94144"
    },
    {
      "type": "rect",
      "parent_name": "wrapped",
      "w": 90,
      "h": 24,
      "margin_right": 4,
      "bg_color": "#F3722C"
    },
    {
      "type": "rect",
      "parent_name": "wrapped",
      "w": 120,
      "h": 18,
      "bg_color": "#577590"
    }
  ]
}
```

## 文字

静态场景文字支持 `text_line_space`（行间额外像素，0–4096）和
`text_vertical_align`（`auto`、`top`、`center`、`bottom`）。垂直对齐作用于整个
可见文字块，包括空行。`auto` 保留单行居中和多行顶部对齐。这些属性在编译期应用，
不增加运行时文字排版的开销。

非默认的行间距或垂直对齐要求场景文字无文字绑定、`input` 关闭且 `w`/`h` 为字面量。
文字绑定、输入框、模板文字和有界动态 `w`/`h` 会拒绝这些非默认值。颜色和可见性
绑定仍可使用。

## 图片

`image` 控件的 `image_opacity`（0–255，默认 255）只影响图片像素，不改变控件
背景和边框。普通模板图片 blit 也支持它。

固定、非动画的栅格图片还可以将图片透明度与 fit、缩放、旋转组合——透明度在编译期
烘焙进资源，变换在运行时执行。前提条件：

- `opacity` 保持静态 255（默认值）；
- codec 未设置（省略 `codec`、`auto` 或 `default`）或为 `store`；
- `store_scale` 为 1。

仅设置旧的 `compress: true` 而未指定 codec 时，会选用无损容器，不能用于上述烘焙。
动态图片源、模板 `dynamic_image`、动画资源和 SVG 不使用透明度烘焙；不满足条件的
组合会产生编译错误。

烘焙时，编译器将 `image_opacity` 按 0–255 比例乘入源 alpha，生成无损资源变体并
以 `store` 输出，不套用 Profile 的自动压缩策略。alpha 平面和多个透明度变体可能
增加资源占用；相同编码结果会去重。运行时变换直接使用烘焙后的 alpha，不增加额外
透明度操作。

对于有效透明度小于 255 的位图源文件，字面量 `rotation: 0` 会规范化为不生成运行时
旋转属性，并产生警告。不透明位图源文件在零角度仍保留原有旋转属性；SVG 光栅导入的
字面量零角度保持静态。有界动态旋转即使默认值为 0 也仍是运行时属性。

## name、bind 与 callback

- **`name`** — 创建稳定的组件键和类型化的对象辅助函数。
- **`bind`** — 导出共享场景状态，供声明式动作或通用接口更新。
- **`callback`** — 创建动作 ID 和生成式事件判断函数。
- **`events`** — 把输入连接到声明式动作。

普通应用集成优先使用 `name` 和生成的对象函数。Raw ID 只适用于确实需要数据驱动的
高级场景。

## 字段决策速查

| 目的 | 字段 | 规则 |
|---|---|---|
| 类型与层级 | `type`、`parent` 或 `parent_name` | 两种 parent 选一种；优先用可读名称 |
| 几何 | `x`、`y`、`w`、`h` | 使用逻辑像素；父对象通常先出现 |
| 应用 API | `name` | 给应用需要控制的对象添加，会生成类型化函数 |
| 应用事件 | `callback` | 需要产品代码响应时添加；回调必须非阻塞 |
| 声明式事件 | `events` | 用于场景内显隐、数值和导航动作 |
| 外观 | 颜色、边框、半径、透明度 | 仅使用该控件注册的字段 |
| 布局 | `layout`、`layout_wrap`、`grid_columns` 等 | 行列控制主轴；网格分配等宽轨道；直接子对象几何由编译器负责 |
| 编译期复用 | `styles`、`themes`、`components` | 复用定义，不增加运行时实例 |
| 运行时复用 | `template`、`max_instances` | 创建或回收有界实例 |

## 选择正确的结构

| 需求 | 起点 |
|---|---|
| 固定内容 | Label、Image、Shape、Container |
| 数值与开关 | Progress、Slider、Arc、Toggle、Checkbox |
| 少量固定选项 | Dropdown、Wheel、TabView |
| 动态序列/图库/会话 | List / Grid / Message List 与模板 |
| 相机、视频像素 | Image 目标加 Canvas API |
| 导航 | PageFlow、StackView、Drawer、Layer 或场景 |

## 模板与动态容量

List/Grid 的重复行以及应用动态创建的组件树，应编写为模板。`max_instances` 表示
"同时存活"的最大数量，不是数据集的总记录数。GSPC 会把需求写入 GSPB，普通用户
不需要手工计算 Slot。

## 资源与文字

路径相对于 JSON 文件。静态文本会自动贡献字形；运行时文字需要声明 `font_charset`，
或使用动态字体。图片可选择 `raw`、`lossless`、`jpeg` 或面向目标能力的 `auto`。
构建后才由应用提供的内容应使用运行时图片 API。

## 验证与可接受字段

[控件页面](../components/index.md)直接展示 GSPC 注册表中的全部字段、类型、
必填/默认、范围、选项和动态属性。[中文完整编写参考](../reference/authoring.md)
和共享的 [JSON Schema](../../en/reference/scene.schema.json) 来自同一份编译器
契约。

**优先执行所属 ESP-IDF 工程的 `idf.py build`**，因为它会应用真实的 Bundle 选项、
Target Profile 和容量。构建后在模拟器中检查布局和交互，再在目标板上检查显示、
触控和动画效果。

## 动态属性范围

"字面量"是直接写在 JSON 中的数字或字符串；"有界动态值"是带默认值和上下界的声明，
允许应用在该范围内更新属性。两者的具体写法见下文。

字段表中的"可运行时更新"表示该字段可以生成运行时属性，具体由声明方式决定：

- **`name`** 提供稳定的对象标识，本身不会把所有静态字段变成动态字段。
- **有界几何和透明度字段**：普通标量字面量保持静态。需要暴露运行时属性时，使用
  `{"default":128,"min":0,"max":255,"property":"alpha"}`。`property` 可省略，
  默认使用字段名。
- **位图**：用动态 `x`/`y`、`rotation` 和 `scalable` 调整位置、旋转和缩放；
  `w`/`h`/`opacity`/`radius` 使用静态值。场景 SVG 还可使用动态 `w`/`h`。
  模板图片的这四个字段使用静态值。
- **动态 `x`/`y`** 通过变换组移动对象及其子树；**动态 `w`/`h`/`radius`/`opacity`**
  是对象自身绘制的效果，不会重新布局子节点，也不提供整棵子树的整体透明度。
- **Layer** 的动态 `w`/`h`/`radius`/`opacity` 作用于自身背景填充——使用时需声明
  背景颜色。
- 控件的 `value`、文字、颜色、可见性以及模板实例属性各自有声明和 API。

字段表分别说明场景对象和模板成员的动态能力。"自身填充"表示属性控制对象背景，文字、
图片和子节点保持各自外观。"图片"要求配置图片源，"SVG"适用于场景中的 SVG 图片。
标注 `runtime_style` 时，启用该选项后使用对应接口。

模板 helper 使用以下单位：有界宽、高、圆角和透明度 setter 使用 0..100，JSON
默认值按声明单位填写，由编译器换算。模板中的 `property` 应省略或使用字段名，
不能用它重命名槽位。模板槽位按成员名和字段名导出；场景中的 `property` 名称用于
对应的场景属性 API。

## 声明运行时控制

需要从 C 代码显隐一个普通可见对象时，声明
`bind: "eye_visible", bind_target: "visible"`。需要改文字时声明文字 Bind。数值
控件、有界动态属性和模板槽直接生成对应 API，无需额外声明 Bind。静态对象可以省略
绑定。

命名对象的 `hidden: true` 会保留初始隐藏状态和显隐 API，所需字形也会保留。
未命名、未被显隐动作引用且没有可见性绑定的静态隐藏子树会在编译期移除。模板内
静态隐藏成员不会被绘制。

对象的可见性同时门控整棵已声明子树的绘制与输入，button 等控件的子项以及绘制在
父对象矩形之外的子项也遵循此规则。子项自身的可见性会保留：显示父对象不会自动
显示仍处于隐藏状态的子项，显示子项也不能绕过隐藏祖先。需要保持显示但禁止交互时
使用 `enabled`。可见性门控在编译期写入 bundle，受影响的场景需用修复后的编译器
重新生成资源。

顶层 Bundle 头默认导出 `GSP_<SCENE>_OBJ_KEY_<NAME>`，例如
`GSP_HOME_OBJ_KEY_EYE`。这些稳定对象键无需打开 LEGACY 或 RAW_IDS 开关。JSON
改名后，旧符号会在应用编译时报错。优先使用生成的类型化 helper；确需通用组件 API
时使用上述键，不要手写 FNV 数值。不带场景前缀的旧名称仍需显式启用，以避免不同
页面重名。

## 按压事件与默认反馈

普通可点击控件已有默认按压遮罩，无需 pointer observer。JSON 的 `press`、
`release`、`cancel`、`long`、`click` 绑定 `action: "call"` 后，均以
`ESP_GSP_EVENT_CALL` 携带各自的 callback ID 交给应用。

**点击判定**覆盖整个接触过程：

- 位移达到点击容差、越过控件边缘余量，或父手势接管后，本次接触即被取消，返回也
  不会恢复。
- 轻微的边缘抖动允许返回——余量为点击容差的四分之一，最多 4 个场景像素。松手时
  仍须命中原控件。
- 指针注入与硬件触摸帧使用相同规则。

导航、提交等业务请绑定 `click`；`press` 在按下时立即触发。

**长按**：普通控件按住 500 ms 且未拖动时触发一次 `long`，松手不再触发 `click`。
离开实际命中区域会重新计时。取消会恢复按压外观并发送一次 `cancel`，不发送
`release`/`click`。应用可用 `cancel` 停止由 `press` 启动的动作。隐藏、禁用控件
或离开场景也会取消接触。按住时恢复控件不会重新触发按压。

Slider/Arc 的值变化和提交遵循工作流中的范围控件契约。**回调中不要阻塞。**

## 容器边界与裁剪

Container、Layer 用于分组并提供子对象的坐标原点。设置 `clip_children: true`
后，子对象的绘制与触摸区域会裁剪到容器的矩形边界。嵌套裁剪取交集，并随父容器
移动。裁剪视口使用固定 `w`/`h`；`radius` 设置背景和边框圆角；`overflow` 控制
文字溢出。

List、PageFlow、StackView、Drawer 提供各自的矩形视口。子对象视口采用矩形裁剪。
静态圆形或路径遮罩效果可预先合并进图片的透明通道。

声明了 `enabled` 的 PageFlow 或 Drawer 在禁用交互后，会在下一次输入或帧处理时
取消正在进行的拖动。恢复启用后可接收新接触；已取消的接触会持续被消费直到抬手。
禁用手势期间，应用仍可通过导航 API 控制组件。

## 公共 UI

把公共标题栏、导航栏或键盘放在同一场景的 PageFlow/StackView 页面根节点之外，
切换页面时即可保留这些控件及其状态。多个独立场景可通过组件定义复用相同 UI，
每个场景有各自的控件实例。

PageFlow、StackView、Drawer 只移动各自的后代。与移动视口重叠的独立内容仍保持
声明的位置与绘制顺序。运行时仅在能确认像素与独立绘制隔离时使用整块视口快照，
否则按实时变换绘制（List 的 `scroll_snapshot` 同样遵循此规则），可能增加渲染开销。快照不可用时，
超过一个页面跨度的程序跳转会直接落位。

不透明矩形 Drawer 可以保留固定背景快照，底层页面有渐变或独立变换组时也可使用。
父级裁剪若切入 Drawer 或 StackView 视口，会回退到实时渲染。PageFlow 可以保留
垂直于运动方向的裁剪；沿运动方向的裁剪同样需要实时渲染。运行时自动选择路径，
保持绘制顺序和输入行为，应用无需增加配置。

Drawer、PageFlow、StackView 使用快照期间，自动程序效果与媒体播放暂停。
显式更新若影响已捕获的像素，会结束快照并恢复实时渲染；无关的屏外属性更新可以
保留快照。快照结束后自动播放恢复。

需要应用自行绘制叠加内容时，注册 `esp_gsp_set_overlay_contributor()`。它支持
圆角矩形和应用持有的 A8 字形位图，切场景后会在活动场景上重新绘制。自定义叠加
内容的输入通过顶层 input interceptor 处理；布局、焦点和控件命中使用场景控件
提供的接口。

Keyboard 的页面和按键在构建时生成。活动页面负责显示和输入，资源包保存所有页面。
让多个页面共享一个键盘，可以同时减少 JSON 重复和编译资源占用。

## 动态几何与输入

动态宽高、位置同时更新绘制范围和命中范围。模板几何值槽保持原有 0–100 百分比语义。布局和 `clip_children` 保持声明范围。隐藏祖先会停放其行实例，嵌套移动记录旧位置及最终绘制范围。

请使用配套编译器和运行时重新生成资源。动态交互几何按需生成 GSB 2.21 的 `HIT_GEOMETRY` 必需小节，旧运行时明确拒绝；已有资源保持原有契约。
