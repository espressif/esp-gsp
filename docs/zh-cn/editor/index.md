# 可视化编辑器

ESP-GSP Studio 是面向 ESP-GSP 工程的独立可视化编辑器。它提供画布设计界面、实时
WASM 预览、属性检查、结构化编辑和 Agent 协作功能。

## 下载

- Windows x64 - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-win-x64.exe>
- Linux x64 AppImage - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-linux-x86_64.AppImage>
- Linux x64 DEB - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-linux-amd64.deb>
- 协作脚本包 - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-scripts.zip>
- SHA-256 校验清单 - <https://dl.espressif.com/AE/gsp/gsp-editor/SHA256SUMS-0.12.0.txt>

## 打开工程

使用 **打开工程目录** 打开任意 GSP 工程目录。打开时仅发现源文件和构建信息，
不会创建文件、复制资源或迁移源码。目录中可以包含 `.gsp-editor/` 下的编辑器
元数据，也可以没有。

场景 JSON 始终可直接由 GSPC 编译。资源路径相对于原始场景文件。编辑器元数据
完全位于 `.gsp-editor/` 中，删除后不影响原生源文件。

## 功能概览

- **画布编辑** — 在设计画布上拖放、调整大小和排列控件。支持选择、多选和对齐。
- **属性面板** — 在结构化面板中编辑控件属性。属性来源区分本地值、样式和主题。
- **实时预览** — 内置 WASM 模拟器在每次修改后渲染当前场景。
  使用 **仿真 / F5** 打开带动画播放的完整交互式模拟器。
- **场景管理** — 通过标签页创建、重命名、删除和重新排序场景。
  拖动标签页或使用 Alt+Left/Right 调整构建顺序。
- **撤销 / 重做** — 每个场景维护独立的撤销历史。
- **自动保存** — 视觉编辑在最后一次操作后 500 ms 自动保存；
  指针手势（拖动、调整大小）在手势结束时保存。
- **外部变更检测** — 文件系统变更自动协调。冲突时显示两个版本。
- **草稿恢复** — 未保存的草稿保存在 `.gsp-editor/drafts/` 下，
  下次启动时提供恢复。
- **组件编辑** — 就地编辑共享组件、复制创建独立变体、
  或通过编译器验证执行分离操作。
- **资源管理** — 导入图片、字体和视频，经编译器验证。
  替换图片保留现有布局尺寸。
- **导出** — 可以继续在 ESP-IDF 中使用、导出可重新构建的独立 UI 目录、
  或导出针对固件 Bundle 上下文的预编译资源。

## 随包工具链

ESP-GSP Studio 0.12.0 附带 GSP/模拟器 1.10.1 和 GSPC 0.11.1。应用版本号与随包
工具链版本号独立。内置编译器、Schema、WASM 运行时和导出功能均使用这套固定
工具链，当前版本不支持导入或切换工具链。

工程设置 → **随包工具链** 显示实际版本、平台和验证状态。原生工程兼容性使用
编译器版本和格式/ABI 契约。不受支持的原生需求会禁用内容编辑，但源码和恢复
功能仍可访问。

随包工具链使用配置 ABI 4，参见[兼容性契约](../reference/compatibility.md)。

## 构建上下文

对于已有 ESP-IDF 构建的工程，打开工程设置 → **编译与预览配置**，选择构建目录
和 Bundle。编辑器通过 `gspc doctor <project> --build <build-dir>` 发现上下文，
提供场景顺序、Profile、组件目录、编译器传递和容量限制。

没有构建上下文时，**手动预览配置** 允许显式设置预览和导出选项。该模式在工具栏中
标识，不声称能复现设备构建。

## 下一步

- [基本操作](operations.md) — 常用编辑工作流。
- [Agent 协作](collaboration.md) — 使用 CLI 或 MCP 将编辑交给 AI Agent。
