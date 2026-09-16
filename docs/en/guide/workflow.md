# Recommended Development Workflow

ESP-GSP works best when scene structure, generated interfaces, application
state, and board integration have clear ownership. This workflow applies to
manual development and AI-assisted UI iteration.

## Start with the product constraints

Lock these values before editing a scene:

- ESP-IDF project and selected target;
- logical display width and height;
- RGB565 or RGB888 bundle format;
- panel interface, physical rotation, and byte order supplied by the BSP;
- touch controller and coordinate orientation, when used;
- PSRAM availability and the main memory constraints;
- scene files and bundle symbol owned by the application component.

Do not infer resolution or pixel format from the SoC. The same target can be
paired with different panels and presentation paths.

## Put each change in the right layer

| Requirement | Preferred owner |
|---|---|
| Layout, style, static text, local assets | JSON scene |
| Named value, visibility, color, text, selection, or navigation update | Generated C API |
| Product state, networking, storage, sensors, and blocking work | Application task |
| Runtime images, camera/video frames, dynamic rows | Public media/list/Canvas API |
| Panel timing, framebuffer exposure, rotation, byte swap, TE, touch mapping | BSP/display target |
| Framework-wide behavior missing from the public contract | Report as a framework capability gap |

Keep product-specific policy out of the scene compiler and display layer. Keep
panel-specific policy out of application UI code.

## Prefer the default integration path

For an ordinary firmware-owned UI, embed the bundle and use its generated
header:

```cmake
gsp_add_bundle(${COMPONENT_LIB})
```

This compiles the sorted `scenes/*.json` set with RGB565. Use explicit
`SCENES`, `SYMBOL`, or `PIXEL_FORMAT` only when the application differs from
that convention.

Add an option only when the product needs the corresponding behavior:

- `DYNAMIC_FONT` when runtime text contains glyphs unknown at build time;
- `IMAGE_CACHE_BYTES` after measuring a product-specific decoded-image budget;
- `SYMBOL` when one application component owns multiple independent bundles;
- `PROFILE` for a reviewed expert compiler override;
- `DEPLOYABLE` when a UI package must be delivered independently from the
  application firmware.

`DEPLOYABLE` does not define storage, transport, authentication, rollback, or
partition policy. Keep those decisions in the product update layer. For normal
embedded bundles, the generated-header path remains simpler and is the
recommended default.

Likewise, keep display presentation on `ESP_DISPLAY_PRESENT_MODE_AUTO` unless
measurement on the real target justifies a reviewed override.

## Author the scene for generated APIs

Use a stable `name` for every element the application must update. Use
`callback` for actions that must be decoded by the application. Leave purely
decorative elements unnamed.

Prefer authored structure over runtime construction:

- properties for values, text, colors, checked state, and visibility;
- templates for repeated component structure;
- List, Grid, Wheel, and Message List for application-backed collections;
- PageFlow, StackView, Drawer, and scenes for navigation;
- Canvas for an external producer that owns continuously changing pixels.

Validate fields against [Scene authoring](../reference/authoring.md). Do not add a plausible
JSON key absent from that version’s Schema.

## Build before writing the integration layer

Compile the scene early. The generated `<symbol>_gsp.h` is the exact contract
for the current bundle and includes configuration, scene identifiers, event
predicates, template descriptors, and typed operations for named elements.

Use editor completion and the declarations in the generated header when writing
application code. Update JSON and rebuild after changing names, callbacks, properties,
templates or the scene list.

Generated typed helpers provide object-specific operations; generic public APIs support
data-driven integration. To access raw identifiers, define `GSP_BUNDLE_ENABLE_RAW_IDS`
before including the generated bundle header.

## Connect product state

Treat the application as the source of truth. UI events request product
actions; completed product changes update the UI through setters.

```text
UI callback -> short message/notification -> application task
application state change -> generated setter -> render-task commit
```

Setters are asynchronous on ESP-IDF. A successful return means the update was
accepted. Use `esp_gsp_flush()` only at a deterministic test, capture, or
orderly synchronization boundary.

