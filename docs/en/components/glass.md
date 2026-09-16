# Frosted Glass (`gsp_glass`)

Build-time frosted crop of a named static image backdrop.

## When to use

Use it to communicate status, measurement, motion, or visual hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/glass/glass.json \
  --deployable -o gsp-out/widget-preview/glass.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/glass.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Blur, tint and rounded corners are baked at build time and drawn as native images at runtime. `backdrop` must reference an earlier static opaque Image with the same parent, containing the glass bounds. It does not blur other widgets or resample after runtime background replacement or independent glass movement. Preconfigure material variants and switch visibility for themes. Template membership and dynamic geometry expressions are unsupported.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "glass_demo",
  "w": 320,
  "h": 240,
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "image",
      "parent": -1,
      "name": "wall",
      "x": 0,
      "y": 0,
      "w": 320,
      "h": 240,
      "image": "../../effects/assets/backdrop.svg",
      "fit": "stretch"
    },
    {
      "type": "glass",
      "parent": -1,
      "name": "panel",
      "backdrop": "wall",
      "x": 40,
      "y": 45,
      "w": 240,
      "h": 150,
      "blur": 14,
      "tint_color": "#D8EAFF",
      "tint_opacity": 65,
      "radius": 18
    },
    {
      "type": "label",
      "parent": -1,
      "x": 65,
      "y": 70,
      "w": 190,
      "h": 24,
      "text": "LIVING ROOM",
      "font_size": 14,
      "fg_color": "#FFFFFF",
      "text_align": "center"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 65,
      "y": 103,
      "w": 190,
      "h": 45,
      "text": "22.6 C",
      "font_size": 34,
      "fg_color": "#FFFFFF",
      "text_align": "center"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 65,
      "y": 157,
      "w": 190,
      "h": 20,
      "text": "Comfortable",
      "font_size": 12,
      "fg_color": "#E9F5FF",
      "text_align": "center"
    }
  ]
}
```

This is `examples/widgets/glass/glass.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_glass_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_glass_demo_panel_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_glass_demo_panel_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_glass_demo_wall_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_glass_demo_wall_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_glass_docs_config(void)
size_t gsp_glass_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | y relative to parent |
| `w` | `int` | yes | 1…512 | scene: —; template: — | width in px |
| `h` | `int` | yes | 1…512 | scene: —; template: — | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `backdrop` | `string` | yes | — | — | name of a preceding static image with the same parent |
| `blur` | `int` | — | default 12; 0…32 | — | build-time blur radius in display pixels |
| `tint_color` | `color` | — | default #D8EAFF | — | frosted material tint |
| `tint_opacity` | `int` | — | default 70; 0…255 | — | tint strength |
| `radius` | `int` | — | default 16; 0…128 | scene: —; template: — | rounded corner radius in pixels |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `hidden` | `bool` | — | default `false` | yes | start hidden (show via actions or set_visible) |
| `opacity` | `int` | — | default 255; 0…255 | scene: —; template: — | overall opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
