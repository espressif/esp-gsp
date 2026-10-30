# LINEA material artwork

`cotton-fold.png` is a transparent textile image prepared from artwork
generated with the built-in image generation tool. The component restores
it from the verified example asset archive.


Only the 164×164 prepared PNG is referenced by the scene. Its RGB channels
are dithered for RGB565; source alpha is preserved. The same image is reused
on the selector and inside both drum views, with no animation frame sequence.
The source original is not included in the compiled GSPB resource graph.

`drum-shell.svg` is a static retained vector rim/glass overlay. The controller
rotates only the textile load during an active cycle; shell and highlight stay
fixed. Motion stops when paused, covered or off screen.
