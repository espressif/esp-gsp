# Scene JSON Guide

Scene JSON describes the build-time UI: logical size, hierarchy, appearance,
assets and interactions. Product state, I/O and display-driver setup remain in
application/BSP code. Use this guide for common decisions and the generated
[authoring reference](../reference/authoring.md) to look up every accepted field and default.

## From a description or reference image

Lock the logical resolution, pixel format, display orientation, available
fonts/assets, dynamic content and user actions first. Use a screenshot or design
image as a visual reference:

1. Model visible elements as containers, labels, controls, shapes and reusable
   images. A screenshot asset cannot provide their states or interactions.
2. Scale geometry when the reference and display resolutions
   differ.
3. Record assumptions for missing fonts, exact colors and interaction states.

A screenshot asset does not define pressed/disabled state, scrolling, navigation,
loading or error behavior; declare those behaviors in the scene and application.

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

Required top-level fields are `screen`, `w`, `h` and an `objects` array. An
empty array is valid for a background-only scene. Scene size is in logical
pixels and must match the bundle/BSP display path. `w` and `h` must be greater
than zero. `default_font_size`, when present,
must be 1–255. Build uses the same field type, enum, range and semantic checks
as `gspc diagnose`, so unsupported combinations fail before lowering. The
documented rectangle aliases retain their legacy color fields; unknown fields
on registered widgets are errors.

Ordinary parent references may point forward or backward in the object array.
Inherited hidden state and template membership do not depend on that ordering;
the compiler preserves authored draw order.

## Reusable project components

Grid/wrap and component expansion were introduced with GSPC 0.7.0 and
ESP-GSP 1.6.0. For current projects, use the matched compiler/runtime pair from
the [compatibility contract](../reference/compatibility.md).
Compiler-owned virtual collection content can use the GSB 2.20
`instance_order` variant; rebuild affected scenes with the matched toolchain.

A scene's `components` map, or JSON files loaded with `--components-dir`, defines
parameterized subtrees. Each definition has an `objects` array and an optional
`params` array of required argument names. A whole string such as `"$title"` is
replaced by the corresponding argument, preserving its JSON type. Strings that
do not start with `$` also support textual substitution, for example
`"Reading: $value"`; non-string arguments become JSON text in this form. A string
starting with `$` is treated as one complete parameter reference, not as a
prefix followed by a template suffix. Substitution does not evaluate arithmetic
expressions.

Instantiate a definition with `use`. The use site can override `name`, `x`, `y`
and either `parent` or `parent_name` on the root. Named parents can be declared
later in the scene. Other use-site fields must be declared parameters and used
explicitly by the definition. A child's numeric `parent` inside the definition
is a local object index; omitted parents attach to the definition's root.
`parent_name` references scene-wide names. Give each repeated instance distinct
names for any exposed children, for example through required name parameters.

Definitions can contain ordinary widgets and built-in composites such as
TabView, Dropdown, Keyboard and MsgBox. Their generated controls participate in
the same event and overlay paths as directly authored controls. Nested `use`
inside a definition is diagnosed; combine definitions at scene level or flatten
shared subtrees. Scene-level `styles` and root font defaults remain available.
Asset paths resolve relative to the scene, not to the component definition file.

The [component patterns example](../../../examples/usage/component_patterns/README.md)
provides settings rows, icon buttons and badges. These are editable project
components, not new runtime widget types. Expanded objects still consume command,
state and resource capacity; compare against an equivalent hand-authored scene.
Use `gspc diagnose --components-dir <directory> <scene.json>` for expansion and
build-semantic diagnostics. A daemon request with `level: "schema"` checks source
structure only; use `level: "compile"` for the complete validation path.

For project builds, pass `COMPONENTS_DIR` to `gsp_add_bundle()` and to the PC
adapter's `gsp_add_backend()`. Both use the same definitions and resolve the
directory relative to the calling `CMakeLists.txt`:

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    COMPONENTS_DIR ../ui/components
    SCENES ../ui/scenes/home.json ../ui/scenes/settings.json)
