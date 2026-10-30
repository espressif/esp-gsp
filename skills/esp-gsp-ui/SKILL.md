---
name: esp-gsp-ui
description: Create and change ESP-GSP scenes and their ESP-IDF C integration. Use for UI requirements or reference images, widget/layout selection, dynamic data and events, GSPC diagnostics, and simulator or display troubleshooting.
---

# ESP-GSP UI

Produce scene JSON and application integration compatible with the component
and GSPC actually selected by the application. Reuse the project's existing
Bundle registration, names, target, assets and build conventions.

## Select the task path

- **Create or modify a screen:** inspect the affected scenes and callers, query
  the relevant widget cards/examples, edit JSON and validate through the selected
  compile or owning build. Inspect changed visuals and interactions in preview.
- **Reconstruct a reference design:** read [design reconstruction](references/design-reconstruction.md)
  for assets/fonts, native controls, independent visual references and settled-state checks.
- **Connect live data, events, collections or Canvas:** read
  [application integration](references/application-integration.md), then the
  selected component's relevant public API guide and generated headers.
- **Resolve tools, target settings or an unfamiliar project:** read
  [project context](references/project-context.md). Discover facts from files;
  ask only for product choices the project and conversation do not establish.
- **Preview or test behavior:** read
  [preview and automation](references/preview.md). Use a standalone simulator
  for declarative scene behavior, and the native `sim_bridge` for portable
  application C callbacks, timers and state. Use the board for panel, timing,
  peripherals and device performance.

Read only references needed by the current task. Keep validation proportional
to the change and honor the user's requested scope.

## Establish the selected contract

Identify the application root, active component, GSPC executable, Bundle and
affected scenes. Use `gspc doctor <project> --build <build-dir>` for an existing
build and reuse its observed Bundle context for checks and previews. Resolve
missing tools, contexts or target facts through the
[project context reference](references/project-context.md); refresh the report
when the selected tool, project or build metadata changes.

Load widget details on demand: use `gspc cards <widget>` for a known control,
or `gspc cards --list` to discover summaries when advertised. A card's example is
an authoring fragment; use `example_path` and `doc_paths` for the checked full
example and matching component page. Export full Schema/docs/inventory only
when the task needs them. Read the selected component's language-matched
`docs/en/` or `docs/zh-cn/`; source pages live in `docs/src/content/docs/`.

## Author the smallest complete change

For requirements or a reference image, determine screen states, hierarchy,
dynamic values, actions, assets and acceptance criteria. A reference image is
visual guidance unless identified as a product asset. Reconstruct text and
controls as native objects. Preserve existing scene and callback names when
modifying a screen; prefer `parent_name` for relationships that should survive
object insertion or reordering. Array order also controls paint/hit order.

Use the selected version's layout, styles, themes and reusable components
where they fit the design. Keep layout, static content and declarative actions
in scene JSON; product state and blocking work belong to the application;
panel timing, rotation and touch mapping belong to the BSP. Use declarative
navigation for ordinary scene changes, application callbacks for history, and
`stack_pop` for a StackView.

Preserve explicit `SCENES`, Bundle `SYMBOL` and pixel format. For a new ordinary
registration, use `scenes/*.json` and `gsp_add_bundle(${COMPONENT_LIB})`.
Names are for application-controlled objects; callbacks are for behavior that
crosses into product code. Inspect dynamic declarations before choosing a
setter. Compiler-derived collection/slot demand should remain compiler-owned.

## Validate and integrate

Validate meaningful JSON changes with the owning build or a target-configured
compile/preview, using that run's diagnostics and artifacts for integration and
preview. Reuse a successful check while inputs, scene set and resolved options
remain unchanged. Verify application C changes through their owning build.

For a check without generating artifacts, use:

```sh
gspc diagnose --context <selected-bundle.context.json>
```

Without a generated context, use `gspc diagnose scenes/main.json --format json`
with the project's confirmed Profile/capacity arguments. Empty CLI input is
rejected. Use reported source files, paths/ranges and deterministic `suggestions` to
repair errors. Daemon `diagnostics/pull` defaults to Schema checks; final
Profile/resource checking requires `level: "compile"` with the selected context
or actual CLI compilation.

Generate the Bundle and headers before writing C against new helpers. Inspect
`<symbol>_gsp.h`, per-scene headers and matching `*.api.json`; use generated
typed setters, event helpers and collection/template APIs. Fix source inputs
and use the sidecar's declared property keys, which can differ from JSON field names,
then regenerate outputs. Build application changes through the owning ESP-IDF
project. For ownership, lifecycle or special data APIs, load the
[application integration reference](references/application-integration.md).

When existing layout is ambiguous, use optional compiler `inspect` in the same
preview to identify source and layout owners before editing coordinates. Follow
the [project context reference](references/project-context.md); keep the user's design.

For daemon previews, verify `result.success`, `stale: false`, `inputIdentity`,
`artifactIdentity` and the matching Bundle/API sidecar before accepting the
returned Bundle. `compile/previewProject` invalidates changed compiler settings
and referenced assets/components and recompiles dirty screens.

## Verify and deliver

For automation, prefer headless testing. Give a user-requested interactive
preview its own `browser-exclusive` session and distinct endpoints. Keep the
project/Bundle identity with each session. Full setup and commands are in
[preview and automation](references/preview.md).

Verify application behavior through actual input and the shared C Backend:

- Derive acceptance paths from the requested behavior and affected states,
  covering input boundaries, confirm/cancel, return/reenter and empty/error states.
- Use optional scenario `checks`/`check` labels to track assertion coverage and
  resolve any unverified items before completion.
- When advertised, use `get_property` and `wait_property` for scalar runtime state,
  `wait_keyboard_text` for keyboard buffers, and callback events plus
  screenshots for behavioral and visual verification.
- Review `GSPC-TEXT-CLIPPED` diagnostics (box dimensions, line height and omitted
  lines) and adjust unintended text clipping while preserving intentional
  clipping or ellipsis.
- Because compiled bounds describe initial layout, verify overlapping controls,
  modal layers and dynamically moved geometry by probing target coordinates
  with exact `callbacks` expectations and checking the resulting application
  state and screenshots.

Summarize changed sources, build/preview entry points and completed checks in
the user's language. For visual work, inspect a captured screenshot against the
user's acceptance criteria and include it. Report verification layers
and remaining gaps using the [preview verification scope](references/preview.md).
Keep generated files in the normal build tree and implementation documentation
focused on public behavior, actual dependencies and established constraints.
