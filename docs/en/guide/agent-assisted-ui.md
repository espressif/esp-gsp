# Agent-assisted UI development

ESP-GSP provides an installable coding-agent Skill for turning product requirements,
reference images, or existing designs into validated scene JSON and generated
C API integration. The Skill uses the GSPC and ESP-GSP version selected by the
application and reads its schema, API reference and examples.

## Install the Skill

The canonical Skill is `skills/esp-gsp-ui` in the
[ESP-GSP repository](https://github.com/espressif/esp-gsp). Install it from the
same tag or commit as the ESP-GSP version used by the project. For Codex,
`$skill-installer` can install that path.

```text
Use $skill-installer to install skills/esp-gsp-ui from espressif/esp-gsp at
the ref used by this project.
```

From an existing source checkout, copy or link it into the skills directory
used by your coding agent. For example, with Codex:

```sh
mkdir -p ~/.codex/skills
cp -R skills/esp-gsp-ui ~/.codex/skills/
```

Invoke it as `$esp-gsp-ui` on the next turn. The Skill belongs to the GitHub
development experience and is not included in the ESP-IDF
component archive. A project using the managed component still has the
bilingual documentation, widget examples, and generated references that the
Skill reads. Without installing the Skill, follow this guide and use
`gspc doctor`, `gspc cards`, and `gspc diagnose` directly.

## Give it a useful requirement

Provide the project path and describe the desired page behavior. The Skill
reads existing target, display, bundle and tool settings from the project.
For a new project, include the display size or orientation when it is relevant.
Existing project configuration is used when available, and product decisions
that are not represented in the project can be supplied in the request.

Useful details include page states, dynamic data, navigation, available assets,
and the interactions you want checked. You do not need to know generated API
names or resource slot rules.

For assets that should stay outside the firmware, follow the
[external-assets workflow](external-assets.md) and point the agent at
`examples/usage/external_assets`. Keep the manifest, target Image placeholders,
and the UI profile compatible, and request a fallback and status check for
missing or replaced files. For an SD/NAND font, use the
[font-file workflow](external-assets.md#fonts-on-sd): apply the loaded font
before UI startup with an explicit size limit, then close it after the UI has
stopped. This keeps storage ownership and font lifetime in the application
while the generated UI API remains unchanged.

A concise creation request can be:

```text
Use $esp-gsp-ui in this ESP-IDF project. Create scenes/settings.json for a
480x320 RGB888 display with a brightness slider, Wi-Fi toggle, language
dropdown, and Back action. Integrate the generated API, build the project, and
preview the page in the simulator.
```

For a reference image, attach it and state whether it is a visual reference or
an asset that must ship. A static image does not define pressed, disabled,
scrolling, loading, error, or navigation behavior; include those states in the
request when they matter.

For an existing UI, name the behavior rather than asking for a blind rewrite:

```text
Use $esp-gsp-ui to add an application-backed Wheel to scenes/setup.json.
The available options come from the application and can change after loading.
Keep the current selection when possible, support an empty list, and preview
scrolling and selection.
```

For a reported problem, provide the steps and expected result:

```text
Use $esp-gsp-ui to investigate this: tapping Enter switches to the clock page
and also triggers the card at the same position. Reproduce it in the simulator,
find the cause, and fix it so one touch only activates the starting page.
```

## What the Skill does

The Skill follows the same ownership model as the
[recommended workflow](workflow.md):

1. identifies the selected component, target, GSPC, bundle, scene, and BSP;
2. reads the current Schema, relevant Widget page, and checked Widget example;
3. keeps layout and declarative behavior in JSON, product state in the
   application, and panel behavior in the BSP;
4. uses `gspc cards` for the target widget, then runs `gspc compatibility` and
   `gspc diagnose` against the real scene (diagnostics include `suggestions`);
   CLI diagnosis includes compilation checks. Daemon `diagnostics/pull` defaults
   to schema checks; use `level: "compile"` when advertised by `doctor/get`;
5. generates bundle headers and `*.api.json`, integrates their APIs, and builds
   application changes;
6. when preview is requested, resolves or builds the simulator, loads the
   `--api-json` from the same compile, and verifies named input, state and
   inspection against the compiled initial layout; with an application backend,
   state writes and navigation stay backend-owned;
7. summarizes changes, build results and preview checks.

When an ESP-IDF build already exists, run `gspc doctor <project> --build
<build-dir>` first. Use its observed target and component contract for the
agent workflow; an unavailable build description must remain an unknown rather
than being replaced with a host assumption.

Use [Scene JSON](scenes.md) to understand authored structure,
[Runtime and generated API](runtime-api.md) for application integration, and
the [Widget library](../components/index.md) for every supported control.
Each card's `example` is a fragment: use its `example_path` and documentation
paths for the complete example and field reference. For PageFlow/Drawer
automation, check `capabilities` for component-motion support before using
`component_get_motion`, `wait_component(name, optional value, max_frames)`, or
`component_event`; a successful wait requires idle and, if supplied, the requested value,
and timeout is a JSON-RPC error. Compiled bounds are initial metadata, not
runtime hit-test proof.

## Choose the preview path

Use the standalone simulator from [Simulator preview and testing](simulator-preview.md)
for scene layout, declarative actions, and control behavior. This checks the
scene without running the owning application's C tasks or product state logic.

When acceptance depends on application C callbacks, timers, dynamic collections,
or live state, prefer the component's `sim_bridge`. It runs portable application
C code on the PC and sends UI rendering to the simulator. After the ESP-GSP
component is installed in the project (for example, after `idf.py reconfigure`),
run this from the application root:

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py --project pc
```

The component also includes `examples/usage/hello_world/pc` and
`examples/usage/sim_bridge_media/pc`; pass either path as `--project` to try the
flow. From a source checkout, invoke that checkout's
`tools/sim_bridge/run.py`. See the [sim_bridge guide](../../../tools/sim_bridge/README.md)
for requirements, tool selection, PC HAL/mocks, and the supported API subset.

Use sim_bridge to check application behavior. Its supported API subset and
single-thread Backend contract differ from ESP-IDF task scheduling; it does not
verify peripherals, panel timing, or device performance. Build the owning
ESP-IDF application and use the board for those checks.

## Review the result

Review the source JSON and application changes, then rebuild to refresh generated
headers and bundles. Check object names, callback responsiveness, resource ownership
and capacity for simultaneously active objects. Preview layout and interaction in the
simulator, then follow [Display integration](display.md) to check the board.

For a source-checkout release gate, run
`python3 tools/sim_host/tests/agent_loop_smoke.py --host <sim-host> --gspc <gspc>`.
It checks one deterministic diagnose-and-repair path and then loads the matching
Bundle/API sidecar into the simulator. It does not replace the owning ESP-IDF
build or board acceptance.

The Skill reads the selected component's schema, headers and examples and follows
the language used in your request.
