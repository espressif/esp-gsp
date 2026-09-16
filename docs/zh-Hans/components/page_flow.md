# 页面流（`gsp_page_flow`）

支持横向或纵向滑动的多页面视口。

## 适用场景

适合组织内容、可见性或导航，同时保留清晰的场景层级。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/page_flow/page_flow.json \
  --deployable -o gsp-out/widget-preview/page_flow.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/page_flow.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

子节点保留原始层级。在 Container 或 Layer 上设置 clip_children，可将子内容绘制和触摸区域裁剪到容器矩形边界。嵌套裁剪取交集，并随父容器移动。视口使用固定 w/h；radius 设置背景与边框圆角。

使用 `pages: ["first", "second"]` 显式指定页面根节点，列表顺序就是页面索引；数量必须等于 `page_count`，每项必须唯一引用一个直接子对象。页面仍需沿 `axis` 按视口宽度或高度排列。未列入 `pages` 的子对象保持固定。省略 `pages` 时兼容 `<name>_tab0`、`<name>_tab1` 等直接子对象命名。嵌套变换按父子关系计算；对象数组顺序仍决定绘制顺序。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "widget_page_flow",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "swipe": false,
  "objects": [
    {
      "type": "page_flow",
      "parent": -1,
      "name": "onboarding",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "page_count": 3,
      "selected": 0,
      "axis": "horizontal",
      "cyclic": true,
      "bar_height": 0,
      "pages": [
        "onboarding_tab0",
        "onboarding_tab1",
        "onboarding_tab2"
      ]
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "onboarding_tab0",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#123A5A"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 55,
      "y": 95,
      "w": 370,
      "h": 55,
      "text": "Page Flow 1",
      "font_size": 30,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 55,
      "y": 165,
      "w": 370,
      "h": 40,
      "text": "Swipe horizontally",
      "text_align": "center",
      "fg_color": "#BAE6FD"
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "onboarding_tab1",
      "x": 480,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#4A234F"
    },
    {
      "type": "label",
      "parent": 4,
      "x": 55,
      "y": 95,
      "w": 370,
      "h": 55,
      "text": "Page Flow 2",
      "font_size": 30,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 4,
      "x": 55,
      "y": 165,
      "w": 370,
      "h": 40,
      "text": "Content follows the gesture",
      "text_align": "center",
      "fg_color": "#F5D0FE"
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "onboarding_tab2",
      "x": 960,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#234A38"
    },
    {
      "type": "label",
      "parent": 7,
      "x": 55,
      "y": 95,
      "w": 370,
      "h": 55,
      "text": "Page Flow 3",
      "font_size": 30,
      "text_align": "center",
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 7,
      "x": 55,
      "y": 165,
      "w": 370,
      "h": 40,
      "text": "Cyclic navigation enabled",
      "text_align": "center",
      "fg_color": "#BBF7D0"
    }
  ]
}
```

该文件来自 `examples/widgets/page_flow/page_flow.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_page_flow_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_page_flow_onboarding_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_page_flow_onboarding_tab0_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_tab0_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_page_flow_onboarding_tab1_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_tab1_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_page_flow_onboarding_tab2_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_page_flow_onboarding_tab2_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_page_flow_docs_config(void)
size_t gsp_page_flow_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 自身填充 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 自身填充 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 自身填充; 模板: 自身填充 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 自身填充; 模板: 自身填充 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中条目索引 |
| `cyclic` | `bool` | — | 默认 `false` | — | 滚轮首尾连续循环 |
| `pages` | `string_list` | — | — | — | 按页面索引排列的直接子对象名称；优先于旧版 &lt;name&gt;_tabN 命名规则 |
| `page_count` | `int` | — | 1…65535 | — | 编译后的页面数量 |
| `axis` | `enum` | — | 默认 horizontal; `horizontal`, `vertical` | — | 页面运动轴向 |
| `bar_height` | `int` | — | 默认 56; 0…4096 | — | 选项卡栏高度 |

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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
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
| `items` | `string_list` | — | — | — | 条目文字（list/wheel/dropdown/tabview） |
| `item_height` | `int` | — | 默认 0; 0…65535 | — | 列表/滚轮行高 |
| `items_per_page` | `int` | — | 默认 0; 0…65535 | — | TabView 每页条目数 |
| `visible_rows` | `int` | — | 默认 0; 0…65535 | — | 可见行数（item_height 的替代配置） |
| `snap_to_item` | `bool` | — | 默认 `false` | — | 滚动结束时吸附到行边界 |
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
| `stop_anywhere` | `bool` | — | 默认 `false` | — | 允许 PageFlow 在页面之间回弹定位 |
| `page_extent` | `int` | — | 默认 0; 0…65535 | — | PageFlow 拖动范围 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
