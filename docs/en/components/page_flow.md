# Page Flow (`gsp_page_flow`)

Swipeable multi-page viewport with horizontal or vertical motion.

## When to use

Use it to organize content, visibility, or navigation while preserving hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.5.0 gspc pack \
  examples/widgets/page_flow/page_flow.json \
  --deployable -o gsp-out/widget-preview/page_flow.gspb
python -m gsp.execute --version 1.4.0 sim \
  --bundle gsp-out/widget-preview/page_flow.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Children retain their hierarchy. Set clip_children on a Container or Layer to clip child drawing and input to its rectangular bounds. Nested clips intersect and move with their parent. Use fixed w/h for the viewport; radius styles the background and border.

Use `pages: ["first", "second"]` to name page roots explicitly in page-index order. Its length must equal `page_count`; each entry must uniquely reference a direct child. Lay pages out along `axis` in viewport-width or viewport-height steps. Children not listed in `pages` stay fixed. When omitted, direct children named `<name>_tab0`, `<name>_tab1`, etc. remain supported. Nested transforms follow parent relationships; object array order still controls paint order.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_page_flow",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "swipe": false,
  "objects": [
    {
      "type": "page_flow",
      "parent": -1,
      "name": "onboarding",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "page_count": 3,
      "selected": 0,
      "axis": "horizontal",
      "cyclic": true,
      "bar_height": 0,
      "pages": [
        "onboarding_tab0",
        "onboarding_tab1",
        "onboarding_tab2"
      ]
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "onboarding_tab0",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#123A5A"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 55,
      "y": 95,
      "w": 370,
      "h": 55,
      "text": "Page Flow 1",
      "font_size": 30,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 55,
      "y": 165,
      "w": 370,
      "h": 40,
      "text": "Swipe horizontally",
      "text_align": "center",
      "fg_color": "#BAE6FD"
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "onboarding_tab1",
      "x": 480,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#4A234F"
    },
    {
      "type": "label",
      "parent": 4,
      "x": 55,
      "y": 95,
      "w": 370,
      "h": 55,
      "text": "Page Flow 2",
      "font_size": 30,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 4,
      "x": 55,
      "y": 165,
      "w": 370,
      "h": 40,
      "text": "Content follows the gesture",
      "text_align": "center",
      "fg_color": "#F5D0FE"
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "onboarding_tab2",
      "x": 960,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#234A38"
    },
    {
      "type": "label",
      "parent": 7,
      "x": 55,
      "y": 95,
      "w": 370,
      "h": 55,
      "text": "Page Flow 3",
      "font_size": 30,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 7,
      "x": 55,
      "y": 165,
      "w": 370,
      "h": 40,
      "text": "Cyclic navigation enabled",
      "text_align": "center",
      "fg_color": "#BBF7D0"
    }
  ]
}
```

This is `examples/widgets/page_flow/page_flow.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_page_flow_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_page_flow_onboarding_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_page_flow_onboarding_tab0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_tab0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_page_flow_onboarding_tab1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_tab1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_page_flow_onboarding_tab2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_tab2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_page_flow_docs_config(void)
size_t gsp_page_flow_docs_dynamic_image_slots(void)
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
| `selected` | `int` | — | default 0; 0…65535 | yes | initially selected item index |
| `cyclic` | `bool` | — | default `false` | — | wheel wraps around |
| `pages` | `string_list` | — | — | — | ordered names of direct page children; overrides legacy &lt;name&gt;_tabN discovery |
| `page_count` | `int` | — | 1…65535 | — | compiled page count |
| `axis` | `enum` | — | default horizontal; `horizontal`, `vertical` | — | page motion axis |
| `bar_height` | `int` | — | default 56; 0…4096 | — | tab bar height |

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
| `items` | `string_list` | — | — | — | item texts (list/wheel/dropdown/tabview) |
| `item_height` | `int` | — | default 0; 0…65535 | — | row height for list/wheel |
| `items_per_page` | `int` | — | default 0; 0…65535 | — | tabview items per page |
| `visible_rows` | `int` | — | default 0; 0…65535 | — | visible row count (alt to item_height) |
| `snap_to_item` | `bool` | — | default `false` | — | scrolling snaps to row boundaries |
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
| `stop_anywhere` | `bool` | — | default `false` | — | allow PageFlow to settle between pages |
| `page_extent` | `int` | — | default 0; 0…65535 | — | PageFlow drag extent |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
