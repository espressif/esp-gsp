# ESP-GSP Component Inventory

Current GSPC registry coverage.

## Summary

| Metric | Count |
|---|---:|
| Registered widgets | 23 |
| Compile-time composites | 6 |
| Total field definitions | 1571 |
| Dynamic capability fields | 278 |

## Components

| Component | Kind | Facets | Fields | Events |
|---|---|---|---:|---|
| `container` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 58 | yes |
| `label` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 58 | yes |
| `button` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `text`, `border`, `image`, `interaction`, `template` | 58 | yes |
| `image` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `image`, `text`, `interaction`, `template` | 58 | yes |
| `rect` | `Widget` | `core`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 49 | yes |
| `shape` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `shape`, `interaction`, `template` | 60 | yes |
| `progress` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `value`, `interaction`, `template` | 62 | yes |
| `chart` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `stroke`, `interaction`, `template` | 61 | yes |
| `slider` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `value`, `slider`, `interaction`, `template` | 65 | yes |
| `arc` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `value`, `arc`, `interaction`, `template` | 65 | yes |
| `needle` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `needle`, `stroke`, `interaction`, `template` | 61 | yes |
| `spinner` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `spinner`, `stroke`, `interaction`, `template` | 60 | yes |
| `toggle` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `boolean`, `slider`, `interaction`, `template` | 62 | yes |
| `checkbox` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `boolean`, `interaction`, `template` | 59 | yes |
| `radio` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `boolean`, `interaction`, `template` | 59 | yes |
| `clock` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 66 | yes |
| `page_flow` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `data`, `interaction`, `template` | 70 | yes |
| `stackview` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 64 | yes |
| `drawer` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 60 | yes |
| `layer` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `interaction`, `template` | 59 | yes |
| `tabview` | `Composite` | `core` | 25 | yes |
| `dropdown` | `Composite` | `core` | 26 | yes |
| `msgbox` | `Composite` | `composite_core` | 21 | yes |
| `table` | `Composite` | `core` | 19 | yes |
| `keyboard` | `Composite` | `core` | 36 | yes |
| `list` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `data`, `list_data`, `scroll_snapshot`, `interaction`, `template` | 68 | yes |
| `wheel` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `data`, `list_data`, `interaction`, `template` | 67 | yes |
| `grid` | `Widget` | `core`, `layout`, `visibility`, `appearance`, `fill`, `border`, `text`, `image`, `grid_data`, `scroll_snapshot`, `interaction`, `template` | 64 | yes |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#FFFFFF"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

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
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `points` | `int_list` | no | no | — | chart data points |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `value` | `int` | no | yes | — | initial value (in min..max units) |
| `min` | `int` | no | no | `0` | value range lower bound |
| `max` | `int` | no | no | `100` | value range upper bound |
| `vertical` | `bool` | no | no | `false` | vertical orientation |
| `start_angle` | `int` | no | yes | `135` | arc start angle in degrees |
| `sweep` | `int` | no | no | `270` | arc sweep in degrees |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `thickness` | `int` | no | no | — | stroke thickness; omitted = max(min(w,h)/8, 2); 0 clamps to 1 |

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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |

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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#22c55e"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | `"#3f3f46"` | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#50B878"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | `"#55A0E8"` | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `items` | `string_list` | no | no | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | `int` | no | yes | `0` | initially selected item index |
| `item_height` | `int` | no | no | `0` | row height for list/wheel |
| `items_per_page` | `int` | no | no | `0` | tabview items per page |
| `visible_rows` | `int` | no | no | `0` | visible row count (alt to item_height) |
| `cyclic` | `bool` | no | no | `false` | wheel wraps around |
| `snap_to_item` | `bool` | no | no | `false` | scrolling snaps to row boundaries |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
| `page_count` | `int` | no | no | — | compiled page count |
| `axis` | `enum` | no | no | `"horizontal"` | page motion axis |
| `bar_height` | `int` | no | no | `56` | tab bar height |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | `action_list` | no | no | — | input bindings: [{event, action, ...}] |
| `template` | `identifier` | no | no | — | declare this subtree as a render template |
| `max_instances` | `int` | no | no | — | maximum simultaneously live template instances; included in the automatic pool requirement |
| `dynamic_color` | `bool` | no | no | — | template member exposes a per-instance color slot |
| `dynamic_image` | `bool` | no | no | — | template image exposes a per-instance resource slot |
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
| `hidden` | `bool` | no | yes | `false` | start hidden |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `items` | `string_list` | no | no | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | `int` | no | yes | `0` | initially selected item index |
| `item_height` | `int` | no | no | `0` | row height for list/wheel |
| `items_per_page` | `int` | no | no | `0` | tabview items per page |
| `visible_rows` | `int` | no | no | `0` | visible row count (alt to item_height) |
| `cyclic` | `bool` | no | no | `false` | wheel wraps around |
| `snap_to_item` | `bool` | no | no | `false` | scrolling snaps to row boundaries |
| `row_template` | `identifier` | no | no | — | recycled row template |
| `item_count` | `int` | no | no | — | initial dynamic item count |
| `scroll_snapshot` | `bool` | no | no | `false` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
| `scalable` | `bool` | no | no | `false` | enable runtime image scaling |
| `scale` | `number` | no | no | `1` | initial runtime image scale |
| `min_scale` | `number` | no | no | `0.5` | minimum runtime image scale |
| `max_scale` | `number` | no | no | `4` | maximum runtime image scale |
| `items` | `string_list` | no | no | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | `int` | no | yes | `0` | initially selected item index |
| `item_height` | `int` | no | no | `0` | row height for list/wheel |
| `items_per_page` | `int` | no | no | `0` | tabview items per page |
| `visible_rows` | `int` | no | no | `0` | visible row count (alt to item_height) |
| `cyclic` | `bool` | no | no | `false` | wheel wraps around |
| `snap_to_item` | `bool` | no | no | `false` | scrolling snaps to row boundaries |
| `row_template` | `identifier` | no | no | — | recycled row template |
| `item_count` | `int` | no | no | — | initial dynamic item count |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
| `bind` | `identifier` | no | no | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | no | no | — | explicit bind state family |
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `layout` | `enum` | no | no | — | child auto-layout: row/column |
| `gap` | `int` | no | no | `0` | auto-layout gap in px |
| `padding` | `int` | no | no | `0` | auto-layout padding in px |
| `padding_left` | `int` | no | no | — | row layout: leading padding override |
| `padding_right` | `int` | no | no | — | row layout: trailing padding override |
| `padding_top` | `int` | no | no | — | column layout: leading padding override |
| `padding_bottom` | `int` | no | no | — | column layout: trailing padding override |
| `grow` | `int` | no | no | `0` | auto-layout grow weight |
| `margin` | `int` | no | no | `0` | auto-layout space on both child sides |
| `hidden` | `bool` | no | yes | `false` | start hidden (show via actions or set_visible) |
| `fg_color` | `color` | no | no | — | foreground color (text/knob/line/mark per type) |
| `opacity` | `int` | no | yes | `255` | 0-255 blend opacity |
| `bg_color` | `color` | no | yes | — | background/fill color (#RRGGBB or #RRGGBBAA) |
| `bg_gradient` | `color` | no | no | — | second gradient stop (with bg_color) |
| `gradient_dir` | `enum` | no | no | `"vertical"` | gradient direction |
| `radius` | `int` | no | yes | `0` | corner radius in px |
| `border_color` | `color` | no | no | — | border stroke color |
| `border_width` | `int` | no | no | — | border stroke width (needs border_color) |
| `text` | `string` | no | yes | — | static text content (UTF-8) |
| `text_align` | `enum` | no | no | — | text alignment |
| `overflow` | `enum` | no | no | `"clip"` | single-line overflow |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | `enum` | no | no | — | font storage policy: embedded/external/auto |
| `input` | `bool` | no | no | `false` | text field: attaches the caret/keyboard flow |
| `image` | `path` | no | yes | — | image file path (PNG) |
| `codec` | `enum` | no | no | — | image codec |
| `quality` | `int` | no | no | — | JPEG quality 1-100 (0 = profile default) |
| `compress` | `bool` | no | no | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | `number` | no | no | — | pre-scale factor applied when encoding |
| `max_fps` | `int` | no | no | — | GIF/animation frame-rate cap (0 = uncapped) |
| `fit` | `enum` | no | no | `"stretch"` | image fit mode |
| `position_x` | `number` | no | no | `0.5` | image fit horizontal alignment |
| `position_y` | `number` | no | no | `0.5` | image fit vertical alignment |
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
| `callback` | `identifier` | no | no | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
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
| `bubble_radius` | `int` | no | no | — | message bubble radius |
| `bubble_padding_x` | `int` | no | no | — | bubble horizontal padding |
| `bubble_padding_y` | `int` | no | no | — | bubble vertical padding |
| `message_gap` | `int` | no | no | — | message gap |
| `side_margin` | `int` | no | no | — | message side margin |
| `max_bubble_width` | `int` | no | no | — | maximum bubble width |
| `max_message_height` | `int` | no | no | — | maximum message height |
| `callback` | `identifier` | no | no | — | app callback name |
| `font` | `path` | no | no | — | per-object TTF/OTF override |
| `font_size` | `int` | no | no | — | per-object font pixel size |
| `font_charset` | `string` | no | no | — | glyphs available to runtime-bound message text |
| `scroll_snapshot` | `bool` | no | no | `true` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `item_height` | `int` | no | no | `0` | row height |
| `enabled` | `bool` | no | no | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | no | no | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | `int` | no | no | `112` | disabled-state overlay opacity |
