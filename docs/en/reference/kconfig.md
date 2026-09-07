# ESP-IDF Kconfig Guide

ESP-GSP exposes product capacities, feature defaults, task settings, and
interaction tuning under:

```text
Component config -> ESP-GSP
```

Use these settings for constraints shared by the whole firmware. Keep
scene-specific choices in JSON, bundle-specific choices in `gsp_add_bundle()`,
and values that genuinely vary between application instances in the public
runtime configuration.

The same menu exists for a source-free component. Every visible setting is
implemented by the consuming-project bridge and remains editable and effective.
Fixed arrays, stack storage, and archive layout limits are not shown as false
Kconfig controls; they are published as generated `ESP_GSP_BUILD_CAP_*`
constants and checked against the bundle at build time and startup.

## Configure interactively

From the application project root:

```sh
idf.py menuconfig
```

Open `Component config -> ESP-GSP`, change the required values, save, and
rebuild:

```sh
idf.py build
```

`menuconfig` writes the selected values to the project's active `sdkconfig`.
That file, not `sdkconfig.defaults`, is the effective configuration of an
existing build. Confirm what the build actually selected with:

```sh
rg '^CONFIG_ESP_GSP_' sdkconfig
```

If the project sets the CMake `SDKCONFIG` variable, inspect that selected file
instead. The corresponding compile-time header is generated at
`build/config/sdkconfig.h` for the default build directory.

## Keep product settings reproducible

Use `menuconfig` to explore settings. Record intentional product defaults in
`sdkconfig.defaults` so clean builds and other developers receive the same
baseline. For example, the following values describe a hypothetical product
that has measured requirements for six simultaneous List/Grid controls, up to
40 slots in one viewport, and 24 live template instances:

```ini
# sdkconfig.defaults
CONFIG_ESP_GSP_MAX_LISTS=6
CONFIG_ESP_GSP_LIST_MAX_SLOTS=40
CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES=24
```

These numbers are an example, not recommended defaults. Size each pool from
the largest simultaneously active scene rather than the total dataset. Keep
the component defaults until compiler diagnostics or measurements show a
different requirement.

After interactively preparing a new product profile, this ESP-IDF command can
write non-default selections to `sdkconfig.defaults`:

```sh
idf.py save-defconfig
```

Review the resulting diff: it represents the whole project's changed defaults,
not only ESP-GSP settings.

### Target-specific defaults

Put settings that apply only to one chip in
`sdkconfig.defaults.<IDF_TARGET>`, for example
`sdkconfig.defaults.esp32p4`. When `sdkconfig.defaults` exists, ESP-IDF loads
the matching target-specific file automatically after it. Do not list that
automatic target file a second time.

This target-specific example moves both framework task stacks to PSRAM:

```ini
# sdkconfig.defaults.esp32p4
CONFIG_ESP_GSP_RENDER_TASK_STACK_PSRAM=y
CONFIG_ESP_GSP_DECODE_TASK_STACK_PSRAM=y
```

Use those choices only when the profile enables PSRAM and FreeRTOS static
allocation. Keep the internal-SRAM default when render callbacks may access
flash, NVS, or filesystems, or when code must run while the flash cache is
disabled. A PSRAM render-stack allocation failure makes runtime startup fail;
a PSRAM decode-stack allocation failure leaves the background worker disabled.

To layer an additional product or board fragment, pass a semicolon-separated
list:

```sh
idf.py -D 'SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.product' build
```

Each listed file also gets its own automatic `.<IDF_TARGET>` lookup. When
overriding `SDKCONFIG_DEFAULTS`, include `sdkconfig.defaults` explicitly if the
standard project defaults must still apply.

### Existing `sdkconfig` files

Defaults initialize a new setting or fill one that is absent; they do not
replace a value already stored in `sdkconfig`. After changing a defaults file,
either update the active value through `menuconfig`, or validate the defaults
with an isolated build/configuration pair:

```sh
idf.py -B build_esp32p4 \
  -D SDKCONFIG=build_esp32p4/sdkconfig \
  -D SDKCONFIG_DEFAULTS=sdkconfig.defaults \
  set-target esp32p4 build
```

