# Public Function Reference

This page covers the application-level public functions shipped in the
firmware component. Ordinary applications include generated `<symbol>_gsp.h`
first and call these functions only for runtime capabilities not represented
by a generated typed helper.

Declarations, return types and contracts come from the public headers. No entry
is ISR-safe unless its contract says so. Follow each group's callback, ownership
and synchronization rules.

See the [API map](api.md), [Lifecycle and threading](../guide/lifecycle.md), and
[Application structures](../guide/application-structures.md) for complete usage flows.

## Lifecycle and synchronization

Create platform-owned instances through the platform adapter. Stop and flush calls define the application synchronization boundary.

### `esp_gsp_config_init()`

Use the config init operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_config_t`

```c
static inline esp_gsp_config_t esp_gsp_config_init(void);
```

### `esp_gsp_config_set()`

Sets one per-instance runtime policy or capacity. Project-wide defaults remain in Kconfig; GSPB scene requirements are applied automatically. Read-only ESP_GSP_BUILD_CAP_* fields are rejected.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_config_set_result_t`

```c
static inline esp_gsp_config_set_result_t esp_gsp_config_set(esp_gsp_config_t *config, esp_gsp_config_field_id_t field, uint64_t value);
```

### `esp_gsp_stop()`

Stops a platform-created UI instance. Creation belongs to a platform backend, for example the ESP-IDF LCD backend or the SDL adapter on host.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stop(esp_gsp_handle_t gsp);
```

### `esp_gsp_flush()`

Wait until all commands submitted before this call have been applied and the display backend has completed the following render attempt.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_flush(esp_gsp_handle_t gsp, uint32_t timeout_ms);
```

**Contract details**

This is a low-frequency synchronization fence for tests, screenshots and
orderly application state changes. Normal UI updates should remain
asynchronous. It must not be called from a render- or decode-task callback.

Parameter `timeout_ms`: Maximum total wait, including command queue admission.
Zero performs a non-blocking check/submit.
A timeout does not cancel an accepted fence.

### `esp_gsp_render_error_stats()`

Cumulative render failures and the most recent engine error. The first frame is strict: esp_gsp_start() returns an error instead of publishing a handle when it cannot be rendered. Runtime failures remain recoverable and are counted here rather than silently discarded.

- **Header:** `include/esp_gsp.h`
- **Return type:** `void`

```c
void esp_gsp_render_error_stats(esp_gsp_handle_t gsp, uint32_t *out_failures, gsp_err_t *out_last_error);
```

## Generated state and keyboard

Bind-based setters enqueue work for the render task. Getters return the last committed value; use generated typed wrappers in ordinary code.

### `esp_gsp_set_value()`

Set value.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_value(esp_gsp_handle_t gsp, uint16_t bind, int32_t value);
```

### `esp_gsp_set_color()`

Color in the scene's native pixel format (RGB565 scenes take 16-bit colors, RGB888/ARGB8888 scenes take 24/32-bit).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_color(esp_gsp_handle_t gsp, uint16_t bind, uint32_t color);
```

### `esp_gsp_set_visible()`

Set visible.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_visible(esp_gsp_handle_t gsp, uint16_t bind, bool visible);
```

### `esp_gsp_set_text()`

Shapes UTF-8 on the render task. The string is copied before return; short values stay inline in the command and longer values use temporary framework-owned storage.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_text(esp_gsp_handle_t gsp, uint16_t bind, const char *utf8);
```

### `esp_gsp_get_value()`

Get value.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_value(esp_gsp_handle_t gsp, uint16_t bind, int32_t *out_value);
```

### `esp_gsp_get_color()`

Get color.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_color(esp_gsp_handle_t gsp, uint16_t bind, uint32_t *out_color);
```

### `esp_gsp_get_visible()`

Get visible.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_visible(esp_gsp_handle_t gsp, uint16_t bind, bool *out_visible);
```

### `esp_gsp_get_toggle()`

Logical state of a toggle's value bind (the knob tweens 0..100; >= 50 reads as ON so a mid-animation read reports the target side). Checkbox/radio store plain 0/1 — read those with esp_gsp_get_value.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_toggle(esp_gsp_handle_t gsp, uint16_t bind, bool *out_on);
```

### `esp_gsp_keyboard_attach()`

Use the keyboard attach operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_attach(esp_gsp_handle_t gsp, uint16_t action_id, uint16_t text_bind);
```

### `esp_gsp_keyboard_attach_ex()`

Extended attachment with an application-selected UTF-8 byte limit. Storage is allocated once during attachment and reused for editing. max_bytes excludes the trailing NUL and may use the available address space; allocation failure is reported as ESP_GSP_ERR_NO_MEM. It is ignored when action_id is ESP_GSP_KEYBOARD_NONE.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_attach_ex(esp_gsp_handle_t gsp, uint16_t action_id, uint16_t text_bind, size_t max_bytes);
```

### `esp_gsp_keyboard_text()`

Copies the attached keyboard's current text (NUL terminated). Returns ESP_GSP_ERR_INVALID_SIZE when capacity is too small; in that case out_text still contains a valid UTF-8 prefix.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_text(esp_gsp_handle_t gsp, char *out_text, size_t capacity);
```

### `esp_gsp_keyboard_text_size()`

Returns the buffer size, including the trailing NUL, required by esp_gsp_keyboard_text().

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_text_size(esp_gsp_handle_t gsp, size_t *out_size);
```

### `esp_gsp_set_cursor()`

Set cursor.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_cursor(esp_gsp_handle_t gsp, uint16_t bind);
```

## Named components

Component APIs address generated stable object and property keys. Typed generated wrappers remain the preferred application surface.

### `esp_gsp_component_get()`

Get component.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get(esp_gsp_handle_t gsp, gsp_component_key_t key, gsp_component_prop_kind_t prop, gsp_value_t *out_value);
```

### `esp_gsp_component_set()`

Set component.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set(esp_gsp_handle_t gsp, gsp_component_key_t key, gsp_component_prop_kind_t prop, const gsp_value_t *value);
```

### `esp_gsp_component_get_property()`

Open semantic-key API for component-local properties. Generated typed wrappers hide the key and tagged value in ordinary application code.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_property(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, gsp_value_t *out_value);
```

### `esp_gsp_component_get_property_info()`

Get component property info.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_property_info(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, gsp_property_info_t *out_info);
```

### `esp_gsp_component_get_property_info_at()`

Get component property info at.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_property_info_at(esp_gsp_handle_t gsp, gsp_component_key_t component, size_t index, gsp_property_info_t *out_info);
```

### `esp_gsp_component_set_property()`

Set component property.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_property(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *value);
```

### `esp_gsp_component_set_many()`

Queues typed updates as one atomic render-task transaction. The array is copied before return and may be stack allocated. Every entry is validated before the batch is queued; repeated component/property pairs are allowed and the last value wins. Large batches use temporary framework-owned heap storage and report ESP_GSP_ERR_NO_MEM if it cannot be allocated.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_many(esp_gsp_handle_t gsp, const gsp_component_update_t *updates, size_t count);
```

### `esp_gsp_component_set_properties()`

Open-key atomic batch; same validation/copy/lifetime rules as set_many.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_properties(esp_gsp_handle_t gsp, const gsp_component_property_update_t *updates, size_t count);
```

### `esp_gsp_component_set_position()`

Atomically moves a compiled static component subtree. X/Y are authored scene pixels stored in runtime SRAM; compiled commands remain read-only.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_position(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t x, int32_t y);
```

