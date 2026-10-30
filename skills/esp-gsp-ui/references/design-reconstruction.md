# Reconstructing an independent design

Use ordinary scene JSON and the application's selected GSPC contract. No editor
plugin or intermediate runtime format is required. A screenshot is a visual
reference unless the user identifies it as an actual product asset.

Establish logical dimensions/pixel format, supplied font files and original
assets, dynamic fields/character sets, page states, actions and acceptance
references. Discover existing project facts and reuse existing behavior first.
When the picture omits behavior, state reasonable assumptions and continue the
authorized implementation. Ask only when a choice materially changes product
semantics and cannot be inferred from the project or user request.

Prefer native Label/Button/Slider/Toggle/List/Grid objects for text, controls and
data. Use original PNG/SVG assets for decoration. The compiler's vector SVG
subset and explicit static raster mode have different capabilities: inspect
the matching media guide, and use `gspc svg-info` when relevant. A static image
does not preserve the dynamic semantics of objects baked into it.

Keep stable names and `parent_name`; preserve Bundle scene order/symbol across
PC and ESP-IDF integration. Generate headers/API sidecars before writing C.
Compiler-supported Slider `track_inset`/`track_radius` and Toggle
`track_size`/`knob_size` can adapt appearance while preserving existing hit
geometry and legacy defaults. Check current cards/schema before using them.
Use child Labels when button text requires an explicit offset.
Choose one owner for a selected button: `checkable: true` supplies an automatic
click toggle; an application-controlled button uses `selected_image`/`checked`
and its C setter without also enabling that automatic toggle. Writing the same
state from a synchronous click callback and a later declarative toggle can
produce a different result from an asynchronous PC Backend.

SVG text `y` denotes a baseline; GSP object `y` denotes a box origin. Account for
the selected font's ascent/descent and box height rather than copying the
baseline into object coordinates. Keep native dynamic text and the charset
needed for future values. GSP layout constraints are evaluated during compile;
runtime text changes do not implicitly reflow sibling objects.

For design-fidelity acceptance, freeze independent reference images, original
asset/font hashes, states and critical visual regions. Without independent
references, continue implementation and behavior checks, and leave fidelity
unverified. Implementation captures cannot serve as the original design oracle.
Compare regions as well as the whole frame; inspect text presence/readability,
crop/focus, clipping and paint order.
Different rasterizers and RGB565 quantization require explicit tolerances,
not an unsupported promise of universal pixel equality.

Use actual pointer input and the shared native C Backend for application-owned
state. Verify callbacks, state projection and return/re-entry. A Toggle's
`checked` property can change before its thumb finishes animating: wait for
numeric `value` 0/100 before a settled screenshot. A property write from the
automation channel does not prove an application callback works.

The source checkout contains `examples/usage/design_reference` and
`tools/design/check.py` for a fresh build, input/assertion trace, seven frozen
reference captures, regional visual comparisons and C-state observations.
`tools/design/README.md` defines its verification manifest. This is an optional
source development tool; do not assume it is installed with a managed component
or force its example structure onto an existing application.

Separate scene/API/PC evidence from an owning ESP-IDF build and real-device
evidence. The source-only device diagnostic freezes an already-validated PC
implementation for cross-platform checks; those pixels do not replace the
independent design references. Offscreen render time is not panel FPS.
