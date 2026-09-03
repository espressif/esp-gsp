# Application Lifecycle and Threading

This document is the application-facing contract for starting, updating,
synchronizing and stopping an ESP-GSP UI on ESP-IDF.

## Recommended API layers

Use the narrowest layer that fits the application:

1. Include the generated `<symbol>_gsp.h` bundle header.
2. Start from `gsp_<symbol>_config()`.
3. Use generated `gsp_<scene>_<object>_*()` functions for named controls.
4. Use `esp_gsp_on_event()` with the generated scene event decoder.
5. Use the generic image, canvas, list or navigation APIs only when the
   generated object API does not cover the operation.

The bind, stable-key/property and raw widget/template APIs remain available
for compatibility, editors and data-driven bridges. They are not separate
application startup paths.

For a UI package obtained independently from the application firmware, build
the bundle with `gsp_add_bundle(... DEPLOYABLE)` (or `gspc pack --deployable`),
then open it with `esp_gsp_deployable_bundle_open()` and derive the normal
`esp_gsp_config_t` using `esp_gsp_deployable_bundle_make_config()`. This is an
optional package-loading path; the generated-header path above remains the
default.

## Configuration lifetime

`gsp_<symbol>_config()` initializes `esp_gsp_config_t.struct_size`, the
embedded bundle and every generated component directory. Applications that
construct the configuration manually should start from
`ESP_GSP_CONFIG_INIT()`.

The ESP-LCD adapter configuration should start from
`ESP_GSP_ESP_LCD_CONFIG_INIT()`:

```c
esp_gsp_config_t app_config = gsp_bundle_config();

esp_gsp_esp_lcd_config_t lcd_config =
    ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd_config.display = display;
lcd_config.touch = touch;

esp_gsp_handle_t ui;
ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(
    &app_config, &lcd_config, &ui));
```

The runtime also accepts a zero `struct_size` from aggregate-zero initialized
configurations. A nonzero size must match the public structure used by the
current component.

The bundle bytes, generated component directories and optional TTF bytes must
remain valid until `esp_gsp_stop()` succeeds.

For a deployable bundle, both the borrowed GSPB bytes and the
`esp_gsp_deployable_bundle_t` handle must remain valid until the UI stops.
Close the handle only after `esp_gsp_stop()`; closing it never frees the
caller-owned GSPB bytes.

## Submission and committed state

State setters are asynchronous on ESP-IDF:

```text
application task -> command queue -> render task -> state commit -> render
```

A successful setter means the command was accepted. It does not mean that a
frame has already reached the display. The normal queue admission wait is
bounded; queue pressure is returned as `ESP_GSP_ERR_TIMEOUT`.

Getters read the committed state of the current scene. A setter followed
immediately by a getter from another task may still observe the previous
value.

Use `esp_gsp_component_set_many()` or
`esp_gsp_component_set_properties()` when related scalar properties must
become visible atomically.

## Synchronization fence

`esp_gsp_flush(ui, timeout_ms)` is a low-frequency fence. It waits until
commands submitted before the call have been applied and the display backend
has completed the following render attempt.

Use it for deterministic tests, screenshots or an orderly state boundary:

```c
ESP_ERROR_CHECK(gsp_dashboard_load_set_value(ui, 80));
ESP_ERROR_CHECK(esp_gsp_flush(ui, 250));
```

Do not put `esp_gsp_flush()` in an animation or telemetry loop. It preserves
the asynchronous setter model and is invalid from render-task callbacks.
Portable apps without an asynchronous platform adapter already apply commands
directly, so the fence completes immediately. Render failures remain available
through `esp_gsp_render_error_stats()`; the fence reports synchronization
status, not the render result. A timeout does not cancel a fence that was
already accepted by the command queue.

## Callback contexts

The following callbacks run on the render task and must stay short and
non-blocking:

- application events registered with `esp_gsp_on_event()`;
- framework timers;
- list row binders and scroll callbacks;
- image completion callbacks that complete on the render task;
- canvas release callbacks.

