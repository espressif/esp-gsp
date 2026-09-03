# ESP-GSP

English | [中文](README_CN.md)

ESP-GSP (Espressif Graphics Scene Processor) compiles JSON scenes and assets
into an embedded bundle plus a typed C API for ESP-IDF. The device renders a
bounded runtime model through ESP-LCD instead of maintaining a general-purpose
UI object tree.

```text
JSON + assets -> ESP-IDF build -> generated C API -> ESP-GSP -> display
```

## Highlights

| Capability | What it provides |
|---|---|
| Build-time UI | Schema validation, asset compilation, deterministic bundles |
| Generated API | Typed setters and event predicates for named elements |
| Embedded runtime | Configurable fixed pools, dirty rendering, RGB565/RGB888 |
| UI features | Controls, layouts, lists, grids, templates, navigation, media and Canvas |
| Input | Tap, drag, scroll, flick and optional application-owned two-contact pinch |
| Display paths | RGB, MIPI-DSI, SPI and QSPI with safe software fallbacks |

ESP-GSP suits products whose screen structure is known at build time while
text, values, media, collections and visibility change at runtime.

## Requirements and installation

- ESP-IDF 6.0 or later.
- The `esp-gsp-tools` Python package. It automatically downloads the GSPC
  version recorded by the component's `.gspc_version` marker. A
  `.gspc_version` in the IDF project root takes precedence over the component
  marker.
- An application or BSP that initializes an `esp_lcd` display target.

```sh
idf.py add-dependency "espressif/esp-gsp^1.1.0"
python -m pip install -U esp-gsp-tools
```

The managed component contains the sealed runtime, public headers, CMake
integration and maintained release examples. GSPC and the simulator are
separate release executables; the component downloads no compiler or simulator
source and no repository `tools/` directory.

## Quick start

Give application-controlled elements a stable `name` and application actions a
`callback`:

```json
{
  "screen": "main", "w": 320, "h": 240,
  "objects": [
    {"type": "progress", "parent": -1, "name": "load",
     "x": 24, "y": 80, "w": 272, "h": 18,
     "value": 0, "fg_color": "#4CC9F0"},
    {"type": "toggle", "parent": -1, "name": "power",
     "callback": "power_changed",
     "x": 128, "y": 140, "w": 64, "h": 32,
     "fg_color": "#4361EE"}
  ]
}
```

Register it in the application component:

```cmake
idf_component_register(SRCS "app_main.c" PRIV_REQUIRES esp-gsp)
gsp_add_bundle(${COMPONENT_LIB})
```

By default, the build compiles every JSON file in the project's `scenes/`
directory, embeds one bundle, and generates `bundle_gsp.h`. Explicit `SCENES`
paths remain available for multi-bundle projects or JSON kept elsewhere. Start with
`gsp_bundle_config()`, `ESP_GSP_ESP_LCD_CONFIG_INIT()`, and the generated
`gsp_<scene>_<name>_*()` helpers. The
[Getting started guide](docs/en/getting-started.md) contains the complete runnable
integration.

Configuration resolves through one path:

```text
project Kconfig -> JSON-derived GSPB demand -> per-instance override -> library capability
```

GSPC derives authored demand, including `max_instances` on JSON templates.
Kconfig remains effective for firmware-wide runtime policy and heap-backed
capacities in a sealed-library build; only capabilities already fixed into the
library are exposed as read-only `ESP_GSP_BUILD_CAP_*` constants.

## Recommended use

1. Lock the target, logical resolution, pixel format and BSP display path.
2. Keep layout and declarative behavior in JSON; keep product state and
   blocking work in application tasks.
3. Keep Kconfig defaults until compiler output or measurement justifies a
   product-wide change.
4. Rebuild before using generated APIs; never edit generated headers.
5. Preview first, then report target build, board operation and visual
   acceptance separately.

Setters are asynchronous on ESP-IDF. Use `esp_gsp_flush()` only at explicit
synchronization boundaries and call `esp_gsp_stop()` from an application task.

## Documentation

| Task | Documentation |
|---|---|
| Install and run the first scene | [Getting started](docs/en/getting-started.md) |
| Create or revise a UI with Codex | [Agent-assisted UI development](docs/en/guide/agent-assisted-ui.md) |
| Learn every Widget | [Widget library](docs/en/components/index.md) and [compiler inventory](docs/en/reference/widget-inventory.md) |
| Author JSON | [Scene guide](docs/en/guide/scenes.md), [field reference](docs/en/reference/authoring.md), and Schema |
| Connect application state | [Runtime/API guide](docs/en/guide/runtime-api.md) and [public API map](docs/en/reference/api.md) |
| Look up a public C function | [Function reference](docs/en/reference/api-functions.md) |
| Configure a product | [Configuration](docs/en/reference/configuration.md) and [Kconfig usage](docs/en/reference/kconfig.md) |
| Integrate and diagnose | [Workflow](docs/en/guide/workflow.md), [display](docs/en/guide/display.md) and [troubleshooting](docs/en/guide/troubleshooting.md) |
| Preview a deployable bundle | [Standalone simulator](docs/en/reference/simulator.md) |

The [documentation language index](docs/README.md) links the complete English
and Chinese documentation trees. Widget pages are validated against GSPC and
the real `examples/widgets` scenes. Local interaction preview compiles those
scenes with the recommended GSPC and opens them in the published ESP-GSP
simulator.

Examples in the project repository:
[`hello_world`](examples/hello_world)
for the minimum integration,
[`showcase`](examples/showcase)
for product UI, and
[`benchmark`](examples/benchmark)
for repeatable measurement.

## Current limitations

- Scene structure is fixed at build time; use properties, templates,
  collections, runtime media or Canvas for dynamic content.
- Pinch reports two contacts but does not automatically zoom a control.
- A bundle uses one logical scene resolution and one output pixel format.
- Right-to-left layout and complex-script shaping are not supported.

## License

Espressif Modified MIT License. See [LICENSE](LICENSE).
