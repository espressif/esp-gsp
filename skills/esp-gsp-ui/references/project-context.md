# Project context and tool resolution

## Establish the project contract

Identify the application root, active ESP-GSP component, GSPC executable,
bundle registration and affected scenes. Read logical resolution, pixel format
and required interactions for scene work; inspect `IDF_TARGET`, BSP display
path and PSRAM when integrating or diagnosing the device.

Use `gspc doctor <project> --build <build-dir>` when an ESP-IDF build exists; it
reads `build/project_description.json` and reports the observed Target, build
revision, selected `esp-gsp` component path, sdkconfig capabilities and relevant
CMake options. Use that contract to identify the component selected by the
build. If no build exists, inspect the project dependency and CMake
configuration without assuming that a nearby ESP-GSP checkout is active. Resolve
GSPC from the CMake cache or `GSPC_EXECUTABLE`. Otherwise read `.gspc_version`
from the application root first and the selected component second, then use
the installed `esp-gsp-tools` implicit resolver:

```sh
python -m gsp.execute --version <version> gspc compatibility
```

When developing the ESP-GSP framework itself, `ci/gspc-dev` remains available
for CI and source validation. Use the selected component's version metadata to
configure the project marker or `GSPC_EXECUTABLE`. Ask for a version or executable
only when it cannot be determined from the project. Reuse an existing compiler
report, including `doctor.compatibility`, if it came from this executable and
its inputs remain unchanged. Otherwise query the resolved compiler:

```sh
gspc compatibility
```

Treat this output as the compiler side of the contract; the owning CMake
configure or build performs the final component/compiler compatibility check.
Do not continue with a known incompatibility. Ask only when a missing choice
changes product behavior, such as resolution, rotation, pixel format, dynamic
data ownership, navigation, or asset treatment.

Commands below use `gspc` for readability. Invoke the resolved absolute
executable when it is not available through `PATH`.

## Use current sources of truth

Read documentation from the ESP-GSP component selected by the application
build. Start with the matching language under `docs/en/` or `docs/zh-cn/`.
In a framework source checkout, the corresponding maintained pages live under
`docs/src/content/docs/` and its `zh-cn/` subtree. Load only the pages needed
for the task:

- `guide/workflow.md` for JSON, application, and BSP ownership;
- `guide/scenes.md` for hierarchy, naming, actions, and templates;
- `guide/runtime-api.md` and `guide/media-and-data.md` for generated APIs,
  callbacks, collections, Canvas, and buffer ownership;
- `reference/configuration.md` for `gsp_add_bundle()` and runtime configuration;
- `reference/simulator.md` for simulator CLI, automation API, and evidence limits;
- `configuration/simulator-preview.md` for preview workflows and testing strategies;
- `tools/sim_bridge/README.md` for running portable application C code with the
  simulator and the bridge's supported API subset;
- `components/<widget>.md` and `examples/usage/widgets/<widget>/<widget>.json` for a
  concrete control.

Query GSPC instead of copying field or version tables into the project or this
skill. Load a known control's card directly; use `gspc cards --list` when
discovering controls. Read its component page and checked example as needed;
add schema, docs, or inventory when the task needs field ranges or the full
registry:

```sh
mkdir -p build/gsp-agent-cache
gspc cards --list
gspc cards slider
gspc schema --authoring -o build/gsp-agent-cache/scene.schema.json
gspc docs -o build/gsp-agent-cache/authoring-reference.md
gspc inventory --format json -o build/gsp-agent-cache/widget-inventory.json
```

After compile or pack, read the sidecar `*.api.json` next to generated headers
for object names, typed operations, helpers, bind/component keys and callbacks.
Use the bundle-level sidecar produced by the same compile as the preview bundle;
the simulator rejects mismatched bundle length/CRC metadata. Those mappings
come from the compiler sidecar, not from inspecting GSPB bytes.
For `gsp_add_bundle`, use the sidecar path reported by the build next to
`<symbol>_gsp.h`. For direct GSPC use, pass `--api-header`; its extension is
replaced with `.api.json`.
`gspc doctor [project]` reports the compiler contract, sidecar commands and
scene files; when a build description is available it also reports target
facts. An unavailable build contract is unknown, not a host-profile fallback.
Read `build_contract.status` and resolve `error`, `missing`, or `stale_reasons`
when reported. `observed` identifies the selected build's metadata snapshot.
An explicit `--build` selects only that build directory. Use the owning
configure/build to validate the configuration and tool compatibility.

Keep these disposable exports below the ignored build tree.
The installed compiler schema, selected component documentation, public
headers, generated headers, and checked widget examples are authoritative.

Each `gspc cards` example is an authoring fragment; use its `example_path` and
`doc_paths` to load the checked full example and widget page.


## Explain an existing element when needed

For `compiler_features.scene_inspection: 1`, daemon compile methods accept
`inspect: {"name":"title"}` in the existing preview request, alongside its
selected `context`, scene `path` and edited text. The optional `inspection`
response explains source locations, component arguments/definitions, styles,
themes and initial layout-controlled geometry.

- `inspect: true` returns a compact inventory with bounded text previews to
  locate anonymous content.
- `nodeId` selects an anonymous or generated node; `objectIndex` selects an
  authored object and its expansion (use one selector at a time; `offset` and
  `limit` page results).
- Node IDs belong to the current source snapshot; verify the indexed `files`
  CRCs and JSON Pointers before editing.
- For CLI compilation, add `--inspect` to `compile`/`pack` with `--api-header`
  to include `scenes[].inspection` in `*.api.json`.

## Reuse the registered application context

For a compiler advertising `compiler_features.compile_context: 1`, Bundle
compilation with `--api-header` also emits a sibling `*.context.json`.
`gsp_add_bundle()` and `gsp_add_backend()` index those sidecars in the selected
build directory. Use the existing doctor report, or refresh it with
`gspc doctor <project> --build <build-dir>` after tool, project or build metadata
changes. Choose the observed context whose registered scenes belong to the task.
Multiple contexts represent separate Bundles; keep their scene sets and settings
distinct.

The context records the effective target Profile, codec/cache choices, all
slot and project limits, compiler passes, component inputs, registered scene
order and generated artifact paths. It is a build snapshot, not an extra
project configuration to maintain. Reconfigure/build after changing the owning
CMake or sdkconfig. A pending context needs its owning Bundle built; invalid
context metadata must be resolved before using it.

For a check without artifact generation, run
`gspc diagnose --context <selected-bundle.context.json>`. A target-configured
compile/preview or owning build already supplies diagnostics for its input;
reuse those results when generating the Bundle or previewing the same edit.

With no positional scenes, this validates the registered application, shared
resources and project capacity ceilings. Add specific scene paths for a scoped
check. Context options cannot be mixed with manual Profile/capacity overrides.
Daemon compile methods and `diagnostics/pull` at `level: "compile"` accept the
same `context` path; `compile/build` and `compile/pack` can use its scene set.
For `compile/preview`, also supply the affected scene `path` and optional edited
`text`. Schema-only diagnostics remain a quick structural check.

If the selected compiler does not advertise this feature, use its existing
explicit Profile/target arguments and the owning build; do not guess target
capabilities or require an unavailable new command. Likewise, use
`gspc cards --list` when `doctor.daemon_methods` includes `cards/list`; otherwise
query a known widget's card and the selected version's documentation.

## Release unused compiler images

A long-lived compiler daemon advertises `cache/clearImages` in `daemon_methods`.
Use it when cached encodings are no longer needed to release their memory while
keeping unsaved edits and the last-good preview. Subsequent image use re-encodes.
