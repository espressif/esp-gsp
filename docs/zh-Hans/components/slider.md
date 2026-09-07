# 滑块（`gsp_slider`）

可拖动的水平或垂直区间数值控件。

## 适用场景

适合让用户查看或调整有明确上下限的数值。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.3.0 gspc pack \
  examples/widgets/slider/slider.json \
  --deployable -o build/widget-preview/slider.gspb
python -m gsp.execute --version 1.2.0 sim \
  --bundle build/widget-preview/slider.gspb
```

这些命令用 GSPC 编译下方同一份 JSON，并通过发布版 ESP-GSP 模拟器打开浏览器预览，
不是 HTML 仿制控件。最终像素格式、字体、显示链路和性能仍需在目标硬件确认。

## 运行方式

鼠标或触控输入通过与设备端相同的运行时属性路径更新组件保留态。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 会为示例中的命名对象生成下方列出的类型化函数；JSON 中没有命名的对象不会产生无用 API。

## 完整示例 JSON

```json
{
  "screen": "widget_slider",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "default_font_size": 18,
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 32,
      "y": 28,
      "w": 416,
      "h": 30,
      "text": "Slider",
      "font_size": 26,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 32,
      "y": 68,
      "w": 416,
      "h": 24,
      "text": "Drag the handle to change the value.",
      "fg_color": "#A9B8CC"
    },
    {
      "type": "slider",
      "parent": -1,
      "name": "volume",
      "x": 32,
      "y": 118,
      "w": 320,
      "h": 32,
      "min": 0,
      "max": 100,
      "value": 68,
      "track_size": 10,
      "radius": 16,
      "bg_color": "#334155",
      "fg_color": "#38BDF8",
      "knob_color": "#F8FAFC"
    },
    {
      "type": "slider",
      "parent": -1,
      "name": "temperature",
      "x": 32,
      "y": 190,
      "w": 320,
      "h": 32,
      "min": 16,
      "max": 30,
      "value": 23,
      "track_size": 8,
      "radius": 16,
      "bg_color": "#334155",
      "fg_color": "#F59E0B",
      "knob_color": "#FEF3C7"
    },
    {
      "type": "slider",
      "parent": -1,
      "name": "vertical_level",
      "x": 398,
      "y": 106,
      "w": 28,
      "h": 152,
      "vertical": true,
      "value": 44,
      "track_size": 9,
      "radius": 14,
      "bg_color": "#334155",
      "fg_color": "#A78BFA",
      "knob_color": "#F5F3FF"
    }
  ]
}
```

该文件来自 `examples/widgets/slider/slider.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_slider_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_slider_temperature_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_slider_temperature_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_slider_temperature_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_widget_slider_vertical_level_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_slider_vertical_level_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_slider_vertical_level_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_widget_slider_volume_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_slider_volume_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_slider_volume_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_gsp_config_t gsp_slider_docs_config(void)
size_t gsp_slider_docs_dynamic_image_slots(void)
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `radius` | `int` | — | 默认 0; 0…65535 | 是 | 圆角半径（像素） |
| `value` | `int` | — | — | 是 | 初始值（位于 min 到 max 范围内） |
| `min` | `int` | — | 默认 0; -2147483648…2147483647 | — | 数值范围下限 |
| `max` | `int` | — | 默认 100; -2147483648…2147483647 | — | 数值范围上限 |
| `vertical` | `bool` | — | 默认 `false` | — | 垂直方向 |
| `knob_color` | `color` | — | 默认 #FFFFFF | — | 滑块/开关旋钮颜色 |
| `track_size` | `int` | — | 默认 0; 0…4096 | — | 轨道粗细 |

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
| `opacity` | `int` | — | 默认 255; 0…255 | 是 | 0–255 混合透明度 |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | 文字对齐方式 |
| `overflow` | `enum` | — | 默认 clip; `clip`, `ellipsis` | — | 单行文字溢出方式 |
| `font_charset` | `string` | — | — | — | 运行时绑定文字可使用的字形；静态文字会自动加入 |
| `font_link` | `enum` | — | `embedded`, `external`, `auto` | — | 字体存储策略：内嵌/外部/自动 |
| `input` | `bool` | — | 默认 `false` | — | 文字输入字段：接入光标/键盘流程 |
| `image` | `path` | — | — | 是 | 图片文件路径（PNG） |
| `codec` | `enum` | — | `raw`, `lossless`, `jpeg`, `auto` | — | 图片编码格式 |
| `quality` | `int` | — | 1…100 | — | JPEG 质量 1–100（0 表示使用 Profile 默认值） |
| `compress` | `bool` | — | — | — | 图片压缩开关（兼容字段；优先使用 codec） |
| `store_scale` | `number` | — | 0.05…1.0 | — | 编码时应用的预缩放比例 |
| `max_fps` | `int` | — | 1…120 | — | GIF/动画帧率上限（0 表示不限制） |
| `fit` | `enum` | — | 默认 stretch; `stretch`, `fill`, `contain`, `cover` | — | 图片适配模式 |
| `position_x` | `number` | — | 默认 0.5; 0.0…1.0 | — | 图片适配的水平对齐位置 |
| `position_y` | `number` | — | 默认 0.5; 0.0…1.0 | — | 图片适配的垂直对齐位置 |
| `rotation` | `int` | — | 默认 0; -32768…32767 | 是 | 不透明图片绕边界框中心顺时针旋转的角度；超出边界框的部分会被裁剪 |
| `scalable` | `bool` | — | 默认 `false` | — | 启用运行时图片缩放 |
| `scale` | `number` | — | 默认 1.0; 0.0625…16.0 | — | 运行时图片初始缩放比例 |
| `min_scale` | `number` | — | 默认 0.5; 0.0625…16.0 | — | 运行时图片最小缩放比例 |
| `max_scale` | `number` | — | 默认 4.0; 0.0625…16.0 | — | 运行时图片最大缩放比例 |
| `knob` | `bool` | — | 默认 `true` | — | 是否显示旋钮 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成 GSP_ACT_ID_&lt;NAME&gt; |
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
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |

</details>
