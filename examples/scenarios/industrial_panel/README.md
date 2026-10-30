# Industrial process panel / 1024×600

![Current native simulator screen](showcase/home.png)


A P4 MIPI-DSI touch panel using RGB565, PSRAM and rotation 0. Four full-screen
PageFlow pages present production, the pneumatic circuit, maintenance and a
read-only pressure Trend inspector.
An expanded production image and a bounded baked-glass caption accompany
the primary pressure readout. Six type sizes and explicit numeric glyph sets
keep telemetry legible without embedding the body alphabet at large sizes. The retained
vector circuit and connection status remain still while live telemetry changes.

## Controls

- Pull down from the top edge on any of the four pages to open **Process alerts**.
  The panel shows current simulated pressure, target and alarm status.
  **Acknowledge alert** shares the Maintenance acknowledgement state; it is
  enabled only for an unacknowledged active alarm. **Close** or an upward swipe
  dismisses the panel. A short pull cancels. The pressure settings and alerts
  panels are mutually exclusive, and reviewing alerts never changes the target.
- Swipe between Overview, Live process, Maintenance and Trend inspector, or use
  the footer tabs.
- **Trend inspector** opens an expanded 30-second pressure history. Drag the
  scrubber to move the selected point; the vertical cue, marker and sample
  readout follow that retained sample. **Return to live process** leaves the
  live target, draft and alarm acknowledgement state untouched.
- **Adjust pressure target** opens a Drawer with a 4.0–8.0 bar draft value.
  Drag the slider for coarse adjustment or use −/+ for 0.1 bar steps. The
  preview warns when the draft exceeds the 7.2 bar alarm threshold.
  **Apply target** saves it; **Cancel** or vertical dismissal discards the draft.
- Pressure follows the target with a small simulated disturbance. Above 7.2 bar,
  **View alerts** opens the active alert. **Acknowledge** records its review.
  The fault clears below 7.0 bar; a new rising fault needs acknowledgement again.

Telemetry and alerts are simulated and reset on restart. The chart retains
30 one-second samples and starts at 6.2 bar. Page observation uses a separate
50 ms timer.

Opaque drawers use a shared pair of RGB565 viewport snapshots (about 2.34 MiB
at 1024×600), retained while their content and other transforms remain unchanged.
Telemetry and history continue during drawer motion; routine visual updates
coalesce until the drag or settle ends. Alarm transitions still publish immediately.
Guarded handoff also retains the already presented endpoint after a short
cancelled pull, avoiding a redundant full repaint. Changed content and explicit
full repaint requests still use the live renderer.
The fixed Overview illustration is compiled at its displayed 450×300 size,
avoiding repeated runtime resampling while keeping its centered placement.

## Build

From this directory after exporting ESP-IDF:

```sh
idf.py -B build_p4 -D SDKCONFIG=build_p4/sdkconfig set-target esp32p4 build
```

For P4 revision 1.x, use a separate build and its revision preset:

```sh
idf.py -B build_p4_rev1 -D SDKCONFIG=build_p4_rev1/sdkconfig \
  -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.esp32p4_rev_less_v3' \
  set-target esp32p4 build
```


## PC preview

Install the matching `esp-gsp-tools` package, then run from the component root:

```sh
python3 tools/sim_bridge/run.py --project examples/scenarios/industrial_panel/pc
```

The PC backend shares the application controller with the device.


The overview pairs a transparent equipment illustration with a high-contrast
pressure instrument. Typography uses baked Ubuntu Regular/Light subsets.
Artwork sources are listed in [artwork](scenes/artwork/README.md) and
[assets](scenes/assets/README.md).
