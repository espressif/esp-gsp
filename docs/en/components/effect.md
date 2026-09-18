# Effect (`gsp_effect`)

Pulse, rotating ring, liquid progress or shimmer with automatic playback.

## When to use

Use it to communicate status, measurement, motion, or visual hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.5.0 gspc pack \
  examples/widgets/effect/effect.json \
  --deployable -o gsp-out/widget-preview/effect.gspb
python -m gsp.execute --version 1.4.0 sim \
  --bundle gsp-out/widget-preview/effect.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

The simulator and device runtime render the same compiled geometry. Application-driven property changes require a simulator backend or device code.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "effects",
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
      "text": "GSP  /  MOTION",
      "font_size": 18,
      "fg_color": "#E8F4FF"
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "charge",
      "x": 28,
      "y": 54,
      "w": 112,
      "h": 112,
      "effect": "liquid",
      "value": 62,
      "fg_color": "#35DCBA",
      "bg_color": "#153D4A",
      "period_ms": 2400
    },
    {
      "type": "label",
      "parent": -1,
      "x": 48,
      "y": 97,
      "w": 72,
      "h": 32,
      "text": "62%",
      "font_size": 26,
      "fg_color": "#FFFFFF",
      "text_align": "center"
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "loading",
      "x": 188,
      "y": 56,
      "w": 76,
      "h": 76,
      "effect": "ring",
      "value": 35,
      "fg_color": "#6BAFFF",
      "period_ms": 1200
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "status",
      "x": 212,
      "y": 144,
      "w": 28,
      "h": 28,
      "effect": "pulse",
      "fg_color": "#C99AFF",
      "period_ms": 2000
    },
    {
      "type": "effect",
      "parent": -1,
      "name": "scan",
      "x": 28,
      "y": 196,
      "w": 264,
      "h": 14,
      "effect": "shimmer",
      "fg_color": "#9ACFFF",
      "bg_color": "#1C3350",
      "period_ms": 1800
    }
  ]
}
```

This is `examples/widgets/effect/effect.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_effect_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_effects_charge_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_charge_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_charge_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_charge_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_charge_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_charge_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_charge_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_charge_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_charge_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_charge_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_charge_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_charge_stop_phase(esp_gsp_handle_t gsp)
esp_err_t gsp_effects_loading_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_loading_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_loading_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_loading_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_loading_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_loading_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_loading_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_loading_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_loading_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_loading_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_loading_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_loading_stop_phase(esp_gsp_handle_t gsp)
esp_err_t gsp_effects_scan_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_scan_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_scan_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_scan_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_scan_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_scan_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_scan_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_scan_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_scan_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_scan_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_scan_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_scan_stop_phase(esp_gsp_handle_t gsp)
esp_err_t gsp_effects_status_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_status_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_effects_status_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_effects_status_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_effects_status_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_effects_status_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_effects_status_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_effects_status_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_effects_status_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_effects_status_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_effects_status_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_effects_status_stop_phase(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_effect_docs_config(void)
size_t gsp_effect_docs_dynamic_image_slots(void)
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
| `effect` | `enum` | yes | `pulse`, `ring`, `liquid`, `shimmer` | — | procedural visual |
| `fg_color` | `color` | — | default #38BDF8 | `runtime_style` | effect color |
| `bg_color` | `color` | — | — | — | optional static backing color |
| `value` | `int` | — | default 100; 0…100 | — | liquid level or visual intensity in percent |
| `period_ms` | `int` | — | default 2800; 100…60000 | `runtime_style` | one animation cycle in milliseconds |

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
| `shimmer_style` | `enum` | — | default linear; `linear`, `soft`, `diagonal` | — | highlight appearance |
| `playing` | `bool` | — | default `true` | — | advance automatically while visible |
| `phase` | `int` | — | default 0; 0…65535 | — | initial cycle progress, 0..65535 |
| `ring_style` | `enum` | — | default trail; `solid`, `glow`, `trail` | — | ring appearance |
| `softness` | `int` | — | default 75; 0…100 | `runtime_style` | glow feather softness in percent |
| `strength` | `int` | — | default 65; 0…100 | `runtime_style` | soft glow strength in percent |
| `opacity` | `int` | — | default 255; 0…255 | `runtime_style` | overall effect opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
