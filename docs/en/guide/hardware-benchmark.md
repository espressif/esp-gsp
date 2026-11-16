# Hardware performance statistics

The complete 58-case ESP-GSP 1.10.3 benchmark covers **five chip families, four display interfaces and 11 hardware configurations**, from the single-core 160 MHz ESP32-C3 to the dual-core ESP32-P4. Normal updates and full-repaint pressure report FPS, CPU and memory separately. See [GSP / LVGL performance statistics](performance.md) for the LVGL comparison.

Data: [Per-case results CSV](../../assets/performance/gsp-1.10.3-benchmark/cases.csv) · [Resource costs CSV](../../assets/performance/gsp-1.10.3-benchmark/resources.csv)

<details>
<summary>What the CSV files contain</summary>

<div class="gsp-statistics">

| File | Contents |
| --- | --- |
| cases.csv | 11 configurations × 2 modes × 58 cases: frames, time, FPS, CPU, two-run ranges and sampled memory increases. |
| resources.csv | 11 configurations × 2 modes: resolution, frequency, presentation, Flash and RAM. Memory fields use bytes. |

</div>

</details>

## Results at a glance

**How to read the tables:** cells show FPS / CPU. CPU is averaged across all cores, so about 50% on a dual-core chip means one core is fully busy. Resolutions and pixel formats differ, so results support hardware planning rather than ranking chips.

- **Animation:** four-view GIF pages run at 19.92–20.24 FPS on all 11 configurations, keeping pace with the 20 FPS source. Dual-core chips use no more than 9.91% CPU; single-core chips peak at 26.69% (ESP32-C3 SPI · 240×240).
- **Moving content:** every configuration reaches at least 30.03 FPS, up to 91.23 FPS on ESP32-P4 SPI · 240×240.
- **Full-screen drag:** 15.87–39.35 FPS, including pauses in input replay.
- **Full-repaint pressure:** the combined averages across 58 cases in this mode range from 17.51 to 70.57 FPS.

## Normal updates

Each scene retains its own update cadence.

### Text, images and moving content

<div class="gsp-statistics">

| Configuration | Text updates | Static move | Grid scroll |
| --- | ---: | ---: | ---: |
| ESP32-C3 SPI · 240×240 | 47.29 / 83.26% | 30.03 / 92.61% | 34.76 / 68.46% |
| ESP32-C5 QSPI · 466×466 | 35.71 / 67.82% | 58.14 / 91.20% | 32.38 / 75.28% |
| ESP32-S3 SPI · 320×240 | 28.57 / 10.21% | 40.32 / 19.09% | 15.55 / 7.81% |
| ESP32-S3 QSPI · 360×360 | 37.13 / 35.38% | 41.68 / 33.33% | 34.18 / 25.35% |
| ESP32-S3 RGB · 800×480 | 35.23 / 49.40% | 35.14 / 27.50% | 21.00 / 35.17% |
| ESP32-S31 RGB · 480×480 | 60.33 / 18.12% | 60.33 / 13.37% | 45.50 / 12.11% |
| ESP32-S31 RGB · 480×800 | 60.99 / 30.79% | 60.93 / 15.71% | 12.37 / 10.81% |
| ESP32-S31 RGB888 · 800×480 | 46.96 / 18.73% | 46.96 / 13.36% | 26.37 / 16.33% |
| ESP32-P4 SPI · 240×240 | 76.91 / 9.57% | 91.23 / 15.80% | 39.43 / 3.66% |
| ESP32-P4 DSI 360 MHz · 1024×600 | 60.48 / 24.02% | 60.53 / 13.06% | 37.34 / 22.93% |
| ESP32-P4 DSI 400 MHz · 1024×600 | 60.49 / 20.45% | 60.48 / 11.08% | 40.21 / 19.43% |

</div>

### Animation updates

Both GIF sources are 120×120 at 20 FPS. The GIF page has four synchronized views and the sparse GIF page has one. Approaching 20 FPS means playback keeps pace with the source.

<div class="gsp-statistics">

| Configuration | GIF page | Sparse GIF page |
| --- | ---: | ---: |
| ESP32-C3 SPI · 240×240 | 20.14 / 26.69% | 20.16 / 14.51% |
| ESP32-C5 QSPI · 466×466 | 20.13 / 16.48% | 20.16 / 6.86% |
| ESP32-S3 SPI · 320×240 | 20.15 / 3.71% | 20.16 / 2.21% |
| ESP32-S3 QSPI · 360×360 | 19.98 / 9.41% | 20.06 / 6.41% |
| ESP32-S3 RGB · 800×480 | 20.07 / 9.91% | 20.15 / 4.06% |
| ESP32-S31 RGB · 480×480 | 20.22 / 2.53% | 20.24 / 1.38% |
| ESP32-S31 RGB · 480×800 | 20.24 / 3.22% | 20.18 / 1.47% |
| ESP32-S31 RGB888 · 800×480 | 20.14 / 3.29% | 20.15 / 1.40% |
| ESP32-P4 SPI · 240×240 | 19.92 / 1.43% | 19.92 / 0.83% |
| ESP32-P4 DSI 360 MHz · 1024×600 | 20.15 / 2.76% | 20.14 / 1.05% |
| ESP32-P4 DSI 400 MHz · 1024×600 | 20.13 / 2.32% | 20.22 / 0.88% |

