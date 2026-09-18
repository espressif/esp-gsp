# 参数特效（`gsp_effect`）

支持自动播放的呼吸、光环、水波进度和扫光。

## 适用场景

适合传达状态、测量结果、运动过程或视觉层级。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.5.0 gspc pack \
  examples/widgets/effect/effect.json \
  --deployable -o gsp-out/widget-preview/effect.gspb
python -m gsp.execute --version 1.4.0 sim \
  --bundle gsp-out/widget-preview/effect.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

模拟器与设备运行时渲染相同的编译几何；由应用驱动的属性变化需要模拟器后端或设备代码。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "effects",
  "w": 320,
  "h": 240,
  "screen_bg": "#0B1424",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 16,
      "y": 10,
      "w": 288,
      "h": 24,
      "text": "GSP  /  MOTION",
      "font_size": 18,
      "fg_color": "#E8F4FF"
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "charge",
      "x": 28,
      "y": 54,
      "w": 112,
      "h": 112,
      "effect": "liquid",
      "value": 62,
      "fg_color": "#35DCBA",
      "bg_color": "#153D4A",
      "period_ms": 2400
    },
    {
      "type": "label",
      "parent": -1,
      "x": 48,
      "y": 97,
      "w": 72,
      "h": 32,
      "text": "62%",
      "font_size": 26,
      "fg_color": "#FFFFFF",
      "text_align": "center"
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "loading",
      "x": 188,
      "y": 56,
      "w": 76,
      "h": 76,
      "effect": "ring",
      "value": 35,
      "fg_color": "#6BAFFF",
      "period_ms": 1200
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "status",
      "x": 212,
      "y": 144,
      "w": 28,
      "h": 28,
      "effect": "pulse",
      "fg_color": "#C99AFF",
      "period_ms": 2000
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "scan",
      "x": 28,
      "y": 196,
      "w": 264,
      "h": 14,
      "effect": "shimmer",
      "fg_color": "#9ACFFF",
      "bg_color": "#1C3350",
      "period_ms": 1800
    }
  ]
}
```

该文件来自 `examples/widgets/effect/effect.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_effect_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_effects_charge_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_charge_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_charge_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_charge_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_charge_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_charge_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_charge_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_charge_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_charge_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_charge_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_charge_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_charge_stop_phase(esp_gsp_handle_t gsp)
esp_err_t gsp_effects_loading_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_loading_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_loading_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_loading_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_loading_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_loading_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_loading_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_loading_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_loading_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_loading_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_loading_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_loading_stop_phase(esp_gsp_handle_t gsp)
esp_err_t gsp_effects_scan_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_scan_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_scan_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_scan_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_scan_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_scan_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_scan_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_scan_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_scan_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_scan_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_scan_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_scan_stop_phase(esp_gsp_handle_t gsp)
esp_err_t gsp_effects_status_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_status_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_status_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_status_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_status_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_status_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_status_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_status_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_status_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_status_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_status_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_status_stop_phase(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_effect_docs_config(void)
size_t gsp_effect_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `effect` | `enum` | 是 | `pulse`, `ring`, `liquid`, `shimmer` | — | 参数特效类型 |
| `fg_color` | `color` | — | 默认 #38BDF8 | `runtime_style` | 特效颜色 |
| `bg_color` | `color` | — | — | — | 可选静态背景颜色 |
| `value` | `int` | — | 默认 100; 0…100 | — | 液面高度或特效强度，百分比 |
| `period_ms` | `int` | — | 默认 2800; 100…60000 | `runtime_style` | 动画周期，毫秒 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `runtime_style` | `bool` | — | 默认 `false` | — | 生成运行时外观设置接口（按需启用） |
| `shimmer_style` | `enum` | — | 默认 linear; `linear`, `soft`, `diagonal` | — | 扫光外观 |
| `playing` | `bool` | — | 默认 `true` | — | 可见时自动推进动画 |
| `phase` | `int` | — | 默认 0; 0…65535 | — | 初始周期相位，0..65535 |
| `ring_style` | `enum` | — | 默认 trail; `solid`, `glow`, `trail` | — | 光环样式 |
| `softness` | `int` | — | 默认 75; 0…100 | `runtime_style` | 光晕边缘柔和程度，百分比 |
| `strength` | `int` | — | 默认 65; 0…100 | `runtime_style` | 柔光强度，百分比 |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 特效整体透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
