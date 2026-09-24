# ESP-GSP Component Inventory

Current GSPC registry coverage.

## Summary

| Metric | Count |
|---|---:|
| Registered widgets | 28 |
| Compile-time composites | 6 |
| Total field definitions | 2495 |
| Dynamic capability fields | 236 |

## Components

| Component | Kind | Facets | Fields | Events |
|---|---|---|---:|---|
| `container` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `text`, `image`, `interaction`, `template` | 94 | yes |
| `label` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `text`, `image`, `interaction`, `template` | 93 | yes |
| `button` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `text`, `background_opacity`, `border`, `border_side`, `image`, `boolean`, `interaction`, `template` | 98 | yes |
| `image` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `image`, `text`, `interaction`, `template` | 94 | yes |
| `rect` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `text`, `image`, `interaction`, `template` | 93 | yes |
| `shape` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `shape`, `interaction`, `template` | 93 | yes |
| `progress` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `value`, `interaction`, `template` | 95 | yes |
| `chart` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `stroke`, `interaction`, `template` | 97 | yes |
| `slider` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `value`, `slider`, `interaction`, `template` | 98 | yes |
| `arc` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `value`, `arc`, `interaction`, `template` | 99 | yes |
| `needle` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `needle`, `stroke`, `interaction`, `template` | 94 | yes |
| `spinner` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `spinner`, `stroke`, `interaction`, `template` | 93 | yes |
| `effect` | `Widget` | `core`, `visibility`, `interaction` | 29 | yes |
| `charging_orb` | `Widget` | `core`, `visibility`, `interaction` | 30 | yes |
| `carousel` | `Widget` | `core`, `visibility`, `interaction` | 26 | yes |
| `flip_card` | `Widget` | `core`, `visibility`, `interaction` | 25 | yes |
| `glass` | `Widget` | `core`, `visibility` | 15 | no |
| `toggle` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `boolean`, `slider`, `interaction`, `template` | 95 | yes |
| `checkbox` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `boolean`, `interaction`, `template` | 92 | yes |
| `radio` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `boolean`, `interaction`, `template` | 92 | yes |
| `clock` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 99 | yes |
| `page_flow` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `text`, `image`, `data`, `interaction`, `template` | 106 | yes |
| `stackview` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `text`, `image`, `interaction`, `template` | 99 | yes |
| `drawer` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `text`, `image`, `interaction`, `template` | 95 | yes |
| `layer` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `background_opacity`, `border`, `border_side`, `text`, `image`, `interaction`, `template` | 95 | yes |
| `tabview` | `Composite` | `core` | 25 | yes |
| `dropdown` | `Composite` | `core` | 26 | yes |
| `msgbox` | `Composite` | `composite_core` | 21 | yes |
| `table` | `Composite` | `core` | 19 | yes |
| `keyboard` | `Composite` | `core` | 36 | yes |
| `list` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `data`, `list_data`, `scroll_snapshot`, `interaction`, `template` | 100 | yes |
| `wheel` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `wheel_behavior`, `data`, `list_data`, `interaction`, `template` | 101 | yes |
| `grid` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `grid_data`, `scroll_snapshot`, `interaction`, `template` | 97 | yes |
| `message_list` | `Composite` | `core`, `visibility`, `row_template`, `message_list` | 31 | yes |

## Fields by widget

### `container`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `clip_children` | `bool` | no | no | `false` | clip descendants and their hit areas to the container bounds |

### `label`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#FFFFFF"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `button`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `checked` | `bool` | no | yes | `false` | initial on/off state |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `pressed_image` | `path` | no | no | — | pressed-state image path |
| `selected_image` | `path` | no | no | — | selected-state image path |
| `disabled_image` | `path` | no | no | — | disabled-state image path |
| `checkable` | `bool` | no | no | `false` | toggle selected state on click |

### `image`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `image_opacity` | `int` | no | no | `255` | image-only opacity; fixed raster transforms may bake alpha at compile time |

