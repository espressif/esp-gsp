# GUI Editor

ESP-GSP Studio is a standalone visual editor for ESP-GSP projects. It provides
a canvas-based design surface, real-time WASM preview, property inspection,
structural editing and agent collaboration.

## Download

- Windows x64 - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-win-x64.exe>
- Linux x64 AppImage - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-linux-x86_64.AppImage>
- Linux x64 DEB - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-linux-amd64.deb>
- Collaboration scripts - <https://dl.espressif.com/AE/gsp/gsp-editor/ESP-GSP%20Studio-0.12.0-scripts.zip>
- SHA-256 checksums - <https://dl.espressif.com/AE/gsp/gsp-editor/SHA256SUMS-0.12.0.txt>

## Open a project

Use **Open Project Directory** (打开工程目录) to open any GSP project directory.
Opening only discovers source files and build information; it does not create
files, copy resources or migrate sources. The directory can have existing editor
metadata under `.gsp-editor/`, or none at all.

Scene JSON remains directly compilable by GSPC. Resource paths are relative to
the original scene file. Editor metadata lives exclusively under `.gsp-editor/`
and can be removed without affecting native sources.

## Feature overview

- **Canvas editing** — drag, resize and visually arrange widgets on a design
  surface. Selection, multi-select and alignment tools are available.
- **Property inspector** — edit widget properties in a structured panel.
  Property origins distinguish local values, styles and themes.
- **Real-time preview** — the bundled WASM simulator renders the current scene
  after each change. Use **Simulate / F5** for animation playback.
- **Scene management** — create, rename, delete and reorder scenes from tabs.
  Tab drag or Alt+Left/Right changes the build order.
- **Undo / redo** — each scene maintains its own undo history.
- **Auto-save** — visual edits are saved after 500 ms of inactivity; pointer
  gestures save when they finish.
- **External change detection** — filesystem changes are reconciled
  automatically. Conflicts show both versions.
- **Draft recovery** — unsaved drafts are preserved under `.gsp-editor/drafts/`
  and offered for recovery on next launch.
- **Component editing** — edit shared components in-place, copy to create
  independent variants, or detach with compiler verification.
- **Asset management** — import images, fonts and video with compiler
  validation. Replacement preserves existing layout dimensions.
- **Export** — continue in ESP-IDF, export a rebuildable standalone UI
  directory, or export precompiled resources for a firmware Bundle.

## Bundled toolchain

ESP-GSP Studio 0.12.0 ships with GSP/simulator 1.10.1 and GSPC 0.11.1. The
application version is independent of its bundled toolchain versions. The
compiler, schema, WASM runtime and export use this fixed toolchain; importing
or switching toolchains is not available in this release.

Project settings → **Bundled Toolchain** (随包工具链) shows the actual
versions, platform and verification status. Native project compatibility uses
the required compiler version and format/ABI contracts. Unsupported native
requirements disable content editing but leave source and recovery accessible.

The bundled toolchain uses configuration ABI 4; see the
[compatibility contract](../reference/compatibility.md).

## Build context

For projects with an existing ESP-IDF build, open project settings →
**Compilation and Preview** (编译与预览配置) and select a build directory and
Bundle. The editor discovers contexts through `gspc doctor <project> --build
<build-dir>`. This provides scene order, Profile, component directories,
compiler passes and capacity limits.

Without a build context, **Manual Preview** (手动预览配置) allows explicit
preview and export settings. This mode is labeled in the toolbar and does not
claim to reproduce the device build.

## Next steps

- [Basic operations](operations.md) — common editing workflows.
- [Agent collaboration](collaboration.md) — use CLI or MCP to hand off editing
  to an AI agent.
