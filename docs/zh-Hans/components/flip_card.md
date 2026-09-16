# 翻转卡片（`gsp_flip_card`）

两张静态图片之间的透视翻转，支持中断和反向切换。

## 适用场景

适合展示或触发界面中的主要内容。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/flip_card/flip_card.json \
  --deployable -o gsp-out/widget-preview/flip_card.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/flip_card.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

设置 `runtime_style: true` 后，可在运行时设置深度、透明度和翻转周期；`flipped` 控制目标面。默认点击翻面，`enabled: false` 禁用点击。点击翻面后的回调使用 `value` 事件，不表示动画完成。图片路径相对场景文件，支持 PNG、JPEG 和 GSP 支持的 SVG 子集；单张源文件不超过 2 MiB，自动生成最大边长 256 的纹理。通过 `image_sets` 配置最多三组额外皮肤，再用 `set_image_set()` 切换；第 0 组是原始正反面。图片随资源包编译，不支持运行时任意导入文件、模板成员或动态几何表达式。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "flip_card_demo",
  "w": 320,
  "h": 240,
  "screen_bg": "#08131F",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 20,
      "y": 10,
      "w": 280,
      "h": 28,
      "text": "Flip card",
      "font_size": 20,
      "fg_color": "#E8F4FF",
      "text_align": "center"
    },
    {
      "type": "flip_card",
      "parent": -1,
      "name": "card",
      "x": 35,
      "y": 42,
      "w": 250,
      "h": 166,
      "front": "../../effects/assets/card_front.svg",
      "back": "../../effects/assets/card_back.svg",
      "depth": 60,
      "period_ms": 600,
      "runtime_style": true,
      "image_sets": [
        [
          "../../effects/assets/card_front_teal.svg",
          "../../effects/assets/card_back_teal.svg"
        ]
      ]
    },
    {
      "type": "label",
      "parent": -1,
      "x": 20,
      "y": 212,
      "w": 280,
      "h": 20,
      "text": "Tap to turn over",
      "font_size": 13,
      "fg_color": "#A6BDCC",
      "text_align": "center"
    }
  ]
}
```

该文件来自 `examples/widgets/flip_card/flip_card.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_flip_card_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_flip_card_demo_card_animate_depth(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_depth_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_opacity(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_opacity_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_period_ms(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_period_ms_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_get_depth(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_flip_card_demo_card_get_flipped(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_flip_card_demo_card_get_image_set(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_flip_card_demo_card_get_opacity(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_period_ms(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_flip_card_demo_card_play_depth(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_flip_card_demo_card_play_opacity(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_flip_card_demo_card_play_period_ms(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_flip_card_demo_card_set_depth(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_flipped(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_flip_card_demo_card_set_image_set(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_opacity(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_period_ms(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_flip_card_demo_card_stop_depth(esp_gsp_handle_t gsp)
esp_err_t gsp_flip_card_demo_card_stop_opacity(esp_gsp_handle_t gsp)
esp_err_t gsp_flip_card_demo_card_stop_period_ms(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_flip_card_docs_config(void)
size_t gsp_flip_card_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 32…512 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 32…512 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `runtime_style` | `bool` | — | 默认 `false` | — | 生成运行时外观设置接口（按需启用） |
| `image_sets` | `string_matrix` | — | — | — | 用于运行时主题切换的额外图片组 |
| `front` | `path` | 是 | — | — | 正面图片路径 |
| `back` | `path` | 是 | — | — | 背面图片路径 |
| `depth` | `int` | — | 默认 60; 0…100 | `runtime_style` | 透视强度百分比 |
| `period_ms` | `int` | — | 默认 560; 100…60000 | `runtime_style` | 单次翻转时长（毫秒） |

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
| `image_set` | `int` | — | 默认 0; 0…3 | `runtime_style` | 初始编译图片组索引 |
| `flipped` | `bool` | — | 默认 `false` | 是 | 是否显示背面 |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 整体透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
