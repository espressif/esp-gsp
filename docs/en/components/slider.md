# Slider (`gsp_slider`)

Draggable horizontal or vertical ranged-value control.

## When to use

Use it when a user must inspect or change a bounded numeric value.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.0 gspc pack \
  examples/widgets/slider/slider.json \
  --deployable -o gsp-out/widget-preview/slider.gspb
python -m gsp.execute --version 1.3.0 sim \
  --bundle gsp-out/widget-preview/slider.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Pointer input updates retained component state through the same runtime property path used on device.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_slider",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "default_font_size": 18,
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 32,
      "y": 28,
      "w": 416,
      "h": 30,
      "text": "Slider",
      "font_size": 26,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 32,
      "y": 68,
      "w": 416,
      "h": 24,
      "text": "Drag the handle to change the value.",
      "fg_color": "#A9B8CC"
    },
    {
      "type": "slider",
      "parent": -1,
      "name": "volume",
      "x": 32,
      "y": 118,
      "w": 320,
      "h": 32,
      "min": 0,
      "max": 100,
      "value": 68,
      "track_size": 10,
      "radius": 16,
      "bg_color": "#334155",
      "fg_color": "#38BDF8",
      "knob_color": "#F8FAFC"
    },
    {
      "type": "slider",
      "parent": -1,
      "name": "temperature",
      "x": 32,
      "y": 190,
      "w": 320,
      "h": 32,
      "min": 16,
      "max": 30,
      "value": 23,
      "track_size": 8,
      "radius": 16,
      "bg_color": "#334155",
      "fg_color": "#F59E0B",
      "knob_color": "#FEF3C7"
    },
    {
      "type": "slider",
      "parent": -1,
      "name": "vertical_level",
      "x": 398,
      "y": 106,
      "w": 28,
      "h": 152,
      "vertical": true,
      "value": 44,
      "track_size": 9,
      "radius": 14,
      "bg_color": "#334155",
      "fg_color": "#A78BFA",
      "knob_color": "#F5F3FF"
    }
  ]
}
```

This is `examples/widgets/slider/slider.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_slider_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_slider_temperature_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_slider_temperature_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_slider_temperature_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_slider_temperature_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_widget_slider_vertical_level_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_slider_vertical_level_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_slider_vertical_level_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_slider_vertical_level_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_widget_slider_volume_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_slider_volume_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_slider_volume_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_slider_volume_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_gsp_config_t gsp_slider_docs_config(void)
size_t gsp_slider_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | scene: yes; template: — | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | scene: yes; template: — | y relative to parent |
| `w` | `int` | yes | 0…65535 | scene: —; template: — | width in px |
| `h` | `int` | yes | 0…65535 | scene: —; template: — | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `fg_color` | `color` | — | — | — | foreground color (text/knob/line/mark per type) |
| `bg_color` | `color` | — | — | yes | background/fill color (#RRGGBB or #RRGGBBAA) |
| `radius` | `int` | — | default 0; 0…65535 | scene: —; template: — | corner radius in px |
| `value` | `int` | — | — | yes | initial value (in min..max units) |
| `min` | `int` | — | default 0; -2147483648…2147483647 | — | value range lower bound |
| `max` | `int` | — | default 100; -2147483648…2147483647 | — | value range upper bound |
| `vertical` | `bool` | — | default `false` | — | vertical orientation |
| `knob_color` | `color` | — | default #FFFFFF | — | slider/toggle knob color |
| `track_size` | `int` | — | default 0; 0…4096 | — | track thickness |

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
| `opacity` | `int` | — | default 255; 0…255 | scene: —; template: — | 0-255 blend opacity |
| `bg_gradient` | `color` | — | — | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | — | default vertical; `vertical`, `horizontal` | — | gradient direction |
| `border_color` | `color` | — | — | — | border stroke color |
| `border_width` | `int` | — | 0…65535 | — | border stroke width (needs border_color) |
| `text` | `string` | — | — | yes | static text content (UTF-8) |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | text alignment |
| `overflow` | `enum` | — | default clip; `clip`, `ellipsis` | — | single-line overflow |
| `font` | `path` | — | — | — | per-object TTF/OTF override |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `font_charset` | `string` | — | — | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | — | — | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | — | `embedded`, `external`, `auto` | — | font storage policy: embedded/external/auto |
| `input` | `bool` | — | default `false` | — | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | — | `lossless`, `jpeg`, `hardware_jpeg` | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | — | `content`, `canvas` | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | — | — | — | SVG end shape with matching paths and paints |
| `morph` | `int` | — | 0…100 | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | — | — | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | — | — | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | — | — | yes | image file path (raster or compiled SVG) |
| `codec` | `enum` | — | `raw`, `lossless`, `jpeg`, `auto`, `store`, `qoi`, `rle16`, `default`, `hardware_jpeg` | — | image codec |
| `quality` | `int` | — | 1…100 | — | JPEG quality 1-100 (omitted = profile default) |
| `compress` | `bool` | — | — | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | — | 0.05…1.0 | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | — | 1…120 | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | — | default stretch; `stretch`, `fill`, `contain`, `cover` | — | image fit mode |
| `position_x` | `number` | — | default 0.5; 0.0…1.0 | — | image fit horizontal alignment |
| `position_y` | `number` | — | default 0.5; 0.0…1.0 | — | image fit vertical alignment |
| `rotation` | `int` | — | default 0; -32768…32767 | scene: image; template: — | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | — | default `false` | — | enable runtime image scaling |
| `scale` | `number` | — | default 1.0; 0.0625…16.0 | — | initial runtime image scale |
| `min_scale` | `number` | — | default 0.5; 0.0625…16.0 | — | minimum runtime image scale |
| `max_scale` | `number` | — | default 4.0; 0.0625…16.0 | — | maximum runtime image scale |
| `knob` | `bool` | — | default `true` | — | knob enabled |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |
| `bind` | `identifier` | — | — | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource` | — | explicit bind state family |
| `callback` | `identifier` | — | — | — | app callback name; generates scene-qualified event helpers |
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

</details>
