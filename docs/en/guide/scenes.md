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
3. Scale geometry deliberately when the reference and display resolutions
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

## Field decisions

| Purpose | Fields | Rule |
|---|---|---|
| Type and hierarchy | `type`, `parent` or `parent_name` | Use one parent form. Parent-relative coordinates and object-array paint order apply. |
| Geometry | `x`, `y`, `w`, `h` | Author in logical pixels. Parents normally appear before children. |
| Application API | `name` | Add to application-controlled elements; it drives generated typed helpers. |
| Application event | `callback` | Use when product code must react; keep the resulting callback non-blocking. |
| Declarative event | `events` | Use for scene-local show/hide, value or navigation actions. |
| Advanced raw state | `bind`, `bind_target` | Not needed for generated typed helpers; use only for generic integration. |
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
using generated APIs. Compiler success proves schema and assets; preview,
board execution and visual acceptance remain separate results.
