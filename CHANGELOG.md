# Changelog

## Unreleased

## 1.1.0

- Added code-checked English and Chinese public-function references, complete
  Chinese authoring and Widget inventories, and matching configuration,
  Kconfig, compatibility, and binary-format navigation.
- Completed bilingual standalone-tool onboarding with one stable release
  entry point, compatibility checks, simulator dependency boundaries, and
  actionable missing-GSPC diagnostics.
- Reorganized documentation into explicit `docs/en` and `docs/zh-Hans` trees
  with one language entry point and no parallel documentation website stack.
- Generate every bilingual Widget page from the GSPC registry and its checked-in
  JSON example, including actual generated C signatures and the canonical local
  WASM preview command.
- Added exact registry, example, English/Chinese page and field-translation
  coverage gates, and removed the obsolete Web catalog and duplicate Widget
  preview scripts.
- Added configure-time GSPC compatibility validation based on generated
  binary-format, requirements, configuration and GMD runtime ABI contracts.
  Incompatible standalone compilers now fail before scene build rules run.
- Removed compiler-local configuration and runtime-local GMD ABI version
  constants; both sides now consume their existing schema-generated values.
- Added an independent GSPC version/changelog release gate and generated
  bilingual compatibility reference.
- Consolidated the Rust GSPC source into the ESP-GSP repository so compiler,
  runtime formats, generated documentation and compatibility tests have one
  maintained source of truth. The released component still consumes a
  separately published executable through `GSPC_EXECUTABLE`.
- Added the conventional `scenes/` project directory while retaining explicit
  `SCENES` paths for multi-bundle and non-standard layouts.
- Kept runtime Kconfig policies and heap-backed capacities available to
  source-free consumers, while hiding only capabilities fixed into prebuilt
  archives and deriving authored scene requirements from JSON.
- Separated GSPC and native/browser simulators from the Component Registry
  archive, and added generated schema, authoring and widget-inventory checks
  backed by one real preview example per compiler-registered widget.
- Published `esp_display_present` 1.0.1 so its bundled handoff test app follows
  the conventional `scenes/` layout used by the release examples.
- Added an installable ESP-GSP UI authoring Skill backed by GSPC schema,
  diagnostics, checked-in Widget examples and bilingual documentation, while
  keeping the Skill outside the Component Registry archive.
- Clarified the Arc thickness default and the current Slider/Arc value-event
  and reserved `back` action contracts in generated references and guides.

## 1.0.0

- Publish `esp-gsp` and `esp_display_present` under the official `espressif`
  Component Registry namespace.
- Align the runtime, manifests, examples, documentation, and package consumer
  checks on the 1.0.0 component release contract. The standalone `gspc`
  compiler keeps its independent version.

## 0.2.10

- Removed the in-repository Python `gspc` implementation from the Component
  Registry package. Component consumers now select a separately released,
  format-compatible compiler through `GSPC_EXECUTABLE`.
- Decoupled ESP-GSP component/runtime release validation from the standalone
  `gspc` version. Existing `.gspb` files remain directly usable by the
  packaged simulator without a compiler.

## 0.2.7

- Fixed prebuilt-package consumption when `esp_display_present` is installed
  under its Component Registry namespace (`espressif2022__esp_display_present`).
- Added a namespaced dependency-layout smoke test to prevent unqualified
  component lookups from passing CI accidentally.

## 0.2.6

- Fixed source-free ESP-IDF packages so the public touch adapter is retained
  when the prebuilt core references it during the final firmware link.

## 0.2.5

- Added the public, opaque Simulator session SDK with explicit lifecycle,
  frame, input, and surface contracts.
- Added consumer-built SDL, headless, Web, and ESP-IDF touch adapters while
  keeping renderer/runtime composition internals in prebuilt archives.
- Split simulator CMake targets into core and adapter targets, retaining the
  legacy fat simulator target for migration compatibility.
- Added native simulator artifact gates and source-free Registry package
  validation for the public adapter boundary.

Breaking changes in the display-present and LCD-host APIs:

- `esp_display_presenter_begin_frame()` renamed to
  `esp_display_presenter_begin_next_frame()`; the frame timeline is now
  presenter-owned and render areas are returned up front.
- `esp_display_presenter_acquire_region()`/`esp_display_presenter_submit_region()`
  renamed to `esp_display_presenter_acquire_buffer()`/
  `esp_display_presenter_submit_buffer()`; submission takes an explicit
  logical area and stride.
- Presenter buffer configuration unified under `drawbuf`
  (`drawbuf_bytes` replaces the per-partition sizing).
- Removed `esp_display_presenter_get_completed_transfer()`/
  `esp_display_presenter_get_completed_present()`; completion tickets are
  now private to the presenter, use `esp_display_presenter_quiesce()`.
- `esp_gsp_esp_lcd_pause()` now yields an opaque pause token consumed by
  `esp_gsp_esp_lcd_resume_paused()`, which resumes the exact paused UI and
  forces its first frame to redraw in full.

## 0.2.4

