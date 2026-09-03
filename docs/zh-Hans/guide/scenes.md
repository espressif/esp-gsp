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
层级：后面的对象显示并命中在前面对象之上。

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

路径相对于 JSON 文件。静态文本会自动贡献字形；运行时文字需要声明 `font_charset`，或采用经过评审的动态字体路径。图片可选择 `raw`、`lossless`、`jpeg` 或面向目标能力的 `auto`；构建后才由应用提供的内容应使用运行时图片 API。

## 验证与可接受字段

[控件页面](../components/index.md)直接展示 GSPC 注册表中的全部字段、类型、必填/默认、范围、选项和动态属性。[中文完整编写参考](../reference/authoring.md)和共享的
[JSON Schema](../../en/reference/scene.schema.json)来自同一份编译器契约。

优先执行所属 ESP-IDF 工程的 `idf.py build`，因为它会应用真实 Bundle 选项、Target
Profile 和容量。构建成功证明 Schema、资源和生成接口成立；主机预览、开发板运行与
人工视觉验收仍是相互独立的结果。
