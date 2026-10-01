# Application customization

Use public callbacks and generated helpers to customize an application without
including private headers or replacing the renderer. Extensions execute in the
application's address space: they must obey buffer bounds, scheduling and
lifetime rules.

## Choose an extension point

- Business actions: author `callback` actions and handle `esp_gsp_on_event()`.
- Component state: use `esp_gsp_on_component_event()` for supported components.
- Input: use the pointer observer for observation, pinch for two-contact
  gestures, and the input interceptor only when the application owns routing
  for the whole touch sequence. Do not swallow only the release of a sequence
  already delivered to a control. Synthetic input does not necessarily follow
  the platform interceptor path.
- Custom pixels: use a Canvas draw callback for short CPU drawing, or submit
  producer-owned Canvas frames for camera, video or worker-produced content.
- Transient foreground graphics: use the overlay contributor. It is one
  application-owned contributor; compose application features before registering
  it instead of having independent modules overwrite each other's registration.
- Dynamic content: use existing list/grid data sources, image setters and
  external assets before adding a new resource backend.

These callbacks do not grant access to private scene state, cache entries,
framebuffer ownership or DMA fences. Display submission stays with the framework.

## Canvas drawing contract

Author a named, bound, opaque image with native dimensions and a lossless
placeholder. Use the target's RGB565 or RGB888 profile. Direct drawing requires
an unscaled opaque image; it is not a general transparent or transformed widget.
The placeholder remains the content when drawing is stopped.

Use one producer per bind. Stop Canvas and synchronize before switching that
bind to ordinary image setters; do not interleave the two resource owners.

The runtime currently replaces a resource view in place. Canvas registration
therefore rejects views shared by another image bind, static element or image
bank; it leaves both elements unchanged and returns accepted borrowed frames
through their release callbacks. Identical placeholders may share a view after
compilation. Independent mutable views are required before such elements can
be used as independent Canvases; named helpers alone do not change this limit.
With an asynchronous adapter, API success means queue admission, not target
compatibility. Check render-task diagnostics for rejected registration; flush
is a synchronization fence, not a registration-result query.

For programmatic confirmation, include `esp_gsp_extension.h`, register an
instance observer with `esp_gsp_on_extension_event()`, then submit
`esp_gsp_canvas_set_draw_cb_ex()` with an application request ID. Match
`ESP_GSP_EXTENSION_CANVAS_RESULT` by request ID, scene, bind and operation.
Submission returns an ESP_GSP queue error; the processed result uses GSP error
codes. Successful application does not certify presentation. A rejected
registration does not release the previous draw context.
The request ID correlates results but does not pin a scene: like existing bind
setters, it addresses the current scene at execution. The result names that
actual scene; serialize navigation and producer changes accordingly.

`esp_gsp_canvas_check()` checks current direct-draw compatibility without
changing resources or consuming/requiring a free Canvas slot. It does not
reserve anything: actual registration can still fail if capacity, scene or
resource state changes. This is a processed-command observation stream, not an
exactly-once completion API for every queued request. Commands discarded on
shutdown and coalesced frames do not each emit results. Flush the active
runtime before exit when results must be observed.

`esp_gsp_canvas_set_draw_cb()` installs the callback and requests a full repaint.
Use `esp_gsp_canvas_invalidate()` or `esp_gsp_canvas_invalidate_dirty()` after
changing its data. Dirty rectangles are Canvas-local half-open ranges
`[x1, x2) × [y1, y2)` inside the authored dimensions.

For every callback invocation:

1. `pixels` points to the start of the supplied region, at Canvas-local `x, y`.
   Write exactly `width × height` pixels, advancing rows by `stride_bytes`.
2. Fully initialize the region. Never read unspecified destination pixels as
   preserved background, write row padding, or keep the surface pointer.
3. Keep drawing short and nonblocking. Do not call setters, invalidate, flush,
   stop, or other APIs that mutate GSP state from inside the draw callback.
