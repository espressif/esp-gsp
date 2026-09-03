# Message Box (`gsp_msgbox`)

Compile-time modal dialog with title, message and action buttons.

## When to use

Use it when a familiar interaction is best delivered as a ready-made group of native controls.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.2.8 gspc pack \
  examples/widgets/msgbox/msgbox.json \
  --deployable -o build/widget-preview/msgbox.gspb
python -m gsp.execute --version 1.1.0 sim \
  --bundle build/widget-preview/msgbox.gspb
```

These commands compile the same JSON below with GSPC and open it in the
published ESP-GSP simulator's browser preview. It is not an HTML recreation.
Confirm the final pixel format, fonts, display path, and performance on target
hardware.

## Runtime behavior

GSPC compiles the group into native primitives and generates adapters for application behavior exposed by named elements.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects in this example; unnamed objects do not create unused API.

## Complete example JSON

```json
{
  "screen": "widget_msgbox",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "container",
      "parent": -1,
      "x": 34,
      "y": 30,
      "w": 412,
      "h": 260,
      "bg_color": "#172235",
      "border_color": "#2D425F",
      "border_width": 1,
      "radius": 16
    },
    {
      "type": "label",
      "parent": 0,
      "x": 30,
      "y": 34,
      "w": 352,
      "h": 38,
      "text": "Display preferences",
      "font_size": 24,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": 0,
      "x": 30,
      "y": 86,
      "w": 352,
      "h": 54,
      "text": "Changes are reviewed before they are applied to the device.",
      "font_size": 15,
      "fg_color": "#94A3B8"
    },
    {
      "type": "button",
      "parent": 0,
      "x": 106,
      "y": 174,
      "w": 200,
      "h": 48,
      "text": "Review changes",
      "font_size": 16,
      "bg_color": "#2563EB",
      "fg_color": "#FFFFFF",
      "radius": 12,
      "events": [
        {
          "event": "click",
          "action": "show",
          "target_name": "confirm_dialog"
        }
      ]
    },
    {
      "type": "msgbox",
      "name": "confirm_dialog",
      "title": "Apply changes?",
      "text": "The new display and interaction settings will become active immediately.",
      "buttons": [
        "Cancel",
        "Apply"
      ],
      "callback": "confirm_choice",
      "w": 390,
      "h": 235,
      "font_size": 15,
      "dismissable": true,
      "hidden": true,
      "bg_color": "#172235",
      "fg_color": "#E2E8F0",
      "button_color": "#2563EB"
    }
  ]
}
```

This is `examples/widgets/msgbox/msgbox.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
bool gsp_widget_msgbox_event_decode_call( const esp_gsp_event_t *event, gsp_widget_msgbox_call_event_t *out_event)
bool gsp_widget_msgbox_event_is_confirm_choice( const esp_gsp_event_t *event)
const gsp_component_directory_t *const * gsp_msgbox_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_msgbox_confirm_dialog_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_msgbox_confirm_dialog_get_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_msgbox_confirm_dialog_set_visible(esp_gsp_handle_t gsp, bool visible)
esp_gsp_config_t gsp_msgbox_docs_config(void)
size_t gsp_msgbox_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `w` | `int` | yes | 0…65535 | yes | width in px |
| `h` | `int` | yes | 0…65535 | yes | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `title` | `string` | — | — | — | dialog title |
| `text` | `string` | — | — | yes | dialog body |
| `buttons` | `string_list` | yes | — | — | button labels |
| `callback` | `identifier` | — | — | — | button press callback |
| `bg_color` | `color` | — | — | yes | dialog background |
| `fg_color` | `color` | — | — | — | dialog text color |
| `button_color` | `color` | — | — | — | button fill color |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `hidden` | `bool` | — | default `false` | yes | start hidden |
| `dismissable` | `bool` | — | default `false` | — | scrim tap closes |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `radius` | `int` | — | default 0; 0…65535 | yes | corner radius |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `parent_name` | `string` | — | — | — | parent by name instead of index |
| `font` | `path` | — | — | — | per-object TTF/OTF override |

</details>
