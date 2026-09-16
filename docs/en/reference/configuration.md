# Configuration Reference

Start with the generated `gsp_<symbol>_config()` result and
`ESP_GSP_ESP_LCD_CONFIG_INIT()`. Zero-valued optional fields select framework
defaults unless this document says otherwise. For callback views and other
public data structures, see the
[Application structure guide](../guide/application-structures.md).

## Bundle configuration

`esp_gsp_config_t` describes one compiled UI. The generated configuration
already supplies the bundle bytes, component directories, and JSON-derived
minimum requirements. Do not copy capacity and policy values into structure
members: all of them use `esp_gsp_config_set()` and stable field IDs.

| Field | Zero/default behavior | Set it when |
|---|---|---|
| `struct_size` | generated/config initializer sets the current structure size | never set it manually to another value |
| `schema_version` | initializer selects the current schema | never change it; mismatches are rejected |
| `abi_version` | initializer selects the current ABI | never change it; 0.1.x objects must be rebuilt |
| `reserved_config` | zero | reserved for a future ABI revision |
| `overrides` | eight empty inline entries and no extension table | normally leave it to `esp_gsp_config_set()`; bind a caller-owned read-only extension table only when more than eight fields differ |
| `bundle`, `bundle_size` | supplied by the generated configuration | constructing an advanced configuration without the generated header |
| `font_catalog` | no external shared font catalog | a launcher has opened a catalog used by bundles linked with external fonts |
| `directories`, `directory_count` | supplied by the generated configuration | constructing an advanced configuration that uses component-key APIs |
| `ttf`, `ttf_size` | no runtime outline-font fallback | using `gsp_add_bundle(DYNAMIC_FONT ...)` or an advanced caller-owned font blob |
| `disable_swipe` | multi-scene horizontal swipe remains enabled unless the scene disables it | the whole UI must reject scene swipes |
| `disable_bundle_crc` | bundle and nested resource/font CRCs are verified | trusted build-time assets live in a container or partition that is verified separately; structural and per-scene checks still run |
| `image_cache_bytes` | target-derived decoded-image budget | simultaneously visible decoded images require a measured larger or smaller budget |

### One per-instance configuration API

Use Kconfig for firmware-wide defaults and JSON for authored scene demand.
Only values that differ for one UI instance need code:

```c
esp_gsp_config_t config = gsp_product_config();

assert(esp_gsp_config_set(
           &config, ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, 24) ==
       ESP_GSP_CONFIG_SET_OK);
assert(esp_gsp_config_set(
           &config, ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS, 8) ==
       ESP_GSP_CONFIG_SET_OK);
```

This is the only normal per-instance policy/capacity path. It validates field
ownership and range immediately. Library-layout fields such as
`ESP_GSP_FIELD_TEXT_CAPACITY` are read-only and return
`ESP_GSP_CONFIG_SET_INVALID_FIELD`; inspect their generated
`ESP_GSP_BUILD_CAP_*` constants instead.

The resolver applies one deterministic order:

```text
project Kconfig -> GSPB JSON requirement -> esp_gsp_config_set() -> library cap
```

`DEFAULT_DYNAMIC_IMAGE_SLOTS` counts active logical targets, not all product
images. `CONTEXT_DEFAULT_INSTANCES` counts simultaneously live template
copies, including visible recycled List/Grid rows and application-created
widgets. The compiler records authored demand automatically; override only the
additional peak created exclusively by application code. Too-small values and
values above a library cap fail explicitly instead of truncating content.

`gsp_add_bundle(IMAGE_CACHE_BYTES ...)` sets `image_cache_bytes` in the
generated configuration so one budget applies at compile time and at runtime.

Left unset, the runtime derives it from the reachable heap: it covers the
compiled startup set, keeps the remaining headroom for runtime images whenever
`dynamic_image_slots` is non-zero, and never exceeds what the heap can serve,
including the heap's largest currently allocatable block. This matters on a
fragmented PSRAM heap: a decoded surface needs a sufficiently large
contiguous block. An unreachable budget stops eviction and turns every
miss into an allocation failure. A startup set larger than the reachable budget
is logged and the excess decodes lazily instead of failing initialization; the
preparation pass stops at the budget rather than evicting what it has already
decoded, which would make the scene re-decode an image every frame.

A decode that fails for lack of room leaves the image requestable and it
retries by itself once other images are released, so a transient shortage
shows a placeholder rather than a permanently blank image. Those retries are
bounded per shortage: an exhausted heap stops being retried until pixels are
actually released, at which point every failed image gets a fresh attempt.
Malformed payloads stay failed, since retrying cannot fix them.

