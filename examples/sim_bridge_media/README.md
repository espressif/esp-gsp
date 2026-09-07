# Native media Backend example

This PC example exercises portable public C APIs for dynamic List/Grid data,
QOI images and a Canvas draw callback. It uses the native
[sim_bridge](../../tools/sim_bridge/README.md) included in the ESP-GSP component.

From an application's root with ESP-GSP installed under `managed_components`,
Python 3.10+, CMake 3.20+ and a native C11 compiler:

```sh
python -m pip install -U esp-gsp-tools
python managed_components/espressif__esp-gsp/tools/sim_bridge/run.py \
  --project managed_components/espressif__esp-gsp/examples/sim_bridge_media/pc
```

Add `--headless --duration 3` for a bounded smoke run. The host must advertise
`bridge_version: 1`, `bridge_media_version: 1`, `bridge_image_version: 1` and
`bridge_fence_version: 1`.
The runner automatically selects GSPC from `.gspc_version` and the simulator
from the component version. `GSPC_EXECUTABLE` / `GSP_SIM_EXECUTABLE` can select
downloaded binaries instead. No implementation sources or active ESP-IDF
environment are needed for the native build. On Windows, use a compiler developer
terminal and Windows executable paths.

`main/media_ui.c` contains only public GSP calls and ordinary C business
state. `pc/platform_pc.c` provides the PC lifecycle and prints row/draw counts
on exit. After initialization it calls `poll(0)` to draw/upload local Canvas
work, then `esp_gsp_flush(ui, 1000)` to wait for a host render attempt. This
one-time boundary does not wait for all image decoding, animations or browser
presentation. It is not added to the recurring timer. There is no board
target in this example; a device project can reuse
the business file after supplying hardware startup and headers generated
alongside that target's bundle/profile (not copied from a different PC build).
Device lifecycle code must also honor render-task callback lifetime when
stopping. `hello_world` demonstrates the complete device/PC split.

The scene contains a dynamic List, a two-column Grid with text and QOI images,
a Canvas gradient updated by a timer, and a separate QOI image bind. The List
has enough authored items to generate a runtime row template; a short static
list that fits the viewport may not have one. The Grid template marks its
image `dynamic_image: true` and binds its text. These details are necessary
for runtime row updates, not just for the bridge.

See [scenes/media.json](scenes/media.json) for the exact syntax. Canvas uses
an ordinary opaque image with `codec: "lossless"` and `bind: "surface"`;
the generated `GSP_BIND_SURFACE` identifies it in C. The build's RGB565
profile selects the native pixel format. The callback receives resource
dimensions, not necessarily the image widget's scaled layout dimensions.

Binders run serially in the native event loop and can synchronously publish
row fields. The Canvas callback receives a complete offscreen surface and
the bridge uploads a full frame after it returns. This intentionally differs
from device render-task scheduling and tile partitioning; it is not a
performance benchmark. Grid images use TAKE (`*_image_owned`), with explicit
free on submission failure. The standalone QOI uses BORROW, a stable cache
key and completion/release callbacks. Transport still copies the bytes;
native sources remain retained until GSP releases its input or the local
session closes. Upload success is not publication or display presentation.
On orderly exit the default main settles image callbacks before PC deinit prints
their counters. See the bridge README for token lifetime, scene changes,
disconnect status and release timing; device code must still synchronize
its own render/decode-task callbacks before freeing shared state.
Forced termination (including immediate Windows process termination) cannot
guarantee callback delivery.

For a raw-frame producer, replace draw registration with
`esp_gsp_canvas_try_push*`; keep accepted buffers immutable until release.
The native bridge admits at most eight frames locally and returns TIMEOUT
without ownership transfer when full. Poll uploads them; a later synchronous
GSP RPC also drains earlier frames. Releases can therefore happen before such
an RPC, not just during poll, and must not mutate GSP state. See
[queue and fence semantics](../../tools/sim_bridge/README.md#nonblocking-canvas-producers-and-synchronization)
for a C usage snippet, asynchronous rejection and timeout behavior. This
example continues to use the offscreen draw producer, not both producers on
the same Canvas.
