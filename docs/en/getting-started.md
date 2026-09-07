# Getting Started

This guide builds the default embedded-bundle path: an ESP-IDF application
owns the display, ESP-GSP compiles one JSON scene during the build, and the
application updates the UI through the generated C API.

If you want to evaluate ESP-GSP before integrating it, start with the maintained
example:

```sh
idf.py create-project-from-example "espressif/esp-gsp=1.2.0:hello_world"
cd hello_world
python -m pip install -U esp-gsp-tools
```

Then follow the example's `README.md` for a simulator preview or a matching
board profile. The rest of this guide explains how to integrate ESP-GSP into an
existing application.

## Prerequisites

- ESP-IDF 6.0 or later is exported in the current shell.
- Either `esp-gsp-tools` is installed for automatic resolution, or a compatible
  GSPC release is available for explicit `GSPC_EXECUTABLE` selection.
- A BSP or application layer can initialize an `esp_lcd` panel and fill an
  `esp_display_present_target_config_t`.
- The logical scene size and pixel format are known.

Touch is optional. When present, the BSP supplies an
`esp_lcd_touch_handle_t` whose coordinates match the logical display
orientation.

## 1. Add the component

From the ESP-IDF project root:

```sh
idf.py add-dependency "espressif/esp-gsp^1.2.0"
```

The equivalent component manifest entry is:

```yaml
dependencies:
  espressif/esp-gsp: "^1.2.0"
```

For a local ESP-GSP component directory, use Component Manager `override_path`
or add it to `EXTRA_COMPONENT_DIRS`. Keep only one selected component copy in a
build so the compiler, public headers, and runtime come from the same version;
applications do not need their implementation sources.

## 2. Install GSPC; add the simulator when needed

Install the toolchain manager. CMake reads the component's `.gspc_version` and
invokes `esp-gsp-tools`; the first invocation downloads and verifies that GSPC
release into the local cache automatically:

```sh
python -m pip install -U esp-gsp-tools
```

The default `.gspc_version` in the component package is the latest GSPC version
compatible with the current GSP release at publish time. To override the GSPC
version for an IDF project, create `.gspc_version` in the project root; the
project marker takes precedence over the component marker:

```sh
echo '0.3.0' > .gspc_version # Pin GSPC 0.3.0
idf.py build
```

> [!TIP]
>
> If the manager cannot be installed or automatic downloads are unsuitable, see
> the [Compatibility contract](./reference/compatibility.md), unpack the
> archive into a stable tool directory, and override via an environment variable
> or CMake argument:
>
> ```shell
> GSPC_EXECUTABLE=/absolute/path/to/gspc
> # You can also override via CMake:
> idf.py -D GSPC_EXECUTABLE=/absolute/path/to/gspc build
> ```

`gsp_add_bundle()` validates the selected compiler during CMake configuration.
Do not copy a compiler from an unrelated source checkout merely because
`gspc --version` runs.

The simulator is optional; firmware builds do not require it. Version
detection for `sim` is currently manual: use the `version` field from
`idf_component.yml` (the ESP-GSP component version), not the GSPC version in
`.gspc_version`:

```sh
python -m gsp.execute --version '<ESP-GSP version>' sim --bundle product.gspb
```

Alternatively, download the matching simulator archive from the
[ESP-GSP Releases page](https://github.com/espressif/esp-gsp/releases), set
`GSP_SIM_EXECUTABLE`, and invoke that executable directly.

See the [Simulator reference](reference/simulator.md) for capabilities,
CLI options, and evidence limits.

## 3. Add a scene

A small application can use this layout:

```text
my_app/
├── CMakeLists.txt
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   ├── app_main.c
│   └── idf_component.yml
└── scenes/
    ├── main.json
    └── assets/
```

Create `scenes/main.json`:

```json
{
  "screen": "main",
  "w": 320,
  "h": 240,
  "screen_bg": "#101820",
  "objects": [
    {
      "type": "progress",
      "parent": -1,
      "name": "load",
      "x": 40,
      "y": 96,
      "w": 240,
      "h": 24,
      "value": 30,
      "fg_color": "#4CC9F0"
    },
    {
      "type": "toggle",
      "parent": -1,
      "name": "power",
      "x": 128,
      "y": 164,
      "w": 64,
      "h": 32,
      "fg_color": "#4361EE",
      "callback": "power_changed"
    }
  ]
}
```

Use `name` only for elements the application must address. Use `callback` for
actions that must cross from the UI into product logic. Asset paths are
relative to the scene file.

Read the [Scene JSON guide](guide/scenes.md) for hierarchy, field selection,
runtime naming, and reference-image translation. The exhaustive field and
widget reference is available in [Scene authoring](reference/authoring.md).

## 4. Register the bundle

In `main/CMakeLists.txt`:

```cmake
idf_component_register(SRCS "app_main.c"
                       PRIV_REQUIRES esp-gsp)

gsp_add_bundle(${COMPONENT_LIB})
```

The default call discovers and sorts `PROJECT_DIR/scenes/*.json`, and uses
RGB565. The directory name is deliberately `scenes`: it describes application
content without being confused with the ESP-GSP component or a generic UI
source directory. It also gives small applications one predictable place for
JSON and scene-relative assets.

`gsp_add_bundle()` performs four jobs during the ESP-IDF build:

1. validates and compiles the JSON scene;
2. tracks referenced fonts and images as build dependencies;
3. packs the generated scene and resources into an embedded bundle;
4. adds `bundle_gsp.h` and the per-scene generated headers to the component's
   include path.

The default symbol is `bundle`. For multiple independent bundles, or for JSON
that must remain elsewhere, list paths explicitly:

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../product/home.json" "../shared/settings.json"
    SYMBOL product
    PIXEL_FORMAT rgb888)
