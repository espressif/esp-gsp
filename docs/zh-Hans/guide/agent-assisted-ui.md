# Agent 辅助 UI 开发

ESP-GSP 提供一份可安装的 Codex Skill，用于把产品需求、参考图或现有设计转换为
经过验证的场景 JSON 和生成式 C API 集成。Skill 使用应用实际选中的 GSPC 与
ESP-GSP 版本，并读取对应的控件 Schema、API 参考和示例。

## 安装 Skill

权威 Skill 位于 [ESP-GSP 仓库](https://github.com/espressif/esp-gsp)的
`skills/esp-gsp-ui`。可以让 Codex 使用 `$skill-installer`，从
`espressif/esp-gsp` 仓库的 `master` ref 安装该路径。

```text
使用 $skill-installer，从 espressif/esp-gsp 仓库的 master ref 安装
skills/esp-gsp-ui。
```

如果已经取得源码仓库，也可以在仓库根目录手动安装：

```sh
mkdir -p ~/.codex/skills
cp -R skills/esp-gsp-ui ~/.codex/skills/
```

下一轮对话通过 `$esp-gsp-ui` 调用。Skill 属于 GitHub 开发体验，不进入
ESP-IDF 组件压缩包。使用托管组件的工程仍然包含中英文文档、控件示例与生成参考，
Skill 会读取这些与组件版本匹配的内容。

## 提供有效需求

提供工程路径，描述希望实现的页面行为即可。Skill 会从现有工程读取芯片、显示、
Bundle 和工具配置。新工程可以补充已知的显示尺寸与方向，缺少必要信息时再确认。

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
4. 对真实场景执行 `gspc compatibility` 和 `gspc diagnose`；
5. 生成 Bundle 头文件，接入其中的 API，并构建应用修改；
6. 按预览需求查找或构建模拟器，检查页面与交互；
7. 汇总修改内容、构建结果和预览情况。

[场景 JSON](scenes.md)解释编写结构，
[运行时与生成 API](runtime-api.md)解释应用集成，
[控件库](../components/index.md)覆盖当前支持的全部控件。

## 检查生成结果

检查源 JSON 和应用代码变更，重新构建以更新生成头文件和 Bundle。确认对象命名、
回调响应、资源所有权和同时活跃对象的容量。先在模拟器中预览布局与交互，再按
[显示集成](display.md)检查开发板上的运行效果。

Skill 读取所选组件的 Schema、头文件和示例，并按提问语言选择说明与文档。