Generated setters may be called from these callbacks; the render task applies
them directly. Canvas direct-draw callbacks are the exception: they run inside
an active renderer frame, so APIs that change GSP state return
`ESP_GSP_ERR_INVALID_STATE`. Do not wait for `esp_gsp_flush()` or call
`esp_gsp_stop()` from any render-task callback. Defer state changes, shutdown
and blocking work to an application task.

Image decode completion may also run on the decode task. Completion and
release callbacks must not assume application-task context.

## Image and canvas ownership

- `esp_gsp_set_image()` copies the encoded input before returning.
- `esp_gsp_set_image_borrowed()` borrows the input until its release callback.
- `esp_gsp_set_image_owned()` takes malloc-compatible storage after a
  successful submission.
- `esp_gsp_set_image_ex()` selects COPY, BORROW or TAKE explicitly.
- Canvas frames remain borrowed until their release callbacks run.
- A Canvas direct-draw callback runs synchronously in render-task context. Its
  surface pointer is valid only during that call and must not be retained. It
  must fill every supplied region and keep partitioned calls for one repaint
  visually coherent.

A borrowed or taken payload is held while its image is the one on screen, not
merely until it decodes: under memory pressure the cache may drop the decoded
pixels and decode again from the encoded bytes. The release therefore lands
when the next image for that bind is adopted, or at shutdown — so budget one
outstanding payload per image bind, not one per submission.

A bind keeps showing its current image for the whole time a replacement is
decoding, and keeps it if that decode fails. An update is visible in one step
or not at all, so applications never need to pre-load a placeholder to avoid a
flash.

If a borrowed or TAKE submission returns an error immediately, ownership
remains with the caller and no later release callback follows.

`esp_gsp_canvas_stop()` is asynchronous. Its return does not prove that every
accepted frame has been released or that a direct-draw callback can no longer
run. Call `esp_gsp_flush()` before freeing callback context or borrowed frames.

## List binding lifetime

Bind each logical List or Wheel once and retain the returned handle for the
life of the UI instance. A binding persists across scene changes: the runtime
parks its rows while the owning scene or PageFlow page is hidden, then
reactivates them when it becomes visible again.

Do not call `esp_gsp_list_bind_component()` from every scene-ready event.
Repeated binding allocates another entry from the configured list pool
(`CONFIG_ESP_GSP_MAX_LISTS`, default 5) and eventually returns
`ESP_GSP_LIST_NONE`. Use `esp_gsp_list_set_total()` when the item count changes
and `esp_gsp_list_refresh()` when visible row data changes. The current API has
no unbind operation; all bindings are released by `esp_gsp_stop()`.

## Shutdown

Call `esp_gsp_stop()` from an application task. It requests render-task
shutdown, drains display callbacks, stops background decode and releases the
UI instance. It can return `ESP_GSP_ERR_TIMEOUT`; the handle remains owned by
the application and shutdown may be retried.

After `esp_gsp_stop()` succeeds, the handle and every framework-owned callback
registration are invalid.

## Suspend and temporary applications

The ESP-LCD adapter can retain one hub UI and its LCD render task while a
temporary bundle uses the same display host:

```text
hub -> suspend -> session -> start temporary app
                         -> stop app and resume hub
```

Call `esp_gsp_esp_lcd_suspend()` from an application task. On success the hub
runtime, scene and component state remain owned by the returned session, while
decoded image surfaces are released. Do not submit work through the suspended
hub handle.

`esp_gsp_esp_lcd_start_on_session()` requires a bundle with the same logical
width, height and pixel format as the retained display. On success it returns
the temporary app handle. If app creation or compatibility validation fails,
the adapter resumes the hub and consumes the session as part of recovery.

Pass that exact temporary app handle to `esp_gsp_esp_lcd_resume()`. The call
stops the temporary app, resumes the retained hub, schedules a full first
frame and consumes the session on success.

If no temporary app is active, `esp_gsp_esp_lcd_session_destroy()` destroys
the suspended hub and display host instead of resuming them. A session with an
active temporary app cannot be destroyed.
