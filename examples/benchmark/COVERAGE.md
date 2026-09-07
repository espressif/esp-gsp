# Benchmark coverage and acceptance

The canonical manifest contains 49 cases: 39 authored pages and 10 transition
or gesture workloads. Every case repeats during its dwell window. A full run
warms one lap, measures the next, displays results outside the measurement,
and continues. `P_*` names below are stable case IDs, not performance scores.

## Controls and exercised behavior

`test_scene_coverage.py` compares every resolution against GSPC's widget
inventory (currently 29 types). This proves authored presence; the driver and
acceptance columns describe the additional runtime coverage.

| Authored types | Cases | Repeated behavior / acceptance |
| --- | --- | --- |
| `rect`, `container`, `shape` | `P_RECT1`, `P_RECTS`, `P_CARDS`, `P_SHAPES`, `P_GRAD`, `P_LOAD1/8/32/64` | Value/color/theme changes, rounded borders and vector shapes; load tiers have identical prefix geometry and alpha, positive rendered frames required |
| `layer` | `P_OPA`, `P_OPAL`, `P_OVER`, `P_STORM` and page ownership | Translucent fills, nested layers, scrim, moving mixed content; repaint requests and actual region output recorded |
| `label` | `P_TEXT`, `P_BIGTEXT`, `P_SCROLL` | Text replacement, wrapping, glyph reuse and color blending; full-screen text intentionally clips at its viewport |
| `image` | `P_IMGRGB`, `P_IMGARGB`, `P_QOI`, `P_SCALE`, `P_ROTATE`, `P_WALL` | Opaque/alpha images, dynamic QOI publication, scaling and arbitrary/cardinal rotation; command failures rejected, rotation requires two commands per update |
| `arc`, `needle`, `clock` | `P_ARCS`, `P_CLOCK` | Six arc value ramps, clock and independent needle updates; API error counters and rendered frames |
| `spinner`, `progress`, `slider` | `P_MOTION`, `P_RECTS`, `P_WIDGETS`, `P_DRAWER` | Continuous animation/value ramps and real slider input in the drawer |
| `button`, `toggle`, `checkbox`, `radio` | `P_WIDGETS`, `P_DRAWER` | Synthetic press/release, boolean value changes and drawer actions; this is not exhaustive hit-target or accessibility qualification |
| `list`, `wheel` | `P_WIDGETS`, `P_WHEEL` | Bound rows, repeated absolute scrolling, three momentum rollers, snapping/fade; lists are emptied on exit and restored on re-entry |
| `grid` | `P_GRID` | 24-cell industrial gallery, cell binding and repeated synthetic drags; positive binds/drags and zero errors required |
| `message_list` | `P_MESSAGES` | Variable-height incoming/outgoing bubbles, scrolling, prepend/append and anchor updates; immutable samples use trusted revisions (logged in the fingerprint); API failures rejected |
| `page_flow`, `stackview` | `P_FLOW`, `P_STACK` | Cyclic page selection, animated stack push/pop; failures recorded in component counters |
| `drawer` | `P_DRAWER` | Repeated edge drag, open-state check, slider/toggle/button interaction, close-state check; positive equal open/close counts and zero errors required |
| `dropdown` | `P_DROPDOWN`, `P_COMPOSITES` | Dedicated menu open/select/read-back loop; verified selections must be positive with zero errors |
| `tabview`, `table`, `keyboard`, `msgbox` | `P_COMPOSITES` | Three 12-second tabs: data table, bounded typing/deletion, repeated modal show/hide; keyboard final text must match the expected string and exceed 63 bytes |
| `chart` | `P_WIDGETS` | Static point geometry participates in continuous composition; live data-series replacement is not separately benchmarked |

## Rendering and runtime mechanisms

