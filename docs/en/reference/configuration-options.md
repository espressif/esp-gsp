# ESP-GSP configuration reference

This reference covers `Kconfig` and `config/esp_gsp_config_schema.yml`. Runtime settings are firmware defaults and remain configurable with source or prebuilt components. Library capabilities are read-only limits published by the archive; they are intentionally not Kconfig options.

## Capacity and feature limits / Core object and component pools

### `CONFIG_ESP_GSP_MAX_SCENES`

Maximum number of scenes held in gsp_ui_core_t

- Project default: `0`
- Type/range: `int` / `0..65534`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the exact scene count stored in the GSPB. A non-zero value is an explicit project budget and generation fails when the bundle needs more scenes.

### `CONFIG_ESP_GSP_MAX_TIMERS`

Maximum simultaneous application timers

- Project default: `8`
- Type/range: `int` / `1..32`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Timer slot pool in gsp_ui_core_t. Timer registration fails (ESP_GSP_ERR_LIMIT_EXCEEDED) when the pool is full.

### `CONFIG_ESP_GSP_MAX_WIDGETS`

Maximum simultaneous template-widget instances

- Project default: `16`
- Type/range: `int` / `1..255`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

The handle index width is derived from this value, up to the uint8_t-backed pool limit of 255. The remaining handle bits hold a recycling generation. Each slot costs about 20 bytes.

### `CONFIG_ESP_GSP_MAX_ANIMATIONS`

Maximum concurrent runtime animations

- Project default: `32`
- Type/range: `int` / `0..128`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

esp_gsp_anim_slot_t pool in gsp_ui_core_t (~56 B per slot). Exhaustion degrades gracefully (legacy tween) and logs.

### `CONFIG_ESP_GSP_MAX_LISTS`

Retained List/Wheel bindings per UI instance

- Project default: `0`
- Type/range: `int` / `0..256`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) sums authored List/Wheel bindings across bundled scenes because handles currently have UI-instance lifetime. A non-zero value is an explicit project budget.

### `CONFIG_ESP_GSP_LIST_MAX_SLOTS`

Row slots per visible list viewport

- Project default: `0`
- Type/range: `int` / `0..65535`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) writes the exact authored requirement into GSPB and runtime allocates that many rows. A non-zero project value is an explicit upper bound and generation fails when it is below the requirement.

### `CONFIG_ESP_GSP_LIST_TEXT_SLOTS`

Dynamic text slots per list template row

- Project default: `0`
- Type/range: `int` / `0..65533`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the requirement derived from authored row templates. A non-zero value explicitly reserves at least that many shaped-text slots per visible row.

### `CONFIG_ESP_GSP_COMPONENT_INSTANCES`

Runtime component-instance pool

- Project default: `0`
- Type/range: `int` / `0..256`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) keeps the legacy baseline of eight and raises it from each scene's generated semantic directory when authored components need more slots. A non-zero project value reserves additional application-created concurrency but never limits authored content. Applications that need a strict budget can set a non-zero per-instance override; values below authored demand fail initialization instead of truncating it.

### `CONFIG_ESP_GSP_STACK_VIEW_MAX_DEPTH`

Maximum StackView navigation depth

- Project default: `0`
- Type/range: `int` / `0..64`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the deepest authored StackView requirement. A non-zero value is an explicit project budget.

## Capacity and feature limits / Renderer and frame-planning capacity

### `CONFIG_ESP_GSP_COMPONENT_OVERLAY_COMMANDS`

Base component overlay command capacity

- Project default: `32`
- Type/range: `int` / `2..128`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Resident overlay commands available to component chrome before the two commands reserved for each List/Wheel fade. Each command costs about 40 bytes. Exhaustion drops later overlay commands and logs the configured capacity.

### `CONFIG_ESP_GSP_DIRTY_RECT_CAPACITY`

Dirty rectangles retained per frame

