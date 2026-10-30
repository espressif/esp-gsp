# Target-frame Update Receipts

Include `esp_gsp_updates.h` when an application must observe a group of required
updates. Each receipt records the furthest stage reached and a separate outcome.

| Stage | Meaning |
|---|---|
| `ACCEPTED` | The group owns its copied commands and has entered the UI queue. |
| `APPLIED` | Every required operation succeeded, including asynchronous image publication. |
| `RENDERED` | A successfully committed render contains the required target states. |
| `TRANSFERRED` | The backend confirmed that frame's transfers. |
| `PRESENTED` | Correlated hardware refresh feedback confirmed the tracked frame. |

Query `esp_gsp_update_capabilities()` before selecting a completion stage.
The simulator provides `RENDERED`; SPI/QSPI GRAM panels provide transfer
confirmation. Framebuffer backends with hardware VSYNC can provide `PRESENTED`. Unsupported stages
are rejected before admission and are never silently downgraded.

`PRESENTED` uses the presenter's binding generation and frame ID. RGB waits for
the switch ticket followed by two hardware VSYNCs; MIPI follows the same rule.
MIPI drivers without hardware VSYNC, including P4 rev < v3, report
`TRANSFERRED` as their maximum stage. Query the current SDK/target configuration's
capabilities. While confirmation is pending, the same presenter holds the tracked
frame and delays the next frame, including frames from ordinary updates. This
reduces throughput when every goal requests `PRESENTED`; use the earliest stage
that meets the application's needs. Untracked operation keeps its existing
pipeline. Tracking adds no task or framebuffer.
The scanout clock callback is enabled on the first tracked display submission
and retained until the display binding is torn down.

## Submit a group

Use generated scene-prefixed component keys for scalar `PROPERTY` commands.
`VALUE`, `COLOR`, `VISIBLE`, `TEXT` and `IMAGE` use generated bind IDs. When these
are needed, define `GSP_BUNDLE_ENABLE_RAW_IDS` before including the bundle header
and use its scene-prefixed constants. A `SCENE` command must be first, because a
target frame belongs to one scene. Subsequent binds refer to that target scene.

```c
#include "esp_gsp_updates.h"
#include "bundle_gsp.h"

/* Replace the component keys with those emitted for the application's scene. */
static const esp_gsp_update_command_t changes[] = {
    {
        .kind = ESP_GSP_UPDATE_PROPERTY,
        .property = {
            .component = GSP_HOME_OBJ_KEY_LEVEL,
            .property = GSP_PROPERTY_KEY_VALUE,
            .value = {.type = GSP_VALUE_I32, .data.i32 = 75},
        },
    },
    {
        .kind = ESP_GSP_UPDATE_PROPERTY,
        .property = {
            .component = GSP_HOME_OBJ_KEY_PANEL,
            .property = GSP_PROPERTY_KEY_VISIBLE,
            .value = {.type = GSP_VALUE_BOOL, .data.boolean = true},
        },
    },
};

esp_gsp_update_receipt_t *receipt = NULL;
esp_gsp_update_options_t options = {
    .complete_at = ESP_GSP_UPDATE_RENDERED, .timeout_ms = 1000,
};
esp_gsp_err_t error = esp_gsp_update_submit(
    ui, changes, sizeof(changes) / sizeof(changes[0]), &options, &receipt);
if (error == ESP_GSP_OK) {
    /* Retain receipt for query, or arrange a terminal callback in options. */
}
```

A zero `complete_at` selects `RENDERED`. Pure `PROPERTY` groups reuse the atomic
scalar transaction. Mixed groups execute in order and can leave partial state
on failure; they do not roll back earlier updates. `applied_count` records
confirmed progress, and `failed_command` identifies an individual failure or
is `UINT32_MAX` for a group-wide failure. Existing property codecs, ranges,
text wrapping and clipping rules continue to apply.
For repeated commands with the same kind and bind, or the same component/property
for `PROPERTY`, the last value is the required target.

The terminal outcomes are `COMPLETE`, `FAILED`, `SUPERSEDED`, `CANCELLED` and `TIMED_OUT`.
Check the outcome, not only submission success or the last reached stage.
If another update replaces the target before its matching frame, the receipt
reports `SUPERSEDED`. An unchanged target can reuse a matching committed frame;
queue emptiness alone is never a render witness.

