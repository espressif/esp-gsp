# 按钮（`gsp_button`）

可触发应用回调或声明式动作的点击内容区域。

## 适用场景

适合展示或触发界面中的主要内容。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/button/button.json \
  --deployable -o gsp-out/widget-preview/button.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/button.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

状态图片按禁用、按下、选中、普通的优先级选择。未声明禁用或按下图片时，选中状态回退到 selected_image，其余回退到 image。设置 checkable 为 true 后，点击切换 checked；也可通过生成的 API 设置选中状态。各状态复用普通图片缓存，可混用位图与编译矢量。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "widget_button",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "button",
      "parent": -1,
      "name": "primary",
      "callback": "primary_click",
      "x": 70,
      "y": 70,
      "w": 160,
      "h": 54,
      "text": "Primary",
      "font_size": 18,
      "bg_color": "#2563EB",
      "fg_color": "#FFFFFF",
      "radius": 12
    },
    {
      "type": "button",
      "parent": -1,
      "callback": "secondary_click",
      "x": 250,
      "y": 70,
      "w": 160,
      "h": 54,
      "text": "Secondary",
      "font_size": 18,
      "bg_color": "#26364D",
      "fg_color": "#DCE8F7",
      "border_color": "#4B607D",
      "border_width": 1,
      "radius": 12
    },
    {
      "type": "button",
      "parent": -1,
      "callback": "muted_click",
      "x": 150,
      "y": 160,
      "w": 180,
      "h": 48,
      "text": "Muted",
      "font_size": 16,
      "bg_color": "#334155",
      "fg_color": "#94A3B8",
      "radius": 24
    },
    {
      "type": "button",
      "parent": -1,
      "name": "favorite",
      "x": 190,
      "y": 232,
      "w": 100,
      "h": 56,
      "image": "favorite_normal.svg",
      "pressed_image": "favorite_pressed.svg",
      "selected_image": "favorite_selected.svg",
      "disabled_image": "favorite_disabled.svg",
      "checkable": true
    }
  ]
}
```

该文件来自 `examples/widgets/button/button.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
bool gsp_widget_button_event_decode_call( const esp_gsp_event_t *event, gsp_widget_button_call_event_t *out_event)
bool gsp_widget_button_event_is_muted_click( const esp_gsp_event_t *event)
bool gsp_widget_button_event_is_primary_click( const esp_gsp_event_t *event)
bool gsp_widget_button_event_is_secondary_click( const esp_gsp_event_t *event)
const gsp_component_directory_t *const * gsp_button_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_button_favorite_get_checked(esp_gsp_handle_t gsp, bool *out_checked)
esp_err_t gsp_widget_button_favorite_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_button_favorite_get_enabled(esp_gsp_handle_t gsp, bool *out_enabled)
esp_err_t gsp_widget_button_favorite_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_button_favorite_set_checked(esp_gsp_handle_t gsp, bool checked)
esp_err_t gsp_widget_button_favorite_set_enabled(esp_gsp_handle_t gsp, bool enabled)
esp_err_t gsp_widget_button_primary_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_button_primary_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_button_docs_config(void)
size_t gsp_button_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `image` | `path` | — | — | 是 | 图片文件路径（位图或编译式 SVG） |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `pressed_image` | `path` | — | — | — | 按下状态图片路径 |
| `selected_image` | `path` | — | — | — | 选中状态图片路径 |
| `disabled_image` | `path` | — | — | — | 禁用状态图片路径 |
| `checkable` | `bool` | — | 默认 `false` | — | 点击时切换选中状态 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `layout` | `enum` | — | `row`, `column` | — | 子对象自动布局：行/列 |
| `gap` | `int` | — | 默认 0; 0…4096 | — | 自动布局间距（像素） |
| `padding` | `int` | — | 默认 0; 0…4096 | — | 自动布局内边距（像素） |
| `padding_left` | `int` | — | 0…4096 | — | 行布局起始内边距覆盖值 |
| `padding_right` | `int` | — | 0…4096 | — | 行布局末尾内边距覆盖值 |
| `padding_top` | `int` | — | 0…4096 | — | 列布局起始内边距覆盖值 |
| `padding_bottom` | `int` | — | 0…4096 | — | 列布局末尾内边距覆盖值 |
| `grow` | `int` | — | 默认 0; 0…100 | — | 自动布局扩展权重 |
| `margin` | `int` | — | 默认 0; 0…4096 | — | 子对象两侧的自动布局外边距 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | 文字对齐方式 |
| `overflow` | `enum` | — | 默认 clip; `clip`, `ellipsis` | — | 单行文字溢出方式 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_charset` | `string` | — | — | — | 运行时绑定文字可使用的字形；静态文字会自动加入 |
| `font_charset_file` | `path` | — | — | — | 相对于场景的 UTF-8 字符集文件，与 font_charset 和静态文本合并 |
| `font_link` | `enum` | — | `embedded`, `external`, `auto` | — | 字体存储策略：内嵌/外部/自动 |
| `input` | `bool` | — | 默认 `false` | — | 文字输入字段：接入光标/键盘流程 |
| `animation_codec` | `enum` | — | `lossless`, `jpeg`, `hardware_jpeg` | — | 动画帧策略：无损增量帧、完整 JPEG 帧，或仅在目标具备硬件解码能力时使用 JPEG |
| `svg_layout` | `enum` | — | `content`, `canvas` | — | SVG 部件定位：裁紧内容或保留原画布布局 |
| `morph_to` | `path` | — | — | — | 路径结构和填充一致的 SVG 结束形状 |
| `morph` | `int` | — | 0…100 | — | SVG 形变进度（百分比）；生成运行时设置接口 |
| `svg_element` | `string` | — | — | — | SVG 元素 ID；按绘制边界导入为独立图片 |
| `tint` | `color` | — | — | — | SVG 轮廓颜色；生成运行时颜色设置接口 |
| `codec` | `enum` | — | `raw`, `lossless`, `jpeg`, `auto`, `store`, `qoi`, `rle16`, `default`, `hardware_jpeg` | — | 图片编码格式 |
| `quality` | `int` | — | 1…100 | — | JPEG 质量 1–100（省略时使用 Profile 默认值） |
| `compress` | `bool` | — | — | — | 图片压缩开关（兼容字段；优先使用 codec） |
| `store_scale` | `number` | — | 0.05…1.0 | — | 编码时应用的预缩放比例 |
| `max_fps` | `int` | — | 1…120 | — | GIF/动画帧率上限（0 表示不限制） |
| `fit` | `enum` | — | 默认 stretch; `stretch`, `fill`, `contain`, `cover` | — | 图片适配模式 |
| `position_x` | `number` | — | 默认 0.5; 0.0…1.0 | — | 图片适配的水平对齐位置 |
| `position_y` | `number` | — | 默认 0.5; 0.0…1.0 | — | 图片适配的垂直对齐位置 |
| `rotation` | `int` | — | 默认 0; -32768…32767 | 场景: 图片; 模板: 不支持 | 图片绕边界框中心顺时针旋转，支持源图透明度；超出边界框的部分会被裁剪 |
| `scalable` | `bool` | — | 默认 `false` | — | 启用运行时图片缩放 |
| `scale` | `number` | — | 默认 1.0; 0.0625…16.0 | — | 运行时图片初始缩放比例 |
| `min_scale` | `number` | — | 默认 0.5; 0.0625…16.0 | — | 运行时图片最小缩放比例 |
| `max_scale` | `number` | — | 默认 4.0; 0.0625…16.0 | — | 运行时图片最大缩放比例 |
| `checked` | `bool` | — | 默认 `false` | 是 | 初始开关状态 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource` | — | 显式绑定状态类型 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
