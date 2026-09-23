# 消息列表（`gsp_message_list`）

带收发气泡样式的虚拟化会话视口。

## 适用场景

适合用一组原生控件快速提供完整、熟悉的交互。

## 本地交互预览

先按[兼容性页的工具命令](../reference/compatibility.md#工具命令)配置 `gspc` 和
`gsp_sim_host`，再在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/message_list/message_list.json \
  --deployable -o gsp-out/widget-preview/message_list.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/message_list.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

此预览显示 JSON 中编写的初始状态。若要复现由应用驱动的动态变化，请通过模拟器后端/API 或设备代码更新对应属性。

## 运行方式

GSPC 将组合组件编译为原生基础图元，并为命名元素生成连接应用行为的适配 API。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "widget_message_list",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../../common/fonts/DejaVuSans.ttf",
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

该文件来自 `examples/usage/widgets/message_list/message_list.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

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
| `background_color` | `color` | — | — | — | 消息视口背景 |
| `incoming_color` | `color` | — | 默认 #E9EDF3 | — | 接收消息气泡颜色 |
| `outgoing_color` | `color` | — | 默认 #246BFD | — | 发送消息气泡颜色 |
| `message_text_color` | `color` | — | 默认 #111827 | — | 消息文字颜色 |
| `outgoing_text_color` | `color` | — | 默认 #FFFFFF | — | 发送消息文字颜色 |
| `bubble_radius` | `int` | — | 默认 14; 0…1024 | — | 消息气泡圆角半径 |
| `bubble_padding_x` | `int` | — | 默认 14; 0…1024 | — | 气泡水平内边距 |
| `bubble_padding_y` | `int` | — | 默认 10; 0…1024 | — | 气泡垂直内边距 |
| `message_gap` | `int` | — | 默认 8; 0…1024 | — | 消息间距 |
| `side_margin` | `int` | — | 默认 12; 0…4096 | — | 消息两侧外边距 |
| `max_bubble_width` | `int` | — | 默认 0; 0…32767 | — | 气泡最大宽度；0 或省略时取视口宽度的 76% |
| `max_message_height` | `int` | — | 默认 320; 16…32767 | — | 消息最大高度 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `row_template` | `identifier` | — | — | — | 复用的行模板 |
| `callback` | `identifier` | — | — | — | 应用回调名称 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_charset` | `string` | — | — | — | 运行时绑定消息文字可使用的字形 |
| `scroll_snapshot` | `bool` | — | 默认 `true` | — | 滚动时缓存两个视口帧；内存不足时回退到实时渲染 |
| `item_height` | `int` | — | 默认 0; 0…65535 | — | 消息最小行高；0 或省略时为 2 × bubble_padding_y + message_gap + 1 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
