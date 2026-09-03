# Configuration model

ESP-GSP keeps configuration useful in both source and prebuilt-library consumption. Each setting has one owner and one precedence chain.

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

Yes, for settings intentionally resolved by the consuming project: runtime policies, interaction tuning, task settings and heap-backed capacities. They are emitted through the project configuration bridge and remain effective with sealed libraries.

Values that change compiled library layout cannot be changed after the library is built. Those do not masquerade as usable Kconfig settings in a source-free release; the archive publishes them as `ESP_GSP_BUILD_CAP_*` capability constants. This removes ineffective options while keeping a clear failure boundary.

## Slots and dynamic instances

A slot is bounded runtime storage, not the total number of authored objects or dataset records. GSPC derives normal scene demand. For application-created instances, declare the reusable subtree as a JSON template and set `max_instances` to the maximum simultaneously live copies. List/Grid recycling therefore scales with visible rows or cells rather than total records.

Use `esp_gsp_config_set()` only when the extra peak truly cannot be authored:

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
