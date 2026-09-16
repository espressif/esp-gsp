# ESP-GSP Authoring Reference

GSPC version: 0.4.1.

Declare runtime-update fields using the dynamic forms listed in the tables. See [dynamic property scope](../guide/scenes.md#dynamic-property-scope) for declaration forms and object-versus-subtree behavior.

## Scene keys

| Key | Meaning |
|---|---|
| `screen` | scene name (drives generated symbol/file stems) |
| `w` | logical width in px |
| `h` | logical height in px |
| `screen_bg` | background color behind every object |
| `font` | default TTF/OTF path for baked text |
| `default_font_size` | default font pixel size (default 16) |
| `font_charset` | additional characters for runtime text; static text is included automatically |
| `font_charset_file` | UTF-8 character corpus path relative to the scene; objects can override it |
| `font_max_bytes` | optional positive byte limit for all generated GFB resources in this scene |
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
| `release` | pointer released; slider/arc call receives the final authored value, including releases outside the control |
| `long` | ordinary control held for 500 ms without dragging; fires once and suppresses click on release |
| `value` | slider/arc value changed; call receives the committed value in authored min/max units |

## Actions

| Action | Description |
|---|---|
| `show` | make the target object visible |
| `hide` | make the target object hidden |
| `toggle` | toggle the target object's visibility |
| `set_text` | replace the target text bind with `param` |
| `set_bg_color` | set the target color bind: arg is #RRGGBB (converted for the profile), or a legacy profile-native integer; alpha is not accepted |
| `call` | invoke the named application callback |
| `goto` | show a target layer or navigate to a scene id |
| `back` | rejected; use call with callback back, or stack_pop |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `w` | int | **required** | width in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `h` | int | **required** | height in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(bounded: scene=own_fill, template=own_fill)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `clip_children` | bool | `false` | clip descendants and their hit areas to the container bounds |

### `label`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `pressed_image` | path | — | pressed-state image path |
| `selected_image` | path | — | selected-state image path |
| `disabled_image` | path | — | disabled-state image path |
| `checkable` | bool | `false` | toggle selected state on click |

### `image`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] *(bounded: scene=svg, template=unsupported)* |
| `h` | int | **required** | height in px [0..65535] *(bounded: scene=svg, template=unsupported)* |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=image)* |
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
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `w` | int | **required** | width in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `h` | int | **required** | height in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `name` | identifier | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `fg_color` | color | — | foreground color (text/knob/line/mark per type) |
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(bounded: scene=own_fill, template=own_fill)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] |
| `y` | int | `0` | y relative to parent [-32768..32767] |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `points` | int_list | — | single-series chart data points |
| `series` | chart_series | — | fixed-capacity chart series objects |
| `min` | int | `0` | business value range lower bound |
| `max` | int | `100` | business value range upper bound |
| `grid_lines` | int | `0` | chart grid line count [0..32] |

### `slider`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `value` | int | — | initial value (in min..max units) *(dynamic)* |
| `min` | int | `0` | value range lower bound [-2147483648..2147483647] |
| `max` | int | `100` | value range upper bound [-2147483648..2147483647] |
| `vertical` | bool | `false` | vertical orientation |
| `start_angle` | int | `135` | arc start angle clockwise from top (0 top, 90 right) [0..359] *(bounded: scene=supported, template=unsupported)* |
| `sweep` | int | `270` | clockwise arc span in degrees [1..360] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |

### `effect`

Supports events: `click`, `press`, `release`, `long`

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
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `runtime_style` | bool | `false` | generate runtime appearance setters |
| `effect` | enum(`pulse`/`ring`/`liquid`/`shimmer`) | **required** | procedural visual |
| `shimmer_style` | enum(`linear`/`soft`/`diagonal`) | `"linear"` | highlight appearance |
| `fg_color` | color | `"#38BDF8"` | effect color *(runtime_style)* |
| `bg_color` | color | — | optional static backing color |
| `value` | int | `100` | liquid level or visual intensity in percent [0..100] |
| `period_ms` | int | `2800` | one animation cycle in milliseconds [100..60000] *(runtime_style)* |
| `playing` | bool | `true` | advance automatically while visible |
| `phase` | int | `0` | initial cycle progress, 0..65535 [0..65535] |
| `ring_style` | enum(`solid`/`glow`/`trail`) | `"trail"` | ring appearance |
| `softness` | int | `75` | glow feather softness in percent [0..100] *(runtime_style)* |
| `strength` | int | `65` | soft glow strength in percent [0..100] *(runtime_style)* |
| `opacity` | int | `255` | overall effect opacity [0..255] *(runtime_style)* |

### `charging_orb`

