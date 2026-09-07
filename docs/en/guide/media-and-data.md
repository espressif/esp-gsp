# Media and Application Data

ESP-GSP keeps authored structure immutable while allowing product data to
change through bounded state, runtime resources, Canvas producers, and
recycled collections.

## Static images and fonts

Reference assets relative to the scene file:

```json
{
  "type": "image",
  "parent": -1,
  "x": 24,
  "y": 24,
  "w": 64,
  "h": 64,
  "image": "assets/status.png",
  "codec": "auto"
}
```

Set `font` and `default_font_size` on the scene or override them on a text
element. The build tracks referenced assets and rebuilds the bundle when they
change.

Compiled fonts contain the glyphs reachable from authored text. If runtime
text may contain glyphs that are unknown during the build, add a dynamic
TTF/OTF font:

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../scenes/chat.json"
    PIXEL_FORMAT rgb565
    DYNAMIC_FONT "../scenes/assets/NotoSansSC-Regular.otf")
```

Dynamic fonts use additional code, heap, and task stack. Size them from the
actual character set and measured workload rather than enabling them for text
that is already known at build time.

## Dynamic text and scalar state

Give the element a stable `name`, rebuild, and use its generated helper:

```c
ESP_ERROR_CHECK(gsp_status_temperature_set_text(ui, "24.5 C"));
ESP_ERROR_CHECK(gsp_status_battery_set_value(ui, 82));
```

The exact helper depends on the element type and dynamic property. Inspect the
generated header after every scene-interface change. When several scalar
properties must become visible atomically, use the public component batch API
instead of depending on the order of separate asynchronous setters.

## Runtime encoded images

Use a named image and its generated `set_image()` helper for occasional PNG,
JPEG, QOI, or other supported encoded input. The generic public API offers
three ownership models:

| API family | Ownership |
|---|---|
| `esp_gsp_set_image()` | Copies encoded bytes before returning |
| `esp_gsp_set_image_borrowed()` | Borrows bytes until the release callback |
| `esp_gsp_set_image_owned()` | Takes malloc-compatible storage after successful submission |
| `esp_gsp_set_image_ex()` | Selects COPY, BORROW, or TAKE explicitly |

A successful borrowed or TAKE submission can retain the encoded payload while
that image remains current, because decoded pixels may be evicted and decoded
again. If submission returns an error immediately, ownership remains with the
caller and no later release callback runs.

The currently displayed image remains visible while its replacement decodes.
If replacement decoding fails, the previous image stays visible.

## Canvas producers

Use Canvas when an external producer owns continuously changing pixels, such
as a camera preview, video frame, or live raster plot.

The frame-push API accepts complete or dirty frame updates and borrows each
buffer until its release callback. Do not reuse or free a submitted frame
before release.

When the producer can draw a requested region directly, register a direct-draw
callback:

```c
ESP_ERROR_CHECK(esp_gsp_canvas_set_draw_cb(
    ui, GSP_BIND_SCOPE, draw_scope, state));
ESP_ERROR_CHECK(esp_gsp_canvas_invalidate_dirty(
    ui, GSP_BIND_SCOPE, dirty));
```

The callback receives a temporary native RGB565 or RGB888 surface. It runs
inside rendering, must fill every supplied region, must return promptly, and
must not retain the surface pointer. Stop and synchronize before releasing its
context:

```c
ESP_ERROR_CHECK(esp_gsp_canvas_stop(ui, GSP_BIND_SCOPE));
ESP_ERROR_CHECK(esp_gsp_flush(ui, 250));
```

Direct drawing requires an unscaled opaque image target. Use frame submission
when scaling, alpha, or producer ownership makes direct drawing unsuitable.

## Lists and wheels

Lists and Wheels keep a bounded number of visible row instances. The
application binds a logical collection once, publishes content for requested
items, and refreshes when backing data changes.

```c
static const char *s_contacts[] = {"Ada", "Linus", "Margaret"};

static gsp_err_t bind_contact(esp_gsp_handle_t ui, esp_gsp_row_t row,
                              uint32_t item, void *user_ctx)
{
    (void)user_ctx;
    return esp_gsp_row_text(ui, row, s_contacts[item]);
}

esp_gsp_list_t contacts = gsp_home_contacts_bind(
    ui, bind_contact, NULL);
if (contacts != ESP_GSP_LIST_NONE) {
    ESP_ERROR_CHECK(gsp_home_contacts_set_total(
        ui, contacts, sizeof(s_contacts) / sizeof(s_contacts[0])));
}
```

Bind each logical collection once for the UI lifetime. Rows are recycled while
scrolling; the row handle is valid only during the binder call. Use the row
token supplied by the framework so an asynchronous result cannot be published
into a row that now represents another item.

For composite row templates, use `esp_gsp_row_set_text()`,
`esp_gsp_row_set_value()`, `esp_gsp_row_set_color()`, or
`esp_gsp_row_set_image()` with generated template slot constants. After
replacing backing data, call the generated `gsp_<scene>_<list>_refresh()`.

## Image grids and galleries

Declare an image inside a row template with `"dynamic_image": true`. The Grid
binder publishes content only for visible and retained spare cells, so the
runtime does not allocate one image target per item in the dataset.

Use a stable nonzero `cache_key` with `esp_gsp_grid_cell_set_image_ex()` when
immutable encoded content is reused across recycled cells. Change the key
whenever the encoded bytes change. A zero key preserves per-submission cache
behavior.

Size `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS` for simultaneously active
targets—normally visible cells plus overscan—not the total gallery size.

## Message lists

Use `message_list` for chat-like data whose visible rows can have variable
content and extent. Keep the message store in the application. Notify the
framework when data is appended, prepended, replaced, or refreshed rather
than rebuilding the scene structure.

As with Lists and Grids, row publication runs in framework callback context.
Publish already available data and defer storage, networking, or slow decoding
to application tasks.

Initialize `esp_gsp_message_source_t` to zero and set `struct_size` to its
size. The default `flags = 0` retains text-hash validation, including detection
of text changes whose revision was not updated. Legacy source structs without
the `flags` field retain that behavior.

Data sources that maintain reliable revisions may set
`flags = ESP_GSP_MESSAGE_SOURCE_TRUST_REVISION`. An unchanged `id`/`revision`
pair then allows the framework to reuse text measurements without scanning the
text. IDs identify messages rather than array indices; preserve them across
insertion and reordering, and change the revision for text or decoration
changes. Reusing a pair for different content can leave stale pixels. This
still reads each message's metadata; it is not a range-update API.

Keep one coherent source during reconciliation. Publish changes on the render
task or retain an immutable snapshot for readers; the revision flag does not
synchronize background producers. Returned text must remain valid until the
next `get()` call. Append/prepend notifications and failure retries retain
the existing `esp_gsp_message_list_changed()` behavior.

## Capacity and memory rules

- `ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES` covers simultaneously live
  template instances and recycled rows.
- `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS` covers simultaneously active
  runtime image targets.
- List slot and per-row text capacities are configured limits and are checked
  during scene generation where possible.
- `image_cache_bytes` bounds decoded image retention, not the encoded source
  payload owned by the application.
- Disabling the image cache changes which runtime image paths are available;
  review [Configuration](../reference/configuration.md) before using it on a no-PSRAM
  target.

For exact callback and shutdown behavior, see
[Application lifecycle](lifecycle.md).