### `esp_gsp_component_get_position()`

Reads the last committed authored position.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_position(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t *out_x, int32_t *out_y);
```

### `esp_gsp_component_translate()`

Queues a relative move from the last committed authored position.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_translate(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t dx, int32_t dy);
```

### `esp_gsp_component_get_info()`

Queries kind and available properties without reading component state.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_info(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_component_info_t *out_info);
```

### `esp_gsp_component_get_value()`

Strongly typed convenience APIs over esp_gsp_component_get/set().

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t *out_value);
```

### `esp_gsp_component_set_value()`

Set component value.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t value);
```

### `esp_gsp_component_get_color()`

Native scene color: RGB565 uses packed 16-bit values, RGB888 uses 0xRRGGBB.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_color(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t *out_native_color);
```

### `esp_gsp_component_set_color()`

Sets the component's primary color in the scene's native format.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_color(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t native_color);
```

### `esp_gsp_component_set_color_rgb888()`

Sets the primary color from 0xRRGGBB, converting to the scene format. Requires a canonical color property or a legacy color binding. For named colors such as fg_color, use the generated property-specific helper. RGB565 uses 5/6/5-bit truncation; ARGB8888 receives opaque alpha. Values above 0xFFFFFF are rejected. Existing native-color APIs are unchanged.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t rgb888);
```

### `esp_gsp_component_set_property_color_rgb888()`

Sets a named COLOR property from 0xRRGGBB with scene-format conversion. Uses the same validation, queue and transaction path as set_property(). Does not change component opacity or accept an alpha byte.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_property_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, uint32_t rgb888);
```

### `esp_gsp_component_get_color_rgb888()`

Gets the primary color as 0xRRGGBB.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t *out_rgb888);
```

### `esp_gsp_component_get_visible()`

Get component visible.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_visible(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_visible);
```

### `esp_gsp_component_set_visible()`

Set component visible.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_visible(esp_gsp_handle_t gsp, gsp_component_key_t key, bool visible);
```

### `esp_gsp_component_set_text()`

Structured content setters resolve the generated property bind by key.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_text(esp_gsp_handle_t gsp, gsp_component_key_t key, const char *text);
```

### `esp_gsp_component_set_image()`

Set component image.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_image(esp_gsp_handle_t gsp, gsp_component_key_t key, const void *data, size_t size);
```

### `esp_gsp_component_get_checked()`

Toggle on/off by key — CHECKED semantics, no bind ids, no 0..100 rail convention.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_checked(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_checked);
```

### `esp_gsp_component_set_checked()`

Set component checked.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_checked(esp_gsp_handle_t gsp, gsp_component_key_t key, bool checked);
```

### `esp_gsp_component_get_enabled()`

Enables or disables a component that opted into authored `enabled`.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_enabled(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_enabled);
```

### `esp_gsp_component_set_enabled()`

Set component enabled.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_enabled(esp_gsp_handle_t gsp, gsp_component_key_t key, bool enabled);
```

### `esp_gsp_component_play_animation()`

Play component animation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_play_animation(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *from, const gsp_value_t *to, const esp_gsp_animation_config_t *config);
```

### `esp_gsp_component_stop_animation()`

Stop component animation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_stop_animation(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property);
```

### `esp_gsp_component_stop_position_animation()`

Stops the component's synchronized position tween, if active. The component stays where the last committed step put it.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_stop_position_animation(esp_gsp_handle_t gsp, gsp_component_key_t component);
```

## Navigation and retained containers

Scene, PageFlow, StackView and Drawer operations share the runtime's retained navigation state and animation rules.

### `esp_gsp_page_flow_set_page()`

Standard PageFlow navigation. Tab clicks and pointer settle use the same driver state machine as this API.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_set_page(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t page, bool animated);
```

### `esp_gsp_page_flow_get_page()`

Get page flow page.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_get_page(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t *out_page);
```

### `esp_gsp_page_flow_get_offset()`

Current transform offset in pixels along the PageFlow axis. Unlike the selected page, this remains precise while dragging and for stop-anywhere.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_get_offset(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t *out_offset_px);
```

### `esp_gsp_page_flow_is_dragging()`

Use the page flow is dragging operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_is_dragging(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_dragging);
```

### `esp_gsp_stack_view_push()`

In-scene StackView navigation. Push/pop requests are rejected while an animation is active; the root page cannot be popped.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_push(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t page, bool animated);
```

### `esp_gsp_stack_view_pop()`

Pop stack view.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_pop(esp_gsp_handle_t gsp, gsp_component_key_t key, bool animated);
```

### `esp_gsp_stack_view_get_top()`

Get stack view top.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_get_top(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t *out_page);
```

### `esp_gsp_stack_view_is_animating()`

Use the stack view is animating operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_is_animating(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_animating);
```

### `esp_gsp_drawer_open()`

Overlay Drawer state. Gesture and Close-button actions use this same settle state machine.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_drawer_open(esp_gsp_handle_t gsp, gsp_component_key_t key, bool animated);
```

### `esp_gsp_drawer_close()`

Close drawer.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_drawer_close(esp_gsp_handle_t gsp, gsp_component_key_t key, bool animated);
```

### `esp_gsp_drawer_is_open()`

Open drawer is.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_drawer_is_open(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_open);
```

### `esp_gsp_goto_scene()`

Switches scenes (multi-scene bundles) with an optional transition. Use ESP_GSP_CROSS_FADE to blend overlapping scenes, or ESP_GSP_FADE_THROUGH_BLACK to dim the old scene fully to black before revealing the new scene. Strip displays can render fade-through-black incrementally without full-screen snapshots; cross-fade still requires both scene snapshots. If no visual path is available, the scene still switches safely. Safe from any task; also invoked automatically by `goto` actions and swipe navigation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_goto_scene(esp_gsp_handle_t gsp, uint16_t scene_id, esp_gsp_transition_t transition);
```

### `esp_gsp_set_swipe_enabled()`

Enables or disables horizontal swipe scene navigation at runtime. Multi-scene bundles enable swipes by default (`disable_swipe=false` in esp_gsp_config_t); this mirrors that flag. The default classifier locks only when horizontal travel clearly dominates vertical travel. Release commits after a one-fifth-screen pull, or after a shorter deliberate fling; tiny fast movements remain below the fling-distance guard. Safe from any task.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_enabled(esp_gsp_handle_t gsp, bool enabled);
```

### `esp_gsp_set_swipe_transition()`

Selects the visual that follows horizontal scene-swipe progress. The default is ESP_GSP_SWIPE_SLIDE. The slide-fade option moves the pages directly from gesture distance; the transition layer derives brightness independently from that same raw distance, so no effect-specific calculation lives in the touch path. Safe from any task.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_transition(esp_gsp_handle_t gsp, esp_gsp_swipe_transition_t transition);
```

### `esp_gsp_set_swipe_fade_black_point()`

Sets where a fade-through-black swipe reaches full black. drag_percent is the horizontal drag distance as a percentage of screen width (1..99). The default is 50. A smaller value dims the current scene faster; the target scene still reaches full brightness at 100%. This changes only brightness response, never page position, gesture recognition or commit thresholds. Safe from any task.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_fade_black_point(esp_gsp_handle_t gsp, uint8_t drag_percent);
```