- Project default: `32`
- Type/range: `int` / `4..128`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Shared capacity for scene dirty storage, portable frame planning and the ESP-LCD presenter damage contract. Each persistent frame loop keeps two arrays at about 16 bytes per entry. When the set is full, GSP compacts it or promotes the frame to a full redraw. This is a project policy, not a scene-derived AUTO requirement.

## Capacity and feature limits / Canvas, text, image, and font pools

### `CONFIG_ESP_GSP_CANVAS_SLOTS`

External frame-sink (canvas) targets

- Project default: `2`
- Type/range: `int` / `1..8`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Concurrent esp_gsp_canvas_* bindings.

### `CONFIG_ESP_GSP_MAX_ASSET_ANIMS`

Compiled per-scene animation slots

- Project default: `0`
- Type/range: `int` / `0..255`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the maximum per-scene anim_qoi count stored in the GSPB. A non-zero value is an explicit project budget.

### `CONFIG_ESP_GSP_TEXT_SLOTS`

Retained dynamic-text shaping slots

- Project default: `0`
- Type/range: `int` / `0..65533`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) sums scene text binds across the bundle because published glyph runs retain their backing storage. A non-zero project value reserves at least that many slots; each slot starts with about 386 bytes of inline run storage and grows on demand.

### `CONFIG_ESP_GSP_DEFAULT_DYNAMIC_IMAGE_SLOTS`

Default simultaneous runtime image targets

- Project default: `0`
- Type/range: `int` / `0..32767`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the GSPB requirement. Applications that deliberately disable runtime images use esp_gsp_config_set() with value zero, which is distinguishable through the override bitset.

### `CONFIG_ESP_GSP_FREETYPE_CACHE_GLYPHS`

Default FreeType glyph cache entries

- Project default: `32`
- Type/range: `int` / `1..512`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default FreeType cache capacity. Override one UI instance with esp_gsp_config_set() and ESP_GSP_FIELD_FREETYPE_CACHE_GLYPHS. Pixel storage is entries * glyph_max_px * glyph_max_px bytes.

### `CONFIG_ESP_GSP_FREETYPE_GLYPH_MAX_PX`

Default maximum FreeType glyph width and height

- Project default: `40`
- Type/range: `int` / `8..256`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default maximum cached glyph dimension. Override one UI instance with esp_gsp_config_set() and ESP_GSP_FIELD_FREETYPE_GLYPH_MAX_PX. Memory grows quadratically; glyphs larger than this bound cannot be cached by the dynamic-font fallback.

## Project runtime defaults / Rendering acceleration

### `CONFIG_ESP_GSP_ACCEL_MAX_DRIVERS`

Maximum registered acceleration backends

- Project default: `4`
- Type/range: `int` / `1..16`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Maximum number of hardware or software acceleration drivers kept in the runtime registry. Increase only when the application registers more independent backends at the same time.

### `CONFIG_ESP_GSP_ACCEL_MAX_PENDING_BLITS`

Maximum asynchronous accelerator blits in flight

- Project default: `4`
- Type/range: `int` / `1..16`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Shared generic/PPA/DMA2D batching limit. Higher values may improve overlap but reserve more internal bookkeeping and transaction slots.

### `CONFIG_ESP_GSP_ACCEL_SYNC_TIMEOUT_MS`

Accelerator completion timeout in milliseconds