`failed_stage` identifies the stage that could not be established, separately
from the last reached `stage`: `APPLIED`, `RENDERED`, `TRANSFERRED` or `PRESENTED`.
It is zero except for `FAILED` and `TIMED_OUT`. A failed render attempt terminates
its receipt; ordinary renderer recovery continues and applications may submit a
new goal. `error` is `GSP_OK` while pending and preserves the terminal cause.

`timeout_ms` bounds acceptance-to-observed-evidence time, including queueing and
pauses; zero disables the deadline. Expiry is observed at the next UI service,
and returns `TIMED_OUT` with `GSP_ERR_TIMEOUT`. Evidence first observed at or after
the deadline does not turn expiry into success. This deadline differs from a
caller's wait timeout, which only ends that wait. Neither timeout rolls back
applied state or frees inputs still in use. Cancelling or expiring the last goal
tracking a frame withdraws its display-confirmation hold, preserving the ordinary
buffer ownership fences; late feedback cannot change a terminal receipt.

## Ownership and lifetime

The native service allocates tracking state on first use. At most 16 pending goals
and 128 commands per goal are admitted. Admission does not wait for queue space;
queue or goal-capacity pressure returns `ESP_GSP_ERR_TIMEOUT`.

Commands and UTF-8 strings are copied. Image `COPY` inputs are copied once;
accepted `BORROW` and `TAKE` inputs follow the existing image release contract.
An immediate error leaves the output receipt NULL and retains caller ownership
of those inputs. A completed frame receipt does not release an image still
used by the scene: keep its release context alive until `on_release` runs.
Terminal receipts retain their status. Command copies are reclaimed at completion
unless an in-flight image callback still needs their entries.

`on_complete` receives a terminal snapshot on the UI/render task, or on the
caller of stop when shutdown cancels work. Its context must remain alive until
notification. The output receipt is installed before notification can run,
including when the render task runs before submit returns. Callbacks must not
block, flush or stop their own UI.

`esp_gsp_update_query()` reads a coherent snapshot while the caller owns a
receipt reference. Retry later if it returns false under contention. Serialize
UI teardown with submission calls. A retained receipt remains queryable after
its UI stops. Release it with `esp_gsp_update_release()`; releasing the caller's
reference does not cancel the work. `esp_gsp_update_cancel()` requests cancellation
without undoing state or reclaiming decoder/DMA-owned inputs prematurely.

Pause preserves pending receipts but suspends their servicing and callbacks.
On resume, cancellation and elapsed deadlines are checked before further goal
work. A failed pause that resumes the same UI follows the same rule. Quiesce
waits for buffer ownership fences; it does not promote a receipt to `PRESENTED`.
Replacing the display binding invalidates old frame generations, and stopping
the UI cancels outstanding goals. Serialize submission with pause/resume and
teardown; do not submit to a paused UI. Retained receipts may still be queried
or cancellation requested during a pause. Terminal callbacks resume with service
or run during stop. The native header contract is `ESP_GSP_UPDATE_API_VERSION`;
use headers and implementation from the same component build.

## Simulator and AI operation

The named RPC surface provides `update_submit`, `query_update`, `wait_update`,
`cancel_update`, `release_update` and `update_capabilities`. For example, submit
`commands` containing `{"kind":"property","name":"level","property":"value","value":75}`
and `{"kind":"text","name":"caption","text":"Ready"}`. Pass the returned
opaque `id` unchanged to subsequent calls.
`update_submit` accepts the same optional `timeout_ms`; status includes
`failed_stage` and `failed_stage_code` for structured failure handling.

`wait_update` succeeds only for `COMPLETE`; failures, cancellation and replacement
return errors with the observed receipt. `max_frames` bounds stepped frames,
and a wait timeout does not cancel the goal. Runtime reloads/restarts invalidate
old IDs. Only the originating connection can cancel or release its goals;
read-only observers may query them. Disconnection cancels and releases the
connection's retained goals.
Call `release_update` after consuming a terminal result, including a failed result.
The simulator retains at most 64 receipts across its connections, including
completed goals. Releasing a pending receipt does not cancel its work.

The real C Backend implements the same API through `gsp_sim_bridge`. Its normal
open/poll thread contract applies: call `gsp_sim_bridge_poll()` while idle so
receipt notifications and image releases are delivered. Native simulator
receipts confirm rendered output; use reference/region image comparison for
visual acceptance and device feedback for physical-display acceptance.
Completion confirms the required state in a frame, not the end of an animation
or agreement with a reference design. Check those postconditions separately.

`esp_gsp_flush()` remains a queue and render-attempt barrier. It is useful for
its existing synchronization role; target groups provide their own explicit
state and frame evidence.