### `rect`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `shape`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `shape` | `enum` | no | no | `"rect"` | shape geometry |
| `line_direction` | `enum` | no | no | `"tl_br"` | line direction |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `progress`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `value` | `int` | no | yes | — | initial value (in min..max units) |
| `min` | `int` | no | no | `0` | value range lower bound |
| `max` | `int` | no | no | `100` | value range upper bound |
| `vertical` | `bool` | no | no | `false` | vertical orientation |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `chart`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `thickness` | `int` | no | no | `0` | stroke thickness |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `points` | `int_list` | no | no | — | single-series chart data points |
| `series` | `chart_series` | no | no | — | fixed-capacity chart series objects |
| `min` | `int` | no | no | `0` | business value range lower bound |
| `max` | `int` | no | no | `100` | business value range upper bound |
| `grid_lines` | `int` | no | no | `0` | chart grid line count |

### `slider`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `value` | `int` | no | yes | — | initial value (in min..max units) |
| `min` | `int` | no | no | `0` | value range lower bound |
| `max` | `int` | no | no | `100` | value range upper bound |
| `vertical` | `bool` | no | no | `false` | vertical orientation |
| `knob_color` | `color` | no | no | `"#FFFFFF"` | slider/toggle knob color |
| `track_size` | `int` | no | no | `0` | track thickness |
| `knob` | `bool` | no | no | `true` | knob enabled |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `arc`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `value` | `int` | no | yes | — | initial value (in min..max units) |
| `min` | `int` | no | no | `0` | value range lower bound |
| `max` | `int` | no | no | `100` | value range upper bound |
| `vertical` | `bool` | no | no | `false` | vertical orientation |
| `start_angle` | `int` | no | yes | `135` | arc start angle clockwise from top (0 top, 90 right) |
| `sweep` | `int` | no | no | `270` | clockwise arc span in degrees |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `thickness` | `int` | no | no | — | stroke thickness; omitted = max(min(w,h)/8, 2); 0 clamps to 1 |
| `interactive` | `bool` | no | no | `true` | allow pointer interaction |

### `needle`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `angle` | `int` | no | no | `0` | clockwise angle (0=up) |
| `tail_length` | `int` | no | no | `0` | length behind center pivot in px |
| `thickness` | `int` | no | no | `0` | stroke thickness |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `spinner`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `speed` | `int` | no | no | `1000` | rotation speed in ms per revolution |
| `thickness` | `int` | no | no | `0` | stroke thickness |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `effect`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `runtime_style` | `bool` | no | no | `false` | generate runtime appearance setters |
| `effect` | `enum` | yes | no | — | procedural visual |
| `shimmer_style` | `enum` | no | no | `"linear"` | highlight appearance |
| `fg_color` | `color` | no | no | `"#38BDF8"` | effect color |
| `bg_color` | `color` | no | no | — | optional static backing color |
| `value` | `int` | no | no | `100` | liquid level or visual intensity in percent |
| `period_ms` | `int` | no | no | `2800` | one animation cycle in milliseconds |
| `playing` | `bool` | no | no | `true` | advance automatically while visible |
| `phase` | `int` | no | no | `0` | initial cycle progress, 0..65535 |
| `ring_style` | `enum` | no | no | `"trail"` | ring appearance |
| `softness` | `int` | no | no | `75` | glow feather softness in percent |
| `strength` | `int` | no | no | `65` | soft glow strength in percent |
| `opacity` | `int` | no | no | `255` | overall effect opacity |

### `charging_orb`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `runtime_style` | `bool` | no | no | `false` | generate runtime appearance setters |
| `fg_color` | `color` | no | no | `"#38BDF8"` | effect color |
| `bg_color` | `color` | no | no | — | optional static backing color |
| `value` | `int` | no | no | `100` | liquid level or visual intensity in percent |
| `period_ms` | `int` | no | no | `2800` | one animation cycle in milliseconds |
| `playing` | `bool` | no | no | `true` | advance automatically while visible |
| `phase` | `int` | no | no | `0` | initial cycle progress, 0..65535 |
| `softness` | `int` | no | no | `75` | glow feather softness in percent |
| `strength` | `int` | no | no | `65` | soft glow strength in percent |
| `opacity` | `int` | no | no | `255` | overall effect opacity |
| `ripple` | `int` | no | no | `45` | attached membrane ripple strength in percent |
| `style` | `enum` | no | no | `"halo"` | charging orb visual style |
| `charging` | `bool` | no | no | `true` | enable charging motion and particles |
| `particles` | `int` | no | no | `4` | maximum soft energy particles |

