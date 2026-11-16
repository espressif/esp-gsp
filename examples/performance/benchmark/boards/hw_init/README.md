# Additional benchmark board adapters

The benchmark's empty `GSP_BENCH_BOARD_PROFILE` keeps using `examples/common`.
These opt-in profiles replace its `hw_init` component only. They do not start
LVGL, camera, audio, storage, physical input, or the original product services.

| CMake profile | Target | Additional defaults | Scene |
| --- | --- | --- | --- |
| `C5_CO5300` | `esp32c5` | `sdkconfig.defaults.c5_co5300` | `bench_square_466.json` |
| `P4_EYE_SPI` | `esp32p4` | `sdkconfig.defaults.p4_eye_spi` | existing `bench_240.json` |
| `S31_GC9503` | `esp32s31` | `sdkconfig.defaults.s31_gc9503` | `bench_square_480.json` |
| `S31_MOSAICO` | `esp32s31` | `sdkconfig.defaults.s31_mosaico` | `bench_square_480.json` |

Pass base and board defaults together, for example
`-D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.c5_co5300'`
and `-D GSP_BENCH_BOARD_PROFILE=C5_CO5300`. Use a fresh build/config directory.
S31 requires `idf.py --preview`. Target defaults select the matching
PSRAM/XIP configuration; board fragments supply the fixture settings.
All additional fixtures use rotation 0 and RGB565.

`S31_MOSAICO` follows the Mosaico BSP's CO5300 wiring: QSPI40MHz, TE GPIO43,
hardware-version-based clock/reset pins, and peripheral power soft-start.
The shared CO5300 profile (`sdkconfig.defaults.s31_co5300`, empty board profile)
uses different wiring, including TE GPIO38; select it only for matching hardware.

## Configuration and licenses

- C5 LCD code uses CC0-1.0. The profile selects CO5300 2.2.0,
  QSPI 40 MHz, a 6-pixel horizontal gap, TE off, and indirect PSRAM DMA.
  The module has 4 MiB PSRAM; the measured matrix disables PSRAM XIP to
  leave room for transition snapshots.
- P4-EYE LCD code, vendor commands, and backlight PWM are adapted from
  `esp-dev-kits/examples/esp32-p4-eye/examples/common_components/esp32_p4_eye`
  (Apache-2.0). The source BSP requests SPI80MHz/mode3 and inverted PWM on
  GPIO20. Validate the actual SPI clock and display with the selected IDF.
- S31 RGB480 initialization uses Apache-2.0 code and selects 16 MHz,
  no bounce buffer, two framebuffers, a 16-bit RGB565 bus, and 18-bit
  panel color depth. The GC9503 3.0.1 driver and its license are included
  unchanged. The local
  manifest admits S31 because the corresponding Registry metadata only
  admits S3. Panel-IO additions uses the `^1.0.1` requirement.
- Mosaico LCD and power code are adapted from `esp-mosaico-bsp` (Apache-2.0).
  The board adapter selects the supported eFuse hardware mapping; the benchmark
  initializes the power rail before starting the LCD.

The parent benchmark validates the 58-case workload and supplies CPU
instrumentation. These adapters do not establish hardware acceptance on their
own. Preserve the original firmware and media before flashing. Restoring Flash
does not restore external power-control signals or verify the original application.
