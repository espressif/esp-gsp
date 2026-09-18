# Grid (`gsp_grid`)

Virtualized multi-column collection backed by a reusable cell template.

## When to use

Use it to browse, select, or present repeated application data efficiently.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.5.0 gspc pack \
  examples/widgets/grid/grid.json \
  --deployable -o gsp-out/widget-preview/grid.gspb
python -m gsp.execute --version 1.4.0 sim \
  --bundle gsp-out/widget-preview/grid.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Rows or cells are rendered from scene data and can connect to scrolling, selection, or application data adapters.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_grid",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "layer",
      "parent": -1,
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320
    },
    {
      "type": "container",
      "parent": 0,
      "x": 0,
      "y": 0,
      "w": 116,
      "h": 82,
      "template": "preview_cell",
      "bg_color": "#1D2D44",
      "border_color": "#334A68",
      "border_width": 1,
      "radius": 10
    },
    {
      "type": "image",
      "parent": 1,
      "x": 8,
      "y": 8,
      "w": 100,
      "h": 66,
      "image": "../../benchmark/scenes/bench_industrial.png"
    },
    {
      "type": "grid",
      "parent": 0,
      "name": "gallery",
      "x": 44,
      "y": 28,
      "w": 392,
      "h": 264,
      "column_count": 3,
      "column_gap": 14,
      "row_gap": 12,
      "cell_template": "preview_cell",
      "item_count": 15,
      "scroll_snapshot": true
    }
  ]
}
```

This is `examples/widgets/grid/grid.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_grid_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_grid_gallery_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_grid_gallery_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_grid_gallery_refresh(esp_gsp_handle_t gsp, esp_gsp_grid_t grid)
esp_err_t gsp_widget_grid_gallery_set_total(esp_gsp_handle_t gsp, esp_gsp_grid_t grid, uint32_t total)
esp_gsp_config_t gsp_grid_docs_config(void)
esp_gsp_grid_t gsp_widget_grid_gallery_bind( esp_gsp_handle_t gsp, esp_gsp_grid_bind_cb_t bind_item, void *user_ctx)
esp_gsp_widget_t gsp_widget_grid_preview_cell_create( esp_gsp_handle_t gsp, int16_t x, int16_t y)
size_t gsp_grid_docs_dynamic_image_slots(void)
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
| `cell_template` | `identifier` | yes | — | — | recycled grid cell template |
| `column_count` | `int` | yes | 1…32 | — | fixed grid column count |
| `column_gap` | `int` | — | default 0; 0…4096 | — | horizontal cell gap |
| `row_gap` | `int` | — | default 0; 0…4096 | — | vertical cell gap |
| `scroll_snapshot` | `bool` | — | default `false` | — | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `item_count` | `int` | — | default 0; 0…4294967295 | — | initial grid item count |

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
| `opacity` | `int` | — | default 255; 0…255 | scene: —; template: — | 0-255 blend opacity |
| `bg_color` | `color` | — | — | yes | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | — | — | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | — | default vertical; `vertical`, `horizontal` | — | gradient direction |
| `radius` | `int` | — | default 0; 0…65535 | scene: —; template: — | corner radius in px |
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
