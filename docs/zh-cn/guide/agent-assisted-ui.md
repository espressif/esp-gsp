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

从参考设计开始时，提供独立参考图、原始素材、字体文件、逻辑尺寸，以及需要实现的
状态和动作。普通场景编写不依赖设计工具插件。交互控件和动态文字保持原生对象，
静态装饰使用独立图片；用实际输入和应用状态验证功能，并与独立设计对照画面。
运行时文字变化不会自动推动兄弟对象重排。

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
2. 按需读取目标控件卡片、经过检查的示例和相关文档；尚未确定控件类型时，使用 `gspc cards --list` 查找；
3. 布局和声明式行为放在 JSON，产品状态放在应用，面板行为放在 BSP；
4. 通过目标配置下的 compile/preview 或所属构建验证修改，使用其诊断和 `suggestions`；只检查而不生成产物时使用 `gspc diagnose`。daemon 的 `diagnostics/pull` 默认只检查 Schema，`doctor/get` 声明支持后可使用 `level: "compile"` 检查资源和 Profile；
5. 复用同次编译生成的 Bundle 头文件和 `*.api.json`，接入其中的 API，并构建应用修改；
6. 界面或交互有改动时，通过模拟器检查，按需查找或构建工具，加载同一次编译生成的 `--api-json`；连接应用 Backend 时，状态写入与导航仍由 Backend 负责；
7. 汇总修改内容、构建结果和预览情况。

