# 消息框（`gsp_msgbox`）

带标题、正文和操作按钮的编译期模态对话框。

## 适用场景

适合用一组原生控件快速提供完整、熟悉的交互。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.2.8 gspc pack \
  examples/widgets/msgbox/msgbox.json \
  --deployable -o build/widget-preview/msgbox.gspb
python -m gsp.execute --version 1.1.0 sim \
  --bundle build/widget-preview/msgbox.gspb
```

这些命令用 GSPC 编译下方同一份 JSON，并通过发布版 ESP-GSP 模拟器打开浏览器预览，
不是 HTML 仿制控件。最终像素格式、字体、显示链路和性能仍需在目标硬件确认。

## 运行方式

GSPC 将组合组件编译为原生基础图元，并为命名元素生成连接应用行为的适配 API。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 会为示例中的命名对象生成下方列出的类型化函数；JSON 中没有命名的对象不会产生无用 API。

## 完整示例 JSON

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

该文件来自 `examples/widgets/msgbox/msgbox.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

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

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `w` | `int` | 是 | 0…65535 | 是 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 是 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `title` | `string` | — | — | — | 对话框标题 |
| `text` | `string` | — | — | 是 | 对话框正文 |
| `buttons` | `string_list` | 是 | — | — | 按钮标签 |
| `callback` | `identifier` | — | — | — | 按钮按下回调 |
| `bg_color` | `color` | — | — | 是 | 对话框背景 |
| `fg_color` | `color` | — | — | — | 对话框文字颜色 |
| `button_color` | `color` | — | — | — | 按钮填充颜色 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏 |
| `dismissable` | `bool` | — | 默认 `false` | — | 点击遮罩时关闭 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `radius` | `int` | — | 默认 0; 0…65535 | 是 | 圆角半径 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |

</details>
