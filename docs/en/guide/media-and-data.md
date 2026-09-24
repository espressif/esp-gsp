# Media and Application Data

ESP-GSP keeps authored structure immutable while allowing product data to
change through state updates, runtime resources, Canvas producers, and
recycled collections.

## Static images and fonts

### Image storage and memory

`raw` (also called `store`) keeps native pixels in the bundle. With `mmap_direct`,
the renderer reads them directly; their Flash size is not a decoded-cache charge.
Compressed images save Flash, while whole-image decoding needs pixel memory.
For RGB565, a 240×240 image needs 115,200 bytes; a separate A8 plane adds 57,600
bytes. Row alignment and JPEG decode alignment can increase these values.

When the image cache is enabled, each static compressed raster must fit the
declared cache budget. Several images can share the cache through LRU eviction;
their combined size may exceed the budget. Frequent eviction can increase decode
work. Reserve `preload` for resources that need to remain resident.

On memory-constrained targets, region decoding handles supported lossless images
without retaining every whole image. Select it with `image_cache_enabled: false`
in the profile or `--default-disable-image-cache` for bundle compilation. JPEG
uses the whole-image cache. Automatic encoding selects native pixels for image
fit/rotation in region mode; explicitly compressed transformed images require
the cache. When loading binaries directly, use the same cache mode in the runtime
configuration. Bundle API headers apply the selected mode in their config helper.

`gsp_add_bundle(... IMAGE_CACHE_BYTES 262144)` sets an explicit runtime cache
budget and the compiler's matching budget, including with an explicit `PROFILE`.
The CLI equivalent is `--image-cache-bytes 262144`. Setting only the Profile's
`image_cache_budget_bytes` constrains compilation; the runtime still derives
its budget automatically. Pass `IMAGE_CACHE_BYTES` when both budgets must match.
Automatic sizing also depends on available heap and its largest free block.
Check firmware size separately from decoded-image memory.

With image caching enabled, the runtime synchronously prepares the scene's
currently visible compiled compressed images before its first frame, then uses remaining cache space to prefetch hidden images. An
incomplete hidden-image prefetch is normal and only appears in debug diagnostics.
If the visible decoded working set exceeds the budget, the warning reports the
budget, resident bytes and skipped count; skipped visible images can show the scene
background placeholder on the first frame. A single image larger than the budget
reports its required decoded bytes. Increase `IMAGE_CACHE_BYTES` only with heap
headroom, or reduce decoded dimensions/use `raw` or region decoding where appropriate.

Linked bundles share encoded assets. Each scene registers its own
compiled image resources, including hidden controls and templates. Images used
exclusively by other scenes do not enter its startup preparation or cache sizing.
Recompile existing bundles to apply per-scene resource registration.

The manifest and execution report separate registered resources, scene draw references
and initial visibility:

- `largest_static_decode_bytes`: largest static decode among the scene's draw references.
- `largest_registered_static_decode_bytes`: largest static decode in the scene's registered resource set.
- `initial_visible_static_decode_bytes_estimate`: total for unique static images intersecting
  the screen and viewports in the default state, before application updates.

Draw references include all pages and templates. The initial estimate follows selected
pages, closed drawers and hidden groups. Budget preloaded and retained images, application-created instances, runtime
images and animation/vector buffers separately. Use `esp_gsp_media_stats()` to inspect
runtime cache usage and peaks. `runtime_peak_bytes`
is reserved for runtime measurements and is null in compiler-only reports.

Prefer the portable codec names `auto`, `speed`, `size`, `raw`, `lossless`, and `jpeg`.
With a separately installed GSPC, check that `gspc compatibility` lists the
requested modes in `compiler_features.image_policy_modes`. The source-tree
compiler supports these modes.
`auto` selects STORE, QOI/RLE, or hardware JPEG using target capability, cache
budget and compression gain. `speed` weighs runtime work: STORE for small
images, QOI/RLE when compression is strong, and hardware JPEG/JPEG+A8 when
eligible. `size` compares actual encoded bytes and can use software JPEG.
JPEG quality defaults to 85; set `quality` (1–100) to change it. For exact
pixels, use `lossless` or set `image_auto_allow_lossy: false` in the Profile.

