# Industrial process panel / 1024×600

![Production overview](preview.png)

![Pressure history inspector](preview_trend.png)
![Pull-down process alerts](preview_alerts.png)

A P4 MIPI-DSI touch panel using RGB565, PSRAM and rotation 0. Four full-screen
PageFlow pages present production, the pneumatic circuit, maintenance and a
read-only pressure Trend inspector.
Photography and a baked glass material accompany native telemetry; the retained
vector circuit uses flow shimmers and an animated scanner.

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
50 ms timer. The animated scanner and flow effects stop when their page is
hidden or covered by the setpoint Drawer.

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


Artwork sources are listed in [assets](scenes/assets/README.md).
