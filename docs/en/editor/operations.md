# Basic Operations

This guide covers everyday editing workflows in ESP-GSP Studio.

## Creating a scene

1. Click the **+** button in the scene tab bar, or use the menu
   **File → New Scene**.
2. Enter a file name. New scenes default to the `scenes/` directory.
3. Set the scene dimensions and background in the right-side **Style → Scene
   Settings** (样式 → 场景设置) panel.

Scenes are ordinary JSON files. The editor writes them directly; no
intermediate project format is involved.

## Adding widgets

- From the widget palette, drag a widget onto the canvas.
- Or right-click the canvas / object tree and choose **Add** from the context
  menu.

Widgets are placed at the drop position. Use the property inspector on the
right to fine-tune position (`x`, `y`), size (`w`, `h`) and all type-specific
fields.

## Selecting and transforming

- **Click** to select a single widget.
- **Ctrl+Click** or drag a marquee to multi-select.
- Drag selection handles to resize.
- Drag the widget body to reposition.
- Use the **object tree** on the left to navigate the hierarchy, reorder
  siblings or change parent relationships.

## Editing properties

The right-side panel shows every editable field for the selected widget.
Fields are grouped by category.

- **Name** — set a stable `name` for elements the application must address at
  runtime. The compiler generates typed C helpers from this name.
- **Callback** — set `callback` for actions that must reach the application.
- **Style / theme** — style edits create local overrides. Clearing a style
  override restores the inherited value.

All property changes are immediately reflected in the WASM preview.

## Managing assets

- **Reference an existing file** — click the **↗** icon next to an asset field
  to reference a file already on disk, including files outside the project.
- **Import a copy** — click **+** to copy a file into the project's `assets/`
  directory. Imports are validated by the compiler before being exposed.
- **Replace** — replacing an image preserves the existing layout dimensions.
  Use the explicit fit-to-source action to update dimensions from the new file.

Supported asset types include raster images (JPEG, PNG), SVG, bitmap
animations, fonts (TTF/OTF) and video files (MP4, AVI, MOV, WebM — requires
FFmpeg for compilation).

## Scene settings

Open the right-side **Style → Scene Settings** panel:

- **Dimensions** (`w`, `h`) and **background** (`screen_bg`).
- **Swipe policy** and **default font / font size**.
- **Advanced settings** (collapsed by default): extra character corpus, UTF-8
  corpus file, font byte budget and top-level actions.

Project defaults apply only when creating new scenes; explicit control font
settings take priority over scene defaults.

## Undo and redo

Each scene has its own undo stack. Use **Ctrl+Z** / **Ctrl+Shift+Z** (or
**Cmd** on macOS) for undo and redo. Clean scene changes enter undo history.
External changes that conflict with local edits pause auto-save and present
both versions.

## Preview and simulation

- The canvas provides a live WASM preview after each edit.
- Press **F5** or use the menu **Simulate** (仿真) to open the full
  interactive simulator with animation playback.
- For headless or CI preview, use the standalone simulator as described in
  [Simulator preview](../configuration/simulator-preview.md).

## Exporting

Open the **Export** (导出) panel for three delivery paths:

1. **ESP-IDF integration** — continue with an existing ESP-IDF application.
   The generated interaction API and C headers are produced by the owning
   CMake build.
2. **Standalone UI directory** — a rebuildable directory with scene inputs,
   the matching compiler, licenses and a `rebuild.cjs` script. Run it with
   Node.js 22+ on the recorded OS/architecture.
3. **Precompiled package** — a deployable Bundle for a selected firmware
   context. The manifest records toolchain, runtime contracts, Profile,
   capacities and scene order.

Compile results remain visible and include a file manager entry. The most
recent export result is saved per project.

## Keyboard shortcuts

| Action | Shortcut |
|---|---|
| Undo | Ctrl+Z |
| Redo | Ctrl+Shift+Z |
| Save | Ctrl+S |
| Simulate | F5 |
| Delete selected | Delete |

## Tips

- **Auto-save** triggers 500 ms after the last edit. Pointer gestures (drag,
  resize) save when they finish.
- **File watching** reconciles external changes after 250 ms, with a 5-second
  polling fallback and focus reconciliation.
- Removing the `.gsp-editor/` directory does not affect source files. The
  project can be reopened without editor metadata.