4. Use the same visual state for every region of one repaint. A callback count
   is not a frame count. Update state in a render-task timer or another
   serialized application callback, then invalidate. External producers need a
   snapshot/ownership protocol; a local copy alone does not make concurrent
   writes safe.

The [media example](../../../examples/usage/sim_bridge_media/README.md) shows a
timer-owned generation and handles both native pixel formats. Its native bridge
draws into a whole offscreen surface; device callbacks may receive partitions.
Do not infer device performance or scheduling from the bridge.

## Registration and shutdown

Use `esp_gsp_on_extension_event_ex()` to select events and Canvas operations:

```c
esp_gsp_err_t result = esp_gsp_on_extension_event_ex(
    ui, observe, state,
    ESP_GSP_EXTENSION_MASK_CANVAS_RESULT | ESP_GSP_EXTENSION_MASK_RENDER_ERROR,
    ESP_GSP_CANVAS_MASK_CHECK | ESP_GSP_CANVAS_MASK_SET_DRAW);
```

Both masks apply to the instance observer only; global hooks remain
independent. The original `esp_gsp_on_extension_event()` still subscribes to all
events/operations known to the runtime. Unknown bits fail immediately and queue
failure preserves the old registration. Masks, callback and context take effect
in one command. Zero masks mute notifications but retain the context; clear the
registration and synchronize before releasing it.

Event and pointer observer registration follow the runtime command path, like
input-interceptor and overlay registration. On ESP-IDF, a successful call from
another task means the command was accepted; it may not yet have taken effect.
Queue failure leaves the previous registration in place. Calls made from a
permitted render-task callback may take effect immediately.

Each registration replaces the previous callback at that endpoint. Keep both
old and new contexts alive while replacement is pending. To release an old
event/observer/interceptor/overlay context from the application task:

1. Successfully clear or replace its registration.
2. Successfully call `esp_gsp_flush()` on the same active runtime.
3. Ensure no other callback or application worker still uses the context.

Do not free the context after a failed unregister or flush. Do not perform this
wait inside a callback or while holding a lock that the callback needs.
Serialize registration and teardown. A bare portable core without a task-owning
adapter requires application-level serialization with app stepping.

The native sim_bridge is a separate client implementation: registration and
callback dispatch are serialized with its application event loop and `poll()`;
it does not use the device command queue. Follow the
[bridge threading contract](../../../tools/sim_bridge/README.md).