Hardware JPEG needs both dimensions to be at least 64 pixels. For RGB888
alpha, `auto` and `speed` also require 16-pixel alignment before choosing
JPEG+A8. `size` and explicit `jpeg` accept unaligned dimensions but may need
MCU scratch storage at decode time. `size` includes that scratch estimate with
the decoded image when checking its candidate budget; other allocations still
affect the actual memory peak. JPEG+A8 has lossy colour and lossless alpha.
Compiled `size` resources do not use PNG: the decoder needs a
full-image inflate buffer and cannot decode a region. Comparing only encoded
bytes would understate device peak RAM. PNG remains available for runtime input.
The compiler and diagnostics also accept the existing names `store` (`raw`),
`qoi` (`lossless`), `default` (`auto`), and explicit `rle16` (opaque RGB565),
`rle16_a8` (alpha RGB565), and `rle32` (RGB888). An incompatible explicit RLE
request is a compile error.
The ARGB8888 overlay profile stores native pixels only; other explicit codecs
and `store_scale` are diagnosed at compile time.
`cache_policy` also applies to `auto`, `speed`, and `size`. `mmap_direct` is
valid only for STORE; requesting it for a compressed result is a compile error.
Existing scenes may specify `preload` or `decode_lru` for STORE, but STORE still
uses direct mapping and does not enter the decoded-image cache. New scenes can
omit those settings for STORE.
`hardware_jpeg` retains its compatibility behavior: JPEG with a hardware
decoder, lossless encoding otherwise. It is not an alias for explicit `jpeg`.

### Reading scene resource budgets

The current `*.execution.json` contract is schema version 2. It reflects the
resource reachability and nullable decode-size fields emitted by the current
GSPC; consumers should select the schema by `schema_version` rather than by
the GSPC product version.

Each `*.execution.json` includes a `resource_budget` summary. `resource_blob_bytes`
counts this scene's resource blobs, including their headers; `image_payload_bytes`
and `font_blob_bytes` identify the image payload and font portions. These are not
firmware Flash totals: scene GSB, bundle tables/alignment and runtime code are
separate, and shared blobs must not be summed across scene reports.

`resources[].source_path` identifies the input behind a compiled resource.
`inclusion` distinguishes an initial draw, a draw/template reference, and a
scene-addressable resource. These labels describe compiled reachability, not
actual preparation or residency. `source_variants` groups multiple encoded
variants of one source; their sizes and codecs help identify expensive authored
uses, but different variants are not necessarily redundant.

`initial_static_set_exceeds_cache_budget` is advisory: it compares the initial
static decode estimate against an explicit enabled cache budget. It is null for
an automatic or disabled cache. An excess can cause eviction; it is not a measured
RAM peak or proof of allocation failure. Review `esp_gsp_media_stats()` before
increasing memory. `requested_codec: "generated"` means per-resource authoring
provenance is unavailable; the profile's encoding policy is reported separately.

### Compiled SVG Images

Start with the [vector image example](../../../examples/usage/widgets/image/vector.json).

Use `.svg` in an Image's `image` field. GSPC stores these images as compiled
curves for runtime rendering.
The compiler records the required binary format versions in the output;
use the [compatibility contract](../reference/compatibility.md) when pairing tools.

Carousel, Flip Card and effect image sets rasterize accepted SVG inputs during compilation.
Their resulting bitmap assets follow bitmap sizing and cache rules. Choose a named
Image for runtime curve scaling, tinting or morphing.

SVG paths retain their curves and are rasterized at the requested size. Use a
decoded-image cache budget large enough for the visible working set when
reusing large vectors: an untinted cached surface uses four bytes per pixel,
and a tinted mask uses one. Vector surfaces share the scene's image-cache
budget with decoded images and in-flight decodes; bitmap decodes reclaim
vector surfaces first. If the budget or allocator cannot accommodate a
surface, rendering continues with bounded scratch tiles.

Use a named image with `fit`, `rotation`, and `scalable` as for raster images. For
runtime box resizing, author `w` and `h` as bounded dynamic fields with semantic
properties such as `width` and `height`; GSPC generates typed setters. Box
resizing changes the drawing area; `scale` zooms inside that area and is clipped
to it. Declared bounds must fit the scene's coordinate range.
Width/height use the framework's existing 0..100 bounded-property interpolation;
wide ranges therefore quantize pixel sizes. Keep bounds close to the intended
sizes, or use the Q16.16 scale setter for fine uniform zoom inside the box.

An optional `tint` color turns a scene image into a monochrome silhouette and
generates a `set_tint()` setter. Without `tint`, source colors and transparency remain.
Template SVG images use static colors and dimensions.

