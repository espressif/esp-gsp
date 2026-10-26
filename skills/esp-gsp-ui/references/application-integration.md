# Application data and generated API integration

## Connect data and assets with predictable behavior

Use the selected version's widget page and generated header to confirm these
patterns before applying them:

- **Wheel data:** use `dynamic_items: true` when items change at runtime so the
  generated collection API remains available for empty, short and long lists.
  Bind once; use compiled text through the documented default binding or a row
  callback for application data. Selection uses logical item indexes. Let the
  compiler manage row slots. Use `cyclic` when continuous scrolling is wanted.
- **Chart data:** declare the series, value range and point capacity in JSON;
  use generated series setters or append helpers for updates. Keep sampling in
  the application and choose an update rate appropriate to the display.
- **Dynamic text:** declare the characters needed by future values, size the
  text area using the selected font metrics, and check the documented multiline
  alignment/overflow combinations before integration. Preview long, empty and
  representative translated values when relevant.
- **SVG and images:** distinguish runtime vector Image capabilities from
  bitmap assets used by carousel/effects. Inspect compiler diagnostics for the
  source element and cause. If raster export is needed, preserve the requested
  appearance and confirm that it still supports the required interaction or
  animation; do not silently discard those behaviors.
- **Canvas drawing:** direct callbacks may run once per partition. Use
  object-local coordinates and `stride_bytes`, initialize every supplied
  region, and keep one consistent state across the repaint. Advance animation
  or consume samples before drawing. Follow the selected version's target,
  callback and shutdown contracts; the PC bridge's offscreen preview does not
  verify device partition timing or memory use.
- **Image memory:** inspect `*.execution.json` resources and
  `resource_budget.source_variants` before changing assets or cache budgets.
  Follow the selected component's `guide/media-and-data.md` for encoding,
  decoding and budget rules; verify changes with `esp_gsp_media_stats()` and
  the target's free heap and largest free blocks.

When a requested control or style is absent, check the current public contract
and offer the closest composition with its concrete behavior differences.
Implement a composition when it satisfies the request; ask when the choice
would change the intended user experience.

## Validate before integrating C

Validate meaningful JSON changes through the target-configured compile/preview
or owning build, using its diagnostics and generated artifacts for integration.
Reuse a successful check while inputs, scene set and resolved options remain
unchanged. Pass target capabilities only when confirmed by the project or user.

When the project uses shared component JSON files, pass the same
`COMPONENTS_DIR` to device `gsp_add_bundle()` and PC `gsp_add_backend()`, using
paths relative to each calling `CMakeLists.txt`. Reuse the project's component
definitions and parameters; inline definitions remain valid. Generated contexts
carry this directory, so context-based checks need no extra component flags.

For a check without generating artifacts, prefer the registered Bundle's
generated context when available:

```sh
gspc diagnose --context path/to/bundle_gsp.context.json
```

Otherwise diagnose the actual scene paths with the project's confirmed Profile
and capacity arguments, for example `gspc diagnose scenes/main.json --format json`.

CLI `gspc diagnose` includes profile-sensitive compilation checks. For daemon
clients, `diagnostics/pull` defaults to a schema-only quick check. If
`doctor/get` advertises `diagnostic_levels` containing `compile`, request
`level: "compile"` with the selected context or confirmed profile;
otherwise use CLI diagnosis or actual compilation. An unsupported level is
not a clean diagnostic result.

Fix diagnostics at their reported JSON paths. Apply `suggestions` when present.
Use the properties defined by the installed ESP-GSP Schema, `gspc cards`, and
widget reference.

Without a generated context, pass actual scene paths to CLI diagnosis; empty input
is rejected. For daemon
previews, check the result's `success`, not just the response's `ok`. Verify
`inputIdentity`, `artifactIdentity` and `stale` before accepting a returned
Bundle. A stale artifact belongs to an earlier source snapshot and is only a
display fallback. Give `compile/preview` a stable `path`; fallback is scoped to
the same root, path and resolved compiler options. `compile/previewProject`
reports identities and success per dirty screen; no dirty screens means no new
compile, and an unopened project is rejected.

Generate the bundle and headers through `gsp_add_bundle()` or the matching GSPC
before using new application APIs. Inspect compiler diagnostics, execution
requirements, `<symbol>_gsp.h`, and per-scene headers. Use the declared typed
setters, event predicates, collection binders, and template helpers. Validate
application changes through the owning ESP-IDF build.

Fix source JSON or compiler inputs and regenerate outputs. Prefer generated
helpers for named controls; use public generic APIs with generated keys for
batch updates or data-driven operations.

For runtime movement, `esp_gsp_component_set_position()` requires both x and y
to have bounded dynamic declarations. A literal axis rejects the whole update;
use the individual property setter when only one axis is dynamic. Check the
generated properties before choosing the API.

Keep callbacks non-blocking and follow COPY, BORROW, and TAKE ownership. Treat
setters as asynchronous submissions on ESP-IDF. Use `esp_gsp_flush()` only for
an explicit synchronization boundary.

A navigation submission does not activate the destination immediately. Restore
destination state and attach its keyboard from `ESP_GSP_EVENT_SCENE_CHANGED`,
checking `event->scene_id`, rather than calling destination setters immediately
after `esp_gsp_goto_scene()`. Keep the handler non-blocking and check API results.

For bindings or callbacks required before the first frame, use the selected
version's `esp_gsp_esp_lcd_start_prepared()` contract in
`guide/runtime-api.md`; check setter results and avoid flush or lifecycle calls
inside prepare. For timer teardown, deletion does not wait for an in-flight
callback. Follow that guide's timer lifecycle rules before releasing `user_ctx`:
the external-task delete/flush barrier requires successful calls and a
continuously attached runtime, and does not cover other users of the context.

## Keep implementation documentation product-facing

Keep implementation documents focused on public behavior, actual dependencies,
verified evidence and genuine product or framework constraints. When work is
incomplete or blocked by the environment, record the pending check and status
in the validation or handoff section rather than describing an unrun check as a
product limitation.
Keep temporary workspace state and authoring decisions in the validation report.
