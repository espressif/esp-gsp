# Scene JSON Guide

Scene JSON describes the build-time UI: logical size, hierarchy, appearance,
assets and interactions. Product state, I/O and display-driver setup remain in
application/BSP code. Use this guide for common decisions and the generated
[authoring reference](../reference/authoring.md) to look up every accepted field and default.

## From a description or reference image

Lock the logical resolution, available fonts/assets, dynamic content and user
actions first. For a screenshot or design image:

1. Treat it as a reference unless it is an actual product asset.
2. Split it into containers, labels, controls, shapes and reusable images;
   do not flatten the complete UI into one screenshot image.
3. Scale geometry when the reference and display resolutions
   differ.
4. Record assumptions for missing fonts, exact colors and interaction states.

A static image does not define pressed/disabled state, scrolling, navigation,
loading or error behavior; derive those from the product requirement.

## Minimal scene

```json
{
  "screen": "dashboard",
  "w": 320,
  "h": 240,
  "screen_bg": "#101820",
  "objects": [
    {
      "type": "container",
      "parent": -1,
      "name": "card",
      "x": 20, "y": 24, "w": 280, "h": 192,
      "bg_color": "#1B263B",
      "radius": 16
    },
    {
      "type": "progress",
      "parent_name": "card",
      "name": "load",
      "x": 20, "y": 64, "w": 240, "h": 18,
      "value": 30,
      "fg_color": "#4CC9F0"
    },
    {
      "type": "toggle",
      "parent_name": "card",
      "name": "power",
      "callback": "power_changed",
      "x": 108, "y": 124, "w": 64, "h": 32,
      "fg_color": "#4361EE"
    }
  ]
}
```

Required top-level fields are `screen`, `w`, `h` and a non-empty `objects`
array. Scene size is in logical pixels and must match the bundle/BSP display
path.

Ordinary parent references may point forward or backward in the object array.
Inherited hidden state and template membership do not depend on that ordering;
the compiler preserves authored draw order.

## Field decisions

| Purpose | Fields | Rule |
|---|---|---|
| Type and hierarchy | `type`, `parent` or `parent_name` | Use one parent form. Parent-relative coordinates and object-array paint order apply. |
| Geometry | `x`, `y`, `w`, `h` | Author in logical pixels. Parents normally appear before children. |
| Application API | `name` | Add to application-controlled elements; it drives generated typed helpers. |
| Application event | `callback` | Use when product code must react; keep the resulting callback non-blocking. |
| Declarative event | `events` | Use for scene-local show/hide, value or navigation actions. |
| Advanced raw state | `bind`, `bind_target` | Declare when the desired runtime property requires a bind; see the declaration rules below. |
| Appearance | `bg_color`, `fg_color`, `border_*`, `radius`, `opacity` | The selected widget must accept the field. Colors use `#RRGGBB` or `#RRGGBBAA`. |
| Text and assets | `text`, `font*`, `image`, `fit` | Paths are relative to the scene. Declare `font_charset` for runtime text beyond static/ASCII characters. |
| Layout | `layout`, `padding`, `gap`, child `margin`/`grow` | Row/column layout controls its main axis; child coordinates control the cross axis. |
| Reuse | `styles`, `themes`, `components`, `template` | Styles/themes/components are compile-time reuse; templates create/recycle runtime instances. |

Later objects paint and hit-test above earlier objects. Keep Drawer descendants
after their Drawer. Use `hidden` for initial visibility and `enabled` only when
runtime enabled/disabled behavior is required.

### Runtime-created template instances

When application code calls a generated `*_create()` helper or
`esp_gsp_widget_create()`, declare the maximum number of simultaneously live
copies on that template. For example, code that may keep four cards alive uses:

```json
{
  "type": "container",
  "template": "card",
  "max_instances": 4,
  "w": 100,
  "h": 100
}
```

`gspc` adds this quota to the bundle's automatic shared-instance requirement,
so the application does not calculate or duplicate a total pool size in
Kconfig. Generated `gsp_<bundle>_<template>_create()` helpers retain the same
typed API. If a fifth copy is requested, the runtime reports the template name
ID, live count and the JSON setting to raise. Keep
`CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES=0` unless consuming an externally
compiled bundle that cannot be regenerated.

## Choose the right structure

| Requirement | Start with |
|---|---|
| Fixed content | `label`, `image`, `shape`, `container` |
| Values and switches | `progress`, `slider`, `arc`, `toggle`, `checkbox` |
| Small fixed choices | `dropdown`, `wheel`, `tabview` |
| Dynamic sequence | `list` plus `row_template` |
| Album | `grid` plus `cell_template` |
| Conversation | `message_list` |
| Camera/video pixels | authored image target plus Canvas API |
| Navigation | `page_flow`, `stackview`, `drawer`, `layer` |

`item_count` is logical dataset length. List/Grid Kconfig and runtime limits
cover only simultaneously active controls and recycled rows/cells.

## Validate