The vector importer supports paths, basic shapes, solid fills, fill rules,
static transforms and strokes expanded at compile time. Prepare artwork as follows:

- Convert text and non-scaling strokes to paths in the source editor.
- Keep painted bounds, including strokes and inherited transforms, inside the
  SVG viewport. The import diagnostic identifies the element and any overflow.
- Export artwork that needs embedded images, external references, gradients,
  filters, active clips/masks or multi-paint group opacity as PNG to preserve
  those effects. Reference that PNG as a bitmap image.
- Keep the Image object's `opacity` at 255; use source paint alpha for transparency.

Editor metadata, foreign-namespace content and unused gradient definitions are
accepted. Single-paint group opacity is folded into paint alpha. A rectangular
clip that contains all group paint is redundant and can be removed during import.
Other unsupported drawing semantics produce a diagnostic, so select a compatible
`svg_element` or supply the prepared bitmap.

Raster `codec`, `store_scale` and `cache_policy` options apply to bitmap inputs.
For vectors, edit source geometry and use the Image size, rotation and scale properties.

Use `svg_element` to select a source element/group ID without splitting the
asset into separate files. Inherited transforms are preserved and the result
is cropped to its painted bounds. Scene `x/y/w/h` place and size this cropped
image, not the original SVG viewport. Give each independently controlled part
its own named Image and reuse position, rotation, visibility and color APIs.
Duplicate IDs, empty IDs and selections without painted geometry are errors.

For composite artwork, use `svg_layout: "canvas"` and give all parts the same
authored `x/y/w/h` canvas rectangle. GSPC derives placement and emits tightly
bounded curves with subpixel alignment preserved; it does not retain a full
canvas pixel cache per part. X/Y move that part's canvas position; image
rotation and in-box scaling operate on the selected part. Canvas layout
clips rotation/zoom to the image box. It requires `svg_element`, static W/H and the default `fit: "stretch"`. Authored
style/hit bounds remain the canvas rectangle; define interactive regions on
parent controls explicitly. Use `content` layout for template images.
Omitting the field, or choosing `content`, retains cropped-image placement.

Run `gspc svg-info artwork.svg`, optionally with `--element iris`, to inspect
canvas size, element IDs, stroke-inclusive normalized bounds, path/point counts, cache estimates
and compatibility reasons. The JSON `supported` field indicates whether the
source geometry can be compiled. The default element list is
capped at 256 entries; `elements_truncated` reports truncation and `--element`
can inspect a specific ID. This checks the source, not a scene's authoring
options, a morph pair or the final target budget; build the actual scene too.
A viewport-overflow reason identifies the source file and element (or unnamed
selection), then reports the viewport, selected bounds, actual painted bounds
and excess on each side.
A selected compatible part can be imported without
rendering unrelated decoration groups that need unsupported effects.

`morph_to` supplies an end-state SVG; `morph` is the initial 0..100 percent
progress, defaulting to zero. Generated `set_morph()`, `animate_morph()` and
`animate_morph_to()` use the existing property/animation APIs. See the
[vector motion example](../../../examples/usage/widgets/image/vector_motion.json).
The runtime interpolates curve points without XML parsing or full-frame
bitmap sequences. Both states must have identical viewport dimensions,
paint order/colors, contour structure and segment types. Preserve path start
points and directions. Duplicate the source and edit node positions to keep
corresponding points aligned; the compiler checks structural compatibility. Selected elements use a shared union
of both endpoint bounds, so morphing does not recrop or shift the image.
Use morphing on named scene Images; template images use static geometry.

Vector Image input supports up to 2 MiB, 128 fill/stroke draws and 8192 outline points.
Vector images support RGB565/RGB888 scenes. Rendering cost and memory use grow
with image dimensions and complexity; measure continuous scaling, rotation and
morphing on the target device. See [memory configuration](../reference/configuration.md).

Use the execution plan's `vector` summary to inspect asset complexity and memory
estimates. These estimates do not include total application memory. Keep SVGs
self-contained and drive animation through GSP properties. Compare imported
artwork with a reference image.

Vector applications require additional Flash and RAM. The render-task stack is
automatically raised when needed (at least 32 KiB with the supplied build);
larger application settings remain effective. Include this in memory planning.
Source builds without dynamic font support link only the outline rasterizer;
enabling dynamic fonts, including in prebuilt packages, retains the full font
engine. Compare firmware sizes using the same build mode and font features.

