# ESP-GSP Experience Showcase

This example is the polished application demo for ESP-GSP. It is deliberately
different from `hello_world` (smallest integration) and `benchmark`
(machine-readable stress workload): the showcase presents the same features as
a coherent smart-space product UI.

The presentation has three user-controlled scenes:

- **Command** — retained vectors, images, gradients, clock, range controls,
  chart, spinner, typed property animation and a gesture-aware Drawer.
- **Gallery** — a scene-owned media wall whose six visible tiles are compiled
  as direct raw resources, avoiding first-use decode or runtime scaling.
- **Lab** — PageFlow, StackView, list-backed Wheel, controls, a static Canvas
  surface, TabView, Dropdown, Table, Keyboard and modal MsgBox.

The checked-in default never navigates or animates content by itself. Images
and Canvas content remain static until the user interacts; only the functional
clock advances once per second. This keeps the product demo focused on GSP's
compiled static UI without presenting a stopped clock as live data.

## Resolution strategy

The example provides two full-size layouts, not every `common` profile:

| Scene size | Target path | Pixel formats |
| --- | --- | --- |
| 1024x600 | ESP32-P4 MIPI-DSI | RGB565, RGB888-capable pipeline |
| 800x480 | ESP32-S31 RGB | RGB565 and native RGB888 |

The 240x240, 320x240 and 360x360 SPI/QSPI boards need a compact information
architecture rather than a mechanically scaled desktop layout. They are not
silently mapped to this example.

## Simulate

Install the toolchain manager, then compile all three 800x480 scenes and open
the bundle through its implicit cache and download path:

```sh
python -m pip install -U esp-gsp-tools
python -m gsp.execute --version '<GSPC version>' gspc pack \
  scenes/showcase_command_800.json \
  scenes/showcase_gallery_800.json \
  scenes/showcase_lab_800.json \
  --deployable -o showcase.gspb
python -m gsp.execute --version '<ESP-GSP version>' sim showcase.gspb
```

Use the value from the selected component's `.gspc_version` for
`<GSPC version>`, and use the `version` field in `idf_component.yml` for
`<ESP-GSP version>`. `sim` version detection is currently manual; its version
must match the ESP-GSP component version. Alternatively, download matching `gspc` and `gsp_sim`
archives from the [ESP-GSP Releases](https://github.com/espressif/esp-gsp/releases)
page and set `GSPC_EXECUTABLE` and `GSP_SIM_EXECUTABLE` to their absolute paths.

Swipe horizontally inside the display to move between scenes. A source
checkout also provides the developer-only `preview_wasm.sh` helper; it is
intentionally not part of the Component Registry example.

Render and interact with an individual scene without hardware:

```sh
"$GSP_SIM_EXECUTABLE" showcase.gspb \
  --frames 30 --headless --dump showcase-command.ppm
```

The host renderer validates authored layout, direct resources, controls,
component gestures and named fixed authored list items. The Showcase firmware
still owns its clock, dynamic data binding and application callbacks; use
hardware for those device/application paths.

## Build profiles

Run these commands from this directory after exporting ESP-IDF.

ESP32-P4 MIPI-DSI, 1024x600 RGB565:

```sh
idf.py -B build_esp32p4 \
  -D SDKCONFIG=build_esp32p4/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32p4 build
```

ESP32-S31 RGB, 800x480 RGB565:

```sh
idf.py --preview -B build_esp32s31_rgb565 \
  -D SDKCONFIG=build_esp32s31_rgb565/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32s31 build
```

ESP32-S31 24-bit RGB, 800x480 native RGB888:

```sh
idf.py --preview -B build_esp32s31_rgb888 \
  -D SDKCONFIG=build_esp32s31_rgb888/sdkconfig \
  -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.esp32s31_rgb888' \
  -D GSP_SHOWCASE_RGB888=ON \
  set-target esp32s31 build
```

ESP-IDF automatically applies `sdkconfig.defaults.<target>` after the base
file. The RGB888 command adds a product variant on top. See the
[Kconfig guide](../../docs/en/reference/kconfig.md) for the loading and precedence rules.