Event, timer, list-binding, image-release, and Canvas callbacks run on
framework tasks and must not block. Defer storage, networking, decoding owned
by the application, and shutdown to an application task.

## Choose the right data path

| Data shape | Use |
|---|---|
| Build-time image or font | Scene-relative asset |
| Occasional encoded image replacement | Named image generated setter or `esp_gsp_set_image*()` |
| Camera, video, or continuously produced pixels | Canvas frame or direct-draw callback |
| Large application collection | List/Grid/Wheel binder with recycled rows |
| Dynamic chat-style collection | Message List |
| Fixed repeated UI structure | Authored template |

See [Media and application data](media-and-data.md) before selecting ownership
or cache settings.

## Check the application

1. Run `idf.py build` to compile scenes, resources and application code.
2. Check layout, data updates and interaction in the simulator.
3. Run on the target board to check colors, orientation, clipping, animation,
   touch and memory use.

## Compiler-assisted development

Use the compiler as the machine-readable source for editors, scripts and AI
assistants. This avoids copying repository-specific prompt files into an
application:

For installation and example requests, see
[Agent-assisted UI development](agent-assisted-ui.md).

```sh
gspc schema --authoring -o scene.schema.json
gspc diagnose scenes/*.json --format json -o diagnostics.json
gspc docs -o authoring-reference.md
```

The schema describes fields and ranges; diagnostics identify the JSON paths to edit.
Give the agent the scene path, resolution, pixel format, interactions, assets and a
relevant Widget example. Use GSPC to generate resources and application APIs.
See [Agent-assisted UI development](agent-assisted-ui.md) for setup and sample prompts.

## Completion checklist

- Scene dimensions and bundle format match the BSP display target.
- Application-controlled elements have stable names.
- Application actions use generated event predicates.
- Generated artifacts were rebuilt and not edited.
- Callback and buffer ownership follows the lifecycle contract.
- Runtime capacities cover simultaneously active objects, not total dataset
  size.
- Check display, touch and resource use on the target board.

## Multi-scene builds and input completion

`gsp_add_bundle()` links scenes with `compile`, sharing images and font character sets.
The CLI `pack` command uses the same linker; use `bundle` for existing binary members.
To generate application outputs independently:

```sh
gspc compile scenes/*.json -o gsp-out/app.gspb \
  --api-header gsp-out/app_gsp.h --depfile gsp-out/app.d --symbol app
```

Scene IDs follow explicit input order, or sorted filenames with CMake discovery. They
are not permanent identifiers across bundle versions. JSON edits trigger regeneration;
generated action predicates check both the scene and its local action number.

Arc requires an explicit `bind` (for example, `bind: "volume"`) to enable pointer
dragging; a `name` and value setters alone leave it as a gauge. Slider is draggable
by default.

Slider/Arc `events` can bind `value -> call` and `release -> call` for changes and commits.
Cast `arg` to `int32_t` to read the committed value in authored `min/max` units. Unchanged
values do not notify again. A press that changes the value notifies too; release outside
the control still commits, while cancellation does not. A bare Slider/Arc `callback`
defaults to release. Application setters do not emit these user-input callbacks. Queue
business work to an application task from the callback.

