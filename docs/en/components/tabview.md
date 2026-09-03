# Tab View (`gsp_tabview`)

Compile-time tab bar and swipeable page content.

## When to use

Use it when a familiar interaction is best delivered as a ready-made group of native controls.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.2.8 gspc pack \
  examples/widgets/tabview/tabview.json \
  --deployable -o build/widget-preview/tabview.gspb
python -m gsp.execute --version 1.1.0 sim \
  --bundle build/widget-preview/tabview.gspb
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
  "screen": "widget_tabview",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "swipe": false,
  "objects": [
    {
      "type": "tabview",
      "parent": -1,
      "name": "feature_tabs",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "tabs": [
        "Overview",
        "Metrics",
        "Settings"
      ],
      "selected": 0,
      "bar_height": 52,
      "cyclic": true,
      "bg_color": "#101827",
      "active_color": "#38BDF8",
      "font_size": 15
    },
    {
      "type": "label",
      "parent_name": "feature_tabs_tab0",
      "x": 40,
      "y": 55,
      "w": 400,
      "h": 50,
      "text": "Overview content",
      "font_size": 26,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "chart",
      "parent_name": "feature_tabs_tab1",
      "x": 50,
      "y": 35,
      "w": 380,
      "h": 170,
      "points": [
        12,
        35,
        27,
        58,
        44,
        79,
        65,
        92
      ],
      "grid_lines": 3,
      "bg_color": "#142033",
      "fg_color": "#A78BFA"
    },
    {
      "type": "toggle",
      "parent_name": "feature_tabs_tab2",
      "x": 200,
      "y": 70,
      "w": 80,
      "h": 40,
      "checked": true,
      "bg_color": "#334155",
      "fg_color": "#22C55E"
    },
    {
      "type": "label",
      "parent_name": "feature_tabs_tab2",
      "x": 80,
      "y": 140,
      "w": 320,
      "h": 35,
      "text": "Swipe or tap a tab",
      "text_align": "center",
      "fg_color": "#94A3B8"
    }
  ]
}
```

This is `examples/widgets/tabview/tabview.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_tabview_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_tabview_feature_tabs_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_tabview_docs_config(void)
size_t gsp_tabview_docs_dynamic_image_slots(void)
```

These signatures come from the actual compiler output for this JSON.

## Fields used by this example

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `type` | `string` | yes | — | — | widget type |
| `parent` | `int` | yes | default -1; -1…65534 | — | parent object index (-1 = screen root) |
| `x` | `int` | yes | default 0; -32768…32767 | yes | x relative to parent |
| `y` | `int` | yes | default 0; -32768…32767 | yes | y relative to parent |
| `w` | `int` | yes | 0…65535 | yes | width in px |
| `h` | `int` | yes | 0…65535 | yes | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | yes | tabview background |
| `tabs` | `string_list` | yes | — | — | tab labels |
| `bar_height` | `int` | — | default 56; 1…65535 | — | tab bar height |
| `active_color` | `color` | — | — | — | active indicator color |
| `cyclic` | `bool` | — | default `true` | — | cyclic page wrap |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `selected` | `int` | — | default 0; 0…65535 | yes | initially selected tab |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `fg_color` | `color` | — | — | — | tab label color |
| `radius` | `int` | — | default 0; 0…65535 | yes | corner radius |
| `hidden` | `bool` | — | default `false` | yes | start hidden |
| `axis` | `enum` | — | default horizontal; `horizontal`, `vertical` | — | page axis |
| `stop_anywhere` | `bool` | — | default `false` | — | allow PageFlow to settle between pages |
| `page_extent` | `int` | — | default 0; 0…65535 | — | PageFlow drag extent |
| `enabled` | `bool` | — | — | — | initial interaction state; when present, exposes a runtime enabled property inherited by descendant controls |
| `disabled_color` | `color` | — | default #808080 | — | disabled-state overlay color |
| `disabled_opacity` | `int` | — | default 112; 0…255 | — | disabled-state overlay opacity |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |
| `font` | `path` | — | — | — | per-object TTF/OTF override |

</details>
