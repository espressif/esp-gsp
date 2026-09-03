# ESP-GSP Hardware Benchmark

This example exercises the complete renderer and interaction surface on real
display targets. It covers controls, components, text, image fit and runtime
scaling, animations, Canvas, fixed and variable-height scrolling, elastic
message bubbles, drawers, navigation, and transitions under continuous updates.

The first lap warms caches. The second lap prints the measured summary. Allow
at least 361 seconds after reset for a complete run. Use a 390-second matrix
capture to retain margin for slower targets.

## Metrics

- `wall throughput`: frames divided by total elapsed time.
- `active throughput`: frames divided by renderer/presenter busy time.
- `rndr ms`: CPU rendering time.
- `subm ms`: cache synchronization and panel submission time.
- `svc us`: average UI service time per render-task iteration.
- `cmd/s`: applied update rate.

The log also records exact frame and microsecond counters, transition paths,
Canvas publication, media decode backends, and a target configuration
fingerprint. A valid run must complete the measured lap without panic,
watchdog, assertion, or display-underrun logs.

Cross-fade and fade-through-black are visually successful only when both
`no_visual` and `path_failures` are zero. When snapshot memory is unavailable,
cross-fade safely degrades to a direct, zero-snapshot fade-through-black;
`direct` records that route. A direct switch with `no_visual=1` completes
navigation safely but is not a rendered transition result.

## Build

Export the matching ESP-IDF environment and build with the appropriate
configuration fragment. Separate build directories prevent retained target
settings from affecting another board.

```sh
# ESP32-P4, MIPI-DSI, RGB565
idf.py -B build_esp32p4 \
  -D SDKCONFIG=build_esp32p4/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32p4 build

# ESP32-P4, MIPI-DSI, RGB888
idf.py -B build_esp32p4_rgb888 \
  -D SDKCONFIG=build_esp32p4_rgb888/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  -D GSP_BENCH_RGB888=ON \
  set-target esp32p4 build

# ESP32-C3, SPI
idf.py -B build_esp32c3 \
  -D SDKCONFIG=build_esp32c3/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32c3 build

# ESP32-S3, QSPI
idf.py -B build_esp32s3 \
  -D SDKCONFIG=build_esp32s3/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32s3 build

# ESP32-S3, SPI
idf.py -B build_esp32s3_spi \
  -D SDKCONFIG=build_esp32s3_spi/sdkconfig \
  -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.esp32s3_spi' \
  set-target esp32s3 build

# ESP32-S31, RGB565
idf.py --preview -B build_esp32s31 \
  -D SDKCONFIG=build_esp32s31/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32s31 build

# ESP32-S31, RGB888
idf.py --preview -B build_esp32s31_rgb888 \
  -D SDKCONFIG=build_esp32s31_rgb888/sdkconfig \
  -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.esp32s31_rgb888' \
  -D GSP_BENCH_RGB888=ON \
  set-target esp32s31 build
```

ESP-IDF automatically applies `sdkconfig.defaults.<target>` after the base
file. The SPI and RGB888 commands add another profile fragment on top. See the
[Kconfig guide](../../docs/en/reference/kconfig.md) for the loading and precedence rules.

The checked-in profiles are examples for specific boards, not a complete list
of supported panel combinations. Verify panel timing, buffering, rotation,
byte order, and anti-tearing behavior on the product hardware.

Add `-D GSP_BENCH_DEPLOYABLE=ON` to any build above to embed GMD metadata and
start the same benchmark through `esp_gsp_deployable_bundle_open()`. The
default remains the generated component-directory path. The startup log prints
`bench: bundle directory=deployable` or `generated` so captures identify the
path under test.

## Compare logs

Use the same target, panel configuration, scene bundle, and ESP-IDF revision
for both runs:

```sh
python3 tools/compare_logs.py \
  --pair P4-MIPI baseline-p4.log current-p4.log \
  --pair S3-QSPI baseline-s3.log current-s3.log \
  --details
```

The parser validates the measured summary and rejects incompatible or fatal
logs before reporting differences.

## Assets

`scenes/gen_scenes.py --check` verifies the benchmark scenes and media assets.

## Automated simulator run

The simulator benchmark is a fast host-side gate for the shared frame-plan,
renderer, transition and canvas-lease path. It is not a replacement for the
panel throughput figures above: it has no panel transfer, cache-coherency or
TE cost.

Install `esp-gsp-tools` and create `.gspc_version` in the project root when the
source checkout has no component marker. The benchmark's scene runner then
uses `python -m gsp.execute` and its implicit cache/download path. Alternatively
set `GSPC_EXECUTABLE` to a manually downloaded GSPC release.

```sh
# Default 320x240, automatically cycle through all benchmark pages
python3 tools/run_sim_benchmark.py

# All RGB565 resolutions
python3 tools/run_sim_benchmark.py --all --frames 300
```

Add `--window` to watch the pages change in SDL while retaining automatic
frame-count and output validation. Each page is shown for 60 frames by
default; use `--page-frames 120` to keep each page on screen longer.

Each case runs headless, requires one committed frame per requested loop, and
saves its log and final PPM under `build/gsp-sim-benchmark/<width>/`.

## Capture a board matrix

`tools/capture_matrix.py` resets multiple boards together and captures their
serial output concurrently. Install `pyserial` in the active Python
environment, then provide one `--device LABEL PORT LOG` option per board:

```sh
python3 tools/capture_matrix.py \
  --device P4-MIPI /dev/ttyACM0 p4-mipi.log \
  --device S3-QSPI /dev/ttyACM1 s3-qspi.log
```
