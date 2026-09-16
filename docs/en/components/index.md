# Widget library

This page lists every Widget currently registered by GSPC. Each page provides a real example, exact fields and C APIs, and a published simulator preview command.

## Value controls

| Widget | JSON type | Purpose | Example |
|---|---|---|---|
| [Arc](./arc.md) | `arc` | Circular value indicator with an optional draggable range. | [`arc.json`](../../../examples/widgets/arc/arc.json) |
| [Checkbox](./checkbox.md) | `checkbox` | Independent checked state with a square visual affordance. | [`checkbox.json`](../../../examples/widgets/checkbox/checkbox.json) |
| [Progress](./progress.md) | `progress` | Read-only horizontal or vertical representation of a ranged value. | [`progress.json`](../../../examples/widgets/progress/progress.json) |
| [Radio](./radio.md) | `radio` | Circular boolean choice visual commonly coordinated by application state. | [`radio.json`](../../../examples/widgets/radio/radio.json) |
| [Slider](./slider.md) | `slider` | Draggable horizontal or vertical ranged-value control. | [`slider.json`](../../../examples/widgets/slider/slider.json) |
| [Toggle](./toggle.md) | `toggle` | Switch-style checked state with a draggable knob. | [`toggle.json`](../../../examples/widgets/toggle/toggle.json) |

## Basic

| Widget | JSON type | Purpose | Example |
|---|---|---|---|
| [Button](./button.md) | `button` | Clickable content surface that triggers declarative actions or application callbacks. | [`button.json`](../../../examples/widgets/button/button.json) |
| [Image](./image.md) | `image` | Static or animated raster asset with contain, cover and stretch fitting. | [`image.json`](../../../examples/widgets/image/image.json) |
| [Label](./label.md) | `label` | Static or runtime-bound UTF-8 text with alignment and overflow control. | [`label.json`](../../../examples/widgets/label/label.json) |

## Data and choices

| Widget | JSON type | Purpose | Example |
|---|---|---|---|
| [Carousel](./carousel.md) | `carousel` | Orbit icon picker with drag, depth ordering and snap. | [`carousel.json`](../../../examples/widgets/carousel/carousel.json) |
| [Dropdown](./dropdown.md) | `dropdown` | Compact single-choice control that opens an option panel. | [`dropdown.json`](../../../examples/widgets/dropdown/dropdown.json) |
| [Grid](./grid.md) | `grid` | Virtualized multi-column collection backed by a reusable cell template. | [`grid.json`](../../../examples/widgets/grid/grid.json) |
| [List](./list.md) | `list` | Scrollable fixed or application-backed vertical collection. | [`list.json`](../../../examples/widgets/list/list.json) |
| [Message List](./message_list.md) | `message_list` | Virtualized conversation viewport with incoming and outgoing bubbles. | [`message_list.json`](../../../examples/widgets/message_list/message_list.json) |
| [Wheel](./wheel.md) | `wheel` | Single-column collection with row snapping and optional cyclic selection. | [`wheel.json`](../../../examples/widgets/wheel/wheel.json) |

## Visualization

| Widget | JSON type | Purpose | Example |
|---|---|---|---|
| [Charging Orb](./charging_orb.md) | `charging_orb` | Charging progress with liquid, halo or plasma styles and optional particles. | [`charging_orb.json`](../../../examples/widgets/charging_orb/charging_orb.json) |
| [Chart](./chart.md) | `chart` | Bounded single- or multi-series line chart. | [`chart.json`](../../../examples/widgets/chart/chart.json) |
| [Clock](./clock.md) | `clock` | Analog clock face with independently configurable hands and ticks. | [`clock.json`](../../../examples/widgets/clock/clock.json) |
| [Effect](./effect.md) | `effect` | Pulse, rotating ring, liquid progress or shimmer with automatic playback. | [`effect.json`](../../../examples/widgets/effect/effect.json) |
| [Flip Card](./flip_card.md) | `flip_card` | Two compiled image faces with an interruptible perspective flip. | [`flip_card.json`](../../../examples/widgets/flip_card/flip_card.json) |
| [Frosted Glass](./glass.md) | `glass` | Build-time frosted crop of a named static image backdrop. | [`glass.json`](../../../examples/widgets/glass/glass.json) |
| [Needle](./needle.md) | `needle` | Rotating gauge hand with configurable pivot, angle and tail. | [`needle.json`](../../../examples/widgets/needle/needle.json) |
| [Rectangle](./rect.md) | `rect` | Lightweight rectangular visual with fill, border, radius and opacity. | [`rect.json`](../../../examples/widgets/rect/rect.json) |
| [Shape](./shape.md) | `shape` | Rectangle, rounded geometry, circle or line rendered without an image asset. | [`shape.json`](../../../examples/widgets/shape/shape.json) |
| [Spinner](./spinner.md) | `spinner` | Indeterminate animated activity indicator. | [`spinner.json`](../../../examples/widgets/spinner/spinner.json) |

## Layout

| Widget | JSON type | Purpose | Example |
|---|---|---|---|
| [Container](./container.md) | `container` | Styled parent with optional rectangular child clipping and automatic layout. | [`container.json`](../../../examples/widgets/container/container.json) |
| [Layer](./layer.md) | `layer` | Full or partial visual layer used for overlays and visibility groups. | [`layer.json`](../../../examples/widgets/layer/layer.json) |

## Navigation

| Widget | JSON type | Purpose | Example |
|---|---|---|---|
| [Drawer](./drawer.md) | `drawer` | Edge-attached overlay that opens and closes with pointer gestures. | [`drawer.json`](../../../examples/widgets/drawer/drawer.json) |
| [Page Flow](./page_flow.md) | `page_flow` | Swipeable multi-page viewport with horizontal or vertical motion. | [`page_flow.json`](../../../examples/widgets/page_flow/page_flow.json) |
| [Stack View](./stackview.md) | `stackview` | Push and pop page stack with animated navigation transitions. | [`stackview.json`](../../../examples/widgets/stackview/stackview.json) |
| [Tab View](./tabview.md) | `tabview` | Compile-time tab bar and swipeable page content. | [`tabview.json`](../../../examples/widgets/tabview/tabview.json) |

## Composites

| Widget | JSON type | Purpose | Example |
|---|---|---|---|
| [Keyboard](./keyboard.md) | `keyboard` | Compile-time on-screen keyboard assembled from native controls. | [`keyboard.json`](../../../examples/widgets/keyboard/keyboard.json) |
| [Message Box](./msgbox.md) | `msgbox` | Compile-time modal dialog with title, message and action buttons. | [`msgbox.json`](../../../examples/widgets/msgbox/msgbox.json) |
| [Table](./table.md) | `table` | Compile-time table assembled from labels, cells and grid lines. | [`table.json`](../../../examples/widgets/table/table.json) |
