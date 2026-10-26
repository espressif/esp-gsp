# Configuration model

Source builds and prebuilt libraries share the same configuration entries and precedence.

## Start with defaults

Ordinary integration does not require filling in pool sizes. Just:

1. Select your board and PSRAM configuration in ESP-IDF / BSP.
2. Author your scenes.
3. Build.

GSPC derives the declared scene, list, text, image-target and template-instance requirements automatically.

`menuconfig → ESP-GSP` exposes only three feature options at the top level: image caching, background decoding and transition snapshots. Other settings live under **Advanced settings (optional)**. Any saved `sdkconfig` values and instance overrides remain active.

**Authored-capacity fields labelled "0 = automatic"** use bundle requirements.
Other automatic settings, such as cache budgets and task defaults, follow their
own field descriptions. Advanced settings cover application concurrency, task tuning and product capacities. The compiler reports known capacity conflicts together with their configuration symbols. Prebuilt capability limits are available as `ESP_GSP_BUILD_CAP_*` constants.

Applications can supply images, text, timers and instances at runtime. Declare known peaks in scenes and templates; use instance overrides for any additional application capacity.

Queue and synchronization waits use the final firmware's FreeRTOS tick rate in both source and prebuilt builds. When updating a prebuilt component, use its matching component files and rebuild the application.

## Reading the build output

Build logs show, in order:

1. Derived scene needs.
2. Encoded resource sizes.
3. Animation working sets.
4. The image-cache policy.

When the cache budget reads "automatic runtime budget," it means the budget is determined at runtime — the cache is **not** disabled.

The report also shows the profile's internal / PSRAM allocation preferences. When sizing the whole system, include task stacks, control pools, DMA scratch, display buffers and application allocations separately. Check the target's actual heap capacities. Allocation and fallback policies vary by buffer category; see [media memory policies](../guide/media-and-data.md) and the display-buffer budget below.

Build-time checks cover known resource requirements. Runtime allocation failures report the requested memory and available heap. Use these diagnostics together with BSP buffer sizes to tune application budgets.

## Specialize the runtime for an application

Keep the Bundle definition in the application's `main/CMakeLists.txt` and declare
optional capabilities used outside compiled resources:

```cmake
gsp_add_bundle(${COMPONENT_LIB} SCENES "../scenes/home.json")
gsp_specialize_runtime(DYNAMIC_FEATURES NONE)
```

`NONE` declares no additional use of the capabilities below. Text, values,
themes, lists, template instances, timers and compiled animations remain dynamic.
The build considers every registered bundle and asset package, including hidden
content and templates, without changing resource encoding or rendering quality.

Declare additional inputs such as downloaded images or standalone EAF playback:

```cmake
gsp_specialize_runtime(DYNAMIC_FEATURES jpeg png eaf)
```

| Dynamic feature | Additional use |
|---|---|
| `jpeg` | Runtime JPEG/JPEG_A8 images; compiled JPEG requirements are derived automatically |
| `png` | Runtime PNG images |
| `eaf` | Standalone files passed to `esp_gsp_play()`; compiled animations derive their own dependencies |
| `reference` | Additional scenes using formats other than RGB565/RGB888, such as ARGB8888 overlays |
| `vector` | Vector resources outside the compiled input set |
| `procedural` | Additional scenes containing procedural commands |
| `anim_deflate` | Additional packages containing grouped DEFLATE animations, including GRB 1.7 and 1.8 |
| `anim_spans` | Additional GRB 1.8 packages containing compiled span animations; also declare `anim_deflate` when their frames use grouped DEFLATE |

Call once per project, after `idf_component_register()`. Omitting `DYNAMIC_FEATURES`
selects `AUTO`, conservatively retaining all manually enabled features. `ALL`
retains all enabled capabilities. Projects that do not call the helper keep their existing
linking behavior. Manual Kconfig disables remain capability ceilings; conflicting
resources or explicit dynamic requirements fail with the relevant configuration name.

All `gsp_add_bundle()` and `gsp_add_assets()` outputs participate, including outputs
registered later by other components. Add prebuilt packages with
`EXTRA_BUNDLES "path/to/media.gspb"`. For future UI packages with unknown feature
requirements, select `DYNAMIC_FEATURES ALL` or keep the default full runtime.

