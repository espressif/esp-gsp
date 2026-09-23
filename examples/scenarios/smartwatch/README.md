# Field watch / 360×360

![Watch face](preview.png)

An outdoor watch for the ESP32-S3 QSPI touch profile, RGB565, octal PSRAM and
rotation 0. Six full-screen PageFlow pages combine a retained vector dial and
seconds marker, activity rings, notifications, a procedural charging halo,
a perspective landscape carousel and an interactive breathing glow.

![Native simulator drag preview](preview_swipe.gif)

![Route gallery](preview_gallery.png)
![Adjustable breathing effect](preview_breathe.png)

## Controls

- Swipe horizontally between the dial, activity, notifications, charging,
  adventures and breathing. The sequence wraps in both directions.
- **View trail** opens a Drawer with the ridge photograph and walk summary;
  **Done** closes it.
- **Read message** opens the contact invitation. **On my way** records a local
  reply and returns to the inbox.
- Tap the dial battery pill to open charging. **Undock** returns to the dial.
  The halo stops at 100% or when leaving the charging page.
- Drag the landscape carousel to select a ridge, coast or forest route. Swipe
  below the carousel to change pages without changing the selected route.
  **View route** opens its illustration, distance and activity estimate;
  **Done** returns to the selected route.
- Drag the breathing slider to adjust glow intensity. **Pause / Resume**
  controls the pulse; leaving the page stops it without discarding the setting.

Time starts at 10:08. Activity, heart rate, weather and charging are simulated;
charging advances one percentage point per three seconds. UI events and timers
use `main/watch_ui.c`; page observation runs at 50 ms, separately from the
one-second model timer.

## Build

From this directory after exporting ESP-IDF:

```sh
idf.py set-target esp32s3 build
```


## PC preview

Install the matching `esp-gsp-tools` package, then run from the component root:

```sh
python3 tools/sim_bridge/run.py --project examples/scenarios/smartwatch/pc
```

The PC backend shares the application controller with the device.

## On-device capture timing

Build with `idf.py -D WATCH_DEMO_PROBE=ON build` to measure two complete page cycles
on the actual display. The `watch_probe` log reports the synchronous viewport
capture time, result, snapshot counts, decoded-cache usage and free heaps.
It also reports each preceding page interval's frame count, elapsed time,
average renderer busy time and cumulative render errors. Intervals include
page transitions; these are workload samples, not isolated effect benchmarks.
Keep the scene, compiler, optimization level and board configuration identical
when comparing builds. These timings cover page capture, not physical touch
latency or end-to-end frame rate. The probe returns to the dial and stops.
Build with `-D WATCH_DEMO_PROBE=OFF` for normal manual interaction.


Artwork sources are listed in [assets](scenes/assets/README.md).
The three compact landscape illustrations in `scenes/artwork/` are original
SVG source assets, compiled into the carousel's image set.
