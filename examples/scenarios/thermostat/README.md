# Thermostat / 240×240

![Thermostat](preview.png)

A rotary climate controller for ESP32-C3-LCDkit with the GC9A01 round SPI
screen, RGB565, no PSRAM and rotation 0. A vector dial and climate icons frame
native temperature readouts. Four PageFlow pages contain temperature, mode,
sleep timer and quick presets; a small warmth pulse indicates active climate control.

## Controls

- Rotate to adjust temperature (16–30°C in 0.5°C steps), Heat/Cool/Off, or a
  0–12 hour sleep timer.
- Short press advances to the next page; hold 800 ms to return home. Settings
  apply immediately except on the preset page. Timer expiry selects Off.
- On **Quick preset**, rotate to preview Home (22°C), Away (18°C) or Night
  (19°C). Press to apply the heating preset and return home; hold to leave
  without applying. An existing sleep timer is preserved.
- In the PC preview, use the side −/+ targets, centre press and footer back
  target. Horizontal pointer movement drags the full page; knob page changes
  are immediate.

Room temperature is simulated at 20.5°C. The encoder defaults are GPIO10 (A),
GPIO6 (B) and GPIO9 (press), configurable under **Thermostat encoder**.
GPIO9 is a boot strap and must be released during reset. Debounced actions pass
through a bounded queue to the UI. Page observation and the sleep timer run on
separate timers.

![Climate mode](preview_mode.png)
![Sleep timer](preview_timer.png)
![Heating preset selection](preview_preset.png)
![Native simulator held drag](preview_drag.png)

## Build

From this directory after exporting ESP-IDF:

```sh
idf.py set-target esp32c3 build
```


## PC preview

Install the matching `esp-gsp-tools` package, then run from the component root:

```sh
python3 tools/sim_bridge/run.py --project examples/scenarios/thermostat/pc
```

The PC backend shares the application controller with the device.


Artwork sources are listed in [assets](scenes/assets/README.md).
