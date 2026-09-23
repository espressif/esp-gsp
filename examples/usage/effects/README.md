# Interactive effect preview

Use **Next** to select charging, ring, carousel, pulse, SVG morph, shimmer, touch glow, flip cards or glass. Three style
buttons select variants; the underline indicates the current choice. The example
is manual by default. Charging **Stop / Charge** disables/enables motion and
particles while retaining the static halo and percentage. Ring, pulse and SVG
pages use **Pause / Play**. The carousel follows drag/tap selection and has no
pause button. Visibility changes are submitted as one batch.

Start with [the minimal integration example](minimal/README.md) for application code.
See [the Chinese guide](README_zh.md) for complete authoring examples and options.
Use the tool pairing in the [compatibility guide](../../../docs/en/reference/compatibility.md).
Procedural scenes use GSB 2.10;
flip-card and soft-shimmer scenes use GSB 2.11.


## Styles

- `charging_orb`: `style: liquid | halo | plasma` (default halo), `charging`,
  `value`, `fg_color`, `period_ms`, `strength`, `softness`, `ripple: 0..100`, and `particles: 0..6`.
  Ripple is an attached, gently folding membrane, not concentric expanding rings.
  Halo/Plasma show particles only while charging. Typography is an ordinary Label
  so applications can choose precision, units and layout. `set_charging()` and
  `set_playing()` share the same playback state. Visual options are authored by default; enable `runtime_style` for appearance setters.
  Value, charging/playing and phase have generated runtime setters.
- `effect: ring`: `ring_style: solid | glow | trail`; strength, softness and period
  control the glow, feather and motion. Ring contours do not ripple. Value controls activity-arc length.
- `effect: pulse`: soft radial coverage, adjustable strength/softness/period.
- `carousel`: `projection: flat | tilt | perspective`, depth and spacing 0..100,
  3..12 icon paths and a selected index. Flat/tilt use lower-cost sampling;
  perspective uses alpha-correct bilinear sampling. Paint and hits share geometry.
- Numeric properties expose `play_*()` / `stop_*()` where supported. Animation
  configuration supplies duration, delay, iterations (0 repeats) and alternate
  playback. Hidden command-backed targets pause. Carousel settling uses one slot
  in the existing animation pool; capacity exhaustion falls back to direct selection.

GSPC bakes icon thumbnails. The runtime uses fixed-capacity state,
transactions, dirty regions and native A8/span kernels. There is no per-frame
blur, full-size temporary effect image or device-side SVG parser.

## Hardware

Export ESP-IDF and run from this example directory. Build sequentially because
`managed_components` is shared. Use separate build and sdkconfig paths.

| Board | Target | Extra defaults | RGB888 |
|---|---|---|---|
| S3 SPI | esp32s3 | sdkconfig.defaults.esp32s3_spi | OFF |
| C3 SPI | esp32c3 | none | OFF |
| S3 QSPI | esp32s3 | none | OFF |
| P4 MIPI-DSI | esp32p4 | none | OFF |
| S31 RGB888 | esp32s31 | sdkconfig.defaults.esp32s31_rgb888 | ON |
| S31 RGB565 | esp32s31 | none | OFF |

```sh
idf.py -B build_s3_spi -D SDKCONFIG=build_s3_spi/sdkconfig \
  -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.esp32s3_spi' \
  set-target esp32s3 build
idf.py -B build_s3_spi -p PORT flash monitor
```

S31 uses `idf.py --preview`. RGB888 requires `-D GSP_EFFECTS_RGB888=ON`.
This example uses a 3 MiB application partition.
`-D GSP_EFFECTS_AUTOPLAY=ON` changes style every two seconds, switches pages
every ten seconds, and cycles carousel selection. The default is OFF for manual
previews. C3 SPI and S31 RGB888 have no touch in the shared board layer.

Samples include frames, wall-time FPS, average frame-production busy time,
raster/submit time, render errors and internal heap. An idle carousel stops
continuous redraw; its average FPS includes idle time. Measure active motion
over a time window in which the carousel is animating.

## Bounds

