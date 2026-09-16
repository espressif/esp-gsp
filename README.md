# ESP-GSP

English | [简体中文](README_zh.md)

ESP-GSP (Espressif Graphics Scene Processor) is an ahead-of-time compiled UI
framework for ESP-IDF. It turns JSON scenes and assets into an embedded Bundle
and a typed C API, then renders them through an application-provided ESP-LCD
display target.

```text
JSON + assets -> ESP-IDF build -> generated C API -> ESP-GSP -> display
```

Use ESP-GSP when the screen structure is known at build time while text,
values, images, collections and visibility change at runtime. The
runtime, deterministic assets and generated API are designed for embedded
products that need predictable memory use and reviewable UI sources.

## Highlights

| Capability | What it provides |
|---|---|
| Build-time UI | Schema validation, asset compilation and deterministic Bundles |
| Generated API | Typed setters and event predicates for named elements |
| Embedded runtime | Configurable memory use, RGB565 and RGB888 rendering |
| UI features | Controls, layouts, lists, grids, templates, navigation, media and Canvas |
| Input | Tap, drag, scroll, flick and optional application-owned two-contact pinch |
| Display paths | RGB, MIPI-DSI, SPI and QSPI with safe software fallbacks |

## Requirements

- ESP-IDF 6.0 or later.
- Python 3.10 or later for the `esp-gsp-tools` toolchain manager.
- An application or BSP that initializes an `esp_lcd` display target.

## Try the maintained example

Create the smallest complete example directly from the ESP Component Registry:

```sh
idf.py create-project-from-example "espressif/esp-gsp=1.3.0:hello_world"
cd hello_world
python -m pip install -U esp-gsp-tools
```

The example includes checked profiles for ESP32-C3, ESP32-S3, ESP32-P4 and
ESP32-S31. Select the profile that matches the real board and verify its panel
pins and timings before flashing. See the
[`hello_world` instructions](examples/hello_world/README.md) for preview and
build commands.

Compiler setup uses the active ESP-IDF Python environment. A matching `gspc`
already on PATH can be used offline; otherwise CMake uses the tool manager.
Source checkouts automatically build their compiler with Cargo. See
[compiler selection](docs/en/guide/workflow.md#compiler-selection) for version
pinning, offline use and recovery from installation errors.

## Add ESP-GSP to an existing project

From the ESP-IDF project root:

```sh
idf.py add-dependency "espressif/esp-gsp^1.3.0"
python -m pip install -U esp-gsp-tools
```

Create scene JSON files under the project-level `scenes/` directory. Give an
element a stable `name` when the application must update it, and add a
`callback` when an action must enter application logic:

```json
{
  "screen": "main", "w": 320, "h": 240,
  "objects": [
    {"type": "progress", "parent": -1, "name": "load",
     "x": 24, "y": 80, "w": 272, "h": 18, "value": 0},
    {"type": "toggle", "parent": -1, "name": "power",
     "callback": "power_changed", "x": 128, "y": 140, "w": 64, "h": 32}
  ]
}
```

Register the Bundle after the application component:

```cmake
idf_component_register(SRCS "app_main.c" PRIV_REQUIRES esp-gsp)
gsp_add_bundle(${COMPONENT_LIB})
```

The next build validates every scene, compiles its assets, embeds one Bundle
and generates `bundle_gsp.h`. Start from `gsp_bundle_config()` and
`ESP_GSP_ESP_LCD_CONFIG_INIT()`, then use the generated
`gsp_<scene>_<name>_*()` helpers. The
[Getting Started guide](docs/en/getting-started.md) provides the complete
integration and display startup code.

## Important runtime contracts

- Keep layout and declarative behavior in JSON; keep product state and blocking
  work in application tasks.
- Generated setters are asynchronous on ESP-IDF. Use `esp_gsp_flush()` only at
  explicit synchronization boundaries.
- Stop external producers first, then call `esp_gsp_stop()` from an application
  task. Do not block framework callbacks.
- A Bundle has one logical scene resolution and one RGB565 or RGB888 output
  format. The BSP owns panel timing, rotation, byte order and touch mapping.

## Documentation

| Task | Start here |
|---|---|
| Install and run the first scene | [Getting Started](docs/en/getting-started.md) |
| Understand the recommended workflow | [Development workflow](docs/en/guide/workflow.md) |
| Author JSON and choose fields | [Scene guide](docs/en/guide/scenes.md) and [field reference](docs/en/reference/authoring.md) |
| Learn a control | [Widget library](docs/en/components/index.md) |
| Connect application state | [Runtime and generated API](docs/en/guide/runtime-api.md) |
| Configure memory and features | [Configuration](docs/en/guide/configuration.md) and [Kconfig](docs/en/reference/kconfig.md) |
| Integrate a display | [Display presentation](docs/en/guide/display.md) |
| Preview without hardware | [Simulator preview](docs/en/guide/simulator-preview.md) |
| Diagnose a problem | [Troubleshooting](docs/en/guide/troubleshooting.md) |
| Look up exact C declarations | [Function reference](docs/en/reference/api-functions.md) |

The [documentation index](docs/README.md) links the complete English and
Simplified Chinese documentation sets. The repository also contains the
[`showcase`](examples/showcase/README.md) product demo and the
[`benchmark`](examples/benchmark/README.md) hardware workload.

## Choose the integration path

- Build the scene structure with GSPC. Use properties and templates for dynamic
  UI, collections for application datasets, runtime media for encoded images,
  and Canvas for continuous pixel producers.
- Implement pinch scaling in application logic using the reported two-contact
  gesture, then update the target control or Canvas.
- For right-to-left or complex scripts, prepare rendered text assets or use
  Canvas with an application text engine.

## License

Espressif Modified MIT License. See [LICENSE](LICENSE).
