# ESP-GSP Hello World

This example is the minimum application path:

1. initialize an ESP-LCD target with `hw_init`;
2. create the generated bundle configuration with `gsp_bundle_config()`;
3. start GSP with `esp_gsp_esp_lcd_start()`;
4. update one named control through its generated typed API.

The scene contains static text and one progress control. Advanced media,
navigation, component and performance workloads live in `../benchmark`.

## Preview without hardware

Install the toolchain manager and run the example with its implicit cache and
download path:

```sh
python -m pip install -U esp-gsp-tools
python -m gsp.execute --version '<GSPC version>' gspc pack scenes/hello_320.json \
  --deployable -o hello.gspb
python -m gsp.execute --version '<ESP-GSP version>' sim hello.gspb
```

Use the selected component's `.gspc_version` for `<GSPC version>` and the
`version` field in `idf_component.yml` for `<ESP-GSP version>`. `sim` version
detection is currently manual, and must match the ESP-GSP component version.
For a release-only setup, download matching `gspc` and `gsp_sim` archives from
the [ESP-GSP Releases](https://github.com/espressif/esp-gsp/releases) page,
then set their absolute paths and invoke them directly:

```sh
export GSPC_EXECUTABLE=/absolute/path/to/gspc
export GSP_SIM_EXECUTABLE=/absolute/path/to/gsp_sim
"$GSPC_EXECUTABLE" pack scenes/hello_320.json --deployable -o hello.gspb
"$GSP_SIM_EXECUTABLE" hello.gspb
```

For CI or a remote terminal, add
`--headless --frames 3 --dump hello.ppm`. A source checkout also provides the
developer-only `preview_wasm.sh` helper; it is intentionally not part of the
Component Registry example.

Export the matching ESP-IDF environment, then build with one board profile:

```sh
idf.py -B build_esp32c3 \
  -D SDKCONFIG=build_esp32c3/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32c3 build
```

Native RGB888 uses the same minimal scene:

```sh
idf.py -B build_esp32p4_rgb888 \
  -D SDKCONFIG=build_esp32p4_rgb888/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  -D GSP_HELLO_RGB888=ON \
  set-target esp32p4 build
```

ESP-IDF automatically loads the matching `sdkconfig.defaults.<target>` after
the base file. See the [Kconfig guide](../../docs/en/reference/kconfig.md) before changing
framework capacities, features, or task memory placement.