Using a separate build directory avoids confusing an older root `sdkconfig`
with the profile being tested.

## Understand the configuration path

The consuming project's `sdkconfig` remains authoritative for both source and
prebuilt use. CMake compiles it into a small, independent project-config bridge;
it is never compiled into `libesp-gsp.a`. The archive contains the complete
feature set and named build capabilities only. Therefore changing a policy or
heap-backed capacity does not require rebuilding the archive.

Each UI instance resolves one immutable snapshot in this order:

```text
project bridge -> GSPB requirements -> application overrides -> build caps
```

Requirements may raise only schema fields marked AUTO. A non-AUTO capacity is
validated against the bundle minimum but is not silently raised. Policy,
dirty-region capacity, and build-capability fields are illegal in a GSPB
requirements member. A 0.1.x bundle has no versioned requirements member and
is rejected; rebuild it with the 0.2.0 `gspc`.

`CONFIG_ESP_GSP_LIST_MAX_SLOTS=0` means AUTO/no extra project restriction.
`gspc` computes the scene requirement, the runtime allocates at least that
amount, and the serialized `uint16_t` row-slot count is the final format bound.
Multiple scenes use the maximum single active-scene requirement rather than a
sum. `DIRTY_RECT_CAPACITY` is not AUTO because a scene cannot predict the
number of dirty rectangles in a frame; overflow safely coalesces or redraws.

`ESP_GSP_CONFIG_INIT()` and generated `gsp_<bundle>_config()` initialize ABI
metadata and an empty override set; they do not copy Kconfig into the public
structure. Common overrides use eight inline entries. Advanced callers can
bind a caller-owned, read-only extension table for additional entries. Stable
field IDs, not structure offsets, define this override ABI.

### What a slot means

`slot` is a family of bounded runtime storage units, not one global count and
not the total number of objects authored in JSON. Each pool answers a different
simultaneous-use question:

| Slot family | What consumes one | How it is sized |
|---|---|---|
| Scene state slot | A dynamic bind, visibility gate, transform value, component property or dynamic theme value | Written into each compiled GSB; no project Kconfig count is required |
| `TEXT_SLOTS` | One simultaneously shaped non-template dynamic text | AUTO from the maximum active-scene requirement |
| `LIST_MAX_SLOTS` | One recycled visible List/Wheel row or Grid cell | AUTO from viewport geometry and template `max_instances`; Grid includes every visible column |
| `LIST_TEXT_SLOTS` | One dynamic text field inside one recycled row/cell | AUTO from the row template; total row text storage scales with rows × text fields |
| `CONTEXT_DEFAULT_INSTANCES` | One simultaneously live copy of a template, including recycled collection entries | AUTO from the maximum active-scene template demand |
| `INSTANCE_STATES_PER_SLOT` | One dynamic value carried by each template copy | Compiler checks the largest template against this project/build limit |
| `CONTEXT_DEFAULT_GLYPH_RUNS` | One shaped-text run used by direct text, collection rows or internal runtime text | AUTO from `text + list_count × rows × row_text + 2` |
| `DEFAULT_DYNAMIC_IMAGE_SLOTS` | One named runtime image target that may hold a published image | AUTO from compiled image binds; application-created targets must be added explicitly |
| `CANVAS_SLOTS` | One concurrently bound external frame producer | Firmware policy because a scene cannot infer application-created producers |
| Timer, widget, animation and component-instance slots | One live application timer, runtime widget handle, active animation or transient gesture/tween behavior | Firmware policy based on measured simultaneous use |

For a multi-scene Bundle, GSPC records the maximum requirement of one active
scene instead of summing mutually exclusive scenes. List glyph-run storage is
the exception within a scene: it includes every simultaneously visible row and
every dynamic text field in those row templates.

The resolver applies the layers in a fixed order:

```text
project Kconfig -> GSPB minimum requirement -> instance override -> library cap
```

- For an AUTO field, project value `0` accepts the GSPB-derived requirement.
- A non-zero project value is explicit. A value below the compiled requirement
  fails at build time or startup instead of silently truncating the scene.
- An instance override may raise or specialize an overrideable field, but it
  cannot go below the GSPB minimum or above the published library capability.
