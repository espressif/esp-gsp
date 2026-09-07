# ESP-GSP Authoring Reference

GSPC version: 0.3.0.

## Scene keys

| Key | Meaning |
|---|---|
| `screen` | scene name (drives generated symbol/file stems) |
| `w` | logical width in px |
| `h` | logical height in px |
| `screen_bg` | background color behind every object |
| `font` | default TTF/OTF path for baked text |
| `default_font_size` | default font pixel size (default 16) |
| `objects` | the object array (paint order = array order) |
| `styles` | named property-default sets; objects opt in via `style` |
| `themes` | named color tokens; static by default, dynamic with `dynamic: true` |
| `components` | user-defined parametric composites; instantiate with `{"use": <name>}` |
| `swipe` | allow horizontal-drag scene navigation (default true) |

## Events

| Event | Description |
|---|---|
| `click` | pointer activation completed on the object |
| `press` | pointer entered the object's pressed state |
| `release` | pointer was released from the object |
| `long` | long-press gesture completed on the object |
| `value` | compiler range-input route; not an automatic application callback |

## Actions

| Action | Description |
|---|---|
| `show` | make the target object visible |
| `hide` | make the target object hidden |
| `toggle` | toggle the target object's visibility |
| `set_text` | replace the target text bind with `param` |
| `set_bg_color` | set the target color bind from `arg` |
| `call` | invoke the named application callback |
| `goto` | show a target layer or navigate to a scene id |
| `back` | reserved; current public runtimes do not dispatch it (use call or stack_pop) |
| `set_value` | assign the target value bind |
| `toggle_value` | toggle the target value bind between zero and one |
| `add_value` | add `arg` to the target value bind |
| `set_page` | select a page in the target PageFlow |
| `stack_push` | push a page onto the target StackView |
| `stack_pop` | pop the current page from the target StackView |
| `drawer_open` | open the target drawer |
| `drawer_close` | close the target drawer |

## Widgets

### `container`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `label`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | `"#FFFFFF"` | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `button`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `image`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `rect`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `shape`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `shape` | enum(`rect`/`round_rect`/`circle`/`ellipse`/`line`) | `"rect"` | shape geometry |
| `line_direction` | enum(`horizontal`/`vertical`/`tl_br`/`bl_tr`) | `"tl_br"` | line direction |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `progress`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `value` | int | — | initial value (in min..max units) *(dynamic)* |
| `min` | int | `0` | value range lower bound [-2147483648..2147483647] |
| `max` | int | `100` | value range upper bound [-2147483648..2147483647] |
| `vertical` | bool | `false` | vertical orientation |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `chart`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `thickness` | int | `0` | stroke thickness [0..65535] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `points` | int_list | — | chart data points |
| `grid_lines` | int | `0` | chart grid line count [0..32] |

### `slider`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `value` | int | — | initial value (in min..max units) *(dynamic)* |
| `min` | int | `0` | value range lower bound [-2147483648..2147483647] |
| `max` | int | `100` | value range upper bound [-2147483648..2147483647] |
| `vertical` | bool | `false` | vertical orientation |
| `knob_color` | color | `"#FFFFFF"` | slider/toggle knob color |
| `track_size` | int | `0` | track thickness [0..4096] |
| `knob` | bool | `true` | knob enabled |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `arc`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `value` | int | — | initial value (in min..max units) *(dynamic)* |
| `min` | int | `0` | value range lower bound [-2147483648..2147483647] |
| `max` | int | `100` | value range upper bound [-2147483648..2147483647] |
| `vertical` | bool | `false` | vertical orientation |
| `start_angle` | int | `135` | arc start angle in degrees [0..359] *(dynamic)* |
| `sweep` | int | `270` | arc sweep in degrees [1..360] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `thickness` | int | — | stroke thickness; omitted = max(min(w,h)/8, 2); 0 clamps to 1 [0..65535] |

### `needle`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `angle` | int | `0` | clockwise angle (0=up) [0..359] |
| `tail_length` | int | `0` | length behind center pivot in px [0..65535] |
| `thickness` | int | `0` | stroke thickness [0..65535] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `spinner`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `speed` | int | `1000` | rotation speed in ms per revolution [100..60000] |
| `thickness` | int | `0` | stroke thickness [0..65535] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `toggle`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | `"#22c55e"` | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | `"#3f3f46"` | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `checked` | bool | `false` | initial on/off state *(dynamic)* |
| `knob_color` | color | `"#FFFFFF"` | slider/toggle knob color |
| `track_size` | int | `0` | track thickness [0..4096] |
| `knob` | bool | `true` | knob enabled |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `checkbox`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | `"#50B878"` | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `checked` | bool | `false` | initial on/off state *(dynamic)* |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `radio`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | `"#55A0E8"` | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `checked` | bool | `false` | initial on/off state *(dynamic)* |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `clock`

Supports events: `click`, `press`, `release`, `long`

