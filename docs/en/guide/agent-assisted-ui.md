# Agent-assisted UI development

ESP-GSP provides one installable Codex Skill for turning product requirements,
reference images, or existing designs into validated scene JSON and generated
C API integration. The Skill uses the GSPC and ESP-GSP version selected by the
application; it does not carry a second widget schema or API reference.

## Install the Skill

The canonical Skill is `skills/esp-gsp-ui` in the
[ESP-GSP repository](https://github.com/espressif/esp-gsp). Ask Codex to install
that path from repository `espressif/esp-gsp` at ref `master` with
`$skill-installer`.

```text
Use $skill-installer to install skills/esp-gsp-ui from espressif/esp-gsp at
ref master.
```

From an existing source checkout, it can also be installed manually:

```sh
mkdir -p ~/.codex/skills
cp -R skills/esp-gsp-ui ~/.codex/skills/
```

Invoke it as `$esp-gsp-ui` on the next turn. The Skill belongs to the GitHub
development experience and is intentionally not included in the ESP-IDF
component archive. A project using the managed component still has the
bilingual documentation, widget examples, and generated references that the
Skill reads.

## Give it a useful requirement

Provide the project path and the product facts that cannot be derived safely:

- target and logical display size;
- RGB565 or RGB888 output;
- intended BSP/display path and orientation;
- screen states, dynamic data, actions, and navigation;
- available fonts and product assets;
- desired scene path when the project does not use `scenes/`;
- required validation level.

A concise creation request can be:

```text
Use $esp-gsp-ui in this ESP-IDF project. Create scenes/settings.json for a
480x320 RGB888 display with a brightness slider, Wi-Fi toggle, language
dropdown, and Back action. Integrate the generated API and run scene and target
build validation. Do not change the BSP.
```

For a reference image, attach it and state whether it is a visual reference or
an asset that must ship. A static image does not define pressed, disabled,
scrolling, loading, error, or navigation behavior; include those states in the
request when they matter.

For an existing UI, name the behavior rather than asking for a blind rewrite:

```text
Use $esp-gsp-ui to add an application-backed Wheel to scenes/setup.json.
Preserve the current bundle symbol and pixel format, derive the required slots
from the active scene, and update application code only through generated or
public APIs.
```

## What the Skill does

The Skill follows the same ownership model as the
[recommended workflow](workflow.md):

1. identifies the selected component, target, GSPC, bundle, scene, and BSP;
2. reads the current Schema, relevant Widget page, and checked Widget example;
3. keeps layout and declarative behavior in JSON, product state in the
   application, and panel behavior in the BSP;
4. runs `gspc compatibility` and `gspc diagnose` against the real scene;
5. builds before using the generated `<symbol>_gsp.h` API;
6. previews through the standalone simulator when it is available;
7. reports scene, generated API, target, simulator, board, and visual evidence
   separately.

Use [Scene JSON](scenes.md) to understand authored structure,
[Runtime and generated API](runtime-api.md) for application integration, and
the [Widget library](../components/index.md) for every supported control.

## Review the result

Review the source JSON and application diff. Generated headers, GSPB files,
compiler exports, and preview output must remain tool-owned and uncommitted.
Confirm that application-controlled objects have stable names, callbacks do not
block, asset ownership is explicit, and configuration reflects simultaneously
active resources rather than total data-set size.

Compiler acceptance proves the scene contract. A target build proves
configuration, compilation, and linking. Simulator output does not prove panel
wiring, rotation, byte order, tearing, touch mapping, performance, or final
visual quality; use the intended hardware for those acceptance steps.

## Maintenance contract

There is one Skill for both English and Chinese users. It answers in the
request language and routes to the matching documentation tree. Widget fields,
defaults, C signatures, compatibility versions, and examples stay owned by
GSPC, public headers, documentation generators, and `examples/widgets/`.
Updating those sources updates what the Skill discovers without copying facts
into another prompt file.