- Project default: `500`
- Type/range: `int` / `1..10000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Maximum wait for an asynchronous accelerator operation to complete. A timeout reports a backend failure instead of blocking the render task indefinitely.

### `CONFIG_ESP_GSP_ACCEL_MIN_BLIT_PIXELS`

Minimum pixels for accelerated copy

- Project default: `8192`
- Type/range: `int` / `0..1048576`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Copy regions smaller than this remain on the software path because accelerator setup can cost more than the copy. Zero allows every eligible copy to use an accelerator.

### `CONFIG_ESP_GSP_PPA_MIN_FILL_PIXELS`

Minimum pixels for PPA fill

- Project default: `32768`
- Type/range: `int` / `0..1048576`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Fill regions below this size stay on the software renderer. Tune only from same-board measurements; zero permits every eligible fill.

### `CONFIG_ESP_GSP_PPA_MIN_BLEND_PIXELS`

Minimum pixels for PPA blend/fade

- Project default: `8192`
- Type/range: `int` / `0..1048576`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Blend or fade regions below this size stay on the software renderer. Tune only from same-board measurements; zero permits every eligible blend.

### `CONFIG_ESP_GSP_PPA_RUNTIME_SCALE`

Enable PPA SRM for runtime image scaling

- Project default: `false`
- Type/range: `bool` / `boolean`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Opt in to hardware bilinear filtering for large, fully visible, opaque RGB565/RGB888 STORE images. The existing software renderer uses nearest-neighbour sampling, so keep this disabled when exact pixel compatibility is required.

### `CONFIG_ESP_GSP_PPA_MIN_SCALE_PIXELS`

Minimum destination pixels for runtime PPA scaling

- Project default: `32768`
- Type/range: `int` / `0..1048576`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Destination images below this size keep software scaling so PPA setup does not dominate small operations. Zero permits every eligible scale.

### `CONFIG_ESP_GSP_PPA_FILL_MAX_SPAN_PIXELS`

Maximum pixels in one PPA fill span

- Project default: `524288`
- Type/range: `int` / `1024..16777216`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Splits very large PPA fills into bounded spans. Lower values reduce the duration of one submitted operation; higher values reduce setup count.

## Project runtime defaults / Runtime pools and image cache

### `CONFIG_ESP_GSP_IMAGE_CACHE_ENTRIES`

Default decoded-image cache entry count

- Project default: `16`
- Type/range: `int` / `0..512`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Metadata entry default for a scene image cache. GSP automatically raises this when the compiled startup set plus runtime image slots requires more entries, so lowering it never truncates that set.

### `CONFIG_ESP_GSP_IMAGE_CACHE_SHORTAGE_RETRIES`

Image-cache allocation shortage retries

- Project default: `3`
- Type/range: `int` / `0..16`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Number of cache-budget eviction/retry rounds after an image surface allocation fails. Higher values may recover fragmented memory at the cost of additional decode latency.

### `CONFIG_ESP_GSP_CONTEXT_DEFAULT_GLYPH_RUNS`

Default low-level context glyph-run slots

- Project default: `0`
- Type/range: `int` / `0..65535`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the versioned GSPB requirements. A non-zero value is an explicit project capacity and startup fails if the bundle needs more.

### `CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES`

Default shared template instance slots per scene

- Project default: `0`
- Type/range: `int` / `0..65534`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the template and visible-row requirement stored in the GSPB. A non-zero value explicitly reserves at least that many slots; startup fails if it is below the bundle requirement.

### `CONFIG_ESP_GSP_INSTANCE_STATES_PER_SLOT`

Runtime state values reserved per template instance

- Project default: `0`
- Type/range: `int` / `0..65535`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: yes

AUTO (0) uses the largest authored template state count stored in the GSPB. A non-zero value is an explicit project budget.

### `CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MIN_BYTES`

Minimum automatic image-cache budget with PSRAM

- Project default: `65536`
- Type/range: `int` / `0..16777216`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Floor used only when PSRAM has enough reachable free space. Zero removes the floor. This is a budget, not an eager allocation.

### `CONFIG_ESP_GSP_IMAGE_CACHE_AUTO_MAX_BYTES`

Maximum automatic image-cache budget

- Project default: `4194304`
- Type/range: `int` / `0..67108864`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Cap for the automatically derived cross-scene decoded-image budget. Zero removes the cap. Explicit API/generated budgets still win.

### `CONFIG_ESP_GSP_IMAGE_CACHE_HOST_DEFAULT_BYTES`

Non-ESP automatic image-cache budget

- Project default: `2097152`
- Type/range: `int` / `0..67108864`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default used where heap-capability discovery is unavailable, such as host integrations. This is a budget, not an eager allocation.

## Project runtime defaults

### `CONFIG_ESP_GSP_ENABLE_IMAGE_CACHE`

Enable decoded-image cache by default

- Project default: `true`
- Type/range: `bool` / `boolean`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Runtime product setting. Disable to use bounded region decoding. The prebuilt archive still contains both implementations.

### `CONFIG_ESP_GSP_ENABLE_TRANSITION_SNAPSHOTS`

Enable full-screen transition snapshots

- Project default: `true`
- Type/range: `bool` / `boolean`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Runtime product setting. Disable to select the lower-memory transition path. The prebuilt archive supports both paths.

## Project runtime defaults / Runtime tasks / Render task

### `CONFIG_ESP_GSP_RENDER_TASK_STACK_PSRAM`

Allocate render task stack in PSRAM

- Project default: `false`
- Type/range: `bool` / `boolean`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Moves the render task stack to PSRAM, saving 12 KiB of internal SRAM by default, or 24 KiB with FreeType. Runtime startup fails if the configured stack cannot be allocated from PSRAM. PSRAM stacks cannot be used while the flash cache is disabled. Keep the default if render callbacks may access flash, NVS or filesystems. Applications that require those operations can dispatch them to an internal-stack task or initialize espressif/esp_flash_dispatcher.

### `CONFIG_ESP_GSP_RENDER_TASK_STACK_SIZE`

Render task stack size in bytes

- Project default: `12288`
- Type/range: `int` / `4096..65536`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default used when esp_gsp_esp_lcd_config_t.task_stack_size is zero and dynamic FreeType is not attached. Lower only after checking the task's stack high-water mark on every enabled renderer/codec path.

### `CONFIG_ESP_GSP_RENDER_TASK_STACK_SIZE_FREETYPE`

Render task stack size with FreeType in bytes

- Project default: `24576`
- Type/range: `int` / `8192..65536`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default render stack when a runtime TTF/OTF face is attached.

### `CONFIG_ESP_GSP_RENDER_TASK_PRIORITY`

Default render task priority

- Project default: `4`
- Type/range: `int` / `1..24`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Used when the platform API priority is zero. The asynchronous decode task defaults one priority below this value.

## Project runtime defaults / Runtime tasks / Background decode task

### `CONFIG_ESP_GSP_ENABLE_ASYNC_DECODE`

Enable background image decode task

- Project default: `true`
- Type/range: `bool` / `boolean`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Creates a decode worker when the current scene has an image cache. Disabling saves its stack and RTOS objects but moves decode work to the render-task fallback and may increase frame latency.

### `CONFIG_ESP_GSP_DECODE_TASK_STACK_PSRAM`

Allocate decode task stack in PSRAM

- Project default: `false`
- Type/range: `bool` / `boolean`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Moves the background decode task stack to PSRAM, saving 4 KiB of internal SRAM with the default stack size. The worker remains disabled if its stack cannot be allocated from PSRAM. PSRAM stacks cannot be used while the flash cache is disabled. Applications that require Flash operations from decode callbacks can dispatch them to an internal-stack task or initialize espressif/esp_flash_dispatcher.

### `CONFIG_ESP_GSP_DECODE_TASK_STACK_SIZE`

Decode task stack size in bytes

- Project default: `4096`
- Type/range: `int` / `2048..32768`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Stack reserved for the background decoder. Lower only after measuring the high-water mark with every enabled image format and callback path.

### `CONFIG_ESP_GSP_DECODE_TASK_PRIORITY`

Standalone decode task default priority

- Project default: `3`
- Type/range: `int` / `1..24`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Used by direct gsp_decode_task_start() callers. The high-level ESP-IDF runtime derives its worker priority from the render task.

### `CONFIG_ESP_GSP_DECODE_TASK_IDLE_POLL_MS`

Decode worker idle poll interval in milliseconds

- Project default: `100`
- Type/range: `int` / `1..10000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