### More than eight application overrides

Kconfig is the normal choice for firmware-wide defaults. Application
overrides are for values that differ between ESP-GSP instances or are chosen
at runtime. The first eight entries are stored inline. For additional settings,
bind a caller-owned, read-only table before creating the app:

```c
static const esp_gsp_config_override_entry_t extra_gsp_config[] = {
    { .field_id = ESP_GSP_FIELD_SCROLL_DECAY_MS, .value = 400 },
    { .field_id = ESP_GSP_FIELD_SCROLL_MAX_TICK_MS, .value = 80 },
    { .field_id = ESP_GSP_FIELD_SWIPE_SETTLE_MAX_MS, .value = 240 },
};

esp_gsp_config_t config = gsp_product_config();
esp_gsp_config_set_result_t result = esp_gsp_config_override_bind_external(
    &config.overrides, extra_gsp_config,
    sizeof(extra_gsp_config) / sizeof(extra_gsp_config[0]));
assert(result == ESP_GSP_CONFIG_SET_OK);
```

The table must remain alive and unchanged until `gsp_app_create()` returns;
static storage as above is the simplest safe choice. Field IDs are stable and
must not be duplicated between inline and external entries. The bind helper
validates IDs, ranges and duplicates before accepting the table.

### Decoded images across scenes

Before the first frame, the runtime prepares compiled images used by currently
visible commands. It then uses any remaining `image_cache_bytes` budget to
prefetch images on hidden pages. This gives the initial screen priority even
when resource-registration order starts with hidden content. A retained scene
reuses decoded images when you return to it; when the total exceeds the budget,
the least recently visited inactive scene releases its decoded surfaces first.

Budget for the visible decoded working set. If it does not fit, startup logs
`Visible scene images exceed cache budget`; the images that fit stay resident,
and skipped images use the placeholder and lazy-decode path. Increase
`image_cache_bytes` or reduce decoded image sizes to make the complete first
frame ready before presentation.

An animated scene change is the one point where two scenes are resident at
once, because the transition composites both. Applications that cannot afford
that peak should navigate with `ESP_GSP_NO_TRANSITION`.

When `ESP_GSP_FIELD_ENABLE_IMAGE_CACHE` is set to zero, compressed QOI and
RLE-family resources decode by region. Runtime-published PNG and JPEG images
require the decoded-image cache and are rejected in this mode.

## ESP-LCD configuration

`esp_gsp_esp_lcd_config_t` binds the UI to one display target and optional
touch device.

| Field | Zero/default behavior | Set it when |
|---|---|---|
| `struct_size` | `ESP_GSP_ESP_LCD_CONFIG_INIT()` sets the current size | never set it manually to another value |
| `display` | no valid default; use the target returned by the BSP | always |
| `presenter` | null; a presenter is created from `display` | advanced handoff reuses a borrowed presenter; when non-null, `display` is ignored |
| `touch` | input disabled | the BSP provides an `esp_lcd_touch_handle_t` |
| `touch_input_mode` | interrupt when available, otherwise polling | force polling or require interrupt mode |
| `touch_wake_from_isr` | no application-owned interrupt notification | an ISR-safe application callback must wake its own task from the same touch interrupt |
| `touch_wake_user_ctx` | null context is passed to `touch_wake_from_isr` | the wake callback needs application-owned context |
| `task_stack_size` | 12288 bytes, or 24576 bytes when a dynamic TTF/OTF blob is configured | stack measurement shows a different requirement |
| `task_stack_size_freetype` | 24576 bytes when a dynamic TTF/OTF blob is configured | measured FreeType paths justify another stack size |
| `task_priority` | 4 | integration scheduling requires a reviewed priority change |
| `task_stack_psram` | follows the consuming project's render-stack memory choice | the render stack should use PSRAM and callbacks obey its flash/cache restrictions |
| `perf_log` | false | five-second rendered-FPS logging is useful during measurement |
| `render_alignment` | zero fields request no expansion | a producer or display path requires dirty regions aligned in x, y, width, or height |

`ESP_GSP_TOUCH_INPUT_POLLING` overrides an available INT pin.
`ESP_GSP_TOUCH_INPUT_INTERRUPT` requires one and fails if another callback
already owns it or interrupt registration fails. The default AUTO mode logs a
warning and continues in polling mode when interrupt setup is unavailable.
`touch_wake_from_isr` runs from that registered touch ISR in addition to the
framework wake notification. It must not perform I2C or ESP-GSP operations;
use it only for ISR-safe notification of application-owned work. It is not
called when touch input uses polling.