## Runtime images

COPY, BORROW and TAKE variants differ in encoded-buffer ownership. Follow the source contract before releasing or reusing a submitted buffer.

### `esp_gsp_image_get_scale()`

Reads or updates a named scalable image's unsigned Q16.16 scale.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_image_get_scale(esp_gsp_handle_t gsp, gsp_component_key_t image, uint32_t *out_scale_q16);
```

### `esp_gsp_image_set_scale()`

Set image scale.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_image_set_scale(esp_gsp_handle_t gsp, gsp_component_key_t image, uint32_t scale_q16);
```

### `esp_gsp_set_image()`

Shows a runtime-provided encoded image (JPEG/PNG/QOI, e.g. a network download) on an image bind. The container is sniffed from the data — no metadata needed. The encoded payload is copied before this function returns and decoded by a background worker, so the caller may immediately release or reuse `data`.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size);
```

### `esp_gsp_set_image_ex()`

Complete runtime-image submission contract. NULL options select COPY. BORROW returns the immutable source through on_release. TAKE transfers the source only when this function returns ESP_GSP_OK; on immediate submission failure the caller still owns it. on_complete is called only for accepted submissions.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_ex(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size, const esp_gsp_image_options_t *options);
```

### `esp_gsp_set_image_borrowed()`

Borrowing variant of esp_gsp_set_image(). The encoded bytes are never copied; `data` must remain immutable until `on_release` is called. Replacement, cancellation, decode failure and shutdown all release the payload exactly once. `on_release` is required.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_borrowed(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

**Contract details**

Note that a published image holds its payload for as long as it is the one
on screen, not just until it decodes: the decode cache may drop the pixels
under memory pressure and re-decode from these bytes. So a borrowed buffer
comes back when the next image for that bind is adopted, or at shutdown —
budget for one outstanding buffer per image bind.

This removes only the encoded-input copy. Decode and framebuffer composition
still follow the target image/cache policy.

### `esp_gsp_set_image_owned()`

Transfers malloc-compatible encoded storage without copying. Ownership is transferred only when ESP_GSP_OK is returned; the framework calls free() once the image is replaced, cancelled or shut down — same lifetime as the borrowed variant, without the callback.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_owned(esp_gsp_handle_t gsp, uint16_t bind, void *data, size_t size);
```

### `esp_gsp_asset_image_target()`

Use the asset image target operation.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_asset_target_t`

```c
static inline esp_gsp_asset_target_t esp_gsp_asset_image_target(uint16_t bind);
```

### `esp_gsp_asset_row_target()`

Use the asset row target operation.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_asset_target_t`

```c
static inline esp_gsp_asset_target_t esp_gsp_asset_row_target(esp_gsp_row_t row, uint16_t slot);
```

### `esp_gsp_asset_widget_target()`

Use the asset widget target operation.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_asset_target_t`

```c
static inline esp_gsp_asset_target_t esp_gsp_asset_widget_target(esp_gsp_widget_t widget, uint16_t slot);
```

### `esp_gsp_assets_open()`

Open a mounted SD/NAND filesystem package using the generated open helper. index_crc comes from its generated header. A matching index takes the fast path; a replacement package (or index_crc=0) is fully CRC-checked in bounded chunks at open. Each requested member is CRC-checked when read. No whole-pack RAM copy. Call from an application task; open performs IO. File contents must stay immutable until close. Close before unmount/update. NULL config selects defaults. On failure *out_assets is NULL.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_open(const char *path, uint32_t index_crc, const esp_gsp_assets_config_t *config, esp_gsp_assets_t **out_assets);
```

### `esp_gsp_assets_show()`

Queue an image or animation for an authored Image bind. Asset names come from the generated header. Submission returns without file IO or decoding. once plays one animation cycle; false follows the authored loop count. Existing content stays visible until a complete replacement is ready. Each target has at most one in-flight frame. New requests coalesce per bind. Animation memory follows ANIM_FRAME_MEMORY / ANIM_MAX_FRAME_BYTES and is separate from the encoded read budget. JPEG animations also work with the image cache disabled; measure playback performance on the target device. The target placeholder must match the exported pixel/alpha form and native dimensions (opaque image-fit binds can vary dimensions). Export to the same RGB565/RGB888 profile as the UI. Show targets the active scene. Switching scenes cancels subsequent frames; resubmit when returning to that scene. Hiding a page/group does not stop file IO or decoding. Stop its targets when hidden and show them again when visible; show restarts playback. gsp must remain alive until this service is closed. Use one service owner per target; do not concurrently replace it with another image producer.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, esp_gsp_asset_ref_t asset, bool once);
```

### `esp_gsp_assets_show_name()`

Resolve an asset by its manifest name, including newly added assets. Names are copied as a stable key; the caller may release name on return. Same asynchronous result and lifetime contract as esp_gsp_assets_show().

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, const char *name, bool once);
```

### `esp_gsp_assets_show_row()`

Same ownership and scene rules as show; a recycled row token is checked again on the UI task, so delayed data cannot replace a newly assigned row.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_row(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t slot, esp_gsp_asset_ref_t asset, bool once);
```

### `esp_gsp_assets_show_widget()`

Use the assets show widget operation.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_widget(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, esp_gsp_asset_ref_t asset, bool once);
```

### `esp_gsp_assets_show_target()`

Generic target forms for Image, Row and Widget destinations.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target, esp_gsp_asset_ref_t asset, bool once);
```

### `esp_gsp_assets_show_target_name()`

Use the assets show target name operation.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_target_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target, const char *name, bool once);
```

### `esp_gsp_assets_get_status()`

Get assets status.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_get_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target, esp_gsp_asset_status_t *out_status);
```

### `esp_gsp_assets_stop_target()`

Stop assets target.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_stop_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target);
```

### `esp_gsp_assets_get_stats()`

On-demand overview: encoded bytes belong to this package; optional gsp contributes all live scene decoded caches, including non-external images. Decoded counters also include this service's animation canvases/patch scratch. Peaks are conservative sums, not simultaneous samples. Display buffers and codec-internal scratch are not included.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_get_stats(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_assets_stats_t *out_stats);
```

### `esp_gsp_assets_status()`

Last load/decode result for this target; out_pending reports unfinished work. Returns GSP_ERR_NOT_FOUND before the target has been submitted.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t esp_gsp_assets_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, bool *out_pending);
```

### `esp_gsp_assets_stop()`

Stop scheduling this target; its last published image remains visible. An already accepted frame may finish. Call show again to restart/replace.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_stop(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind);
```

### `esp_gsp_assets_close()`

