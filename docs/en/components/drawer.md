# Drawer (`gsp_drawer`)

Edge-attached overlay that opens and closes with pointer gestures.

## When to use

Use it to organize content, visibility, or navigation while preserving hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.0 gspc pack \
  examples/widgets/drawer/drawer.json \
  --deployable -o gsp-out/widget-preview/drawer.gspb
python -m gsp.execute --version 1.3.0 sim \
  --bundle gsp-out/widget-preview/drawer.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Children retain their hierarchy. Set clip_children on a Container or Layer to clip child drawing and input to its rectangular bounds. Nested clips intersect and move with their parent. Use fixed w/h for the viewport; radius styles the background and border.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_drawer",
  "w": 480,
  "h": 320,
  "screen_bg": "#0E1726",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 42,
      "y": 62,
      "w": 396,
      "h": 42,
      "text": "Device dashboard",
      "text_align": "center",
      "font_size": 28,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 62,
      "y": 112,
      "w": 356,
      "h": 44,
      "text": "Open the edge panel, adjust its controls, then close it by button or drag.",
      "text_align": "center",
      "font_size": 14,
      "fg_color": "#94A3B8"
    },
    {
      "type": "button",
      "parent": -1,
      "x": 150,
      "y": 190,
      "w": 180,
      "h": 48,
      "text": "Open settings",
      "font_size": 16,
      "bg_color": "#2563EB",
      "fg_color": "#FFFFFF",
      "radius": 12,
      "events": [
        {
          "event": "click",
          "action": "drawer_open",
          "target_name": "settings_drawer",
          "animated": true
        }
      ]
    },
    {
      "type": "drawer",
      "parent": -1,
      "name": "settings_drawer",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 208,
      "edge": "top",
      "open": false,
      "bg_color": "#172235F8"
    },
    {
      "type": "label",
      "parent": 3,
      "x": 28,
      "y": 22,
      "w": 260,
      "h": 30,
      "text": "Quick settings",
      "font_size": 22,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "button",
      "parent": 3,
      "x": 390,
      "y": 18,
      "w": 58,
      "h": 36,
      "text": "Close",
      "font_size": 12,
      "bg_color": "#293A52",
      "fg_color": "#DCE8F7",
      "radius": 9,
      "events": [
        {
          "event": "click",
          "action": "drawer_close",
          "target_name": "settings_drawer",
          "animated": true
        }
      ]
    },
    {
      "type": "label",
      "parent": 3,
      "x": 28,
      "y": 78,
      "w": 120,
      "h": 24,
      "text": "Brightness",
      "font_size": 14,
      "fg_color": "#A8B8CB"
    },
    {
      "type": "slider",
      "parent": 3,
      "x": 138,
      "y": 75,
      "w": 190,
      "h": 28,
      "value": 68,
      "track_size": 9,
      "bg_color": "#334155",
      "fg_color": "#38BDF8",
      "knob_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": 3,
      "x": 28,
      "y": 127,
      "w": 190,
      "h": 24,
      "text": "High quality mode",
      "font_size": 14,
      "fg_color": "#A8B8CB"
    },
    {
      "type": "toggle",
      "parent": 3,
      "x": 360,
      "y": 119,
      "w": 72,
      "h": 36,
      "checked": true,
      "bg_color": "#334155",
      "fg_color": "#22C55E"
    },
    {
      "type": "shape",
      "parent": 3,
      "x": 205,
      "y": 187,
      "w": 70,
      "h": 5,
      "shape": "rect",
      "bg_color": "#64748B",
      "radius": 3
    }
  ]
}
```

This is `examples/widgets/drawer/drawer.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_drawer_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_drawer_settings_drawer_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_drawer_settings_drawer_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_drawer_docs_config(void)
size_t gsp_drawer_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | scene: yes; template: own fill | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | scene: yes; template: own fill | y relative to parent |
| `w` | `int` | yes | 0…65535 | scene: own fill; template: own fill | width in px |
| `h` | `int` | yes | 0…65535 | scene: own fill; template: own fill | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | yes | background/fill color (#RRGGBB or #RRGGBBAA) |
| `edge` | `enum` | — | default top; `top`, `bottom`, `left`, `right` | — | drawer attachment edge |
| `open` | `bool` | — | default `false` | — | initial drawer state |

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
| `opacity` | `int` | — | default 255; 0…255 | scene: own fill; template: own fill | 0-255 blend opacity |
| `bg_gradient` | `color` | — | — | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | — | default vertical; `vertical`, `horizontal` | — | gradient direction |
| `radius` | `int` | — | default 0; 0…65535 | scene: own fill; template: own fill | corner radius in px |
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