Keep display presentation mode on `ESP_DISPLAY_PRESENT_MODE_AUTO` for the
normal path. Panel classification, framebuffer exposure, byte swapping,
rotation and TE configuration belong to the BSP display target; see
[Display Presentation](../guide/display.md).

## Adjusting memory use

Distinguish internal RAM, PSRAM and contiguous-block shortages before changing
settings. `image_cache_bytes` limits decoded-image caching, not all GSP memory.
Increasing it does not add heap memory. Zero selects automatic sizing; it does
not disable the cache.

Read device heaps on demand, including after initialization fails; no UI handle
is required:

```c
#include "esp_gsp_debug.h"

esp_gsp_heap_stats_t heap;
if (esp_gsp_heap_stats(&heap)) {
    // Inspect heap.internal and heap.psram:
    // free_bytes and largest_free_block, both in bytes.
}
```

These are device-wide heaps including application and driver allocations, not
GSP-only accounting. Each heap is sampled separately and concurrent allocations
can change the result. Absent PSRAM returns zero values. Hosts without these
capabilities return `false` and clear the output. Query on demand from task context. Account separately for DMA capabilities, alignment
and simultaneous allocations.

| Observation | First adjustment | Tradeoff or condition |
|---|---|---|
| Internal RAM low, PSRAM available | Check whether render/decode stacks and animation frames can use PSRAM | Stack callbacks must satisfy Flash/cache restrictions; Prefer PSRAM animation frames may still fall back to internal RAM |
| No evictable image-cache room | Budget for active images and pending replacements together | Increase `image_cache_bytes` only with heap headroom; otherwise reduce simultaneously resident decoded images |
| Image allocation failed | Compare requested bytes with both largest blocks; check display buffers and codec scratch | Allocation also depends on heap capabilities and contiguous space; reducing cache or disabling snapshots can increase rendering/decoding work |
| Total memory low | Review display buffers, cache budget, transition snapshots and background decoding | Disabling background decoding moves work to the render task; display-buffer changes belong to the BSP/presenter |

The first heap allocation failure in each image cache also logs both heaps.
Later failures retain the requested size without repeatedly scanning heaps;
use the on-demand query for a fresh reading.

`esp_gsp_media_stats()` exposes cache usage, peaks, allocation failures and decode
failures. These counters do not cover every budget rejection. Combine them with
the failure log; `LIMIT_EXCEEDED` alone does not prove heap exhaustion. Observe
the media API's UI serialization requirements when reading its counters.

Task-stack settings are under `ESP-GSP → Advanced settings (optional) → Project
runtime defaults → Runtime tasks`. The default render stack is 12 KiB, or 24 KiB
with dynamic FreeType; the decode stack is 4 KiB. Before enabling PSRAM stacks,
audit Flash, NVS and filesystem calls made by the task and its callbacks; route
them to an internal-stack task or Flash dispatcher where needed. Initialization
still uses the caller's stack: changing the render stack does not change the
`app_main()` stack. Reduce stack sizes only after measuring remaining stack
across all enabled paths.

Startup `memory` logs inspect the actual stack address, including IDF global
external-stack placement and fallback. Failed creation reports
`requested_memory` because there is no allocated stack to inspect. Use these diagnostics to choose stack placement and memory budgets.

## Build-time bundle options

```cmake
gsp_add_bundle(<component-target>
    [SCENES <scene0.json> [scene1.json ...]]
    [PIXEL_FORMAT rgb565|rgb888]
    [IMAGE_CACHE_BYTES <bytes>]
    [DYNAMIC_FONT <font.ttf>]
    [SYMBOL <c_identifier>]
    [PROFILE <expert-profile.yaml>]
    [DEPLOYABLE])
```

With no `SCENES` argument, ESP-GSP discovers and lexically sorts
`PROJECT_DIR/scenes/*.json`. Configuration fails with a direct error when that
directory has no JSON files. Explicit paths are the advanced path for scattered
JSON, a subset of scenes, or multiple bundles. `PIXEL_FORMAT` defaults to
`rgb565` when `PROFILE` is not supplied. `PROFILE` is an expert compiler
override.

`DEPLOYABLE` is opt-in. It adds one self-describing GMD metadata member per
scene so the resulting GSPB can be loaded without its generated C directory.
It does not select a partition, transport, update protocol, rollback policy or
signature scheme. Without this option, bundle output and the generated-header
startup path remain unchanged.

