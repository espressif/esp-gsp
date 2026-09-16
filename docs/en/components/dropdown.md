# Dropdown (`gsp_dropdown`)

Compact single-choice control that opens an option panel.

## When to use

Use it when a familiar interaction is best delivered as a ready-made group of native controls.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/dropdown/dropdown.json \
  --deployable -o gsp-out/widget-preview/dropdown.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/dropdown.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

GSPC compiles the group into native primitives and generates adapters for application behavior exposed by named elements.

`selected` is the zero-based initial index (default 0). Generated `set_selected` updates the active scene's runtime selection and label, not the JSON. Restore saved selection after entering that scene. Read a clicked choice from `event->arg`; a getter inside the callback can still observe the value before the input transaction commits.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_dropdown",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "font_charset": " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
  "objects": [
    {
      "type": "dropdown",
      "parent": -1,
      "name": "metric",
      "x": 70,
      "y": 55,
      "w": 340,
      "h": 48,
      "options": [
        "Frame throughput",
        "Memory budget",
        "Media decode",
        "Input latency"
      ],
      "selected": 1,
      "open_direction": "down",
      "font_size": 16,
      "bg_color": "#172235",
      "panel_color": "#1D2D44",
      "fg_color": "#E2E8F0",
      "border_color": "#334A68",
      "border_width": 1,
      "radius": 10
    },
    {
      "type": "label",
      "parent": -1,
      "x": 70,
      "y": 130,
      "w": 340,
      "h": 60,
      "text": "Tap the field, then choose an option from the panel.",
      "font_size": 16,
      "fg_color": "#94A3B8"
    }
  ]
}
```

This is `examples/widgets/dropdown/dropdown.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_dropdown_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_dropdown___metric_label_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown___metric_panel_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown_metric_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown_metric_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_dropdown_metric_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_widget_dropdown_metric_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_gsp_config_t gsp_dropdown_docs_config(void)
size_t gsp_dropdown_docs_dynamic_image_slots(void)
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
| `bg_color` | `color` | — | — | yes | dropdown background |
| `fg_color` | `color` | — | — | — | dropdown text color |
| `border_color` | `color` | — | — | — | border stroke color |
| `border_width` | `int` | — | 0…65535 | — | border stroke width |
| `radius` | `int` | — | default 0; 0…65535 | scene: —; template: — | corner radius |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `options` | `string_list` | yes | — | — | option labels |
| `open_direction` | `enum` | — | default down; `down`, `up` | — | direction in which the option panel opens |
| `selected` | `int` | — | default 0; 0…65535 | yes | initially selected index |
| `panel_color` | `color` | — | — | — | panel fill color |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `grow` | `int` | — | default 0; 0…100 | — | auto-layout grow weight |
| `font` | `path` | — | — | — | per-object TTF/OTF override |
| `items` | `string_list` | — | — | — | normalized dropdown option labels |
| `item_height` | `int` | — | 0…65535 | — | item row height |
| `callback` | `identifier` | — | — | — | selection callback |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