How often an idle decode worker wakes when no job notification is pending. Lower values reduce fallback wake latency at higher idle cost.

## Project runtime defaults / Animation and transitions

### `CONFIG_ESP_GSP_ANIM_FRAME_MEMORY`

Animation frame memory preference

- Project default: `auto`
- Type/range: `enum` / `0..2`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no
- Choices: `CONFIG_ESP_GSP_ANIM_FRAME_MEMORY_AUTO`, `CONFIG_ESP_GSP_ANIM_FRAME_MEMORY_INTERNAL`, `CONFIG_ESP_GSP_ANIM_FRAME_MEMORY_SPIRAM`

Chooses the preferred heap for decoded runtime animation frames. AUTO uses ESP_GSP_ANIM_INTERNAL_FRAME_MAX_BYTES; every mode falls back to the other reachable heap when the preferred allocation fails.

### `CONFIG_ESP_GSP_ANIM_MAX_FRAME_BYTES`

Maximum bytes in one runtime animation frame

- Project default: `16777216`
- Type/range: `int` / `1024..67108864`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Safety and functional limit checked before allocating runtime-supplied animation frame buffers and decode scratch. Compiled animation assets remain bounded by their validated resource metadata and reachable heap.

### `CONFIG_ESP_GSP_ANIM_INTERNAL_FRAME_MAX_BYTES`

