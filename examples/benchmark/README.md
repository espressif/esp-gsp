# ESP-GSP Hardware Benchmark

This example exercises the renderer and interaction surface on real
display targets. It covers controls, components, text, image fit and runtime
scaling and rotation, animations, Canvas, fixed and variable-height scrolling, elastic
message bubbles, drawers, navigation, transitions, and a dense mixed-content
render-saturation scene under continuous updates.

All 58 cases loop their workload throughout their dwell window. The first lap
warms caches; every following lap prints a fresh measured summary and starts
again automatically after an on-screen results carousel. No touch input is
required or enabled by default. Use a 660-second capture initially
and extend it if the final summary has not arrived. Slow boards may need extra
time to finish an in-flight drawer cycle or transition.

## Quick start

1. In `examples/benchmark`, select your board's [build profile](#build) below
   and run its build command in an exported ESP-IDF environment.
2. Flash and monitor the same build directory, for example
   `idf.py -B build_esp32p4 -p PORT flash monitor`. Replace `PORT` with the
   confirmed device port. The demo starts automatically; no interaction is needed.
3. Wait for `bench: warm-up lap done, measuring`, then
   `bench: measurement end`. Read the on-screen result cards or retain the
   complete serial log. FPS means submitted engine frames, not measured panel refresh.
