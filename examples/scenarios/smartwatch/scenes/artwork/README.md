# Field route artwork

`ridge-route.svg`, `coast-route.svg` and `forest-route.svg` are original
240×110 vector illustrations authored for this example under its SPDX
license. They are illustrative walking routes, not georeferenced maps.
The palette follows the dial's dark terrain contours and bright route line.

The route selector and detail Drawer reference the same three resources at
the same size. The ridge map is also used in the local message invitation.
No video or frame sequence is stored, and the application allocates no image
buffers. The renderer's existing cache can retain rasterized maps. Maps
remain still until the user chooses another route.