Maximum animation frame preferred in SRAM

- Project default: `65536`
- Type/range: `int` / `0..16777216`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

AUTO mode prefers SRAM at or below this size and PSRAM above it. Zero sends every non-empty animation surface to PSRAM first. Every mode falls back to the other heap when the preferred heap is full.

## Scheduling and interaction / Runtime cadence and command queue

### `CONFIG_ESP_GSP_QUEUE_DEPTH`

Platform command queue depth

- Project default: `24`
- Type/range: `int` / `4..128`
- Ownership: `runtime_capacity` / `heap`
- Prebuilt component: configurable
- JSON AUTO-derived: no

esp_gsp_cmd_t queue entries (xQueueCreate) and the per-frame release-callback stack arrays in the ESP-IDF runtime.

### `CONFIG_ESP_GSP_ACTIVE_TICK_MS`

Active render-loop tick in milliseconds

- Project default: `10`
- Type/range: `int` / `1..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Loop cadence while a finger, animation or scroll momentum keeps the UI active. Lower values improve temporal resolution at higher CPU cost.

### `CONFIG_ESP_GSP_IDLE_POLL_MS`

Default idle application poll in milliseconds

- Project default: `100`
- Type/range: `int` / `1..10000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default application-service interval while the UI has no active input, animation or momentum. Larger values reduce idle wakeups but delay polling-only application work.

### `CONFIG_ESP_GSP_POINTER_POLL_MS`

Idle touch discovery interval in milliseconds

- Project default: `33`
- Type/range: `int` / `1..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Polling touch integrations use this interval while the UI is otherwise idle. Once input or animation is active, ESP_GSP_ACTIVE_TICK_MS drives the shared UI service loop. Interrupt-driven touch wakes the loop directly and does not read without a fresh interrupt.

### `CONFIG_ESP_GSP_TOUCH_RELEASE_CONFIRM_POLLS`

Empty touch polls required to confirm release

- Project default: `2`
- Type/range: `int` / `1..8`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Higher values tolerate intermittent empty controller reports but add pointer-release latency in polling mode. Interrupt mode consumes only fresh IRQ reports and treats a fresh empty report as release.

## Scheduling and interaction / Scroll physics

### `CONFIG_ESP_GSP_SCROLL_VELOCITY_WINDOW_MS`

Scroll release velocity window in milliseconds

- Project default: `100`
- Type/range: `int` / `10..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Only pointer samples inside this recent time window contribute to the release velocity. A shorter window reacts faster; a longer one smooths noisy motion.

### `CONFIG_ESP_GSP_SCROLL_RELEASE_GRACE_MS`

Scroll release motion grace in milliseconds

- Project default: `100`
- Type/range: `int` / `0..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Motion older than this interval before release is ignored for starting a coast. Zero requires motion in the release sample itself.

### `CONFIG_ESP_GSP_SCROLL_STOP_SPEED_PX_S`

Scroll coast stop speed in pixels per second

- Project default: `20`
- Type/range: `int` / `1..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Existing inertial scrolling stops after its absolute velocity falls below this threshold.

