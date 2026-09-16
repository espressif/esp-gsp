# Changelog

## Unreleased

- Add child clipping, state images, cyclic wheels and dynamic chart series.
- Fix scrolling, text layout, generated APIs and image preparation.
- Improve resource diagnostics and UI development guidance.

Rebuild GSPC and the runtime together for the new controls.

## 1.3.0

- Add SVG rendering, visual effects and animation controls.
- Add filesystem media and font loading.
- Improve rendering, message lists and resource management.
- Update platform compatibility, examples and packaging.

Use GSPC 0.4.0 and simulator 1.3.0. JPEG quality follows the Profile;
set `quality: 80` to retain the previous default. SVG support increases firmware size.

## 1.2.0

- Use the separately maintained `esp_display_present` ^1.0.2 dependency.
- Add runtime image rotation, automatic UI capacity sizing and longer dynamic text.
- Improve message updates, image scaling, low-memory behavior and diagnostics.
- Add native application backends and the portable `tools/sim_bridge` integration,
  with PC examples and dynamic List/Grid, image and Canvas support.
- Fix packaged Benchmark builds and startup page visibility.

Use GSPC 0.3.0 with this release.

## 1.1.0

- Complete bilingual API, Widget, configuration and compatibility documentation.
- Improve standalone-tool setup and detect incompatible compilers at configure time.
- Support the conventional `scenes/` directory and configurable prebuilt packages.
- Distribute GSPC and simulators separately from the firmware component.
- Add the installable ESP-GSP UI authoring Skill and clarify control events.

## 1.0.0

- Publish ESP-GSP and `esp_display_present` under the official `espressif`
  Component Registry namespace.
- Align examples and documentation with the 1.0.0 release.

## 0.2.10

- Use a separately installed, compatible GSPC through `GSPC_EXECUTABLE`.
- Allow packaged simulators to open existing `.gspb` files without GSPC.

## 0.2.7

- Fix prebuilt-package integration with Registry-installed `esp_display_present`.

## 0.2.6

- Fix touch support when linking prebuilt ESP-IDF packages.

## 0.2.5

- Add the public Simulator session SDK and SDL, headless, Web and ESP-IDF adapters.
- Improve packaged simulator integration and preserve legacy CMake target support.

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

- Fix loading empty scenes and scenes without optional components.

## 0.2.3

- Fix a watchdog timeout when switching applications during PPA rendering.

## 0.2.2

- Add Dropdown `open_direction`: `down` (default) or `up`.

## 0.2.1

- Size image caches and UI capacities from scene requirements.
- Apply effective configuration consistently and reject incompatible bundles.
- Improve configuration handling for prebuilt packages.

## 0.2.0

- Apply application `menuconfig` settings to prebuilt libraries and diagnose
  insufficient UI capacities.
- Add native and WebAssembly simulators to the component package, with improved
  image and dynamic-font support.

Regenerate 0.1.x bundles and generated configuration files with GSPC 0.2.0.

## 0.1.3

- Improve CMake integration for Registry-installed components.

## 0.1.2

- Fix managed-component integration and dynamic CJK font support in prebuilt packages.

## 0.1.1

- Add generated List helpers and improve low-memory media and transitions.
- Fix the Dropdown indicator and improve integration documentation.
- Add the ESP-GSP UI authoring Skill.

## 0.1.0

- Introduce JSON scenes, generated C APIs, RGB565/RGB888 rendering and ESP-LCD support.
- Add controls, layouts, lists, navigation, gestures, text, media and Canvas.
- Provide a host simulator and device examples.