A named clock generates `gsp_<scene>_<name>_set_time()`; the helper validates a 24-hour time and updates all three hands atomically.

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `hour_angle` | int | `0` | hour-hand angle [0..359] |
| `minute_angle` | int | `0` | minute-hand angle [0..359] |
| `second_angle` | int | `0` | second-hand angle [0..359] |
| `hour_color` | color | — | hour-hand color |
| `minute_color` | color | — | minute-hand color |
| `second_color` | color | — | second-hand color |
| `tick_color` | color | — | hour tick color |
| `hand_thickness` | int | `0` | base hand thickness [0..65535] |

### `page_flow`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `items` | string_list | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | int | `0` | initially selected item index [0..65535] *(dynamic)* |
| `item_height` | int | `0` | row height for list/wheel [0..65535] |
| `items_per_page` | int | `0` | tabview items per page [0..65535] |
| `visible_rows` | int | `0` | visible row count (alt to item_height) [0..65535] |
| `cyclic` | bool | `false` | wheel wraps around |
| `snap_to_item` | bool | `false` | scrolling snaps to row boundaries |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `page_count` | int | — | compiled page count [1..65535] |
| `axis` | enum(`horizontal`/`vertical`) | `"horizontal"` | page motion axis |
| `bar_height` | int | `56` | tab bar height [0..4096] |
| `stop_anywhere` | bool | `false` | allow PageFlow to settle between pages |
| `page_extent` | int | `0` | PageFlow drag extent [0..65535] |

### `stackview`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `page_count` | int | — | compiled page count [1..65535] |
| `initial_page` | int | `0` | initial stack root page [0..65534] |
| `capacity` | int | `8` | maximum stack depth [1..8] |
| `axis` | enum(`horizontal`/`vertical`) | `"horizontal"` | motion axis |
| `transition_ms` | int | `0` | programmatic push/pop duration; 0 keeps runtime default [0..65535] |
| `transition_easing` | enum(`linear`/`ease_out`/`ease_in_out`) | `"linear"` | programmatic push/pop easing curve |

### `drawer`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `edge` | enum(`top`/`bottom`/`left`/`right`) | `"top"` | drawer attachment edge |
| `open` | bool | `false` | initial drawer state |

### `layer`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `block_scene_swipe` | bool | `false` | while visible, block horizontal scene swipes |

