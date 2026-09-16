# Carousel (`gsp_carousel`)

Orbit icon picker with drag, depth ordering and snap.

## When to use

Use it to browse, select, or present repeated application data efficiently.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/carousel/carousel.json \
  --deployable -o gsp-out/widget-preview/carousel.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/carousel.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

Rows or cells are rendered from scene data and can connect to scrolling, selection, or application data adapters.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "carousel_demo",
  "w": 320,
  "h": 240,
  "screen_bg": "#0B1424",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 16,
      "y": 10,
      "w": 288,
      "h": 24,
      "text": "GSP  /  ORBIT",
      "font_size": 18,
      "fg_color": "#E8F4FF"
    },
    {
      "type": "carousel",
      "parent": -1,
      "name": "menu",
      "x": 16,
      "y": 42,
      "w": 288,
      "h": 168,
      "icons": [
        "home.svg",
        "music.svg",
        "water.svg",
        "sun.svg",
        "power.svg"
      ],
      "selected": 0
    },
    {
      "type": "label",
      "parent": -1,
      "x": 16,
      "y": 214,
      "w": 288,
      "h": 20,
      "text": "Drag to rotate  /  tap to select",
      "font_size": 14,
      "fg_color": "#9FB5CB",
      "text_align": "center"
    }
  ]
}
```

This is `examples/widgets/carousel/carousel.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_carousel_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_carousel_demo_menu_animate_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_carousel_demo_menu_animate_selected_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_carousel_demo_menu_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_carousel_demo_menu_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_carousel_demo_menu_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_carousel_demo_menu_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_carousel_demo_menu_play_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_carousel_demo_menu_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_carousel_demo_menu_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_carousel_demo_menu_stop_selected(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_carousel_docs_config(void)
size_t gsp_carousel_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | y relative to parent |
| `w` | `int` | yes | 0…65535 | scene: —; template: — | width in px |
| `h` | `int` | yes | 0…65535 | scene: —; template: — | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `icons` | `string_list` | yes | — | — | 3..12 image paths, compiled to native thumbnails |
| `selected` | `int` | — | default 0; 0…11 | — | initial selected icon index |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `hidden` | `bool` | — | default `false` | yes | start hidden (show via actions or set_visible) |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |
| `bind` | `identifier` | — | — | — | public state name; generates GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource` | — | explicit bind state family |
| `callback` | `identifier` | — | — | — | app callback name; generates scene-qualified event helpers |
| `events` | `action_list` | — | — | — | input bindings: [{event, action, ...}] |
| `runtime_style` | `bool` | — | default `false` | — | generate runtime appearance setters |
| `image_sets` | `string_matrix` | — | — | — | additional image groups for runtime theme selection |
| `image_set` | `int` | — | default 0; 0…3 | `runtime_style` | initial compiled image group index |
| `projection` | `enum` | — | default perspective; `flat`, `tilt`, `perspective` | — | carousel projection |
| `depth` | `int` | — | default 60; 0…100 | `runtime_style` | carousel perspective strength in percent |
| `spacing` | `int` | — | default 50; 0…100 | `runtime_style` | carousel orbit spacing in percent |
| `period_ms` | `int` | — | default 280; 100…60000 | `runtime_style` | carousel settling duration with runtime_style |
| `opacity` | `int` | — | default 255; 0…255 | `runtime_style` | overall icon opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