Supports events: `click`, `press`, `release`, `long`

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
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `runtime_style` | bool | `false` | generate runtime appearance setters |
| `fg_color` | color | `"#38BDF8"` | effect color *(runtime_style)* |
| `bg_color` | color | — | optional static backing color |
| `value` | int | `100` | liquid level or visual intensity in percent [0..100] |
| `period_ms` | int | `2800` | one animation cycle in milliseconds [100..60000] *(runtime_style)* |
| `playing` | bool | `true` | advance automatically while visible |
| `phase` | int | `0` | initial cycle progress, 0..65535 [0..65535] |
| `softness` | int | `75` | glow feather softness in percent [0..100] *(runtime_style)* |
| `strength` | int | `65` | soft glow strength in percent [0..100] *(runtime_style)* |
| `opacity` | int | `255` | overall effect opacity [0..255] *(runtime_style)* |
| `ripple` | int | `45` | attached membrane ripple strength in percent [0..100] *(runtime_style)* |
| `style` | enum(`liquid`/`halo`/`plasma`) | `"halo"` | charging orb visual style |
| `charging` | bool | `true` | enable charging motion and particles |
| `particles` | int | `4` | maximum soft energy particles [0..6] *(runtime_style)* |

### `carousel`

Supports events: `click`, `press`, `release`, `long`

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
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `runtime_style` | bool | `false` | generate runtime appearance setters |
| `image_sets` | string_matrix | — | additional image groups for runtime theme selection |
| `image_set` | int | `0` | initial compiled image group index [0..3] *(runtime_style)* |
| `icons` | string_list | **required** | 3..12 image paths, compiled to native thumbnails |
| `projection` | enum(`flat`/`tilt`/`perspective`) | `"perspective"` | carousel projection |
| `depth` | int | `60` | carousel perspective strength in percent [0..100] *(runtime_style)* |
| `spacing` | int | `50` | carousel orbit spacing in percent [0..100] *(runtime_style)* |
| `period_ms` | int | `280` | carousel settling duration with runtime_style [100..60000] *(runtime_style)* |
| `selected` | int | `0` | initial selected icon index [0..11] |
| `opacity` | int | `255` | overall icon opacity [0..255] *(runtime_style)* |

### `flip_card`

Supports events: `click`, `press`, `release`, `long`

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
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `runtime_style` | bool | `false` | generate runtime appearance setters |
| `image_sets` | string_matrix | — | additional image groups for runtime theme selection |
| `image_set` | int | `0` | initial compiled image group index [0..3] *(runtime_style)* |
| `front` | path | **required** | front image path |
| `back` | path | **required** | back image path |
| `flipped` | bool | `false` | show the back face *(dynamic)* |
| `depth` | int | `60` | perspective strength in percent [0..100] *(runtime_style)* |
| `period_ms` | int | `560` | flip duration in milliseconds [100..60000] *(runtime_style)* |
| `opacity` | int | `255` | overall opacity [0..255] *(runtime_style)* |

### `glass`

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
| `hidden` | bool | `false` | start hidden (show via actions or set_visible) *(dynamic)* |
| `backdrop` | string | **required** | name of a preceding static image with the same parent |
| `blur` | int | `12` | build-time blur radius in display pixels [0..32] |
| `tint_color` | color | `"#D8EAFF"` | frosted material tint |
| `tint_opacity` | int | `70` | tint strength [0..255] |
| `radius` | int | `16` | rounded corner radius in pixels [0..128] |
| `opacity` | int | `255` | overall opacity [0..255] |

### `toggle`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | `"#3f3f46"` | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `w` | int | **required** | width in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `h` | int | **required** | height in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(bounded: scene=own_fill, template=own_fill)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `pages` | string_list | — | ordered names of direct page children; overrides legacy &lt;name&gt;_tabN discovery |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `w` | int | **required** | width in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `h` | int | **required** | height in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(bounded: scene=own_fill, template=own_fill)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `w` | int | **required** | width in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `h` | int | **required** | height in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(bounded: scene=own_fill, template=own_fill)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=own_fill)* |
| `w` | int | **required** | width in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `h` | int | **required** | height in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] *(bounded: scene=own_fill, template=own_fill)* |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] *(bounded: scene=own_fill, template=own_fill)* |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
| `events` | action_list | — | input bindings: [{event, action, ...}] |
| `template` | identifier | — | declare this subtree as a render template |
| `max_instances` | int | — | maximum simultaneously live template instances; included in the automatic pool requirement [1..65535] |
| `dynamic_color` | bool | — | template member exposes a per-instance color slot |
| `dynamic_image` | bool | — | template image exposes a per-instance resource slot |
| `clip_children` | bool | `false` | clip descendants and their hit areas to the layer bounds |
| `block_scene_swipe` | bool | `false` | while visible, block horizontal scene swipes |

### `list`

Supports events: `click`, `press`, `release`, `long`

