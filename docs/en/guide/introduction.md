# Introduction

ESP-GSP builds ESP-IDF interfaces from JSON scenes and image/font assets.
GSPC compiles them into a bundle before the firmware runs; the device runtime
renders that bundle and handles input. Application code updates named controls
through generated C functions, while the BSP configures the display and touch hardware.

## What it provides

| Capability | Responsibility |
|---|---|
| Declarative scenes | JSON owns hierarchy, layout, styles, static resources, and local actions. |
| Compiled bundles | GSPC validates input and packs scenes and assets into versioned GSPB data. |
| Typed integration | Stable names generate setters, getters, event predicates, and component adapters. |
| Portable rendering | The same core runs through WebAssembly for local preview and ESP-LCD on the device. |

## Ownership boundaries

| Concern | Owner |
|---|---|
| Structure, appearance, static assets | Scene JSON |
| Validation, resource conversion, bundle/API generation | GSPC |
| Product state, networking, storage, sensors | Application tasks |
| Panel timing, rotation, buffers, touch mapping | BSP and ESP-LCD display target |
| Rendering, input dispatch, component state | ESP-GSP runtime |

UI callbacks request work from application tasks. Update the interface through generated setters after the application state changes.

## Choose a data interface

Use Canvas for camera/video or another producer that owns continuously changing pixels. Use runtime image APIs for occasional encoded image replacement. Use List, Grid, Wheel or Message List for large application-owned collections rather than creating one authored object per data item.

Continue with [Getting started](../getting-started.md) or browse the complete
[Widget library](../components/index.md).
