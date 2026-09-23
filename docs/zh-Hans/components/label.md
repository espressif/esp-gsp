# 标签（`gsp_label`）

支持对齐、溢出和运行时绑定的 UTF-8 文本。

## 适用场景

适合展示或触发界面中的主要内容。

## 本地交互预览

先按[兼容性页的工具命令](../reference/compatibility.md#工具命令)配置 `gspc` 和
`gsp_sim_host`，再在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/label/label.json \
  --deployable -o gsp-out/widget-preview/label.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/label.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

内容编译进场景后，可参与指针动作、数据绑定和生成式运行时 API。

静态文字保留 LF、CRLF 和空行，行高由字体度量决定。动态多行文字使用 `text_align: left` 和 `overflow: clip`，盒高至少容纳两个实际字体行高。可先运行 `gspc diagnose scene.json`；对齐和溢出问题会定位到相应字段，盒高不足时会报告所需像素数。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "widget_label",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "name": "heading",
      "x": 40,
      "y": 42,
      "w": 400,
      "h": 42,
      "text": "ESP-GSP Label",
      "font_size": 30,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 40,
      "y": 105,
      "w": 400,
      "h": 44,
      "text": "Labels support UTF-8 text, alignment, colors and runtime-bound content.",
      "font_size": 17,
      "fg_color": "#A9B8CC"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 40,
      "y": 164,
      "w": 400,
      "h": 34,
      "text": "Centered text",
      "text_align": "center",
      "font_size": 18,
      "bg_color": "#172235",
      "fg_color": "#7DD3FC",
      "radius": 8
    },
    {
      "type": "label",
      "parent": -1,
      "x": 40,
      "y": 208,
      "w": 210,
      "h": 28,
      "text": "Long text clips at the boundary",
      "overflow": "ellipsis",
      "fg_color": "#94A3B8"
    },
    {
      "type": "label",
      "name": "status",
      "bind": "status",
      "x": 40,
      "y": 244,
      "w": 400,
      "h": 60,
      "font_size": 17,
      "text_align": "left",
      "overflow": "clip",
      "fg_color": "#7DD3FC",
      "text": "Dynamic text\nTwo aligned lines",
      "font_charset": " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,!?"
    }
  ]
}
```

该文件来自 `examples/usage/widgets/label/label.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_label_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_label_heading_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_label_heading_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_label_status_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_label_status_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_label_status_set_text( esp_gsp_handle_t gsp, const char *text)
esp_gsp_config_t gsp_label_docs_config(void)
size_t gsp_label_docs_dynamic_image_slots(void)
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
| `fg_color` | `color` | — | 默认 #FFFFFF | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | 文字对齐方式 |
| `overflow` | `enum` | — | 默认 clip; `clip`, `ellipsis` | — | 单行文字溢出方式 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `font_charset` | `string` | — | — | — | 运行时绑定文字可使用的字形；静态文字会自动加入 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `min_width` | `int` | — | 0…65535 | — | 编译期最小宽度，单位像素 |
| `max_width` | `int` | — | 0…65535 | — | 编译期最大宽度，单位像素 |
| `min_height` | `int` | — | 0…65535 | — | 编译期最小高度，单位像素 |
| `max_height` | `int` | — | 0…65535 | — | 编译期最大高度，单位像素 |
| `layout` | `enum` | — | `row`, `column` | — | 子对象自动布局：行/列 |
| `gap` | `int` | — | 默认 0; 0…4096 | — | 自动布局间距（像素） |
| `padding` | `int` | — | 默认 0; 0…4096 | — | 自动布局内边距（像素） |
| `padding_left` | `int` | — | 0…4096 | — | 行布局起始内边距覆盖值 |
| `padding_right` | `int` | — | 0…4096 | — | 行布局末尾内边距覆盖值 |
| `padding_top` | `int` | — | 0…4096 | — | 列布局起始内边距覆盖值 |
| `padding_bottom` | `int` | — | 0…4096 | — | 列布局末尾内边距覆盖值 |
| `grow` | `int` | — | 默认 0; 0…100 | — | 自动布局扩展权重 |
| `margin` | `int` | — | 默认 0; 0…4096 | — | 子对象两侧的自动布局外边距 |
| `margin_left` | `int` | — | 0…4096 | — | 自动布局 x 轴起始外边距 |
| `margin_right` | `int` | — | 0…4096 | — | 自动布局 x 轴末尾外边距 |
| `margin_top` | `int` | — | 0…4096 | — | 自动布局 y 轴起始外边距 |
| `margin_bottom` | `int` | — | 0…4096 | — | 自动布局 y 轴末尾外边距 |
| `align_main` | `enum` | — | `start`, `center`, `end`, `space_between` | — | 自动布局主轴对齐方式 |
| `align_cross` | `enum` | — | `start`, `center`, `end`, `stretch` | — | 自动布局交叉轴对齐方式 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `bg_opacity` | `int` | — | 默认 255; 0…255 | — | 仅背景透明度，与 opacity 和颜色 alpha 相乘 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `border_side` | `enum` | — | 默认 all; `all`, `none`, `top`, `bottom`, `left`, `right`, `horizontal`, `vertical` | — | 内侧边框选择；局部边要求静态直角矩形几何 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
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
| `codec` | `enum` | — | `raw`, `lossless`, `jpeg`, `auto`, `speed`, `size`, `store`, `qoi`, `rle16`, `rle16_a8`, `rle32`, `default`, `hardware_jpeg` | — | 图片编码格式 |
| `quality` | `int` | — | 1…100 | — | JPEG 质量 1–100（省略时使用 Profile 默认值） |
| `jpeg_quality` | `int` | — | 1…100 | — | 旧版 JPEG 质量字段别名 |
| `compress` | `bool` | — | — | — | 图片压缩开关（兼容字段；优先使用 codec） |
| `cache_policy` | `enum` | — | `mmap_direct`, `mmap`, `decode_lru`, `lru`, `preload` | — | 图片缓存策略：mmap_direct、decode_lru 或 preload |
| `store_scale` | `number` | — | 0.05…1.0 | — | 编码时应用的预缩放比例 |
| `max_fps` | `int` | — | 0…120 | — | GIF/动画帧率上限（0 表示不限制） |
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
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
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
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