### `CONFIG_ESP_GSP_SCROLL_MIN_FLING_SPEED_PX_S`

Minimum release speed that starts a coast, in pixels per second

- Project default: `160`
- Type/range: `int` / `0..5000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Releases slower than this stop where the finger left off instead of coasting, matching the fling threshold phones apply. Keep it well above the coast stop speed, which only ends an existing coast. The default corresponds to crossing the default 16-pixel tap slop over the 100 ms velocity window, so motion beyond the slop can coast without making a slow drag coast. Set to 0 to let any measurable release speed coast.

### `CONFIG_ESP_GSP_SCROLL_MAX_RELEASE_SPEED_PX_S`

Maximum scroll release speed in pixels per second

- Project default: `6000`
- Type/range: `int` / `100..50000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Clamps the initial coast velocity so noisy or sparse touch samples cannot launch an unbounded scroll.

### `CONFIG_ESP_GSP_SCROLL_DECAY_MS`

Scroll velocity decay time in milliseconds

- Project default: `325`
- Type/range: `int` / `10..5000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Time constant used to decay inertial scroll velocity. Larger values coast longer; smaller values stop sooner.

### `CONFIG_ESP_GSP_SCROLL_MAX_TICK_MS`

Maximum integrated scroll tick in milliseconds

- Project default: `100`
- Type/range: `int` / `1..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Caps one physics integration step after scheduler stalls or delayed frames, preventing a single update from jumping too far.

### `CONFIG_ESP_GSP_SCROLL_SNAP_RESPONSE_MS`

List row snap response time in milliseconds

- Project default: `40`
- Type/range: `int` / `1..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Response time used when a List or Wheel settles onto its nearest row. Lower values snap more aggressively.

## Scheduling and interaction / Swipe and component motion

### `CONFIG_ESP_GSP_SWIPE_VELOCITY_WINDOW_MS`

Page-swipe velocity window in milliseconds

- Project default: `80`
- Type/range: `int` / `10..1000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Recent pointer interval used to estimate page-swipe release velocity. Shorter windows favor the last motion; longer windows smooth input.

### `CONFIG_ESP_GSP_SWIPE_RELEASE_GRACE_MS`

Page-swipe release motion grace in milliseconds

- Project default: `350`
- Type/range: `int` / `0..2000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Maximum age of motion that can still influence a swipe release. A shorter value makes pauses before release cancel fling momentum.

### `CONFIG_ESP_GSP_SWIPE_SETTLE_MIN_MS`

Minimum page-swipe settle time in milliseconds

- Project default: `80`
- Type/range: `int` / `1..2000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Lower bound for the velocity- and distance-derived page settle time.

### `CONFIG_ESP_GSP_SWIPE_SETTLE_BASE_MS`

Base page-swipe settle time in milliseconds

- Project default: `110`
- Type/range: `int` / `1..2000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Fixed part of the page settle duration before distance and velocity adjustments are applied.

### `CONFIG_ESP_GSP_SWIPE_SETTLE_DISTANCE_MS`

Distance-dependent page-swipe settle time in milliseconds

- Project default: `90`
- Type/range: `int` / `0..2000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Maximum distance-proportional contribution to page settle duration.

### `CONFIG_ESP_GSP_SWIPE_SETTLE_MAX_MS`

Maximum page-swipe settle time in milliseconds

- Project default: `220`
- Type/range: `int` / `1..5000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Upper bound for the final page settle duration.

### `CONFIG_ESP_GSP_SWIPE_SETTLE_SPEED_REDUCTION_MS`

Swipe settle speed reduction scale in milliseconds

- Project default: `60`
- Type/range: `int` / `0..2000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Scale of the duration reduction applied as release speed increases.

### `CONFIG_ESP_GSP_SWIPE_SETTLE_MAX_SPEED_REDUCTION_MS`

Maximum swipe settle speed reduction in milliseconds

