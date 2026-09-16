# Flip Card (`gsp_flip_card`)

Two compiled image faces with an interruptible perspective flip.

## When to use

Use it to present or activate a primary piece of interface content.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.0 gspc pack \
  examples/widgets/flip_card/flip_card.json \
  --deployable -o gsp-out/widget-preview/flip_card.gspb
python -m gsp.execute --version 1.3.0 sim \
  --bundle gsp-out/widget-preview/flip_card.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

With `runtime_style: true`, generated setters control depth, opacity and duration at runtime; `flipped` selects the target face. Tap flips by default; `enabled: false` disables taps. The tap callback uses the `value` event and does not signal animation completion. Paths are relative to the scene and accept PNG, JPEG and the supported GSP SVG subset; each source is limited to 2 MiB and is resized to a texture of at most 256 pixels per side. Configure up to three additional skins in `image_sets` and switch with `set_image_set()`; group 0 is the original front/back pair. Images are compiled into the bundle. Arbitrary runtime file imports, template membership and dynamic geometry expressions are unsupported.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "flip_card_demo",
  "w": 320,
  "h": 240,
  "screen_bg": "#08131F",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 20,
      "y": 10,
      "w": 280,
      "h": 28,
      "text": "Flip card",
      "font_size": 20,
      "fg_color": "#E8F4FF",
      "text_align": "center"
    },
    {
      "type": "flip_card",
      "parent": -1,
      "name": "card",
      "x": 35,
      "y": 42,
      "w": 250,
      "h": 166,
      "front": "../../effects/assets/card_front.svg",
      "back": "../../effects/assets/card_back.svg",
      "depth": 60,
      "period_ms": 600,
      "runtime_style": true,
      "image_sets": [
        [
          "../../effects/assets/card_front_teal.svg",
          "../../effects/assets/card_back_teal.svg"
        ]
      ]
    },
    {
      "type": "label",
      "parent": -1,
      "x": 20,
      "y": 212,
      "w": 280,
      "h": 20,
      "text": "Tap to turn over",
      "font_size": 13,
      "fg_color": "#A6BDCC",
      "text_align": "center"
    }
  ]
}
```

This is `examples/widgets/flip_card/flip_card.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_flip_card_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_flip_card_demo_card_animate_depth(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_depth_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_opacity(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_opacity_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_period_ms(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_animate_period_ms_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_flip_card_demo_card_get_depth(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_flip_card_demo_card_get_flipped(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_flip_card_demo_card_get_image_set(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_flip_card_demo_card_get_opacity(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_period_ms(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_flip_card_demo_card_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_flip_card_demo_card_play_depth(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_flip_card_demo_card_play_opacity(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_flip_card_demo_card_play_period_ms(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_flip_card_demo_card_set_depth(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_flipped(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_flip_card_demo_card_set_image_set(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_opacity(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_period_ms(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_flip_card_demo_card_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_flip_card_demo_card_stop_depth(esp_gsp_handle_t gsp)
esp_err_t gsp_flip_card_demo_card_stop_opacity(esp_gsp_handle_t gsp)
esp_err_t gsp_flip_card_demo_card_stop_period_ms(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_flip_card_docs_config(void)
size_t gsp_flip_card_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | y relative to parent |
| `w` | `int` | yes | 32…512 | scene: —; template: — | width in px |
| `h` | `int` | yes | 32…512 | scene: —; template: — | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `runtime_style` | `bool` | — | default `false` | — | generate runtime appearance setters |
| `image_sets` | `string_matrix` | — | — | — | additional image groups for runtime theme selection |
| `front` | `path` | yes | — | — | front image path |
| `back` | `path` | yes | — | — | back image path |
| `depth` | `int` | — | default 60; 0…100 | `runtime_style` | perspective strength in percent |
| `period_ms` | `int` | — | default 560; 100…60000 | `runtime_style` | flip duration in milliseconds |

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
| `image_set` | `int` | — | default 0; 0…3 | `runtime_style` | initial compiled image group index |
| `flipped` | `bool` | — | default `false` | yes | show the back face |
| `opacity` | `int` | — | default 255; 0…255 | `runtime_style` | overall opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