### Raster Images and Fonts

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

For runtime text with a known vocabulary, set `font_charset_file` on the scene
or text object to a UTF-8 corpus such as `assets/ui-words.txt`. Paths are relative
to the scene; an object path overrides the scene path. GSPC merges its characters
with `font_charset` and authored text, removes duplicate characters, and ignores
an initial BOM and CR/LF line separators. Editing the corpus rebuilds the bundle.
The generated `set_text()` helpers use these baked glyphs without FreeType.

The optional scene-level `font_max_bytes` sets a positive byte limit for the sum
of generated GFB resources across all fonts and sizes, before external linking.
An over-budget build reports the required bytes; it does not remove characters
or change rendering quality. This budget excludes dynamic font files, glyph-run
storage and runtime caches. Existing resource reports list each font's size,
glyph count and character set; compiler diagnostics identify missing codepoints.

Compiled fonts contain the glyphs reachable from authored text. If runtime
text may contain glyphs that are unknown during the build, add a dynamic
TTF/OTF font:

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../scenes/chat.json"
    PIXEL_FORMAT rgb565
    DYNAMIC_FONT "../scenes/assets/NotoSansSC-Regular.otf")
```

Dynamic fonts are an optional link feature, including in prebuilt components.
`DYNAMIC_FONT` enables it automatically; applications supplying a font blob at
runtime must call `gsp_enable_freetype()` in CMake. AOT-only applications do not
link the full font engine. The outline rasterizer used by vectors remains
available independently.

With dynamic font fallback, each static GFB can contain up to 32768 glyphs.
UI startup rejects larger packs in this combination. Static-only GFB packs
can contain up to 65535 glyphs.

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

Give the image a `name`, a `bind`, and `bind_target: "resource"`, then use its
generated `set_image()` helper for occasional PNG,
JPEG, QOI, or other supported encoded input. The generic public API offers
three ownership models:

| API family | Ownership |
|---|---|
| `esp_gsp_set_image()` | Copies encoded bytes before returning |
| `esp_gsp_set_image_borrowed()` | Borrows bytes until the release callback |
| `esp_gsp_set_image_owned()` | Takes malloc-compatible storage after successful submission |
| `esp_gsp_set_image_ex()` | Selects COPY, BORROW, or TAKE explicitly |

The explicit `bind_target` keeps the binding on the image resource when the
object also has a background color. An image without a background color or text
can omit it.

A successful borrowed or TAKE submission can retain the encoded payload while
that image remains current, because decoded pixels may be evicted and decoded
again. If submission returns an error immediately, ownership remains with the
caller and no later release callback runs.

The currently displayed image remains visible while its replacement decodes.
If replacement decoding fails, the previous image stays visible.

The authored placeholder, not the replacement, fixes the decoded pixel format.
An opaque PNG may replace a transparent placeholder (alpha becomes 255). An RGBA
PNG replaces an opaque placeholder only when every pixel is opaque; a PNG with
transparent pixels is rejected there. Author a transparent placeholder when the
replacement needs transparency.

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

After binding a visible list, `esp_gsp_list_fling()` can be called immediately;
the command activates the list before starting motion. To preserve visible
history when prepending messages, keep their IDs stable and pass the inserted
count to `esp_gsp_message_list_changed()`. The framework rebinds recycled rows
to the corresponding messages as it adjusts the scroll anchor.

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
  target. For no-cache runtime QOI updates, use a fixed-size `fit: "stretch"`
  placeholder and encode replacements at its native dimensions. Fitting/scaling an encoded
  runtime replacement requires decoded pixels in the image cache. A successful setter only
  confirms submission; use `esp_gsp_set_image_ex()` completion to confirm
  publication and diagnose rejected replacements.

For exact callback and shutdown behavior, see
[Application lifecycle](lifecycle.md).


## JPEG animation frames

GIF/APNG images accept these `animation_codec` policies:

| Value | Compilation behavior |
|---|---|
| `lossless` | Force QOI delta patches; also the default for uncalibrated `auto` targets |
| `jpeg` | Full JPEG frames, even when larger than QOI |
| `hardware_jpeg` | JPEG when the target profile declares `hardware_jpeg: true`, otherwise QOI |

```json
{"type":"image","name":"motion","x":0,"y":0,"w":256,"h":256,
 "image":"assets/motion.png","animation_codec":"hardware_jpeg","quality":85}
