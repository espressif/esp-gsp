# 充电球（`gsp_charging_orb`）

提供水波、柔光光环和彩色光环，可配置充电粒子和膜状波纹。

## 适用场景

适合传达状态、测量结果、运动过程或视觉层级。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/charging_orb/charging_orb.json \
  --deployable -o gsp-out/widget-preview/charging_orb.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/charging_orb.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

模拟器与设备运行时渲染相同的编译几何；由应用驱动的属性变化需要模拟器后端或设备代码。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "charging_orb_demo",
  "w": 320,
  "h": 240,
  "screen_bg": "#08131F",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "charging_orb",
      "name": "charge",
      "parent": -1,
      "x": 70,
      "y": 20,
      "w": 180,
      "h": 180,
      "style": "halo",
      "value": 62,
      "fg_color": "#69CEF6",
      "strength": 85,
      "period_ms": 4200,
      "particles": 4,
      "softness": 90,
      "ripple": 55,
      "charging": true
    },
    {
      "type": "label",
      "parent": -1,
      "x": 90,
      "y": 92,
      "w": 140,
      "h": 40,
      "text": "62%",
      "font_size": 32,
      "fg_color": "#EFFFFF",
      "text_align": "center"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 20,
      "y": 210,
      "w": 280,
      "h": 22,
      "text": "ENERGY / CHARGING",
      "font_size": 14,
      "fg_color": "#7AADB5",
      "text_align": "center"
    }
  ]
}
```

该文件来自 `examples/widgets/charging_orb/charging_orb.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_charging_orb_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_charging_orb_demo_charge_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_charging_orb_demo_charge_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_charging_orb_demo_charge_get_charging(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_charging_orb_demo_charge_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_charging_orb_demo_charge_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_charging_orb_demo_charge_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_charging_orb_demo_charge_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_charging_orb_demo_charge_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_charging_orb_demo_charge_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_charging_orb_demo_charge_set_charging(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_charging_orb_demo_charge_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_charging_orb_demo_charge_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_charging_orb_demo_charge_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_charging_orb_demo_charge_stop_phase(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_charging_orb_docs_config(void)
size_t gsp_charging_orb_docs_dynamic_image_slots(void)
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
| `fg_color` | `color` | — | 默认 #38BDF8 | `runtime_style` | 特效颜色 |
| `value` | `int` | — | 默认 100; 0…100 | — | 液面高度或特效强度，百分比 |
| `period_ms` | `int` | — | 默认 2800; 100…60000 | `runtime_style` | 动画周期，毫秒 |
| `softness` | `int` | — | 默认 75; 0…100 | `runtime_style` | 光晕边缘柔和程度，百分比 |
| `strength` | `int` | — | 默认 65; 0…100 | `runtime_style` | 柔光强度，百分比 |
| `ripple` | `int` | — | 默认 45; 0…100 | `runtime_style` | 贴附式膜状波纹强度，百分比 |
| `style` | `enum` | — | 默认 halo; `liquid`, `halo`, `plasma` | — | 充电球视觉风格 |
| `charging` | `bool` | — | 默认 `true` | — | 开启充电动态效果和粒子 |
| `particles` | `int` | — | 默认 4; 0…6 | `runtime_style` | 柔光能量粒子数量上限 |

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
| `bg_color` | `color` | — | — | — | 可选静态背景颜色 |
| `playing` | `bool` | — | 默认 `true` | — | 可见时自动推进动画 |
| `phase` | `int` | — | 默认 0; 0…65535 | — | 初始周期相位，0..65535 |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 特效整体透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
