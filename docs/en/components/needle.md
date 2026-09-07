# Needle (`gsp_needle`)

Rotating gauge hand with configurable pivot, angle and tail.

## When to use

Use it to communicate status, measurement, motion, or visual hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.3.0 gspc pack \
  examples/widgets/needle/needle.json \
  --deployable -o build/widget-preview/needle.gspb
python -m gsp.execute --version 1.2.0 sim \
  --bundle build/widget-preview/needle.gspb
```

These commands compile the same JSON below with GSPC and open it in the
published ESP-GSP simulator's browser preview. It is not an HTML recreation.
Confirm the final pixel format, fonts, display path, and performance on target
hardware.

This preview shows the initial state authored in JSON. To reproduce application-driven motion, update the corresponding properties through the simulator backend/API or device code.

## Runtime behavior

The simulator and device runtime render the same compiled geometry. Application-driven property changes require a simulator backend or device code.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects in this example; unnamed objects do not create unused API.

## Complete example JSON

```json
{
  "screen": "widget_needle",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "objects": [
    {
      "type": "shape",
      "shape": "circle",
      "parent": -1,
      "x": 120,
      "y": 30,
      "w": 240,
      "h": 240,
      "bg_color": "#172235",
      "border_color": "#334A68",
      "border_width": 2
    },
    {
      "type": "arc",
      "parent": -1,
      "x": 145,
      "y": 55,
      "w": 190,
      "h": 190,
      "value": 68,
      "start_angle": 135,
      "sweep": 270,
      "thickness": 8,
      "bg_color": "#29384E",
      "fg_color": "#38BDF8"
    },
    {
      "type": "needle",
      "parent": -1,
      "name": "gauge_needle",
      "x": 150,
      "y": 60,
      "w": 180,
      "h": 180,
      "angle": 245,
      "thickness": 5,
      "tail_length": 22,
      "fg_color": "#FB7185"
    },
    {
      "type": "shape",
      "shape": "circle",
      "parent": -1,
      "x": 230,
      "y": 140,
      "w": 20,
      "h": 20,
      "bg_color": "#F8FAFC"
    }
  ]
}
```

This is `examples/widgets/needle/needle.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_needle_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_needle_gauge_needle_animate_angle(esp_gsp_handle_t gsp, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_needle_gauge_needle_animate_angle_to(esp_gsp_handle_t gsp, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_needle_gauge_needle_get_angle(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_needle_gauge_needle_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_needle_gauge_needle_set_angle(esp_gsp_handle_t gsp, int32_t new_value)
esp_gsp_config_t gsp_needle_docs_config(void)
size_t gsp_needle_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | yes | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | yes | y relative to parent |
| `w` | `int` | yes | 0…65535 | yes | width in px |
| `h` | `int` | yes | 0…65535 | yes | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `fg_color` | `color` | — | — | — | foreground color (text/knob/line/mark per type) |
| `angle` | `int` | — | default 0; 0…359 | — | clockwise angle (0=up) |
| `tail_length` | `int` | — | default 0; 0…65535 | — | length behind center pivot in px |
| `thickness` | `int` | — | default 0; 0…65535 | — | stroke thickness |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `layout` | `enum` | — | `row`, `column` | — | child auto-layout: row/column |
| `gap` | `int` | — | default 0; 0…4096 | — | auto-layout gap in px |
| `padding` | `int` | — | default 0; 0…4096 | — | auto-layout padding in px |
| `padding_left` | `int` | — | 0…4096 | — | row layout: leading padding override |
| `padding_right` | `int` | — | 0…4096 | — | row layout: trailing padding override |
| `padding_top` | `int` | — | 0…4096 | — | column layout: leading padding override |
| `padding_bottom` | `int` | — | 0…4096 | — | column layout: trailing padding override |
| `grow` | `int` | — | default 0; 0…100 | — | auto-layout grow weight |
| `margin` | `int` | — | default 0; 0…4096 | — | auto-layout space on both child sides |
| `hidden` | `bool` | — | default `false` | yes | start hidden (show via actions or set_visible) |
| `opacity` | `int` | — | default 255; 0…255 | yes | 0-255 blend opacity |
| `bg_color` | `color` | — | — | yes | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | — | — | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | — | default vertical; `vertical`, `horizontal` | — | gradient direction |
| `radius` | `int` | — | default 0; 0…65535 | yes | corner radius in px |
| `border_color` | `color` | — | — | — | border stroke color |
| `border_width` | `int` | — | 0…65535 | — | border stroke width (needs border_color) |
| `text` | `string` | — | — | yes | static text content (UTF-8) |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | text alignment |
| `overflow` | `enum` | — | default clip; `clip`, `ellipsis` | — | single-line overflow |
| `font_charset` | `string` | — | — | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | — | `embedded`, `external`, `auto` | — | font storage policy: embedded/external/auto |
| `input` | `bool` | — | default `false` | — | text field: attaches the caret/keyboard flow |
| `image` | `path` | — | — | yes | image file path (PNG) |
| `codec` | `enum` | — | `raw`, `lossless`, `jpeg`, `auto` | — | image codec |
| `quality` | `int` | — | 1…100 | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | — | — | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | — | 0.05…1.0 | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | — | 1…120 | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | — | default stretch; `stretch`, `fill`, `contain`, `cover` | — | image fit mode |
| `position_x` | `number` | — | default 0.5; 0.0…1.0 | — | image fit horizontal alignment |
| `position_y` | `number` | — | default 0.5; 0.0…1.0 | — | image fit vertical alignment |
| `rotation` | `int` | — | default 0; -32768…32767 | yes | clockwise opaque-image rotation around the bounding-box center; clipped to the box |
| `scalable` | `bool` | — | default `false` | — | enable runtime image scaling |
| `scale` | `number` | — | default 1.0; 0.0625…16.0 | — | initial runtime image scale |
| `min_scale` | `number` | — | default 0.5; 0.0625…16.0 | — | minimum runtime image scale |
| `max_scale` | `number` | — | default 4.0; 0.0625…16.0 | — | maximum runtime image scale |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |
| `bind` | `identifier` | — | — | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource` | — | explicit bind state family |
| `callback` | `identifier` | — | — | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | — | — | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | — | — | — | declare this subtree as a render template |
| `max_instances` | `int` | — | 1…65535 | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | — | — | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | — | — | — | template image exposes a per-instance resource slot |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |
| `font` | `path` | — | — | — | per-object TTF/OTF override |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |

</details>
