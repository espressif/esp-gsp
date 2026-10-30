# LINEA fabric care / 320×240

![Current native simulator screen](showcase/home.png)

A washing-machine interface for ESP32-S3 with ILI9341 SPI, GT911 touch, octal
PSRAM, RGB565 and rotation 0. Four full-screen PageFlow pages cover fabric
selection, settings, the wash cycle and live details. A single textile image anchors the program selector; arrow controls and four
position dots select the program. A 48-pixel remaining-time readout,
12-pixel minimum text and top-edge grab cues clarify the small display;
large numeric bindings use numeric glyph sets. The progress bar and stage text report cycle state. The same textile image rotates within a stationary vector drum during an
active cycle. The shell and glass highlight stay fixed.

## Controls

- Pull down from the top-edge grab cue on each page to open the native
  cycle shortcuts Drawer. It shows the selected program, remaining minutes,
  cycle phase and door lock. **Start / Pause / Resume / New load** and **Stop cycle**
  use the same controller actions as the Cycle page. **Close** (or an upward
  swipe) dismisses the panel without changing the cycle. A short pull cancels;
  horizontal page swipes do not open it. The panel starts closed, with no auto-demo.
- The door must be closed before starting, including from shortcuts. **Check
  door** opens Details when it is open. Running
  and paused cycles keep program, temperature, spin and door controls locked.
- Use the previous/next arrows to select Cotton, Daily, Delicates or Quick 15;
  selection restores the program's temperature and spin defaults. **Set up
  cycle** opens Cycle Settings; an active cycle keeps its settings locked.
- Open **Cycle Settings**. **+10** cycles 20–60°C; **+200** cycles 400–1200 rpm.
- **Review cycle** opens **Cycle** without starting the machine. Close the door before starting. Program, parameters and
  door state stay locked while running or paused. The progress bar
  and live stage caption follow the simulated cycle.
- **Start / Pause / Resume** controls washing, rinsing and spinning. **Stop cycle**
  resets and unlocks; **New load** returns to program selection.
- Tap **Details** to inspect the stage, completion, remaining time, door state
  and stop control; the header arrow returns without changing the run state.

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


Artwork sources and layers are described in [artwork](scenes/artwork/README.md).