### `carousel`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `runtime_style` | `bool` | no | no | `false` | generate runtime appearance setters |
| `image_sets` | `string_matrix` | no | no | — | additional image groups for runtime theme selection |
| `image_set` | `int` | no | no | `0` | initial compiled image group index |
| `icons` | `string_list` | yes | no | — | 3..12 image paths, compiled to native thumbnails |
| `projection` | `enum` | no | no | `"perspective"` | carousel projection |
| `depth` | `int` | no | no | `60` | carousel perspective strength in percent |
| `spacing` | `int` | no | no | `50` | carousel orbit spacing in percent |
| `period_ms` | `int` | no | no | `280` | carousel settling duration with runtime_style |
| `selected` | `int` | no | no | `0` | initial selected icon index |
| `opacity` | `int` | no | no | `255` | overall icon opacity |

### `flip_card`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `runtime_style` | `bool` | no | no | `false` | generate runtime appearance setters |
| `image_sets` | `string_matrix` | no | no | — | additional image groups for runtime theme selection |
| `image_set` | `int` | no | no | `0` | initial compiled image group index |
| `front` | `path` | yes | no | — | front image path |
| `back` | `path` | yes | no | — | back image path |
| `flipped` | `bool` | no | yes | `false` | show the back face |
| `depth` | `int` | no | no | `60` | perspective strength in percent |
| `period_ms` | `int` | no | no | `560` | flip duration in milliseconds |
| `opacity` | `int` | no | no | `255` | overall opacity |

### `glass`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `backdrop` | `string` | yes | no | — | name of a preceding static image with the same parent |
| `blur` | `int` | no | no | `12` | build-time blur radius in display pixels |
| `tint_color` | `color` | no | no | `"#D8EAFF"` | frosted material tint |
| `tint_opacity` | `int` | no | no | `70` | tint strength |
| `radius` | `int` | no | no | `16` | rounded corner radius in pixels |
| `opacity` | `int` | no | no | `255` | overall opacity |

### `toggle`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#22c55e"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | `"#3f3f46"` | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `checked` | `bool` | no | yes | `false` | initial on/off state |
| `knob_color` | `color` | no | no | `"#FFFFFF"` | slider/toggle knob color |
| `track_size` | `int` | no | no | `0` | track thickness |
| `knob` | `bool` | no | no | `true` | knob enabled |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `checkbox`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#50B878"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `checked` | `bool` | no | yes | `false` | initial on/off state |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `radio`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#55A0E8"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `checked` | `bool` | no | yes | `false` | initial on/off state |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `clock`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `hour_angle` | `int` | no | no | `0` | hour-hand angle |
| `minute_angle` | `int` | no | no | `0` | minute-hand angle |
| `second_angle` | `int` | no | no | `0` | second-hand angle |
| `hour_color` | `color` | no | no | — | hour-hand color |
| `minute_color` | `color` | no | no | — | minute-hand color |
| `second_color` | `color` | no | no | — | second-hand color |
| `tick_color` | `color` | no | no | — | hour tick color |
| `hand_thickness` | `int` | no | no | `0` | base hand thickness |

### `page_flow`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `items` | `string_list` | no | no | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | `int` | no | yes | `0` | initially selected item index |
| `item_height` | `int` | no | no | `0` | row height for list/wheel |
| `items_per_page` | `int` | no | no | `0` | tabview items per page |
| `visible_rows` | `int` | no | no | `0` | visible row count (alt to item_height) |
| `snap_to_item` | `bool` | no | no | `false` | scrolling snaps to row boundaries |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `pages` | `string_list` | no | no | — | ordered names of direct page children; overrides legacy &lt;name&gt;_tabN discovery |
| `page_count` | `int` | no | no | — | compiled page count |
| `axis` | `enum` | no | no | `"horizontal"` | page motion axis |
| `bar_height` | `int` | no | no | `56` | tab bar height |
| `cyclic` | `bool` | no | no | `false` | cyclic page wrap |
| `stop_anywhere` | `bool` | no | no | `false` | allow PageFlow to settle between pages |
| `page_extent` | `int` | no | no | `0` | PageFlow drag extent |

