# Runtime and generated API

## Start from generated configuration

`gsp_<symbol>_config()` supplies bundle bytes, component directories and JSON-derived requirements. Pair it with `ESP_GSP_ESP_LCD_CONFIG_INIT()` and a BSP-created display target, then call `esp_gsp_esp_lcd_start()`.

```c
esp_gsp_config_t app = gsp_bundle_config();
esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd.display = display;
lcd.touch = touch;

esp_gsp_handle_t ui;
ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
```

## Generated object operations

GSPC generates operations only when the Widget and authored dynamic properties support them. A named Slider may produce `get_value()` and `set_value()`; a named Label with runtime text may produce a text setter; callbacks produce event predicates. Inspect the generated header or the exact signatures shown on each [Widget example page](../components/index.md).

Setters are asynchronous in ESP-IDF. Success means the command was accepted, not necessarily already presented. `esp_gsp_flush()` is for deterministic tests, captures and explicit synchronization boundaries—not every update.

Slider and Arc drag update retained value state, but the current public runtime
does not automatically emit an application value-change callback. Generated
getters and setters remain valid; when product logic requires a drag
notification, treat it as a capability gap instead of assuming that a
`callback` fires for every value step.

## Events and task context

Event, list-binding, image-release, timer and Canvas callbacks run on framework tasks. Do not block, access slow storage or perform networking inside them. Send a queue item or task notification to product logic and return.

Use `callback` for application-owned Back behavior and `stack_pop` for a
StackView. The authored `back` action is reserved and is not dispatched by the
current public runtimes.

```text
UI event → short callback → application task → product state change
                                           → generated setter → render commit
```

## Dynamic content

| Content | API path |
|---|---|
| Named scalar/text/visibility/selection | Generated object helper |
| Application-owned rows or cells | List/Grid/Wheel/Message List adapter |
| Occasional encoded image | Generated image setter or `esp_gsp_set_image*()` |
| Camera/video/continuous pixels | Canvas frame or direct-draw callback |
| Page or stack navigation | Generated navigation/component helper |

Buffer ownership must use the documented COPY, BORROW or TAKE mode. Stop producers, detach callbacks and release borrowed resources before `esp_gsp_stop()` completes.

## Shut down cleanly

Application code owns external producers and should stop them before stopping the UI. Do not call blocking teardown from a framework callback. See the detailed [application lifecycle](lifecycle.md) and [API reference](../reference/api.md).
