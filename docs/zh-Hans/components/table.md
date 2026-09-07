# 表格（`gsp_table`）

由标签、单元格和网格线组成的编译期表格。

## 适用场景

适合用一组原生控件快速提供完整、熟悉的交互。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.3.0 gspc pack \
  examples/widgets/table/table.json \
  --deployable -o build/widget-preview/table.gspb
python -m gsp.execute --version 1.2.0 sim \
  --bundle build/widget-preview/table.gspb
```

这些命令用 GSPC 编译下方同一份 JSON，并通过发布版 ESP-GSP 模拟器打开浏览器预览，
不是 HTML 仿制控件。最终像素格式、字体、显示链路和性能仍需在目标硬件确认。

## 运行方式

GSPC 将组合组件编译为原生基础图元，并为命名元素生成连接应用行为的适配 API。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 会为示例中的命名对象生成下方列出的类型化函数；JSON 中没有命名的对象不会产生无用 API。

## 完整示例 JSON

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

该文件来自 `examples/widgets/table/table.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_table_docs_component_directories(uint16_t *out_count)
esp_gsp_config_t gsp_table_docs_config(void)
size_t gsp_table_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 是 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 是 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 是 | 宽度（像素） |
| `bg_color` | `color` | — | — | 是 | 表格背景 |
| `fg_color` | `color` | — | — | — | 表格文字颜色 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `columns` | `string_list` | 是 | — | — | 列标签 |
| `rows` | `string_matrix` | 是 | — | — | 单元格行数据 |
| `col_widths` | `number_list` | — | — | — | 各列相对宽度 |
| `row_height` | `int` | — | 默认 44; 1…65535 | — | 行高 |
| `header_color` | `color` | — | — | — | 表头填充颜色 |
| `grid_color` | `color` | — | — | — | 网格线颜色 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `radius` | `int` | — | 默认 0; 0…65535 | 是 | 圆角半径 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `h` | `int` | 是 | 0…65535 | 是 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |

</details>