### `list`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `items` | string_list | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | int | `0` | initially selected item index [0..65535] *(dynamic)* |
| `item_height` | int | `0` | row height for list/wheel [0..65535] |
| `items_per_page` | int | `0` | tabview items per page [0..65535] |
| `visible_rows` | int | `0` | visible row count (alt to item_height) [0..65535] |
| `cyclic` | bool | `false` | wheel wraps around |
| `snap_to_item` | bool | `false` | scrolling snaps to row boundaries |
| `row_template` | identifier | — | recycled row template |
| `item_count` | int | — | initial dynamic item count [0..4294967295] |
| `scroll_snapshot` | bool | `false` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `wheel`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `items` | string_list | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | int | `0` | initially selected item index [0..65535] *(dynamic)* |
| `item_height` | int | `0` | row height for list/wheel [0..65535] |
| `items_per_page` | int | `0` | tabview items per page [0..65535] |
| `visible_rows` | int | `0` | visible row count (alt to item_height) [0..65535] |
| `cyclic` | bool | `false` | wheel wraps around |
| `snap_to_item` | bool | `false` | scrolling snaps to row boundaries |
| `row_template` | identifier | — | recycled row template |
| `item_count` | int | — | initial dynamic item count [0..4294967295] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `grid`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(dynamic)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(dynamic)* |
| `w` | int | **required** | width in px [0..65535] *(dynamic)* |
| `h` | int | **required** | height in px [0..65535] *(dynamic)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | enum(`row`/`column`) | — | child auto-layout: row/column |
| `gap` | int | `0` | auto-layout gap in px [0..4096] |
| `padding` | int | `0` | auto-layout padding in px [0..4096] |
| `padding_left` | int | — | row layout: leading padding override [0..4096] |
| `padding_right` | int | — | row layout: trailing padding override [0..4096] |
| `padding_top` | int | — | column layout: leading padding override [0..4096] |
| `padding_bottom` | int | — | column layout: trailing padding override [0..4096] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `margin` | int | `0` | auto-layout space on both child sides [0..4096] |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(dynamic)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(dynamic)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `image` | path | — | image file path (PNG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (0 = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise opaque-image rotation around the bounding-box center; clipped to the box [-32768..32767] *(dynamic)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `cell_template` | identifier | **required** | recycled grid cell template |
| `column_count` | int | **required** | fixed grid column count [1..32] |
| `column_gap` | int | `0` | horizontal cell gap [0..4096] |
| `row_gap` | int | `0` | vertical cell gap [0..4096] |
| `scroll_snapshot` | bool | `false` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates GSP_ACT_ID_&lt;NAME&gt; |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `item_count` | int | `0` | initial grid item count [0..4294967295] |

## Compile-time composites

These objects expand into base widget types before scene validation.

### `tabview`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] |
| `y` | int | `0` | y relative to parent [-32768..32767] |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | color | — | tabview background |
| `fg_color` | color | — | tab label color |
| `radius` | int | `0` | corner radius [0..65535] |
| `hidden` | bool | `false` | start hidden |
| `tabs` | string_list | **required** | tab labels |
| `bar_height` | int | `56` | tab bar height [1..65535] |
| `active_color` | color | — | active indicator color |
| `axis` | enum(`horizontal`/`vertical`) | `"horizontal"` | page axis |
| `cyclic` | bool | `true` | cyclic page wrap |
| `stop_anywhere` | bool | `false` | allow PageFlow to settle between pages |
| `page_extent` | int | `0` | PageFlow drag extent [0..65535] |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `selected` | int | `0` | initially selected tab [0..65535] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |

### `dropdown`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] |
| `y` | int | `0` | y relative to parent [-32768..32767] |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | color | — | dropdown background |
| `fg_color` | color | — | dropdown text color |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width [0..65535] |
| `grow` | int | `0` | auto-layout grow weight [0..100] |
| `radius` | int | `0` | corner radius [0..65535] |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `options` | string_list | **required** | option labels |
| `items` | string_list | — | normalized dropdown option labels |
| `item_height` | int | — | item row height [0..65535] |
| `open_direction` | enum(`down`/`up`) | `"down"` | direction in which the option panel opens |
| `selected` | int | `0` | initially selected index [0..65535] |
| `callback` | identifier | — | selection callback |
| `panel_color` | color | — | panel fill color |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |

### `msgbox`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `title` | string | — | dialog title |
| `text` | string | — | dialog body |
| `buttons` | string_list | **required** | button labels |
| `callback` | identifier | — | button press callback |
| `bg_color` | color | — | dialog background |
| `fg_color` | color | — | dialog text color |
| `button_color` | color | — | button fill color |
| `radius` | int | `0` | corner radius [0..65535] |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `hidden` | bool | `false` | start hidden |
| `dismissable` | bool | `false` | scrim tap closes |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |

### `table`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] |
| `y` | int | `0` | y relative to parent [-32768..32767] |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | color | — | table background |
| `fg_color` | color | — | table text color |
| `radius` | int | `0` | corner radius [0..65535] |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `columns` | string_list | **required** | column labels |
| `rows` | string_matrix | **required** | cell rows |
| `col_widths` | number_list | — | relative column widths |
| `row_height` | int | `44` | row height [1..65535] |
| `header_color` | color | — | header fill color |
| `grid_color` | color | — | grid line color |

### `keyboard`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] |
| `y` | int | `0` | y relative to parent [-32768..32767] |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | color | — | keyboard background |
| `fg_color` | color | — | keyboard text color |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `hidden` | bool | `false` | start hidden |
| `key_color` | color | — | keyboard character-key fill color |
| `function_color` | color | — | keyboard modifier-key fill color |
| `function_text_color` | color | — | keyboard modifier-key text color |
| `delete_color` | color | — | keyboard delete-key fill color |
| `delete_text_color` | color | — | keyboard delete-key text color |
| `ok_color` | color | — | keyboard confirmation-key fill color |
| `ok_text_color` | color | — | keyboard confirmation-key text color |
| `space_color` | color | — | keyboard space-key fill color |
| `space_text_color` | color | — | keyboard space-key text color |
| `key_radius` | int | — | keyboard key corner radius in px [0..65535] |
| `shift_label` | string | — | keyboard shift-key label |
| `delete_label` | string | — | keyboard delete-key label |
| `ok_label` | string | — | keyboard confirmation-key label |
| `symbols_label` | string | — | keyboard symbols-page key label |
| `letters_label` | string | — | keyboard letters-page key label |
| `space_label` | string | — | keyboard space-key label |
| `function_font_size` | int | — | keyboard text modifier-key font size [1..255] |
| `shift_icon` | path | — | keyboard shift-key icon image |
| `delete_icon` | path | — | keyboard delete-key icon image |
| `ok_icon` | path | — | keyboard confirmation-key icon image |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |

### `message_list`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] |
| `y` | int | `0` | y relative to parent [-32768..32767] |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) |
| `row_template` | identifier | — | recycled row template |
| `background_color` | color | — | message viewport background |
| `incoming_color` | color | `"#E9EDF3"` | incoming bubble color |
| `outgoing_color` | color | `"#246BFD"` | outgoing bubble color |
| `message_text_color` | color | `"#111827"` | message text color |
| `outgoing_text_color` | color | `"#FFFFFF"` | outgoing text color |
| `bubble_radius` | int | — | message bubble radius [0..1024] |
| `bubble_padding_x` | int | — | bubble horizontal padding [0..1024] |
| `bubble_padding_y` | int | — | bubble vertical padding [0..1024] |
| `message_gap` | int | — | message gap [0..1024] |
| `side_margin` | int | — | message side margin [0..4096] |
| `max_bubble_width` | int | — | maximum bubble width [0..32767] |
| `max_message_height` | int | — | maximum message height [16..32767] |
| `callback` | identifier | — | app callback name |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound message text |
| `scroll_snapshot` | bool | `true` | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `item_height` | int | `0` | row height [0..65535] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
