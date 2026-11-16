# Migrate LVGL scenes to native GSP

Follow the requested scope: assessment, one page, or an application migration.
An assessment lists each affected page's source behaviors, proposed native GSP
mappings, gaps and risks, followed by a suggested migration order. It changes no
project files. Implementation produces native scenes, assets and application
integration for the affected flow.

## Establish enough source context

Read the page's creation, update, event and teardown paths, following wrappers
and business callers where they affect behavior. Identify the selected LVGL from
actual headers/version macros, dependency locks and build metadata; aliases and
dependency ranges alone do not establish a version. Inspect configuration,
local patches, style defaults and decoders where relevant. The user need not
know the version; probe include paths only to resolve a material ambiguity.
Missing dependencies leave specific facts unknown while understood work continues.
An exact patch version is unnecessary when the affected semantics are established.
Resolve destination tools, target and IDF requirements through
[project context](project-context.md). Screenshots establish appearance, not
hidden business rules; ask only for product choices missing from the project
and conversation.

## Preserve behavior and select native representations

Use the selected GSP version's cards and generated API. Runtime uses determine
state, dynamic declarations, capacity and ownership; creation code alone is
insufficient. These are decisions for the affected flow, not a complete API map:

| LVGL use | Native GSP decision |
| --- | --- |
| Fixed hierarchy and styles | Scene objects, layouts/styles/components; check affected state/part combinations and hit/paint order. |
| Runtime property updates | Stable names and required dynamic declarations; check units and bounds. A name alone does not make all fields writable. |
| Repeated children or data collections | Templates or collections with bounded simultaneous instances; dataset size is separate from live capacity. |
| Content-dependent layout | Check current reflow semantics; compile-time layout or dynamic geometry alone does not imply runtime sibling reflow. |
| Events, timers, navigation or custom drawing | Preserve submission timing, state ownership and lifecycle; use native actions, animation or Canvas where appropriate. |
| Images and fonts | Prefer original assets. Decode C arrays/private binaries only after identifying their descriptor, encoding, channel order, stride and alpha semantics. Check runtime text and converted font coverage; declare required characters with `font_charset` or `font_charset_file` and review charset warnings. |

Retain reusable business code and interfaces; use native GSP APIs by default.
Follow [application integration](application-integration.md) for callback
ownership and asynchronous updates, including activation before state restoration.
Reuse compatible BSP/display/input setup. Renderer handover is relevant only
when the product requires both frameworks. Check remaining consumers before
removing old UI code or dependencies.

## Keep the result usable in its project

Keep visual intent unless redesign was requested; explain intentional theme,
motion or layout changes. Prefer original assets and retain their licenses.
Follow project build conventions with relative resource paths or declared shared
assets. For a new standalone project or changed asset layout, check a fresh copy
with only declared dependencies; routine edits need no separate relocation check.

## Verify and hand off at the requested scope

Follow the main Skill's build checks and reuse successful evidence with unchanged
inputs. Exercise changed behavior through actual input and application logic:
submission timing, navigation/re-entry and relevant boundary/error states.
Reuse a PC Backend where suitable; create an adapter only when needed within scope.
Simulator property writes or a second business model do not validate application
logic. Use adapter logs/getters for test observation, not extra scene objects.
See [preview and automation](preview.md) for that path.

Compare equivalent settled states with original LVGL captures when available.
Missing captures leave fidelity unverified; requested redesign uses its own
independent targets. Keep Host, application build, board appearance and measured
performance/resource benefits distinct.

Report changed sources, build/preview entry points, preserved behavior and gaps;
use a short behavior-to-result checklist for complex flows. Distinguish unverified
implementation from unsupported/deferred work at the requested scope.

The source checkout's `examples/usage/design_reference/README.md` (or `README_zh.md`)
provides an optional example and LVGL baseline runner; its theme, runner version
and fixture structure are not customer requirements. Maintainer qualification
is described in `dev_docs/agent_ui_evaluation.md`.
