# 滚轮选择器（`gsp_wheel`）

支持行吸附与首尾连续循环的虚拟化选择滚轮。

## 适用场景

适合高效浏览、选择或展示重复的应用数据。

## 本地交互预览

[安装 `esp-gsp-tools`](../guide/simulator-preview.md) 后，在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
python -m gsp.execute --version 0.4.1 gspc pack \
  examples/widgets/wheel/wheel.json \
  --deployable -o gsp-out/widget-preview/wheel.gspb
python -m gsp.execute --version 1.3.1 sim \
  --bundle gsp-out/widget-preview/wheel.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

设置 `cyclic: true` 可在首项和末项之间连续滚动。运行时需要更新条目时，设置 `dynamic_items: true`，即可让 bind、set_total、refresh API 不随初始条目数量变化。使用编译好的条目文字时，以 `(ui, NULL, NULL)` 调用生成的绑定函数一次；应用提供的数据则使用行回调，并用 font_charset 声明后续需要的字符。无需操作 Slot ID，选中项 API 使用原始项目索引。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "widget_wheel",
  "w": 480,
  "h": 320,
  "screen_bg": "#101827",
  "font": "../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "container",
      "parent": -1,
      "x": 75,
      "y": 137,
      "w": 330,
      "h": 46,
      "bg_color": "#2563EB38",
      "border_color": "#38BDF8",
      "border_width": 1,
      "radius": 8
    },
    {
      "type": "wheel",
      "parent": -1,
      "name": "city",
      "x": 75,
      "y": 35,
      "w": 330,
      "h": 250,
      "items": [
        "Shanghai",
        "Shenzhen",
        "Beijing",
        "Chengdu",
        "Hangzhou",
        "Suzhou",
        "Xiamen",
        "Nanjing"
      ],
      "selected": 2,
      "item_height": 46,
      "cyclic": true,
      "snap_to_item": true,
      "font_size": 18,
      "bg_color": "#142033",
      "fg_color": "#E2E8F0",
      "radius": 12
    }
  ]
}
```

该文件来自 `examples/widgets/wheel/wheel.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_wheel_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_wheel_city_animate_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_wheel_city_animate_selected_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_widget_wheel_city_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_wheel_city_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_wheel_city_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_widget_wheel_city_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_widget_wheel_city_play_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_widget_wheel_city_refresh(esp_gsp_handle_t gsp, esp_gsp_list_t list)
esp_err_t gsp_widget_wheel_city_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_widget_wheel_city_set_text( esp_gsp_handle_t gsp, esp_gsp_widget_t widget, const char *utf8)
esp_err_t gsp_widget_wheel_city_set_total(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t total)
esp_err_t gsp_widget_wheel_city_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_widget_wheel_city_stop_selected(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_wheel_docs_config(void)
esp_gsp_err_t gsp_widget_wheel_city_row_set_text( esp_gsp_handle_t gsp, esp_gsp_row_t row, const char *utf8)
esp_gsp_list_t gsp_widget_wheel_city_bind( esp_gsp_handle_t gsp, esp_gsp_list_bind_cb_t bind_item, void *user_ctx)
esp_gsp_widget_t gsp_widget_wheel_city_create( esp_gsp_handle_t gsp, int16_t x, int16_t y)
size_t gsp_wheel_docs_dynamic_image_slots(void)
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
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `snap_to_item` | `bool` | — | 默认 `true` | — | 滚动结束时吸附到行边界 |
| `cyclic` | `bool` | — | 默认 `false` | — | 选择项支持首尾连续循环 |
| `items` | `string_list` | — | — | — | 条目文字（list/wheel/dropdown/tabview） |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中条目索引 |
| `item_height` | `int` | — | 默认 0; 0…65535 | — | 列表/滚轮行高 |

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
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
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
| `dynamic_items` | `bool` | — | 默认 `false` | — | 预留条目更新 API，不随初始条目数量变化 |
| `items_per_page` | `int` | — | 默认 0; 0…65535 | — | TabView 每页条目数 |
| `visible_rows` | `int` | — | 默认 0; 0…65535 | — | 可见行数（item_height 的替代配置） |
| `row_template` | `identifier` | — | — | — | 复用的行模板 |
| `item_count` | `int` | — | 0…4294967295 | — | 动态条目初始数量 |
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
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
