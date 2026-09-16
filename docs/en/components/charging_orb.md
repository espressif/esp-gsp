# Charging Orb (`gsp_charging_orb`)

Charging progress with liquid, halo or plasma styles and optional particles.

## When to use

Use it to communicate status, measurement, motion, or visual hierarchy.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.0 gspc pack \
  examples/widgets/charging_orb/charging_orb.json \
  --deployable -o gsp-out/widget-preview/charging_orb.gspb
python -m gsp.execute --version 1.3.0 sim \
  --bundle gsp-out/widget-preview/charging_orb.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

The simulator and device runtime render the same compiled geometry. Application-driven property changes require a simulator backend or device code.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "charging_orb_demo",
  "w": 320,
  "h": 240,
  "screen_bg": "#08131F",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "charging_orb",
      "name": "charge",
      "parent": -1,
      "x": 70,
      "y": 20,
      "w": 180,
      "h": 180,
      "style": "halo",
      "value": 62,
      "fg_color": "#69CEF6",
      "strength": 85,
      "period_ms": 4200,
      "particles": 4,
      "softness": 90,
      "ripple": 55,
      "charging": true
    },
    {
      "type": "label",
      "parent": -1,
      "x": 90,
      "y": 92,
      "w": 140,
      "h": 40,
      "text": "62%",
      "font_size": 32,
      "fg_color": "#EFFFFF",
      "text_align": "center"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 20,
      "y": 210,
      "w": 280,
      "h": 22,
      "text": "ENERGY / CHARGING",
      "font_size": 14,
      "fg_color": "#7AADB5",
      "text_align": "center"
    }
  ]
}
```

This is `examples/widgets/charging_orb/charging_orb.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_charging_orb_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_charging_orb_demo_charge_animate_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_charging_orb_demo_charge_animate_phase_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_charging_orb_demo_charge_get_charging(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_charging_orb_demo_charge_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_charging_orb_demo_charge_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_charging_orb_demo_charge_get_phase(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_charging_orb_demo_charge_get_playing(esp_gsp_handle_t gsp, bool *out_value)
esp_err_t gsp_charging_orb_demo_charge_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_charging_orb_demo_charge_play_phase(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_charging_orb_demo_charge_set_charging(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_charging_orb_demo_charge_set_phase(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_charging_orb_demo_charge_set_playing(esp_gsp_handle_t gsp, bool new_value)
esp_err_t gsp_charging_orb_demo_charge_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_charging_orb_demo_charge_stop_phase(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_charging_orb_docs_config(void)
size_t gsp_charging_orb_docs_dynamic_image_slots(void)
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
| `fg_color` | `color` | — | default #38BDF8 | `runtime_style` | effect color |
| `value` | `int` | — | default 100; 0…100 | — | liquid level or visual intensity in percent |
| `period_ms` | `int` | — | default 2800; 100…60000 | `runtime_style` | one animation cycle in milliseconds |
| `softness` | `int` | — | default 75; 0…100 | `runtime_style` | glow feather softness in percent |
| `strength` | `int` | — | default 65; 0…100 | `runtime_style` | soft glow strength in percent |
| `ripple` | `int` | — | default 45; 0…100 | `runtime_style` | attached membrane ripple strength in percent |
| `style` | `enum` | — | default halo; `liquid`, `halo`, `plasma` | — | charging orb visual style |
| `charging` | `bool` | — | default `true` | — | enable charging motion and particles |
| `particles` | `int` | — | default 4; 0…6 | `runtime_style` | maximum soft energy particles |

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
| `bg_color` | `color` | — | — | — | optional static backing color |
| `playing` | `bool` | — | default `true` | — | advance automatically while visible |
| `phase` | `int` | — | default 0; 0…65535 | — | initial cycle progress, 0..65535 |
| `opacity` | `int` | — | default 255; 0…255 | `runtime_style` | overall effect opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