The default platform profile uses `codec: auto`. On targets that advertise a
hardware JPEG decoder, an image takes the hardware JPEG path when it clears
`image_auto_min_pixels`, is not runtime-scaled, its decoded surface fits the
cache budget, and JPEG is smaller than STORE. The floor is a pixel count rather
than a per-side limit, matching the decoder, which only constrains the total. This hardware
decision runs before QOI/RLE size heuristics. Alpha images use the JPEG_A8
container: the colour plane decodes in hardware, the A8 plane stays lossless.
Use explicit `codec: raw` for zero-decode MMAP data, `codec: lossless` for
exact pixels, or set `image_auto_allow_lossy: false` in an expert profile to
disable automatic JPEG. RGB888 alpha has no JPEG container and stays lossless.
The compiler emits 4:2:0 JPEG, so the hardware decoder processes 16x16 MCU
blocks. For non-aligned dimensions it automatically edge-extends the encoded
image and allocates an MCU-aligned cache surface while preserving the authored
logical size. The hardware decoder can therefore write directly into PSRAM;
rendering reads only the logical area through the aligned stride. The padded
cache size is included in the scene budget and reported in the execution plan.
On builds without PSRAM, the generated default disables the image cache and
therefore does not advertise hardware JPEG to `codec: auto`. Supplying an
explicit `IMAGE_CACHE_BYTES` internal-RAM budget opts back in; an expert
`PROFILE` remains fully caller-controlled.

## Kconfig (ESP-IDF) capacity and policy tunables

On ESP-IDF builds, product defaults are exposed under
`Component config → ESP-GSP`. CMake emits them into an independent project
bridge that provides exactly one strong configuration definition. The same
bridge contract is used by source, prebuilt, and Host builds; the published
archive does not contain the consuming product's settings.

In a source-free prebuilt component, runtime policies and heap-backed
capacities remain visible and effective. Values fixed into the archive are
published as generated `ESP_GSP_BUILD_CAP_*` constants and do not appear in
Kconfig. The
compiler records scene requirements in JSON-derived bundle metadata, the
runtime raises AUTO capacities as needed, and initialization rejects a
requirement above the archive's published build capability. This keeps ordinary
scenes self-sizing without preventing product or per-instance configuration.

For the complete workflow—interactive configuration, reproducible defaults,
target-specific fragments, precedence, and validation—read the
[ESP-IDF Kconfig guide](kconfig.md).

At startup each instance resolves project defaults, versioned GSPB
requirements, explicit application overrides, and finally build capabilities.
The resulting policy and capacities are stored on that instance and remain
read-only for its lifetime. Two instances may therefore use different app
overrides without changing each other. Platform-scoped task settings are
locked while the shared platform service is active.

Runtime capacities back heap allocations, including List variable-stride row
storage and StackView page storage. They never set a fixed array bound or a
public/private protocol layout. The named build capabilities are the only
values permitted to bound unavoidable scratch arrays inside the archive.

The Kconfig help is the authoritative per-symbol reference. The groups below
show which product trade-off each setting controls.

Authored capacities default to AUTO: the compiler records demand according to
the runtime lifetime of each resource. Reusable pools use the maximum per-scene
demand, while retained List bindings and dynamic-text backing storage are
summed across the bundle. Operational budgets that content cannot predict,
such as application timers, cache policy, dirty rectangles and
application-created widgets, keep balanced configurable defaults. Increasing
authored scene density therefore needs no benchmark- or application-specific
framework setting.

