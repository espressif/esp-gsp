# ESP-GSP 中文文档

[文档语言入口](../README.md) | [English](../en/README.md) | **中文**

从入门指南开始构建应用；专题指南提供使用步骤，参考页用于查询字段与 API。

## 从这里开始

1. [了解 ESP-GSP](guide/introduction.md)
2. [安装并运行第一个场景](getting-started.md)
3. [推荐工程工作流](guide/workflow.md)
4. [场景结构与命名](guide/scenes.md)
5. [运行时与生成 API](guide/runtime-api.md)
6. [完整控件库](components/index.md)
7. [Agent 辅助 UI 开发](guide/agent-assisted-ui.md)

## 开发应用

| 任务 | 相关文档 |
|---|---|
| 判断内容应放在 JSON、应用代码还是 BSP | [工程工作流](guide/workflow.md) |
| 使用 Codex 创建或修改 UI | [Agent 辅助 UI 开发](guide/agent-assisted-ui.md) |
| 编写层级、样式、动作、模板与资源 | [场景 JSON](guide/scenes.md) |
| 使用命名对象、回调与生成式 C API | [运行时与生成 API](guide/runtime-api.md) |
| 使用运行时图片、Canvas 和应用数据集 | [媒体与应用数据](guide/media-and-data.md) |
| 从 SD/NAND 加载预处理图片与动画 | [外部资源](guide/external-assets.md) |
| 使用页面、堆栈、抽屉与滚动视口 | [导航与手势](guide/navigation.md) |
| 管理启动、异步更新、回调和停止 | [生命周期与线程](guide/lifecycle.md) |
| 理解公共结构、临时视图与资源所有权 | [应用结构与所有权](guide/application-structures.md) |
| 配置 RGB、MIPI-DSI、SPI、QSPI 显示链路 | [显示集成](guide/display.md) |
| 选择 JSON 自适应、Kconfig 与实例覆盖 | [统一配置模型](guide/configuration.md) |
| 查询配置结构、Bundle 参数和覆盖顺序 | [配置参考](reference/configuration.md) |
| 使用 `menuconfig` 与可复现默认配置 | [Kconfig 使用](reference/kconfig.md) |
| 查询每个 Kconfig 与只读库能力的作用 | [配置项参考](reference/configuration-options.md) |
| 排查编译、链接、运行、显示与输入问题 | [故障排查](guide/troubleshooting.md) |
| 查找公共 API 分层与所有权 | [API 参考](reference/api.md) |
| 查询每个公共函数的准确声明与契约 | [公共函数参考](reference/api-functions.md) |
| 核对组件、GSPC、格式和模拟器 ABI | [兼容性契约](reference/compatibility.md) |
| 使用模拟器预览、测试和自动化 | [模拟器预览与测试](guide/simulator-preview.md) |
| 查询模拟器 CLI、API 和后端参考 | [模拟器参考](reference/simulator.md) |

## 控件、示例与预览

[控件库](components/index.md)与当前 GSPC 注册表、字段定义和
`examples/usage/widgets/` 中的真实 JSON 保持一致。每个控件页包含：

- 适用场景与运行行为；
- 可直接复制的完整 JSON；
- 实际编译产物中提取的 C API；
- 示例使用字段、控件专用字段和公共字段；
- 使用发布版 ESP-GSP 模拟器的本地交互预览命令。

在控件页运行示例，预览布局、文字和交互效果。

## 参考

- [完整场景编写参考](reference/authoring.md)：GSPC 当前场景键、事件、动作和字段。
- [控件清单](reference/widget-inventory.md)：编译器注册表覆盖情况。
- [模拟器参考](reference/simulator.md)：浏览器预览、CLI、输入模式、自动化 API 与应用后端。
- [机器可读 JSON Schema](../en/reference/scene.schema.json)：编辑器与校验契约。

## 检查应用效果

构建后用模拟器检查布局与交互，再在开发板上检查显示方向、颜色、触控和动画。
详细步骤见[显示集成](guide/display.md)。

## 文档与组件版本

使用随已安装组件提供的文档和示例，以保持 API、场景字段与预编译工具一致。
工具版本选择和覆盖方式见[模拟器预览](guide/simulator-preview.md)及
[兼容性契约](reference/compatibility.md)。
