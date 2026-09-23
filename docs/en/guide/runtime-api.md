# Runtime and generated API

## Start from generated configuration

`gsp_<symbol>_config()` supplies bundle bytes, component directories and JSON-derived requirements. Pair it with `ESP_GSP_ESP_LCD_CONFIG_INIT()` and a BSP-created display target, then call `esp_gsp_esp_lcd_start()`.

```c
esp_gsp_config_t app = gsp_bundle_config();
esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd.display = display;
lcd.touch = touch;

esp_gsp_handle_t ui;
ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
```

## Generated object operations

GSPC generates operations only when the Widget and authored dynamic properties support them. A Slider with `name` and `fg_color` generates `get_value()` and `set_value()`; a named Label with a text `bind` generates a text setter; callbacks produce event predicates. Inspect the sidecar `*.api.json` next to the generated header, or the exact signatures shown on each [Widget example page](../components/index.md).

Setters are asynchronous in ESP-IDF. Success confirms queue admission; the render task commits changes and schedules redraws. Use `esp_gsp_flush()` for deterministic tests, captures and explicit synchronization boundaries.

Slider and draggable Arc controls support `value → call` and `release → call` events
for value changes and release commits. Cast `event->arg` to `int32_t` to read the
value in authored `min/max` units; unchanged values do not notify again. A bare
`callback` fires on release. Add a value `bind` to enable Arc dragging; see the
[workflow guide](workflow.md) for examples.

Carousel callbacks run after the selected index is committed. Read that index
with the generated `get_selected()` or `get_value()` getter. Their CALL
`event->arg` remains the authored argument, not the selected index.

## Events and task context

Event, list-binding, image-release, timer and Canvas callbacks run on framework tasks. Do not block, access slow storage or perform networking inside them. Send a queue item or task notification to product logic and return.

Use `callback` for application-owned Back behavior and `stack_pop` for a
StackView. The authored `back` action is reserved and is not dispatched by the
current public runtimes.

```text
UI event → short callback → application task → product state change
                                           → generated setter → render commit
```

### Timer lifecycle

`esp_gsp_timer_cb_t` runs on the render task. With the ESP-IDF runtime attached,
an application task may create or delete a timer (never from an ISR); keep the
application and runtime alive while doing so. Deletion stops future scheduling,
but does not wait for a callback that has already been selected, so keep
`user_ctx` alive until the callback has returned. An external task using the same
continuously attached runtime may wait for that callback only after a successful
delete, followed by a successful `esp_gsp_flush()`; release `user_ctx` only then.
A failed delete or failed/timed-out flush does not establish callback completion.
Do not flush from a timer callback or hold a lock needed by the callback while
waiting. If other timers or asynchronous work share `user_ctx`, wait for those
users too before freeing it. Without
the ESP-IDF adapter, serialize timer operations with app stepping. A deleted
timer handle must not be reused; timer slots may be reused internally.

## Component state without polling

Register `esp_gsp_on_component_event()` to observe named PageFlow and Drawer
components. The separate callback leaves `esp_gsp_on_event()` and its event
layout unchanged. Match `event->key` with the generated `GSP_OBJ_KEY_*` constant.
`VALUE_CHANGED` identifies a committed page/open value; `MOTION_FINISHED` also
reports a return to the original value. `MOTION_CHANGED` reports changes of
target, dragging or settling, not every pixel of motion. Flags may be combined.

Read `event->state.value` for the committed value and `target` for the requested
destination. For a Drawer, these are 0 (closed) and 1 (open). A false `is_open()`
alone does not mean that an opening animation has finished. Use
`esp_gsp_component_get_motion()` when an initial snapshot is needed. Existing
getters retain their meaning; stop-anywhere PageFlow selection can change during
coasting, before motion finishes.

Notifications are coalesced once per component per UI step, after driver and
transform updates. They are not a history of every intermediate command and do
not certify LCD presentation. Registration emits no initial event, and scene
teardown drops pending notifications. Callbacks may issue setters, but must not
flush, block or destroy the app. Keep callback context alive through external
unsubscription and successful synchronization, as with other queued callbacks.

## First-frame initialization

Use `esp_gsp_esp_lcd_start_prepared()` to install bindings, event callbacks and
timers on the render task before the first frame. It uses the same prepare
callback type as session startup; passing NULL preserves ordinary start behavior.
The callback's clock is initialized before creating timers. Preparation has no
error return: check setter results in your callback and communicate application
errors through your context. Do not call flush, stop or session lifecycle APIs
from prepare.

## Visibility, input and Drawer edges

Visibility controls display. An authored `enabled` property controls acceptance
of new input; it does not prohibit programmatic navigation. Author `enabled:true`
on a PageFlow/Drawer that the app needs to enable or disable dynamically. Their
structural gesture candidates now honor that property, including closed-Drawer
edge pulls. A visible open/closing Drawer retains its modal tap barrier while
disabled; a hidden Drawer does not steal edge scrolls or block underlying taps.

