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
JSON key based on another UI framework.

## Build before writing the integration layer

Compile the scene early. The generated `<symbol>_gsp.h` is the exact contract
for the current bundle and includes configuration, scene identifiers, event
predicates, template descriptors, and typed operations for named elements.

Follow these rules:

- do not edit generated headers, generated bundles, or compiler output;
- do not guess a setter from the element name;
- prefer generated typed helpers over raw bind/object/property identifiers;
- use the generic public API only for data-driven features that the generated
  object API does not cover;
- rebuild after changing a name, callback, property, template, or scene list.

Raw IDs are an advanced compatibility surface. Define
`GSP_BUNDLE_ENABLE_RAW_IDS` only when a real data-driven integration requires
them.

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

## Validate in layers

Report each layer independently:

1. **Scene generation:** JSON, assets, profiles, and configured capacities are
   accepted.
2. **Generated API integration:** application code compiles against the new
   header without hand-written IDs.
3. **Target build:** configuration, compilation, and linking pass for the
   selected `IDF_TARGET`.
4. **Host behavior:** preview or host tests cover the intended interaction.
5. **Board operation:** firmware starts and the real panel/touch path works.
6. **Visual acceptance:** a person confirms layout, motion, colors, clipping,
   tearing, and touch feel on the intended hardware.

A host preview is not board proof. A clean device log is not visual proof.

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

The schema describes accepted fields and ranges, diagnostics identify exact
JSON paths, and the generated reference follows the compiler registry. Product
state, ownership and hardware decisions still belong in application code and
the BSP; generated content must pass the same layered validation as hand-written
content.

Agent-assisted or "vibe coding" is supported when it stays inside this
contract:

1. Provide the exact repository version, target/profile, logical resolution,
   assets, required interactions, scene path, and closest Widget example.
2. Give the agent the current Schema, authoring reference, and relevant Widget
   pages; do not let it borrow properties from another UI framework.
3. Let it edit source JSON and assets only. Keep headers, GSPB, IDs, capacity
   calculations, and other compiler output tool-owned.
4. Run `gspc diagnose` and the real build after every meaningful change, then
   feed exact JSON paths and diagnostics back to the agent.
5. Review the JSON diff and validate the local simulator preview separately from target
   hardware and visual acceptance.

## Completion checklist

- Scene dimensions and bundle format match the BSP display target.
- Application-controlled elements have stable names.
- Application actions use generated event predicates.
- Generated artifacts were rebuilt and not edited.
- Callback and buffer ownership follows the lifecycle contract.
- Runtime capacities cover simultaneously active objects, not total dataset
  size.
- Validation results state exactly which layers were exercised.
