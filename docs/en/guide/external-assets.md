# External SD / NAND assets

Keep firmware assets on `gsp_add_bundle()` and export large raster assets to a
mounted filesystem. Both reuse preprocessing/codecs; devices do not parse GIF/APNG
or composite their original frames. Packages load by image/frame, not as a whole
RAM copy. Startup/fallback pictures can remain embedded.

## Minimal integration

`assets/assets.json` uses paths relative to the manifest:

```json
{"assets":[{"name":"cover","src":"cover.png","codec":"auto",
  "target":{"scene":"../scenes/gallery.json","image":"cover"}}]}
```

Names are stable C identifiers (`open` is reserved). The optional target selects
a unique named Image or matching bind, infers known static dimensions and checks
pixel/alpha compatibility. Image-fit keeps source dimensions; advanced/dynamic
layout still needs runtime validation. Without a hint, standalone libraries and
dynamic targets remain supported: specify width/height or retain source dimensions.

```cmake
gsp_add_bundle(${COMPONENT_LIB} PIXEL_FORMAT rgb565 IMAGE_CACHE_BYTES 262144)
gsp_add_assets(${COMPONENT_LIB} MANIFEST "../assets/assets.json" SYMBOL media)
```

Assets inherit the same component's previously declared Bundle profile, including
pixel format, target capabilities and image budget. Without a preceding bundle,
target defaults (RGB565) apply. Explicit PIXEL_FORMAT/PROFILE overrides remain;
components with different bundle profiles must choose explicitly.

Copy `build/esp-idf/main/gsp_assets_media/media.gspb` to storage; use `media.h` in
firmware. Builds track source assets, target scenes/placeholders and profiles.
After mounting, open from an application task:

```c
#include "media.h"
esp_gsp_assets_t *assets;
ESP_ERROR_CHECK(gsp_media_open("/sdcard/media.gspb", NULL, &assets));
ESP_ERROR_CHECK(gsp_gallery_cover_set_asset(ui, assets, gsp_media_cover, false));
```

The generated setter corresponds to replaceable Image `cover` in scene `gallery`;
use the actual generated names. False follows authored loop counts; true plays one
cycle. Opening performs IO/validation: do not call it from rendering/decoding
callbacks. Standalone export:

```sh
gspc assets assets/assets.json --platform-soc esp32s31 --psram --hardware-jpeg --symbol media -o output/media.gspb
```

## Codecs and targets

Raster input uses compiler PNG/APNG, JPEG, GIF, BMP and WebP support. Optional
quality is 1–100 (profile default); max_fps limits imported animation rate.

| Preference | Behavior |
|---|---|
| auto (default) | Target policy for still images; lossless QOI animation patches |
| lossless | Lossless QOI retaining alpha |
| speed / hardware_jpeg | JPEG/JPEG+A8 with target hardware capability, otherwise QOI |
| jpeg | Explicit JPEG/JPEG+A8, including software decoding; lossy color, lossless alpha |
| raw | Native pixels for still images; lossless animation unless animation_codec overrides it |

Pixel format, alpha representation and native dimensions must match the target;
opaque image-fit allows different source dimensions. Use transparent placeholders
for alpha assets. Mismatches retain the previous picture rather than removing
alpha. Static JPEG/JPEG+A8 requires image caching. Animations have their own frame
storage. STORE and supported QOI/RLE still-image codecs can work without
whole-image caching.

## Row / Widget and names

Template Resource slots generate Widget and Row *_asset setters. Generic forms:

```c
ESP_ERROR_CHECK(esp_gsp_assets_show_row(assets, ui, row, image_slot, gsp_media_cover, false));
ESP_ERROR_CHECK(esp_gsp_assets_show_widget(assets, ui, widget, image_slot, gsp_media_cover, false));
ESP_ERROR_CHECK(esp_gsp_assets_show_target_name(assets, ui,
    esp_gsp_asset_row_target(row, image_slot), record_asset_name, false));
```

