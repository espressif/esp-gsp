# 下拉选择（`gsp_dropdown`）

展开选项面板的紧凑型单选控件。

## 适用场景

适合用一组原生控件快速提供完整、熟悉的交互。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.2.8 gspc pack \
  examples/widgets/dropdown/dropdown.json \
  --deployable -o build/widget-preview/dropdown.gspb
python -m gsp.execute --version 1.1.0 sim \
  --bundle build/widget-preview/dropdown.gspb
```

这些命令用 GSPC 编译下方同一份 JSON，并通过发布版 ESP-GSP 模拟器打开浏览器预览，
不是 HTML 仿制控件。最终像素格式、字体、显示链路和性能仍需在目标硬件确认。

## 运行方式

GSPC 将组合组件编译为原生基础图元，并为命名元素生成连接应用行为的适配 API。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 会为示例中的命名对象生成下方列出的类型化函数；JSON 中没有命名的对象不会产生无用 API。

## 完整示例 JSON

```json
{
  "screen": "widget_dropdown",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "font_charset": " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
  "objects": [
    {
      "type": "dropdown",
      "parent": -1,
      "name": "metric",
      "x": 70,
      "y": 55,
      "w": 340,
      "h": 48,
      "options": [
        "Frame throughput",
        "Memory budget",
        "Media decode",
        "Input latency"
      ],
      "selected": 1,
      "open_direction": "down",
      "font_size": 16,
      "bg_color": "#172235",
      "panel_color": "#1D2D44",
      "fg_color": "#E2E8F0",
      "border_color": "#334A68",
      "border_width": 1,
      "radius": 10
    },
    {
      "type": "label",
      "parent": -1,
      "x": 70,
      "y": 130,
      "w": 340,
      "h": 60,
      "text": "Tap the field, then choose an option from the panel.",
      "font_size": 16,
      "fg_color": "#94A3B8"
    }
  ]
}
```

该文件来自 `examples/widgets/dropdown/dropdown.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_dropdown_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_dropdown_metric_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_dropdown_metric_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_widget_dropdown_metric_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_gsp_config_t gsp_dropdown_docs_config(void)
size_t gsp_dropdown_docs_dynamic_image_slots(void)
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
| `h` | `int` | 是 | 0…65535 | 是 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | 是 | 下拉控件背景 |
| `fg_color` | `color` | — | — | — | 下拉控件文字颜色 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度 |
| `radius` | `int` | — | 默认 0; 0…65535 | 是 | 圆角半径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `options` | `string_list` | 是 | — | — | 选项标签 |
| `open_direction` | `enum` | — | 默认 down; `down`, `up` | — | 选项面板展开方向 |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中索引 |
| `panel_color` | `color` | — | — | — | 面板填充颜色 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `grow` | `int` | — | 默认 0; 0…100 | — | 自动布局扩展权重 |
| `items` | `string_list` | — | — | — | 规范化后的下拉选项标签 |
| `item_height` | `int` | — | 0…65535 | — | 条目行高 |
| `callback` | `identifier` | — | — | — | 选中状态回调 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |

</details>
