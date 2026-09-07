# ESP-GSP Hello World

This example is the minimum application path:

1. initialize an ESP-LCD target with `hw_init`;
2. create the generated bundle configuration with `gsp_bundle_config()`;
3. start GSP with `esp_gsp_esp_lcd_start()`;
4. update one named control through its generated typed API.

The scene contains static text and one progress control. Advanced media,
navigation, component and performance workloads live in `../benchmark`.

## Run the existing C UI logic on a PC

The device build and `pc/` target share `main/hello_ui.c`. Only `app_main.c`
initializes ESP hardware; `pc/platform_pc.c` supplies the native lifecycle
adapter. After the example's ESP-GSP dependency has been installed, run from
the application root:

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py --project pc
```

Unlike a scene-only preview, this runs the same C timer and generated
control setter used on the device. Python 3.10+, CMake 3.20+ and a native
C11 compiler are required; the native build does not require an active IDF
environment. GSPC and simulator versions are selected automatically from the
installed component. Use `GSPC_EXECUTABLE` / `GSP_SIM_EXECUTABLE` for binary
overrides. See
[sim_bridge](../../tools/sim_bridge/README.md) for Windows paths, build-only
usage, protocol version requirements and C API limitations.

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
`version` field in `idf_component.yml` for `<ESP-GSP version>`. These standalone
commands select versions explicitly; the native bridge runner selects them
automatically.
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
`--headless --frames 3 --dump hello.ppm`.

## Build for hardware

Export the matching ESP-IDF environment, verify that the selected profile
matches the panel pins and timing, then build for one target:

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
