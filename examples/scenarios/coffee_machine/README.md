# AURA coffee machine

![Drink collection](preview.png)

![Interactive drink gallery](preview_explore.png)

A complete 800 × 480 coffee flow: drink collection, personal recipe,
preparation and an Explore page. The four screens are pages in one full-screen
PageFlow viewport. Page drags commit or return on release; sliders and the
native drink Carousel own their gestures independently.

Choose Espresso, Latte or Cappuccino. The selection underline and recipe photo
follow the drink. Cup volume is 20–300 ml, strength is Mild/Balanced/Strong,
and temperature is 86–96 C. Preparation runs through grinding, warming,
extraction and milk finishing (18 seconds for espresso, 24 for milk drinks).
The collection keeps the three large drink cards and links to Explore, where a
native drag-to-select Carousel uses the same drink images. Carousel and card
selection update the same drink and recipe state. During preparation, page
browsing remains available while the Carousel, drink buttons and recipe sliders
are disabled and edits are rejected. “View preparation” returns to the active
cup and Cancel returns to its recipe.

The Supplies Drawer dims and blocks the entire page underneath it. Tap water
or beans to simulate empty/refill, then close by button or rightward drag.
Starting with an empty supply opens the Drawer without starting preparation.
The heater, grinder, pump and tank sensors use simulated application state.

All three RGB profiles use rotation 0. S31 RGB565 and S3 RGB565 use GT1151
touch. The S31 RGB888 profile has no touch input.

For an unattended gallery check, build with `-D COFFEE_DEMO_PROBE=ON`.
The optional UI-task probe enters Explore, changes the selected drink and
returns to the collection three times. Logs include render errors and the
FreeRTOS stack high-water mark. It stops after twelve steps; build with
`-D COFFEE_DEMO_PROBE=OFF` for normal interaction. This exercises rendering
and page capture, not physical touch timing.

![Native simulator full-screen drag frames](preview_swipe.gif)

## Build for the board

From this directory, with an ESP-IDF checkout exported:

```sh
idf.py -B build_s3_rgb \
  -D SDKCONFIG=build_s3_rgb/sdkconfig \
  -D SDKCONFIG_DEFAULTS='sdkconfig.defaults;sdkconfig.defaults.esp32s3' \
  set-target esp32s3 build
```

The corresponding S31 RGB565 profile is:

```sh
idf.py --preview -B build_s31_rgb565 \
  -D SDKCONFIG=build_s31_rgb565/sdkconfig \
  -D SDKCONFIG_DEFAULTS='sdkconfig.defaults;sdkconfig.defaults.esp32s31' \
  set-target esp32s31 build
```

The S31 RGB888 profile is:

```sh
idf.py --preview -B build_s31_rgb888 \
  -D SDKCONFIG=build_s31_rgb888/sdkconfig \
  -D SDKCONFIG_DEFAULTS='sdkconfig.defaults;sdkconfig.defaults.esp32s31_rgb888' \
  set-target esp32s31 build
```

Each profile can then be deployed with its matching port:

```sh
idf.py -B build_s3_rgb -p PORT flash monitor
```


## PC interaction

From the repository root, with matching `esp-gsp-tools` installed:

```sh
python3 tools/sim_bridge/run.py --project examples/scenarios/coffee_machine/pc
```

The device and PC backend share `main/coffee_ui.c`. Navigation and Drawer
state use a 50 ms timer; preparation advances on a separate one-second timer.
The Explore Carousel commits its selected image on release and writes that
selection back to the shared drink state.


## Screens

![Recipe with native sliders](preview_recipe.png)
![Preparation with native effects](preview_brewing.png)
![A live full-screen drag](preview_drag.png)
![Empty-water guard](preview_supplies.png)
![Completed cup](preview_done.png)

Drink photographs and transparent brewing cutouts are compiled assets. GSP
retains the static page content while the controller updates values and effect
state. The glow ring and steam/extraction shimmers run during preparation and
pause when leaving that page. See [artwork sources](scenes/assets/README.md).
