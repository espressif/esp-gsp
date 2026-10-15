# Changelog

## 1.7.0

Use with GSPC 0.8.0 and simulator 1.7.0.

- Add ESP32-H4, ESP32-C2 and classic ESP32 packages.
- Add opt-in application-specific runtime linking with dynamic capability declarations and bundle admission checks.
- Reduce rendering, binding, text and idle work; bound command batches and preserve shared accelerator settings.
- Preserve PNG alpha and JPEG colors; support JPEG on alpha targets and fix animation timing and callbacks.
- Isolate Canvas views and expose request-scoped image publication.
- Fix paint/hit order, TabView selection, dynamic geometry, template animation damage, scrolling and snapshots.
- Improve PC Backend loading, input, timers and error handling.

### Upgrading from 1.6.0

Require `esp_display_present ^1.1.2`. Regenerate affected scenes and headers with
GSPC 0.8.0, and rebuild precompiled libraries, PC Backends and applications with
runtime/simulator 1.7.0.
New paint order and dynamic hit geometry use GSB 2.20/2.21.
Existing resource replay and public ABI versions are unchanged.

## 1.6.0

Use with GSPC 0.7.0 and simulator 1.6.0.

- Add Grid and wrapped Row/Column layouts, reusable components and Button press styles with edge jitter tolerance.
- Add application hooks, observers and named, length-checked Canvas APIs.
- Reduce rendering work, retained memory and stack use; support optional feature linking.
- Improve JPEG scaling and image memory handling; fix PNG/QOI/RLE decoding and PIE isolation.
- Use the consuming firmware's tick rate and require `esp_display_present ^1.1.1` for the TE wakeup fix.
- Improve diagnostics, simulator sessions and native backends; fix animated navigation and build/package integration.
- Add trusted in-process simulator Backends using `.so`, `.dll` or `.dylib`
  libraries on Linux, Windows and macOS, with defensive ABI and exact GSP
  version validation before application code starts.
- Reduce Socket Backend latency by disabling Nagle on accepted TCP connections,
  while retaining the out-of-process transport for isolation and diagnostics.
- Build executable and shared-library variants from `gsp_add_backend()`, and use
  the shared library by default in the PC Backend runner.
- Stop dispatching application callbacks after an in-process Backend observes
  host shutdown, including timers that become due at the shutdown boundary.

### Upgrading from 1.5.1

Rebuild precompiled libraries, generated headers and applications together.

- Cancelled contacts emit `cancel` instead of `release/click`; use it to stop actions started by `press`.
- Canvas rejects shared resource views, including legacy push APIs. Stop and flush Canvas before switching to image setters.
- Image buffers on PSRAM targets no longer fall back to internal RAM. Reserve PSRAM for the decode working set or reduce image sizes.

Reconfigure and rebuild PC Backend build directories. Their generated manifest
changes from version 1 to version 2 and now contains `backend_library`; stale
manifests are rejected by the current build helper. Dynamic Backend libraries
must be rebuilt against ESP-GSP 1.6.0 because the simulator requires an exact
GSP version match.

`tools/sim_bridge/run.py` now loads the shared library by default; use this mode
for lower simulator call latency. Pass `--socket-backend` to retain the 1.5.1
out-of-process behavior, including process isolation. Only load trusted dynamic
Backend libraries because they run inside the simulator process.

## 1.5.1

- Reduce Bundle loading overhead by reusing validated startup views, using
  indexed bind lookup, and accelerating CRC checks on P4/S31.
- Keep CRC enabled by default and make explicit opt-out cover GSPB and all
  nested GSB/GRB/GFB checks while retaining structural validation.
- Protect PIE rendering from rounding modes set by other tasks and reduce
  solid-fill and rounded-border work.
- Repaint raw bind effect updates across both old and new image bounds, and
  accept non-negative signed values for unsigned component properties.
- Decode runtime PNGs into the placeholder's alpha layout; reject transparent
  pixels on opaque targets before writing the destination.

### Upgrading from 1.5.0

Use GSPC 0.6.1 with ESP-GSP and simulator 1.5.1. Public ABI and binary format
versions are unchanged. Regenerate headers to obtain the corrected Image setters.
Remove `cyclic` from List objects, including `cyclic: false`; Wheel and PageFlow
continue to support it.

Deployable `make_config()` now inherits the CRC policy passed to `open()`.
After opening with `verify_crc=false`, set `config.disable_bundle_crc=false`
if runtime creation should verify the package again.

## 1.5.0

- Improve gestures, page transitions and component motion events.
- Reduce rendering memory usage and speed up bundle validation and startup.
- Fix transparency, timer and diagnostic concurrency, image handling and chart redraws.
- Extend simulator automation and PC bridge controls.
- Expand HMI examples and media setup; add GSPC image policies and authoring improvements.

### Upgrading from 1.4.0

Use GSPC 0.6.0 with ESP-GSP and simulator 1.5.0. Rebuild bundles, generated headers
and application code with matching libraries. Existing bundles and public ABI versions
remain supported; new transparency features require the matching runtime.

- GSPC now rejects previously ignored invalid or unknown widget fields.
- Default `CONFIG_ESP_GSP_DRAG_START_PX` changes 24 → 12 and
  `CONFIG_ESP_GSP_SCENE_SWIPE_MIN_FLING_PX` changes 32 → 24; explicit settings are unchanged.
- Example media downloads at build time; prepare a cache for
  [offline builds](examples/README.md#example-asset-retrieval).

## 1.4.0

- Reduce Flash and RAM usage with optional font/JPEG support and scene-local resources.
- Improve Arc rendering and resource handling.
- Add clipping, state images, cyclic wheels, and dynamic chart series.
- Improve diagnostics, resource budgets, and build compatibility.
- Fix scrolling, text layout, generated APIs and image preparation.
- Improve resource diagnostics and UI development guidance.
- Add agent-oriented GSPC cards, versioned API sidecars and named simulator automation.
- Reduce component package size while retaining all supported targets and examples.

Update GSPC and ESP-GSP together. Applications using runtime TTF data must enable
[dynamic font support](docs/en/guide/media-and-data.md#raster-images-and-fonts).

## 1.3.1

- Fix a crash when clearing message lists during session changes.
- Fix text alignment, ellipsis and multiline layout in dynamic labels and template rows.
- Preserve list state when layout updates or binding submissions fail.

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