### `stackview`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `page_count` | `int` | no | no | — | compiled page count |
| `initial_page` | `int` | no | no | `0` | initial stack root page |
| `capacity` | `int` | no | no | `8` | maximum stack depth |
| `axis` | `enum` | no | no | `"horizontal"` | motion axis |
| `transition_ms` | `int` | no | no | `0` | programmatic push/pop duration; 0 keeps runtime default |
| `transition_easing` | `enum` | no | no | `"linear"` | programmatic push/pop easing curve |

### `drawer`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `edge` | `enum` | no | no | `"top"` | drawer attachment edge |
| `open` | `bool` | no | no | `false` | initial drawer state |

### `layer`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `bg_opacity` | `int` | no | no | `255` | background-only opacity; multiplied by opacity and color alpha |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `border_side` | `enum` | no | no | `"all"` | inside border selection; partial sides require static rectangular geometry |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `clip_children` | `bool` | no | no | `false` | clip descendants and their hit areas to the layer bounds |
| `block_scene_swipe` | `bool` | no | no | `false` | while visible, block horizontal scene swipes |

### `tabview`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | no | yes | — | tabview background |
| `fg_color` | `color` | no | no | — | tab label color |
| `radius` | `int` | no | yes | `0` | corner radius |
| `hidden` | `bool` | no | yes | `false` | start hidden |
| `tabs` | `string_list` | yes | no | — | tab labels |
| `bar_height` | `int` | no | no | `56` | tab bar height |
| `active_color` | `color` | no | no | — | active indicator color |
| `axis` | `enum` | no | no | `"horizontal"` | page axis |
| `cyclic` | `bool` | no | no | `true` | cyclic page wrap |
| `stop_anywhere` | `bool` | no | no | `false` | allow PageFlow to settle between pages |
| `page_extent` | `int` | no | no | `0` | PageFlow drag extent |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `selected` | `int` | no | yes | `0` | initially selected tab |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |

### `dropdown`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | no | yes | — | dropdown background |
| `fg_color` | `color` | no | no | — | dropdown text color |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `radius` | `int` | no | yes | `0` | corner radius |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `options` | `string_list` | yes | no | — | option labels |
| `items` | `string_list` | no | no | — | normalized dropdown option labels |
| `item_height` | `int` | no | no | — | item row height |
| `open_direction` | `enum` | no | no | `"down"` | direction in which the option panel opens |
| `selected` | `int` | no | yes | `0` | initially selected index |
| `callback` | `identifier` | no | no | — | selection callback |
| `panel_color` | `color` | no | no | — | panel fill color |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |

### `msgbox`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `title` | `string` | no | no | — | dialog title |
| `text` | `string` | no | yes | — | dialog body |
| `buttons` | `string_list` | yes | no | — | button labels |
| `callback` | `identifier` | no | no | — | button press callback |
| `bg_color` | `color` | no | yes | — | dialog background |
| `fg_color` | `color` | no | no | — | dialog text color |
| `button_color` | `color` | no | no | — | button fill color |
| `radius` | `int` | no | yes | `0` | corner radius |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `hidden` | `bool` | no | yes | `true` | start hidden |
| `dismissable` | `bool` | no | no | `false` | scrim tap closes |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |

### `table`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | no | yes | — | table background |
| `fg_color` | `color` | no | no | — | table text color |
| `radius` | `int` | no | yes | `0` | corner radius |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `columns` | `string_list` | yes | no | — | column labels |
| `rows` | `string_matrix` | yes | no | — | cell rows |
| `col_widths` | `number_list` | no | no | — | relative column widths |
| `row_height` | `int` | no | no | `44` | row height |
| `header_color` | `color` | no | no | — | header fill color |
| `grid_color` | `color` | no | no | — | grid line color |

