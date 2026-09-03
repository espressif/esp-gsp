# Introduction

ESP-GSP is an ahead-of-time UI toolchain and bounded graphics runtime for ESP-IDF. It is designed for products that need reviewable interface source, deterministic resources, a stable application contract and a display path owned by the BSP.

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

The runtime does not replace a BSP, and JSON does not hide product logic. UI callbacks should request work from application tasks; completed application state is then written back through generated setters.

## When to use another path

Use Canvas for camera/video or another producer that owns continuously changing pixels. Use runtime image APIs for occasional encoded image replacement. Use List, Grid, Wheel or Message List for large application-owned collections rather than creating one authored object per data item.

Continue with [Getting started](../getting-started.md) or browse the complete
[Widget library](../components/index.md).