```

Opaque animations use JPEG. If any frame has transparency, the entire animation uses JPEG+A8: lossy color and lossless alpha, with no silent transparency removal. GIF/APNG frames are composited onto complete canvases before encoding; frame durations and loop counts are preserved. `max_fps` still caps the imported frame rate. Explicit `codec: "jpeg"` also works for animations; `animation_codec` takes precedence when both are present. With `animation_codec`, omitted `quality` uses the profile's `jpeg_quality`.

JPEG mode decodes and redraws complete frames. It trades resource space for access to hardware decoding; sparse UI animations may still benefit more from QOI patches. `animation_frame_budget_bytes` limits decoded-frame storage, not encoded Flash size; inspect resource reports for encoded size. Reports distinguish `anim_qoi`, `anim_jpeg`, and `anim_jpeg_a8`.

Without `animation_codec`, `size` compares the complete QOI delta and
JPEG/JPEG+A8 payloads. `auto` and `speed` consider hardware JPEG only when the
Profile sets `animation_speed_hardware_jpeg: true`, changes cover most of each
frame, frames are opaque, and dimensions are MCU-aligned. `auto` also requires
at least 20% encoded-size savings; `speed` permits up to 10% growth. The board
setting defaults to false and should follow a measurement of its display path.
Set `hardware_jpeg: true` and `animation_speed_hardware_jpeg: true` in a
measured board Profile to enable this choice.
Transparent animations keep QOI automatically; use explicit
`animation_codec: hardware_jpeg` to request JPEG+A8. An explicit
`animation_codec` always takes precedence.
Animations use a separate frame buffer; `cache_policy` and `store_scale` are
static-image options and are rejected on animations.

JPEG color conversion can differ between hardware and software decoders. Use
`lossless` for icons, brand colors and other UI assets that need consistent color;
JPEG+A8 preserves alpha, not exact RGB values.

Omitted `quality` uses the Profile's `jpeg_quality`, for embedded and filesystem
images alike. An explicit value also applies to automatic JPEG selection.
`hardware_jpeg` falls back to lossless encoding when the Profile lacks hardware JPEG.

For codec selection, compile with `lossless` and `jpeg` and compare the generated
`*.execution.json`: `resources[].encoded_bytes` reports encoded size,
`animation.frame_bytes` the full pixel frame, and `animation.patch_ratio` the
fraction of patch pixels. Small ratios favor keeping patches; near-full updates
are candidates for hardware JPEG measurement. These reports do not include actual
SD throughput or total playback time; validate on the target board.

The JPEG decoder chooses hardware or software according to platform, dimensions and layout. Small images may use software; non-MCU-aligned dimensions may need scratch storage. Measure frame reads, alpha processing and display submission as well as decode time. Update GSPC and the runtime together before enabling JPEG animations: older runtimes reject the new frame format. Default QOI animations retain their existing format.

See [external assets](external-assets.md) for preprocessed SD/NAND images and frame streaming.

For SD fonts, use the [font-file API](external-assets.md#fonts-on-sd) with an explicit size limit and lifetime.

## Transparent rasters, previews and resource budgets

RGB565+A8 and ARGB8888 rasters support `fit`, `rotation` and bounded dynamic rotation,
including template images. The image box clips transformed content and source alpha
participates in blending. Object `opacity` must still be 255. Transparent transforms
use software sampling. `codec: raw` needs no
decode cache, while compressed resources require sufficient decoded-image cache space.
Encoded Flash size is not the runtime pixel-memory budget.

For camera/decoder frames, author an opaque Image placeholder with a `bind`, then use
`esp_gsp_canvas_push()` or `esp_gsp_canvas_try_push()`. Frames must match its dimensions
and the target RGB565/RGB888 format. Use at least two producer buffers and reuse frames
only after the release callback. Use Image setters for occasional replacements; see
[external assets](external-assets.md) for filesystem resources.

Prefer `codec: auto` across chips; use `speed` or `size` for a specific resource goal.
Explicit `jpeg` is a strict requirement; incompatible
targets diagnose the alternative. Without JPEG, use QOI/RLE lossless encoding and
explicit `store_scale` where reduced detail is acceptable. Smaller stored images need
a scaling-capable container codec and enough decoded cache. Budget the decoded
surface alongside the Flash saving. Compare the image/font/bundle byte report with the application
partition budget; the compiler never silently reduces resolution. Font diagnostics
list all missing codepoints and identify authored text-use paths. Custom fonts without
a question mark receive a visible replacement box instead of aliasing the first icon.

`store_scale < 1` automatically selects fitting into the authored image box; it does
not require the additional `scalable` flag. With reduced storage, `auto` and
`speed` use QOI, while `size` compares scaled QOI with eligible JPEG. Explicit
`raw` is unsupported for this option and diagnoses the object path and
alternative encoding at compile time. Ordinary and template images share option
parsing. Compressed thumbnails still require a decoded-image cache.

Prefer original PNG and TTF/OTF sources when migrating. For LVGL C arrays or private
binary assets, establish the LVGL version, pixel format, channel order, stride,
alpha convention and compression before converting to supported input resources.
A 32-bit pixel width alone does not determine channel order; private `.bin` files
are not a single standard format.

## Removing JPEG from the firmware

JPEG decoding is enabled by default for compiled resources, runtime images and
external assets, including hardware-to-software fallback. If the application
never uses JPEG, omit its decoder code from the firmware with:

```ini
CONFIG_ESP_GSP_ENABLE_JPEG=n
```

For an existing project, clear **Link the JPEG decoder** under **ESP-GSP** in
`idf.py menuconfig`. New projects can place the setting in `sdkconfig.defaults`.
The option works with source and prebuilt components and takes effect at link
time. Keep the component's build files and archive from the same release.

### Resource compilation

`gsp_add_bundle()` and `gsp_add_assets()` pass `--disable-jpeg` to GSPC. Explicit
`PROFILE` settings remain supported: JPEG is excluded while other capabilities
and custom budgets are preserved. A profile allowing only JPEG produces a
configuration error; declare the required non-JPEG codecs or keep JPEG enabled.
Standalone builds should pass the same flag. CMake checks compiler support
during configuration and explains how to update an incompatible compiler.

- For still images, `codec: auto` selects a lossless encoding. An explicit
  `codec: "jpeg"` produces `GSPC-RS-CAPABILITY`; this also covers its derived
  `jpeg_a8` output for transparent images.
- For animations, use `animation_codec: "lossless"`. An explicit
  `animation_codec: "jpeg"` produces the same diagnostic; changing `codec`
  alone does not override it. `animation_codec: "hardware_jpeg"` selects
  lossless frames when JPEG is disabled.

PNG, native pixels, supported lossless rasters, compiled SVG and QOI animations
remain available. Container probing can still identify JPEG. Decoder selection
is explicit, so an application with no bundled JPEG can retain support for JPEG
images supplied later at runtime.

### Runtime images and external assets

An accepted runtime-image request containing JPEG completes with
`GSP_ERR_UNSUPPORTED` before a decode surface is reserved. Use the completion
callback to observe the result; buffer ownership and release rules are unchanged.
Externally compiled bundles and asset packages are checked when their JPEG
resources are used. Compiled JPEG animations fail when opened, and external JPEG
frame streams fail before allocating a frame canvas.

The software decoder dependency remains part of component resolution. JPEG code
can still be linked if another component uses it. Host and WASM/WASI simulators
retain their codec support; successful preview alone does not establish that a
device built with JPEG disabled can display the same external JPEG data.

## Software JPEG and scale animation

ESP-IDF builds can use the `esp_new_jpeg` software decoder on targets such as
ESP32-S3. `gsp_add_bundle`/`gsp_add_assets` detect that dependency independently
of the hardware JPEG engine and require a usable decoded-image cache for static
JPEG resources. Standalone platform compilation can declare `--software-jpeg`.
This enables explicit `codec: "jpeg"`; software availability alone does not make
`auto` choose lossy JPEG. A finite declared cache budget must fit the decoded
JPEG surface, including its alignment/alpha storage. JPEG saves Flash, not that
pixel memory. Animation frame buffers have a separate budget.

Use `animate_scale_q16` and `animate_scale_q16_to` to animate named scalable Images.
65536 is 1× and 131072 is 2×. Call these helpers in the image's scene and choose values
within its declared scale range.

## Replacing images

Add a resource `bind` to a scene image, for example `name: "cover", bind: "cover_image"`,
to generate its replacement API. For template images, set `dynamic_image: true`.
Size `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS` for the number of simultaneously active
image targets in application-created templates.

Build precompiled asset packages with `gsp_add_assets()` and display them with the bound
image's generated `set_asset()` helper. Use the generated asset references to select images;
they carry the package information required by the API.
