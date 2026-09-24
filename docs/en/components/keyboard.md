# Keyboard (`gsp_keyboard`)

Compile-time on-screen keyboard assembled from native controls.

## When to use

Use it when a familiar interaction is best delivered as a ready-made group of native controls.

## Local interactive preview

First [set up the commands from the compatibility guide](../reference/compatibility.md#tool-commands),
then run from an unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/keyboard/keyboard.json \
  --deployable -o gsp-out/widget-preview/keyboard.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/keyboard.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

`input: true` marks the target text field, but key taps write to it only after the application attaches the keyboard action to that field's text bind. For C integration, use the bundle header from `gspc compile --api-header app.h` or the ESP-GSP CMake build. After entering this scene, expose its IDs before including that header and attach them:

```c
#define GSP_BUNDLE_ENABLE_RAW_IDS
#include "app.h"  /* Replace with your generated bundle API header. */

esp_gsp_err_t err = esp_gsp_keyboard_attach(
    ui, GSP_WIDGET_KEYBOARD_ACT_ID_ONSCREEN_KEYBOARD_KEY,
    GSP_WIDGET_KEYBOARD_BIND_INPUT_VALUE);
```

Check `err` for `ESP_GSP_OK`. The attachment starts with an empty edit buffer; character keys update the field, DEL removes a character, and OK reaches the application callback.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_keyboard",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../../common/fonts/DejaVuSans.ttf",
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

This is `examples/usage/widgets/keyboard/keyboard.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

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

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | y relative to parent |
| `w` | `int` | yes | 0…65535 | scene: —; template: — | width in px |
| `h` | `int` | yes | 0…65535 | scene: —; template: — | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | yes | keyboard background |
| `fg_color` | `color` | — | — | — | keyboard text color |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `key_color` | `color` | — | — | — | keyboard character-key fill color |
| `function_color` | `color` | — | — | — | keyboard modifier-key fill color |
| `ok_color` | `color` | — | — | — | keyboard confirmation-key fill color |
| `key_radius` | `int` | — | 0…65535 | — | keyboard key corner radius in px |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `font` | `path` | — | — | — | per-object TTF/OTF override |
| `hidden` | `bool` | — | default `false` | yes | start hidden |
| `function_text_color` | `color` | — | — | — | keyboard modifier-key text color |
| `delete_color` | `color` | — | — | — | keyboard delete-key fill color |
| `delete_text_color` | `color` | — | — | — | keyboard delete-key text color |
| `ok_text_color` | `color` | — | — | — | keyboard confirmation-key text color |
| `space_color` | `color` | — | — | — | keyboard space-key fill color |
| `space_text_color` | `color` | — | — | — | keyboard space-key text color |
| `shift_label` | `string` | — | — | — | keyboard shift-key label |
| `delete_label` | `string` | — | — | — | keyboard delete-key label |
| `ok_label` | `string` | — | — | — | keyboard confirmation-key label |
| `symbols_label` | `string` | — | — | — | keyboard symbols-page key label |
| `letters_label` | `string` | — | — | — | keyboard letters-page key label |
| `space_label` | `string` | — | — | — | keyboard space-key label |
| `function_font_size` | `int` | — | 1…255 | — | keyboard text modifier-key font size |
| `shift_icon` | `path` | — | — | — | keyboard shift-key icon image |
| `delete_icon` | `path` | — | — | — | keyboard delete-key icon image |
| `ok_icon` | `path` | — | — | — | keyboard confirmation-key icon image |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