```

Each top-level JSON filename is its component name (`Card.json` defines `Card`).
Edits, additions and removals rebuild the bundle; generated compilation contexts
retain the directory for diagnosis and preview. Keep existing inline definitions
when they suit the project; a shared directory is optional.

Compilation checks substituted component fields with the same widget field,
type, range and action rules as directly declared objects. Invalid fields stop
compilation before new resources and headers are generated. Field diagnostics
identify the authored definition, instance argument or shared style through
`file`, `path` and `range`; a definition need not have a flattened `objectIndex`.
Keys with punctuation use bracket notation in diagnostic paths.

## Appearance

This card combines a rounded background, an outside outline, a hard shadow and
column layout. The two bars are ordinary child rectangles:

```json
{
  "screen": "styled_card", "w": 240, "h": 160, "screen_bg": "#101827",
  "objects": [
    {"type":"container","name":"card","x":20,"y":20,"w":200,"h":104,
     "bg_color":"#24334A","radius":12,"outline_color":"#5AA9E6","outline_width":2,
     "shadow_color":"#000000","shadow_offset_y":6,"shadow_opacity":96,
     "layout":"column","padding":16,"gap":12,"align_cross":"stretch"},
    {"type":"rect","parent_name":"card","w":168,"h":16,"bg_color":"#5AA9E6"},
    {"type":"rect","parent_name":"card","w":168,"h":16,"bg_color":"#5AA9E6","bg_opacity":96}
  ]
}
```

Save it as `styled_card.json` and run `gspc pack styled_card.json --deployable -o styled_card.gspb`.
The fields below describe how to adapt the appearance and layout.

For content and structural widgets, `bg_opacity` (0–255, default 255) affects
only the background. Its effective alpha is
`round(round(opacity * bg_opacity / 255) * color_alpha / 255)`.
`border_opacity` independently scales an authored border. A non-255 `bg_opacity`
requires literal `opacity`.

Widgets with the outline fields use `outline_color`, `outline_width`,
`outline_pad` and `outline_opacity` for an outside rounded outline. The outline
requires a color and a positive width. `border_side` is available on content and
structural widgets with the values `all` (default), `none`, `top`, `bottom`,
`left`, `right`, `horizontal` and `vertical`. Partial borders are inside the
original box and require static `w`, `h` and `radius: 0`; width, color and
opacity are shared. Opposite thick sides are split so a pixel is not blended
twice. `all` keeps the rounded-border behavior, while `none` suppresses only
the border.

`shadow_offset_x/y`, `shadow_spread`, `shadow_radius`, `shadow_color` and
`shadow_opacity` enable a hard, filled rounded shadow. Defaults are black,
opacity 96, zero offset/spread and the element radius; spread increases the
paint bounds and shadow radius. Decorations use the element box, not the image
alpha silhouette or the union of child bounds. They expand painted and dirty
regions without changing layout size or hit targets, and ancestor clipping
still applies. Each enabled outline or shadow adds at most one primitive before
optimization; zero effective opacity emits none. A partial border adds one strip
per selected side, up to two. Translucent pixels and larger dirty regions add
rendering cost.

Scene position groups and whole template instances can move decorations with
their element. Outlines and shadows require static `w/h/radius`.
Template members with partial borders require literal `x/y` and
a literal or static-theme border color. Template members with outlines or
shadows require literal `x/y` and literal or static-theme decoration colors.

## Layout

`layout` resolves direct children at build time. `row` and `column` keep their
existing behavior when `layout_wrap` is omitted or false. In row/column layout,
`margin_left`, `margin_right`, `margin_top` and `margin_bottom` override the
corresponding sides of `margin`.

For row/column, `align_main` accepts `start`, `center`, `end` and
`space_between`; `align_cross` accepts `start`, `center`, `end` and `stretch`.
In non-wrapped row/column, omitting `align_cross` preserves the authored
cross-axis position. Set `layout_wrap: true` to partition children into lines
using each child's bounded authored main size, including margins. `gap` separates
adjacent children and lines. An oversized child remains a single item. Alignment
is applied independently to each line, and wrapped row/column defaults to
cross-axis `start`. A wrapped line distributes extra main-axis space to `grow`
children from their authored bounded size, subject to their maximum bounds.

`layout: "grid"` arranges authored children; it is distinct from the virtualized
[data Grid widget](../components/grid.md), `type: "grid"`, which recycles cells.

`layout: "grid"` requires `grid_columns` from 1 to 65535. It creates equal-width
column tracks; when the inner width is not divisible evenly, the remainder goes
to the leading columns. Children are placed in authored order, filling rows from
left to right; a `grid_column_span` (default 1) may occupy 1 through
`grid_columns` columns and moves to the next row when it does not fit. A grid row
height is the maximum authored bounded child height, including its vertical
margins. Grid child width comes from the spanned track width minus horizontal
margins, clamped by the child's width bounds. `align_main` (`start`, `center` or
`end`) positions a max-width-capped child within its cell; `space_between` is
invalid for grid. `align_cross` (`start`, `center`, `end` or `stretch`) positions
or stretches a child within its row. Grid children cannot use `grow`.

`min_width`, `max_width`, `min_height` and `max_height` (0–65535 pixels)
constrain static dimensions, including row/column grow, cross-axis stretch and
grid cell sizing.
The minimum must not exceed the maximum, and a bound cannot share its dimension
with a bounded dynamic declaration. Grow allocation reserves minima, distributes
remaining space by grow weight, and returns space from children that reach their
maxima. `align_main` places any space left after all grow children reach their
maxima. If minima do not fit, they are preserved and content overflows. These
constraints do not add runtime reflow. Layouts with explicit alignment or size
constraints reject bounded dynamic child positions or grow sizes that they would
overwrite; when these fields are omitted, the legacy path remains permissive.
Grid and wrapped row/column reject bounded dynamic `x`, `y`, `w` or `h` on direct
children because the solver owns or uses that geometry. Nested layout parents
resolve before their child layouts, including when a parent is declared later in
the object array. Runtime setters do not reflow siblings. Existing non-wrapped
row/column scenes remain unchanged when these new fields are not used. Move the
enclosing group when the compiled arrangement should move together.

An expanded composite used as a direct Grid/Wrap child may move when its root
keeps its authored size. If layout would change the expanded root's actual
`w`/`h`, compilation reports a diagnostic because the composite's internal
geometry was fixed during expansion. Keep the composite at its authored fixed
size inside a container and make that container the direct Grid/Wrap child; the
container can take the layout-assigned geometry while the composite retains its
internal geometry.
Generated composite overlays and placeholders are excluded from Grid/Wrap
placement and never consume cells.

This complete scene shows a three-column grid with a spanning child and a wrapped
row. The child `x`/`y` positions are omitted because the selected layout supplies
them:

```json
{
  "screen": "grid_wrap",
  "w": 320,
  "h": 240,
  "screen_bg": "#101820",
  "objects": [
    {
      "type": "container",
      "name": "grid",
      "x": 16,
      "y": 16,
      "w": 288,
      "h": 104,
      "layout": "grid",
      "grid_columns": 3,
      "padding": 8,
      "gap": 8,
      "align_main": "center",
      "align_cross": "center",
      "bg_color": "#1B263B"
    },
    {
      "type": "rect",
      "parent_name": "grid",
      "w": 64,
      "h": 24,
      "max_width": 72,
      "bg_color": "#4CC9F0"
    },
    {
      "type": "rect",
      "parent_name": "grid",
      "w": 64,
      "h": 24,
      "max_width": 72,
      "bg_color": "#90BE6D"
    },
    {
      "type": "rect",
      "parent_name": "grid",
      "w": 128,
      "h": 24,
      "grid_column_span": 2,
      "max_width": 152,
      "bg_color": "#F9C74F"
    },
    {
      "type": "container",
      "name": "wrapped",
      "x": 16,
      "y": 136,
      "w": 288,
      "h": 88,
      "layout": "row",
      "layout_wrap": true,
      "padding": 8,
      "gap": 8,
      "align_main": "center",
      "bg_color": "#1B263B"
    },
    {
      "type": "rect",
      "parent_name": "wrapped",
      "w": 100,
      "h": 20,
      "bg_color": "#F94144"
    },
    {
      "type": "rect",
      "parent_name": "wrapped",
      "w": 90,
      "h": 24,
      "margin_right": 4,
      "bg_color": "#F3722C"
    },
    {
      "type": "rect",
      "parent_name": "wrapped",
      "w": 120,
      "h": 18,
      "bg_color": "#577590"
    }
  ]
}
```

## Text

Static scene text supports `text_line_space` (0–4096 extra pixels between rows)
and `text_vertical_align` (`auto`, `top`, `center`, `bottom`). Alignment applies
to the complete visible text block, including empty rows. `auto` keeps
single-line text centered and multiline text top-aligned. The values are applied
at compile time and add no runtime text-layout work.

Non-default spacing or vertical alignment requires unbound scene text, `input`
disabled and literal `w/h`. Text bindings, input fields, template text and
bounded dynamic `w/h` reject these non-default values. Color and visibility
bindings remain available.

## Images

The `image` widget's `image_opacity` (0–255, default 255) affects image pixels
without changing the widget background or border. It also applies to ordinary
template image blits. A fixed, non-animated raster can combine image opacity
with runtime fit, scaling and rotation when `opacity` is static 255 (the default), the
effective codec is unset (`codec` omitted, `auto` or `default`) or `store`, and
`store_scale` is 1. With `codec` omitted, the legacy `compress: true` setting
selects a lossless container and cannot use this conversion. Dynamic image sources,
template `dynamic_image`, animated assets and SVG do not use opacity baking;
combinations that require baking produce a compiler error.

When baking is used, the compiler multiplies source alpha by `image_opacity / 255`
and emits a lossless `store` resource variant, without the profile's automatic
compression policy. The alpha plane and multiple opacity variants can increase
resource size; identical encoded payloads are deduplicated. Runtime transforms
then use the baked alpha without an extra opacity operation.

For a translucent bitmap source, literal `rotation: 0` is normalized to no
runtime rotation property and emits a warning. Opaque bitmap sources retain
their rotation property at zero. SVG raster imports keep literal zero rotation
static. A bounded dynamic rotation remains a runtime property even when its
default is zero.

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
| Layout | `layout`, `layout_wrap`, `grid_columns`, child `grid_column_span`, `padding`, `gap`, child `margin`/`grow` | Row/column controls its main axis; grid assigns equal tracks; direct child geometry is compiler-owned. |
| Reuse | `styles`, `themes`, `components`, `template` | Styles/themes/components are compile-time reuse; templates create/recycle runtime instances. |

Later objects paint and hit-test above earlier objects. Keep Drawer descendants
after their Drawer. Use `hidden` for initial visibility and `enabled` only when
runtime enabled/disabled behavior is required.

`gspc diagnose scene.json --layout-check` can list candidate intersections
between static root Button/Label/Toggle boxes, with a suggested probe point. Reports
are bounded and informational; intentional layering remains valid. Dynamic or
parented geometry needs preview and real input checks. Use the native runner's
[coordinate taps and callback expectations](../../../tools/sim_bridge/README.md#repeatable-application-checks)
to check event routing and underlying application state.

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
- Widget values, text, colors, visibility and template-instance properties each have their own declarations and APIs.

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

An object's visibility gates drawing and input for its entire authored subtree,
including children of a button or other widget and children painted outside the
parent's rectangle. Each child's own visibility is preserved: showing the parent
does not show a child that is itself hidden, and showing a child cannot bypass a
hidden ancestor. Use `enabled` to control interaction while keeping the object
visible. Visibility is compiled into the bundle; regenerate affected bundles
with the corrected compiler to obtain this behavior.

The bundle header exports `GSP_<SCENE>_OBJ_KEY_<NAME>` by default, for example
`GSP_HOME_OBJ_KEY_EYE`. These stable keys require neither LEGACY nor RAW_IDS switches.
Renaming the JSON object makes use of its old symbol fail during application compilation.
Prefer generated typed helpers; use qualified keys for generic component APIs rather
than hard-coded FNV values. Legacy unqualified names remain opt-in to avoid collisions.

## Press actions and default feedback

Ordinary clickable controls already have a default pressed overlay; a pointer observer
is not required. JSON `press`, `release`, `cancel`, `long` and `click` actions using `call` arrive
as `ESP_GSP_EVENT_CALL` with their respective callback IDs.

Click eligibility covers the entire contact. Reaching the configured tap tolerance,
leaving the target beyond its edge margin, or parent gesture takeover cancels the
contact even if the finger returns. Small edge jitter may return within the margin
(one quarter of tap tolerance, capped at 4 scene pixels); release must still hit the
original control. Pointer injection and hardware touch frames use the same rules.
Use `click` for navigation or submission; `press` fires immediately on DOWN.

Holding an ordinary control for 500 ms without dragging fires `long` once and
suppresses `click` on release. Leaving its actual hit area restarts the long-press
timer. Cancellation restores pressed feedback and emits `cancel` once instead of
`release/click`; bind `cancel` to stop application actions started by `press`.
Hiding, disabling or leaving the scene also cancels the contact. Restoring a control
while held does not start another press. Slider/Arc changes and commits follow the
range-control contract in the workflow guide. Keep callbacks nonblocking.

## Container bounds and clipping

A Container or Layer groups children and provides their coordinate origin.
Set `clip_children: true` to clip child drawing and hit testing to its rectangular
bounds. Nested clips intersect and move with their parent. Use fixed `w` and `h`
for the clipping viewport; `radius` styles the background and border, and
`overflow` controls text overflow.

List, PageFlow, StackView and Drawer provide their own rectangular viewports.
The child viewport is rectangular. For static circular or path masks, bake the
mask into the image alpha channel.

For a PageFlow or Drawer with authored `enabled`, disabling interaction cancels
an active pan on the next input sample or frame. Re-enabling allows new contacts;
the cancelled contact stays consumed until release. Application navigation APIs
remain available while gestures are disabled.

## Shared UI

Place a shared header, navigation bar or keyboard outside the page roots of a
PageFlow/StackView in one scene. The pages can then change while the shared controls
keep their state. Use component definitions to reuse the same UI across separate scenes;
each scene has its own control instances.

PageFlow, StackView and Drawer move only their descendants. Independent content
that overlaps a moving viewport keeps its authored position and paint order.
The runtime uses frozen viewport snapshots only when it can establish isolation
from independent paint; otherwise it renders live transforms (List's
`scroll_snapshot` follows the same rule), which can increase rendering work.
Animated jumps longer than one page
extent land immediately when snapshots are unavailable.

An opaque rectangular Drawer can use a stationary background capture even when
the underlying page has gradients or independent transform groups. A parent clip
that cuts into a Drawer or StackView viewport requires live rendering. PageFlow
can retain clipping perpendicular to its motion; clipping the motion axis also
requires live rendering. These choices preserve drawing order and input behavior
without additional application configuration.

During Drawer, PageFlow and StackView snapshots, automatic procedural effects and
media playback pause. An explicit update affecting captured pixels ends the
snapshot and resumes live rendering; unrelated offscreen property updates can
preserve it. Playback resumes after the snapshot ends.

For application-drawn overlays, register `esp_gsp_set_overlay_contributor()`. It draws
rounded rectangles and caller-owned A8 glyph bitmaps, and rebuilds them against the
active scene after a scene switch. Handle custom overlay input through the top-level
input interceptor; use scene widgets for built-in layout, focus and hit handling.

A Keyboard declaration generates its pages and keys at build time. The active page
handles rendering and input, while the bundle stores all pages. Sharing one keyboard
across pages keeps both the JSON and compiled resources smaller.

## Dynamic geometry and input

Dynamic width, height and position update painted bounds and interactive hit bounds together. Template geometry value slots retain their existing 0–100 percentage semantics. Layout and `clip_children` keep their declared bounds. Parent visibility parks owned row instances, and nested movement invalidates the old and final painted areas.

Regenerate scenes with the matching compiler and runtime. Dynamic interactive geometry emits the REQUIRED `HIT_GEOMETRY` section in GSB 2.21; an earlier runtime rejects it. Existing resources keep their original contract.
