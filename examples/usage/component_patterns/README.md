# Reusable project component patterns

This 480x320 scene shows five editable project components loaded from a
`--components-dir`: three settings-row variants, an icon button, and a badge.
They expand to ordinary Container, Label, Button, Image and Toggle objects; no
new runtime widget type is introduced.

Use a GSPC build matching this component. Run from the component root.


```sh
python -m pip install -U esp-gsp-tools
gspc diagnose \
  examples/usage/component_patterns/settings.json \
  --components-dir examples/usage/component_patterns/components
mkdir -p gsp-out/component-patterns
gspc pack \
  examples/usage/component_patterns/settings.json \
  --components-dir examples/usage/component_patterns/components \
  --api-header gsp-out/component-patterns/settings.h \
  --deployable -o gsp-out/component-patterns/settings.gspb
python -m gsp.execute --version 1.6.0 sim \
  --bundle gsp-out/component-patterns/settings.gspb \
  --api-json gsp-out/component-patterns/settings.api.json
```

`params` entries are required argument names. These definitions use whole-string
`$param` tokens to preserve each argument's JSON type. GSPC also supports
[prefixed text substitution](../../../docs/en/guide/scenes.md#reusable-project-components).
Parameter substitution does not evaluate expressions or supply missing arguments.
The root's `name` is
provided at each `use` site. The row internals use local numeric parents to
keep the call sites small; only trailing values or toggles that the application
may update receive explicit unique-name parameters.

To copy the example into an application, preserve the scene-relative asset
layout or update the two paths in `settings.json`:

```sh
mkdir -p my_app/ui/components my_app/ui/scenes my_app/common/fonts my_app/ui/widgets/image
cp examples/usage/component_patterns/settings.json \
  my_app/ui/scenes/
cp examples/usage/component_patterns/components/*.json \
  my_app/ui/components/
cp examples/common/fonts/DejaVuSans.ttf my_app/common/fonts/
cp examples/usage/widgets/image/vector_icon.svg \
  my_app/ui/widgets/image/
```

With that layout, compile `my_app/ui/scenes/settings.json` with
`--components-dir my_app/ui/components`; its existing relative font and icon
paths resolve to the copied files.

The badge accepts already formatted short display text. Numeric range or text
clamping belongs in the application; the JSON component only lays out and
clips the supplied label. These layouts are compile-time geometry: changing a
parent width at runtime does not reflow sibling rows.

See [README_zh.md](README_zh.md) for the Chinese version.

The value and arrow rows are buttons with native press feedback. The toggle row
has one interactive control: tapping its text does not change the switch or
emit its callback. The example emits `settings_info`, `settings_network`,
`settings_appearance` and `settings_notifications`; application handlers supply
navigation and persistence. Read the toggle's checked property in the handler
rather than treating the callback argument as the checked state.

The generated header exposes text setters for `network_value` and `status_badge`
and checked accessors for `notifications_toggle`. The badge requires a `name`
argument because that name also supplies its text binding. Format count limits
in the application (for example `99+`). Dynamic text uses the declared baked
`font_charset`; edit that set and the font for other languages or symbols.

The icon button places its icon before the label using `icon_size` and `gap`.
Give the button enough width for its padding, icon, gap and label; edit the
component's row order for another arrangement. Reuse the same callback for
multiple instances only when the application intentionally shares their action.

These patterns use existing renderer operations, but expanded controls and
mutable text still consume state, text-slot, font and bundle capacity. Static
labels can omit bindings in a copied definition when runtime updates are not
needed. No device-side layout engine or global component cache is added.
