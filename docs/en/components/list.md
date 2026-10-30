# List (`gsp_list`)

A scrollable vertical collection that supports both fixed items and application-driven data sources.

## When to use

Use List when you need to display or select from a collection of repeated items in a single column — for example, a contacts list or a settings menu.

## Local interactive preview

First [set up the commands from the compatibility guide](../reference/compatibility.md#tool-commands), then run from an unpacked component or public repository root with `gspc` and `gsp_sim_host`:

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/list/list.json \
  --deployable -o gsp-out/widget-preview/list.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/list.gspb --frames 0
```

These commands compile the example and open it in the ESP-GSP simulator browser preview.

## Runtime behavior

Short fixed `items` lists that fit the viewport are drawn directly. When fixed
items exceed the visible row count, GSPC emits a recycled row template: call its
generated bind helper once with `(ui, NULL, NULL)` to supply the compiled text.
An explicit `row_template` uses an application row binder. Retain the returned
handle across scene changes; use `set_total` and `refresh` instead of rebinding.

Generated rows paint at the List's position in scene order. Foreground objects declared later in the scene (such as a Drawer) will cover the rows. If you need an overlay to stay above the List, place its content after the List in the scene.

Give any object that your application code needs to read or update a stable `name`. GSPC generates the typed functions listed below for each named object.

## Example scene

```json
{"screen":"widget_list","w":480,"h":320,"screen_bg":"#101827","font":"../../../common/fonts/DejaVuSans.ttf","objects":[{"type":"list","parent":-1,"name":"devices","x":65,"y":25,"w":350,"h":270,"items":["Living room display","Kitchen sensor","Bedroom lamp","Studio speaker","Front door camera","Garage controller","Garden monitor"],"item_height":48,"snap_to_item":true,"font_size":16,"bg_color":"#142033","fg_color":"#E2E8F0","border_color":"#2D425F","border_width":1,"radius":12}]}
```

The canonical example is [`list.json`](../../../examples/usage/widgets/list/list.json). Copy any relative assets referenced by the scene with it.

## Generated C API

```c
const gsp_component_directory_t *const * gsp_list_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_list_devices_animate_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_list_devices_animate_selected_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_list_devices_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_list_devices_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_list_devices_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_widget_list_devices_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_list_devices_play_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_widget_list_devices_refresh(esp_gsp_handle_t gsp, esp_gsp_list_t list)
esp_err_t gsp_widget_list_devices_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_widget_list_devices_set_text( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, const char *utf8)
esp_err_t gsp_widget_list_devices_set_total(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t total)
esp_err_t gsp_widget_list_devices_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_widget_list_devices_stop_selected(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_list_docs_config(void)
esp_gsp_err_t gsp_widget_list_devices_row_set_text( esp_gsp_handle_t gsp, esp_gsp_row_t row, const char *utf8)
esp_gsp_list_t gsp_widget_list_devices_bind( esp_gsp_handle_t gsp, esp_gsp_list_bind_cb_t bind_item, void *user_ctx)
esp_gsp_widget_t gsp_widget_list_devices_create( esp_gsp_handle_t gsp, int16_t x, int16_t y)
size_t gsp_list_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields in this example

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
| `border_color` | `color` | — | — | — | border stroke color |
| `border_width` | `int` | — | 0…65535 | — | border stroke width (needs border_color) |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `items` | `string_list` | — | — | — | item texts (list/wheel/dropdown/tabview) |
| `item_height` | `int` | — | default 0; 0…65535 | — | row height for list/wheel |
| `snap_to_item` | `bool` | — | default `false` | — | scrolling snaps to row boundaries |

<details>
<summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `min_width` | `int` | — | 0…65535 | — | compile-time minimum width in px |
| `max_width` | `int` | — | 0…65535 | — | compile-time maximum width in px |
| `min_height` | `int` | — | 0…65535 | — | compile-time minimum height in px |
| `max_height` | `int` | — | 0…65535 | — | compile-time maximum height in px |
| `layout` | `enum` | — | `row`, `column`, `grid` | — | child auto-layout: row/column/grid |
| `layout_wrap` | `bool` | — | default `false` | — | wrap row/column children into additional lines |
| `grid_columns` | `int` | — | 1…65535 | — | equal-width column count for grid layout |
| `grid_column_span` | `int` | — | default 1; 1…65535 | — | column span in the parent grid layout |
| `gap` | `int` | — | default 0; 0…4096 | — | auto-layout gap in px |
| `padding` | `int` | — | default 0; 0…4096 | — | auto-layout padding in px |
| `padding_left` | `int` | — | 0…4096 | — | row layout: leading padding override |
| `padding_right` | `int` | — | 0…4096 | — | row layout: trailing padding override |
| `padding_top` | `int` | — | 0…4096 | — | column layout: leading padding override |
| `padding_bottom` | `int` | — | 0…4096 | — | column layout: trailing padding override |
| `grow` | `int` | — | default 0; 0…100 | — | auto-layout grow weight |
| `margin` | `int` | — | default 0; 0…4096 | — | auto-layout space on both child sides |
| `margin_left` | `int` | — | 0…4096 | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | — | 0…4096 | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | — | 0…4096 | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | — | 0…4096 | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | — | `start`, `center`, `end`, `space_between` | — | auto-layout main-axis placement |
| `align_cross` | `enum` | — | `start`, `center`, `end`, `stretch` | — | auto-layout cross-axis placement |
| `hidden` | `bool` | — | default `false` | yes | start hidden (show via actions or set_visible) |
| `opacity` | `int` | — | default 255; 0…255 | scene: —; template: — | 0-255 blend opacity |
| `bg_gradient` | `color` | — | — | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | — | default vertical; `vertical`, `horizontal` | — | gradient direction |
| `shadow_color` | `color` | — | — | — | static hard-shadow color |
| `shadow_opacity` | `int` | — | default 96; 0…255 | — | static hard-shadow opacity |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | static hard-shadow y offset |
| `shadow_spread` | `int` | — | 0…4096 | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | — | 0…65535 | — | static hard-shadow corner radius |
| `border_opacity` | `int` | — | default 255; 0…255 | — | border stroke opacity |
| `outline_color` | `color` | — | — | — | outside outline color |
| `outline_width` | `int` | — | default 0; 0…65535 | — | outside outline width |
| `outline_opacity` | `int` | — | default 255; 0…255 | — | outside outline opacity |
| `outline_pad` | `int` | — | default 0; 0…4096 | — | gap between the element and its outline |
| `text_line_space` | `int` | — | default 0; 0…4096 | — | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | — | default auto; `auto`, `top`, `center`, `bottom` | — | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | — | — | yes | static text content (UTF-8) |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | text alignment |
| `overflow` | `enum` | — | default clip; `clip`, `ellipsis` | — | single-line overflow |
| `font` | `path` | — | — | — | per-object TTF/OTF override |
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
| `image` | `path` | — | — | yes | image, animated image, video (optional FFmpeg), or compiled SVG file path |
| `video_start_ms` | `int` | — | 0…4294967295 | — | video clip start in milliseconds |
| `video_duration_ms` | `int` | — | 1…4294967295 | — | video clip duration in milliseconds; omitted imports to the end |
| `video_fit` | `enum` | — | `contain`, `cover`, `stretch` | — | build-time video aspect fit; contain pads with black |
| `video_loop_count` | `int` | — | 0…65535 | — | video playback count (default 1; 0 loops forever) |
| `video_max_decoded_bytes` | `int` | — | 1…9223372036854775807 | — | host retained RGBA pixel budget (default 256 MiB; not total process RSS) |
| `video_max_frames` | `int` | — | 1…65535 | — | maximum decoded video frames (default 4096) |
| `codec` | `enum` | — | `raw`, `lossless`, `jpeg`, `auto`, `speed`, `size`, `store`, `qoi`, `rle16`, `rle16_a8`, `rle32`, `default`, `hardware_jpeg` | — | image codec |
| `dither` | `bool` | — | — | — | reduce RGB565 banding in images and animations; overrides profile image_dither |
| `quality` | `int` | — | 1…100 | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | — | 1…100 | — | legacy JPEG quality alias |
| `compress` | `bool` | — | — | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | — | `mmap_direct`, `mmap`, `decode_lru`, `lru`, `preload` | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | — | 0.05…1.0 | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | — | 0…120 | — | animation import frame-rate cap; video defaults to 30, explicit 0 preserves source timestamps |
| `fit` | `enum` | — | default stretch; `stretch`, `fill`, `contain`, `cover` | — | image fit mode |
| `position_x` | `number` | — | default 0.5; 0.0…1.0 | — | image fit horizontal alignment |
| `position_y` | `number` | — | default 0.5; 0.0…1.0 | — | image fit vertical alignment |
| `rotation` | `int` | — | default 0; -32768…32767 | scene: image; template: — | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | — | default `false` | — | enable runtime image scaling |
| `scale` | `number` | — | default 1.0; 0.0625…16.0 | — | initial runtime image scale |
| `min_scale` | `number` | — | default 0.5; 0.0625…16.0 | — | minimum runtime image scale |
| `max_scale` | `number` | — | default 4.0; 0.0625…16.0 | — | maximum runtime image scale |
| `selected` | `int` | — | default 0; 0…65535 | yes | initially selected item index |
| `items_per_page` | `int` | — | default 0; 0…65535 | — | tabview items per page |
| `visible_rows` | `int` | — | default 0; 0…65535 | — | visible row count (alt to item_height) |
| `row_template` | `identifier` | — | — | — | recycled row template |
| `item_count` | `int` | — | 0…4294967295 | — | initial dynamic item count |
| `scroll_snapshot` | `bool` | — | default `false` | — | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |
| `bind` | `identifier` | — | — | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | explicit bind state family |
| `callback` | `identifier` | — | — | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | — | — | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | — | — | — | declare this subtree as a render template |
| `max_instances` | `int` | — | 1…65535 | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | — | — | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | — | — | — | template image exposes a per-instance resource slot |

</details>

<details>
<summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