</div>

See [single-GIF statistics](performance.md#animation-updates) for the playback comparison with LVGL.

### Dragging and dropdown interaction

<div class="gsp-statistics">

| Configuration | Page drag | Drawer | Dropdown |
| --- | ---: | ---: | ---: |
| ESP32-C3 SPI · 240×240 | 25.12 / 80.83% | 23.27 / 57.90% | 3.98 / 33.81% |
| ESP32-C5 QSPI · 466×466 | 22.09 / 86.12% | 15.20 / 69.65% | 3.98 / 11.45% |
| ESP32-S3 SPI · 320×240 | 25.21 / 9.18% | 18.20 / 9.62% | 3.99 / 6.06% |
| ESP32-S3 QSPI · 360×360 | 30.30 / 30.73% | 21.28 / 24.88% | 3.98 / 7.44% |
| ESP32-S3 RGB · 800×480 | 15.87 / 45.91% | 12.21 / 38.94% | 3.99 / 10.41% |
| ESP32-S31 RGB · 480×480 | 39.35 / 18.62% | 28.69 / 16.03% | 3.99 / 4.40% |
| ESP32-S31 RGB · 480×800 | 32.72 / 24.81% | 28.44 / 26.40% | 3.98 / 4.50% |
| ESP32-S31 RGB888 · 800×480 | 35.85 / 37.65% | 26.39 / 29.56% | 3.98 / 4.47% |
| ESP32-P4 SPI · 240×240 | 36.35 / 4.34% | 27.70 / 4.77% | 3.99 / 3.00% |
| ESP32-P4 DSI 360 MHz · 1024×600 | 32.94 / 40.32% | 27.14 / 34.26% | 3.99 / 4.00% |
| ESP32-P4 DSI 400 MHz · 1024×600 | 35.19 / 35.22% | 28.07 / 29.04% | 3.99 / 3.30% |

</div>

Interaction FPS includes pauses in input replay: about 4 FPS for dropdown selection is the submission rate across the whole replay, not the control's refresh limit.

## Full-repaint pressure

The 48 ordinary pages request a full-screen background update every 1 ms, while the ten transition and drag cases keep their normal cadence. The table divides total frames across all 58 cases by total sampled time and is read separately from normal updates.

<div class="gsp-statistics">

| Configuration | 58-case average FPS | CPU % |
| --- | ---: | ---: |
| ESP32-C3 SPI · 240×240 | 31.70 | 86.95 |
| ESP32-C5 QSPI · 466×466 | 23.70 | 82.52 |
| ESP32-S3 SPI · 320×240 | 27.04 | 16.16 |
| ESP32-S3 QSPI · 360×360 | 29.76 | 36.97 |
| ESP32-S3 RGB · 800×480 | 17.51 | 49.00 |
| ESP32-S31 RGB · 480×480 | 53.28 | 26.71 |
| ESP32-S31 RGB · 480×800 | 46.10 | 35.12 |
| ESP32-S31 RGB888 · 800×480 | 42.20 | 36.39 |
| ESP32-P4 SPI · 240×240 | 70.57 | 13.27 |
| ESP32-P4 DSI 360 MHz · 1024×600 | 50.04 | 45.14 |
| ESP32-P4 DSI 400 MHz · 1024×600 | 51.48 | 40.39 |

</div>

## Flash and memory

The table describes the normal-update firmware. Flash covers the complete 58-case test application and embedded assets, rather than the GSP library alone. Startup increases include display framebuffers and UI initialization.

<div class="gsp-statistics">

| Configuration | App in Flash MiB | Static internal RAM KiB | Startup SRAM increase KiB | Startup PSRAM increase MiB |
| --- | ---: | ---: | ---: | ---: |
| ESP32-C3 SPI · 240×240 | 2.06 | 107.02 | 175.79 | 0.00 |
| ESP32-C5 QSPI · 466×466 | 2.37 | 112.12 | 167.55 | 0.27 |
| ESP32-S3 SPI · 320×240 | 2.11 | 124.57 | 167.76 | 0.21 |
| ESP32-S3 QSPI · 360×360 | 2.19 | 125.99 | 176.16 | 0.76 |
| ESP32-S3 RGB · 800×480 | 2.49 | 115.70 | 201.23 | 2.58 |
| ESP32-S31 RGB · 480×480 | 1.99 | 131.68 | 208.08 | 2.29 |
| ESP32-S31 RGB · 480×800 | 2.32 | 131.13 | 223.20 | 3.66 |
| ESP32-S31 RGB888 · 800×480 | 2.21 | 131.13 | 222.23 | 5.23 |
| ESP32-P4 SPI · 240×240 | 1.66 | 145.83 | 172.33 | 1.25 |
| ESP32-P4 DSI 360 MHz · 1024×600 | 2.37 | 135.00 | 214.27 | 5.18 |
| ESP32-P4 DSI 400 MHz · 1024×600 | 2.37 | 134.14 | 213.75 | 5.18 |

</div>

<details>
<summary>Flash and memory for full-repaint firmware</summary>

<div class="gsp-statistics">

| Configuration | App in Flash MiB | Static internal RAM KiB | Startup SRAM increase KiB | Startup PSRAM increase MiB |
| --- | ---: | ---: | ---: | ---: |
| ESP32-C3 SPI · 240×240 | 2.06 | 107.02 | 175.79 | 0.00 |
| ESP32-C5 QSPI · 466×466 | 2.37 | 112.12 | 167.55 | 0.27 |
| ESP32-S3 SPI · 320×240 | 2.11 | 124.57 | 167.76 | 0.21 |
| ESP32-S3 QSPI · 360×360 | 2.19 | 125.99 | 167.29–176.16 | 0.76 |
| ESP32-S3 RGB · 800×480 | 2.49 | 115.70 | 201.23 | 2.58 |
| ESP32-S31 RGB · 480×480 | 1.99 | 131.68 | 208.08 | 2.29 |
| ESP32-S31 RGB · 480×800 | 2.32 | 131.13 | 223.20 | 3.66 |
| ESP32-S31 RGB888 · 800×480 | 2.21 | 131.13 | 222.23 | 5.23 |
| ESP32-P4 SPI · 240×240 | 1.66 | 145.83 | 172.33 | 1.25 |
| ESP32-P4 DSI 360 MHz · 1024×600 | 2.37 | 135.00 | 214.27 | 5.18 |
| ESP32-P4 DSI 400 MHz · 1024×600 | 2.37 | 134.14 | 213.75 | 5.18 |

</div>

</details>

<details>
<summary>Free memory after initialization</summary>

<div class="gsp-statistics">

| Configuration / mode | Free SRAM KiB | Largest SRAM block KiB | Free PSRAM MiB |
| --- | ---: | ---: | ---: |
| ESP32-C3 SPI · 240×240 / normal | 69.09 | 48.00 | 0.00 |
| ESP32-C3 SPI · 240×240 / full repaint | 69.09 | 48.00 | 0.00 |
| ESP32-C5 QSPI · 466×466 / normal | 84.07 | 37.00 | 3.73 |
| ESP32-C5 QSPI · 466×466 / full repaint | 84.07 | 37.00 | 3.73 |
| ESP32-S3 SPI · 320×240 / normal | 89.17 | 50.00 | 13.77 |
| ESP32-S3 SPI · 320×240 / full repaint | 89.17 | 50.00 | 13.77 |
| ESP32-S3 QSPI · 360×360 / normal | 79.34 | 40.00 | 13.14 |
| ESP32-S3 QSPI · 360×360 / full repaint | 79.34–88.21 | 40.00–49.00 | 13.14 |
| ESP32-S3 RGB · 800×480 / normal | 64.66 | 31.00 | 11.01 |
| ESP32-S3 RGB · 800×480 / full repaint | 64.66 | 31.00 | 11.01 |
| ESP32-S31 RGB · 480×480 / normal | 162.33 | 98.00 | 11.82 |
| ESP32-S31 RGB · 480×480 / full repaint | 162.33 | 98.00 | 11.82 |
| ESP32-S31 RGB · 480×800 / normal | 147.77 | 84.00 | 26.13 |
| ESP32-S31 RGB · 480×800 / full repaint | 147.77 | 84.00 | 26.13 |
| ESP32-S31 RGB888 · 800×480 / normal | 148.74 | 84.00 | 24.67 |
| ESP32-S31 RGB888 · 800×480 / full repaint | 148.74 | 84.00 | 24.67 |
| ESP32-P4 SPI · 240×240 / normal | 192.14 | 152.00 | 29.22 |
| ESP32-P4 SPI · 240×240 / full repaint | 192.14 | 152.00 | 29.22 |
| ESP32-P4 DSI 360 MHz · 1024×600 / normal | 161.03 | 64.00 | 24.56 |
| ESP32-P4 DSI 360 MHz · 1024×600 / full repaint | 161.03 | 64.00 | 24.56 |
| ESP32-P4 DSI 400 MHz · 1024×600 / normal | 162.57 | 122.00 | 24.56 |
| ESP32-P4 DSI 400 MHz · 1024×600 / full repaint | 162.57 | 122.00 | 24.56 |

</div>

Measured after display and UI initialization, rather than the minimum across the full replay.

</details>

## All 58 cases

Expand the tables for per-case FPS. The CSV files retain per-case CPU, memory and observed ranges across the two runs.

<details>
<summary>Normal updates: all 58 cases, FPS</summary>

<div class="gsp-statistics">

| Case | ESP32-C3 SPI · 240×240 | ESP32-C5 QSPI · 466×466 | ESP32-S3 SPI · 320×240 | ESP32-S3 QSPI · 360×360 | ESP32-S3 RGB · 800×480 | ESP32-S31 RGB · 480×480 | ESP32-S31 RGB · 480×800 | ESP32-S31 RGB888 · 800×480 | ESP32-P4 SPI · 240×240 | ESP32-P4 DSI 360 MHz · 1024×600 | ESP32-P4 DSI 400 MHz · 1024×600 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| rect x1 | 61.13 | 58.44 | 61.13 | 42.14 | 27.80 | 42.19 | 43.37 | 36.18 | 60.98 | 42.87 | 43.03 |
| rect x9 | 52.21 | 64.60 | 44.40 | 42.08 | 34.96 | 60.33 | 61.04 | 47.04 | 84.93 | 60.44 | 60.60 |
| containers | 13.66 | 19.01 | 18.72 | 19.79 | 19.34 | 26.82 | 27.90 | 38.03 | 29.06 | 28.51 | 28.71 |
| shapes | 9.99 | 8.66 | 17.76 | 15.72 | 12.41 | 19.64 | 18.76 | 32.96 | 24.87 | 21.30 | 22.78 |
| clock needles | 8.38 | 8.04 | 49.65 | 27.85 | 23.16 | 59.99 | 55.21 | 46.83 | 58.47 | 56.38 | 55.32 |
| gradients | 33.59 | 37.42 | 28.85 | 31.85 | 21.02 | 60.32 | 49.95 | 47.11 | 76.90 | 40.00 | 45.06 |
| opa fills | 20.07 | 14.69 | 26.63 | 20.99 | 16.82 | 28.23 | 30.20 | 39.81 | 34.00 | 29.70 | 30.20 |
| opa layers | 17.95 | 11.93 | 24.99 | 21.10 | 15.30 | 26.08 | 25.75 | 38.56 | 27.09 | 25.54 | 26.87 |
| overlay | 14.11 | 14.39 | 14.04 | 14.96 | 10.38 | 19.06 | 16.66 | 35.69 | 18.21 | 18.05 | 17.94 |
| labels | 68.07 | 62.41 | 71.41 | 53.64 | 35.19 | 60.33 | 61.05 | 46.95 | 153.91 | 60.44 | 60.47 |
| screen text | 17.96 | 14.42 | 15.07 | 15.08 | 8.97 | 18.44 | 17.87 | 28.52 | 19.47 | 17.04 | 17.78 |
| scroll rows | 47.29 | 35.71 | 28.57 | 37.13 | 35.23 | 60.33 | 60.99 | 46.96 | 76.91 | 60.48 | 60.49 |
| arcs x6 | 46.10 | 58.53 | 59.78 | 40.67 | 28.24 | 42.84 | 44.36 | 36.13 | 60.87 | 44.24 | 44.53 |
| img rgb x9 | 51.93 | 40.00 | 58.80 | 48.01 | 32.82 | 60.32 | 61.10 | 46.88 | 76.91 | 60.62 | 60.57 |
| img argb x9 | 31.24 | 24.00 | 55.54 | 36.71 | 20.00 | 60.33 | 45.30 | 46.88 | 71.40 | 45.29 | 52.46 |
| img qoi x9 | 27.77 | 36.94 | 26.73 | 27.56 | 35.00 | 60.33 | 61.04 | 47.09 | 99.97 | 60.64 | 60.64 |
| image scale | 49.89 | 44.21 | 28.54 | 31.37 | 22.85 | 60.33 | 60.95 | 47.01 | 62.31 | 60.49 | 60.46 |
| image rotation | 55.83 | 62.57 | 62.36 | 45.37 | 34.99 | 60.33 | 60.99 | 47.02 | 62.46 | 60.38 | 60.57 |
| vector resize | 52.18 | 62.29 | 62.56 | 49.21 | 35.27 | 60.33 | 61.04 | 46.85 | 62.38 | 60.42 | 60.40 |
| vector rotation | 39.37 | 55.54 | 62.50 | 40.02 | 35.09 | 60.33 | 61.06 | 46.87 | 62.44 | 60.45 | 60.43 |
| vector tint | 37.85 | 54.70 | 54.77 | 52.41 | 35.06 | 52.76 | 54.70 | 46.85 | 54.59 | 54.36 | 54.61 |
| vector morph | 55.54 | 61.91 | 62.52 | 50.11 | 35.26 | 60.33 | 61.07 | 46.84 | 62.48 | 60.48 | 60.49 |
| vector move | 39.80 | 62.55 | 62.56 | 60.06 | 35.26 | 60.33 | 61.03 | 46.84 | 62.40 | 60.42 | 60.39 |
| vector fit | 32.20 | 42.56 | 58.27 | 37.64 | 35.12 | 60.33 | 61.06 | 46.85 | 62.50 | 60.44 | 60.42 |
| vector paints | 45.77 | 62.59 | 62.61 | 60.11 | 35.27 | 60.33 | 61.00 | 47.02 | 62.33 | 60.41 | 60.39 |
| vector eyes | 12.86 | 14.23 | 14.25 | 11.98 | 12.97 | 18.74 | 21.11 | 18.60 | 24.83 | 21.24 | 21.53 |
| effects mixed | 24.99 | 43.85 | 56.33 | 37.12 | 35.13 | 60.33 | 60.94 | 46.87 | 91.65 | 60.44 | 60.40 |
| gram_te | 249.95 | 238.64 | 249.95 | 60.08 | 35.22 | 60.33 | 60.99 | 47.01 | 245.73 | 60.39 | 60.58 |
| wallpaper | 18.27 | 9.36 | 17.41 | 17.64 | 11.73 | 23.09 | 22.33 | 33.15 | 24.56 | 22.44 | 23.16 |
| static move | 30.03 | 58.14 | 40.32 | 41.68 | 35.14 | 60.33 | 60.93 | 46.96 | 91.23 | 60.53 | 60.48 |
| moving | 28.17 | 56.11 | 52.20 | 40.71 | 35.05 | 60.34 | 60.99 | 46.93 | 86.76 | 60.40 | 60.38 |
| grid album | 34.76 | 32.38 | 15.55 | 34.18 | 21.00 | 45.50 | 12.37 | 26.37 | 39.43 | 37.34 | 40.21 |
| messages | 38.37 | 38.45 | 47.48 | 37.06 | 25.66 | 60.32 | 59.65 | 46.89 | 76.90 | 60.48 | 60.48 |
| widgets | 37.18 | 53.28 | 98.72 | 52.70 | 35.10 | 60.33 | 60.94 | 46.89 | 99.56 | 60.49 | 60.46 |
| page flow | 6.79 | 4.80 | 10.79 | 11.98 | 4.79 | 17.55 | 17.46 | 14.56 | 23.92 | 16.24 | 16.64 |
| stack view | 36.00 | 13.39 | 29.63 | 36.33 | 12.69 | 56.54 | 56.58 | 43.08 | 81.57 | 52.77 | 52.73 |
| composites | 14.75 | 17.20 | 20.58 | 17.46 | 10.71 | 25.45 | 23.03 | 21.16 | 25.30 | 23.89 | 24.38 |
| drawer | 23.27 | 15.20 | 18.20 | 21.28 | 12.21 | 28.69 | 28.44 | 26.39 | 27.70 | 27.14 | 28.07 |
| wheels x3 | 69.16 | 69.83 | 72.96 | 45.47 | 25.41 | 44.68 | 47.45 | 36.09 | 72.44 | 46.85 | 47.23 |
| anim gif x4 | 20.14 | 20.13 | 20.15 | 19.98 | 20.07 | 20.22 | 20.24 | 20.14 | 19.92 | 20.15 | 20.13 |
| anim sparse | 20.16 | 20.16 | 20.16 | 20.06 | 20.15 | 20.24 | 20.18 | 20.15 | 19.92 | 20.14 | 20.22 |
| render storm | 5.93 | 4.25 | 17.64 | 10.82 | 6.42 | 30.17 | 21.42 | 22.29 | 55.51 | 19.63 | 22.59 |
| slide left | 30.33 | 15.84 | 25.82 | 28.04 | 12.79 | 42.18 | 38.76 | 35.55 | 68.77 | 37.38 | 38.78 |
| slide right | 29.49 | 15.83 | 25.82 | 28.18 | 12.78 | 42.18 | 38.66 | 35.52 | 68.75 | 37.47 | 38.76 |
| slide up | 32.56 | 15.97 | 25.08 | 28.86 | 12.06 | 42.18 | 40.50 | 34.35 | 68.81 | 35.77 | 37.10 |
| slide down | 32.39 | 15.97 | 25.08 | 28.82 | 12.07 | 42.18 | 40.56 | 34.34 | 68.78 | 35.76 | 37.09 |
| cross fade | 28.58 | 6.62 | 17.51 | 15.39 | 5.20 | 40.23 | 30.17 | 13.04 | 63.42 | 25.13 | 27.29 |
| fade through black | 28.94 | 7.96 | 22.02 | 22.14 | 7.56 | 29.08 | 23.01 | 18.78 | 65.23 | 20.56 | 22.40 |
| drag commit | 25.12 | 22.09 | 25.21 | 30.30 | 15.87 | 39.35 | 32.72 | 35.85 | 36.35 | 32.94 | 35.19 |
| drag cancel | 17.33 | 18.16 | 18.23 | 22.35 | 14.23 | 31.96 | 26.77 | 29.50 | 25.12 | 30.37 | 31.51 |
| drag flick | 9.90 | 8.26 | 9.32 | 10.87 | 6.22 | 13.80 | 14.55 | 12.91 | 18.07 | 13.96 | 15.07 |
| drag slide fade | 19.79 | 14.22 | 25.21 | 28.64 | 14.95 | 38.63 | 32.77 | 33.94 | 36.36 | 31.50 | 34.26 |
| blend load x1 | 142.81 | 124.79 | 199.80 | 60.10 | 35.12 | 60.33 | 60.90 | 47.15 | 243.48 | 60.54 | 60.50 |
| blend load x8 | 47.54 | 29.47 | 27.78 | 36.07 | 32.20 | 60.32 | 60.80 | 47.14 | 71.32 | 60.48 | 60.47 |
| blend load x32 | 23.80 | 8.57 | 24.39 | 20.02 | 12.40 | 30.16 | 26.27 | 46.98 | 62.39 | 24.64 | 27.69 |
| blend load x64 | 14.08 | 4.61 | 20.72 | 14.14 | 7.95 | 20.11 | 15.70 | 26.03 | 49.94 | 14.05 | 15.94 |
| dropdown select | 3.98 | 3.98 | 3.99 | 3.98 | 3.99 | 3.99 | 3.98 | 3.98 | 3.99 | 3.99 | 3.99 |
| motion | 39.43 | 31.57 | 31.64 | 30.79 | 23.32 | 41.10 | 39.89 | 42.25 | 43.82 | 41.48 | 42.22 |

</div>

</details>

<details>
<summary>Full-repaint pressure: all 58 cases, FPS</summary>

<div class="gsp-statistics">

| Case | ESP32-C3 SPI · 240×240 | ESP32-C5 QSPI · 466×466 | ESP32-S3 SPI · 320×240 | ESP32-S3 QSPI · 360×360 | ESP32-S3 RGB · 800×480 | ESP32-S31 RGB · 480×480 | ESP32-S31 RGB · 480×800 | ESP32-S31 RGB888 · 800×480 | ESP32-P4 SPI · 240×240 | ESP32-P4 DSI 360 MHz · 1024×600 | ESP32-P4 DSI 400 MHz · 1024×600 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| rect x1 | 53.05 | 34.80 | 29.65 | 39.97 | 24.81 | 60.33 | 58.82 | 47.03 | 76.92 | 60.62 | 60.58 |
| rect x9 | 36.51 | 33.77 | 28.74 | 37.09 | 23.41 | 60.32 | 59.01 | 46.99 | 76.92 | 60.55 | 60.53 |
| containers | 17.03 | 21.27 | 27.75 | 29.08 | 19.20 | 60.32 | 47.62 | 47.08 | 71.43 | 60.64 | 60.63 |
| shapes | 12.98 | 10.69 | 28.57 | 24.14 | 14.21 | 35.34 | 30.27 | 46.86 | 76.90 | 40.04 | 45.48 |
| clock needles | 8.03 | 7.50 | 28.13 | 24.16 | 15.23 | 42.53 | 37.74 | 46.84 | 71.43 | 49.44 | 56.48 |
| gradients | 33.31 | 26.36 | 28.85 | 31.80 | 15.38 | 51.92 | 39.35 | 47.00 | 76.91 | 35.01 | 38.71 |
| opa fills | 26.32 | 17.90 | 29.04 | 30.10 | 17.74 | 60.33 | 47.46 | 47.06 | 73.05 | 52.63 | 58.82 |
| opa layers | 23.17 | 13.79 | 29.30 | 30.07 | 16.51 | 60.32 | 41.71 | 47.07 | 76.90 | 43.48 | 49.98 |
| overlay | 25.61 | 26.32 | 28.56 | 29.19 | 16.99 | 60.32 | 40.00 | 47.08 | 71.43 | 45.45 | 50.00 |
| labels | 45.42 | 35.80 | 28.57 | 39.78 | 24.13 | 60.33 | 60.97 | 46.93 | 76.92 | 60.39 | 60.48 |
| screen text | 43.47 | 27.03 | 28.57 | 30.08 | 14.22 | 60.33 | 47.63 | 35.71 | 76.91 | 40.00 | 47.60 |
| scroll rows | 47.61 | 35.71 | 28.57 | 37.00 | 23.58 | 60.33 | 58.09 | 46.98 | 76.91 | 60.47 | 60.55 |
| arcs x6 | 30.58 | 24.12 | 27.69 | 22.73 | 14.26 | 30.17 | 31.76 | 39.99 | 66.70 | 34.62 | 40.13 |
| img rgb x9 | 52.61 | 35.75 | 29.39 | 37.39 | 21.17 | 60.32 | 60.98 | 46.99 | 76.92 | 60.59 | 60.70 |
| img argb x9 | 31.24 | 15.62 | 28.56 | 26.41 | 11.92 | 30.16 | 26.19 | 46.87 | 71.43 | 23.25 | 27.89 |
| img qoi x9 | 18.50 | 17.70 | 26.83 | 27.41 | 18.63 | 60.34 | 47.41 | 47.09 | 72.09 | 60.46 | 60.64 |
| image scale | 48.11 | 35.68 | 28.51 | 30.06 | 17.42 | 60.33 | 60.95 | 47.01 | 76.92 | 60.49 | 60.56 |
| image rotation | 39.78 | 33.74 | 29.51 | 30.49 | 21.65 | 60.33 | 58.93 | 47.02 | 76.92 | 60.47 | 60.56 |
| vector resize | 34.21 | 29.47 | 29.41 | 30.30 | 21.58 | 60.33 | 52.14 | 46.86 | 76.92 | 60.43 | 60.41 |
| vector rotation | 28.37 | 25.25 | 27.88 | 30.12 | 19.90 | 60.33 | 47.62 | 46.87 | 76.36 | 60.45 | 60.43 |
| vector tint | 30.29 | 35.79 | 29.41 | 39.86 | 24.67 | 60.33 | 60.93 | 46.86 | 76.92 | 60.44 | 60.41 |
| vector morph | 38.45 | 27.89 | 29.41 | 30.19 | 21.71 | 60.34 | 55.58 | 46.84 | 76.92 | 60.40 | 60.38 |
| vector move | 28.62 | 34.48 | 29.41 | 39.76 | 24.33 | 60.33 | 61.07 | 46.85 | 76.92 | 60.41 | 60.39 |
| vector fit | 24.67 | 22.46 | 26.66 | 28.11 | 18.49 | 60.33 | 45.25 | 46.85 | 72.61 | 60.44 | 60.43 |
| vector paints | 23.84 | 29.68 | 28.57 | 37.17 | 23.73 | 60.33 | 59.34 | 46.93 | 76.92 | 60.42 | 60.40 |
| vector eyes | 24.91 | 26.73 | 29.38 | 28.72 | 17.65 | 60.33 | 42.94 | 46.86 | 76.92 | 60.46 | 60.44 |
| effects mixed | 16.70 | 20.90 | 24.35 | 24.04 | 16.34 | 60.33 | 42.28 | 46.89 | 62.91 | 60.46 | 60.45 |
| gram_te | 55.49 | 37.12 | 29.59 | 40.11 | 25.90 | 60.33 | 61.04 | 47.02 | 76.92 | 60.39 | 60.48 |
| wallpaper | 33.31 | 12.19 | 28.57 | 30.10 | 15.75 | 60.32 | 52.63 | 47.08 | 76.90 | 49.99 | 55.52 |
| static move | 28.66 | 28.63 | 28.88 | 30.08 | 21.93 | 60.33 | 59.49 | 46.97 | 76.59 | 60.41 | 60.54 |
| moving | 27.56 | 28.01 | 29.42 | 30.10 | 22.03 | 60.33 | 57.79 | 46.84 | 76.91 | 60.40 | 60.38 |
| grid album | 50.22 | 34.24 | 33.53 | 41.20 | 23.66 | 58.94 | 51.05 | 45.83 | 95.66 | 58.14 | 58.87 |
| messages | 34.40 | 26.99 | 28.72 | 30.10 | 18.36 | 60.33 | 47.49 | 46.86 | 76.88 | 60.48 | 60.49 |
| widgets | 19.39 | 20.03 | 27.73 | 30.12 | 19.76 | 60.34 | 46.48 | 46.89 | 71.43 | 60.48 | 60.46 |
| page flow | 31.27 | 24.72 | 28.31 | 31.19 | 17.29 | 59.13 | 53.13 | 45.76 | 76.28 | 58.13 | 58.10 |
| stack view | 36.76 | 10.78 | 26.10 | 30.05 | 10.84 | 56.13 | 53.17 | 46.14 | 74.97 | 51.91 | 51.94 |
| composites | 30.99 | 26.31 | 28.54 | 29.78 | 17.11 | 60.25 | 51.84 | 46.35 | 75.01 | 55.41 | 59.19 |
| drawer | 33.50 | 19.05 | 27.29 | 28.77 | 17.94 | 56.17 | 26.94 | 44.26 | 74.55 | 51.76 | 54.30 |
| wheels x3 | 44.62 | 28.38 | 29.34 | 32.95 | 19.99 | 60.33 | 49.27 | 46.82 | 76.91 | 60.41 | 60.38 |
| anim gif x4 | 51.93 | 34.37 | 29.29 | 40.12 | 25.16 | 60.33 | 60.95 | 47.02 | 76.85 | 60.40 | 60.39 |
| anim sparse | 55.31 | 36.64 | 29.51 | 40.10 | 26.29 | 60.33 | 60.99 | 47.02 | 76.92 | 60.39 | 60.38 |
| render storm | 5.91 | 4.24 | 17.59 | 10.82 | 5.00 | 30.17 | 17.79 | 18.01 | 55.50 | 15.67 | 18.18 |
| slide left | 30.17 | 15.80 | 25.82 | 28.03 | 12.79 | 42.18 | 38.74 | 34.81 | 68.79 | 37.42 | 38.81 |
| slide right | 29.35 | 15.81 | 25.82 | 28.06 | 12.80 | 42.18 | 38.65 | 35.26 | 68.78 | 37.48 | 38.76 |
| slide up | 32.39 | 15.97 | 25.08 | 28.89 | 12.08 | 42.18 | 40.50 | 33.78 | 68.81 | 35.77 | 37.10 |
| slide down | 32.22 | 15.97 | 25.08 | 28.78 | 12.07 | 42.18 | 40.50 | 33.70 | 68.77 | 35.76 | 37.09 |
| cross fade | 28.49 | 6.58 | 17.49 | 15.43 | 5.20 | 40.23 | 30.17 | 13.11 | 63.34 | 25.10 | 27.29 |
| fade through black | 28.87 | 7.95 | 22.02 | 22.14 | 7.56 | 29.08 | 23.39 | 19.06 | 65.28 | 20.56 | 22.39 |
| drag commit | 25.05 | 22.06 | 25.22 | 30.31 | 15.83 | 39.30 | 32.62 | 36.55 | 36.35 | 32.97 | 35.19 |
| drag cancel | 17.29 | 18.14 | 18.23 | 22.34 | 14.27 | 31.51 | 27.05 | 29.99 | 25.13 | 29.83 | 31.38 |
| drag flick | 9.94 | 8.24 | 9.32 | 10.87 | 6.21 | 14.12 | 14.50 | 13.11 | 18.06 | 13.98 | 15.21 |
| drag slide fade | 19.76 | 14.21 | 25.21 | 28.46 | 14.97 | 40.30 | 32.72 | 33.96 | 36.37 | 31.49 | 34.24 |
| blend load x1 | 49.98 | 35.71 | 27.78 | 38.86 | 24.60 | 60.33 | 60.98 | 47.11 | 76.90 | 60.58 | 60.55 |
| blend load x8 | 47.60 | 26.12 | 27.78 | 30.12 | 20.18 | 60.33 | 50.00 | 47.14 | 71.46 | 60.51 | 60.46 |
| blend load x32 | 28.57 | 8.93 | 27.77 | 22.57 | 11.89 | 30.17 | 25.98 | 46.98 | 71.46 | 25.31 | 28.16 |
| blend load x64 | 16.33 | 4.75 | 23.52 | 15.04 | 7.86 | 20.11 | 15.75 | 27.04 | 55.67 | 14.49 | 16.54 |
| dropdown select | 48.58 | 35.09 | 29.38 | 39.29 | 24.47 | 60.33 | 60.91 | 46.85 | 76.77 | 60.54 | 60.46 |
| motion | 46.18 | 29.28 | 29.40 | 30.09 | 20.33 | 60.32 | 49.85 | 46.96 | 76.92 | 60.62 | 60.59 |

</div>

</details>

A cancelled drag may submit no image; interaction completion must also be checked through its functional outcome.

## Hardware configurations

Results include both rendering and display output costs.

<details>
<summary>Display and processor configurations</summary>

<div class="gsp-statistics">

| Configuration | CPU MHz | Interface | Logical → panel | Rotation | Format |
| --- | ---: | ---: | ---: | ---: | ---: |
| ESP32-C3 SPI · 240×240 | 160 | SPI | 240×240 → 240×240 | 0° | RGB565 |
| ESP32-C5 QSPI · 466×466 | 240 | QSPI | 466×466 → 466×466 | 0° | RGB565 |
| ESP32-S3 SPI · 320×240 | 240 | SPI | 320×240 → 320×240 | 0° | RGB565 |
| ESP32-S3 QSPI · 360×360 | 240 | QSPI | 360×360 → 360×360 | 0° | RGB565 |
| ESP32-S3 RGB · 800×480 | 240 | RGB | 800×480 → 800×480 | 0° | RGB565 |
| ESP32-S31 RGB · 480×480 | 320 | RGB | 480×480 → 480×480 | 0° | RGB565 |
| ESP32-S31 RGB · 480×800 | 320 | RGB | 480×800 → 800×480 | 270° | RGB565 |
| ESP32-S31 RGB888 · 800×480 | 320 | RGB | 800×480 → 800×480 | 0° | RGB888 |
| ESP32-P4 SPI · 240×240 | 400 | SPI | 240×240 → 240×240 | 0° | RGB565 |
| ESP32-P4 DSI 360 MHz · 1024×600 | 360 | MIPI-DSI | 1024×600 → 1024×600 | 0° | RGB565 |
| ESP32-P4 DSI 400 MHz · 1024×600 | 400 | MIPI-DSI | 1024×600 → 1024×600 | 0° | RGB565 |

</div>

GSP selects accelerators using its default policy. MIPI-DSI uses a 48 MHz pixel clock. ESP32-C5 has 4 MiB PSRAM with XIP disabled. Presentation mode and framebuffer count are listed in the resource CSV.

</details>

## Measurement method

<details>
<summary>Sampling details and repeatability</summary>

Tests use GSP 1.10.3, GSPC 0.11.2 and ESP-IDF `b6293eeb209`. Each configuration and mode is measured in two independent runs, with warm-up before all 58 cases.

- **FPS**: total frames divided by total sampled time, rather than optical screen refresh. The ordinary driver uses 16 ms; text, effect, scroll and static-move callbacks use 4 ms. Text updates simulate scrolling by rewriting eight rows.
- **CPU**: non-idle runtime divided by total capacity across all configured cores.
- **Animation**: each view is 60×60 when the logical width is below 320, and 120×120 otherwise.
- **Full-repaint pressure**: each full-screen request need not produce a frame. On C3, cross-fade falls back to fade-through-black when snapshots are unavailable.
- **Memory**: startup increases track heap changes from application entry to UI startup; per-case increases are sampled values, rather than exact peaks. ELF external-RAM payload and heap increases must not simply be added.

KiB = 1,024 bytes; MiB = 1,048,576 bytes.

Across configuration/mode pairs, the largest relative range in whole-suite weighted FPS between runs is 0.75%, and the largest CPU range is 0.17 percentage points. Per-case ranges are in the CSV; these observed ranges are not confidence intervals.

</details>
