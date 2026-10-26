# Navigation and Viewports

ESP-GSP provides several navigation mechanisms. Choose the narrowest one that
matches the interaction.

| Mechanism | Purpose | Runtime API |
|---|---|---|
| Scene navigation | Switch between complete screens | `esp_gsp_goto_scene()` |
| PageFlow / TabView | Move between pages inside one scene | generated page helper or `esp_gsp_page_flow_set_page()` |
| StackView | Push and pop pages inside one scene | generated stack helpers |
| Drawer | Reveal a panel from a screen edge | generated drawer helpers |
| List / Wheel | Scroll recycled rows or choices | generated and generic list APIs |

## Scene navigation

List all scenes in one bundle in navigation order:

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../scenes/home.json"
           "../scenes/settings.json"
    PIXEL_FORMAT rgb565)
```

Navigation may be triggered by an authored `goto` action or by
`esp_gsp_goto_scene()`. Horizontal scene swiping is enabled by default and can
be disabled per scene with `"swipe": false` or at runtime with
`esp_gsp_set_swipe_enabled()`.

Available scene transitions include directional slides, cross-fade, and
slide/fade-through-black. Cross-fade needs enough memory for both scene
snapshots; if allocation fails, it degrades to the zero-snapshot
fade-through-black path instead of retaining both scenes.

## Viewport components

PageFlow, TabView, StackView, Drawer, List, and Wheel move content inside a
bounded viewport. Their authored children move and clip together, and only the
affected viewport is invalidated.

PageFlow and Drawer follow the pointer during a drag and settle after release.
Their release policy matches scene navigation: a deliberate quarter-extent
pull commits by position, while a shorter flick uses recent velocity or
displacement. A confirmed reversal cancels before the final quarter of travel.
Settle duration is velocity-adaptive (normally 80–220 ms), so a phone-style
flick completes promptly and a slow drag still lands smoothly.
In polling mode, the ESP-LCD touch adapter confirms release after the configured
number of consecutive empty reports (default two). The default masks a single
controller dropout and adds one active input poll to a real finger lift.
Interrupt mode consumes fresh IRQ reports only and accepts a fresh empty report
as release without waiting for a second interrupt.
StackView changes its active page through push/pop operations. List and Wheel
recycle visible rows, so application row binders must use the row token and
must not retain a row handle after the callback returns.

## Cross-page animation

A jump between non-adjacent pages freezes the source and target viewports into
two snapshots and slides between them, so the pages in between never appear.
The source and target share one contiguous two-frame atlas, allocated from
PSRAM when available. Budget `2 × width × height × pixel bytes`, plus alignment
and any repair metadata, and check the largest free block as well as total free
memory. Composition paints directly into each presenter band, without a third
full-size staging frame.

Progress is normalized to one viewport regardless of how many pages the jump
crosses. A three-page jump therefore animates over the same settle as a
one-page jump instead of appearing three times faster.

When both frozen frames do not fit, a jump of more than one viewport lands
instantly rather than scrolling live: losing the animation is preferable to
dragging the intermediate pages across the screen. Single-viewport jumps and
drags still animate without a snapshot, because only the two neighbouring pages
can enter the viewport.

Endpoint lists retain their rows while the snapshot is active. If a content
update invalidates a multi-page snapshot, navigation lands at its target before
live rendering resumes. Adjacent motion and Drawer can continue from their
current offset. A new navigation request or press finishes a pending multi-page
jump before using its position, so hit testing cannot target an unseen page.
Repeated allocation failures back off; one successful capture
restores the normal retry cadence.

Navigation triggered from scene JSON honors `"animated": false` on `set_page`,
`stack_push`, `stack_pop`, `drawer_open` and `drawer_close`, matching the
`animated` argument of the corresponding C API.

Overlay drawers are a different split: the base page stays put and only the
panel travels, so a band is the base frame plus whichever slice of the panel
has entered. This behavior is consistent for drawers on all four edges.

An animated Drawer open/close request can reverse a running settle from its
current position. Repeating the current target does not restart the animation.
Use the component motion event to observe completion instead of polling before
each request.

## Gesture priority

Interactive children take priority over their container. The effective order
is:

```text
control drag -> list/wheel scroll -> drawer/page flow -> tap -> scene swipe
```

A slider inside a drawer therefore changes the slider instead of moving the
drawer. Scene swipe runs only when no component has claimed the gesture.
Click-driven overlays such as Dropdown claim their authored hits before scene
navigation, so opening or selecting an option cannot become a page swipe.
Before ownership locks, the dominant axis selects scrolling or paging, so
small cross-axis drift does not steal the gesture.

Use `block_scene_swipe` on a visible `layer` when a modal or detail layer must
prevent top-level navigation.

## Runtime positioning

Named components can be moved with `esp_gsp_component_set_position()`,
`esp_gsp_component_translate()`, or their generated helpers when available.
Position updates are asynchronous like other setters. Use
`esp_gsp_flush()` only when a deterministic boundary is required.