| Field | Type | Default | Description |
|---|---|---|---|
| `type` | string | **required** | widget type |
| `parent` | int | `-1` | parent object index (-1 = screen root) [-1..65534] |
| `parent_name` | string | — | parent by name instead of index |
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
| `scalable` | bool | `false` | enable runtime image scaling |
| `scale` | number | `1` | initial runtime image scale |
| `min_scale` | number | `0.5` | minimum runtime image scale |
| `max_scale` | number | `4` | maximum runtime image scale |
| `snap_to_item` | bool | `true` | scrolling snaps to row boundaries |
| `dynamic_items` | bool | `false` | keep item-update APIs available regardless of initial item count |
| `cyclic` | bool | `false` | selection wraps continuously across both ends |
| `items` | string_list | — | item texts (list/wheel/dropdown/tabview) |
| `selected` | int | `0` | initially selected item index [0..65535] *(dynamic)* |
| `item_height` | int | `0` | row height for list/wheel [0..65535] |
| `items_per_page` | int | `0` | tabview items per page [0..65535] |
| `visible_rows` | int | `0` | visible row count (alt to item_height) [0..65535] |
| `row_template` | identifier | — | recycled row template |
| `item_count` | int | — | initial dynamic item count [0..4294967295] |
| `enabled` | bool | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | color | `"#808080"` | disabled-state overlay color |
| `disabled_opacity` | int | `112` | disabled-state overlay opacity [0..255] |
| `bind` | identifier | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | enum(`visible`/`value`/`color`/`text`/`resource`) | — | explicit bind state family |
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
| `x` | int | `0` | x relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `y` | int | `0` | y relative to parent [-32768..32767] *(bounded: scene=supported, template=unsupported)* |
| `w` | int | **required** | width in px [0..65535] |
| `h` | int | **required** | height in px [0..65535] |
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
| `opacity` | int | `255` | 0-255 blend opacity [0..255] |
| `bg_color` | color | — | background/fill color (#RRGGBB or #RRGGBBAA) *(dynamic)* |
| `bg_gradient` | color | — | second gradient stop (with bg_color) |
| `gradient_dir` | enum(`vertical`/`horizontal`) | `"vertical"` | gradient direction |
| `radius` | int | `0` | corner radius in px [0..65535] |
| `border_color` | color | — | border stroke color |
| `border_width` | int | — | border stroke width (needs border_color) [0..65535] |
| `text` | string | — | static text content (UTF-8) *(dynamic)* |
| `text_align` | enum(`left`/`center`/`right`) | — | text alignment |
| `overflow` | enum(`clip`/`ellipsis`) | `"clip"` | single-line overflow |
| `font` | path | — | per-object TTF/OTF override |
| `font_size` | int | — | per-object font pixel size [1..255] |
| `font_charset` | string | — | glyphs available to runtime-bound text; static text is added automatically |
| `font_charset_file` | path | — | UTF-8 character corpus relative to the scene; combined with font_charset and static text |
| `font_link` | enum(`embedded`/`external`/`auto`) | — | font storage policy: embedded/external/auto |
| `input` | bool | `false` | text field: attaches the caret/keyboard flow |
| `animation_codec` | enum(`lossless`/`jpeg`/`hardware_jpeg`) | — | animation frame policy: lossless patches, JPEG full frames, or JPEG when the target has hardware decoding |
| `svg_layout` | enum(`content`/`canvas`) | — | SVG part placement: cropped content or original canvas |
| `morph_to` | path | — | SVG end shape with matching paths and paints |
| `morph` | int | — | SVG shape interpolation progress (percent); generates a runtime setter [0..100] |
| `svg_element` | string | — | SVG element id; imports its painted bounds as an independent image |
| `tint` | color | — | SVG silhouette color; generates a runtime color setter |
| `image` | path | — | image file path (raster or compiled SVG) *(dynamic)* |
| `codec` | enum(`raw`/`lossless`/`jpeg`/`auto`/`store`/`qoi`/`rle16`/`default`/`hardware_jpeg`) | — | image codec |
| `quality` | int | — | JPEG quality 1-100 (omitted = profile default) [1..100] |
| `compress` | bool | — | image compression toggle (legacy; prefer codec) |
| `store_scale` | number | — | pre-scale factor applied when encoding |
| `max_fps` | int | — | GIF/animation frame-rate cap (0 = uncapped) [1..120] |
| `fit` | enum(`stretch`/`fill`/`contain`/`cover`) | `"stretch"` | image fit mode |
| `position_x` | number | `0.5` | image fit horizontal alignment |
| `position_y` | number | `0.5` | image fit vertical alignment |
| `rotation` | int | `0` | clockwise image rotation around the bounding-box center; source alpha is supported; clipped to the box [-32768..32767] *(bounded: scene=image, template=unsupported)* |
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
| `callback` | identifier | — | app callback name; generates scene-qualified event helpers |
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
