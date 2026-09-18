# 容器（`gsp_container`）

支持分组、可选矩形子树裁剪和自动布局的父容器。

## 适用场景

适合组织内容、可见性或导航，同时保留清晰的场景层级。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.5.0 gspc pack \
  examples/widgets/container/container.json \
  --deployable -o gsp-out/widget-preview/container.gspb
python -m gsp.execute --version 1.4.0 sim \
  --bundle gsp-out/widget-preview/container.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

子节点保留原始层级。在 Container 或 Layer 上设置 clip_children，可将子内容绘制和触摸区域裁剪到容器矩形边界。嵌套裁剪取交集，并随父容器移动。视口使用固定 w/h；radius 设置背景与边框圆角。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "widget_container",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "container",
      "parent": -1,
      "name": "card",
      "x": 55,
      "y": 45,
      "w": 370,
      "h": 220,
      "layout": "column",
      "padding": 22,
      "gap": 12,
      "bg_color": "#172235",
      "border_color": "#314766",
      "border_width": 1,
      "radius": 18,
      "clip_children": true
    },
    {
      "type": "label",
      "parent_name": "card",
      "x": 0,
      "y": 0,
      "w": 326,
      "h": 32,
      "text": "Container card",
      "font_size": 22,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent_name": "card",
      "x": 0,
      "y": 0,
      "w": 326,
      "h": 50,
      "text": "Containers group content and layout. clip_children keeps child pixels and input inside the card.",
      "font_size": 15,
      "fg_color": "#9FB0C7"
    },
    {
      "type": "container",
      "parent_name": "card",
      "name": "nested",
      "x": 0,
      "y": 0,
      "w": 326,
      "h": 58,
      "bg_color": "#21324A",
      "radius": 10
    },
    {
      "type": "label",
      "parent_name": "nested",
      "x": 16,
      "y": 16,
      "w": 294,
      "h": 24,
      "text": "Nested content",
      "font_size": 15,
      "fg_color": "#7DD3FC"
    }
  ]
}
```

该文件来自 `examples/widgets/container/container.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_container_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_container_card_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_container_card_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_container_nested_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_container_nested_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_container_docs_config(void)
size_t gsp_container_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 自身填充 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 自身填充 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 自身填充; 模板: 自身填充 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 自身填充; 模板: 自身填充 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `layout` | `enum` | — | `row`, `column` | — | 子对象自动布局：行/列 |
| `gap` | `int` | — | 默认 0; 0…4096 | — | 自动布局间距（像素） |
| `padding` | `int` | — | 默认 0; 0…4096 | — | 自动布局内边距（像素） |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `clip_children` | `bool` | — | 默认 `false` | — | 将子内容绘制和触摸区域裁剪到容器边界 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `padding_left` | `int` | — | 0…4096 | — | 行布局起始内边距覆盖值 |
| `padding_right` | `int` | — | 0…4096 | — | 行布局末尾内边距覆盖值 |
| `padding_top` | `int` | — | 0…4096 | — | 列布局起始内边距覆盖值 |
| `padding_bottom` | `int` | — | 0…4096 | — | 列布局末尾内边距覆盖值 |
| `grow` | `int` | — | 默认 0; 0…100 | — | 自动布局扩展权重 |
| `margin` | `int` | — | 默认 0; 0…4096 | — | 子对象两侧的自动布局外边距 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | 文字对齐方式 |
| `overflow` | `enum` | — | 默认 clip; `clip`, `ellipsis` | — | 单行文字溢出方式 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
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
| `image` | `path` | — | — | 是 | 图片文件路径（位图或编译式 SVG） |
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
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|

</details>
