# ESP-GSP Examples

Choose by purpose. Start with **usage/hello_world** for integration, use
**scenarios** for complete product interactions, and use **performance/benchmark**
for reproducible measurements.

## Performance

[Hardware benchmark](performance/benchmark/README.md) retains the automated
workloads, board profiles, warm-up, measurement protocol and comparison tools.
Engine throughput is not an optical measurement of panel refresh rate.

## Usage

- [Hello world](usage/hello_world/README.md): bundle startup and generated C APIs.
- [Custom drawing](usage/custom_draw/README.md): native Canvas regions, tagged
  registration results, instance observers and overridable core notification hooks.
- [Reusable project components](usage/component_patterns/README.md): compile-time
  component definitions, Grid/Wrap layout and generated application bindings.
- [Widgets](usage/widgets/README.md): individual authored controls and components.
- [Effects](usage/effects/README.md): effects, image projections and minimal integration.
- [External assets](usage/external_assets/README.md): mounted filesystems, asynchronous
  image loading, missing-media fallback and resource lifetime.
- [PC media bridge](usage/sim_bridge_media/README.md): portable C data binding,
  runtime images and Canvas producers.

Widgets are scene examples, not separate ESP-IDF projects. The other guides state
whether they provide a device application, a PC backend, or both.

## Product scenarios

The [product scenarios](scenarios/README.md) provide these layouts and board profiles:

- [Coffee machine](scenarios/coffee_machine/README.md): 800x480, S3/S31 RGB565
  with touch, or S31 RGB888 without touch.
- [Smartwatch](scenarios/smartwatch/README.md): 360x360, S3 QSPI touch.
- [Industrial panel](scenarios/industrial_panel/README.md): 1024x600, P4 MIPI-DSI touch.
- [Thermostat](scenarios/thermostat/README.md): 240x240, C3 SPI with rotary input.
- [Washing machine](scenarios/washing_machine/README.md): 320x240, S3 SPI touch.

The scenarios use simulated appliance and sensor data. Each README describes
the controls, build commands and simulator preview.

## Example asset retrieval

The component includes compact vector artwork and one shared default font.
Scenario bitmaps, showcase previews, Ubuntu fonts and benchmark bitmaps are
restored from a pinned archive before GSPC runs. Each extracted example gets
its own media and required fonts; bundled default fonts are copied locally.
The archive is verified by SHA256 and cached under the build directory. Set
`ESP_GSP_EXAMPLE_ASSETS_OFFLINE=ON` to require a cached copy.
`ESP_GSP_EXAMPLE_ASSETS_FETCH=ON` enables the fetch target even when its sentinel
exists; use a fresh cache to replace incomplete local media.

To restore incomplete scenario assets, first back up any locally edited media:
extraction can overwrite files from the archive. In the scenario directory,
create a fresh cache with `asset_cache=$(mktemp -d)` and rerun the board build
with `-D ESP_GSP_EXAMPLE_ASSETS_FETCH=ON` and
`-D ESP_GSP_EXAMPLE_ASSETS_CACHE_DIR="$asset_cache"`. Keep the same target,
SDKCONFIG and profile arguments. The fresh cache has no success stamp.
For offline recovery, copy the verified `assets-<SHA256>.tar.gz` from the old
cache into the fresh cache before building with
`-D ESP_GSP_EXAMPLE_ASSETS_OFFLINE=ON`; do not copy the old stamp.

The source checkout retains its authored assets and shared fonts. The fetch
target restores missing archived media into their original paths. Extracted
packages restore their own subtree and fonts into the example directory.
Design originals and image-preparation tools remain source-only; they are
excluded from the component and compiled firmware.

The archive is build-time input only. It is never requested by the firmware at
runtime and no account password is used by CMake. Maintainers should publish a
new immutable archive and update the URL/SHA256 pair in
cmake/gsp_example_assets.cmake when the example asset set changes.



## Hardware and preview

`common/hw_init` provides board-specific pins, panel timing, rotation and touch
configuration. Select the profile matching the board. Release packaging copies
this support into each independently extractable device example.

For the native C bridge, from the repository root:

```sh
python3 tools/sim_bridge/run.py --project examples/usage/hello_world/pc \
  --gspc /path/to/gspc --host /path/to/gsp_sim_host
```

This requires Python, CMake and a native C compiler. See
[sim_bridge](../tools/sim_bridge/README.md) for supported APIs. The scenario
guides also provide native preview commands that execute the same UI controller
as the device, including navigation and application callbacks.