Closing an enabled Drawer intentionally leaves edge reopening available. To
disable that entrance, disable the Drawer; hiding it is no longer needed merely
to suppress edge pulls. This does not cancel an already owned gesture.
Ordinary hit actions retain nearest-authored enabled inheritance (an explicitly
enabled child can override its parent). Structural gestures use the component's
own enabled property; do not rely on a parent's setting to gate them.

`block_scene_swipe` still means scene navigation only. A drawn rectangle is not
a universal gesture barrier. While displaying an application modal overlay,
disable the underlying PageFlow explicitly and restore it from the completion
event. For an application-owned top-level input surface, the existing
`esp_gsp_set_input_interceptor()` runs before scene hit testing and gesture
routing; consume the entire pointer sequence, not just its down sample.

## Dynamic content

| Content | API path |
|---|---|
| Named scalar/text/visibility/selection | Generated object helper |
| Application-owned rows or cells | List/Grid/Wheel/Message List adapter |
| Occasional encoded image | Generated image setter or `esp_gsp_set_image*()` |
| Camera/video/continuous pixels | Canvas frame or direct-draw callback |
| Page or stack navigation | Generated navigation/component helper |

Follow the COPY/BORROW/TAKE buffer, callback-context and `esp_gsp_stop()` rules in
[Lifecycle and threading](lifecycle.md#image-and-canvas-ownership). Reuse or free a BORROW
buffer only after its release callback; stopping a producer does not return
buffers it has already submitted.

## Shut down cleanly

Application code owns external producers and should stop them before stopping the UI. Do not call blocking teardown from a framework callback. See the detailed [application lifecycle](lifecycle.md) and [API reference](../reference/api.md).

## Public error codes

Generated setters and `esp_gsp_*` return the existing `esp_gsp_err_t`. Do not invent another error type.

| Macro | Value | Meaning |
|---|---|---|
| `ESP_GSP_OK` | `0` | Accepted (queued, not rendered) |
| `ESP_GSP_FAIL` | `-1` | Unclassified failure |
| `ESP_GSP_ERR_NO_MEM` | `0x101` | Allocation failed |
| `ESP_GSP_ERR_INVALID_ARG` | `0x102` | Invalid argument |
| `ESP_GSP_ERR_INVALID_STATE` | `0x103` | Call is not valid in the current state |
| `ESP_GSP_ERR_INVALID_SIZE` | `0x104` | Buffer or capacity is too small |
| `ESP_GSP_ERR_NOT_FOUND` | `0x105` | Object, bind, or resource does not exist |
| `ESP_GSP_ERR_NOT_SUPPORTED` | `0x106` | The widget or configuration does not support this operation |
| `ESP_GSP_ERR_TIMEOUT` | `0x107` | Queue or stop timed out; the handle remains valid and can retry |


## Colors and press feedback

`esp_gsp_component_set_color()` / `get_color()` retain native scene-color semantics: red is `0xF800` in RGB565 and `0xFF0000` in RGB888. Use `esp_gsp_component_set_color_rgb888()` / `get_color_rgb888()` when callers use `0xRRGGBB` consistently. Existing JSON colors and generated property APIs keep their semantics.

`esp_gsp_set_press_feedback_enabled(gsp, false)` disables the default press shade while retaining clicks and widget-specific interaction. The render task processes this command and clears an existing shade; pass `true` to restore feedback.

## Effective visibility

Regenerate named static scene objects with GSPC to use `gsp_<scene>_<object>_get_effective_visible()`. It calls `esp_gsp_query_visibility()` and checks the active scene, ancestor visibility states, dynamic dimensions and transformed layout bounds against the screen/ancestor viewport. This covers objects moved outside a PageFlow viewport or inside a closed Drawer.

The result covers layout visibility rather than pixel occlusion or interaction
availability. Transparent pixels, unrelated siblings and modal scrims are
outside the calculation. Use `get_visible()` for the authored visibility state
and instance APIs for template instances. Query from a serialized application
callback, like the other state getters.

## Drag snapshot diagnostics

Include `esp_gsp_debug.h` and call `esp_gsp_drag_snapshot_stats()`. `attempts` counts opportunities, `started` counts successful starts, `reused` counts reused list endpoint windows, and `active` reports an active drag snapshot. `skipped[]` distinguishes disabled policy, invisibility, no movement, unsupported capability, retry suppression, backend resource limits and other backend errors. `last_backend_error` retains the backend error code.

These counters are separate from scene-transition `esp_gsp_transition_stats()`. Live rendering remains the fallback. Read counters from a serialized app callback and compare deltas around the operation being diagnosed.
