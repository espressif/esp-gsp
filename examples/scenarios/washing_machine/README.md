# Fabric care / 320×240

![Fabric care](preview.png)

A washing-machine interface for ESP32-S3 with ILI9341 SPI, GT911 touch, octal
PSRAM, RGB565 and rotation 0. Four full-screen PageFlow pages cover fabric
selection, settings, the wash cycle and live details. Retained knitwear artwork
drives a native draggable program carousel; progress rings, a liquid effect and
page-local rotating garments animate the cycle without frame-sequence assets.

![Native simulator drag preview](preview_swipe.gif)

![Pull-down cycle controls](preview_shortcuts.png)

## Controls

- Pull down from the top edge, marked **v PULL** on each page, to open the native
  cycle shortcuts Drawer. It shows the selected program, remaining minutes,
  cycle phase and door lock. **Start / Pause / Resume / New load** and **Cancel**
  use the same controller actions as the Cycle page. **Close** (or an upward
  swipe) dismisses the panel without changing the cycle. A short pull cancels;
  horizontal page swipes do not open it. The panel starts closed, with no auto-demo.
- The door must be closed before starting, including from shortcuts. Running
  and paused cycles keep program, temperature, spin and door controls locked.
- Drag the fabric carousel or tap **Next fabric** to select Cotton, Daily,
  Delicates or Quick 15; both controls restore the program's temperature and
  spin defaults.
- Swipe to **Personalise**. **+10** cycles 20–60°C; **+200** cycles 400–1200 rpm.
- Swipe to **Cycle**. Close the door before starting. Program, parameters and
  door state stay locked while running or paused. The progress bar, phase ring
  and live stage caption follow the simulated cycle.
- **Start / Pause / Resume** controls washing, rinsing and spinning. **Cancel**
  resets and unlocks; **New load** leaves the completed state.
- Tap **Details** during a cycle to inspect the current stage, completion and
  remaining time; **< CYCLE** returns without changing the run state.

The appliance is simulated: one second represents one wash minute. Program
lengths are 60, 45, 30 and 15 minutes respectively. Independent timers handle
page observation, drum motion and cycle time. Motion stops while paused or
when the drum page is off screen.

## Build

From this directory after exporting ESP-IDF:

```sh
idf.py set-target esp32s3 build
```


## PC preview

Install the matching `esp-gsp-tools` package, then run from the component root:

```sh
python3 tools/sim_bridge/run.py --project examples/scenarios/washing_machine/pc
```

The PC backend shares the application controller with the device.
Do not launch the bundle with a bare `gsp_sim_host`: without this backend it
renders the scene but cannot apply the washing-machine application callbacks.


Artwork sources are listed in [assets](scenes/assets/README.md).