By default, appearance is authored at build time; opt into `runtime_style` for
appearance setters and `image_sets` for theme switching. Generated runtime
setters also control state such as charge level, playback,
selection and the flipped face. Preconfigure variants and switch visibility to
change complete styles, as this preview does.

Use your own PNG, JPEG or supported self-contained SVG assets in carousel `icons`,
flip-card `front` / `back`, or the glass backdrop Image's `image` field. Paths are
relative to the scene JSON. The compiler resizes the images automatically.
Carousel and flip-card faces and icons use images bundled at build time.
Declare runtime-selectable image variants with `image_sets`.
Charging orbs use appearance parameters rather than an image-skin interface.

Orbs, rings, pulses and sweeps are rendered procedurally at runtime. Carousels and
flip cards bake static textures and compute their 2.5D projections at runtime;
they do not store animation frame sequences. Glass bakes the complete material.

Effects are at most 512x512; charging/ring/pulse bounds are square. Carousels need
at least 32x32 and bake icons with side 8..128. Source assets are at most 2 MiB.
All output is clipped to authored bounds. Declare effects as scene widgets.
Perspective applies to carousel icons and card faces; glass blur is baked at
build time, and particle counts are set by the effect parameters.
RGB565 glow uses stable spatial dithering but remains subject to panel color depth.

## Additional presets

- Shimmer: `shimmer_style: linear | soft | diagonal`; new styles feather the highlight in both axes.
- Touch glow: a Button click triggers a one-shot Pulse value animation (100 to 0). Native button feedback remains active while held.
- Flip card: `front`, `back`, `flipped`, `depth` and `period_ms`; tap to flip by default; generated `set_flipped()` drives an interruptible transition. Hidden cards settle immediately. Faces are baked to at most 256 pixels per side. Start with a 160..220 pixel card width on constrained devices.
- Glass: `backdrop` names an earlier opaque static Image with the same parent. `blur`, `tint_color`, `tint_opacity` and `radius` are baked into a cropped native bitmap. Backdrops must fit within 2048×2048; glass within 512×512. This does not blur other widgets or track runtime backdrop changes. Use preconfigured variants for themes.

The preview has nine pages; autoplay completes a cycle in 90 seconds. Halo animation stops when both ripple and particles are disabled.

## Runtime appearance and compiled image groups

Set `runtime_style: true` on a named effect, orb, carousel or flip card to generate
appearance getters/setters. Unconfigured widgets allocate no extra style state.
Orbs and soft lights expose color/opacity/period and applicable strength/softness;
Halo/Plasma also expose ripple and particle count. Liquid, solid rings and linear
sweeps expose color/opacity/period. Carousels expose depth/spacing/opacity and
settling duration; flip cards expose depth/opacity and flip duration. Duration
changes apply to subsequent card/carousel transitions. Complete effect styles
and static backgrounds remain build-time choices. Generated `set_<property>_rgb888()`
helpers accept `0xRRGGBB` across scene formats; use `set_fg_color_rgb888()` for an
effect's foreground color. Existing setters/getters retain native-format values.
Set opacity separately; the RGB888 helpers reject an alpha byte.

Add `image_sets: [["front_dark.png", "back_dark.png"]]` to a flip card, or arrays
with the same number of paths as `icons` to a carousel. Up to three additional
groups are allowed; group 0 is the original images. `image_set` selects the initial
group and generated `set_image_set()` switches it at runtime, preserving selection
and flip progress. This does not require `runtime_style`. Groups use matching
texture sizes and duplicate assets are interned. Extra textures consume bundle
space and resource descriptors, but switching does not decode images or allocate
animation frames. `image_sets` selects images packaged at build time.

A Button can trigger feedback without an application callback:
`events: [{"event":"click", "action":"play_glow", "target_name":"feedback", "arg":850}]`.
The named target must be a Pulse with `playing:false, value:0`. Duration defaults
to 850 ms and accepts 100..60000 ms. Repeated clicks restart the fade; hiding the
target clears it and frees its animation slot. Disabled buttons do not trigger it.
The preview's pulse, touch-glow and flip pages each use one retained instance.

These controls use GSB 2.12; the compatibility guide above lists the matching tools.
