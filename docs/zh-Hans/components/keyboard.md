# 键盘（`gsp_keyboard`）

由原生基础控件组成的编译期屏幕键盘。

## 适用场景

适合用一组原生控件快速提供完整、熟悉的交互。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.0 gspc pack \
  examples/widgets/keyboard/keyboard.json \
  --deployable -o gsp-out/widget-preview/keyboard.gspb
python -m gsp.execute --version 1.3.0 sim \
  --bundle gsp-out/widget-preview/keyboard.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

GSPC 将组合组件编译为原生基础图元，并为命名元素生成连接应用行为的适配 API。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "widget_keyboard",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "font_charset": " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
  "objects": [
    {
      "type": "container",
      "parent": -1,
      "x": 24,
      "y": 18,
      "w": 432,
      "h": 50,
      "bg_color": "#172235",
      "border_color": "#334A68",
      "border_width": 1,
      "radius": 9
    },
    {
      "type": "label",
      "parent": 0,
      "name": "input_value",
      "bind": "input_value",
      "bind_target": "text",
      "x": 14,
      "y": 9,
      "w": 404,
      "h": 32,
      "text": "Tap keys to enter text",
      "input": true,
      "font_size": 16,
      "fg_color": "#E2E8F0"
    },
    {
      "type": "keyboard",
      "parent": -1,
      "name": "onscreen_keyboard",
      "x": 0,
      "y": 82,
      "w": 480,
      "h": 238,
      "font_size": 12,
      "bg_color": "#0D1522",
      "fg_color": "#F8FAFC",
      "key_color": "#26364D",
      "function_color": "#1D4A66",
      "ok_color": "#167D66",
      "key_radius": 6
    }
  ]
}
```

该文件来自 `examples/widgets/keyboard/keyboard.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
bool gsp_widget_keyboard_event_decode_call( const esp_gsp_event_t *event, gsp_widget_keyboard_call_event_t *out_event)
bool gsp_widget_keyboard_event_is_onscreen_keyboard_key( const esp_gsp_event_t *event)
const gsp_component_directory_t *const * gsp_keyboard_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_keyboard_input_value_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_input_value_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_input_value_set_text( esp_gsp_handle_t gsp, const char *text)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_get_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_9_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_0_9_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_1_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_2_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_3_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_3_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_3_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_3_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_3_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_key_3_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_lower_set_visible(esp_gsp_handle_t gsp, bool visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_get_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_9_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_0_9_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_9_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_1_9_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_9_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_2_9_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_key_3_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_sym_set_visible(esp_gsp_handle_t gsp, bool visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_get_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_9_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_0_9_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_1_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_3_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_3_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_4_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_4_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_5_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_5_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_6_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_6_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_7_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_7_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_8_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_2_8_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_3_0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_3_0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_3_1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_3_1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_3_2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_key_3_2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_keyboard_onscreen_keyboard_upper_set_visible(esp_gsp_handle_t gsp, bool visible)
esp_gsp_config_t gsp_keyboard_docs_config(void)
size_t gsp_keyboard_docs_dynamic_image_slots(void)
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
| `bg_color` | `color` | — | — | 是 | 键盘背景 |
| `fg_color` | `color` | — | — | — | 键盘文字颜色 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `key_color` | `color` | — | — | — | 键盘字符键填充颜色 |
| `function_color` | `color` | — | — | — | 键盘修饰键填充颜色 |
| `ok_color` | `color` | — | — | — | 键盘确认键填充颜色 |
| `key_radius` | `int` | — | 0…65535 | — | 键盘按键圆角半径（像素） |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏 |
| `function_text_color` | `color` | — | — | — | 键盘修饰键文字颜色 |
| `delete_color` | `color` | — | — | — | 键盘删除键填充颜色 |
| `delete_text_color` | `color` | — | — | — | 键盘删除键文字颜色 |
| `ok_text_color` | `color` | — | — | — | 键盘确认键文字颜色 |
| `space_color` | `color` | — | — | — | 键盘空格键填充颜色 |
| `space_text_color` | `color` | — | — | — | 键盘空格键文字颜色 |
| `shift_label` | `string` | — | — | — | 键盘 Shift 键标签 |
| `delete_label` | `string` | — | — | — | 键盘删除键标签 |
| `ok_label` | `string` | — | — | — | 键盘确认键标签 |
| `symbols_label` | `string` | — | — | — | 键盘符号页切换键标签 |
| `letters_label` | `string` | — | — | — | 键盘字母页切换键标签 |
| `space_label` | `string` | — | — | — | 键盘空格键标签 |
| `function_font_size` | `int` | — | 1…255 | — | 键盘文字修饰键字号 |
| `shift_icon` | `path` | — | — | — | 键盘 Shift 键图标图片 |
| `delete_icon` | `path` | — | — | — | 键盘删除键图标图片 |
| `ok_icon` | `path` | — | — | — | 键盘确认键图标图片 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
