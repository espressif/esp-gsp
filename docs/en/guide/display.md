# Display Presentation

ESP-GSP delegates framebuffer ownership, tearing control, rotation and panel
transfers to the `esp_display_present` dependency. Applications provide
one display target configuration and normally leave the presentation mode on
`ESP_DISPLAY_PRESENT_MODE_AUTO`.

## Panel classes

The location of frame memory determines the presentation strategy:

| Panel class | Frame memory | AUTO behavior |
|---|---|---|
| RGB / MIPI-DSI | Host framebuffer continuously scanned by the panel | Triple-buffered partial rendering when three buffers are available; double-buffered direct rendering otherwise |
| SPI / QSPI with TE | GRAM inside the panel | Dirty-area transfers synchronized to the TE signal |
| SPI / QSPI without TE | GRAM inside the panel | Free-running dirty-area transfers |

The application should describe the hardware accurately and let the presenter
select the strategy. Do not infer the panel interface from the SoC: one SoC can
be used with several display interfaces.

## Recommended configuration

Initialize the target structure supplied by the BSP or board layer:

```c
esp_display_present_target_config_t display;
ESP_ERROR_CHECK(board_display_init(&display));

esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd.display = display;
lcd.touch = touch; /* optional */
```

For RGB and MIPI-DSI targets, expose every framebuffer owned by the panel
driver. Three buffers allow rendering to continue without waiting for the
currently scanned frame; two buffers use the direct fallback.

For SPI and QSPI targets, provide TE configuration only when the signal is
wired and usable. A zeroed TE configuration means that TE synchronization is
disabled; GPIO 0 remains valid when TE is explicitly enabled.

## Memory-constrained GRAM panels

Partition rendering modes share one draw-buffer pool configured through
`target.drawbuf`. `target.drawbuf.lines` controls the height of each
presenter-owned draw stripe. Zero selects a stripe height derived from a
32 KiB default; `target.drawbuf.buffers` defaults to two for free-running
GRAM panels and to one for TE-synchronized and framebuffer-repair paths. Set
`lines` explicitly when the product needs a different memory bound, and use
`target.drawbuf.in_psram` only when the panel transfer path can consume that
memory. Values greater than the logical height and buffer counts greater
than two are rejected.

Host-scanned RGB and MIPI-DSI panels use framebuffers; `target.drawbuf`
applies only to their optional repair-path partition buffer.

## Rotation

The scene remains in logical coordinates. The presenter maps logical damage
and pixels to the physical panel according to the configured rotation.
Applications must compile a scene resolution matching the logical orientation;
90° and 270° therefore use transposed width and height for non-square panels.
The bundle `RGB565` or `RGB888` pixel format must also match the display
target; rotation does not convert between pixel formats.

## Transitions

Scene transitions normally use retained snapshots. When memory is constrained,
set `ESP_GSP_FIELD_ENABLE_TRANSITION_SNAPSHOTS` to zero with
`esp_gsp_config_set()` to request the lower-memory path. Supported slide and
fade-through-black transitions then render visible partitions directly when
the display contract permits it. A requested cross-fade cannot run without
both scene snapshots and uses the zero-snapshot fade-through-black fallback
instead.

## Rendering and SDK integration

Rendering optimizations apply automatically, including native RGB888 image
opacity. Evaluate frame production and display submission separately when
comparing performance; no additional application API is required.

For P4 prebuilt builds, configure the ESP-IDF silicon revision to match the
board. `CONFIG_ESP32P4_SELECTS_REV_LESS_V3` selects the pre-v3 library;
other P4 builds select the v3-or-later library. Reconfigure and rebuild after
changing the revision. The DMA2D adapter is compiled with the consuming ESP-IDF
SDK so its driver configuration uses that SDK's structure layout.

## Ownership boundary

The application and BSP own hardware description and product policy. ESP-GSP
owns logical scene damage, command replay, transitions, and frame planning.
The presenter owns:

- target classification and AUTO selection;
- framebuffer discovery and coherence;
- physical transforms and cache synchronization;
- TE, DMA/PPA transfers and completion fences;
- callback-safe stop and deletion.

Application and framework code should not register panel callbacks or repair
framebuffers independently of the presenter. Custom board integration should
provide one accurate display target instead of depending on internal renderer,
backend, or platform source layout.