Stop new work and wait at most 5000 ms. On ESP_GSP_ERR_TIMEOUT the handle remains valid and closing; resume UI/IO progress and retry close. Never unmount storage or destroy the UI until close returns ESP_GSP_OK.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_close(esp_gsp_assets_t *assets);
```

### `esp_gsp_assets_close_wait()`

Explicit wait budget; zero polls. Does not forcibly cancel a driver read. On timeout status/stats and another close are allowed; show is rejected. Call from an application task, not a render/decode callback. Other callers must be quiescent before successful close frees the handle. Published source buffers survive close until UI replacement/shutdown. NULL succeeds.

- **Header:** `include/esp_gsp_assets.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_close_wait(esp_gsp_assets_t *assets, uint32_t timeout_ms);
```

### `esp_gsp_font_file_open()`

Loads a TTF/OTF/TTC or a GSPB font catalog from a mounted filesystem. max_bytes is a required, nonzero limit on the file size. Oversized files fail before allocation. The entire file stays in RAM (PSRAM preferred on ESP-IDF); this is not on-demand glyph IO. Catalogs are CRC-checked; dynamic fonts are checked by FreeType when the UI starts. Their signature is checked here. TTC uses its first face. Dynamic fonts require gsp_enable_freetype() in source builds. With dynamic fallback, each static GFB must contain at most 32768 glyphs; UI startup rejects larger packs with NOT_SUPPORTED. Call from an application task: open performs blocking file IO. On failure *out_font is NULL. Glyph caches and FreeType working memory are separate.

- **Header:** `include/esp_gsp_font_file.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_font_file_open(const char *path, size_t max_bytes, esp_gsp_font_file_t **out_font);
```

### `esp_gsp_font_file_apply()`

Applies a loaded font to an initialized configuration before UI startup. Sets ttf/ttf_size for a dynamic font, or font_catalog for a linked catalog; the other font source is preserved. No ownership is transferred. Keep the handle alive until every UI using this configuration has stopped successfully. Changing this configuration does not replace a font in a running UI.

- **Header:** `include/esp_gsp_font_file.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_font_file_apply(const esp_gsp_font_file_t *font, esp_gsp_config_t *config);
```

### `esp_gsp_font_file_close()`

Frees a loaded font after all borrowing UIs have stopped, or startup failed. NULL is accepted. Configurations that borrowed it must not be reused without applying another font or clearing the corresponding font source fields.

- **Header:** `include/esp_gsp_font_file.h`
- **Return type:** `void`

```c
void esp_gsp_font_file_close(esp_gsp_font_file_t *font);
```

## Events, input and overlays

Callbacks run in the context stated by the public header and must remain short when invoked from the render task.

### `esp_gsp_on_event()`

Registers the application event callback (render-task context; keep it short, use esp_gsp_set_* freely inside).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_event(esp_gsp_handle_t gsp, esp_gsp_event_cb_t cb, void *user_ctx);
```

### `esp_gsp_on_pinch()`

Registers the independent pinch observer through the render-task queue.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_pinch(esp_gsp_handle_t gsp, esp_gsp_pinch_cb_t cb, void *user_ctx);
```

**Contract details**

BEGIN is emitted once with scale 1.0. Return true to accept the pinch and
cancel the current single-pointer route; false leaves that touch sequence on
the single-pointer route. Later accepted samples emit UPDATE. END means the
contacts were released normally and CANCEL means recognition was
interrupted. Passing NULL unregisters the observer. The callback runs in
render-task context and must not block.

### `esp_gsp_set_pointer_observer()`

Registers a pointer observer (render-task context). Pass `NULL` to clear.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_pointer_observer(esp_gsp_handle_t gsp, esp_gsp_pointer_observer_cb_t cb, void *user_ctx);
```

### `esp_gsp_set_overlay_contributor()`

Registers one application-owned overlay contributor through the render-task queue (like esp_gsp_on_pinch): the registration and the resulting overlay rebuild are applied on the render task, never inline in the calling task. Passing NULL clears it. The callback runs on the render task during overlay rebuild.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_overlay_contributor(esp_gsp_handle_t gsp, esp_gsp_overlay_contributor_cb_t cb, void *user_ctx);
```

### `esp_gsp_set_input_interceptor()`

Registers one top-level input interceptor through the render-task queue. Passing NULL clears it. The callback runs in render-task context and must not block.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_input_interceptor(esp_gsp_handle_t gsp, esp_gsp_input_interceptor_cb_t cb, void *user_ctx);
```

### `esp_gsp_overlay_builder_round_rect()`

Use the overlay builder round rect operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_overlay_builder_round_rect(esp_gsp_overlay_builder_t *builder, int32_t x, int32_t y, uint16_t width, uint16_t height, uint32_t color, uint16_t radius, uint8_t opacity);
```

### `esp_gsp_overlay_builder_glyph_a8()`

The `a8` bitmap is borrowed, not copied: it must stay alive until the next overlay rebuild (any state change that repaints chrome, or the next esp_gsp_set_overlay_contributor call).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_overlay_builder_glyph_a8(esp_gsp_overlay_builder_t *builder, int32_t x, int32_t y, uint16_t width, uint16_t height, uint32_t color, const uint8_t *a8, size_t a8_size, size_t stride_bytes);
```

## Lists, grids and messages

Collection APIs retain only visible rows or cells. Binder callbacks run on the render task and row tokens must not be retained after the callback.

### `esp_gsp_grid_bind_component()`

Binds a generated Grid component. The framework owns row/column mapping, recycling, tail cells and scrolling; the callback only fills one item.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_grid_t`

```c
esp_gsp_grid_t esp_gsp_grid_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_grid_bind_cb_t bind_item, void *user_ctx);
```

### `esp_gsp_grid_cell_set_image()`

Slot-free convenience setters for the Grid cell's first dynamic image or text field. They return ESP_GSP_ERR_NOT_SUPPORTED when the template has no matching dynamic field.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const void *data, size_t size);
```

### `esp_gsp_grid_cell_set_image_ex()`

Set grid cell image ex.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image_ex(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const void *data, size_t size, const esp_gsp_image_options_t *options);
```

### `esp_gsp_grid_cell_set_image_borrowed()`

Set grid cell image borrowed.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image_borrowed(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_grid_cell_set_image_owned()`

Set grid cell image owned.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image_owned(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, void *data, size_t size);
```

### `esp_gsp_grid_cell_set_text()`

Set grid cell text.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_text(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const char *utf8);
```

### `esp_gsp_grid_set_total()`

Grid-named aliases keep application code at the cell abstraction.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_set_total(esp_gsp_handle_t gsp, esp_gsp_grid_t grid, uint32_t total);
```

### `esp_gsp_grid_refresh()`

Refresh grid.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_refresh(esp_gsp_handle_t gsp, esp_gsp_grid_t grid);
```

### `esp_gsp_row_text()`

Shapes UTF-8 into the row's TEXT slot (FreeType fallback and multi-line wrap apply as for esp_gsp_set_text).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_text(esp_gsp_handle_t gsp, esp_gsp_row_t row, const char *utf8);
```

### `esp_gsp_row_set_text()`

Shapes UTF-8 into an explicit TEXT slot of a composite row.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_text(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t text_slot, const char *utf8);
```

### `esp_gsp_row_value()`

Drives the row's first VALUE slot (0..100).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_value(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t value);
```

### `esp_gsp_row_set_value()`

Drives an explicit VALUE slot of a composite row.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_value(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t value_slot, uint32_t value);
```

### `esp_gsp_row_color()`

Drives the row's first COLOR slot (native format).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_color(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t color);
```

### `esp_gsp_row_set_color()`

Drives an explicit COLOR slot of a composite row.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_color(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t color_slot, uint32_t color);
```

### `esp_gsp_row_set_image()`

Replaces one RESOURCE slot in a recycled row from encoded JPEG/PNG/QOI bytes. The input is copied before return. A stale row token cancels the accepted request before publication, so an asynchronous loader cannot publish into a slot that has since been recycled for another item.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, const void *data, size_t size);
```

### `esp_gsp_row_set_image_ex()`

Set row image ex.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image_ex(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, const void *data, size_t size, const esp_gsp_image_options_t *options);
```

