# Agent-assisted UI development

ESP-GSP provides one installable Codex Skill for turning product requirements,
reference images, or existing designs into validated scene JSON and generated
C API integration. The Skill uses the GSPC and ESP-GSP version selected by the
application and reads its schema, API reference and examples.

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
development experience and is not included in the ESP-IDF
component archive. A project using the managed component still has the
bilingual documentation, widget examples, and generated references that the
Skill reads.

## Give it a useful requirement

Provide the project path and describe the desired page behavior. The Skill
reads existing target, display, bundle and tool settings from the project.
For a new project, supply display size and orientation when known; it asks
about missing choices as needed.

Useful details include page states, dynamic data, navigation, available assets,
and the interactions you want checked. You do not need to know generated API
names or resource slot rules.

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
4. runs `gspc compatibility` and `gspc diagnose` against the real scene;
5. generates bundle headers, integrates their APIs, and builds application changes;
6. resolves or builds the simulator for requested previews and checks interactions;
7. summarizes changes, build results and preview checks.

Use [Scene JSON](scenes.md) to understand authored structure,
[Runtime and generated API](runtime-api.md) for application integration, and
the [Widget library](../components/index.md) for every supported control.

## Review the result

Review the source JSON and application changes, then rebuild to refresh generated
headers and bundles. Check object names, callback responsiveness, resource ownership
and capacity for simultaneously active objects. Preview layout and interaction in the
simulator, then follow [Display integration](display.md) to check the board.

The Skill reads the selected component's schema, headers and examples and follows
the language used in your request.