| Mechanism | Workload and evidence |
| --- | --- |
| Native damage vs forced full repaint | Separate build modes; per-case requests, region plans and output pixels. Do not combine the two scores |
| Template instances vs authored objects | `P_MOVE` creates/destroys and moves instances; `P_STATIC_MOVE` moves retained objects |
| Canvas callback and dirty publication | `P_STREAM` draws into borrowed destination rows and invalidates the old/new moving band; logs publication count and dirty-area ratio. The small no-PSRAM profile intentionally uses a smaller surface |
| GIF / animation patches | Four infinite-loop radar players in `P_ANIM`; two distant changing 8×8 regions in `P_ANIM_SPARSE`. Native mode exposes partial-update behavior |
| Lossless / lossy image paths | Compiled assets plus dynamic QOI; decoder and scaling backend counters identify actual routes. A PNG source filename does not prove runtime PNG decoding |
| Cache-free dynamic images | `P_QOI` exercises same-size QOI publication. `P_SCALE` scales four compiled RAW images because encoded region decoding cannot scale. Cache-enabled profiles additionally scale a dynamically published 80×60 QOI. The fingerprint distinguishes these paths |
| JPEG acceleration / fallback | Board-dependent eligibility and logged hardware/software/fallback counters; neither chip name nor visual appearance proves hardware execution |
| Programmatic scene transitions | Four slide directions, cross-fade and fade-through-black; completion, frame count and selected path are recorded |
| Synthetic gestures | Commit, cancel, flick and slide-fade; finish the in-flight semantic operation before closing the measurement window |
| Saturation | `P_STORM` attempts nine updates per callback; every ordinary page also requests repaint in full mode. Accepted updates are not rendered frames |
| Capacity scaling | Fixed-alpha, fixed-size rectangle tiers 1/8/32/64; geometry and count are validated. Includes shared scene overhead, not a pure blend kernel |
| Result isolation | Physical touch is disabled by default. Real touch in diagnostic mode invalidates the log. Result cards and their display time are outside scored windows |

## Visual review

Run the native GSP renderer, not an HTML recreation:

```sh
python3 tools/run_sim_benchmark.py --all --gallery --page-frames 90
python3 tools/run_sim_benchmark.py --all --case RESULTS_OVERLAY --frames 3
```

The gallery saves one PPM per authored page and resolution. It supplies grid
and message fixtures and opens the drawer so their layout is inspectable.
The results preview uses conspicuously labeled synthetic layout values,
including long names and large numbers: **never publish these as scores**.
The simulator does not run the complete board scheduler, media producer or
keyboard correctness test. Inspect additional interactive states as needed.

Validate each intended target and display configuration independently,
including its resolution, pixel format, rotation and transfer interface.
Simulator geometry, serial stability and physical panel correctness are
different gates. Photographs or direct observation are needed to accept colors,
orientation, tearing and perceived motion. Device-port mappings belong to the
local test environment, not the benchmark's supported configuration contract.

## Boundaries and fair comparison

- The suite is broad feature/performance coverage, not proof of every API,
  control state, format/backend combination or application integration.
- Warm-up intentionally populates caches. Cached JPEG composition is not a
  repeated cold JPEG-decode benchmark; read actual decoder counters before
  attributing a page's score to a codec or accelerator.
- Canvas external-buffer push/lease ownership, filesystem/network image
  sources, dynamic font loading and deployable metadata updates are not
  separately scored by this default suite. Use their dedicated tests.
- There is no CPU-utilization, power or optical-refresh measurement. Frame
  throughput and renderer busy time must not be relabeled as those metrics.
- Transition p50/p95 use the latest at most 16 completions; they are not
  whole-run frame-time percentiles. Heap sampling is not an exact peak trace.
- No performance superiority over LVGL or Embedded Wizard follows from this
  suite's scores. A numerical comparison requires equivalent content,
  resolution, panel, buffering, accelerator policy, instrumentation and
  software revisions. Visual quality also requires side-by-side human review.

The organization borrows isolated repeatable scenes and summaries from
[LVGL's benchmark](https://github.com/lvgl/lvgl/blob/master/demos/benchmark/lv_demo_benchmark.c)
and graduated composition load from Embedded Wizard's public
[graphics-performance example](https://ask.embedded-wizard.de/3330/graphics-performance-cpu-arm-neon-opengl-es-2-0).
The workloads and artwork here are GSP-specific, not copies of their scores
or proprietary assets.