### `keyboard`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | no | yes | — | keyboard background |
| `fg_color` | `color` | no | no | — | keyboard text color |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `hidden` | `bool` | no | yes | `false` | start hidden |
| `key_color` | `color` | no | no | — | keyboard character-key fill color |
| `function_color` | `color` | no | no | — | keyboard modifier-key fill color |
| `function_text_color` | `color` | no | no | — | keyboard modifier-key text color |
| `delete_color` | `color` | no | no | — | keyboard delete-key fill color |
| `delete_text_color` | `color` | no | no | — | keyboard delete-key text color |
| `ok_color` | `color` | no | no | — | keyboard confirmation-key fill color |
| `ok_text_color` | `color` | no | no | — | keyboard confirmation-key text color |
| `space_color` | `color` | no | no | — | keyboard space-key fill color |
| `space_text_color` | `color` | no | no | — | keyboard space-key text color |
| `key_radius` | `int` | no | no | — | keyboard key corner radius in px |
| `shift_label` | `string` | no | no | — | keyboard shift-key label |
| `delete_label` | `string` | no | no | — | keyboard delete-key label |
| `ok_label` | `string` | no | no | — | keyboard confirmation-key label |
| `symbols_label` | `string` | no | no | — | keyboard symbols-page key label |
| `letters_label` | `string` | no | no | — | keyboard letters-page key label |
| `space_label` | `string` | no | no | — | keyboard space-key label |
| `function_font_size` | `int` | no | no | — | keyboard text modifier-key font size |
| `shift_icon` | `path` | no | no | — | keyboard shift-key icon image |
| `delete_icon` | `path` | no | no | — | keyboard delete-key icon image |
| `ok_icon` | `path` | no | no | — | keyboard confirmation-key icon image |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |

### `list`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `items` | `string_list` | no | no | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | `int` | no | yes | `0` | initially selected item index |
| `item_height` | `int` | no | no | `0` | row height for list/wheel |
| `items_per_page` | `int` | no | no | `0` | tabview items per page |
| `visible_rows` | `int` | no | no | `0` | visible row count (alt to item_height) |
| `snap_to_item` | `bool` | no | no | `false` | scrolling snaps to row boundaries |
| `row_template` | `identifier` | no | no | — | recycled row template |
| `item_count` | `int` | no | no | — | initial dynamic item count |
| `scroll_snapshot` | `bool` | no | no | `false` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `wheel`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `snap_to_item` | `bool` | no | no | `true` | scrolling snaps to row boundaries |
| `dynamic_items` | `bool` | no | no | `false` | keep item-update APIs available regardless of initial item count |
| `cyclic` | `bool` | no | no | `false` | selection wraps continuously across both ends |
| `items` | `string_list` | no | no | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | `int` | no | yes | `0` | initially selected item index |
| `item_height` | `int` | no | no | `0` | row height for list/wheel |
| `items_per_page` | `int` | no | no | `0` | tabview items per page |
| `visible_rows` | `int` | no | no | `0` | visible row count (alt to item_height) |
| `row_template` | `identifier` | no | no | — | recycled row template |
| `item_count` | `int` | no | no | — | initial dynamic item count |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

