# Minimal effect integration / 最小特效接入

Charging orb with a separate percentage Label, a flip card and touch glow.
Tap the card to flip; tap TOUCH to replay the glow.

充电球搭配独立百分比文字、翻转卡片和点击光效。点击卡片翻面，点击 TOUCH 重播柔光。

## Preview / 预览

Run from the repository root / 从仓库根目录运行：

```sh
GSPC_EXECUTABLE="$PWD/ci/gspc-dev" \
GSP_SIM_APPLICATION_SOURCE="$PWD/examples/effects/minimal/simulator.c" \
  bash tools/sim/run_scene.sh examples/effects/minimal/scene.json
```

## Application / 业务接入

Compile `scene.json` with your application's bundle workflow. Include the generated
bundle header before `app_logic.h`, as shown in `simulator.c`. On hardware, create
and start the UI through your platform adapter, then call these helpers with this
scene active. Handle returned errors according to your application policy.

沿用应用的场景打包与 UI 创建流程；先包含生成的 bundle 头文件，再包含 `app_logic.h`。
`simulator.c` 是模拟器入口。设备侧在本场景激活后调用以下函数并处理返回值：

```c
ESP_ERROR_CHECK(quick_effects_init(ui));
ESP_ERROR_CHECK(quick_effects_update(ui, 78, true));
ESP_ERROR_CHECK(quick_effects_set_color(ui, 0x69CEF6));
ESP_ERROR_CHECK(quick_effects_set_images(ui, 1));
ESP_ERROR_CHECK(quick_effects_stop(ui));
```

Update on battery changes (0..100), not every frame. `stop()` keeps the percentage
and static halo; close the whole UI through the adapter's normal lifecycle.
Helpers submit ordinary setters separately. Use the component batch API when updates must be atomic.

电量变化时更新即可，范围 0..100；文字字库已包含数字和 `%`。`stop()` 保留电量与
静态光环；退出整个 UI 使用平台适配层的生命周期接口。辅助函数分别提交各 setter；需要原子更新时使用组件批量更新 API。

## Customize / 自定义

See the [effect guide](../README.md#runtime-appearance-and-compiled-image-groups)
for `runtime_style`, portable RGB colors, image groups and declarative `play_glow`.
In this scene, image group 0 is the original card pair; group 1 is the teal pair.
Replace relative image paths in `scene.json` and recompile to use your artwork.

外观、统一颜色、图片组和点击光效详见[特效指南](../README_zh.md)。本场景图片组 0
为默认卡片，组 1 为青色卡片；修改 `scene.json` 中的图片相对路径后重新编译即可。

For vector SVG errors, use the reported element ID (file/line/column for XML checks)
to locate the artwork. Convert text to paths, gradients to solid fills, or complex
compositing to PNG. PNG cannot retain vector morphing. `gspc svg-info file.svg`
inspects IDs and vector compatibility; unsupported semantics are rejected.

矢量 SVG 报错可根据元素 ID 定位，XML 检查还提供文件、行、列。文字转路径，渐变改
纯色，复杂合成可导出 PNG；PNG 不支持矢量形变。`gspc svg-info file.svg` 可检查
元素及兼容性，不支持的语义不会被静默忽略。