| Group | Important symbols | What scales |
|---|---|---|
| Resident UI pools | `MAX_SCENES`, `MAX_TIMERS`, `MAX_WIDGETS`, `MAX_ANIMATIONS`, `MAX_LISTS`, `CANVAS_SLOTS`, `MAX_ASSET_ANIMS` | resident UI memory; exhaustion returns/logs a limit error |
| List/text pools | `LIST_MAX_SLOTS`, `LIST_TEXT_SLOTS`, `TEXT_SLOTS`, `TEXT_CAPACITY` | visible rows, shaped-text heap, and the command inline fast path |
| Component limits | `COMPONENT_INSTANCES`, `STACK_VIEW_MAX_DEPTH`, `COMPONENT_BATCH_MAX`, `TRANSACTION_UPDATE_CAPACITY`, `COMPONENT_OVERLAY_COMMANDS` | authored manager instances and stack depth use AUTO; larger transaction batches use temporary heap |
| Image/font limits | `DEFAULT_DYNAMIC_IMAGE_SLOTS`, `MAX_DYNAMIC_IMAGE_TARGETS`, `MAX_FONTS_PER_SCENE`, `FREETYPE_CACHE_GLYPHS`, `FREETYPE_GLYPH_MAX_PX` | resource-view arrays, cache metadata and glyph bitmap heap |
| Renderer scratch | `DIRTY_RECT_CAPACITY`, `RENDER_CLIP_STACK_DEPTH`, `RENDER_TILE_SPAN_CAPACITY` | persistent damage arrays and renderer stack; tile-span overflow falls back to a linear scan |
| Input | `MAX_TOUCH_POINTS`, `TOUCH_RELEASE_CONFIRM_POLLS` | two-contact build capability and polling-mode release latency |
| Animation | `ANIM_FRAME_MEMORY_*`, `ANIM_MAX_FRAME_BYTES`, `ANIM_INTERNAL_FRAME_MAX_BYTES`, `ANIM_PATCH_RECTS`, `ANIM_REFERENCE_COMMANDS` | frame-buffer heap placement, safety bounds and per-animation resident metadata |
| Image-cache policy | `ENABLE_IMAGE_CACHE`, `IMAGE_CACHE_ENTRIES`, `IMAGE_CACHE_SHORTAGE_RETRIES`, `IMAGE_CACHE_AUTO_*` | decoded-image heap budget and retry latency |
| Tasks | `ENABLE_ASYNC_DECODE`, `RENDER_TASK_STACK_SIZE*`, `DECODE_TASK_STACK_SIZE`, task priorities and decode poll interval | internal SRAM task stacks, scheduling and decode latency |
| Acceleration | `ACCEL_*`, `PPA_*` | async transaction capacity, timeout and the CPU/hardware crossover thresholds |
| Motion/latency | `ACTIVE_TICK_MS`, `IDLE_POLL_MS`, `POINTER_POLL_MS`, `SCROLL_*`, `SWIPE_*`, `*_SETTLE_MS`, `*_SLOP_PX` | CPU wake rate and interaction feel; sample capacities also change resident state size |
| Transition memory | `ENABLE_TRANSITION_SNAPSHOTS` | whether supported transitions may allocate two scene snapshots |

Useful SRAM relationships for capacity planning are:

- Vector rendering needs additional memory even when image caching is disabled.
  Measure peak usage with the application's actual assets and transitions;
  memory is managed automatically.
- FreeType bitmap storage is approximately
  `FREETYPE_CACHE_GLYPHS * FREETYPE_GLYPH_MAX_PX^2` bytes.
- List text memory grows with the number of visible rows, text slots and text
  length. Measure peak usage with representative application data.
- Increasing `QUEUE_DEPTH` and `TEXT_CAPACITY` increases memory use. Adjust
  these only when queue diagnostics or text-update workloads justify it;
  `TEXT_CAPACITY` is not a maximum string length.
- `MAX_TOUCH_POINTS` is fixed at the build capability of two, so a
  prebuilt archive and source build expose the same pinch functionality.
- `LIST_MAX_SLOTS` applies per List/Grid viewport. For a Grid, required slots
  are `(visible rows + overscan) * columns`; `MAX_LISTS` covers retained
  bindings retained until the UI instance is destroyed.
- `INSTANCE_STATES_PER_SLOT` applies to fields within one template, while
  `instance_slots` applies to simultaneously live template copies. They are
  independent multipliers and both must cover the authored control.
- Disabling async decode saves its task stack and RTOS objects but makes the
  render task pump decoding. Disabling transition snapshots avoids their peak
  allocation but changes the supported transition path.

Lower bounds are intentional: they keep the implementation's mandatory
sentinels and atomic operations valid. Raise a limit only after budgeting its
pool or stack multiplier; lower one only after exercising the largest authored
scene and the relevant control path.

AUTO is limited to capacities that `gspc` can derive from authored content.
The GSPB requirements member carries exact scene, List, StackView, text, image,
compiled-animation, instance, per-instance-state and glyph-run minima.
Glyph-run capacity uses the same schema-generated formula in Python and C.
`DIRTY_RECT_CAPACITY` remains a normal capacity with default 32 because authored
content cannot predict per-frame damage fragmentation. Old bundles without the
versioned requirements member are rejected rather than guessed.

Protocol constants (format offsets, codec ids, driver extension strides,
`ESP_GSP_IMAGE_REFS_PER_SLOT`, animation handle encoding) are fixed ABI and
wire-format definitions. Keep them consistent between the compiler, runtime
and prebuilt library.