For Canvas, successfully submit `esp_gsp_canvas_stop()` and then flush before
freeing draw state. A successful whole-application stop is also a lifetime
boundary. Stop external producers first; borrowed pixel frames remain borrowed
until their release callbacks, even after the producer stops. Follow the
[media ownership contract](lifecycle.md#image-and-canvas-ownership) for submitted frames.

## Compatibility boundary

Existing callbacks and Canvas preserve the compiled scene model. They do not
add arbitrary JSON element types, layout engines, custom opcodes or runtime
format parsers. Compose existing controls and Canvas first. Font, codec,
accelerator and storage provider interfaces in private source headers are not
application extension contracts.

## Borrowed frames with explicit lengths

Camera or worker producers can use `esp_gsp_canvas_submit_frame()` or its
non-blocking `esp_gsp_canvas_try_submit_frame()` variant. They preserve native
Canvas format/dimensions, single-producer ownership and borrowing rules while
also checking the supplied storage extent:

```c
esp_gsp_canvas_frame_t frame = ESP_GSP_CANVAS_FRAME_INIT();
frame.pixels = producer_pixels;
frame.size_bytes = producer_bytes;
frame.stride_bytes = producer_stride;
frame.on_release = return_to_pool;
frame.release_ctx = producer_frame;
/* dirty defaults to NULL; optionally point it at a Canvas-local rectangle. */
esp_gsp_err_t result = esp_gsp_canvas_try_submit_frame(ui, bind, &frame);
if (result != ESP_GSP_OK) {
    return_to_pool(producer_frame); /* No transfer and no framework release. */
}
```

Descriptor and dirty-rectangle values are copied during submission; the
descriptor can be on the stack. Pixels and release context stay valid until
release. Storage must cover `stride_bytes × authored_height`, including the
last row's padding. Stride must fit the runtime's 32-bit field. These APIs do
not convert pixel formats.

ESP-IDF success confirms queue admission. The render task may later reject a
short buffer or incompatible target, preserving the previous drawing and
returning the accepted frame once through its release callback. Queue-full or
other immediate errors do not trigger release. Release may precede the API's
return; do not access/reuse the producer object after successful submission.
The supplied length must be truthful: it cannot detect dangling pointers or an
application overstating its allocation.

Native bridge non-blocking submission performs no size-query RPC. Extents are
checked before upload during poll/a later drain; the previous draw callback
remains until successful upload, and rejected accepted frames are returned.
Bridge upload success means a copy exists, not presentation. Legacy push/try_push
APIs remain available without a storage-length check. Runtime Canvas admission
also requires an exclusive resource view for those APIs.

## Generated Canvas helpers

For a named image with a public resource bind, GSPC emits
`gsp_<scene>_<name>_canvas_set_draw_cb`, `canvas_set_draw_cb_ex`, `canvas_check`,
`canvas_submit_frame`, `canvas_try_submit_frame`, `canvas_invalidate`,
`canvas_invalidate_dirty`, `canvas_push`, `canvas_try_push`, `canvas_push_dirty`,
`canvas_try_push_dirty`, and `canvas_stop` helpers.
They forward to the public APIs with the authored bind ID, preserving their
ownership, synchronization and target requirements. Check the generated
`*.api.json` for the exact helper names and constraints. The presence of a
helper is not confirmation that a particular target accepts direct drawing.

For the media example, registration becomes:

```c
esp_gsp_err_t result = gsp_media_surface_canvas_set_draw_cb(ui, draw, state);
```

## Optional weak business handlers

On ELF targets using GCC or Clang, define `GSP_GENERATE_WEAK_EVENT_HANDLERS`
before including the generated bundle header to enable optional scene
dispatchers. Existing event predicates remain available without this macro.
For a scene named `home` with a `save` callback:

```c
#define GSP_GENERATE_WEAK_EVENT_HANDLERS
#include "bundle_gsp.h"

void gsp_home_on_save(esp_gsp_handle_t ui,
                      const gsp_home_call_event_t *event, void *user_ctx)
{
    /* Handle the action or notify an application worker; do not block. */
    (void)ui;
    (void)event;
    (void)user_ctx;
}

static void app_event(esp_gsp_handle_t ui,
                      const esp_gsp_event_t *event, void *user_ctx)
{
    if (gsp_home_dispatch_call(ui, event, user_ctx)) {
        return;
    }
    /* Handle scene changes, unimplemented actions or other scenes here. */
}
```

Register `app_event` explicitly with `esp_gsp_on_event()` and check the result.
There is no automatic registration or second event listener. The dispatcher
returns true only when a matching implemented handler was called; null events,
other scenes, non-CALL events, unknown actions and missing handlers return false.
The typed event is borrowed for that call only. All ordinary event callback
context and lifetime rules still apply.

Handler declarations are ordinary C-linkage declarations; only internal alias
references are weak. A user definition therefore remains strong, including
when it follows the header in the same translation unit. To override the default
handler prefix, define `GSP_HOME_EVENT_HANDLER_PREFIX my_app_home` before the
header; the handler becomes `my_app_home_on_save`. Use the same prefix in the
dispatcher and handler translation units. This customizes global symbol names,
not per-instance state; use `user_ctx` for instance-specific behavior.

**Static archives need explicit retention.** A weak reference alone does not
extract a handler's archive member. Compile the handler as a directly linked
object, or add `-Wl,-u,gsp_home_on_save` (using the actual overridden name when
applicable) to the final link. Verify your link map. An unretained handler is
absent and the dispatcher returns false; it is not an error from registration.
On other toolchains, use the ordinary callback predicates without enabling the
weak-handler macro.

## Core notification hooks and instance observers

`esp_gsp_extension.h` provides six default no-op core hooks:

- `esp_gsp_hook_app_created`: successful core initialization, before platform attachment.
- `esp_gsp_hook_app_destroying`: destruction begins; resource release callbacks may still follow.
- `esp_gsp_hook_scene_changed`: scene committed, not a veto or presentation fence.
- `esp_gsp_hook_action`: authored CALL actions before the ordinary business handler, without consuming them.
- `esp_gsp_hook_canvas_result`: results of Canvas commands processed by the core.
- `esp_gsp_hook_render_error`: failures reported by the managed ESP-IDF render loop or simulator session.

ELF GCC/Clang applications can provide ordinary strong definitions. Public
declarations are not weak and have C linkage in C++ too. Use hooks for short
diagnostics, application-owned counters or worker notifications. Do not block,
mutate GSP state, flush or stop; hooks do not own framebuffers, validation or
DMA. Ordinary business callbacks retain their existing setter contract.
Core hooks do not require `GSP_GENERATE_WEAK_EVENT_HANDLERS`, which controls
only generated business dispatchers. Provide one strong definition per global
hook and compose modules explicitly there. Different instances may invoke
global hooks concurrently; synchronize shared data (the example uses atomics).

The global hook runs before the corresponding per-instance observer installed
with `esp_gsp_on_extension_event()`. Use `user_ctx` for instance-specific state.
Events are not replayed: APP_CREATED precedes observer registration. Recursive
notifications on the same instance are suppressed and must not be used as business flow.

Creation/destruction hooks run in the task calling create/destroy; other
notifications run on the UI service context. For a portable core or native
simulator session, this is the thread driving the core. Creation does not certify that
platform start succeeded, so do not launch a GSP-accessing worker yet.
Destruction notification is not a release fence; free shared state only after
the whole stop/destroy succeeds. Event pointers are borrowed during the call.

Prefer the project helper for core hook sources:

```cmake
# After creating the application component target. Do not also list these in SRCS.
gsp_add_hooks(${COMPONENT_LIB} SOURCES "app_hooks.c" "extra_hooks.cpp")
```

`gsp_add_hooks()` generates a compile wrapper with a unique strong anchor for
each selected source. Wrappers compile in the existing consumer target, keeping
its compile options, conditional dependencies and generated-header ordering.
Wrappers are excluded from unity merging, so retaining a hook does not pull in
unrelated unity sources. It does not force the entire application archive into
the executable. Plain CMake consumers can include `cmake/gsp_hooks.cmake` from
the component; the target must be a non-imported STATIC library or executable on an ELF target
with a GNU-compatible linker.
Duplicate sources fail configuration; duplicate strong definitions fail linking.
No handwritten anchors are needed. User overrides still use ordinary strong
definitions. The device example uses this helper.

For manual integration, **core weak definitions need a different archive anchor from the generated
weakrefs above.** The default library already defines these hooks, so
`-Wl,-u,esp_gsp_hook_action` alone cannot guarantee extraction of an override.
Link the user object directly or provide a separately named strong anchor in
the same source file:

```c
#include "esp_gsp_extension.h"

void my_hooks_link_anchor(void) {}

void esp_gsp_hook_action(esp_gsp_handle_t ui, const esp_gsp_event_t *event)
{
    /* Observe briefly or notify an application worker. */
    (void)ui;
    (void)event;
}
```

```cmake
target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,-u,my_hooks_link_anchor")
```

If the anchor is defined in a C++ file, give the anchor itself `extern "C"`
linkage too so its symbol matches the linker option.

Non-ELF builds retain default hooks and support instance observers. Native
sim_bridge returns `ESP_GSP_ERR_NOT_SUPPORTED` for core extension observation and tagged Canvas check/registration APIs:
client callbacks cannot replace hooks in the separate runtime process.
Existing Canvas and event APIs remain available. See the
[device custom drawing example](../../../examples/usage/custom_draw/README.md).
