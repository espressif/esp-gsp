# 控件库

本页完整列出 GSPC 当前注册的控件。每页提供真实示例、准确字段与 C API，以及发布版模拟器预览命令。

## 数值控件

| 控件 | JSON 类型 | 用途 | 示例 |
|---|---|---|---|
| [圆弧](./arc.md) | `arc` | 可显示数值并支持拖动范围的圆弧组件。 | [`arc.json`](../../../examples/usage/widgets/arc/arc.json) |
| [复选框](./checkbox.md) | `checkbox` | 带方形视觉标记的独立勾选状态控件。 | [`checkbox.json`](../../../examples/usage/widgets/checkbox/checkbox.json) |
| [进度条](./progress.md) | `progress` | 只读显示区间数值的水平或垂直进度组件。 | [`progress.json`](../../../examples/usage/widgets/progress/progress.json) |
| [单选框](./radio.md) | `radio` | 常由应用状态协调的一组圆形布尔选项。 | [`radio.json`](../../../examples/usage/widgets/radio/radio.json) |
| [滑块](./slider.md) | `slider` | 可拖动的水平或垂直区间数值控件。 | [`slider.json`](../../../examples/usage/widgets/slider/slider.json) |
| [开关](./toggle.md) | `toggle` | 带可拖动滑块的开关状态控件。 | [`toggle.json`](../../../examples/usage/widgets/toggle/toggle.json) |

## 基础

| 控件 | JSON 类型 | 用途 | 示例 |
|---|---|---|---|
| [按钮](./button.md) | `button` | 可触发应用回调或声明式动作的点击内容区域。 | [`button.json`](../../../examples/usage/widgets/button/button.json) |
| [图片](./image.md) | `image` | 支持 contain、cover 和 stretch 的静态或动态图片。 | [`image.json`](../../../examples/usage/widgets/image/image.json) |
| [标签](./label.md) | `label` | 支持对齐、溢出和运行时绑定的 UTF-8 文本。 | [`label.json`](../../../examples/usage/widgets/label/label.json) |

## 数据与选择

| 控件 | JSON 类型 | 用途 | 示例 |
|---|---|---|---|
| [立体图标轮盘](./carousel.md) | `carousel` | 使用编译期缩略图、前后排序和吸附选择的图标轮盘。 | [`carousel.json`](../../../examples/usage/widgets/carousel/carousel.json) |
| [下拉选择](./dropdown.md) | `dropdown` | 展开选项面板的紧凑型单选控件。 | [`dropdown.json`](../../../examples/usage/widgets/dropdown/dropdown.json) |
| [网格](./grid.md) | `grid` | 由可复用单元模板驱动的虚拟化多列集合。 | [`grid.json`](../../../examples/usage/widgets/grid/grid.json) |
| [列表](./list.md) | `list` | 支持固定数据或应用数据源的纵向滚动列表。 | [`list.json`](../../../examples/usage/widgets/list/list.json) |
| [消息列表](./message_list.md) | `message_list` | 带收发气泡样式的虚拟化会话视口。 | [`message_list.json`](../../../examples/usage/widgets/message_list/message_list.json) |
| [滚轮选择器](./wheel.md) | `wheel` | 支持行吸附与首尾连续循环的虚拟化选择滚轮。 | [`wheel.json`](../../../examples/usage/widgets/wheel/wheel.json) |

## 可视化

| 控件 | JSON 类型 | 用途 | 示例 |
|---|---|---|---|
| [充电球](./charging_orb.md) | `charging_orb` | 提供水波、柔光光环和彩色光环，可配置充电粒子和膜状波纹。 | [`charging_orb.json`](../../../examples/usage/widgets/charging_orb/charging_orb.json) |
| [图表](./chart.md) | `chart` | 支持固定容量多曲线、批量更新与追加采样的折线图。 | [`chart.json`](../../../examples/usage/widgets/chart/chart.json) |
| [时钟](./clock.md) | `clock` | 指针与刻度可独立配置的模拟时钟。 | [`clock.json`](../../../examples/usage/widgets/clock/clock.json) |
| [参数特效](./effect.md) | `effect` | 支持自动播放的呼吸、光环、水波进度和扫光。 | [`effect.json`](../../../examples/usage/widgets/effect/effect.json) |
| [翻转卡片](./flip_card.md) | `flip_card` | 两张静态图片之间的透视翻转，支持中断和反向切换。 | [`flip_card.json`](../../../examples/usage/widgets/flip_card/flip_card.json) |
| [毛玻璃](./glass.md) | `glass` | 构建期生成静态背景的磨砂裁剪，运行时使用原生图片绘制。 | [`glass.json`](../../../examples/usage/widgets/glass/glass.json) |
| [指针](./needle.md) | `needle` | 枢轴、角度和尾长可配置的旋转仪表指针。 | [`needle.json`](../../../examples/usage/widgets/needle/needle.json) |
| [矩形](./rect.md) | `rect` | 支持填充、边框、圆角和透明度的轻量矩形图元。 | [`rect.json`](../../../examples/usage/widgets/rect/rect.json) |
| [形状](./shape.md) | `shape` | 无需图片资源即可绘制矩形、圆角、圆形或线条。 | [`shape.json`](../../../examples/usage/widgets/shape/shape.json) |
| [加载指示器](./spinner.md) | `spinner` | 用于等待状态的非确定性动画指示器。 | [`spinner.json`](../../../examples/usage/widgets/spinner/spinner.json) |

## 布局

| 控件 | JSON 类型 | 用途 | 示例 |
|---|---|---|---|
| [容器](./container.md) | `container` | 支持分组、可选矩形子树裁剪和自动布局的父容器。 | [`container.json`](../../../examples/usage/widgets/container/container.json) |
| [图层](./layer.md) | `layer` | 用于覆盖层和可见性分组的全屏或局部图层。 | [`layer.json`](../../../examples/usage/widgets/layer/layer.json) |

## 导航

| 控件 | JSON 类型 | 用途 | 示例 |
|---|---|---|---|
| [抽屉](./drawer.md) | `drawer` | 从屏幕边缘打开或关闭的手势抽屉。 | [`drawer.json`](../../../examples/usage/widgets/drawer/drawer.json) |
| [页面流](./page_flow.md) | `page_flow` | 支持横向或纵向滑动的多页面视口。 | [`page_flow.json`](../../../examples/usage/widgets/page_flow/page_flow.json) |
| [堆栈视图](./stackview.md) | `stackview` | 通过动画切换实现页面入栈和出栈的导航容器。 | [`stackview.json`](../../../examples/usage/widgets/stackview/stackview.json) |
| [选项卡视图](./tabview.md) | `tabview` | 包含标签栏和可滑动内容页的组合组件。 | [`tabview.json`](../../../examples/usage/widgets/tabview/tabview.json) |

## 组合组件

| 控件 | JSON 类型 | 用途 | 示例 |
|---|---|---|---|
| [键盘](./keyboard.md) | `keyboard` | 由原生基础控件组成的编译期屏幕键盘。 | [`keyboard.json`](../../../examples/usage/widgets/keyboard/keyboard.json) |
| [消息框](./msgbox.md) | `msgbox` | 带标题、正文和操作按钮的编译期模态对话框。 | [`msgbox.json`](../../../examples/usage/widgets/msgbox/msgbox.json) |
| [表格](./table.md) | `table` | 由标签、单元格和网格线组成的编译期表格。 | [`table.json`](../../../examples/usage/widgets/table/table.json) |
