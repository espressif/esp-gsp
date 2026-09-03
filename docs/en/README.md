# ESP-GSP Documentation

[Documentation languages](../README.md) | **English** | [中文](../zh-Hans/README.md)

Use one path for each task. Concept guides explain decisions, reference pages
define compiler facts, and advanced documents describe runtime contracts.

## Start here

1. [Introduction](guide/introduction.md)
2. [Install and run the first scene](getting-started.md)
3. [Recommended project workflow](guide/workflow.md)
4. [Scene structure and naming](guide/scenes.md)
5. [Runtime and typed API](guide/runtime-api.md)
6. [Widget library](components/index.md)
7. [Agent-assisted UI development](guide/agent-assisted-ui.md)

## Build an application

| Task | Canonical document |
|---|---|
| Choose what belongs in JSON, application code, or the BSP | [Recommended workflow](guide/workflow.md) |
| Create or revise a UI with Codex | [Agent-assisted UI development](guide/agent-assisted-ui.md) |
| Author hierarchy, styles, actions, and templates | [Scene JSON guide](guide/scenes.md) |
| Use runtime images, Canvas, lists, grids, and application data | [Media and application data](guide/media-and-data.md) |
| Use scenes, viewports, drawers, and retained navigation | [Navigation and viewports](guide/navigation.md) |
| Start, update, synchronize, suspend, and stop safely | [Lifecycle and threading](guide/lifecycle.md) |
| Understand public structures and ownership | [Application structures](guide/application-structures.md) |
| Integrate RGB, MIPI-DSI, SPI, or QSPI presentation | [Display presentation](guide/display.md) |

## Configure and diagnose

| Task | Canonical document |
|---|---|
| Select JSON demand, Kconfig policy, and instance overrides | [Configuration model](guide/configuration.md) |
| Look up runtime configuration fields and precedence | [Configuration reference](reference/configuration.md) |
| Use ESP-IDF `menuconfig` and reproducible defaults | [Kconfig guide](reference/kconfig.md) |
| Diagnose compiler, link, runtime, display, and input failures | [Troubleshooting](guide/troubleshooting.md) |
| Look up application and platform APIs | [Public API map](reference/api.md) |
| Look up exact public declarations and contracts | [Function reference](reference/api-functions.md) |
| Preview, test, and automate with the simulator | [Simulator preview and testing](guide/simulator-preview.md) |
| Look up simulator CLI, API, and backend reference | [Simulator reference](reference/simulator.md) |

## Reference

- [Complete authoring reference](reference/authoring.md) — fields, defaults, events, and actions from GSPC.
- [Widget inventory](reference/widget-inventory.md) — compiler registry coverage.
- [Configuration inventory](reference/configuration-options.md) — Kconfig and configuration schema.
- [Simulator reference](reference/simulator.md) — browser preview, CLI, input modes, automation API, and application backend.
- [Compatibility contract](reference/compatibility.md) — component, GSPC, format, and simulator ABI versions.
- [JSON Schema](reference/scene.schema.json) — editor and validation contract.

The [Widget library](components/index.md) covers every current control with a
checked JSON example, exact C signatures, and the canonical local simulator
preview command.

## Evidence boundaries

- Scene compilation proves that JSON, assets, profile, and capacities are accepted.
- An ESP-IDF build proves configuration, compilation, and linking for that target.
- Native or WASM preview proves portable behavior, not panel wiring or touch orientation.
- Device logs prove execution, not visual correctness.
- Final display quality and performance require target-hardware acceptance.
