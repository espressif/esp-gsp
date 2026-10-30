# Dropdown (`gsp_dropdown`)

Compact single-choice control that opens an option panel.

## When to use

Use it when a common interaction is easier to express as one ready-made control.

## Local interactive preview

First [set up the commands from the compatibility guide](../reference/compatibility.md#tool-commands), then run from an unpacked component or public repository root with `gspc` and `gsp_sim_host`:

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/dropdown/dropdown.json \
  --deployable -o gsp-out/widget-preview/dropdown.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/dropdown.gspb --frames 0
```

These commands compile the example and open it in the ESP-GSP simulator browser preview.

## Runtime behavior

GSPC lowers the group to native primitives. Give an object a stable `name` when application code must control it.

`selected` is the zero-based initial index (default 0). Generated `set_selected` updates the active scene's runtime selection and label, not the JSON. Restore saved selection after entering that scene. Read a clicked choice from `event->arg`; a getter inside the callback can still observe the value before the input transaction commits.

If application code must read or update an object, give it a stable `name`. GSPC then generates the typed functions listed below.

## Runtime option text

For language switching, opt in with `dynamic_options: true`. `option_text_max_bytes`
defaults to 128 UTF-8 bytes per label, excluding NUL (range 1–65535).
This authoring feature requires GSPC 0.10.0 and ESP-GSP 1.9.0 or later.

```json
{"type":"dropdown", "name":"sleep", "x":20, "y":20, "w":180, "h":32,
 "options":["10 sec", "Never"], "dynamic_options":true, "option_text_max_bytes":32}
```

Configure a font covering the translations and add their characters at the scene
root through `font_charset` (for example `"10 秒永不"`) or `font_charset_file`,
or configure a runtime font provider. Readback confirms text, not glyph coverage.
After entering this scene, use the generated API (shown for a scene named `settings`):

```c
const char *const zh[] = {"10 秒", "永不"};
ESP_ERROR_CHECK(gsp_settings_sleep_set_options_text(ui, zh, 2));
```

Replace the complete label array; its count and order stay fixed. Selection,
callback indices and application values stay unchanged. The SDK copies strings
before returning and prepares all row and closed-label glyph runs before publishing.
Invalid UTF-8, excessive length, allocation failure or a stale queued scene visit
preserves the previous labels. `ESP_OK` means accepted; queued apply/drop failures
are available through `esp_gsp_update_error_stats()` (`component` identifies the
Dropdown, `property=0` identifies the label group).

Opted-in controls also generate `get_option_count` and `get_option_text`. The latter
accepts `NULL, 0, &required` to query bytes including NUL; an insufficient output
buffer returns `ESP_ERR_INVALID_SIZE` without changing it. Readback observes
committed text and requires the UI task or caller serialization with rendering and
destruction. Overrides survive leaving and returning to a scene for the app's
lifetime. This mechanism adds no persistence policy.

Row geometry remains fixed; ordinary text clipping/ellipsis rules apply.
Each opted-in row adds a text binding to the compiler's slot requirements.
After replacement the runtime retains UTF-8 text plus row and closed-label glyph
runs per option; staging temporarily keeps both old and new sets. Static controls
allocate no option-text controller. Choose the byte budget for the actual translations.
Without the opt-in, generated configuration and selection behavior remain static,
and replacement returns `ESP_ERR_NOT_SUPPORTED`.

Rebuild resources with the matching compiler and SDK. Dynamic metadata requires
GMD 1.4; older runtimes reject its required sections. The PC C Backend uses the
same generated APIs, and simulator clients can read committed labels through
`dropdown_get_option_count` / `dropdown_get_option_text`.

## Example scene

```json
{"screen":"widget_dropdown","w":480,"h":320,"screen_bg":"#101827","font":"../../../common/fonts/DejaVuSans.ttf","font_charset":" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~","objects":[{"type":"dropdown","parent":-1,"name":"metric","x":70,"y":55,"w":340,"h":48,"options":["Frame throughput","Memory budget","Media decode","Input latency"],"selected":1,"open_direction":"down","font_size":16,"bg_color":"#172235","panel_color":"#1D2D44","fg_color":"#E2E8F0","border_color":"#334A68","border_width":1,"radius":10},{"type":"label","parent":-1,"x":70,"y":130,"w":340,"h":60,"text":"Tap the field, then choose an option from the panel.","font_size":16,"fg_color":"#94A3B8"}]}
```

The canonical example is [`dropdown.json`](../../../examples/usage/widgets/dropdown/dropdown.json). Copy any relative assets referenced by the scene with it.

## Generated C API

```c
const gsp_component_directory_t *const * gsp_dropdown_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_dropdown___metric_label_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown___metric_panel_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown_metric_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown_metric_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_dropdown_metric_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_widget_dropdown_metric_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_gsp_config_t gsp_dropdown_docs_config(void)
size_t gsp_dropdown_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields in this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | scene: —; template: — | y relative to parent |
| `w` | `int` | yes | 0…65535 | scene: —; template: — | width in px |
| `h` | `int` | yes | 0…65535 | scene: —; template: — | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | yes | dropdown background |
| `fg_color` | `color` | — | — | — | dropdown text color |
| `border_color` | `color` | — | — | — | border stroke color |
| `border_width` | `int` | — | 0…65535 | — | border stroke width |
| `radius` | `int` | — | default 0; 0…65535 | scene: —; template: — | corner radius |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `options` | `string_list` | yes | — | — | option labels |
| `open_direction` | `enum` | — | default down; `down`, `up` | — | direction in which the option panel opens |
| `selected` | `int` | — | default 0; 0…65535 | yes | initially selected index |
| `panel_color` | `color` | — | — | — | panel fill color |

<details>
<summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `grow` | `int` | — | default 0; 0…100 | — | auto-layout grow weight |
| `font` | `path` | — | — | — | per-object TTF/OTF override |
| `dynamic_options` | `bool` | — | default `false` | — | allow atomic runtime option-label replacement |
| `option_text_max_bytes` | `int` | — | default 128; 1…65535 | — | maximum UTF-8 bytes per dynamic option, excluding NUL |
| `items` | `string_list` | — | — | — | normalized dropdown option labels |
| `item_height` | `int` | — | 0…65535 | — | item row height |
| `callback` | `identifier` | — | — | — | selection callback |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |

</details>

<details>
<summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |

</details>
