# ATELIER coffee machine

![Current native simulator screen](showcase/home.png)

A complete 800 × 480 coffee flow with a single-cup collection, personal recipe,
preparation and an Explore page. The collection puts the selected cutout,
recipe summary and **Brew now** first; **Adjust** opens the recipe screen.
The four screens are pages in one full-screen
PageFlow viewport. Page drags commit or return on release; sliders own their gestures independently.

Choose Espresso, Latte or Cappuccino. The selection underline and recipe photo
follow the drink. Cup volume is 20–300 ml, strength is Mild/Balanced/Strong,
and temperature is 86–96 C. Preparation runs through grinding, warming,
extraction and milk finishing (18 seconds for espresso, 24 for milk drinks).
The collection's three footer selectors share state with Explore's three
large product cards. The catalog reuses the same cup cutouts without another
image set. During preparation, page browsing remains available while drink
selection, machine care and recipe sliders are disabled and edits are rejected. “View preparation” returns to the active
cup and Cancel returns to its recipe.

The Supplies Drawer dims and blocks the entire page underneath it. Tap water
or beans to simulate empty/refill, then close by button or rightward drag.
Starting with an empty supply opens the Drawer without starting preparation.
The heater, grinder, pump and tank sensors use simulated application state.

All three RGB profiles use rotation 0. S31 RGB565 and S3 RGB565 use GT1151
touch. The S31 RGB888 profile has no touch input and automatically tours
Collection, Explore, Recipe and Preparation, including drink selection and a
completed cup. Each idle screen stays for six seconds; preparation uses the
normal 18/24-second controller. The tour uses the same supply guard as touch
operation and is disabled when the optional device probe is enabled.

For an unattended gallery check, build with `-D COFFEE_DEMO_PROBE=ON`.
The optional UI-task probe enters Explore, changes the selected drink and
returns to the collection three times. Logs include render errors and the
FreeRTOS stack high-water mark. It stops after twelve steps; build with
`-D COFFEE_DEMO_PROBE=OFF` for normal interaction. This exercises rendering
and page capture, not physical touch timing.

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
Explore cards dispatch the same recipe actions as the home drink selectors.


## Screens

The English typography uses five sizes, static text subsets and explicit
runtime glyph sets. The same stored cutouts are shared between collection
and preparation. Drink photographs and transparent brewing cutouts are compiled assets. GSP
retains the static page content while the controller updates the stage,
percentage and a thin progress ring once per simulated second. No ambient
glow, sweep or simulated steam runs behind the cup. See [artwork sources](scenes/assets/README.md).
