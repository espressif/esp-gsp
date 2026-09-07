# Shape (`gsp_shape`)

Rectangle, rounded geometry, circle or line rendered without an image asset.

## When to use

Use it to communicate status, measurement, motion, or visual hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.3.0 gspc pack \
  examples/widgets/shape/shape.json \
  --deployable -o build/widget-preview/shape.gspb
python -m gsp.execute --version 1.2.0 sim \
  --bundle build/widget-preview/shape.gspb
```

These commands compile the same JSON below with GSPC and open it in the
published ESP-GSP simulator's browser preview. It is not an HTML recreation.
Confirm the final pixel format, fonts, display path, and performance on target
hardware.

## Runtime behavior

The simulator and device runtime render the same compiled geometry. Application-driven property changes require a simulator backend or device code.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects in this example; unnamed objects do not create unused API.

## Complete example JSON

```json
{
  "screen": "widget_shape",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "objects": [
    {
      "type": "shape",
      "parent": -1,
      "name": "rectangle",
      "x": 55,
      "y": 55,
      "w": 150,
      "h": 90,
      "shape": "rect",
      "bg_color": "#2563EB",
      "radius": 16
    },
    {
      "type": "shape",
      "parent": -1,
      "x": 275,
      "y": 45,
      "w": 110,
      "h": 110,
      "shape": "circle",
      "bg_color": "#A78BFA",
      "border_color": "#DDD6FE",
      "border_width": 3
    },
    {
      "type": "shape",
      "parent": -1,
      "x": 55,
      "y": 205,
      "w": 330,
      "h": 8,
      "shape": "line",
      "line_direction": "tl_br",
      "border_color": "#38BDF8",
      "border_width": 4
    },
    {
      "type": "shape",
      "parent": -1,
      "x": 55,
      "y": 245,
      "w": 330,
      "h": 8,
      "shape": "line",
      "line_direction": "bl_tr",
      "border_color": "#34D399",
      "border_width": 4
    }
  ]
}
```

This is `examples/widgets/shape/shape.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_shape_docs_component_directories(uint16_t *out_count)
esp_gsp_config_t gsp_shape_docs_config(void)
size_t gsp_shape_docs_dynamic_image_slots(void)
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
| `bg_color` | `color` | — | — | yes | background/fill color (#RRGGBB or #RRGGBBAA) |
| `radius` | `int` | — | default 0; 0…65535 | yes | corner radius in px |
| `border_color` | `color` | — | — | — | border stroke color |
| `border_width` | `int` | — | 0…65535 | — | border stroke width (needs border_color) |
| `shape` | `enum` | — | default rect; `rect`, `round_rect`, `circle`, `ellipse`, `line` | — | shape geometry |
| `line_direction` | `enum` | — | default tl_br; `horizontal`, `vertical`, `tl_br`, `bl_tr` | — | line direction |

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
| `fg_color` | `color` | — | — | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | — | default 255; 0…255 | yes | 0-255 blend opacity |
| `bg_gradient` | `color` | — | — | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | — | default vertical; `vertical`, `horizontal` | — | gradient direction |
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