- Project default: `120`
- Type/range: `int` / `0..2000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Maximum amount that release velocity may subtract from settle time.

### `CONFIG_ESP_GSP_PAGE_FLOW_SETTLE_MS`

Legacy PageFlow settle duration in milliseconds

- Project default: `220`
- Type/range: `int` / `1..5000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Compatibility duration used by PageFlow paths that do not receive the adaptive swipe timing result.

### `CONFIG_ESP_GSP_STACK_VIEW_SETTLE_MS`

Default StackView settle duration in milliseconds

- Project default: `220`
- Type/range: `int` / `1..5000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default push/pop transition duration for StackView navigation.

### `CONFIG_ESP_GSP_DRAWER_SETTLE_MS`

Drawer settle duration in milliseconds

- Project default: `180`
- Type/range: `int` / `1..5000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Default duration used to finish opening or closing a Drawer after the pointer is released.

### `CONFIG_ESP_GSP_BOOLEAN_TWEEN_MS`

Toggle/checkbox knob tween duration in milliseconds

- Project default: `130`
- Type/range: `int` / `1..5000`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Retained visual tween duration for Toggle and Checkbox state changes.

## Scheduling and interaction / Gesture thresholds

### `CONFIG_ESP_GSP_TAP_SLOP_PX`

Tap classification slop in pixels

- Project default: `16`
- Type/range: `int` / `0..128`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Absolute-pixel tap window; panel noise and touch resolution differ per product. Scaled touch controllers may need a different value.

### `CONFIG_ESP_GSP_DRAG_START_PX`

Drag / gesture start threshold in pixels

- Project default: `24`
- Type/range: `int` / `0..256`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Distance a finger must move before a drag, scroll or swipe gesture starts.

### `CONFIG_ESP_GSP_SWIPE_FLICK_PX`

Flick commit distance in pixels

- Project default: `12`
- Type/range: `int` / `0..256`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Recent pointer movement (sum of the last ~3 samples) above this commits a page swipe regardless of drag distance.

### `CONFIG_ESP_GSP_SCENE_SWIPE_MIN_FLING_PX`

Scene swipe minimum fling distance in pixels

- Project default: `32`
- Type/range: `int` / `0..512`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Fallback velocity/fling gate for scene navigation.

### `CONFIG_ESP_GSP_COMPONENT_SWIPE_COMMIT_DIVISOR`

Component swipe distance divisor

- Project default: `4`
- Type/range: `int` / `2..16`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

A component PageFlow swipe commits after approximately one divided by this value of its viewport, unless the flick rule commits it earlier.

### `CONFIG_ESP_GSP_SCENE_SWIPE_COMMIT_DIVISOR`

Scene swipe distance divisor

- Project default: `5`
- Type/range: `int` / `2..16`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

A scene swipe commits after approximately one divided by this value of the viewport, unless the fling rule commits it earlier.

### `CONFIG_ESP_GSP_SWIPE_VELOCITY_NUMERATOR`

Swipe settle velocity numerator

- Project default: `3`
- Type/range: `int` / `1..16`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Numerator of the release-velocity contribution used by adaptive swipe settling. Adjust together with ESP_GSP_SWIPE_VELOCITY_DENOMINATOR.

### `CONFIG_ESP_GSP_SWIPE_VELOCITY_DENOMINATOR`

Swipe settle velocity denominator

- Project default: `5`
- Type/range: `int` / `1..32`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Denominator of the release-velocity contribution used by adaptive swipe settling. Larger values reduce the velocity effect.

### `CONFIG_ESP_GSP_DRAWER_EDGE_SLOP_PX`

Drawer edge activation slop in pixels

- Project default: `24`
- Type/range: `int` / `0..256`
- Ownership: `runtime_policy` / `scalar`
- Prebuilt component: configurable
- JSON AUTO-derived: no

Maximum distance from the configured screen edge at which a closed Drawer may begin an edge-drag gesture. Zero requires the exact edge.

## Library capabilities

### `ESP_GSP_BUILD_CAP_COMPONENT_BATCH_MAX`

Inline component-batch update capacity