### `esp_gsp_row_set_image_borrowed()`

Borrowed/owned variants of esp_gsp_row_set_image().

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image_borrowed(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_row_set_image_owned()`

Set row image owned.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image_owned(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, void *data, size_t size);
```

### `esp_gsp_list_bind_component()`

Creates the list/wheel by its stable component key: template, viewport and initial total (the authored item count) come from the generated directory — nothing to re-pair by hand. Requires config.directories. A NULL `bind_item` serves the authored item texts directly (fixed lists: zero application assembly). Adjust the count later with esp_gsp_list_set_total when data is dynamic. The returned binding remains valid until the UI instance is destroyed.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_list_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_list_bind_cb_t bind_item, void *user_ctx);
```

### `esp_gsp_bind_list()`

ADVANCED: manual template/viewport pairing (prefer the component-key form above).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_bind_list(esp_gsp_handle_t gsp, uint16_t template_id, int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t total, esp_gsp_list_bind_cb_t bind_item, void *user_ctx);
```

### `esp_gsp_list_set_total()`

Set list total.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_set_total(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t total);
```

### `esp_gsp_list_refresh()`

Re-binds visible rows after the backing data changed.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_refresh(esp_gsp_handle_t gsp, esp_gsp_list_t list);
```

### `esp_gsp_list_set_item_heights()`

Enables variable-height rows and atomically replaces the complete height index. There is no logical item limit other than memory and the signed 31-bit pixel extent. Existing fixed-height lists do not allocate this index and keep their arithmetic fast path.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_set_item_heights(esp_gsp_handle_t gsp, esp_gsp_list_t list, const uint16_t *heights, uint32_t count, uint32_t prepended_count, bool stick_to_end);
```

**Contract details**

`prepended_count` maps the old first visible item onto its new index when
older records were inserted at the front. If `stick_to_end` is true, a
viewport already at the end remains attached to the end (new live messages
do not pull a user who is browsing older history).

### `esp_gsp_message_list_bind_component()`

Binds a generated message_list component and loads its current source. The source is application-owned; its callbacks execute on the render task.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_message_list_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, const esp_gsp_message_source_t *source);
```

### `esp_gsp_message_list_changed()`

Re-reads count/messages and updates layout. Pass the number inserted before the previous item zero to preserve the visible history anchor. Appends keep a viewport already at the end attached to the new end, without pulling a user who is browsing older history.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_message_list_changed(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t prepended_count);
```

### `esp_gsp_list_snap()`

Wheel-style behavior: released drags settle on a row boundary.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_snap(esp_gsp_handle_t gsp, esp_gsp_list_t list, bool enable);
```

### `esp_gsp_list_fling()`

Programmatic momentum: starts coasting at `velocity_px_s` (signed; positive scrolls toward higher items). Friction and row snapping behave exactly as a released drag. A newly bound visible list is activated when this command is applied, so callers need not wait for a UI tick between binding and requesting the fling.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_fling(esp_gsp_handle_t gsp, esp_gsp_list_t list, int32_t velocity_px_s);
```

### `esp_gsp_list_scroll_to()`

Jumps to an absolute scroll offset in pixels (clamped to content; cancels any coasting). Fixed-height row N starts at N * row_height; variable-height lists use cumulative item heights.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_scroll_to(esp_gsp_handle_t gsp, esp_gsp_list_t list, int32_t offset_px);
```

### `esp_gsp_list_fade()`

Roller-style fade: translucent `native_color` bands dim the rows toward the viewport's top and bottom edges. Pass the wheel's background color. The bands render above the row instances, which disables the scroll-blit shortcut over this viewport — scrolling repaints the viewport instead.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_fade(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t native_color, bool enable);
```

### `esp_gsp_on_scroll()`

Use the on scroll operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_scroll(esp_gsp_handle_t gsp, gsp_rect_t region, esp_gsp_scroll_cb_t cb, void *user_ctx);
```

## Canvas

Canvas frames are borrowed. Every accepted frame receives exactly one release callback; immediate errors leave ownership with the caller.

### `esp_gsp_canvas_set_draw_cb()`

Registers a direct draw callback and requests the first full repaint. The authored bind must target an unscaled profile-native opaque image. Existing esp_gsp_canvas_push() users are unaffected; pushing a frame to the same bind replaces the callback. user_ctx must remain valid until the callback is replaced or Canvas stop/destruction has completed; use esp_gsp_flush() before releasing it after esp_gsp_canvas_stop(). A repaint may invoke the callback more than once; render one coherent generation for all regions belonging to that repaint.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_set_draw_cb(esp_gsp_handle_t gsp, uint16_t bind, esp_gsp_canvas_draw_cb_t draw_cb, void *user_ctx);
```

### `esp_gsp_canvas_invalidate()`

Requests a full repaint of a callback-backed Canvas.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_invalidate(esp_gsp_handle_t gsp, uint16_t bind);
```

### `esp_gsp_canvas_invalidate_dirty()`

Requests repaint of a non-empty Canvas-local rectangle.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_invalidate_dirty(esp_gsp_handle_t gsp, uint16_t bind, gsp_rect_t dirty);
```

### `esp_gsp_canvas_push()`

Publishes a borrowed frame. Eligible renderer copies automatically use the available PPA/DMA2D backend and otherwise fall back to software.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_push(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_try_push()`

