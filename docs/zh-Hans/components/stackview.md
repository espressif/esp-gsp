# 堆栈视图（`gsp_stackview`）

通过动画切换实现页面入栈和出栈的导航容器。

## 适用场景

适合组织内容、可见性或导航，同时保留清晰的场景层级。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p build/widget-preview
python -m gsp.execute --version 0.2.8 gspc pack \
  examples/widgets/stackview/stackview.json \
  --deployable -o build/widget-preview/stackview.gspb
python -m gsp.execute --version 1.1.0 sim \
  --bundle build/widget-preview/stackview.gspb
```

这些命令用 GSPC 编译下方同一份 JSON，并通过发布版 ESP-GSP 模拟器打开浏览器预览，
不是 HTML 仿制控件。最终像素格式、字体、显示链路和性能仍需在目标硬件确认。

## 运行方式

子节点保留原始层级，组件负责裁剪、布局、可见性或转场。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 会为示例中的命名对象生成下方列出的类型化函数；JSON 中没有命名的对象不会产生无用 API。

## 完整示例 JSON

```json
{
  "screen": "widget_stackview",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "stackview",
      "parent": -1,
      "name": "details_stack",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "page_count": 3,
      "initial_page": 0,
      "capacity": 3,
      "axis": "horizontal"
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "details_stack_page0",
      "x": 0,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#142033"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 28,
      "y": 22,
      "w": 424,
      "h": 36,
      "text": "ESP-GSP Runtime",
      "font_size": 25,
      "fg_color": "#F8FAFC"
    },
    {
      "type": "label",
      "parent": 1,
      "x": 28,
      "y": 62,
      "w": 424,
      "h": 28,
      "text": "Production display profile · RGB888",
      "font_size": 16,
      "fg_color": "#8EA4BC"
    },
    {
      "type": "container",
      "parent": 1,
      "x": 28,
      "y": 106,
      "w": 202,
      "h": 90,
      "bg_color": "#1B2B40",
      "border_color": "#304762",
      "border_width": 1,
      "radius": 12
    },
    {
      "type": "label",
      "parent": 4,
      "x": 16,
      "y": 13,
      "w": 170,
      "h": 24,
      "text": "FRAME RATE",
      "font_size": 14,
      "fg_color": "#7DD3FC"
    },
    {
      "type": "label",
      "parent": 4,
      "x": 16,
      "y": 43,
      "w": 170,
      "h": 34,
      "text": "60 FPS",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "container",
      "parent": 1,
      "x": 250,
      "y": 106,
      "w": 202,
      "h": 90,
      "bg_color": "#1B2B40",
      "border_color": "#304762",
      "border_width": 1,
      "radius": 12
    },
    {
      "type": "label",
      "parent": 7,
      "x": 16,
      "y": 13,
      "w": 170,
      "h": 24,
      "text": "RENDER PATH",
      "font_size": 14,
      "fg_color": "#86EFAC"
    },
    {
      "type": "label",
      "parent": 7,
      "x": 16,
      "y": 43,
      "w": 170,
      "h": 34,
      "text": "WASM",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "button",
      "parent": 1,
      "x": 120,
      "y": 228,
      "w": 240,
      "h": 56,
      "text": "Open live metrics  →",
      "font_size": 17,
      "bg_color": "#2563EB",
      "fg_color": "#FFFFFF",
      "radius": 13,
      "events": [
        {
          "event": "click",
          "action": "stack_push",
          "target_name": "details_stack",
          "arg": 1
        }
      ]
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "details_stack_page1",
      "x": 480,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#251B38"
    },
    {
      "type": "label",
      "parent": 11,
      "x": 28,
      "y": 20,
      "w": 424,
      "h": 36,
      "text": "Live metrics",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "label",
      "parent": 11,
      "x": 28,
      "y": 58,
      "w": 424,
      "h": 26,
      "text": "Renderer workload over the latest frames",
      "font_size": 16,
      "fg_color": "#C4B5D5"
    },
    {
      "type": "chart",
      "parent": 11,
      "x": 28,
      "y": 98,
      "w": 424,
      "h": 120,
      "points": [
        18,
        34,
        27,
        58,
        49,
        76,
        64,
        88,
        71,
        94
      ],
      "grid_lines": 3,
      "bg_color": "#1C152B",
      "fg_color": "#A78BFA",
      "border_color": "#493566",
      "border_width": 1,
      "radius": 11
    },
    {
      "type": "label",
      "parent": 11,
      "x": 304,
      "y": 105,
      "w": 128,
      "h": 27,
      "text": "58.7 FPS",
      "font_size": 17,
      "text_align": "right",
      "fg_color": "#F5F3FF"
    },
    {
      "type": "button",
      "parent": 11,
      "x": 28,
      "y": 246,
      "w": 154,
      "h": 48,
      "text": "←  Back",
      "font_size": 17,
      "bg_color": "#46345F",
      "fg_color": "#FFFFFF",
      "radius": 11,
      "events": [
        {
          "event": "click",
          "action": "stack_pop",
          "target_name": "details_stack"
        }
      ]
    },
    {
      "type": "button",
      "parent": 11,
      "x": 202,
      "y": 246,
      "w": 250,
      "h": 48,
      "text": "Display settings  →",
      "font_size": 17,
      "bg_color": "#7C3AED",
      "fg_color": "#FFFFFF",
      "radius": 11,
      "events": [
        {
          "event": "click",
          "action": "stack_push",
          "target_name": "details_stack",
          "arg": 2
        }
      ]
    },
    {
      "type": "layer",
      "parent": 0,
      "name": "details_stack_page2",
      "x": 960,
      "y": 0,
      "w": 480,
      "h": 320,
      "bg_color": "#12362F"
    },
    {
      "type": "label",
      "parent": 18,
      "x": 28,
      "y": 22,
      "w": 424,
      "h": 36,
      "text": "Display settings",
      "font_size": 25,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "container",
      "parent": 18,
      "x": 28,
      "y": 82,
      "w": 424,
      "h": 92,
      "bg_color": "#19483E",
      "border_color": "#2B6658",
      "border_width": 1,
      "radius": 12
    },
    {
      "type": "label",
      "parent": 20,
      "x": 18,
      "y": 16,
      "w": 280,
      "h": 28,
      "text": "High quality rendering",
      "font_size": 18,
      "fg_color": "#FFFFFF"
    },
    {
      "type": "toggle",
      "parent": 20,
      "x": 326,
      "y": 26,
      "w": 72,
      "h": 36,
      "checked": true,
      "bg_color": "#285447",
      "fg_color": "#34D399"
    },
    {
      "type": "label",
      "parent": 20,
      "x": 18,
      "y": 50,
      "w": 280,
      "h": 24,
      "text": "RGB888 · 2× supersampling",
      "font_size": 14,
      "fg_color": "#A7F3D0"
    },
    {
      "type": "button",
      "parent": 18,
      "x": 28,
      "y": 232,
      "w": 180,
      "h": 50,
      "text": "←  Back",
      "font_size": 17,
      "bg_color": "#285447",
      "fg_color": "#FFFFFF",
      "radius": 11,
      "events": [
        {
          "event": "click",
          "action": "stack_pop",
          "target_name": "details_stack"
        }
      ]
    }
  ]
}
```

该文件来自 `examples/widgets/stackview/stackview.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_stackview_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_stackview_details_stack_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_stackview_docs_config(void)
size_t gsp_stackview_docs_dynamic_image_slots(void)
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
| `page_count` | `int` | — | 1…65535 | — | 编译后的页面数量 |
| `initial_page` | `int` | — | 默认 0; 0…65534 | — | 堆栈初始根页面 |
| `capacity` | `int` | — | 默认 8; 1…8 | — | 最大堆栈深度 |
| `axis` | `enum` | — | 默认 horizontal; `horizontal`, `vertical` | — | 运动轴向 |

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
| `opacity` | `int` | — | 默认 255; 0…255 | 是 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 是 | 圆角半径（像素） |
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
| `scalable` | `bool` | — | 默认 `false` | — | 启用运行时图片缩放 |
| `scale` | `number` | — | 默认 1.0; 0.0625…16.0 | — | 运行时图片初始缩放比例 |
| `min_scale` | `number` | — | 默认 0.5; 0.0625…16.0 | — | 运行时图片最小缩放比例 |
| `max_scale` | `number` | — | 默认 4.0; 0.0625…16.0 | — | 运行时图片最大缩放比例 |
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
| `transition_ms` | `int` | — | 默认 0; 0…65535 | — | 程序调用 push/pop 的时长；0 表示沿用运行时默认值 |
| `transition_easing` | `enum` | — | 默认 linear; `linear`, `ease_out`, `ease_in_out` | — | 程序调用 push/pop 的缓动曲线 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |

</details>
