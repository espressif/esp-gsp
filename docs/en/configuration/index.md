# Configuration model

Source builds and prebuilt libraries use the same configuration entries and precedence.

## Start with defaults

Ordinary integration does not require filling in pool sizes. Select the board and PSRAM configuration in ESP-IDF/BSP, author the scenes, and build. GSPC derives the declared scene, list, text, image-target and template-instance requirements.

`menuconfig → ESP-GSP` exposes only image caching, background decoding and transition snapshots at the entry level. Other settings live under **Advanced settings (optional)**. Saved `sdkconfig` values and instance overrides remain active.

Fields labelled `0 = automatic` use authored requirements; follow the field descriptions for other values. Advanced settings cover application concurrency, task tuning and product capacities. The compiler reports known capacity conflicts together with their configuration symbols. Prebuilt capability limits are available as `ESP_GSP_BUILD_CAP_*` constants.

Applications can supply images, text, timers and instances at runtime. Declare known peaks in scenes/templates and use instance overrides for additional application capacity.

Queue and synchronization waits use the final firmware's FreeRTOS tick rate in
both source and prebuilt builds. When updating a prebuilt component, use its
matching component files and rebuild the application.

## Read the build result

Build logs show derived scene needs, encoded resource sizes, animation working sets and the image-cache policy. An `automatic runtime budget` does not mean the image cache is disabled.

The report shows the profile's internal/PSRAM allocation preferences. Include task stacks, control pools, DMA scratch, display buffers and application allocations separately when sizing the system. Check the target's actual heap capacities. Allocation and fallback policies depend on the buffer category; see [media memory policies](../guide/media-and-data.md) and the display-buffer budget below.

The build checks known resource requirements. Runtime allocation failures report the requested memory and available heap. Use these diagnostics together with BSP buffer sizes to tune application budgets.

## Display-present buffer budget

ESP-GSP requires `esp_display_present ^1.1.1`. Its buffer policy is separate
from GSP scene capacities. With Present 1.1.0, automatic FB-repair and TE modes
can select two partition draw buffers when multiple FreeRTOS cores are active
and the memory policy permits. Eligible two-buffer paths can also create a copy worker with a
4 KiB stack, plus task/queue overhead. The worker is started lazily; allocation
failure falls back to synchronous copying. The extra buffer and a successfully
created worker remain allocated while the presenter endpoint is configured;
they are not per-frame scratch allocations.

For a product that needs a fixed single-draw-buffer budget, set the BSP target
before passing it to `esp_gsp_esp_lcd_start()`:

```c
/* After hw_lcd_init(&display), before assigning esp_config.display. */
display.drawbuf.buffers = 1;
```

This limits partition draw buffers, not panel framebuffers, TE full-frame
compose buffers or GSP caches. PSRAM-preferred allocation can still fall back
to internal RAM. Inspect the resolved mode and runtime heap on the actual board;
a dependency upgrade does not imply unchanged SRAM usage or guaranteed FPS gains.

## Three input layers, one result

| Layer | Use it for | Avoid using it for |
|---|---|---|
| JSON | Authored structure, templates and demand that GSPC can derive | Firmware scheduling policy |
| Kconfig | Firmware-wide runtime policy and heap-backed capacity defaults | Per-scene appearance |
| `esp_gsp_config_set()` | A bundle/instance that differs from the firmware default | Repeating every project default in C |

```text
project Kconfig → GSPB JSON requirement → per-instance override → library capability
```

The runtime raises AUTO capacity to cover bundle demand and rejects values above the prebuilt library capability instead of truncating content.

## Does Kconfig work with a prebuilt library?

Yes, for settings resolved by the consuming project: runtime policies, interaction tuning, task settings and heap-backed capacities. They are emitted through the project configuration bridge and remain effective with prebuilt libraries.

Parameters that affect the compiled library layout are fixed in the prebuilt archive and exposed as `ESP_GSP_BUILD_CAP_*` constants. The consuming project configures task policies and heap-backed capacities.

## Slots and dynamic instances

A slot is bounded runtime storage, not the total number of authored objects or dataset records. GSPC derives normal scene demand. For application-created instances, declare the reusable subtree as a JSON template and set `max_instances` to the maximum simultaneously live copies. List/Grid recycling therefore scales with visible rows or cells rather than total records.

To configure capacity for one bundle, call `esp_gsp_config_set()` before startup:

```c
esp_gsp_config_t config = gsp_product_config();
ESP_ERROR_CHECK(esp_gsp_config_set(
    &config, ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, 24) ==
    ESP_GSP_CONFIG_SET_OK ? ESP_OK : ESP_FAIL);
```

## Reproducible configuration

Use `idf.py menuconfig` to explore settings and commit intentional defaults in `sdkconfig.defaults` plus target-specific `sdkconfig.defaults.<target>` fragments. The active build uses `sdkconfig`; after changing defaults, validate a clean build so stale configuration is not mistaken for the new policy.

When a component needs an additional defaults fragment, pass it through the
calling project's `GSP_KCONFIG_DEFAULTS` CMake variable. Keep one defaults
chain rather than implementing equivalent fallback values in application code.

The exhaustive current settings, ranges, and prebuilt behavior are generated in
the [Configuration reference](../reference/configuration.md) and explained in the
[Kconfig guide](../reference/kconfig.md).