Use the binder's row token and generated slot constants. UI application validates
row/instance generations; stale status/stop tokens cannot affect a new occupant.
Image binds also support show_name. Scene changes cancel subsequent frames; resubmit on re-entry.
Use one service/image producer per target.
Hiding a widget or parent group does not pause file IO or decoding. Call
`esp_gsp_assets_stop()` or `esp_gsp_assets_stop_target()` when hiding it, then
show it again when visible; playback restarts from the beginning.

## Encoding independent of storage

The same input, geometry, target Profile and codec settings use the same encoding
policy for embedded and external assets, preserving transparency, frame timing
and loop information.

The manifest accepts the Image field `animation_codec`: `lossless`, `jpeg` or
`hardware_jpeg`, overriding the animation policy implied by `codec`. The default
is lossless patches; hardware_jpeg follows target capability. `speed` remains a
legacy convenience alias. Static pictures still use codec; codec raw on an
animation follows the same lossless path as scene imports. The frame must fit the
Profile animation-frame budget.

Invalid animation frames preserve the displayed image. A replacement starts
from its first frame. JPEG animations also work with image caching disabled;
measure playback performance on the target device.

Animation memory follows `ANIM_FRAME_MEMORY` and `ANIM_MAX_FRAME_BYTES`,
separately from the encoded read budget. Allow for complete decoded frames and
peak playback memory. Storage latency can affect frame rate. Update GSPC and
ESP-GSP together before using newly exported animations; older packages remain
readable and can be re-exported to benefit from encoding improvements.

## State and memory

get_status returns a request ID, error details and pending state:

| State | Meaning |
|---|---|
| QUEUED | Waiting, including queue-pressure retries |
| READING | Lookup, IO or validation underway |
| DECODING | Submitted; waiting for decode/publication |
| READY | Current picture ready; animation may continue |
| FINISHED | Requested animation cycles finished; last frame remains |
| FAILED | Inspect last_error, failed_stage, system_errno and required_bytes |
| STOPPING / STOPPED | Stopping / no further frames scheduled |

Pending covers all unfinished work. Fair rotation prevents overdue animations
starving other targets. Queue retries reuse the payload; pressure can reclaim
unsubmitted buffers, preserving displayed sources. failed_stage distinguishes file loading from UI decoding/publication; required_bytes
is the encoded allocation requirement, not the decoded budget. Other failures stop that
request; resubmit after recovery. Stop retains the picture; show restarts it.
The earlier simple status API remains available.

Each package owns one reader with a default 4 KiB internal stack. Encoded budget
defaults to one quarter of free PSRAM (or internal RAM without PSRAM), capped at
4 MiB. It is a ceiling, not preallocation or a total GSP quota. Allow old/new
encoded sources and decoded pixels to coexist. Override after measuring:

```c
esp_gsp_assets_config_t config = { .read_budget_bytes = 512 * 1024 };
ESP_ERROR_CHECK(gsp_media_open("/sdcard/media.gspb", &config, &assets));
```

get_stats(assets, ui, &stats) reports encoded usage/peak/budget, index bytes, UI
decoded memory usage/peak, delivered file bytes, load time and retries. Decoded
memory includes this service's animation canvases/patch scratch plus the supplied
UI's scene caches; added peaks are a conservative bound, not simultaneous samples. Encoded
usage includes in-progress reads. Load time includes lookup, CRC and allocation;
decoded stats also include other UI images. Samples are not simultaneous and omit
display, DMA, task and other temporary memory.

Small indexes use best-effort PSRAM caching up to 64 KiB; no-PSRAM/large indexes
use bounded file reads. Payload allocation can reclaim the optional index cache.
Playback throughput depends on storage read latency, decoding and display
submission time. Measure these stages with the target storage and workload.

## Closing, updates and storage

Check close results. Default close waits at most five seconds, or choose a budget:

