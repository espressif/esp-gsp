# Benchmark media

Runtime assets are local. Missing media is restored from the pinned example
asset archive at build time; see [asset retrieval](../../../README.md#example-asset-retrieval).
The firmware does not download images. The two industrial images were generated
using the built-in image-generation tool, not the API/CLI fallback. They are
also referenced by the image/grid widget examples. No external stock-photo
download was used.

| File | Format | Purpose |
| --- | --- | --- |
| [bench_industrial.png](bench_industrial.png) | 640 × 480 RGB PNG | Detailed, asymmetric factory scene for opaque images, wallpaper and mixed composition |
| [bench_industrial_argb.png](bench_industrial_argb.png) | 320 × 240 RGBA PNG | Separated industrial objects with transparent space and partially transparent edges for blending |
| [bench_scale.png](bench_scale.png) | 96 × 64 RGB PNG | Deterministically resized from the industrial scene by `gen_scenes.py` for runtime scaling/rotation |
| [bench_anim.gif](bench_anim.gif) | 120 × 120, 24 frames | Procedurally generated radar sweep, 50 ms per frame, infinite loop |
| [bench_anim_sparse.gif](bench_anim_sparse.gif) | 120 × 120, 24 frames | Generated diagnostic: exactly two distant 8 × 8 regions change every 50 ms |
| [vector_planet.svg](../../../usage/widgets/image/vector_planet.svg) | 96 × 96 SVG | Original Orbit icon with layered planet shading, front/back rings and sparkles; shared by vector resize, rotation and silhouette-tint cases |
| [vector_emblem.svg](../../../usage/widgets/image/vector_emblem.svg), [vector_emblem_active.svg](../../../usage/widgets/image/vector_emblem_active.svg) | 128 × 128 SVG | Original shield/heart states with matching cubic paths and a named emblem group; color and A8 morph workload |
| [vector_paints.svg](../../../usage/widgets/image/vector_paints.svg) | 128 × 128 SVG | Original orbital ornament: even-odd ring, translucent color overlap, stroked curves and reused transformed rays |
| [vector_eye.svg](../../../usage/widgets/image/vector_eye.svg), [vector_eye_closed.svg](../../../usage/widgets/image/vector_eye_closed.svg) | 128 × 112 SVG | Original layered eyes: detailed iris/highlights, independently selected parts and matching open/closed eyelid paths |

Both GIFs are generated locally by `gen_scenes.py`, with no external artwork.
The Orbit SVG is authored as vector paths and solid paints, not traced from a
bitmap. Its geometry stays inside a rotation-safe circular margin. The older
`vector_icon.svg` remains a separate low-level regression fixture for holes
and alpha math; it is not the benchmark presentation artwork.
The emblem states are authored directly as compatible curves. The benchmark
selects only the `emblem` group; the separate `spark` element is used by the
public vector-motion example to demonstrate independent control of one source.
The eye artwork is vector-authored, not a GIF or a traced bitmap. The same iris
geometry translates in both eyes; matched mask/rim curves close over it on a
uniform face background. The eye case uses existing Image property animations.
The GIF diagnostics are simple: replacing them with decorative
animation would change their measurable patch workload. Scene geometry, grids,
labels, HUDs and controls remain native GSP objects, not baked screenshots.
`gen_scenes.py --check` verifies dimensions, alpha extrema, frame timing,
sparse-patch coverage and generated scene freshness. It does not assess artistic
quality or prove physical-panel correctness.

## Final generation prompts

### Industrial background

```text
Use case: stylized-concept
Asset type: checked-in industrial automation benchmark hero image for an embedded display renderer
Primary request: create a polished futuristic industrial automation scene suitable for repeated image scaling, blending, rotation, and full-screen rendering tests
Scene/backdrop: clean smart factory interior with robotic arms, conveyor machinery, machine-vision cameras, status lights, and layered depth
Subject: one central orange-and-graphite robotic arm with clearly asymmetric geometry and a visible camera module, surrounded by secondary machinery
Style/medium: premium realistic 3D product visualization, technically credible, crisp edges and varied fine detail
Composition/framing: landscape 4:3 composition, strong foreground/midground/background separation, no important subject cropped, obvious top/bottom/left/right orientation
Lighting/mood: cinematic but readable cool blue factory lighting with warm orange accents, balanced contrast
Color palette: graphite, steel blue, cyan, safety orange
Materials/textures: brushed metal, matte polymer, glass lenses, subtle floor reflections
Constraints: no text, no logos, no watermark, no people, no brand marks; visually dense enough to stress image decoding and scaling; avoid large flat empty areas; preserve clear directional cues when rotated or mirrored
```

### Transparent industrial overlay

```text
Use case: stylized-concept
Asset type: transparent RGBA overlay sprite for an embedded industrial UI renderer benchmark
Primary request: create a cohesive set of industrial automation visual elements arranged as one compact 4:3 atlas-like composition
Scene/backdrop: genuinely transparent background
Subject: four separated elements with generous transparent space between them: a robotic gripper, a machine-vision camera lens, a precision gear, and a glowing factory status beacon
Style/medium: premium realistic 3D iconography matching a graphite, steel-blue, cyan, and safety-orange smart-factory theme
Composition/framing: 4:3 landscape layout, each object fully visible and non-overlapping, strong asymmetric orientation cues, crisp silhouettes and fine edges
Lighting/mood: cool cyan rim light with warm orange highlights
Materials/textures: brushed metal, matte polymer, glass and subtle emissive light
Constraints: actual transparent alpha background, no panel or card backgrounds, no text, no logos, no watermark, no people, no brand marks; preserve partial alpha on glows and hard alpha edges on objects; avoid shadows that form a rectangular backdrop
```

The generated outputs were normalized to the dimensions listed above.
Treat changes to any asset, encoding policy or generated scene as workload
changes when comparing performance logs.
