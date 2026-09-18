# 选项卡视图（`gsp_tabview`）

包含标签栏和可滑动内容页的组合组件。

## 适用场景

适合用一组原生控件快速提供完整、熟悉的交互。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.5.0 gspc pack \
  examples/widgets/tabview/tabview.json \
  --deployable -o gsp-out/widget-preview/tabview.gspb
python -m gsp.execute --version 1.4.0 sim \
  --bundle gsp-out/widget-preview/tabview.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

GSPC 将组合组件编译为原生基础图元，并为命名元素生成连接应用行为的适配 API。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

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

该文件来自 `examples/widgets/tabview/tabview.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_tabview_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_tabview___feature_tabs_btn0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview___feature_tabs_btn1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview___feature_tabs_btn2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview___feature_tabs_ind0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview___feature_tabs_ind1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview___feature_tabs_ind2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview_feature_tabs_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview_feature_tabs_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_tabview_feature_tabs_tab0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview_feature_tabs_tab0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_tabview_feature_tabs_tab1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview_feature_tabs_tab1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_tabview_feature_tabs_tab2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_tabview_feature_tabs_tab2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_tabview_docs_config(void)
size_t gsp_tabview_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | 是 | TabView 背景 |
| `tabs` | `string_list` | 是 | — | — | 选项卡标签 |
| `bar_height` | `int` | — | 默认 56; 1…65535 | — | 选项卡栏高度 |
| `active_color` | `color` | — | — | — | 激活指示器颜色 |
| `cyclic` | `bool` | — | 默认 `true` | — | 页面循环切换 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中的选项卡 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `fg_color` | `color` | — | — | — | 选项卡标签颜色 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏 |
| `axis` | `enum` | — | 默认 horizontal; `horizontal`, `vertical` | — | 页面轴向 |
| `stop_anywhere` | `bool` | — | 默认 `false` | — | 允许 PageFlow 在页面之间回弹定位 |
| `page_extent` | `int` | — | 默认 0; 0…65535 | — | PageFlow 拖动范围 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