- Adding more data items to a List does not require more row slots; increasing
  simultaneously visible rows, columns, template fields or application-created
  live objects can require more capacity.

This is why a source-free component remains extensible: common scene demand is
derived automatically, while product-created objects can use Kconfig or the
generated/runtime override APIs without changing the prebuilt library ABI.

### Which layer should own a value?

Use this order to minimize manual configuration and keep applications
unrestricted:

1. Put structure and content in JSON. The compiler derives List/Grid slots,
   dynamic text, runtime image targets, instances, glyph runs, scene count and
   StackView depth where the schema marks the field AUTO.
2. Use Kconfig for a firmware-wide policy or capacity that cannot be inferred,
   such as task stacks, task priorities, cache policy, dirty-region capacity,
   input thresholds, or application-created objects outside authored scenes.
3. Use `esp_gsp_config_set()` for a bundle-specific or runtime-selected value.
   Eight overrides are stored inline; an application-owned extension table
   keeps the same field/value mechanism open-ended without enlarging every
   instance.

Removing Kconfig would force product policy into code and make source-free
users less friendly. Keeping only effective settings visible, while deriving
scene demand from JSON, avoids both that regression and duplicated manual
tuning.

## Choose the setting that matches the constraint

| Requirement or symptom | Start with | Important consequence |
|---|---|---|
| More simultaneously bound List/Grid controls | `CONFIG_ESP_GSP_MAX_LISTS` | Enlarges resident per-list state |
| More visible/recycled rows in one viewport | `CONFIG_ESP_GSP_LIST_MAX_SLOTS` | Applies per List/Grid; Grid slots include all columns |
| More live copies of a template created through the Widget API | Set that template's JSON `max_instances` to the maximum simultaneous count | `gspc` adds every template quota to the GSPB pool requirement; no pool arithmetic is needed |
| An externally compiled/custom bundle whose requirement metadata cannot be regenerated | `CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES` or `esp_gsp_config_set(..., ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES, value)` | Advanced compatibility escape hatch; the value is the total shared pool capacity, not an increment |
| More dynamic text | `CONFIG_ESP_GSP_TEXT_SLOTS`, `CONFIG_ESP_GSP_LIST_TEXT_SLOTS` | `ESP_GSP_BUILD_CAP_TEXT_CAPACITY` is only the inline command threshold; longer strings use framework-owned heap storage |
| Application pinch handling | Always available since 0.2.0 | `ESP_GSP_BUILD_CAP_MAX_TOUCH_POINTS` is two; product settings do not compile pinch out |
| Runtime PNG/JPEG or compiled JPEG | `CONFIG_ESP_GSP_ENABLE_IMAGE_CACHE` and its budget | Disabling the cache makes those paths unavailable in region-decode mode |
| Lower internal-SRAM use | First reduce measured pool demand; then consider PSRAM task stacks | Do not shrink stacks without high-water-mark evidence |
| Lower decode-task memory | `CONFIG_ESP_GSP_ENABLE_ASYNC_DECODE` | Disabling moves decode work to the render task and may increase frame latency |
| Lower transition peak memory | `CONFIG_ESP_GSP_ENABLE_TRANSITION_SNAPSHOTS` | Disabling changes supported transitions to the no-snapshot path |
| Different touch or motion feel | `*_SLOP_PX`, `SCROLL_*`, `SWIPE_*`, cadence values | Requires real-panel touch and motion validation |

The complete grouped symbol map and memory relationships are in
[Configuration reference](configuration.md). The help text beside each
`menuconfig` symbol remains the authoritative per-symbol reference.

## Validate a configuration change

Use the narrowest evidence that proves the change:

1. Inspect the active `sdkconfig`, not only the defaults fragment.
2. Build the selected target so Kconfig dependencies, scene generation,
   compilation, and linking all run.
3. Exercise the largest scene and the feature whose pool or switch changed.
4. Measure stack high-water marks before reducing task stacks.
5. Validate touch thresholds, scheduling, acceleration, and motion on the real
   board; a host build cannot prove their feel or timing.

Report configuration, target build, board execution, and visual/touch
acceptance separately.
