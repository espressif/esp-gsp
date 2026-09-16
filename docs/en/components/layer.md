# Layer (`gsp_layer`)

Full or partial visual layer used for overlays and visibility groups.

## When to use

Use it to organize content, visibility, or navigation while preserving hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/layer/layer.json \
  --deployable -o gsp-out/widget-preview/layer.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/layer.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Children retain their hierarchy. Set clip_children on a Container or Layer to clip child drawing and input to its rectangular bounds. Nested clips intersect and move with their parent. Use fixed w/h for the viewport; radius styles the background and border.

Dynamic `opacity` changes the layer's own drawing, not the opacity of its entire subtree. A grouping-only layer with no visual content of its own cannot use dynamic `w`, `h`, `radius`, or `opacity`; the compiler rejects these declarations. Dynamic `x`/`y` can move such a subtree. See [dynamic property scope](../guide/scenes.md#dynamic-property-scope).

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_layer",
  "w": 480,
  "h": 320,
  "screen_bg": "#0D1522",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "layer",
      "parent": -1,
      "x": 24,
      "y": 28,
      "w": 310,
      "h": 244,
      "bg_color": "#173A72",
      "opacity": 235,
      "border_color": "#2E68B7",
      "border_width": 1,
      "radius": 18
    },
    {
      "type": "label",
      "parent": 0,
      "x": 22,
      "y": 20,
      "w": 260,
      "h": 32,
      "text": "Base layer",
      "font_size": 23,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 0,
      "x": 22,
      "y": 62,
      "w": 255,
      "h": 48,
      "text": "Content remains grouped below the overlay.",
      "font_size": 16,
      "fg_color": "#BFDBFE"
    },
    {
      "type": "container",
      "parent": 0,
      "x": 22,
      "y": 132,
      "w": 120,
      "h": 70,
      "bg_color": "#23529A",
      "radius": 11
    },
    {
      "type": "label",
      "parent": 3,
      "x": 12,
      "y": 10,
      "w": 96,
      "h": 22,
      "text": "STATUS",
      "font_size": 13,
      "fg_color": "#93C5FD"
    },
    {
      "type": "label",
      "parent": 3,
      "x": 12,
      "y": 35,
      "w": 96,
      "h": 25,
      "text": "Ready",
      "font_size": 18,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "button",
      "parent": -1,
      "x": 350,
      "y": 28,
      "w": 104,
      "h": 46,
      "text": "Toggle",
      "font_size": 16,
      "bg_color": "#2563EB",
      "fg_color": "#FFFFFF",
      "radius": 11,
      "events": [
        {
          "event": "click",
          "action": "toggle",
          "target_name": "overlay"
        }
      ]
    },
    {
      "type": "layer",
      "parent": -1,
      "name": "overlay",
      "x": 154,
      "y": 86,
      "w": 300,
      "h": 202,
      "bg_color": "#6D28D9",
      "opacity": 242,
      "border_color": "#C4B5FD",
      "border_width": 1,
      "radius": 18
    },
    {
      "type": "label",
      "parent": 7,
      "x": 22,
      "y": 20,
      "w": 190,
      "h": 32,
      "text": "Overlay layer",
      "font_size": 23,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 7,
      "x": 22,
      "y": 62,
      "w": 255,
      "h": 50,
      "text": "One visibility action controls this complete subtree.",
      "font_size": 16,
      "fg_color": "#EDE9FE"
    },
    {
      "type": "container",
      "parent": 7,
      "x": 22,
      "y": 126,
      "w": 112,
      "h": 40,
      "bg_color": "#8B5CF6",
      "radius": 20
    },
    {
      "type": "label",
      "parent": 10,
      "x": 10,
      "y": 8,
      "w": 92,
      "h": 24,
      "text": "TOP LAYER",
      "font_size": 13,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "button",
      "parent": 7,
      "x": 158,
      "y": 126,
      "w": 120,
      "h": 40,
      "text": "Hide layer",
      "font_size": 15,
      "bg_color": "#4C1D95",
      "fg_color": "#FFFFFF",
      "radius": 10,
      "events": [
        {
          "event": "click",
          "action": "hide",
          "target_name": "overlay"
        }
      ]
    }
  ]
}
```

This is `examples/widgets/layer/layer.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_layer_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_layer_overlay_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_layer_overlay_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_layer_overlay_get_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_layer_overlay_set_visible(esp_gsp_handle_t gsp, bool visible)
esp_gsp_config_t gsp_layer_docs_config(void)
size_t gsp_layer_docs_dynamic_image_slots(void)
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
| `opacity` | `int` | — | default 255; 0…255 | scene: own fill; template: own fill | 0-255 blend opacity |
| `bg_color` | `color` | — | — | yes | background/fill color (#RRGGBB or #RRGGBBAA) |
| `radius` | `int` | — | default 0; 0…65535 | scene: own fill; template: own fill | corner radius in px |
| `border_color` | `color` | — | — | — | border stroke color |
| `border_width` | `int` | — | 0…65535 | — | border stroke width (needs border_color) |

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
| `bg_gradient` | `color` | — | — | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | — | default vertical; `vertical`, `horizontal` | — | gradient direction |
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
| `clip_children` | `bool` | — | default `false` | — | clip descendants and their hit areas to the layer bounds |
| `block_scene_swipe` | `bool` | — | default `false` | — | while visible, block horizontal scene swipes |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
