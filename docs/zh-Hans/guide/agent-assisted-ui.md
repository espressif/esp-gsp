# Agent 辅助 UI 开发

ESP-GSP 提供一份可安装的编码 Agent Skill，用于把产品需求、参考图或现有设计转换为
经过验证的场景 JSON 和生成式 C API 集成。Skill 使用应用实际选中的 GSPC 与
ESP-GSP 版本，并读取对应的控件 Schema、API 参考和示例。

## 安装 Skill

权威 Skill 位于 [ESP-GSP 仓库](https://github.com/espressif/esp-gsp)的
`skills/esp-gsp-ui`。应从工程实际使用的 ESP-GSP 标签或提交安装同版本 Skill。
Codex 可以通过 `$skill-installer` 安装该路径。

```text
使用 $skill-installer，从 espressif/esp-gsp 仓库中本工程使用的 ref 安装
skills/esp-gsp-ui。
```

如果已经取得源码仓库，也可以复制或链接到当前编码 Agent 的 Skill 目录。
例如 Codex：

```sh
mkdir -p ~/.codex/skills
cp -R skills/esp-gsp-ui ~/.codex/skills/
```

下一轮对话通过 `$esp-gsp-ui` 调用。Skill 属于 GitHub 开发体验，不进入
ESP-IDF 组件压缩包。使用托管组件的工程仍然包含中英文文档、控件示例与生成参考，
Skill 会读取这些与组件版本匹配的内容。未安装 Skill 时，也可以直接按本指南运行
`gspc doctor`、`gspc cards` 和 `gspc diagnose`。

## 提供有效需求

提供工程路径，描述希望实现的页面行为即可。Skill 会从现有工程读取芯片、显示、
Bundle 和工具配置。新工程可在需求中补充相关的显示尺寸或方向；已有工程配置会优先
用于确定这些信息，工程中没有表达且会影响产品行为的选择可以直接写在需求里。

页面状态、动态数据、导航、可用资源和需要检查的交互，都有助于明确需求。
无需事先了解生成 API 名称或资源 slot 规则。

创建页面时可以这样描述：

```text
在这个 ESP-IDF 工程中使用 $esp-gsp-ui。为 480x320 RGB888 显示创建
scenes/settings.json，包含亮度 Slider、Wi-Fi Toggle、语言 Dropdown 和返回动作。
接入生成 API，构建工程，并在模拟器中预览页面。
```

使用参考图时，应说明它是视觉参考，还是必须随产品发布的真实资源。静态图不能说明
按下、禁用、滚动、加载、错误和导航行为；这些状态有要求时应在需求中明确。

修改现有 UI 时，应描述具体行为，而不是要求无差别重写：

```text
使用 $esp-gsp-ui 为 scenes/setup.json 增加应用数据驱动的 Wheel。
选项由应用提供，加载后可能变化；尽量保留当前选中项，支持空列表，
并预览滚动和选择行为。
```

排查问题时，描述操作步骤和预期结果即可：

```text
使用 $esp-gsp-ui 排查：点击 Enter 切换到时钟页后，同一位置的卡片也被触发。
请在模拟器中复现、定位并修复，让一次触摸只触发起始页面。
```

## Skill 会执行什么

Skill 遵循与[推荐工程工作流](workflow.md)相同的职责模型：

1. 识别实际选中的组件、Target、GSPC、Bundle、场景和 BSP；
2. 读取当前 Schema、相关控件页和经过检查的控件示例；
3. 布局和声明式行为放在 JSON，产品状态放在应用，面板行为放在 BSP；
4. 用 `gspc cards` 查看目标控件字段，对真实场景执行 `gspc compatibility` 和 `gspc diagnose`（诊断含 `suggestions`）。CLI 诊断包含编译检查；daemon 的 `diagnostics/pull` 默认只检查 Schema，`doctor/get` 声明支持后可使用 `level: "compile"` 检查资源和 Profile；
5. 生成 Bundle 头文件和 `*.api.json`，接入其中的 API，并构建应用修改；
6. 需要预览时查找或构建模拟器，加载同一次编译生成的 `--api-json`，按名验证输入、状态和编译初始布局；连接应用 Backend 时，状态写入与导航仍由 Backend 负责；
7. 汇总修改内容、构建结果和预览情况。

已有 ESP-IDF 构建时，先运行 `gspc doctor <project> --build <build-dir>`。
使用其中报告的 Target、组件和配置契约；如果找不到构建描述，应保持为未知，
不能用 host 默认配置代替。

[场景 JSON](scenes.md)解释编写结构，
[运行时与生成 API](runtime-api.md)解释应用集成，
[控件库](../components/index.md)覆盖当前支持的全部控件。
卡片中的 `example` 是编写片段，不是完整场景；使用 `example_path` 和文档路径读取完整示例与字段参考。
PageFlow/Drawer 自动化前先从 `capabilities` 确认组件运动支持，再使用
`component_get_motion`、`wait_component(name, 可选 value, max_frames)` 或
`component_event`；成功等待必须满足 idle，指定 `value` 时还需匹配目标值，超时返回 JSON-RPC 错误。
编译得到的 bounds 只是初始布局元数据，不能当作运行时命中证明。

## 选择预览路径

只检查场景布局、声明式动作和控件交互时，使用[模拟器预览与测试](simulator-preview.md)
中的独立模拟器流程。它适合快速查看场景，但不会运行应用自己的 C 任务和产品状态逻辑。

> **仅适用于编码 Agent：** 常规自动化测试优先使用 headless 模式。如果用户希望打开
> 模拟器并亲自操作，应另开一个模拟器，并通过 `--input-mode browser-exclusive` 将控制权
> 交给浏览器。除非用户明确要求可视化观察 Agent 的测试或调试过程，否则不要暴露 Agent
> 自动化/调试实例的 WebUI；该页面看似可交互，却可能因自动化独占输入而拒绝用户操作。

如果验收依赖应用 C 回调、定时器、动态集合或实时状态，优先使用组件自带的
`sim_bridge`。它能在 PC 上运行可移植的应用 C 代码，并把界面交给模拟器渲染。
ESP-GSP 组件已安装到工程后（例如执行 `idf.py reconfigure`），已有 PC 工程可从应用根目录运行：

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py --project pc
```

可以先用组件中的 `examples/usage/hello_world/pc` 或
`examples/usage/sim_bridge_media/pc` 熟悉流程；将示例路径作为 `--project` 参数即可。
源码 checkout 可直接使用当前组件的 `tools/sim_bridge/run.py`。完整要求、工具版本选择、
PC HAL/mock 以及支持的 API 子集见[sim_bridge 使用说明](../../../tools/sim_bridge/README.md)。

sim_bridge 用于验证应用逻辑，不等同于 ESP-IDF 任务调度、外设、面板时序或设备性能验证。
它运行的 API 子集和单线程 Backend 契约也应纳入检查。仍需构建目标工程，并按需要上板验收。

## 检查生成结果

检查源 JSON 和应用代码变更，重新构建以更新生成头文件和 Bundle。确认对象命名、
回调响应、资源所有权和同时活跃对象的容量。先在模拟器中预览布局与交互，再按
[显示集成](display.md)检查开发板上的运行效果。

源码 checkout 需要执行发布前 agent 门禁时，可运行
`python3 tools/sim_host/tests/agent_loop_smoke.py --host <sim-host> --gspc <gspc>`。
它会验证一次确定性的“诊断—修复”路径，再把同一次编译生成的 Bundle/API sidecar
交给模拟器检查；它不能替代 ESP-IDF 工程构建和开发板验收。

Skill 读取所选组件的 Schema、头文件和示例，并按提问语言选择说明与文档。