- Project default: `64`
- Type/range: `int` / `1..64`
- Ownership: `build_capability` / `stack`
- Prebuilt component: fixed library capability 64
- JSON AUTO-derived: no

Read-only stack fast-path threshold. Larger batches use temporary framework-owned heap storage and remain atomic.

### `ESP_GSP_BUILD_CAP_TRANSACTION_UPDATE_CAPACITY`

Inline input/component transaction capacity

- Project default: `64`
- Type/range: `int` / `1..64`
- Ownership: `build_capability` / `stack`
- Prebuilt component: fixed library capability 64
- JSON AUTO-derived: no

Read-only stack fast-path threshold. Larger input and component transactions use temporary framework-owned heap storage.

### `ESP_GSP_BUILD_CAP_RENDER_CLIP_STACK_DEPTH`

Maximum nested renderer clip depth

- Project default: `128`
- Type/range: `int` / `4..128`
- Ownership: `build_capability` / `stack`
- Prebuilt component: fixed library capability 128
- JSON AUTO-derived: no

Read-only library limit for nested transformed clipping. Excess depth is rejected.

### `ESP_GSP_BUILD_CAP_RENDER_TILE_SPAN_CAPACITY`

Tile-index fast-path span capacity

- Project default: `256`
- Type/range: `int` / `1..256`
- Ownership: `build_capability` / `stack`
- Prebuilt component: fixed library capability 256
- JSON AUTO-derived: no

Read-only library fast-path limit. Overflow safely falls back to a linear command scan.

### `ESP_GSP_BUILD_CAP_TEXT_CAPACITY`

Inline dynamic-text bytes

- Project default: `63`
- Type/range: `int` / `1..1024`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 63
- JSON AUTO-derived: no

Read-only command fast-path threshold; longer text uses framework-owned heap storage.

### `ESP_GSP_BUILD_CAP_MAX_DYNAMIC_IMAGE_TARGETS`

Maximum simultaneous runtime image targets

- Project default: `32767`
- Type/range: `int` / `1..32767`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 32767
- JSON AUTO-derived: no

Read-only library limit for logical runtime image targets across loaded scenes.

### `ESP_GSP_BUILD_CAP_MAX_FONTS_PER_SCENE`

Maximum font packs referenced by one scene

- Project default: `32`
- Type/range: `int` / `1..32`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 32
- JSON AUTO-derived: no

Read-only library limit for font packs referenced by a single scene.

### `ESP_GSP_BUILD_CAP_ANIM_PATCH_RECTS`

Maximum dirty patches retained per compiled animation frame

- Project default: `32`
- Type/range: `int` / `1..32`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 32
- JSON AUTO-derived: no

Read-only library limit for compiled animation dirty patches per frame.

### `ESP_GSP_BUILD_CAP_ANIM_REFERENCE_COMMANDS`

Animation visibility reference command capacity

- Project default: `32`
- Type/range: `int` / `1..32`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 32
- JSON AUTO-derived: no

Read-only library tracking limit. Overflow disables only the visibility pause optimization.

### `ESP_GSP_BUILD_CAP_SCROLL_PHYS_SAMPLES`

Scroll velocity sample capacity

- Project default: `32`
- Type/range: `int` / `2..32`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 32
- JSON AUTO-derived: no

Read-only library history capacity used by the scroll velocity estimator.

### `ESP_GSP_BUILD_CAP_SWIPE_KINETICS_SAMPLES`

Page-swipe velocity sample capacity

- Project default: `32`
- Type/range: `int` / `2..32`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 32
- JSON AUTO-derived: no

Read-only library history capacity used by PageFlow, StackView, and Drawer kinetics.

### `ESP_GSP_BUILD_CAP_MAX_TOUCH_POINTS`

Maximum touch contacts consumed per sample

- Project default: `2`
- Type/range: `int` / `1..2`
- Ownership: `build_capability` / `inline`
- Prebuilt component: fixed library capability 2
- JSON AUTO-derived: no

Read-only library limit. Two contacts preserve pinch support.