Non-blocking variant of esp_gsp_canvas_push(). Returns ESP_GSP_ERR_TIMEOUT when the render queue cannot accept the frame immediately; ownership remains with the caller in that case.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_try_push(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_push_dirty()`

Publishes a borrowed Canvas frame while repainting only `dirty`, in resource-local pixels with an exclusive x2/y2 edge. Pixels outside dirty must match the previously displayed frame. The dirty rectangle must be non-empty and fit the authored placeholder. Ownership and release ordering are identical to esp_gsp_canvas_push().

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_push_dirty(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_try_push_dirty()`

Non-blocking variant of esp_gsp_canvas_push_dirty().

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_try_push_dirty(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_stop()`

Queues restoration of the placeholder image. The release callback is the completion signal for each borrowed frame; do not free accepted frames immediately after this function returns.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_stop(esp_gsp_handle_t gsp, uint16_t bind);
```

## Timers, templates and animation

Runtime objects consume bounded pools and are applied on the render task. Template quotas come from authored requirements and project overrides.

### `esp_gsp_scale_q16_from_percent()`

Converts an integer percentage to unsigned Q16.16 scale.

- **Header:** `include/esp_gsp.h`
- **Return type:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_from_percent(uint32_t percent);
```

### `esp_gsp_scale_q16_multiply()`

Combines an image scale and a relative pinch scale, with saturation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_multiply(uint32_t scale_q16, uint32_t relative_q16);
```

### `esp_gsp_scale_q16_clamp()`

Clamps an unsigned Q16.16 scale to an authored or application range.

- **Header:** `include/esp_gsp.h`
- **Return type:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_clamp(uint32_t scale_q16, uint32_t minimum_q16, uint32_t maximum_q16);
```

### `esp_gsp_chart_set_series()`

Replaces every point in a fixed-capacity series in one render transaction. Values use the authored business range and are copied before return. count must equal the series capacity (at most ESP_GSP_CHART_MAX_POINTS). Small batches stay inline; larger batches use temporary framework-owned storage and return ESP_GSP_ERR_NO_MEM if allocation fails.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_chart_set_series(esp_gsp_handle_t gsp, const esp_gsp_chart_series_t *series, const int32_t *values, size_t count);
```

### `esp_gsp_chart_append()`

Appends one business value, dropping the oldest point from the fixed window. Consecutive queued appends are applied in order on the render task.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_chart_append(esp_gsp_handle_t gsp, const esp_gsp_chart_series_t *series, int32_t value);
```

### `esp_gsp_set_press_feedback_enabled()`

Enable/disable the default press shade; processed on the render task.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_press_feedback_enabled(esp_gsp_handle_t gsp, bool enabled);
```

### `esp_gsp_query_visibility()`

True when the active scene, visibility gates and transformed layout bounds intersect the screen/ancestor viewport. Does not test pixel alpha or occlusion by unrelated siblings. Prefer the generated get_effective_visible() helper.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_query_visibility(esp_gsp_handle_t gsp, const esp_gsp_visibility_target_t *target, bool *out_visible);
```

### `esp_gsp_timer_create()`

Periodic callback in render-task context (lv_timer equivalent). Returns a handle usable with esp_gsp_timer_delete; NULL on error.

- **Header:** `include/esp_gsp.h`
- **Return type:** `void *`

```c
void *esp_gsp_timer_create(esp_gsp_handle_t gsp, uint32_t period_ms, esp_gsp_timer_cb_t cb, void *user_ctx);
```

### `esp_gsp_timer_delete()`

Delete timer.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_timer_delete(esp_gsp_handle_t gsp, void *timer);
```

### `esp_gsp_widget_create()`

Instantiates `template_id` at (`x`, `y`) in the CURRENT scene. Creation is asynchronous; the handle is immediately usable with the widget setters. Returns ESP_GSP_WIDGET_NONE when the widget quota is used up.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_widget_t`

```c
esp_gsp_widget_t esp_gsp_widget_create(esp_gsp_handle_t gsp, uint16_t template_id, int16_t x, int16_t y);
```

### `esp_gsp_widget_create_template()`

Named-descriptor form; avoids exposing the compiled template id.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_widget_t`

```c
static inline esp_gsp_widget_t esp_gsp_widget_create_template(esp_gsp_handle_t gsp, const esp_gsp_template_desc_t *template_desc, int16_t x, int16_t y);
```

### `esp_gsp_widget_destroy()`

Destroy widget.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_destroy(esp_gsp_handle_t gsp, esp_gsp_widget_t widget);
```

### `esp_gsp_widget_set_value()`

Drives a VALUE slot (progress members), value in 0..100.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_value(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, uint32_t value);
```

### `esp_gsp_widget_set_color()`

Drives a COLOR slot, native pixel format (see esp_gsp_set_color).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_color(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, uint32_t color);
```

### `esp_gsp_widget_set_text()`

Shapes UTF-8 into a TEXT slot; the string is copied before return.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_text(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, const char *utf8);
```

### `esp_gsp_widget_set_image()`

Replaces a template RESOURCE slot from copied encoded image bytes.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_image(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot, const void *data, size_t size);
```

### `esp_gsp_widget_set_image_borrowed()`

Set widget image borrowed.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_image_borrowed(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_widget_set_image_owned()`

Set widget image owned.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_image_owned(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot, void *data, size_t size);
```

### `esp_gsp_widget_value()`

Use the widget value operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_value(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t value);
```

### `esp_gsp_widget_color()`

Use the widget color operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_color(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t color);
```

### `esp_gsp_widget_text()`

Use the widget text operation.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_text(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, const char *utf8);
```

### `esp_gsp_widget_set_visible()`

Set widget visible.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_visible(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, bool visible);
```

### `esp_gsp_widget_set_position()`

Set widget position.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_position(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, int16_t x, int16_t y);
```

### `esp_gsp_play()`

Plays an EAF frame animation into a profile-native RGB565/RGB888 placeholder image resource. The framework ticks it on the render task — no app-side loop. `eaf` is NOT copied. once = stop on the last frame.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_play(esp_gsp_handle_t gsp, const void *eaf, size_t size, bool once);
```

### `esp_gsp_animate()`

Animates a value bind from `from` to `to` (0..100 domain) over `duration_ms`. A new animation on the same bind replaces the running one.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_animate(esp_gsp_handle_t gsp, uint16_t bind, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_animate_color()`

Tweens a color bind between two native colors (per-channel interpolation, RGB565 scenes).

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_animate_color(esp_gsp_handle_t gsp, uint16_t bind, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_value()`

Component-key animations use semantic VALUE/COLOR properties and the same authored-unit conversion as component get/set.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_value_to()`

Animate component value.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_value_to(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_color()`

Animate component color.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_color(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_color_to()`

Animate component color.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_color_to(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_property()`

Open-key animation for any scalar semantic property.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_property(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *from, const gsp_value_t *to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_property_to()`

Animate component property.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_property_to(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_position_to()`

Starts one synchronized x/y tween from the committed position. Both axes are committed together on every step, so no frame shows the component at a position between the two. Animating x and y as two separate properties does not give that guarantee.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_position_to(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t x, int32_t y, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_widget_animate()`

Same, driving a widget's first VALUE slot.

- **Header:** `include/esp_gsp.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_animate(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

## ESP-LCD lifecycle

These functions own the ESP-IDF render task and coordinate display presentation, pause, suspend and retained-session handoff.

### `esp_gsp_esp_lcd_config_init()`

Use the esp lcd config init operation.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_gsp_esp_lcd_config_t`

```c
static inline esp_gsp_esp_lcd_config_t esp_gsp_esp_lcd_config_init(void);
```

### `esp_gsp_esp_lcd_pause()`

Quiesce the active GSP render task without destroying its UI state.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_pause(esp_gsp_handle_t gsp, uint32_t timeout_ms, esp_gsp_esp_lcd_pause_t **out_pause);
```

**Contract details**

On success no new GSP frame can be submitted until
esp_gsp_esp_lcd_resume_paused() is called. The function also waits for the
shared presenter transfer/present fences to retire.

### `esp_gsp_esp_lcd_resume_paused()`

Resume the exact UI paused by esp_gsp_esp_lcd_pause(), including a temporary app running on a retained hub session. The pause token is consumed on success and the first resumed frame is forced to redraw in full.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_resume_paused(esp_gsp_esp_lcd_pause_t *pause, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_suspend()`

Pause a UI while retaining its scenes and component state. Its render task and LCD host remain resident; decoded image surfaces are released.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_suspend(esp_gsp_handle_t hub, esp_gsp_esp_lcd_session_t **out_session);
```

### `esp_gsp_esp_lcd_start_on_session()`

Start an app bundle on a suspended hub's LCD host. The bundle must match the display width, height and pixel format. A startup failure resumes the hub and consumes the session.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start_on_session(esp_gsp_esp_lcd_session_t *session, const esp_gsp_config_t *app_config, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_start_on_session_prepared()`

Start an app and invoke `prepare` on the resident render task before the first frame. This is intended for installing generic runtime contributors, event handlers and timers without racing the initial render.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start_on_session_prepared(esp_gsp_esp_lcd_session_t *session, const esp_gsp_config_t *app_config, esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_replace_on_session_prepared()`

Replace the active temporary app while retaining the suspended hub. Unlike esp_gsp_esp_lcd_resume(), this never reactivates or presents the hub between the two Apps. On startup failure the hub is resumed and the session is consumed.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_replace_on_session_prepared(esp_gsp_esp_lcd_session_t *session, esp_gsp_handle_t current_app, const esp_gsp_config_t *next_config, esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_resume()`

Stop the temporary app, then resume the retained hub and force its first frame to redraw. The session is consumed on success.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_resume(esp_gsp_esp_lcd_session_t *session, esp_gsp_handle_t app, esp_gsp_handle_t *out_hub);
```

### `esp_gsp_esp_lcd_session_destroy()`

Destroy a suspended hub and its LCD host when no temporary app is active.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_session_destroy(esp_gsp_esp_lcd_session_t *session);
```

### `esp_gsp_esp_lcd_start()`

Creation and resource validation use the caller's stack before the render task starts. See getting-started: the example starts with a 20 KiB main stack; measure the caller's high-water mark with the actual asset/font workload.

- **Header:** `include/esp_gsp_esp_lcd.h`
- **Return type:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start(const esp_gsp_config_t *app_config, const esp_gsp_esp_lcd_config_t *esp_config, esp_gsp_handle_t *out_gsp);
```

## Deployable bundles

Deployable metadata is parsed from borrowed GSPB bytes. The bytes must remain valid until the metadata handle is closed.

### `esp_gsp_deployable_bundle_open()`

Opens a borrowed GSPB containing one GMD member for every GSB scene.

- **Header:** `include/esp_gsp_deployable.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_open(const void *data, size_t size, bool verify_crc, esp_gsp_deployable_bundle_t **out_bundle);
```

**Contract details**

The input bytes must be 64-byte aligned and remain valid until the handle
is closed. When `verify_crc` is false, structural validation still runs.

### `esp_gsp_deployable_bundle_get_info()`

Returns immutable package requirements discovered during open.

- **Header:** `include/esp_gsp_deployable.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_get_info(const esp_gsp_deployable_bundle_t *bundle, esp_gsp_deployable_info_t *out_info);
```

### `esp_gsp_deployable_bundle_make_config()`

Builds an application configuration backed by the opened bundle.

- **Header:** `include/esp_gsp_deployable.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_make_config(const esp_gsp_deployable_bundle_t *bundle, esp_gsp_config_t *out_config);
```

**Contract details**

The deployable handle and its borrowed bytes must outlive the UI instance.
Stop the UI before calling esp_gsp_deployable_bundle_close().

### `esp_gsp_deployable_bundle_action_key()`

Resolves a scene-local numeric CALL id to its stable authored-name key.

- **Header:** `include/esp_gsp_deployable.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_action_key(const esp_gsp_deployable_bundle_t *bundle, uint16_t scene_id, uint16_t action_id, uint32_t *out_key);
```

### `esp_gsp_deployable_bundle_close()`

Releases parsed metadata. The borrowed GSPB bytes are never freed.

- **Header:** `include/esp_gsp_deployable.h`
- **Return type:** `void`

```c
void esp_gsp_deployable_bundle_close(esp_gsp_deployable_bundle_t *bundle);
```

## Diagnostics

Diagnostic counters help analyze rendering, input and resource use. Keep product state in application code.

### `esp_gsp_heap_stats()`

Read device-wide internal/PSRAM heaps on demand, including non-GSP users. Does not allocate or change budgets. Call from task context, not an ISR or every frame: heap inspection traverses allocator metadata. The two heaps are sampled separately; concurrent allocations can change the result. Returns false and zeroes the output on hosts without heap capabilities; also returns false for NULL. An absent PSRAM heap has zero values. These values are not DMA guarantees or a complete scene memory budget.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `bool`

```c
bool esp_gsp_heap_stats(esp_gsp_heap_stats_t *out_stats);
```

### `esp_gsp_frame_count()`

Monotonic count of rendered non-idle frames since startup.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `uint32_t`

```c
uint32_t esp_gsp_frame_count(esp_gsp_handle_t gsp);
```

### `esp_gsp_render_stats()`

Read rendered frames and cumulative time spent producing frames.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `void`

```c
void esp_gsp_render_stats(esp_gsp_handle_t gsp, uint32_t *out_frames, uint64_t *out_busy_us);
```

### `esp_gsp_transition_stats()`

Use the transition stats operation.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `void`

```c
void esp_gsp_transition_stats(esp_gsp_handle_t gsp, esp_gsp_transition_stats_t *out_stats);
```

### `esp_gsp_drag_snapshot_stats()`

Cumulative List/Grid/MessageList and viewport drag snapshot accounting. Read from a serialized app callback, like the other debug counters.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `void`

```c
void esp_gsp_drag_snapshot_stats(esp_gsp_handle_t gsp, esp_gsp_drag_snapshot_stats_t *out_stats);
```

### `esp_gsp_region_stats()`

Use the region stats operation.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `void`

```c
void esp_gsp_region_stats(esp_gsp_handle_t gsp, esp_gsp_region_stats_t *out_stats);
```

### `esp_gsp_render_phases()`

Cumulative rasterization and presentation time.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `void`

```c
void esp_gsp_render_phases(esp_gsp_handle_t gsp, uint64_t *out_render_us, uint64_t *out_submit_us);
```

### `esp_gsp_service_stats()`

Render-task service-loop profile. Values are zero unless the integration enables the lightweight GSP_PROFILE_SERVICE build instrumentation.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `void`

```c
void esp_gsp_service_stats(esp_gsp_handle_t gsp, uint32_t *out_iterations, uint64_t *out_service_us, uint32_t *out_commands);
```

### `esp_gsp_media_stats()`

Use the media stats operation.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `void`

```c
void esp_gsp_media_stats(esp_gsp_handle_t gsp, esp_gsp_media_stats_t *out_stats);
```

### `esp_gsp_inject_touch()`

Inject one logical pointer sample. Intended for tests and diagnostics.

- **Header:** `include/esp_gsp_debug.h`
- **Return type:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_inject_touch(esp_gsp_handle_t gsp, int16_t x, int16_t y, bool pressed);
```

## Advanced integration

Advanced entry points expose low-level renderer state for custom rendering integration.

### `esp_gsp_context()`

Return the active low-level rendering context.

- **Header:** `include/esp_gsp_advanced.h`
- **Return type:** `gsp_context_t *`

```c
gsp_context_t *esp_gsp_context(esp_gsp_handle_t gsp);
```

## Portable core and framework integration

These lower-level headers support platform adapters, launchers and generated component directories. Ordinary ESP-IDF applications use esp_gsp.h instead.

### `gsp_app_create()`

Create app.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_app_create(const gsp_app_config_t *config, gsp_app_t **out_app);
```

### `gsp_app_effective_config()`

Use the app effective config operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `const esp_gsp_effective_config_t *`

```c
const esp_gsp_effective_config_t *gsp_app_effective_config(const gsp_app_t *app);
```

### `gsp_app_destroy()`

Destroy app.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `void`

```c
void gsp_app_destroy(gsp_app_t *app);
```

### `gsp_app_apply()`

Use the app apply operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_app_apply(gsp_app_t *app, const gsp_app_command_t *command);
```

### `gsp_app_step()`

Use the app step operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `void`

```c
void gsp_app_step(gsp_app_t *app, int64_t now_us);
```

### `gsp_app_next_deadline_us()`

Use the app next deadline us operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `int64_t`

```c
int64_t gsp_app_next_deadline_us(const gsp_app_t *app);
```

### `gsp_app_feed_pointer()`

Scripted pointer injection (tests, simulators, demos): feeds the classifier directly, bypassing the input interceptor and observer that platform runners route through.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `void`

```c
void gsp_app_feed_pointer(gsp_app_t *app, int32_t x, int32_t y, bool pressed);
```

### `gsp_app_feed_touch()`

Feeds one contact from a multi-touch source. The portable app keeps the active contact set and routes its centroid through the pointer classifier; platform pinch-aware builds may consume the same contact stream directly.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `void`

```c
void gsp_app_feed_touch(gsp_app_t *app, int32_t id, int32_t x, int32_t y, bool pressed);
```

### `gsp_app_current_scene()`

Use the app current scene operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `uint16_t`

```c
uint16_t gsp_app_current_scene(const gsp_app_t *app);
```

### `gsp_app_scene_count()`

Use the app scene count operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `uint16_t`

```c
uint16_t gsp_app_scene_count(const gsp_app_t *app);
```

### `gsp_app_current_context()`

Use the app current context operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `gsp_context_t *`

```c
gsp_context_t *gsp_app_current_context(gsp_app_t *app);
```

### `gsp_app_scene_context()`

Use the app scene context operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `gsp_context_t *`

```c
gsp_context_t *gsp_app_scene_context(gsp_app_t *app, uint16_t scene);
```

### `gsp_app_transition()`

Use the app transition operation.

- **Header:** `include/gsp/gsp_app.h`
- **Return type:** `bool`

```c
bool gsp_app_transition(const gsp_app_t *app, gsp_app_transition_t *out_transition);
```

### `gsp_drawer_axis_is_y()`

True when the drawer travels along Y, so callers know which axis of the transform group carries its offset.

- **Header:** `include/gsp/gsp_component.h`
- **Return type:** `bool`

```c
static inline bool gsp_drawer_axis_is_y(const gsp_drawer_config_t *config);
```

### `gsp_drawer_closed_offset()`

Transform-group offset that parks the drawer off-screen against its edge; open is always offset zero.

- **Header:** `include/gsp/gsp_component.h`
- **Return type:** `int32_t`

```c
static inline int32_t gsp_drawer_closed_offset(const gsp_drawer_config_t *config);
```

**Contract details**

The compiler bakes this same value into the scene's initial state and
the gesture layer measures drag progress against it, so it has to be
one definition — a driver and an input layer that disagree about
where "closed" is produce a drawer that snaps to the wrong end.

### `gsp_component_find()`

Use the component find operation.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_find(const gsp_component_directory_t *directory, gsp_runtime_t *runtime, gsp_component_key_t key, gsp_component_view_t *out_view);
```

### `gsp_component_get()`

Get component.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_get(const gsp_component_view_t *view, gsp_component_prop_kind_t prop, gsp_value_t *out_value);
```

### `gsp_component_get_property()`

Get component property.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_get_property(const gsp_component_view_t *view, gsp_property_key_t property, gsp_value_t *out_value);
```

### `gsp_component_get_property_info()`

Get component property info.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_get_property_info(const gsp_component_view_t *view, gsp_property_key_t property, gsp_property_info_t *out_info);
```

### `gsp_component_get_property_info_at()`

Get component property info at.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_get_property_info_at(const gsp_component_view_t *view, size_t index, gsp_property_info_t *out_info);
```

### `gsp_component_set()`

Set component.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_set(gsp_update_transaction_t *transaction, const gsp_component_view_t *view, gsp_component_prop_kind_t prop, const gsp_value_t *value);
```

### `gsp_component_set_property()`

Set component property.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_set_property(gsp_update_transaction_t *transaction, const gsp_component_view_t *view, gsp_property_key_t property, const gsp_value_t *value);
```

### `gsp_component_set_many()`

Set component many.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_set_many(gsp_update_transaction_t *transaction, const gsp_component_directory_t *directory, const gsp_component_update_t *updates, size_t count);
```

### `gsp_component_set_properties()`

Set component properties.

- **Header:** `include/gsp/gsp_component_advanced.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_component_set_properties(gsp_update_transaction_t *transaction, const gsp_component_directory_t *directory, const gsp_component_property_update_t *updates, size_t count);
```

### `gsp_font_catalog_create()`

Opens a borrowed GSPB containing GFB members keyed by content_id.

- **Header:** `include/gsp/gsp_font_catalog.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_font_catalog_create(const void *data, size_t size, bool verify_crc, gsp_font_catalog_t **out_catalog);
```

### `gsp_font_catalog_destroy()`

Destroy font catalog.

- **Header:** `include/gsp/gsp_font_catalog.h`
- **Return type:** `void`

```c
void gsp_font_catalog_destroy(gsp_font_catalog_t *catalog);
```

### `gsp_font_catalog_find()`

Finds a GFB member by Asset ID (GSP_ERR_NOT_FOUND when absent).

- **Header:** `include/gsp/gsp_font_catalog.h`
- **Return type:** `gsp_err_t`

```c
gsp_err_t gsp_font_catalog_find(const gsp_font_catalog_t *catalog, uint32_t asset_id, const void **out_data, size_t *out_size);
```

### `gsp_renderer_profile_snapshot()`

Copies the per-opcode cumulative render time / call counters (zeros unless the benchmark enables GSP_PROFILE_RENDER). Index = opcode value.

- **Header:** `include/gsp/gsp_render_profile.h`
- **Return type:** `void`

```c
void gsp_renderer_profile_snapshot(uint64_t *out_us, uint32_t *out_calls, size_t cap);
```

### `gsp_esp_touch_interrupt_available()`

Use the esp touch interrupt available operation.

- **Header:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **Return type:** `bool`

```c
static inline bool gsp_esp_touch_interrupt_available(esp_lcd_touch_handle_t handle);
```

### `gsp_esp_touch_init()`

Use the esp touch init operation.

- **Header:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **Return type:** `void`

```c
void gsp_esp_touch_init(gsp_esp_touch_t *touch, esp_lcd_touch_handle_t handle, bool interrupt_mode, bool interrupt_required, uint8_t release_confirm_polls, gsp_esp_touch_wake_from_isr_t external_wake_from_isr, void *external_wake_ctx);
```

### `gsp_esp_touch_start()`

Start esp touch.

- **Header:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **Return type:** `esp_err_t`

```c
esp_err_t gsp_esp_touch_start(gsp_esp_touch_t *touch, gsp_esp_touch_wake_from_isr_t wake_from_isr, void *wake_ctx);
```

### `gsp_esp_touch_stop()`

Stop esp touch.

- **Header:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **Return type:** `void`

```c
void gsp_esp_touch_stop(gsp_esp_touch_t *touch);
```

### `gsp_esp_touch_poll_frame()`

Use the esp touch poll frame operation.

- **Header:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **Return type:** `esp_err_t`

```c
esp_err_t gsp_esp_touch_poll_frame(gsp_esp_touch_t *touch, gsp_platform_touch_frame_t *out_frame, bool *out_has_sample);
```

### `gsp_esp_touch_poll()`

Use the esp touch poll operation.

- **Header:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **Return type:** `esp_err_t`

```c
esp_err_t gsp_esp_touch_poll(gsp_esp_touch_t *touch, gsp_platform_pointer_t *out_pointer, bool *out_has_sample);
```
