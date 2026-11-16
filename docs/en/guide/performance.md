# Performance statistics

**GSP 1.10.3 and LVGL 9.6.0** run the same assets, layouts and requested updates on ESP32-S3 and ESP32-P4, comparing FPS, CPU use and resource costs. GSP prepares scenes and assets at build time to reduce repeated computation on the device.

**Download data:** [Comparison CSV (75 records)](../../assets/performance/gsp-1.10.3-lvgl-9.6.0/comparison.csv) · [Resource CSV (19 records)](../../assets/performance/gsp-1.10.3-lvgl-9.6.0/resources.csv) · [More hardware configurations](hardware-benchmark.md)

<details>
<summary>What the CSV files contain</summary>

| File | Contents |
| --- | --- |
| `comparison.csv` · 75 records | Median FPS / CPU for both frameworks with the observed range across two runs, plus a P4 reference configuration with LVGL PPA enabled. |
| `resources.csv` · 19 records | Complete application Flash, static SRAM and runtime minimum free SRAM / PSRAM by suite, in bytes. |

</details>

## Results at a glance

**How to read the tables:** all value pairs are GSP / LVGL. The Difference column compares GSP with LVGL: it shows the FPS ratio when GSP is more than 5% faster, and the relative CPU change otherwise. CPU is averaged across all cores, so about 50% on a dual-core chip means one core is fully busy. Non-animation updates request 60 Hz; light workloads reach about 42 FPS on S3 with this display configuration. The overview covers only the configurations measured here.

<div class="gsp-statistics">

| Metric | ESP32-S3 | ESP32-P4 |
| --- | ---: | ---: |
| Comparable results | 42 | 14 |
| GSP more than 5% faster | 27 | 3 |
| Similar FPS (within ±5%) | 15 | 11 |
| LVGL more than 5% faster | 0 | 0 |
| GSP CPU reduction at similar FPS | 42.4% | 67.0% |
| FPS ratio, ten core scenarios | 1.74× | 1.42× |

</div>

- **Less CPU for fixed decoration:** with 32 fixed translucent layers, FPS is similar and CPU drops by 75% (S3) / 85% (P4).
- **Smoother image scrolling:** on P4, scrolling 16 / 64 PNG crops holds about 58 FPS, compared with 30 / 4.4 FPS for LVGL.
- **Lists and animation:** on S3, the 1024-row list runs at 1.39× and four Cube views at 2.45×; on P4, a single Cube plays at similar FPS with 79% less CPU.

<details>
<summary>How the overview is calculated</summary>

