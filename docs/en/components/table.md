# Table (`gsp_table`)

Compile-time table assembled from labels, cells and grid lines.

## When to use

Use it when a familiar interaction is best delivered as a ready-made group of native controls.

## Local interactive preview

After [installing `esp-gsp-tools`](../guide/simulator-preview.md), run from an
unpacked component or public repository root:

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/table/table.json \
  --deployable -o gsp-out/widget-preview/table.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/table.gspb
```

These commands compile the JSON below and open it in the ESP-GSP simulator's
browser preview.

## Runtime behavior

GSPC compiles the group into native primitives and generates adapters for application behavior exposed by named elements.

Give every object that application code must read or update a stable `name`. GSPC generates the typed functions listed below for named objects.

## Complete example JSON

```json
{
  "screen": "widget_table",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "table",
      "parent": -1,
      "x": 25,
      "y": 30,
      "w": 430,
      "columns": [
        "Pipeline",
        "State",
        "Budget"
      ],
      "rows": [
        [
          "Compose",
          "Active",
          "2.1 ms"
        ],
        [
          "Present",
          "Synced",
          "5.8 ms"
        ],
        [
          "Media",
          "Ready",
          "12 slots"
        ],
        [
          "Input",
          "Active",
          "1.3 ms"
        ]
      ],
      "col_widths": [
        2,
        1,
        1
      ],
      "font_size": 14,
      "header_color": "#193246",
      "grid_color": "#36516C",
      "row_height": 48,
      "fg_color": "#E2E8F0",
      "bg_color": "#142033"
    }
  ]
}
```

This is `examples/widgets/table/table.json`. Copy any relative assets referenced by the scene with it.

## Generated C API for this example

```c
const gsp_component_directory_t *const * gsp_table_docs_component_directories(uint16_t *out_count)
esp_gsp_config_t gsp_table_docs_config(void)
size_t gsp_table_docs_dynamic_image_slots(void)
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
| `bg_color` | `color` | — | — | yes | table background |
| `fg_color` | `color` | — | — | — | table text color |
| `font_size` | `int` | — | 1…255 | — | per-object font pixel size |
| `columns` | `string_list` | yes | — | — | column labels |
| `rows` | `string_matrix` | yes | — | — | cell rows |
| `col_widths` | `number_list` | — | — | — | relative column widths |
| `row_height` | `int` | — | default 44; 1…65535 | — | row height |
| `header_color` | `color` | — | — | — | header fill color |
| `grid_color` | `color` | — | — | — | grid line color |

<details><summary>Show other fields supported by this Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `radius` | `int` | — | default 0; 0…65535 | scene: —; template: — | corner radius |
| `font` | `path` | — | — | — | per-object TTF/OTF override |

</details>

<details><summary>Show fields shared by every Widget</summary>

| Field | Type | Required | Default / range | Dynamic | Compiler definition |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | parent by name instead of index |
| `h` | `int` | yes | 0…65535 | scene: —; template: — | height in px |
| `name` | `identifier` | — | — | — | stable component name; generates GSP_OBJ_KEY_&lt;NAME&gt; |

</details>