对于交叠控件、弹层和几何变化，验证应包含交集、暴露边缘、弹窗空白区及裁剪边界的
坐标点击。原生 runner 的 `tap` 支持完整回调序列断言，包括 `callbacks: []`；同时
核对应用属性与当前场景。显隐、关闭和移动后重复检查，并查看截图中的视觉遮挡。见
[原生场景测试](../../../tools/sim_bridge/README.md#repeatable-application-checks)。

关注 compile 级 `GSPC-TEXT-CLIPPED` 信息，检查初始静态文字是否被自身文本框裁切；
有意裁切仍然有效。Drawer/PageFlow 动画使用 runner 的 `wait_component` 核对目标值
和静止状态。局部修改有已确认正确的 PNG 时，可选用截图的 `reference` 和
`allow_changes` 检查修改范围之外的变化；比较时固定 Profile 和应用状态。

按需求选择验收路径，覆盖相关输入边界、确认与取消、返回与重新进入、空数据与错误状态。
runner 可选的 `checks`/`check` 标记会报告失败、未执行和未安排断言的验收项；
声明的验收项尚未验证时，执行结果不能通过。`wait_keyboard_text` 可核对已关联键盘的
空文本与 UTF-8 输入，保存和取消还需核对应用状态。

输入、场景集合与已解析编译配置不变时，可复用成功构建或预览的诊断结果。

CLI 诊断必须提供真实场景路径，或通过生成的 context 取得已注册场景集合。
daemon 使用 `cards/list` 查询控件摘要，
使用 `cards/get` 查询字段；`doctor/get` 包含与 CLI doctor 相同的编译器兼容性报告。

调用 `compile/preview` 后，检查结果中的 `success`、`stale`、`inputIdentity`
和 `artifactIdentity`；外层 `ok` 只表示请求已经处理。身份信息包含源码 JSON 的
CRC32、字节数、绝对 root/path、编译选项 CRC32 和像素格式。源码身份不包含素材
文件内容，仍须检查 Bundle/API sidecar 是否配套。编译失败时，只有同一 root、
稳定场景路径和已解析编译选项才可回退旧 Bundle；它仅用于保留画面，不能作为新结果验收。
匿名预览和二进制流预览不复用缓存 Bundle。`compile/previewProject` 要求项目已经
open 或 update，按场景报告身份和成功状态，并保留失败场景的 dirty 状态。
已解析编译选项变化，以及引用素材或组件变化，也会使旧预览失效。
已打开项目返回空结果表示没有 dirty 场景，不能当作重新构建成功。

编译器声明 `compiler_features.scene_inspection: 1` 时，可在原有 daemon 预览请求中
添加 `inspect: {"name":"title"}`，按需查看源码、样式/组件与布局控制关系。
`inspect: true` 列出节点和简短文字提示，`nodeId` 可选择匿名节点。修改前核对源码快照 CRC；
检查结果描述初始布局，运行时命中仍需通过实际交互验证。
CLI 的 `compile`/`pack --inspect --api-header ...` 会把同类说明附加到应用 API sidecar。

修改导航或绘制层级后，用指针输入或组件导航 API 验证运行时中间帧，并覆盖动画未结束
就反向的操作。检查父级裁剪和独立前景叠加。核查性能回归时还应比较
[快照计数](runtime-api.md#拖拽快照诊断)，不能用最终截图正确证明优化路径已启用。
Host 验证与物理屏幕、设备 FPS 验收应分别报告。

已有 ESP-IDF 构建时，先运行 `gspc doctor <project> --build <build-dir>`。
使用其中报告的 Target、组件和配置契约；如果找不到构建描述，应保持为未知，
不能用 host 默认配置代替。

源码 checkout 的目录名可以不同。使用 `build_contract.component.selected_names`
取得实际 CMake 组件标识，使用 `component.paths` 确认对应目录。

`build_contract.status` 区分 `unavailable`（未找到描述）、`invalid`（无效描述）、
`incomplete`（必要信息缺失）、`stale`（项目或 Target 不一致）和
`observed`（已读取构建元数据）。根据 `missing`、`stale_reasons` 处理报告的问题。
显式 `--build` 仅选择指定构建目录。命令退出状态表示报告是否生成成功，
配置和工具兼容性由所属工程的 ESP-IDF configure/build 校验。

当所选编译器声明 `compiler_features.compile_context: 1` 时，应用 Bundle 生成后，
`doctor.contexts` 会列出每个已注册 Bundle 的 `*.context.json`。选择与修改范围
对应、状态为 `observed` 的上下文，然后运行：

```sh
gspc diagnose --context path/to/bundle_gsp.context.json
```

该命令复用实际注册的场景、目标 Profile、编解码与缓存选择，以及 slot 和项目容量
限制，并检查共享资源。显式提供场景路径可缩小检查范围。daemon 编译方法和
compile 级诊断也接受同一个 `context` 路径。无效上下文会明确失败，不会切换成
host 默认配置；`pending` 上下文需要先构建所属 Bundle。上下文是生成的构建快照，
修改 CMake、sdkconfig、Profile 或工具链设置后，应重新配置和构建。
旧版兼容编译器仍可使用原有显式参数流程。

[场景 JSON](scenes.md)解释编写结构，
[运行时与生成 API](runtime-api.md)解释应用集成，
[控件库](../components/index.md)覆盖当前支持的全部控件。
卡片中的 `example` 是编写片段，不是完整场景；使用 `example_path` 和文档路径读取完整示例与字段参考。
PageFlow/Drawer 自动化前先从 `capabilities` 确认组件运动支持，再使用
`component_get_motion`、`wait_component(name, 可选 value, max_frames)` 或
`component_event`；成功等待必须满足 idle，指定 `value` 时还需匹配目标值，超时返回 JSON-RPC 错误。
编译得到的 bounds 只是初始布局元数据，不能当作运行时命中证明。

## 选择预览路径

只检查场景布局、声明式动作和控件交互时，使用[模拟器预览与测试](../configuration/simulator-preview.md)
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

自动化时可为 runner 添加 `--headless --api-port 8266 --backend-port 8684
--input-mode api-exclusive`。runner 会加载配套 API sidecar，并报告实际会话地址和
观察到的工具版本。不同会话使用不同端口；收到 `backend_spawned` 后，等待应用
达到所需状态，再捕获渲染结果。有限时长检查可用 `--duration`，由 runner 等待
Backend 完成并清理模拟器。

需要重复验收输入与状态时，可传入 `--scenario pc/check.json`。该模式自动选择无界面
渲染和 API 独占输入，执行命名点击、拖动、标量断言、回调等待与截图，再输出构建目录下
独立的 `report.json` 路径。断言失败时保留实际观察结果，并以非零状态退出。
场景格式与执行时限见[sim_bridge 使用说明](../../../tools/sim_bridge/README.md)；
使用旧版组件时先检查 `run.py --help`。整个检查期间，应用状态仍由真实 C Backend 修改。
切页后，先使用当前工具支持的 `wait_scene` 步骤等待目标页面，再读取该页控件；
它观察实际页面状态，不会主动切页。

sim_bridge 用于验证应用逻辑，不等同于 ESP-IDF 任务调度、外设、面板时序或设备性能验证。
它运行的 API 子集和单线程 Backend 契约也应纳入检查。仍需构建目标工程，并按需要上板验收。

## 检查生成结果

检查源 JSON 和应用代码变更，重新构建以更新生成头文件和 Bundle。确认对象命名、
回调响应、资源所有权和同时活跃对象的容量。先在模拟器中预览布局与交互，再按
[显示集成](display.md)检查开发板上的运行效果。

Skill 读取所选组件的 Schema、头文件和示例，并按提问语言选择说明与文档。

## 验收组合更新

多个更新需要共同完成时，使用[目标帧回执 RPC](update-receipts.md#模拟器与-ai-操作)。
保留返回的原样回执 ID，确认结果为 `COMPLETE` 后再比较目标区域与参考图；根据后端能力和实际到达
的阶段区分渲染证据与设备显示证据。验收结束后调用 `release_update` 释放回执，
目标或画面对比失败时也应释放。