- **Comparable results:** all main-configuration FPS scenarios, excluding static idle, interaction replays, frame-preparation throughput and the PPA reference. FPS ratios from 0.95 to 1.05 count as similar.
- **CPU reduction:** `1 − geometric mean(GSP CPU / LVGL CPU)` across the similar-FPS cases.
- **Ten core scenarios:** five fixed-decoration cases and five image and animation cases, using the [geometric mean](https://www.spec.org/gwpg/pastissues/Jan1_17/gpc.static/geometric.html) of per-case `GSP FPS / LVGL FPS` with equal weights; this does not represent a product's usage mix. The 64-PNG case has the largest effect; without it, the ratio is 1.39× (S3) / 1.11× (P4). Among these ten, CPU drops by 59.0% (S3, four cases) / 68.2% (P4, seven cases) at similar FPS.

</details>

## Where GSP gains come from

GSP completes work that can be decided at build time to reduce runtime computation:

- **Pre-rendered fixed content (baking):** backgrounds, scales, cards and other fixed decoration are composed into images at build time, then reused and composed with live values and markers at runtime.
- **Preconverted images:** PNGs become runtime assets at build time, avoiding on-device decoding of the original PNG; compressed assets still require decoding.
- **Preprocessed animation:** GIFs become animation resources with preprocessed frame differences, reducing frame preparation and redraw work on the device.

The LVGL cases use native objects and PNG / GIF playback paths, with interfaces that can be built and changed flexibly at runtime. Fixed content and known assets provide more opportunities for GSP preprocessing; this can increase Flash use, as shown in [Memory and Flash](#memory-and-flash).

## Fixed decoration and live updates

GSP bakes fixed decoration while values and markers update live; LVGL uses native objects with the same layout. In the first four cases both run at similar FPS and GSP uses 28%–85% less CPU. The 32 large changing layers repaint every frame, and GSP runs 29%–44% faster.

<div class="gsp-statistics">

**ESP32-S3**

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| Fixed HMI + four live values | 42.17 / 42.17 | 15.61 / 38.47 | CPU −59% |
| 16 fixed translucent layers + moving marker | 42.18 / 42.17 | 7.49 / 18.90 | CPU −60% |
| 32 fixed translucent layers + moving marker | 42.18 / 42.17 | 7.57 / 30.89 | CPU −75% |
| Fixed instrument scales + live readings | 42.18 / 41.97 | 33.24 / 46.14 | CPU −28% |
| 32 large changing layers | 4.24 / 2.95 | 49.90 / 50.09 | FPS 1.44× |

**ESP32-P4**

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| Fixed HMI + four live values | 59.98 / 59.95 | 3.99 / 13.05 | CPU −69% |
| 16 fixed translucent layers + moving marker | 59.99 / 59.89 | 2.12 / 8.88 | CPU −76% |
| 32 fixed translucent layers + moving marker | 59.99 / 59.83 | 2.11 / 14.43 | CPU −85% |
| Fixed instrument scales + live readings | 59.99 / 59.96 | 6.32 / 12.89 | CPU −51% |
| 32 large changing layers | 6.88 / 5.34 | 49.47 / 49.48 | FPS 1.29× |

</div>

## Images and lists

On P4, GSP holds about 58 FPS in all three cases; on S3, it runs at 1.39×–13.3×. Both use the same 160×120 PNG crops and 2 MiB image caches: GSP preconverts them at build time, while LVGL decodes and caches them at runtime. The 64 crops occupy about 2.34 MiB and exceed the cache, which produces the largest gap. Both lists reuse visible rows.

<div class="gsp-statistics">

**ESP32-S3**

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| 1024-row list | 16.25 / 11.72 | 49.85 / 49.89 | FPS 1.39× |
| 16 PNG crops scrolling | 13.23 / 6.93 | 49.75 / 50.05 | FPS 1.91× |
| 64 PNG crops scrolling | 12.89 / 0.97 | 49.75 / 49.96 | FPS 13.3× |

**ESP32-P4**

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| 1024-row list | 58.16 / 58.10 | 15.63 / 39.74 | CPU −61% |
| 16 PNG crops scrolling | 58.17 / 30.41 | 22.64 / 46.59 | FPS 1.91× |
| 64 PNG crops scrolling | 58.17 / 4.41 | 23.95 / 49.42 | FPS 13.2× |

</div>

## Animation updates

Both play the same source GIFs: GSP converts them into animation resources at build time, while LVGL uses its native GIF playback.

### Normal playback

One asset at a time, at original size, following source timing. FPS counts only successful submissions of new source pictures. On P4 both reach the source cadence and GSP uses 45%–79% less CPU. On S3, GSP reaches the source cadence in all four cases and runs Cube and Paper 23%–38% faster.

<div class="gsp-statistics">

**ESP32-S3**

| Single asset | Source FPS | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: | ---: |
| Scanning GIF (120×120) | 20 | 19.97 / 20.10 | 4.61 / 10.39 | CPU −56% |
| Sparse-change GIF (120×120) | 20 | 20.03 / 20.04 | 3.38 / 12.55 | CPU −73% |
| Cube (256×256) | 25 | 24.98 / 20.30 | 17.29 / 49.93 | FPS 1.23× |
| Paper (256×256) | 26.32 average | 26.32 / 19.12 | 37.19 / 45.70 | FPS 1.38× |

**ESP32-P4** (400 MHz)

| Single asset | Source FPS | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: | ---: |
| Scanning GIF (120×120) | 20 | 20.10 / 20.05 | 0.98 / 1.79 | CPU −45% |
| Sparse-change GIF (120×120) | 20 | 20.10 / 20.10 | 0.71 / 2.19 | CPU −68% |
| Cube (256×256) | 25 | 24.98 / 25.09 | 1.68 / 8.17 | CPU −79% |
| Paper (256×256) | 26.32 average | 26.30 / 26.31 | 3.42 / 8.16 | CPU −58% |

</div>

Paper uses variable 30 / 70 ms delays; its source FPS is the loop average.

### Multiple animations with live UI

Several animation views play in sync, with live values added in the second case. This section counts whole-page submissions, including updates triggered by live values, rather than the content FPS of an individual GIF. On S3, GSP runs at more than twice the FPS; on P4, both run at similar FPS and GSP uses 28%–78% less CPU.

<div class="gsp-statistics">

**ESP32-S3**

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| Four synchronized Cube views | 15.07 / 6.15 | 49.70 / 49.85 | FPS 2.45× |
| Four synchronized animations + 16 live values | 34.07 / 16.88 | 49.94 / 50.02 | FPS 2.02× |

**ESP32-P4**

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| Four synchronized Cube views | 24.98 / 24.98 | 8.58 / 38.66 | CPU −78% |
| Four synchronized animations + 16 live values | 60.57 / 60.16 | 12.59 / 17.43 | CPU −28% |

</div>

### Continuous frame-preparation throughput

Every frame is prepared in order into an RGB565 framebuffer in PSRAM, without waiting for source delays. GSP reaches 2.12×–18.1× the LVGL rate. This measures device-side frame preparation, excluding UI composition and display transfer, and is not screen refresh.

<div class="gsp-statistics">

| Single asset | S3 frames/s | S3 ratio | P4 frames/s | P4 ratio |
| --- | ---: | ---: | ---: | ---: |
| Scanning GIF (120×120) | 12,879 / 1,077 | 12.0× | 30,517 / 2,589 | 11.8× |
| Sparse-change GIF (120×120) | 8,782 / 485 | 18.1× | 18,625 / 1,263 | 14.7× |
| Cube (256×256) | 618 / 93 | 6.62× | 2,173 / 223 | 9.74× |
| Paper (256×256) | 184 / 87 | 2.12× | 657 / 237 | 2.77× |

</div>

## Dragging and dropdown interaction

These S3 software-input replays pass functional end-state checks on both frameworks. The table reports observed loads during each native interaction replay. Inertia and release curves differ, so the results neither rank smoothness by frame count nor establish CPU efficiency at equal smoothness.

<div class="gsp-statistics">

| Action | CPU % | Difference |
| --- | ---: | ---: |
| Full-screen page drag | 25.23 / 42.06 | CPU −40% |
| Short drag and rebound | 23.05 / 23.05 | Even |
| List swipe and selection | 41.86 / 48.16 | CPU −13% |
| Pull-down panel | 18.12 / 28.36 | CPU −36% |
| Dropdown selection | 1.77 / 9.71 | CPU −82% |

</div>

## More scenarios

The following 28 comparable S3 scenarios show GSP faster in 19; the other nine run at similar FPS, seven of them with lower GSP CPU. Drawing cases follow LVGL benchmark workload families, without its official total score.

<details>
<summary>Transforms and controls (14 cases)</summary>

<div class="gsp-statistics">

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| Flat HMI + four live values | 42.17 / 42.17 | 14.91 / 33.21 | CPU −55% |
| PNG scaling | 42.16 / 42.14 | 12.87 / 29.11 | CPU −56% |
| PNG rotation | 42.15 / 42.14 | 30.11 / 35.07 | CPU −14% |
| Sensor value updates | 17.26 / 12.69 | 49.88 / 49.95 | FPS 1.36× |
| Status paragraph updates | 19.76 / 15.03 | 49.89 / 49.84 | FPS 1.31× |
| Progress, switches and arcs | 42.16 / 42.15 | 21.24 / 22.25 | CPU −5% |
| 40-row list scrolling | 17.87 / 13.78 | 49.59 / 49.90 | FPS 1.30× |
| Sparse dashboard updates | 42.15 / 42.15 | 8.71 / 11.50 | CPU −24% |
| Live dashboard | 42.16 / 42.14 | 28.88 / 35.15 | CPU −18% |
| Moving card | 42.15 / 42.15 | 11.79 / 11.47 | CPU +3% |
| 8 changing translucent layers | 42.14 / 38.50 | 42.47 / 50.11 | FPS 1.09× |
| 32 changing translucent layers | 9.22 / 8.75 | 49.86 / 49.88 | FPS 1.05× |
| Fixed control panel and live foreground | 42.15 / 42.14 | 15.01 / 22.13 | CPU −32% |
| Static idle | 0.00 / 0.00 | 0.65 / 1.41 | — |

</div>

Static pages stop submitting on demand; 0 FPS does not indicate a stall.

</details>

<details>
<summary>Drawing and composition (15 cases)</summary>

<div class="gsp-statistics">

| Scene | FPS | CPU % | Difference |
| --- | ---: | ---: | ---: |
| Full-screen background recolor | 17.45 / 15.80 | 49.72 / 49.67 | FPS 1.10× |
| Moving background | 13.56 / 3.80 | 49.68 / 50.30 | FPS 3.57× |
| Single rectangle recolor | 42.15 / 42.14 | 15.22 / 14.26 | CPU +7% |
| Nine rectangles recolor | 27.33 / 23.71 | 49.87 / 49.99 | FPS 1.15× |
| Moving opaque images | 27.46 / 13.50 | 49.47 / 49.45 | FPS 2.03× |
| Moving alpha images | 18.77 / 13.83 | 49.53 / 49.85 | FPS 1.36× |
| Moving and rotating alpha images | 20.59 / 18.44 | 49.66 / 49.64 | FPS 1.12× |
| Label recolor | 25.69 / 8.75 | 49.77 / 49.70 | FPS 2.93× |
| Multi-screen text scrolling | 10.13 / 5.43 | 49.82 / 49.79 | FPS 1.87× |
| Multiple live arcs | 11.90 / 7.51 | 49.80 / 50.08 | FPS 1.58× |
| Moving complex cards | 25.23 / 18.59 | 49.14 / 49.43 | FPS 1.36× |
| Moving cards + translucent overlay | 11.11 / 7.57 | 49.86 / 50.09 | FPS 1.47× |
| Individually translucent cards | 22.88 / 15.81 | 48.92 / 49.74 | FPS 1.45× |
| 40 complex cards scrolling | 10.29 / 7.40 | 49.86 / 49.92 | FPS 1.39× |
| Mixed controls | 42.15 / 36.55 | 40.84 / 49.89 | FPS 1.15× |

</div>

</details>

## Memory and Flash

The fixed-decoration suites trade Flash for CPU: baked images add 2.65 MiB (S3) / 4.22 MiB (P4) of Flash, while other suites differ by no more than 0.73 MiB. GSP retains page contexts and LVGL creates the current page on demand, so free SRAM varies in both directions. ↓ means lower is better; ↑ means higher is better.

<div class="gsp-statistics">

| Complete application | Flash MiB ↓ | Static SRAM KiB ↓ | Minimum free SRAM KiB ↑ | Minimum free PSRAM MiB ↑ |
| --- | ---: | ---: | ---: | ---: |
| Fixed decoration · P4 | 5.32 / 1.10 | 131.81 / 128.71 | 222.91 / 207.78 | 23.20 / 27.44 |
| Fixed decoration · S3 | 3.58 / 0.93 | 113.13 / 112.99 | 133.21 / 105.02 | 10.20 / 12.87 |
| Images, lists and animation · P4 | 3.90 / 3.87 | 131.81 / 128.71 | 199.37 / 207.53 | 24.47 / 22.53 |
| Images, lists and animation · S3 | 3.74 / 3.70 | 113.13 / 112.99 | 103.13 / 104.68 | 9.90 / 7.96 |
| Single-GIF suite · P4 | 1.75 / 1.78 | 131.11 / 128.04 | 208.77 / 247.16 | 26.64 / 26.61 |
| Single-GIF suite · S3 | 1.60 / 1.62 | 113.25 / 113.13 | 109.42 / 143.23 | 12.06 / 12.04 |
| Single-page flat HMI · S3 | 2.13 / 1.48 | 113.13 / 112.99 | 138.36 / 106.44 | 11.66 / 10.54 |
| Transforms and controls · S3 | 2.21 / 1.48 | 101.88 / 101.73 | 110.78 / 130.19 | 9.88 / 12.23 |
| Drawing and composition · S3 | 0.99 / 0.97 | 101.88 / 101.73 | 132.59 / 82.09 | 12.59 / 12.58 |

</div>

## Scope

GSP gains in this set concentrate on fixed decoration, known assets and dense updates. Light workloads are close: GSP CPU is 7% higher for the single rectangle recolor and 3% higher for the moving card. Results include GSP build-time preprocessing; optional LVGL preconverted assets, image sequences and alternative decoders are outside this set, which does not represent LVGL's tuned limits.

## Test environment

<div class="gsp-statistics">

| Chip | CPU | PSRAM | Display |
| --- | ---: | ---: | ---: |
| ESP32-S3 | 240 MHz | 16 MiB | RGB · 800×480 · PCLK 18 MHz |
| ESP32-P4 | 360 MHz (single-GIF suite 400 MHz) | 32 MiB | MIPI-DSI · 1024×600 |

</div>

GSP 1.10.3 / GSPC 0.11.2 and LVGL 9.6.0, ESP-IDF `b6293eeb209`, RGB565. The main tables enable GSP's default acceleration, including SIMD and P4 PPA, while LVGL uses software rendering. LVGL PPA reference results are included in the comparison CSV.

<details>
<summary>Measurement definitions</summary>

- **FPS:** successful frame submissions per second, rather than optical screen refreshes. Single-animation and mixed-page counting scopes are specified in their sections.
- **CPU:** the Difference column gives relative changes calculated from unrounded values, rather than percentage-point differences in CPU use.
- **Repeated measurements:** two independent boots after warm-up, aggregated per boot before taking the median. CSV ranges are observations, not confidence intervals; ±5% is an FPS classification rule, not statistical equivalence.
- **Resources:** complete application costs. Static SRAM comes from the ELF; minimum free memory is the median of the minimum free heap from each boot. PSRAM includes resident code and dynamic allocations and must not be added directly to ELF payloads. KiB / MiB mean 1,024 / 1,048,576 bytes.

</details>