4. For repeatable qualification, use [matrix capture](#capture-a-board-matrix)
   and [log validation/comparison](#compare-logs). Use [soak mode](#saturation-soak)
   to repeat one case longer, or [simulator previews](#automated-simulator-run)
   to inspect layouts without hardware.

## Coverage and pressure modes

| Family | Workloads |
| --- | --- |
| Primitives and blending | Rectangles, rounded containers, shapes, gradients, six arcs, translucent fills/layers and overlays |
| Text and scrolling | Labels, full-screen text, scrolling rows, grid album, variable-height message bubbles, three momentum wheels |
| Media | Nine-image RGB/alpha/QOI grids, runtime scaling, arbitrary/cardinal rotation, wallpaper, four GIF players and sparse animation patches |
| Controls and navigation | Clock needles, widgets, page flow, stack view, tab/table/keyboard/modal composites, repeated drawer open/close |
| Motion and composition | Static and template-instance movement, Canvas/GRAM-TE workload, mixed industrial render storm |
| Capacity ladder | 1 / 8 / 32 / 64 overlapping translucent rectangles, fixed size and alpha, deterministic placement |
| Verified selection | Dedicated dropdown open, select, read-back and repeat loop |
| Transitions and gestures | Four slide directions, cross-fade, fade-through-black, drag commit/cancel/flick/fade |

All 34 authored widget types in GSPC's registry are present at every supported
resolution. The [coverage matrix](COVERAGE.md) lists the drivers and checks
for each case. The stable case table is [`main/bench_cases.inc`](main/bench_cases.inc).

The visual language combines native navy/cyan/amber geometry, generated
industrial artwork, transparent machinery overlays and a looping radar
animation. Case headers identify the workload. Results show three cards on
small displays, six in two columns on wide displays, or six rows on the tall
480×800 display. Each page lasts three seconds; wall FPS is prominent and
render/submit milliseconds have separate lines. Results
display time is excluded from the next measurement. There is no live HUD
redrawing inside a measured case.

The default `GSP_BENCH_FULL_REPAINT=ON` stresses **every ordinary page** by
requesting a subtle full-screen background color change every 1 ms, alongside
its own workload. Transitions instead repeat their real transition/gesture
pipeline. The timer period is a requested service cadence, not a promised
update rate or a frame-rate limit: slow rendering delays callbacks, and the
engine may coalesce updates. The pressure and region lines report what
actually happened.

Use `-D GSP_BENCH_FULL_REPAINT=OFF` for a separate native-damage run. It keeps
the same scenes and their normal workload drivers but removes the forced
background repaint, allowing dirty-region and sparse-animation behavior
to be observed. Benchmark timers remain active: this is not an idle-power
test. Full-repaint runs measure sustained composition pressure;
native runs measure the authored update patterns. Do not combine their scores
or describe full repaint as evidence of dirty-region efficiency.

## Native C UI backend

`pc/` directly compiles the already-separated `main/bench_workload.c` with
[sim_bridge](../../tools/sim_bridge/README.md). No copy or rewrite of that
business source is needed. From an application's root with ESP-GSP installed
under `managed_components`:

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project managed_components/espressif__esp-gsp/examples/benchmark/pc
```

The runner automatically selects the component's GSPC and simulator versions.
Use `GSPC_EXECUTABLE` / `GSP_SIM_EXECUTABLE` for executable overrides.
The PC adapter selects the RGB565 profile and drives the shared tween timer.
This is a UI logic reuse example, not the full benchmark harness: board
setup, automatic page cycling, Canvas/media producers, dynamic list
fixtures and performance measurements are not ported. Backend mode disables
sim_host's automatic deployable list binders. Use the hardware workflow
below for benchmark results.

## Metrics

- `wall throughput`: engine frames divided by measured case wall time; not an
  optical measurement of panel refresh rate.
- `active throughput`: frames divided by render-path busy time; excludes UI
  service and idle time, so it is neither CPU utilization nor achieved FPS.
- `rndr ms`: average rasterization time per frame.
- `subm ms`: average cache synchronization and panel submission time per frame.
- `svc us`: average queued-command/input service time per render-task iteration;
  excludes application timer callbacks and is not total UI processing time.
- `qcmd/s`: commands drained from the queue per measured second. Calls made
  inline on the render task are not counted. The raw log field remains
  `commands` for compatibility; this is not total applied update rate.

The log also records exact frame and microsecond counters, transition paths,
Canvas publication, media decode backends, and a target configuration
fingerprint, including the embedded bundle CRC. A valid run must complete the
measured lap through `bench: measurement end` without panic, watchdog,
assertion, display underrun, unexpected scene changes or physical-input logs.

Aggregates divide summed frame counts by summed measured times; they are not
averages of page FPS. Per-page counters are captured before printing. Media
and Canvas deltas reset for each measured lap; explicitly labeled lifetime
peaks remain lifetime values. Sampled heap deltas are not exact allocation
high-water marks. Console output, warm-up and inter-page setup are not part of
the summed page duration.

Each `pressure[...]` line separates callback `ticks`, update `requests`,
`accepted`, queue-timeout `rejected`, and unexpected `errors`. Accepted requests
are not rendered frames or necessarily queued commands. These callbacks run
on the render task and normally dispatch inline; this is render saturation,
not a cross-task queue-admission stress test. `regions[...]` records the normal
region planner's output pixels and full-region promotion counts, not panel-bus
traffic. Snapshot composition can submit frames without running that planner;
use frame/transition counters for those paths. A refused command must never
inflate throughput.

The `render storm` page is the saturation workload. Every 4 ms it attempts
nine updates: four translated alpha-image composites, four meters, and one
shared color update. Its result reports attempted `commands` separately from
queue-timeout `rejected` commands and unexpected `errors` (which invalidate
the run). Rejection counts must not inflate throughput; use the page's
measured frame, render and submit counters when comparing targets.

The capacity ladder changes only the number of alpha-128 rectangles. Object
size and the first N positions remain identical across tiers at one resolution;
the `capacity[...]` line records count, box size and alpha. All tiers include
the same header/background overhead, so these are composition workloads, not
isolated blend-kernel timings. Geometry scales with the logical resolution.

The dropdown case repeatedly opens the menu, selects the next of three output
pipeline labels, and checks the selected index. These labels are demo choices;
they do not reconfigure the display hardware. A valid result requires positive
verified selections and zero errors.

Protocol 25 drives the messages page from the 1 ms pressure callback, advancing
once per completed frame. Each step moves one quarter of the logical screen
height within a one-screen-height range and reverses at either endpoint without
pausing. Each lap restores 32 messages and the same starting offset. This tests
scrolling and row recycling at the achieved frame rate without a fixed velocity
or a periodic fling delay. The case runs as part of the full benchmark suite.
Four history messages are prepended at 750 ms and one message is appended at
2250 ms. The trailer reports successful upward/downward scroll commands in `up`
and `down`, plus `prepends` and `appends`. Protocol 25 checks that scroll commands
keep pace with rendered frames. Compare results within the same protocol and
display configuration.

The composites page gives each tab a twelve-second window and drives the
keyboard at a 4 ms event cadence with a 255-byte edit buffer. Protocol 16 and
later require the final verified text to exceed the 63-byte command inline
threshold. The longer window ensures that even the slowest supported target
can reach that boundary; the parser rejects runs that do not. During long
soaks the keyboard alternates typing and deletion within its bounded buffer,
so it keeps doing useful work without overflowing it.

The `image rotation` page updates two 96 x 64 opaque images every 16 ms. One
uses a continuously changing arbitrary angle; the other cycles through
0/90/180/-90 degrees to cover the cardinal acceleration route. The workload
result must report two successful commands per update. Its page row therefore
represents the combined end-to-end cost of one arbitrary and one cardinal
rotation, not an isolated kernel microbenchmark.

The image-scale case changes display scale on four images. With a decoded-image
cache it also publishes an 80×60 dynamic QOI; without one all four retain
their compiled RAW sources, because encoded region decoding does not support
runtime scaling. Cache-free dynamic QOI publication is exercised separately by
`P_QOI`. The startup fingerprint records `image_cache` and `scale_source`;
unsupported requests or failed publication are not successful workload coverage.

The `anim sparse` page keeps an orientation-readable grid fixed while two
distant 8 x 8 markers alternate color every 50 ms. It exercises animation
multi-patch compilation and persistent-frame composition; background changes,
trails, or missing markers indicate a visual failure.
Use native-damage mode when evaluating its partial-update efficiency; default
full-repaint mode adds full-screen composition pressure.

Cross-fade and fade-through-black are visually successful only when both
`no_visual` and `path_failures` are zero. When snapshot memory is unavailable,
cross-fade safely degrades to a direct, zero-snapshot fade-through-black;
`direct` records that route. A direct switch with `no_visual=1` completes
navigation safely but is not a rendered transition result.
Transition latency mean and maximum cover all completed transitions in that
page window. `p50`/`p95` use the latest at most 16 completions, explicitly
identified by `latency_window=latest samples=N`; they are not whole-soak
percentiles when more than 16 transitions complete.

## Build

Export the matching ESP-IDF environment and build with the appropriate
configuration fragment. Separate build directories prevent retained target
settings from affecting another board.
Run target builds sequentially: ESP-IDF's component manager shares the
example's `managed_components` directory even with separate build directories.

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

### Saturation soak

The default `full` run remains the reproducible all-case benchmark. To keep one
case under sustained load, add its stable ID and a per-cycle measurement
window to the normal build command:

```sh
-D GSP_BENCH_SOAK_CASE=P_STORM \
-D GSP_BENCH_SOAK_DWELL_MS=60000
```

The first 60-second cycle warms caches. Every following cycle prints a fresh
measured summary, displays its result, and continues. Other useful IDs include
`P_ROTATE`, `P_IMGRGB`, `P_IMGARGB`, `P_QOI`, `P_COMPOSITES`, `P_GRID`, and
`P_DRAWER`. The canonical ID, category, display name, and default dwell table
is [`main/bench_cases.inc`](main/bench_cases.inc); an unknown ID fails at
startup instead of silently running the wrong workload.
Set `GSP_BENCH_SOAK_CASE` back to an empty string to restore all-case playback.
Use at least 45000 ms on C3, or 36000 ms on other targets, for `P_COMPOSITES`
to exercise all three tabs; the default
60000 ms soak window covers them and repeats the cycle. Drawer and transition
windows finish their in-flight operation before reporting.

### Manual input diagnostics

`GSP_BENCH_TOUCH_INPUT=OFF` isolates automatic navigation from real fingers.
Use `-D GSP_BENCH_TOUCH_INPUT=ON` only for diagnostics; the first physical
pointer event invalidates the capture. Synthetic dropdown, drawer, keyboard
and gesture workloads still run with physical input disabled. This option
does not change the framework's swipe thresholds. Use an interactive example
to assess manual drag feel without the benchmark scheduler changing scenes.

## Compare logs

Use the same target, panel configuration, scene bundle, and ESP-IDF revision
for both runs:

```sh
python3 tools/compare_logs.py \
  --pair P4-MIPI baseline-p4.log current-p4.log \
  --pair S3-QSPI baseline-s3.log current-s3.log \
  --details
```

The parser validates raw-counter arithmetic, workload checks and per-case
pressure evidence, and rejects incompatible or fatal logs before reporting
differences. Protocol 19 changes scene content and measurement isolation: do not
compare its scores against older protocols as a renderer improvement. Keep
pressure mode, soak selection/window and the entire configuration fingerprint
identical for A/B testing. Hardware acceleration and fallback routes must be
read from the log, not inferred from a chip name.

## Assets

### Vector Cases

Protocol 22 includes eight SVG cases in the full run. Protocol 20 introduced
resize/rotation/tint; protocol 21 added morphing.

| Case | Workload |
| --- | --- |
| `P_VECTOR_SIZE` | Color and monochrome images resize in opposite phases from 32 to 96 px; four setters per update |
| `P_VECTOR_ROTATE` | Color and monochrome 96 px images rotate in opposite directions; two setters per update |
| `P_VECTOR_TINT` | Two fixed 96 px silhouettes change color; two setters per update |
| `P_VECTOR_MORPH` | Two selected emblem groups interpolate matching curves in opposite phases; color and A8 paths, two setters per update |
| `P_VECTOR_MOVE` | Opposite integer translations of fixed-size color/A8 images; two setters per update |
| `P_VECTOR_FIT` | `contain`, `cover`, `stretch` in non-square boxes; Q16.16 zoom from 0.75x to 1.5x, three setters per update |
| `P_VECTOR_STYLE` | Decorative paths with even-odd holes, per-paint alpha, cubic/quadratic curves, expanded strokes and local `use` transforms; silhouette visibility toggles, one setter per update |
| `P_VECTOR_EYES` | Layered iris gaze and actual eyelid curve morphing; 100 ms controller, commands only on target changes (36 per 32 updates), 8-second dwell |

The device frame timer and simulator share the vector update function. Device
logs include `bench: vector[...] updates=... commands=... errors=...`; the parser
requires positive updates, exact command counts and zero errors. Protocol 18/19
logs retain their original case coverage; protocol 20 retains 52 cases and
protocol 21 retains 53 cases.
Do not compare aggregate scores across these protocols: the measured workload has changed.

From the repository root, preview a vector case with GSPC 0.4.1 and simulator 1.3.1:

```sh
export GSPC_EXECUTABLE="$PWD/ci/gspc-dev"
python3 examples/benchmark/tools/run_sim_benchmark.py \
    --size 240 --case P_VECTOR_SIZE --frames 120
```

Use `P_VECTOR_ROTATE` or `P_VECTOR_TINT` for the other cases. For an isolated
device run, use the board build command above with
`-D ESP_GSP_BUILD_PREBUILT=ON -D GSP_BENCH_SOAK_CASE=P_VECTOR_TINT` and the same
source GSPC override. Published binaries predating this branch cannot render
the vector resources.

With the expanded scene, C3 uses 15 seconds per composite tab (45 seconds
total); other targets keep 12 seconds per tab. The former C3 window ended at
63 bytes after successful typing/deletion. The longer window preserves the
strict greater-than-63-byte coverage gate instead of weakening validation.
`keyboard_window_ms` in the configuration fingerprint records the difference.

The normal benchmark keeps all existing workloads and includes these eight
cases in its 58-case cycle. After an isolated run, explicitly clear the cached
selection when rebuilding the same board build directory:

```sh
idf.py -B <board-build-dir> -D GSP_BENCH_SOAK_CASE= build flash
```

Keep the board's existing target, port and profile options. Confirm the boot
log reports `bench: run mode=full cases=58`; a vector-only soak is not a full
benchmark validation.

Tint can reuse A8 coverage when the image cache is enabled and has budget.
Cache-disabled configurations exercise rerasterization instead. The RGB565
mask composition can use existing PIE/SIMD kernels, but simulator timing is
not evidence of hardware throughput. Compare `native` and `full` pressure as
separate measurements.

The eye case uses separately selected `white`, `iris`, `mask` and `rim` parts
from two compatible SVG states. Iris/highlights translate without deformation;
the eyelid opening changes curvature and closes completely. These are solid
background-colored occluding shapes, not SVG clip paths or masks. The iris
and eyelid animations use the shared property animation engine; their timing
does not depend on incrementing geometry once per rendered frame.
The protocol requires observed open and fully closed states, not just successful
animation calls. Eye size scales up with the panel but is capped at 192 pixels
per eye; do not treat cross-board scores as equal-pixel CPU comparisons.

Check actual benchmark frames for complete closure and two-axis pupil movement:

```sh
GSPC_EXECUTABLE="$PWD/ci/gspc-dev" \
python3 examples/benchmark/tools/check_vector_eye_frames.py \
    --build-dir /tmp/gsp-eye-check --output-dir /tmp/gsp-eye-frames
```

Add `--rgb888` to check the 800x480 RGB888 scene with the same pixel assertions.

This complements the hardware counters and the lower-level cache/tile, format,
template and boundary checks listed in [COVERAGE.md](COVERAGE.md). It does not
claim support for gradients, arbitrary topology changes or dynamic clipping.

### Generated Media

`scenes/gen_scenes.py --check` verifies the benchmark scenes and media assets.
The industrial saturation artwork is checked in, has no runtime network
dependency, and is validated for exact dimensions and alpha coverage.
See [asset provenance and generation prompts](scenes/ASSETS.md). Replacing
artwork changes the workload and invalidates comparisons to the old bundle.

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
python3 tools/run_sim_benchmark.py --all

# Save every authored page at every resolution for visual review
python3 tools/run_sim_benchmark.py --all --gallery --page-frames 90

# Small-screen results and keyboard layout (synthetic preview, not scores)
python3 tools/run_sim_benchmark.py --size 240 --case RESULTS_OVERLAY --frames 3
python3 tools/run_sim_benchmark.py --size 240 --case P_COMPOSITES --state keyboard
python3 tools/run_sim_benchmark.py --size 240 --case P_DROPDOWN --state dropdown

# Native RGB888 panel geometry
python3 tools/run_sim_benchmark.py --size 800 --rgb888 --gallery

# Inspect or repeatedly render only the saturation scene
python3 tools/run_sim_benchmark.py --case P_STORM --frames 600 --window
```

Add `--window` to watch the pages change in SDL while retaining automatic
frame-count and output validation. Each page is shown for 60 frames by
default; use `--page-frames 120` to keep each page on screen longer.

Each case runs headless, requires one committed frame per requested loop, and
saves its log and final PPM under `build/gsp-sim-benchmark/<width>/`.
The runner covers authored page layers, not the board application's complete
58-case scheduler or hardware-specific interaction checks. The host supplies
separate visual grid/message fixtures and an open drawer; those are not the
board application's pressure drivers. `--state keyboard` and `--state modal`
inspect composite states. `--case` hides all
other pages. Check small and portrait resolutions visually as well as checking
the host exit status.

## Capture a board matrix

`tools/capture_matrix.py` resets multiple boards together and captures their
serial output concurrently. Install `pyserial` in the active Python
environment, then provide one `--device LABEL PORT LOG` option per board:

```sh
python3 tools/capture_matrix.py \
  --seconds 660 \
  --device board-a PORT_A board-a.log \
  --device board-b PORT_B board-b.log
```

Replace `PORT_A` and `PORT_B` with confirmed serial devices in your environment;
labels are user-defined and any number of devices can be supplied. The example
does not associate serial port numbers with chips, display interfaces or build
profiles. Keep local device mappings outside project configuration.
Confirm ports before flashing or resetting and close other monitors first.
Keep the complete boot/configuration prefix and measured summary.
Capture refuses to overwrite an existing log and validates each result before
returning success. A timeout or incomplete summary is not a passing run; extend
the duration and capture to new paths. Preserve ESP-IDF revision, source commit,
build configuration and logs alongside any published result.

## Mixed effects regression

Protocol 23 adds `P_EFFECTS` (`effects mixed`) at every resolution: charging
orb, pulse with retained foreground/background opacity, flip card, carousel,
static baked glass, a cached SVG and alternating runtime QOI images. The
measured trailer requires nonzero updates, five accepted command submissions
per update, zero submission errors and successful asynchronous image
publication with no failures. The 32x32 QOI inputs also match the placeholder
on no-cache targets; global media checks still validate the complete cycle. Use `GSP_BENCH_SOAK_CASE=P_EFFECTS` for focused
stress. The host opacity oracle compares static and runtime pixels and checks
incremental damage in RGB565/RGB888. Inspect the target display for effect
appearance, clipping and panel output.
