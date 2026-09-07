# Application Structure Guide

This page covers the public structures an ordinary application is most likely
to touch. Use generated helpers where possible; public header comments remain
the exhaustive contract.

## Startup configuration

```c
esp_gsp_config_t app = gsp_bundle_config();

esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd.display = display_from_bsp;
lcd.touch = touch_from_bsp; /* Optional. */

ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
```

| Structure | Important members | Guidance |
|---|---|---|
| `esp_gsp_config_t` | generated bundle/directories; runtime capacities and feature opt-outs | Start from `gsp_bundle_config()`. Do not replace generated bundle fields; override only measured policy or capacity. |
| `esp_gsp_esp_lcd_config_t` | `display`, optional `presenter`/`touch`, task settings, `render_alignment` | Start from `ESP_GSP_ESP_LCD_CONFIG_INIT()`. The ordinary path sets the BSP-provided `display`. |
| `esp_display_present_target_config_t` | `hw`, `fb`, `drawbuf` | Normally produced by the BSP. It owns panel facts, presentation mode/framebuffers and draw-buffer policy. |

Both ESP-GSP configuration structures contain `struct_size`; their initializer
sets it correctly. All `esp_gsp_config_t` and `esp_gsp_esp_lcd_config_t`
members, defaults and trade-offs are listed in
[Configuration reference](../reference/configuration.md).

## Events and input

| Structure | Members to use | Lifetime/rule |
|---|---|---|
| `esp_gsp_event_t` | `type`, `action_id`, `arg`, `scene_id`, `list`, `item` | Borrowed during `esp_gsp_on_event()`. Prefer generated event predicates; `list`/`item` identify a recycled-row event. |
| `esp_gsp_pinch_event_t` | `phase`, `center_x`, `center_y`, `relative_scale_q16` | Borrowed during the pinch callback. Accept BEGIN to capture; the framework reports scale but does not apply it. |

These callbacks run on the render task. Do not retain the pointer or perform
blocking product work there.

## Runtime image ownership

`esp_gsp_image_options_t` controls one encoded-image submission:

| Member | Use |
|---|---|
| `ownership` | COPY clones the input; BORROW waits for release; TAKE transfers only after successful submission. |
| `on_release`, `release_ctx` | Required for BORROW; TAKE uses the callback when supplied. |
| `on_complete`, `complete_ctx` | Optional terminal result for an accepted request. |
| `cache_key` | Nonzero immutable-content identity; change it when bytes change. |

Null options select COPY. Release/completion callbacks may run on render or
decode tasks and must not block. After an immediate API error, ownership stays
with the caller.

## Application-backed collections

| Structure | Important members | Guidance |
|---|---|---|
| `esp_gsp_message_t` | `text`, stable `id`, changing `revision`, `direction` | Filled by the application; text only needs to remain valid during `get()`. |
| `esp_gsp_message_source_t` | `struct_size`, `count`, `get`, optional `decorate`, `user_ctx`, `flags` | Zero-initialize and set `struct_size` to `sizeof(esp_gsp_message_source_t)`. Callbacks run on the render task; `flags = 0` retains text-hash validation. |
| `esp_gsp_row_t` | `list`, `slot`, `instance`, `item` | Framework token for one recycled row; pass it unchanged to row setters. |
| `esp_gsp_grid_cell_t` | row token plus resolved resource/text slots | Pass it to generated/public cell setters; do not construct it manually. |

Row and Grid tokens are callback-scoped. Do not retain them for asynchronous
publication or treat recycled `slot` as dataset identity.

## Canvas and generated descriptors

| Structure | Important members | Guidance |
|---|---|---|
| `esp_gsp_canvas_surface_t` | `pixels`, `stride_bytes`, `x`, `y`, `width`, `height`, `pixel_format` | Temporary draw destination. Fill the supplied region and never retain `pixels`. |
| `esp_gsp_template_desc_t` | generated `id`, `width`, `height`, `max_instances` | Read-only generated metadata for `esp_gsp_widget_create_template()`. |
| `esp_gsp_component_info_t` | `key`, `kind`, `capabilities`, `property_count` | Advanced runtime-generic inspection; ordinary code should use typed helpers. |

Canvas draw callbacks run on the render task and may receive partitioned
regions. They must not call state-changing or waiting GSP APIs.

## Ownership rule of thumb

- Configuration is application-created from generated/BSP initializers;
  referenced bundle, font and display resources follow their documented
  lifetime.
- Events, pinch data, row/cell tokens and Canvas surfaces are framework-owned
  callback views.
- Image options and message-source contexts are application-owned and must
  survive the callbacks they register.