```c
esp_gsp_err_t ret = esp_gsp_assets_close_wait(assets, 1000);
if (ret == ESP_GSP_OK) {
    assets = NULL;
    esp_gsp_stop(ui);
    // Now unmount/update storage.
} else if (ret == ESP_GSP_ERR_TIMEOUT) {
    // Keep assets, UI and mount alive; retry after progress resumes.
}
```

Timeout does not forcibly cancel driver IO or free in-flight data. Closing handles
reject new show requests but permit queries/retry. Keep UI commands/decodes
progressing until successful close; other callers must quiesce before the handle
is freed. Published sources survive close until replacement/UI destruction.
Change the encoded budget by closing and reopening with a new configuration.

Compatible same-name assets can update independently; new names work through name
APIs, without relying on old member positions/counts. Keep files immutable while
open; close, replace, then reopen. Matching builds verify the index quickly;
replacement packages receive a one-time chunked full CRC scan. Members are checked
when read. Sequential scans avoid seeking each small block and periodically yield;
initial validation still costs IO time. CRC is not authentication.

Export replaces complete files atomically and commits the package last. Failed
writes do not truncate the previous package. This is not a multi-file transaction;
deploy only after successful builds.

This service uses mounted ESP-IDF filesystems. SD/NAND share the API; raw NAND ECC,
bad-block and wear management belong to the driver. fseek/ftell limits apply
(below 2 GiB with 32-bit long); larger libraries can be split. Use this file service through the ESP-IDF API; the PC bridge has no corresponding endpoint.

examples/external_assets includes SDSPI mounting, board pins, a BSP-managed
SDMMC/NAND alternative and built-in fallbacks. It never formats on mount failure.
Use GSPC 0.4.1 with ESP-GSP 1.3.1.

## Fonts on SD

`esp_gsp_font_file.h` loads fonts before UI startup. Dynamic TTF/OTF/TTC files and
catalogs generated by `gspc font-link` share `open → apply → close`; the signature
selects the kind. `max_bytes` must be nonzero; oversized files fail before allocation.
The whole file stays in RAM, preferring PSRAM on ESP-IDF. This is not demand-paged
glyph IO. The limit covers file bytes; alignment, handles, FreeType and glyph caches
need additional headroom.

For dynamic fonts in source builds, call `gsp_enable_freetype()` in CMake, then load
from the mounted filesystem:

```c
#include "esp_gsp_font_file.h"

esp_gsp_font_file_t *font = NULL;
esp_gsp_config_t app = gsp_bundle_config();
if (esp_gsp_font_file_open("/sdcard/ui.ttf", 8 * 1024 * 1024, &font) == ESP_GSP_OK) {
    ESP_ERROR_CHECK(esp_gsp_font_file_apply(font, &app));
}
// On load failure, app keeps its embedded fonts.
// Use app to start the UI; keep font alive until esp_gsp_stop(ui) succeeds.
```

On load failure, an application can keep its embedded fonts. Call
`esp_gsp_font_file_close(font)` after startup failure or after every borrowing UI
has stopped successfully; retain the handle on stop timeout. Do not reuse the old
configuration after closing its font. Applying prepares startup configuration;
it does not hot-replace a running font. Dynamic font signatures are checked on load;
FreeType parses them at UI startup; TTC uses its first face. Catalogs are CRC-checked on load.

To externalize fixed glyphs, author `"font_link":"external"` and link the bundle:

```sh
gspc pack scenes/app.json -o build/app.gspb
gspc font-link build/app.gspb --output-dir build/linked --catalog build/fonts.gspb
```

Use `build/linked/app.gspb` for the application and copy `fonts.gspb` to SD. Load and
apply the catalog through the same font-file API. The original unlinked bundle
still embeds its glyphs. Catalogs set `app.font_catalog`; dynamic fonts set
`app.ttf/ttf_size`. Two handles can be applied to the same configuration to combine
fixed glyphs and dynamic fallback. Keep both until UI shutdown.

Font open closes the file before returning. Storage may then be unmounted if no
image service or other user still needs it; the UI uses the RAM copy. This differs
from the per-frame file IO lifetime of external images/animations.
