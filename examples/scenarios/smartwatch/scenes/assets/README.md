# Field watch artwork

`dial.svg`, `seconds.svg`, `trail.svg` and `maya.svg` are original artwork authored for this example, under the example's SPDX license. The dial, terrain contours, seconds marker, mountain trail and portrait are retained compiled SVG resources. Dynamic text is rendered as font glyphs.

Missing media is restored from the pinned example asset archive at build time;
see [asset retrieval](../../../../README.md#example-asset-retrieval). The firmware
uses compiled local resources and does not download artwork at runtime.

`ridge.png` was generated using the built-in image-generation tool on 2026-09-20:
an alpine dawn ridge with a hiking trail, mist, pines and warm sunrise.
Original source is retained in the asset archive. The current route selector
and detail views use the shared vector maps documented in
[`../artwork/README.md`](../artwork/README.md); this photograph is no longer
referenced by the scene and does not consume firmware Flash or decoded-image
memory.
