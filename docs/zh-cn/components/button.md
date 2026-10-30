# 按钮（`gsp_button`）

可触发应用回调或声明式动作的点击内容区域。

## 适用场景

适合承载页面中的主要文字、图片或操作。

## 本地交互预览

先按[兼容性页的工具命令](../reference/compatibility.md#工具命令)配置 `gspc` 和 `gsp_sim_host`，再在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/button/button.json \
  --deployable -o gsp-out/widget-preview/button.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/button.gspb --frames 0
```

这些命令会编译当前示例，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

为选中状态选择一个写入方。由应用 C 设置 `checked` 时，使用 `selected_image`/`checked`
并保持自动切换 `checkable` 关闭，避免同步点击回调与同一点击序列里的声明式切换重复
写入状态。

圆形按钮设置相同的 w/h，并令 radius = w/2。硬阴影默认跟随按钮半径，shadow_spread 扩大阴影范围。目前不支持阴影模糊，也不会按 radius 裁剪图片；圆形图片请使用透明背景。触摸区域仍是矩形。

要在按下时变暗并隐藏阴影，添加 pressed_dim: 64 和 pressed_shadow: false。pressed_dim 范围为 0（不变）至 255（全黑）。pressed_color 可指定按下背景色；边框、轮廓和文字保持原色。press_feedback: false 关闭自动变暗，不能与 pressed_dim 或 pressed_color 同时声明。

以上字段均可省略。声明其中任意字段后，默认变暗强度为 96、保留阴影；未声明时沿用原有全局按下反馈。越过边缘余量或父手势接管会取消 release/click 并恢复外观；应用清理使用 `cancel`。

状态图片优先级为禁用、按下、选中、普通。pressed_image 优先于图片自动变暗；缺少对应图片时，选中按钮回退到 selected_image，否则回退到 image。checkable: true 会在点击时切换 checked，并要求提供 selected_image；也可通过生成的 API 设置 checked。位图与编译式 SVG 使用同一套图片缓存机制，不同资源分别占用缓存。

自动变暗基于声明时的颜色和静态图片生成。绑定或主题背景需提供 pressed_color；之后修改普通背景色不会重新生成按下样式。动画或带 tint 的图片请提供 pressed_image。状态图片不能共用运行时 resource 绑定；按下样式和状态图片均不支持回收模板。

图片变暗会增加一个编译资源，必要时也生成选中态变体。设备无需实时计算变暗像素，但资源包和缓存占用会增加，首次显示仍可能需要解码。

如果应用代码需要读写某个对象，请为它设置稳定的 `name`。GSPC 会为命名对象生成下方列出的类型化函数。

## 场景示例

```json
{"screen":"widget_button","w":480,"h":320,"screen_bg":"#101827","font":"../../../common/fonts/DejaVuSans.ttf","objects":[{"type":"button","parent":-1,"name":"primary","callback":"primary_click","x":70,"y":70,"w":160,"h":54,"text":"Primary","font_size":18,"bg_color":"#2563EB","fg_color":"#FFFFFF","radius":12,"shadow_color":"#000000","shadow_offset_y":4,"pressed_dim":64,"pressed_shadow":false},{"type":"button","parent":-1,"callback":"secondary_click","x":250,"y":70,"w":160,"h":54,"text":"Secondary","font_size":18,"bg_color":"#26364D","fg_color":"#DCE8F7","border_color":"#4B607D","border_width":1,"radius":12,"pressed_color":"#142033"},{"type":"button","parent":-1,"callback":"muted_click","x":150,"y":160,"w":180,"h":48,"text":"Muted","font_size":16,"bg_color":"#334155","fg_color":"#94A3B8","radius":24},{"type":"button","parent":-1,"name":"favorite","x":190,"y":232,"w":100,"h":56,"image":"favorite_normal.svg","pressed_image":"favorite_pressed.svg","selected_image":"favorite_selected.svg","disabled_image":"favorite_disabled.svg","checkable":true},{"type":"button","parent":-1,"callback":"secondary_click","x":40,"y":225,"w":64,"h":64,"bg_color":"#2563EB","radius":32,"shadow_color":"#000000","shadow_offset_y":4,"shadow_spread":2,"pressed_dim":64,"pressed_shadow":false}]}
```

规范示例来自 [`button.json`](../../../examples/usage/widgets/button/button.json)。复制时请一并复制它引用的相对资源。

## 此场景生成的 C API

`dispatch_call` 仅在 ELF GCC/Clang 目标定义 `GSP_GENERATE_WEAK_EVENT_HANDLERS` 时启用；见[可选业务处理函数](../guide/customization.md)。

```c
bool gsp_widget_button_dispatch_call( esp_gsp_handle_t gsp, const esp_gsp_event_t *event, void *user_ctx)
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

## 此示例使用的字段

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
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `image` | `path` | — | — | 是 | 图片、动图、视频（可选 FFmpeg）或编译 SVG 的文件路径 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `pressed_image` | `path` | — | — | — | 按下状态图片路径 |
| `selected_image` | `path` | — | — | — | 选中状态图片路径 |
| `disabled_image` | `path` | — | — | — | 禁用状态图片路径 |
| `checkable` | `bool` | — | 默认 `false` | — | 点击时切换选中状态 |
| `pressed_dim` | `int` | — | 0…255 | — | 按下时背景和图片的变暗强度，0 不变、255 全黑 |
| `pressed_color` | `color` | — | — | — | 按下背景色，替代背景自动变暗 |
| `pressed_shadow` | `bool` | — | — | — | 按下时保留静态阴影（默认 true） |

<details>
<summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `min_width` | `int` | — | 0…65535 | — | 编译期最小宽度，单位像素 |
| `max_width` | `int` | — | 0…65535 | — | 编译期最大宽度，单位像素 |
| `min_height` | `int` | — | 0…65535 | — | 编译期最小高度，单位像素 |
| `max_height` | `int` | — | 0…65535 | — | 编译期最大高度，单位像素 |
| `layout` | `enum` | — | `row`, `column`, `grid` | — | 子对象自动布局：行/列/网格 |
| `layout_wrap` | `bool` | — | 默认 `false` | — | 将行/列子对象换行到其他行 |
| `grid_columns` | `int` | — | 1…65535 | — | 网格布局的等宽列数 |
| `grid_column_span` | `int` | — | 默认 1; 1…65535 | — | 父网格布局中的列跨度 |
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
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | 文字对齐方式 |
| `overflow` | `enum` | — | 默认 clip; `clip`, `ellipsis` | — | 单行文字溢出方式 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_charset` | `string` | — | — | — | 运行时绑定文字可使用的字形；静态文字会自动加入 |
| `font_charset_file` | `path` | — | — | — | 相对于场景的 UTF-8 字符集文件，与 font_charset 和静态文本合并 |
| `font_link` | `enum` | — | `embedded`, `external`, `auto` | — | 字体存储策略：内嵌/外部/自动 |
| `input` | `bool` | — | 默认 `false` | — | 文字输入字段：接入光标/键盘流程 |
| `bg_opacity` | `int` | — | 默认 255; 0…255 | — | 仅背景透明度，与 opacity 和颜色 alpha 相乘 |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `border_side` | `enum` | — | 默认 all; `all`, `none`, `top`, `bottom`, `left`, `right`, `horizontal`, `vertical` | — | 内侧边框选择；局部边要求静态直角矩形几何 |
| `animation_codec` | `enum` | — | `lossless`, `jpeg`, `hardware_jpeg` | — | 动画帧策略：无损增量帧、完整 JPEG 帧，或仅在目标具备硬件解码能力时使用 JPEG |
| `svg_layout` | `enum` | — | `content`, `canvas` | — | SVG 部件定位：裁紧内容或保留原画布布局 |
| `morph_to` | `path` | — | — | — | 路径结构和填充一致的 SVG 结束形状 |
| `morph` | `int` | — | 0…100 | — | SVG 形变进度（百分比）；生成运行时设置接口 |
| `svg_element` | `string` | — | — | — | SVG 元素 ID；按绘制边界导入为独立图片 |
| `tint` | `color` | — | — | — | SVG 轮廓颜色；生成运行时颜色设置接口 |
| `video_start_ms` | `int` | — | 0…4294967295 | — | 视频片段起点，单位毫秒 |
| `video_duration_ms` | `int` | — | 1…4294967295 | — | 视频片段时长，单位毫秒；省略则导入到结尾 |
| `video_fit` | `enum` | — | `contain`, `cover`, `stretch` | — | 构建期视频宽高比适配；contain 使用黑色补边 |
| `video_loop_count` | `int` | — | 0…65535 | — | 视频播放次数（默认 1；0 无限循环） |
| `video_max_decoded_bytes` | `int` | — | 1…9223372036854775807 | — | 主机保留的 RGBA 像素预算（默认 256 MiB，不代表进程总内存） |
| `video_max_frames` | `int` | — | 1…65535 | — | 最大视频解码帧数（默认 4096） |
| `codec` | `enum` | — | `raw`, `lossless`, `jpeg`, `auto`, `speed`, `size`, `store`, `qoi`, `rle16`, `rle16_a8`, `rle32`, `default`, `hardware_jpeg` | — | 图片编码格式 |
| `dither` | `bool` | — | — | — | 减轻图片和动图的 RGB565 色阶；覆盖 Profile 的 image_dither |
| `quality` | `int` | — | 1…100 | — | JPEG 质量 1–100（省略时使用 Profile 默认值） |
| `jpeg_quality` | `int` | — | 1…100 | — | 旧版 JPEG 质量字段别名 |
| `compress` | `bool` | — | — | — | 图片压缩开关（兼容字段；优先使用 codec） |
| `cache_policy` | `enum` | — | `mmap_direct`, `mmap`, `decode_lru`, `lru`, `preload` | — | 图片缓存策略：mmap_direct、decode_lru 或 preload |
| `store_scale` | `number` | — | 0.05…1.0 | — | 编码时应用的预缩放比例 |
| `max_fps` | `int` | — | 0…120 | — | 动画导入帧率上限；视频默认 30，显式 0 保留源时间戳 |
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
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `press_feedback` | `bool` | — | — | — | 启用匹配形状的按下反馈；false 禁用自动变暗 |

</details>

<details>
<summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
