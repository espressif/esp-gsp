# Stack View (`gsp_stackview`)

Push and pop page stack with animated navigation transitions.

## When to use

Use it to organize content, visibility, or navigation while preserving hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.0 gspc pack \
  examples/widgets/stackview/stackview.json \
  --deployable -o gsp-out/widget-preview/stackview.gspb
python -m gsp.execute --version 1.3.0 sim \
  --bundle gsp-out/widget-preview/stackview.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Children retain their hierarchy. Set clip_children on a Container or Layer to clip child drawing and input to its rectangular bounds. Nested clips intersect and move with their parent. Use fixed w/h for the viewport; radius styles the background and border.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_stackview",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "stackview",
      "parent": -1,
      "name": "details_stack",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "page_count": 3,
      "initial_page": 0,
      "capacity": 3,
      "axis": "horizontal"
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "details_stack_page0",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#142033"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 28,
      "y": 22,
      "w": 424,
      "h": 36,
      "text": "ESP-GSP Runtime",
      "font_size": 25,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 28,
      "y": 62,
      "w": 424,
      "h": 28,
      "text": "Production display profile · RGB888",
      "font_size": 16,
      "fg_color": "#8EA4BC"
    },
    {
      "type": "container",
      "parent": 1,
      "x": 28,
      "y": 106,
      "w": 202,
      "h": 90,
      "bg_color": "#1B2B40",
      "border_color": "#304762",
      "border_width": 1,
      "radius": 12
    },
    {
      "type": "label",
      "parent": 4,
      "x": 16,
      "y": 13,
      "w": 170,
      "h": 24,
      "text": "FRAME RATE",
      "font_size": 14,
      "fg_color": "#7DD3FC"
    },
    {
      "type": "label",
      "parent": 4,
      "x": 16,
      "y": 43,
      "w": 170,
      "h": 34,
      "text": "60 FPS",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "container",
      "parent": 1,
      "x": 250,
      "y": 106,
      "w": 202,
      "h": 90,
      "bg_color": "#1B2B40",
      "border_color": "#304762",
      "border_width": 1,
      "radius": 12
    },
    {
      "type": "label",
      "parent": 7,
      "x": 16,
      "y": 13,
      "w": 170,
      "h": 24,
      "text": "RENDER PATH",
      "font_size": 14,
      "fg_color": "#86EFAC"
    },
    {
      "type": "label",
      "parent": 7,
      "x": 16,
      "y": 43,
      "w": 170,
      "h": 34,
      "text": "WASM",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "button",
      "parent": 1,
      "x": 120,
      "y": 228,
      "w": 240,
      "h": 56,
      "text": "Open live metrics  →",
      "font_size": 17,
      "bg_color": "#2563EB",
      "fg_color": "#FFFFFF",
      "radius": 13,
      "events": [
        {
          "event": "click",
          "action": "stack_push",
          "target_name": "details_stack",
          "arg": 1
        }
      ]
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "details_stack_page1",
      "x": 480,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#251B38"
    },
    {
      "type": "label",
      "parent": 11,
      "x": 28,
      "y": 20,
      "w": 424,
      "h": 36,
      "text": "Live metrics",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 11,
      "x": 28,
      "y": 58,
      "w": 424,
      "h": 26,
      "text": "Renderer workload over the latest frames",
      "font_size": 16,
      "fg_color": "#C4B5D5"
    },
    {
      "type": "chart",
      "parent": 11,
      "x": 28,
      "y": 98,
      "w": 424,
      "h": 120,
      "points": [
        18,
        34,
        27,
        58,
        49,
        76,
        64,
        88,
        71,
        94
      ],
      "grid_lines": 3,
      "bg_color": "#1C152B",
      "fg_color": "#A78BFA",
      "border_color": "#493566",
      "border_width": 1,
      "radius": 11
    },
    {
      "type": "label",
      "parent": 11,
      "x": 304,
      "y": 105,
      "w": 128,
      "h": 27,
      "text": "58.7 FPS",
      "font_size": 17,
      "text_align": "right",
      "fg_color": "#F5F3FF"
    },
    {
      "type": "button",
      "parent": 11,
      "x": 28,
      "y": 246,
      "w": 154,
      "h": 48,
      "text": "←  Back",
      "font_size": 17,
      "bg_color": "#46345F",
      "fg_color": "#FFFFFF",
      "radius": 11,
      "events": [
        {
          "event": "click",
          "action": "stack_pop",
          "target_name": "details_stack"
        }
      ]
    },
    {
      "type": "button",
      "parent": 11,
      "x": 202,
      "y": 246,
      "w": 250,
      "h": 48,
      "text": "Display settings  →",
      "font_size": 17,
      "bg_color": "#7C3AED",
      "fg_color": "#FFFFFF",
      "radius": 11,
      "events": [
        {
          "event": "click",
          "action": "stack_push",
          "target_name": "details_stack",
          "arg": 2
        }
      ]
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "details_stack_page2",
      "x": 960,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#12362F"
    },
    {
      "type": "label",
      "parent": 18,
      "x": 28,
      "y": 22,
      "w": 424,
      "h": 36,
      "text": "Display settings",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "container",
      "parent": 18,
      "x": 28,
      "y": 82,
      "w": 424,
      "h": 92,
      "bg_color": "#19483E",
      "border_color": "#2B6658",
      "border_width": 1,
      "radius": 12
    },
    {
      "type": "label",
      "parent": 20,
      "x": 18,
      "y": 16,
      "w": 280,
      "h": 28,
      "text": "High quality rendering",
      "font_size": 18,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "toggle",
      "parent": 20,
      "x": 326,
      "y": 26,
      "w": 72,
      "h": 36,
      "checked": true,
      "bg_color": "#285447",
      "fg_color": "#34D399"
    },
    {
      "type": "label",
      "parent": 20,
      "x": 18,
      "y": 50,
      "w": 280,
      "h": 24,
      "text": "RGB888 · 2× supersampling",
      "font_size": 14,
      "fg_color": "#A7F3D0"
    },
    {
      "type": "button",
      "parent": 18,
      "x": 28,
      "y": 232,
      "w": 180,
      "h": 50,
      "text": "←  Back",
      "font_size": 17,
      "bg_color": "#285447",
      "fg_color": "#FFFFFF",
      "radius": 11,
      "events": [
        {
          "event": "click",
          "action": "stack_pop",
          "target_name": "details_stack"
        }
      ]
    }
  ]
}
```

This is `examples/widgets/stackview/stackview.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_stackview_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_stackview_details_stack_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_stackview_details_stack_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_stackview_details_stack_page0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_stackview_details_stack_page0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_stackview_details_stack_page1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_stackview_details_stack_page1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_stackview_details_stack_page2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_stackview_details_stack_page2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_stackview_docs_config(void)
size_t gsp_stackview_docs_dynamic_image_slots(void)
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
| `page_count` | `int` | — | 1…65535 | — | compiled page count |
| `initial_page` | `int` | — | default 0; 0…65534 | — | initial stack root page |
| `capacity` | `int` | — | default 8; 1…8 | — | maximum stack depth |
| `axis` | `enum` | — | default horizontal; `horizontal`, `vertical` | — | motion axis |

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
| `bg_color` | `color` | — | — | yes | background/fill color (#RRGGBB or #RRGGBBAA) |
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
| `transition_ms` | `int` | — | default 0; 0…65535 | — | programmatic push/pop duration; 0 keeps runtime default |
| `transition_easing` | `enum` | — | default linear; `linear`, `ease_out`, `ease_in_out` | — | programmatic push/pop easing curve |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
