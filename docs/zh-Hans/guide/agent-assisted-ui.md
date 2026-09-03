# Agent 辅助 UI 开发

ESP-GSP 提供一份可安装的 Codex Skill，用于把产品需求、参考图或现有设计转换为
经过验证的场景 JSON 和生成式 C API 集成。Skill 使用应用实际选中的 GSPC 与
ESP-GSP 版本，不另行维护第二套控件 Schema 或 API 参考。

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

给出工程路径，以及无法安全推导的产品信息：

- 芯片目标和逻辑显示尺寸；
- RGB565 或 RGB888 输出；
- BSP/显示路径和方向；
- 页面状态、动态数据、动作与导航；
- 可用字体和产品资源；
- 未使用默认 `scenes/` 时的目标场景路径；
- 期望完成的验证层级。

创建页面时可以这样描述：

```text
在这个 ESP-IDF 工程中使用 $esp-gsp-ui。为 480x320 RGB888 显示创建
scenes/settings.json，包含亮度 Slider、Wi-Fi Toggle、语言 Dropdown 和返回动作。
接入生成 API，并完成场景与 Target 构建验证。不要修改 BSP。
```

使用参考图时，应说明它是视觉参考，还是必须随产品发布的真实资源。静态图不能说明
按下、禁用、滚动、加载、错误和导航行为；这些状态有要求时应在需求中明确。

修改现有 UI 时，应描述具体行为，而不是要求无差别重写：

```text
使用 $esp-gsp-ui 为 scenes/setup.json 增加应用数据驱动的 Wheel。保留现有
Bundle symbol 和像素格式，从活动场景推导所需 slot，只通过生成 API 或公共 API
修改应用代码。
```

## Skill 会执行什么

Skill 遵循与[推荐工程工作流](workflow.md)相同的职责模型：

1. 识别实际选中的组件、Target、GSPC、Bundle、场景和 BSP；
2. 读取当前 Schema、相关控件页和经过检查的控件示例；
3. 布局和声明式行为放在 JSON，产品状态放在应用，面板行为放在 BSP；
4. 对真实场景执行 `gspc compatibility` 和 `gspc diagnose`；
5. 先构建，再使用生成的 `<symbol>_gsp.h` API；
6. 已安装独立模拟器时进行预览；
7. 分开报告场景、生成 API、Target、模拟器、开发板和人工视觉证据。

[场景 JSON](scenes.md)解释编写结构，
[运行时与生成 API](runtime-api.md)解释应用集成，
[控件库](../components/index.md)覆盖当前支持的全部控件。

## 检查生成结果

审查场景源 JSON 和应用代码差异。生成头文件、GSPB、编译器临时导出和预览输出
必须由工具管理，不能提交。确认应用控制对象具有稳定名称，回调没有阻塞，资源所有权
明确，并且容量按同时活动资源而不是数据集总量配置。

编译器通过证明场景契约成立；Target 构建通过证明配置、编译与链接成功。模拟器不能
证明面板接线、旋转、字节序、撕裂、触控映射、性能或最终视觉质量，这些项目必须在
目标硬件上验收。

## 维护约束

中英文用户共用同一份 Skill，Skill 按用户语言回答并选择对应文档树。控件字段、
默认值、C 函数声明、兼容版本和示例继续由 GSPC、公共头文件、文档生成器和
`examples/widgets/` 维护。修改这些权威来源后，Skill 会读取最新结果，不需要再同步
修改一套提示词事实。