Prefer the owning ESP-IDF build because it applies the actual bundle options,
target profile and Kconfig limits:

```sh
idf.py build
```

Scene compilation is integrated by `gsp_add_bundle()`; ordinary applications
do not need compiler source files or private runtime headers. Rebuild before
using generated APIs. Check layout and interaction in the simulator, then verify display, touch and
animation on the target board.


## Dynamic property scope

A runtime-update marker identifies fields that can generate runtime properties.
The declaration determines which properties are generated:

- `name` gives an object a stable identity; it does not make every static field dynamic.
- For bounded geometry and opacity fields, scalar literals remain static. To expose a runtime property, use `{"default":128,"min":0,"max":255,"property":"alpha"}`. The optional `property` defaults to the field name.
- Raster images use dynamic `x`/`y`, `rotation` and `scalable` for position, rotation and scale; use literals for `w`/`h`/`opacity`/`radius`. Scene SVG images also support bounded `w`/`h`; template images use literals for all four fields.
- Dynamic `x`/`y` translate the object and its subtree. Dynamic `w`/`h`/`radius`/`opacity` affect the object's own drawing; they do not relayout children or apply opacity to an entire subtree as a unit.
- A layer's dynamic `w`/`h`/`radius`/`opacity` controls its background fill; declare a background color when using these properties.
- Widget values, text, colors, visibility and template-instance properties have their own declarations and APIs. Use the relevant widget example and generated headers instead of extrapolating from a runtime-update marker.


The field tables distinguish scene objects from template members. “Own fill”
means the property controls the object's background; text, images and children
keep their own appearance. “Image” requires an image source, and “SVG” applies
to scene SVG images. Enable `runtime_style` where the table specifies it.

Template helpers use the following units: bounded width/height/radius/opacity
setters use a 0..100 value, while the JSON default is authored in the declared units.
For templates, omit `property` or use the field name; custom names are rejected.
Template slot names follow member and field names; scene `property` names select the
corresponding generated scene-property API.

## Declaring runtime controls

For C-side visibility of an ordinary visible object, declare `bind: "eye_visible"`
with `bind_target: "visible"`. Declare a text Bind for mutable text. Value controls,
bounded dynamic fields and template slots also generate their corresponding APIs,
with no additional Bind declaration. Static objects can omit bindings.

A named object with `hidden: true` retains its initial hidden state, visibility API
and required glyphs. An unnamed static hidden subtree without visibility bindings
or declarative visibility references is removed at compile time. Statically hidden
template members are not drawn.

The bundle header exports `GSP_<SCENE>_OBJ_KEY_<NAME>` by default, for example
`GSP_HOME_OBJ_KEY_EYE`. These stable keys require neither LEGACY nor RAW_IDS switches.
Renaming the JSON object makes use of its old symbol fail during application compilation.
Prefer generated typed helpers; use qualified keys for generic component APIs rather
than hard-coded FNV values. Legacy unqualified names remain opt-in to avoid collisions.

## Press actions and default feedback

Ordinary clickable controls already have a default pressed overlay; a pointer observer
is not required. JSON `press`, `release`, `long` and `click` actions using `call` arrive
as `ESP_GSP_EVENT_CALL` with their respective callback IDs.

An ordinary control held for 500 ms without dragging fires `long` once and suppresses
`click` on release. Cancellation or scroll takeover does not fire a long press. Use
distinct callback names for the phases and keep callbacks nonblocking. Cancellation
does not dispatch `release`; use the built-in feedback for automatic pressed-state cleanup.
Hiding or disabling a pressed control cancels its interaction. Restoring the control
while the finger remains down does not start another press; lift and press again. Slider/Arc
value changes and commits follow the range-control contract in the workflow guide.

## Container bounds and clipping

A Container or Layer groups children and provides their coordinate origin.
Set `clip_children: true` to clip child drawing and hit testing to its rectangular
bounds. Nested clips intersect and move with their parent. Use fixed `w` and `h`
for the clipping viewport; `radius` styles the background and border, and
`overflow` controls text overflow.

List, PageFlow, StackView and Drawer provide their own rectangular viewports.
The child viewport is rectangular. For static circular or path masks, bake the
mask into the image alpha channel.

## Shared UI

Place a shared header, navigation bar or keyboard outside the page roots of a
PageFlow/StackView in one scene. The pages can then change while the shared controls
keep their state. Use component definitions to reuse the same UI across separate scenes;
each scene has its own control instances.

For application-drawn overlays, register `esp_gsp_set_overlay_contributor()`. It draws
rounded rectangles and caller-owned A8 glyph bitmaps, and rebuilds them against the
active scene after a scene switch. Handle custom overlay input through the top-level
input interceptor; use scene widgets for built-in layout, focus and hit handling.

A Keyboard declaration generates its pages and keys at build time. The active page
handles rendering and input, while the bundle stores all pages. Sharing one keyboard
across pages keeps both the JSON and compiled resources smaller.
