# Troubleshooting

Start with the earliest failing layer: component resolution, scene generation,
generated API compilation, target linking, runtime startup, input, or visual
output. A later symptom often cannot identify an earlier configuration error.

## Component or tool environment

### `gsp_add_bundle: standalone GSPC is required`

The preferred path is to install the toolchain manager and let CMake resolve
the version in `.gspc_version`:

```sh
python -m pip install -U esp-gsp-tools
idf.py build
```

The `.gspc_version` in the IDF project root overrides the compiler-version marker
shipped by the component. To select another compiler version, create the project
marker manually. If no usable marker exists, set `GSPC_EXECUTABLE` explicitly.
If `esp-gsp-tools` is unavailable, download
supported host tools from the [ESP-GSP Releases page](https://github.com/espressif/esp-gsp/releases),
verify the published checksum, and set `GSPC_EXECUTABLE` to the executable.

`GSPC_EXECUTABLE` is independent of the ESP-GSP component version. CMake runs
the GSPC compatibility command (`gspc compatibility`, equivalent to
`"$GSPC_EXECUTABLE" compatibility`) and validates the generated formats,
requirements, configuration contract and GMD runtime ABI before adding build
rules. A compiler without that command, or with any incompatible value, is
rejected during project configuration. The current generated values are listed
in the [Compatibility contract](../reference/compatibility.md).

### The native simulator does not start

Run `gsp_sim --capabilities` from a terminal so loader errors remain visible.
Use the archive for the exact host platform and install only the runtime
dependencies listed for that release. If native dependencies are unavailable,
use the browser/WASM simulator package. Simulator startup validates the host
runtime, not display hardware.

### The wrong ESP-GSP copy is used

Delete stale project dependency state only through the normal ESP-IDF
dependency workflow, then reconfigure. Check `build/project_description.json`
to see the component path selected by the current build. Do not assume that a
nearby component directory overrides a managed component automatically.

## Scene generation

### JSON reports an unknown field or component

Use [Scene authoring](../reference/authoring.md) for the installed component version. Do
not borrow fields from LVGL or from a newer ESP-GSP release. Check the full
JSON path in the compiler diagnostic; repeated templates and component
configuration can make the leaf field appear far from the root cause.

### A scene exceeds a capacity

The build passes selected target capacities to the compiler. Compare the
reported required and configured values, then either reduce simultaneous
scene demand or increase the matching Kconfig/runtime pool. Size recycled
Lists and Grids from visible rows plus overscan, not total dataset size.
For runtime-created templates, set `max_instances` to the maximum number of
simultaneously live copies.

### An image or font is not found

Asset paths are relative to the scene JSON, not the project root or build
directory. Keep assets beside the scene or use a correct relative path. A
scene or referenced asset change should trigger regeneration automatically.

## Generated API

### A generated helper is missing

1. Give the element a stable `name`.
2. Confirm that the requested property is dynamic for that element type.
3. Add `callback` when an event predicate is required.
4. Rebuild the bundle.
5. Inspect `<symbol>_gsp.h` and the included per-scene header.

Do not hand-write a guessed helper and do not edit the generated header.

### An old generated helper still appears

Confirm that the edited JSON is one of the `SCENES` passed to the active
`gsp_add_bundle()` call and that the application includes the header for the
same `SYMBOL`. Reconfigure when component or CMake ownership changed.

## Display output

### The scene does not match the panel

Check these values together:

- scene `w` and `h`;
- logical panel width and height after rotation;
- `gsp_add_bundle(PIXEL_FORMAT ...)`;
- display target pixel width and byte order;
- BSP physical rotation and framebuffer configuration.

For 90° or 270° physical rotation on a non-square display, the logical scene
dimensions are transposed. Application buffers remain in the native ESP-GSP
pixel layout; configure panel byte swapping in the BSP instead of pre-swapping
pixels.

### Colors are swapped or incorrect

ESP-GSP uses little-endian RGB565 or packed B, G, R bytes for RGB888 runtime
surfaces. Verify the panel's wire byte order separately. A panel that needs
big-endian RGB565 should set display-target byte swapping.

### Updates tear or only part of the screen changes

Verify that the BSP exposes the correct panel class, framebuffer count, TE
signal, draw-buffer memory, and completion callbacks. Keep presentation mode
on `ESP_DISPLAY_PRESENT_MODE_AUTO` unless a measured integration requires an
explicit mode. See [Display presentation](display.md).

## State and lifecycle

### A setter succeeds but the display has not changed yet

Setters are asynchronous on ESP-IDF. Success means the update entered the
framework command path. Use `esp_gsp_flush()` only when a test, screenshot, or
other deterministic boundary must observe the committed result.

### A getter returns the previous value

Getters observe committed state. A setter issued by another task may still be
queued. Use one application state owner or an explicit low-frequency flush
when a deterministic read-after-write boundary is required.

### Shutdown times out or a buffer is still owned

Call `esp_gsp_stop()` from an application task, never from a framework
callback. Canvas stop is asynchronous; flush before freeing Canvas callback
context or accepted borrowed frames. For runtime images, wait for the release
callback defined by the selected COPY/BORROW/TAKE ownership model.

## Input

### Touch is unresponsive

Confirm that:

- `lcd.touch` is assigned before startup;
- the touch controller reports coordinates in the configured logical
  orientation;
- the target is visible and not covered by a later object;
- a child control or viewport has not legitimately captured the gesture;
- explicit interrupt mode has a usable, unclaimed interrupt source.

AUTO input mode falls back to polling when interrupt setup is unavailable.
Explicit interrupt mode remains strict.

### Pinch does not update an image

Pinch is application-owned. Register `esp_gsp_on_pinch()`, return `true` from
`ESP_GSP_PINCH_BEGIN` to capture the gesture, and apply the reported relative
scale through the generated image scale helper. ESP-GSP reports the gesture;
it does not automatically zoom or pan a control. `MAX_TOUCH_POINTS=1` disables
the second contact and pinch handling.

## Lists, grids, and runtime media

### List binding eventually returns `ESP_GSP_LIST_NONE`

Bind each logical List or Wheel once and retain the handle for the UI lifetime.
Do not bind again on every scene-ready event. Update item count with the
generated `set_total()` helper and notify visible-data changes with the
generated `refresh()` helper.

### A recycled row shows stale asynchronous content

Do not retain a row handle after the binder returns. Associate asynchronous
results with the row token and publish only if it still identifies the same
list instance and item. Use stable image `cache_key` values only for immutable
encoded content.

### A runtime image stays blank under memory pressure

Check the decoded cache budget, largest allocatable block, active dynamic image
slots, and whether image caching was disabled. A no-PSRAM build may select a
bounded regional decode path that does not support runtime PNG/JPEG
replacement. See [Configuration](../reference/configuration.md) and
[Media and application data](media-and-data.md).

## Collect useful evidence

When reporting a problem, include:

- ESP-GSP component version and selected component path;
- ESP-IDF version and `IDF_TARGET`;
- scene file, logical resolution, and pixel format;
- panel interface, rotation, framebuffer count, and TE configuration;
- the first compiler/runtime error, not only later cascading messages;
- whether the result was reproduced in host preview, target build, real board,
  or visual inspection.