Both events also support declarative value, visibility and navigation actions in JSON
order; state writes commit at the end of the event transaction. Only `call` replaces
`arg` with the completed drag value. Other actions retain their authored arguments
(`set_value` uses the target's 0..100 rail, where 0 selects its minimum).
For example, a release `set_value` with `arg: 0` followed by `call` resets the control
while reporting the drag's final value. A getter inside the callback can still observe
the state before those declarative writes commit. Hiding or disabling a captured
control cancels the drag; after restoring it, lift and press again.

For color selection, use three Sliders for H, S and V (suggested ranges: 0..359, 0..100,
0..100). Update the preview on `value` and commit lighting/theme settings on `release`.
The application converts color spaces and retains its full HSV state. Controls still
resolve values in 100 intervals; choose the hue range with that resolution in mind.

Generated Wheel bindings enable row snapping. Advanced Lists can opt in using
`esp_gsp_list_snap()`. `cyclic: true` is rejected at compile time. `snap_to_item: false` disables snapping in both generated-header and deployable-metadata integration.
Handle item selection through row callbacks; snapping controls the scroll position.

## Animations and transitions

Start property animations through the application animation APIs. Keep animation
startup and lifetime management together and check return values. Use the tween
options for looping and alternating playback.

Use `esp_gsp_goto_scene()` to navigate and select a transition, and
`esp_gsp_set_swipe_transition()` to choose the follow-finger visual. Check the
display backend's supported transitions when configuring navigation.

Source checkouts build from source; precompiled component packages use their
bundled libraries. Update the compiler and component package together.

## Compiler selection

Activate the ESP-IDF environment before installing `esp-gsp-tools`. CMake uses
that environment's Python, so installing into a different Python does not make
the manager available to the build.

An explicit `GSPC_EXECUTABLE` CMake value takes priority over the environment
variable. Source checkouts without a version marker use their Cargo-based
compiler entry point. Component packages use the project `.gspc_version`, or
otherwise the package marker: first reuse an exact-version `gspc` on PATH,
then use the manager's versioned cache/download. A mismatched PATH executable is
reported and skipped; all selected compilers must pass format/ABI checks.

For offline builds, put a matching compiler on PATH, set `GSPC_EXECUTABLE`, or
prepopulate the manager cache while online. On failure, use the reported Python
installation command or compiler path. A manager package version is independent
of the GSPC version; upgrading the manager cannot supply an unpublished compiler.

## Dropdown selection and color actions

The generated `gsp_settings_display_mode_set_selected(ui, index)` uses a zero-based
index in the current scene. Scene prefixes distinguish C symbols; they do not route
commands to inactive scenes. Restore the saved index on the matching
`ESP_GSP_EVENT_SCENE_CHANGED`. The setter updates runtime state and displayed text;
it does not rewrite JSON's initial `selected`. Read user selection from the callback's
`event->arg`; getters during that callback can still see the pre-commit value.
On ESP-IDF, setter success means the command was accepted, as with other queued setters.

Use `set_bg_color` to update a color Bind from a scene event. For example,
with `bind: "button_color"` and `bind_target: "color"`:

```json
"events": [
  {"event":"click", "action":"set_bg_color", "target":"button_color", "arg":"#FF3020"}
]
```

Use `#RRGGBB` strings for colors that work across target profiles. Integer arguments
use the target's native pixel format. This action changes the color; configure
opacity separately. Clickable controls provide automatic pressed feedback.

Numeric actions use integer `arg` values; `value` is a compatibility alias.
For example, `add_value` with `arg: -5` decreases a numeric Bind by five steps
on its 0..100 range. `call` also accepts signed 32-bit arguments; cast the
callback's `event->arg` to `int32_t` to read them. Page and scene indices are
unsigned 16-bit values. Invalid types and out-of-range arguments produce a
diagnostic at the event field. An explicit `arg` takes precedence over `value`.

## Generated entry points, event IDs and capacity reports

`gsp_add_bundle()` defaults to `SYMBOL bundle`, producing `bundle_gsp.h` and
`gsp_bundle_config()`. With `SYMBOL ui` (CLI: `--symbol ui`), the entry point becomes
`gsp_ui_config()` and the CMake header becomes `ui_gsp.h`. The generated header names its actual initializer.

Use `gsp_<scene>_event_is_<callback>()` or `event_decode_call()` for normal callbacks;
they check both scene and action IDs. `action_id` is local to a scene. To access
`GSP_<SCENE>_ACT_ID_*`, define `GSP_BUNDLE_ENABLE_RAW_IDS` before including the bundle
header and also check `scene_id`. Raw IDs are not exported by default.

`text slots outside lists` counts mutable scene text and freestanding template text.
`total glyph-run slots` also includes list text and internal headroom. Template capacity is derived from
`max_instances × text slots per instance`; runtime-only objects still require explicit
application capacity. Static text does not consume runtime text slots.

Linked scenes share one set of `(font, size)` packs. `--max-font-packs` defaults to 32,
matching the ESP-GSP font capacity; custom runtimes can pass their supported limit.
The compiler reports an over-limit shared set once for the bundle.
