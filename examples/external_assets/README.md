# External preprocessed assets

This example keeps a fallback UI in firmware and streams compiled
animation frames from a mounted filesystem. It uses the same image codec/cache
pipeline as built-in resources, including hardware JPEG on capable targets.

1. Configure the LCD through the shared `hw_init` menu.
2. In **External asset example**, select an unused SPI bus and the SD card's
   MOSI/MISO/CLK/CS pins. Pins default to unconfigured because boards differ.
   Alternatively select **Use a filesystem mounted by the application/BSP**
   and mount SDMMC/NAND at the configured path before `mount_storage()` returns.
3. Run `idf.py build`. Copy
   `build/esp-idf/main/gsp_assets_media/media.gspb` to `/sdcard/media.gspb`
   (or the configured mount path). The matching generated header enables fast index validation.
4. Flash and monitor. Built-in pictures remain visible if the card or file is
   unavailable. The example never formats the card.

Resources resolve by stable manifest name. Replacing a package with a valid new
version does not require rebuilding firmware when names, target pixel format and
placeholder geometry/alpha contracts remain compatible. A matching generated
header checks only the index at open; an updated package takes a one-time full
CRC scan in bounded chunks. Payloads are still read/checked per requested frame.
For newly added names use `esp_gsp_assets_show_name()`; always close before
replacing a file, then reopen it. This is integrity checking, not authentication.

The source manifest is `assets/assets.json`. `speed` selects JPEG/JPEG+A8 when
hardware JPEG is advertised by the target, otherwise lossless QOI animation
patches. `lossless` explicitly retains lossless frames, and `jpeg` explicitly
selects JPEG even without a hardware decoder. Defaults preserve alpha.
Embedded and external animations share encoding policy: lossless preserves QOI
patches, while JPEG uses full frames. File members add bounded reads and CRCs.
Use animation_codec for the same explicit animation choice as in scene JSON.

The two Image placeholders match the external assets' dimensions and alpha form.
Existing GSP bindings require this compatibility. Do not send an alpha asset to
an opaque/image-fit placeholder. A mismatch is reported and preserves the old
image; it never silently removes alpha. Author transparent placeholders for
transparent content. The RGB565/RGB888 export profile must match the scene.

A single reader task handles both targets. It uses an internal stack because it
performs filesystem IO. Encoded bytes retained by displayed and pending images
share an automatic read budget (one quarter of free PSRAM, or free internal RAM,
up to 4 MiB); this is separate from decoded-image caching and display buffers.
Override `esp_gsp_assets_config_t.read_budget_bytes` when measurements justify it.
One pending frame per target bounds work; no full-package payload is loaded.

`esp_gsp_assets_status()` reports the last error and pending state. Before
stopping/pausing the UI, unmounting storage or replacing a package, close its
asset service from an application task while the UI still processes commands:

```c
if (esp_gsp_assets_close(assets) == ESP_GSP_OK) {
    assets = NULL;
    esp_gsp_stop(ui);
}
// Now unmount/update storage as appropriate for the BSP.
```

Closing does not invalidate the last published pixels or encoded source: their
buffers are released when the UI replaces them or shuts down. Do not close from
a render/decode callback or concurrently use a closing handle. Treat files as
immutable while open. This example demonstrates mounted filesystems; raw NAND
bad-block/ECC management remains the responsibility of its storage driver.
The file adapter uses the platform's `fseek`/`ftell` range (2 GiB on 32-bit long).

Use GSPC 0.4.1 with ESP-GSP 1.3.1. To validate source changes from this
example directory, select the source compiler and rebuild the runtime:

```sh
export GSPC_EXECUTABLE="$(realpath ../../ci/gspc-dev)"
idf.py -D ESP_GSP_BUILD_PREBUILT=ON build
```

Close waits at most five seconds. On timeout retain the handle, UI and mount,
then retry after IO/decoding resumes. Never unmount or stop the UI on timeout.
The manifest uses target hints to infer dimensions from the scene.

For fonts on the same SD card, use the [font-file workflow](../../docs/en/guide/external-assets.md#fonts-on-sd):
load with a size limit, apply to the UI configuration before startup, then close
after UI shutdown. Dynamic fonts and linked font catalogs use the same API.
