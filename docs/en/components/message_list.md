# Message List (`gsp_message_list`)

Virtualized conversation viewport with incoming and outgoing bubbles.

## When to use

Use it when a familiar interaction is best delivered as a ready-made group of native controls.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/message_list/message_list.json \
  --deployable -o gsp-out/widget-preview/message_list.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/message_list.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

This preview shows the initial state authored in JSON. To reproduce application-driven motion, update the corresponding properties through the simulator backend/API or device code.

## Runtime behavior

GSPC compiles the group into native primitives and generates adapters for application behavior exposed by named elements.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_message_list",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "font_charset": " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,?—'",
  "objects": [
    {
      "type": "message_list",
      "parent": -1,
      "name": "conversation",
      "x": 30,
      "y": 24,
      "w": 420,
      "h": 230,
      "background_color": "#142033",
      "incoming_color": "#263A4F",
      "outgoing_color": "#2563EB",
      "message_text_color": "#EEF4FA",
      "outgoing_text_color": "#FFFFFF",
      "bubble_radius": 14,
      "bubble_padding_x": 12,
      "bubble_padding_y": 8,
      "message_gap": 8,
      "side_margin": 12,
      "max_bubble_width": 310,
      "max_message_height": 120,
      "font_size": 14
    },
    {
      "type": "container",
      "parent": -1,
      "x": 30,
      "y": 270,
      "w": 350,
      "h": 36,
      "bg_color": "#213047",
      "radius": 18
    },
    {
      "type": "label",
      "parent": 1,
      "x": 15,
      "y": 8,
      "w": 320,
      "h": 22,
      "text": "Application-backed messages appear above",
      "font_size": 12,
      "fg_color": "#8294AC"
    },
    {
      "type": "button",
      "parent": -1,
      "callback": "send_click",
      "x": 394,
      "y": 270,
      "w": 56,
      "h": 36,
      "text": ">",
      "font_size": 16,
      "bg_color": "#2563EB",
      "fg_color": "#FFFFFF",
      "radius": 18
    }
  ]
}
```

This is `examples/widgets/message_list/message_list.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
bool gsp_widget_message_list_event_decode_call( const esp_gsp_event_t *event, gsp_widget_message_list_call_event_t *out_event)
bool gsp_widget_message_list_event_is_send_click( const esp_gsp_event_t *event)
const gsp_component_directory_t *const * gsp_message_list_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_message_list___conversation_message_row_set_bubble_color( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t color)
esp_err_t gsp_widget_message_list___conversation_message_row_set_bubble_h( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t value)
esp_err_t gsp_widget_message_list___conversation_message_row_set_bubble_w( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t value)
esp_err_t gsp_widget_message_list___conversation_message_row_set_bubble_x( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t value)
esp_err_t gsp_widget_message_list___conversation_message_row_set_message_text_fg_color( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t color)
esp_err_t gsp_widget_message_list___conversation_message_row_set_message_text_text( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, const char *utf8)
esp_err_t gsp_widget_message_list_conversation_animate_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_message_list_conversation_animate_selected_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_message_list_conversation_changed(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t prepended_count)
esp_err_t gsp_widget_message_list_conversation_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_message_list_conversation_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_message_list_conversation_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_widget_message_list_conversation_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_message_list_conversation_play_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_widget_message_list_conversation_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_widget_message_list_conversation_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_widget_message_list_conversation_stop_selected(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_message_list_docs_config(void)
esp_gsp_err_t gsp_widget_message_list___conversation_message_row_row_set_bubble_color( esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t color)
esp_gsp_err_t gsp_widget_message_list___conversation_message_row_row_set_bubble_h( esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t value)
esp_gsp_err_t gsp_widget_message_list___conversation_message_row_row_set_bubble_w( esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t value)
esp_gsp_err_t gsp_widget_message_list___conversation_message_row_row_set_bubble_x( esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t value)
esp_gsp_err_t gsp_widget_message_list___conversation_message_row_row_set_message_text_fg_color( esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t color)
esp_gsp_err_t gsp_widget_message_list___conversation_message_row_row_set_message_text_text( esp_gsp_handle_t gsp, esp_gsp_row_t row, const char *utf8)
esp_gsp_list_t gsp_widget_message_list_conversation_bind( esp_gsp_handle_t gsp, const esp_gsp_message_source_t *source)
esp_gsp_widget_t gsp_widget_message_list___conversation_message_row_create( esp_gsp_handle_t gsp, int16_t x, int16_t y)
size_t gsp_message_list_docs_dynamic_image_slots(void)
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
| `background_color` | `color` | — | — | — | message viewport background |
| `incoming_color` | `color` | — | default #E9EDF3 | — | incoming bubble color |
| `outgoing_color` | `color` | — | default #246BFD | — | outgoing bubble color |
| `message_text_color` | `color` | — | default #111827 | — | message text color |
| `outgoing_text_color` | `color` | — | default #FFFFFF | — | outgoing text color |
| `bubble_radius` | `int` | — | 0…1024 | — | message bubble radius |
| `bubble_padding_x` | `int` | — | 0…1024 | — | bubble horizontal padding |
| `bubble_padding_y` | `int` | — | 0…1024 | — | bubble vertical padding |
| `message_gap` | `int` | — | 0…1024 | — | message gap |
| `side_margin` | `int` | — | 0…4096 | — | message side margin |
| `max_bubble_width` | `int` | — | 0…32767 | — | maximum bubble width |
| `max_message_height` | `int` | — | 16…32767 | — | maximum message height |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `hidden` | `bool` | — | default `false` | yes | start hidden (show via actions or set_visible) |
| `row_template` | `identifier` | — | — | — | recycled row template |
| `callback` | `identifier` | — | — | — | app callback name |
| `font` | `path` | — | — | — | per-object TTF/OTF override |
| `font_charset` | `string` | — | — | — | glyphs available to runtime-bound message text |
| `scroll_snapshot` | `bool` | — | default `true` | — | cache two viewport frames while scrolling; falls back to live rendering when memory is unavailable |
| `item_height` | `int` | — | default 0; 0…65535 | — | row height |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