The build prints retained and removed features and writes reasons and input identities
to `build/gsp_runtime/runtime.json`. Ordinary incremental builds refresh the plan after
scene or resource changes. Source and precompiled components share this entry point.
GSPC and the archive must support the matching runtime-link ABI; incompatible tools
or libraries produce a configuration-time update message.

Specialized firmware checks bundle capabilities before allocating a UI instance.
Open a replacement UI package successfully before replacing the active UI. Runtime
submissions of a removed image format continue to report `GSP_ERR_UNSUPPORTED`
through the existing completion callback. Flash savings depend on actual requirements
and do not imply proportional reductions in caches, display buffers or task stacks.

## Display-present buffer budget

ESP-GSP requires `esp_display_present ^1.1.2`. Its buffer policy is separate from GSP scene capacities. Present 1.1.0 introduced the automatic FB-repair and TE policy described here; 1.1.2 is the minimum for the current frame-synchronization and renderer-buffer fixes.

When multiple FreeRTOS cores are active and the memory policy permits, these modes can select two partition draw buffers. Eligible two-buffer paths can also create a copy worker (4 KiB stack plus task/queue overhead). The worker is started lazily; allocation failure falls back to synchronous copying. The extra buffer and a successfully created worker remain allocated while the presenter endpoint is configured — they are not per-frame scratch allocations.

To fix a single-draw-buffer budget, set the BSP target before passing it to `esp_gsp_esp_lcd_start()`:

```c
/* After hw_lcd_init(&display), before assigning esp_config.display. */
display.drawbuf.buffers = 1;
```

This limits partition draw buffers only — not panel framebuffers, TE full-frame compose buffers, or GSP caches. PSRAM-preferred allocation can still fall back to internal RAM. Inspect the resolved mode and runtime heap on the actual board; a dependency upgrade does not imply unchanged SRAM usage or guaranteed FPS gains.

## Three input layers, one result

| Layer | Use it for | Avoid using it for |
|---|---|---|
| JSON | Authored structure, templates and demand that GSPC can derive | Firmware scheduling policy |
| Kconfig | Firmware-wide runtime policy and heap-backed capacity defaults | Per-scene appearance |
| `esp_gsp_config_set()` | A bundle/instance that differs from the firmware default | Repeating every project default in C |

These three layers form a single priority chain, not competing systems:

```text
project Kconfig → GSPB JSON requirement → per-instance override → library capability
```

The runtime raises AUTO capacity to cover bundle demand. Values above the prebuilt library capability are rejected explicitly, never truncated silently.

## Does Kconfig work with a prebuilt library?

**Yes.** The retained settings are those resolved by the consuming project: runtime policies, interaction tuning, task settings and heap-backed capacities. They are emitted through the project configuration bridge and remain effective with prebuilt libraries.

Parameters that affect the compiled library layout are fixed in the prebuilt archive and exposed as `ESP_GSP_BUILD_CAP_*` constants. The consuming project configures task policies and heap-backed capacities.

## Slots and dynamic instances

A slot is bounded runtime storage. It is **not** the total number of authored objects or dataset records. GSPC derives normal scene demand automatically.

For application-created instances, declare the reusable subtree as a JSON template and set `max_instances` to the maximum simultaneously live copies. This way, List/Grid recycling scales with visible rows or cells rather than total records.

To configure capacity for one bundle, call `esp_gsp_config_set()` before startup:

```c
esp_gsp_config_t config = gsp_product_config();
ESP_ERROR_CHECK(esp_gsp_config_set(
    &config, ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, 24) ==
    ESP_GSP_CONFIG_SET_OK ? ESP_OK : ESP_FAIL);
```

## Reproducible configuration

- Use `idf.py menuconfig` to explore settings and commit intentional defaults in `sdkconfig.defaults` plus target-specific `sdkconfig.defaults.<target>` fragments.
- After changing defaults, validate a clean build so stale configuration is not mistaken for the new policy.
- When a component needs an additional defaults fragment, pass it through the calling project's `GSP_KCONFIG_DEFAULTS` CMake variable.
- Run `idf.py reconfigure` to inspect the active `sdkconfig` and confirm settings are in effect.
- Bundle instance counts can use AUTO, derived from JSON `max_instances`. Application-determined capacities and product policies are set through `esp_gsp_config_set()`.

The exhaustive current settings, ranges, and prebuilt behavior are generated in the [Configuration reference](../reference/configuration.md) and explained in the [Kconfig guide](../reference/kconfig.md).
