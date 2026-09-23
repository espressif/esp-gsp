# Public API Map

Ordinary applications should include the generated `<symbol>_gsp.h` header.
It is the exact API for the scenes in that bundle: scene identifiers, event
decoders, template descriptors and typed functions for named controls are all
derived from the authored JSON.

## Recommended layers

| Layer | Header | Use |
|---|---|---|
| Generated bundle | `<symbol>_gsp.h` | startup configuration and the normal typed application API |
| Application facade | `esp_gsp.h` | dynamic media, lists, navigation, timers and data-driven integration |
| ESP-LCD lifecycle | `esp_gsp_esp_lcd.h` | start, suspend, temporary-app handoff, resume and session destruction |
| Diagnostics | `esp_gsp_debug.h` | counters, timing, media telemetry and injected test input |
| Framework integration | `esp_gsp_advanced.h` | access to the low-level rendering context; not an ordinary application path |

## Application facade groups

The declarations and ownership comments in `esp_gsp.h` are authoritative.
This map identifies the intended groups without duplicating every prototype.
The [function reference](api-functions.md) lists every application-level
public function, its exact declaration, return type, header and source
contract.
Important configuration, event, image, collection, Canvas, and descriptor
members are explained in the
[Application structure guide](../guide/application-structures.md).

| Group | Main entry points |
|---|---|
| Synchronization and errors | `esp_gsp_flush()`, `esp_gsp_render_error_stats()` |
| Generic state | `esp_gsp_set_*()`, `esp_gsp_get_*()` |
| Components | `esp_gsp_component_get_*()`, `esp_gsp_component_set_*()`, batch, position and animation APIs |
| Navigation | `esp_gsp_goto_scene()`, swipe policy, PageFlow, StackView and Drawer APIs |
| Events and input | `esp_gsp_on_event()`, `esp_gsp_on_pinch()`, `esp_gsp_set_pointer_observer()` |
| Runtime images | `esp_gsp_set_image*()` COPY, BORROW and TAKE variants |
| Lists and wheels | bind, refresh, snap, fling, scroll, fade and row-publishing APIs |
| Canvas | full/dirty frame submission, direct region drawing, and `esp_gsp_canvas_stop()` |
| Template widgets | create, destroy, content, visibility, position and animation APIs |
| Keyboard | `esp_gsp_keyboard_attach()`, `esp_gsp_keyboard_text()` |
| Timers and animation | render-task timers, value/color/property/position animation APIs |

Setters are asynchronous on ESP-IDF. COPY/BORROW/TAKE ownership, callback
context, synchronization and shutdown rules are defined in
[Application Lifecycle and Threading](../guide/lifecycle.md).

`esp_gsp_on_pinch()` reports an optional application-owned two-contact
gesture. The callback must accept `BEGIN` before the framework captures it;
the application remains responsible for applying scale or position changes.

## Generated API

Generated functions follow this form:

```text
gsp_<scene>_<control>_<operation>()
```

The available operations depend on the named component and its dynamic
properties. Do not infer a helper from another component: rebuild the bundle
and inspect `<symbol>_gsp.h`.

### Dropdown selection

Give a Dropdown a stable `name`; `options` defines the index-to-text mapping
and `selected` defines its initial index:

```json
{
  "type": "dropdown",
  "parent": 0,
  "name": "display_mode",
  "x": 20, "y": 20, "w": 180, "h": 40,
  "options": ["Standard", "Vivid", "Reading"],
  "selected": 0
}
```

Restore an application-owned value, such as one read from NVS, through the
generated helper:

```c
ESP_ERROR_CHECK(gsp_settings_display_mode_set_selected(ui, saved_mode));
```

The setter rejects an index outside `options`, then updates both the retained
selection and the displayed option text. A user selection reports the same
zero-based index in the Dropdown callback argument; persist that argument.
Do not overlay another label or access compiler-generated `__...` objects.

Define `GSP_BUNDLE_ENABLE_RAW_IDS` before including the generated header only
when an advanced integration needs raw bind, action, object, property or
template identifiers. Generated typed functions remain the preferred path.

## Keyboard flow

A compiled `keyboard` emits one generated key callback. Attach it to the bind
of an authored text input:

```c
#define GSP_BUNDLE_ENABLE_RAW_IDS 1
#include "bundle_gsp.h"

ESP_ERROR_CHECK(esp_gsp_keyboard_attach(
    ui, GSP_MAIN_ACT_ID_SEARCH_KEY, GSP_MAIN_BIND_SEARCH_TEXT));
```

Character and delete keys update the framework-owned edit buffer. The
application receives the OK action after the final text is already available
through `esp_gsp_keyboard_text()`. Use the corresponding generated action and
bind symbols from the scene header rather than copying the example names.

## Diagnostics

`esp_gsp_debug.h` is optional instrumentation, not application state. It
provides frame, render, transition, region, service and media statistics plus
test input injection. Production decisions should use application state and
normal error returns rather than diagnostic counters.

`esp_gsp_update_error_stats()` also works without profiling. It records failures
from component batches and their typed wrappers, including `set_position()`.
Check `stage` to distinguish rejection before submission from a queued update
that failed to apply or was dropped after a scene change. `component`,
`property` and `entry_index` identify a rejected entry when available; a
batch-wide failure uses `UINT32_MAX` for the index. Existing setter return
codes and `flush()` behavior are unchanged: a completed render attempt does
not imply that every update succeeded.

For `PROPERTY_UNAVAILABLE`, check the authored dynamic declaration first.
For example, `set_position()` needs both axes declared as bounded values:

```json
"x": {"default": 20, "min": 0, "max": 200},
"y": {"default": 30, "min": 0, "max": 160}
```

If only x is dynamic, use the generated `set_x()` instead. Unknown/unsupported
properties and literal properties both have no runtime entry; consult the
component schema or `esp_gsp_component_get_property_info_at()` to inspect the
available runtime properties. The diagnostic preserves the last failure across
successful updates, so compare `failures` with the previous snapshot. Concurrent
access is non-blocking: a busy query returns false without changing its output,
and `details_dropped` counts failures whose details could not be retained.
