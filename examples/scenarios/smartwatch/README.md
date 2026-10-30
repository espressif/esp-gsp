# Field watch / 360×360

![Current native simulator screen](showcase/home.png)

An outdoor watch for the ESP32-S3 QSPI touch profile, RGB565, octal PSRAM and
rotation 0. A 72-pixel clock leads the English type scale;
clock, heart-rate and battery bindings declare their numeric glyphs. Six full-screen PageFlow pages combine a retained vector dial and
seconds marker, a step-goal summary, notifications, a battery progress ring,
a route selector and a one-minute guided breathing session.

![Native route selector](showcase/routes.png)

The route selector gives the map, distance and estimated walking time the
main visual area. Ridge, Coast and Forest use retained vector maps shared
with their detail views; selecting a route updates the selected map and summary.

## Controls

- Swipe horizontally between the dial, activity, notifications, charging,
  routes and breathing. The sequence wraps in both directions.
- **View trail** opens a Drawer with the ridge map and walk summary;
  **Done** closes it.
- **Read message** opens the contact invitation. **On my way** records a local
  reply and returns to the inbox.
- Tap the dial battery pill to open charging. **Undock** returns to the dial.
  The ring follows battery percentage without an ambient glow.
- Tap **Ridge**, **Coast** or **Forest** to select a route. The map, distance,
  estimated time and selected tab update together. **View route** opens the
  same map with the walk summary; **Done** returns to the selected route.
  Horizontal swipes navigate the full-screen pages, including over the map.
- On **One quiet minute**, **Start** begins six 4-second inhale / 6-second
  exhale cycles. The thin ring follows the current phase and a countdown gives
  its remaining seconds. **Pause / Resume** preserves progress. Leaving the
  page pauses the session; completion remains visible until **Again** is selected.

Time starts at 10:08. Activity, heart rate, weather and charging are simulated;
charging advances one percentage point per three seconds. UI events and timers
use `main/watch_ui.c`; page observation runs at 50 ms, separately from the
one-second model timer. Device and public PC backends use a monotonic clock
for the breathing session, so delayed callbacks catch up. Native scene checks
use deterministic scheduler time; the controller test injects explicit delays.

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


Artwork sources are listed in [assets](scenes/assets/README.md) and
[route artwork](scenes/artwork/README.md).
The Activity page uses one dominant step count, a goal bar and separate active /
standing summaries. Notifications use a sender monogram and a high-contrast
message card; the reply view reuses retained contour/route artwork. All UI text
is English, with baked Ubuntu Regular/Light subsets. The map in
`scenes/artwork/ridge-route.svg` is illustrative demo artwork, not navigation data.