```

Set a different `SYMBOL` for each bundle. See
[Configuration](reference/configuration.md) for all build-time options.

## 5. Start the UI

The BSP remains responsible for the panel, framebuffer, byte order, physical
rotation, and optional touch controller. Application startup then uses the
generated bundle configuration:

```c
#include "esp_gsp_esp_lcd.h"
#include "bundle_gsp.h"

static void on_ui_event(esp_gsp_handle_t ui,
                        const esp_gsp_event_t *event,
                        void *user_ctx)
{
    (void)ui;
    (void)user_ctx;
    if (gsp_main_event_is_power_changed(event)) {
        /* Notify an application task; do not block this callback. */
    }
}

void app_main(void)
{
    esp_display_present_target_config_t display;
    ESP_ERROR_CHECK(board_display_init(&display));

    esp_lcd_touch_handle_t touch = NULL;
    (void)board_touch_init(&touch); /* Optional. */

    esp_gsp_config_t app = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
    lcd.display = display;
    lcd.touch = touch;

    esp_gsp_handle_t ui;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
    ESP_ERROR_CHECK(esp_gsp_on_event(ui, on_ui_event, NULL));
    ESP_ERROR_CHECK(gsp_main_load_set_value(ui, 60));
}
```

`board_display_init()` and `board_touch_init()` are placeholders for the
product BSP. The repository's shared
[`examples/common/hw_init`](../../examples/common/hw_init) component shows
complete panel targets used by the examples.

## 6. Build and inspect the generated API

Select the real target and build:

```sh
idf.py set-target <target>
idf.py build
```

If the scene, an asset, a selected Kconfig capacity, or a bundle option is
invalid, generation fails as part of this build. After a successful build,
use editor completion or inspect `bundle_gsp.h`. Generated names follow:

```text
gsp_<scene>_<named-element>_<operation>()
```

The available operations depend on the element type and dynamic properties.
Do not copy a helper from another scene and do not edit generated headers.
Change JSON, rebuild, and consume the API that was generated for that scene.

## 7. Configure product limits when needed

ESP-GSP works with balanced defaults. If the scene compiler reports a capacity
shortage, or product measurements justify a feature, memory, task, or input
change, open:

```sh
idf.py menuconfig
```

Then navigate to `Component config -> ESP-GSP`. Use `menuconfig` for the
active build and keep intentional clean-build values in `sdkconfig.defaults`.
The active `sdkconfig` is the value that actually controls an existing build.
See the [ESP-IDF Kconfig guide](reference/kconfig.md) for target-specific defaults,
runtime precedence, memory trade-offs, and a clean-profile validation command.

## 8. Run the maintained example

`examples/hello_world` is the smallest complete repository integration. For
the included ESP32-P4 profile:

```sh
cd examples/hello_world
idf.py -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32p4 build
idf.py flash monitor
```

ESP-IDF automatically applies `sdkconfig.defaults.esp32p4` after the base
defaults. Other checked-in `sdkconfig.defaults.<target>` fragments exercise
different display paths. Their pins and timings are examples; they must match
the actual board before flashing.

## 9. Use the normal edit loop

```text
edit JSON or assets
        |
        v
build and fix compiler diagnostics
        |
        v
inspect generated API
        |
        v
integrate application state and events
        |
        v
preview -> target build -> board run -> visual acceptance
```

Continue with [Recommended development workflow](guide/workflow.md).
For ownership, callback, and shutdown rules, read
[Application lifecycle](guide/lifecycle.md) before adding runtime media,
lists, or background tasks.