- Fixed component-free and otherwise empty scenes being rejected as out of
  memory when their compiled requirements legitimately resolved optional
  runtime pools to zero capacity.

## 0.2.3

- Fixed a dual-core PPA teardown race where an application switch could delete
  the global SRM mutex while another render task was entering it, leading to
  an interrupt watchdog timeout in `ppa_blit()`.

## 0.2.2

- Added the dropdown `open_direction` scene property. It accepts `down`
  (default) or `up`, allowing panels near the bottom edge to open above their
  selection box.

## 0.2.1

- Fixed decoded-image cache entry negotiation so a non-zero project baseline
  is still raised to fit the compiled startup images and runtime image
  generations. Applications no longer need to predict each scene's image
  count or carry a per-bundle cache-entry override.
- Applied resolved instance-state, image-cache budget, idle/pointer cadence and
  StackView depth settings to their actual runtime consumers. These settings
  no longer report an effective value while silently using a built-in default.
- GSPB requirements now include declared template instances, per-instance
  state width and StackView depth. Standalone `gspc bundle` reads the exact
  requirements emitted by `gspc build` and rejects stale/missing sidecars
  instead of silently packaging placeholder capacities.
- Source-free registry/local components hide the complete ESP-GSP menuconfig
  tree. Hidden defaults still feed the project bridge; source checkouts retain
  the engineering menu.
- Requirements contract version 2 identifies StackView-depth and
  per-instance-state fields so an older runtime rejects newer gspc output
  instead of silently ignoring them. The 0.2.1 runtime still accepts legacy
  version-1 requirements without interpreting those formerly reserved fields.
- Removed the internal `ESP_GSP_CONFIG_*` Kconfig compatibility layer. Build
  capabilities now come from a schema-generated header; runtime code and
  package consumers read the project/effective configuration structures.
- Prebuilt consumers now use one menu-level visibility rule, so every project
  symbol is present in `sdkconfig` with its schema default while the complete
  engineering menu remains hidden.

## 0.2.0

- Replaced compile-time product configuration in prebuilt libraries with a
  versioned project bridge and immutable per-instance effective policy and
  capacity snapshots.
- Added GSPB capacity requirements, strict schema/ABI validation, AUTO
  resolution and precise diagnostics for undersized application overrides.
- Changed the public override ABI to eight sparse inline entries plus an
  optional caller-owned read-only extension table. The transitional typed
  fields remain available until 0.3.0.
- Runtime pools, List rows and StackView pages are allocated from resolved
  capacities; source, prebuilt, SDL and WASM consumers share the same model.
- GSPB and public configuration artifacts from 0.1.x are incompatible and
  must be regenerated with gspc 0.2.0.

- Applied consumer `menuconfig` runtime settings to source-free prebuilt
  libraries instead of freezing product behavior into the release build.
- Added source-free SDL/headless and WebAssembly simulators to the Registry
  component, built from the same portable core as the device archives.
- Made packaged simulation self-describing through deployable metadata and
  fixed JPEG, FreeType, dynamic-font, component-directory and capability ABI
  support across native and browser runtimes.
- Added final-package simulator tests and minimum/current ESP-IDF consumer
  matrices; stripped debug information from distributed device archives.

## 0.1.3

- Made CMake helpers namespace-independent by using the component's registered
  target and validated the exact Registry-qualified consumption path.
- Added component-specific release tags and stricter per-target FreeType
  symbol validation.

## 0.1.2

- Fixed the source-free managed component to resolve its namespaced ESP-GSP
  target and keep a configurable static component target.
- Included the FreeType provider in every target's prebuilt archive so dynamic
  CJK fonts work from the published package.

## 0.1.1

- Added typed generated helpers for application-backed List components.
- Improved low-memory image, animation, and transition behavior.
- Hardened portable compiler, simulator, header, and component-package checks.
- Made the built-in Dropdown indicator independent of application font glyphs.
- Reorganized user documentation around the recommended integration workflow,
  JSON/Kconfig authoring, public structures, ownership, and troubleshooting.
- Added a repository-scoped ESP-GSP UI vibecoding skill for natural-language
  and reference-image scene creation.

## 0.1.0

- Ahead-of-time JSON scene compiler with embedded assets and generated C APIs.
- RGB565/RGB888 rendering, dirty updates, transitions, gestures, and ESP-LCD
  presentation.
- Controls, layouts, lists, page flows, stacks, drawers, shapes, charts,
  needles, clocks, templates, and runtime positioning.
- Static and runtime text, images, animations, dynamic resources, and Canvas.
- Velocity-aware scene, PageFlow, and Drawer gestures with guarded short flicks,
  adaptive settling, and ESP touch-dropout filtering.
- Transform-aware dynamic/visibility damage and stable conditional-control hit
  testing for keyboards, checkbox/radio controls, and nested viewports.
- User-controlled Showcase for 800x480 RGB565/RGB888 and 1024x600 MIPI-DSI.
- Host simulator, reference renderer, compiler tests, and target examples.