### `grid`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `min_width` | `int` | no | no | — | compile-time minimum width in px |
| `max_width` | `int` | no | no | — | compile-time maximum width in px |
| `min_height` | `int` | no | no | — | compile-time minimum height in px |
| `max_height` | `int` | no | no | — | compile-time maximum height in px |
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `margin_left` | `int` | no | no | — | auto-layout leading margin on the x axis |
| `margin_right` | `int` | no | no | — | auto-layout trailing margin on the x axis |
| `margin_top` | `int` | no | no | — | auto-layout leading margin on the y axis |
| `margin_bottom` | `int` | no | no | — | auto-layout trailing margin on the y axis |
| `align_main` | `enum` | no | no | — | auto-layout main-axis placement |
| `align_cross` | `enum` | no | no | — | auto-layout cross-axis placement |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `shadow_color` | `color` | no | no | — | static hard-shadow color |
| `shadow_opacity` | `int` | no | no | `96` | static hard-shadow opacity |
| `shadow_offset_x` | `int` | no | no | — | static hard-shadow x offset |
| `shadow_offset_y` | `int` | no | no | — | static hard-shadow y offset |
| `shadow_spread` | `int` | no | no | — | static hard-shadow spread in px |
| `shadow_radius` | `int` | no | no | — | static hard-shadow corner radius |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `border_opacity` | `int` | no | no | `255` | border stroke opacity |
| `outline_color` | `color` | no | no | — | outside outline color |
| `outline_width` | `int` | no | no | `0` | outside outline width |
| `outline_opacity` | `int` | no | no | `255` | outside outline opacity |
| `outline_pad` | `int` | no | no | `0` | gap between the element and its outline |
| `text_line_space` | `int` | no | no | `0` | extra spacing between static text rows in px |
| `text_vertical_align` | `enum` | no | no | `"auto"` | static text block placement; auto preserves single-line center and multiline top |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | `path` | no | no | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | `enum` | no | no | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | `enum` | no | no | — | SVG part placement: cropped content or original canvas |
| `morph_to` | `path` | no | no | — | SVG end shape with matching paths and paints |
| `morph` | `int` | no | no | — | SVG shape interpolation progress (percent); generates a runtime setter |
| `svg_element` | `string` | no | no | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | `color` | no | no | — | SVG silhouette color; generates a runtime color setter |
| `image` | `path` | no | yes | — | image file path (raster or compiled SVG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (omitted = profile default) |
| `jpeg_quality` | `int` | no | no | — | legacy JPEG quality alias |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `cache_policy` | `enum` | no | no | — | image cache policy: mmap_direct, decode_lru or preload |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `rotation` | `int` | no | yes | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `cell_template` | `identifier` | yes | no | — | recycled grid cell template |
| `column_count` | `int` | yes | no | — | fixed grid column count |
| `column_gap` | `int` | no | no | `0` | horizontal cell gap |
| `row_gap` | `int` | no | no | `0` | vertical cell gap |
| `scroll_snapshot` | `bool` | no | no | `false` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `item_count` | `int` | no | no | `0` | initial grid item count |

### `message_list`

| Field | Kind | Required | Dynamic | Default | Description |
|---|---|---|---|---|---|
| `type` | `string` | yes | no | — | widget type |
| `parent` | `int` | yes | no | `-1` | parent object index (-1 = screen root) |
| `parent_name` | `string` | no | no | — | parent by name instead of index |
| `x` | `int` | yes | yes | `0` | x relative to parent |
| `y` | `int` | yes | yes | `0` | y relative to parent |
| `w` | `int` | yes | yes | — | width in px |
| `h` | `int` | yes | yes | — | height in px |
| `name` | `identifier` | no | no | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `row_template` | `identifier` | no | no | — | recycled row template |
| `background_color` | `color` | no | no | — | message viewport background |
| `incoming_color` | `color` | no | no | `"#E9EDF3"` | incoming bubble color |
| `outgoing_color` | `color` | no | no | `"#246BFD"` | outgoing bubble color |
| `message_text_color` | `color` | no | no | `"#111827"` | message text color |
| `outgoing_text_color` | `color` | no | no | `"#FFFFFF"` | outgoing text color |
| `bubble_radius` | `int` | no | no | `14` | message bubble radius |
| `bubble_padding_x` | `int` | no | no | `14` | bubble horizontal padding |
| `bubble_padding_y` | `int` | no | no | `10` | bubble vertical padding |
| `message_gap` | `int` | no | no | `8` | message gap |
| `side_margin` | `int` | no | no | `12` | message side margin |
| `max_bubble_width` | `int` | no | no | `0` | maximum bubble width; 0 or omitted uses 76% of viewport width |
| `max_message_height` | `int` | no | no | `320` | maximum message height |
| `callback` | `identifier` | no | no | — | app callback name |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound message text |
| `scroll_snapshot` | `bool` | no | no | `true` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `item_height` | `int` | no | no | `0` | minimum message row height; 0 or omitted uses 2 * bubble_padding_y + message_gap + 1 |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
