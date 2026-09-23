# ESP-GSP 场景编写参考

GSPC 版本：0.6.0。

本页是 JSON 场景的完整字段参考。学习单个控件时优先使用
[控件库](../components/index.md)中的可运行示例。

需要运行时更新的字段，按表中标注的动态能力声明。
声明形式、自身绘制与子树作用范围见[动态属性范围](../guide/scenes.md#动态属性范围)。

## 场景键

| 键 | 含义 |
|---|---|
| `screen` | 场景名称，同时决定生成符号和文件名 |
| `w` | 逻辑宽度（像素） |
| `h` | 逻辑高度（像素） |
| `screen_bg` | 所有对象背后的场景背景色 |
| `font` | 烘焙文字使用的默认 TTF/OTF 路径 |
| `default_font_size` | 默认字体像素大小，默认值为 16 |
| `font_charset` | 运行时文本的补充字符，静态文本自动加入 |
| `font_charset_file` | 相对于场景的 UTF-8 字符集文件路径，可由对象覆盖 |
| `font_max_bytes` | 可选的正整数字节上限，约束该场景生成的全部 GFB 资源 |
| `objects` | 对象数组；数组顺序就是绘制顺序 |
| `styles` | 命名属性默认值集合，对象通过 `style` 使用 |
| `themes` | 命名颜色 Token；默认静态，设置 `dynamic: true` 后可动态更新 |
| `components` | 用户定义的参数化组合，通过 `use` 实例化 |
| `swipe` | 是否允许横向拖动切换场景，默认启用 |

## 事件

| 事件 | 含义 |
|---|---|
| `click` | 指针在对象上完成一次激活 |
| `press` | 指针进入对象的按下状态 |
| `release` | 指针释放；slider/arc 的 call 返回最终值，拖出控件后释放也有效 |
| `long` | 普通控件按住 500 ms 且未拖动，触发一次；释放时不再触发 click |
| `value` | slider/arc 值变化；call 的 arg 是已提交的 min/max 声明单位值 |

## 动作

| 动作 | 含义 |
|---|---|
| `show` | 显示目标对象 |
| `hide` | 隐藏目标对象 |
| `toggle` | 切换目标对象可见性 |
| `set_text` | 用 `param` 替换目标文字 Bind |
| `set_bg_color` | 用 `arg` 设置目标颜色 Bind：`#RRGGBB` 按目标格式转换，整数按原生像素值解释；透明度单独配置 |
| `call` | 调用命名的应用回调 |
| `goto` | 显示目标 Layer 或跳转到场景 ID |
| `back` | 编译期拒绝；使用 call 配合名为 back 的 callback，或使用 stack_pop |
| `set_value` | 设置目标数值 Bind |
| `toggle_value` | 在 0 和 1 之间切换目标数值 Bind |
| `add_value` | 把 `arg` 加到目标数值 Bind |
| `set_page` | 选择目标 PageFlow 页面 |
| `stack_push` | 向目标 StackView 压入页面 |
| `stack_pop` | 弹出目标 StackView 当前页面 |
| `drawer_open` | 打开目标 Drawer |
| `drawer_close` | 关闭目标 Drawer |

## 控件字段

### 圆弧（`arc`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `value` | `int` | — | — | 是 | 初始值（位于 min 到 max 范围内） |
| `min` | `int` | — | 默认 0; -2147483648…2147483647 | — | 数值范围下限 |
| `max` | `int` | — | 默认 100; -2147483648…2147483647 | — | 数值范围上限 |
| `vertical` | `bool` | — | 默认 `false` | — | 垂直方向 |
| `start_angle` | `int` | — | 默认 135; 0…359 | 场景: 支持; 模板: 不支持 | 圆弧起始角度：从顶部顺时针计，0 为上、90 为右 |
| `sweep` | `int` | — | 默认 270; 1…360 | — | 顺时针圆弧跨度（度） |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `thickness` | `int` | — | 0…65535 | — | 描边粗细；省略时为 max(min(w,h)/8, 2)，0 会限制为 1 |
| `interactive` | `bool` | — | 默认 `true` | — | 允许指针交互 |

### 按钮（`button`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | 文字对齐方式 |
| `overflow` | `enum` | — | 默认 clip; `clip`, `ellipsis` | — | 单行文字溢出方式 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `font_charset` | `string` | — | — | — | 运行时绑定文字可使用的字形；静态文字会自动加入 |
| `font_charset_file` | `path` | — | — | — | 相对于场景的 UTF-8 字符集文件，与 font_charset 和静态文本合并 |
| `font_link` | `enum` | — | `embedded`, `external`, `auto` | — | 字体存储策略：内嵌/外部/自动 |
| `input` | `bool` | — | 默认 `false` | — | 文字输入字段：接入光标/键盘流程 |
| `bg_opacity` | `int` | — | 默认 255; 0…255 | — | 仅背景透明度，与 opacity 和颜色 alpha 相乘 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
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
| `checked` | `bool` | — | 默认 `false` | 是 | 初始开关状态 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `pressed_image` | `path` | — | — | — | 按下状态图片路径 |
| `selected_image` | `path` | — | — | — | 选中状态图片路径 |
| `disabled_image` | `path` | — | — | — | 禁用状态图片路径 |
| `checkable` | `bool` | — | 默认 `false` | — | 点击时切换选中状态 |

### 立体图标轮盘（`carousel`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `runtime_style` | `bool` | — | 默认 `false` | — | 生成运行时外观设置接口（按需启用） |
| `image_sets` | `string_matrix` | — | — | — | 用于运行时主题切换的额外图片组 |
| `image_set` | `int` | — | 默认 0; 0…3 | `runtime_style` | 初始编译图片组索引 |
| `icons` | `string_list` | 是 | — | — | 3..12 个图片路径，编译为原生格式缩略图 |
| `projection` | `enum` | — | 默认 perspective; `flat`, `tilt`, `perspective` | — | 轮盘投影模式 |
| `depth` | `int` | — | 默认 60; 0…100 | `runtime_style` | 轮盘立体程度，百分比 |
| `spacing` | `int` | — | 默认 50; 0…100 | `runtime_style` | 轮盘轨道间距，百分比 |
| `period_ms` | `int` | — | 默认 280; 100…60000 | `runtime_style` | 启用 runtime_style 后的轮盘吸附时长（毫秒） |
| `selected` | `int` | — | 默认 0; 0…11 | — | 初始选中的图标索引 |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 图标整体透明度 |

### 充电球（`charging_orb`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `runtime_style` | `bool` | — | 默认 `false` | — | 生成运行时外观设置接口（按需启用） |
| `fg_color` | `color` | — | 默认 #38BDF8 | `runtime_style` | 特效颜色 |
| `bg_color` | `color` | — | — | — | 可选静态背景颜色 |
| `value` | `int` | — | 默认 100; 0…100 | — | 液面高度或特效强度，百分比 |
| `period_ms` | `int` | — | 默认 2800; 100…60000 | `runtime_style` | 动画周期，毫秒 |
| `playing` | `bool` | — | 默认 `true` | — | 可见时自动推进动画 |
| `phase` | `int` | — | 默认 0; 0…65535 | — | 初始周期相位，0..65535 |
| `softness` | `int` | — | 默认 75; 0…100 | `runtime_style` | 光晕边缘柔和程度，百分比 |
| `strength` | `int` | — | 默认 65; 0…100 | `runtime_style` | 柔光强度，百分比 |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 特效整体透明度 |
| `ripple` | `int` | — | 默认 45; 0…100 | `runtime_style` | 贴附式膜状波纹强度，百分比 |
| `style` | `enum` | — | 默认 halo; `liquid`, `halo`, `plasma` | — | 充电球视觉风格 |
| `charging` | `bool` | — | 默认 `true` | — | 开启充电动态效果和粒子 |
| `particles` | `int` | — | 默认 4; 0…6 | `runtime_style` | 柔光能量粒子数量上限 |

### 图表（`chart`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `thickness` | `int` | — | 默认 0; 0…65535 | — | 描边粗细 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `points` | `int_list` | — | — | — | 单条曲线的采样点 |
| `series` | `chart_series` | — | — | — | 固定容量的曲线定义，包含名称、颜色和采样点 |
| `min` | `int` | — | 默认 0 | — | 业务数值范围下限 |
| `max` | `int` | — | 默认 100 | — | 业务数值范围上限 |
| `grid_lines` | `int` | — | 默认 0; 0…32 | — | 图表网格线数量 |

### 复选框（`checkbox`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | 默认 #50B878 | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `checked` | `bool` | — | 默认 `false` | 是 | 初始开关状态 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 时钟（`clock`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `hour_angle` | `int` | — | 默认 0; 0…359 | — | 时针角度 |
| `minute_angle` | `int` | — | 默认 0; 0…359 | — | 分针角度 |
| `second_angle` | `int` | — | 默认 0; 0…359 | — | 秒针角度 |
| `hour_color` | `color` | — | — | — | 时针颜色 |
| `minute_color` | `color` | — | — | — | 分针颜色 |
| `second_color` | `color` | — | — | — | 秒针颜色 |
| `tick_color` | `color` | — | — | — | 整点刻度颜色 |
| `hand_thickness` | `int` | — | 默认 0; 0…65535 | — | 指针基础粗细 |

### 容器（`container`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
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
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `clip_children` | `bool` | — | 默认 `false` | — | 将子内容绘制和触摸区域裁剪到容器边界 |

### 抽屉（`drawer`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
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
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `edge` | `enum` | — | 默认 top; `top`, `bottom`, `left`, `right` | — | 抽屉依附边缘 |
| `open` | `bool` | — | 默认 `false` | — | 抽屉初始状态 |

### 下拉选择（`dropdown`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | 是 | 下拉控件背景 |
| `fg_color` | `color` | — | — | — | 下拉控件文字颜色 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度 |
| `grow` | `int` | — | 默认 0; 0…100 | — | 自动布局扩展权重 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `options` | `string_list` | 是 | — | — | 选项标签 |
| `items` | `string_list` | — | — | — | 规范化后的下拉选项标签 |
| `item_height` | `int` | — | 0…65535 | — | 条目行高 |
| `open_direction` | `enum` | — | 默认 down; `down`, `up` | — | 选项面板展开方向 |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中索引 |
| `callback` | `identifier` | — | — | — | 选中状态回调 |
| `panel_color` | `color` | — | — | — | 面板填充颜色 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

### 参数特效（`effect`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `runtime_style` | `bool` | — | 默认 `false` | — | 生成运行时外观设置接口（按需启用） |
| `effect` | `enum` | 是 | `pulse`, `ring`, `liquid`, `shimmer` | — | 参数特效类型 |
| `shimmer_style` | `enum` | — | 默认 linear; `linear`, `soft`, `diagonal` | — | 扫光外观 |
| `fg_color` | `color` | — | 默认 #38BDF8 | `runtime_style` | 特效颜色 |
| `bg_color` | `color` | — | — | — | 可选静态背景颜色 |
| `value` | `int` | — | 默认 100; 0…100 | — | 液面高度或特效强度，百分比 |
| `period_ms` | `int` | — | 默认 2800; 100…60000 | `runtime_style` | 动画周期，毫秒 |
| `playing` | `bool` | — | 默认 `true` | — | 可见时自动推进动画 |
| `phase` | `int` | — | 默认 0; 0…65535 | — | 初始周期相位，0..65535 |
| `ring_style` | `enum` | — | 默认 trail; `solid`, `glow`, `trail` | — | 光环样式 |
| `softness` | `int` | — | 默认 75; 0…100 | `runtime_style` | 光晕边缘柔和程度，百分比 |
| `strength` | `int` | — | 默认 65; 0…100 | `runtime_style` | 柔光强度，百分比 |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 特效整体透明度 |

### 翻转卡片（`flip_card`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `runtime_style` | `bool` | — | 默认 `false` | — | 生成运行时外观设置接口（按需启用） |
| `image_sets` | `string_matrix` | — | — | — | 用于运行时主题切换的额外图片组 |
| `image_set` | `int` | — | 默认 0; 0…3 | `runtime_style` | 初始编译图片组索引 |
| `front` | `path` | 是 | — | — | 正面图片路径 |
| `back` | `path` | 是 | — | — | 背面图片路径 |
| `flipped` | `bool` | — | 默认 `false` | 是 | 是否显示背面 |
| `depth` | `int` | — | 默认 60; 0…100 | `runtime_style` | 透视强度百分比 |
| `period_ms` | `int` | — | 默认 560; 100…60000 | `runtime_style` | 单次翻转时长（毫秒） |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 整体透明度 |

### 毛玻璃（`glass`）

该类型不公开输入事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `backdrop` | `string` | 是 | — | — | 之前声明且具有相同父节点的静态背景图片名称 |
| `blur` | `int` | — | 默认 12; 0…32 | — | 构建期模糊半径（显示像素） |
| `tint_color` | `color` | — | 默认 #D8EAFF | — | 磨砂材质染色 |
| `tint_opacity` | `int` | — | 默认 70; 0…255 | — | 染色强度 |
| `radius` | `int` | — | 默认 16; 0…128 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 整体透明度 |

### 网格（`grid`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `cell_template` | `identifier` | 是 | — | — | 复用的网格单元模板 |
| `column_count` | `int` | 是 | 1…32 | — | 固定网格列数 |
| `column_gap` | `int` | — | 默认 0; 0…4096 | — | 单元格水平间距 |
| `row_gap` | `int` | — | 默认 0; 0…4096 | — | 单元格垂直间距 |
| `scroll_snapshot` | `bool` | — | 默认 `false` | — | 滚动时缓存两个视口帧；内存不足时回退到实时渲染 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `item_count` | `int` | — | 默认 0; 0…4294967295 | — | 网格条目初始数量 |

### 图片（`image`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: SVG; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: SVG; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
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
| `rotation` | `int` | — | 默认 0; -32768…32767 | 场景: 图片; 模板: 图片 | 图片绕边界框中心顺时针旋转，支持源图透明度；超出边界框的部分会被裁剪 |
| `scalable` | `bool` | — | 默认 `false` | — | 启用运行时图片缩放 |
| `scale` | `number` | — | 默认 1.0; 0.0625…16.0 | — | 运行时图片初始缩放比例 |
| `min_scale` | `number` | — | 默认 0.5; 0.0625…16.0 | — | 运行时图片最小缩放比例 |
| `max_scale` | `number` | — | 默认 4.0; 0.0625…16.0 | — | 运行时图片最大缩放比例 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
| `text` | `string` | — | — | 是 | 静态文字内容（UTF-8） |
| `text_align` | `enum` | — | `left`, `center`, `right` | — | 文字对齐方式 |
| `overflow` | `enum` | — | 默认 clip; `clip`, `ellipsis` | — | 单行文字溢出方式 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `font_charset` | `string` | — | — | — | 运行时绑定文字可使用的字形；静态文字会自动加入 |
| `font_charset_file` | `path` | — | — | — | 相对于场景的 UTF-8 字符集文件，与 font_charset 和静态文本合并 |
| `font_link` | `enum` | — | `embedded`, `external`, `auto` | — | 字体存储策略：内嵌/外部/自动 |
| `input` | `bool` | — | 默认 `false` | — | 文字输入字段：接入光标/键盘流程 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `image_opacity` | `int` | — | 默认 255; 0…255 | — | 仅图片透明度；固定栅格图片的变换可在编译期烘焙 alpha |

### 键盘（`keyboard`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | 是 | 键盘背景 |
| `fg_color` | `color` | — | — | — | 键盘文字颜色 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏 |
| `key_color` | `color` | — | — | — | 键盘字符键填充颜色 |
| `function_color` | `color` | — | — | — | 键盘修饰键填充颜色 |
| `function_text_color` | `color` | — | — | — | 键盘修饰键文字颜色 |
| `delete_color` | `color` | — | — | — | 键盘删除键填充颜色 |
| `delete_text_color` | `color` | — | — | — | 键盘删除键文字颜色 |
| `ok_color` | `color` | — | — | — | 键盘确认键填充颜色 |
| `ok_text_color` | `color` | — | — | — | 键盘确认键文字颜色 |
| `space_color` | `color` | — | — | — | 键盘空格键填充颜色 |
| `space_text_color` | `color` | — | — | — | 键盘空格键文字颜色 |
| `key_radius` | `int` | — | 0…65535 | — | 键盘按键圆角半径（像素） |
| `shift_label` | `string` | — | — | — | 键盘 Shift 键标签 |
| `delete_label` | `string` | — | — | — | 键盘删除键标签 |
| `ok_label` | `string` | — | — | — | 键盘确认键标签 |
| `symbols_label` | `string` | — | — | — | 键盘符号页切换键标签 |
| `letters_label` | `string` | — | — | — | 键盘字母页切换键标签 |
| `space_label` | `string` | — | — | — | 键盘空格键标签 |
| `function_font_size` | `int` | — | 1…255 | — | 键盘文字修饰键字号 |
| `shift_icon` | `path` | — | — | — | 键盘 Shift 键图标图片 |
| `delete_icon` | `path` | — | — | — | 键盘删除键图标图片 |
| `ok_icon` | `path` | — | — | — | 键盘确认键图标图片 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

### 标签（`label`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | 默认 #FFFFFF | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
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
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 图层（`layer`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
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
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `clip_children` | `bool` | — | 默认 `false` | — | 将子内容绘制和触摸区域裁剪到图层边界 |
| `block_scene_swipe` | `bool` | — | 默认 `false` | — | 可见时阻止场景水平滑动 |

### 列表（`list`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `items` | `string_list` | — | — | — | 条目文字（list/wheel/dropdown/tabview） |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中条目索引 |
| `item_height` | `int` | — | 默认 0; 0…65535 | — | 列表/滚轮行高 |
| `items_per_page` | `int` | — | 默认 0; 0…65535 | — | TabView 每页条目数 |
| `visible_rows` | `int` | — | 默认 0; 0…65535 | — | 可见行数（item_height 的替代配置） |
| `cyclic` | `bool` | — | 默认 `false` | — | 滚轮首尾连续循环 |
| `snap_to_item` | `bool` | — | 默认 `false` | — | 滚动结束时吸附到行边界 |
| `row_template` | `identifier` | — | — | — | 复用的行模板 |
| `item_count` | `int` | — | 0…4294967295 | — | 动态条目初始数量 |
| `scroll_snapshot` | `bool` | — | 默认 `false` | — | 滚动时缓存两个视口帧；内存不足时回退到实时渲染 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 消息列表（`message_list`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `row_template` | `identifier` | — | — | — | 复用的行模板 |
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
| `callback` | `identifier` | — | — | — | 应用回调名称 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `font_charset` | `string` | — | — | — | 运行时绑定消息文字可使用的字形 |
| `scroll_snapshot` | `bool` | — | 默认 `true` | — | 滚动时缓存两个视口帧；内存不足时回退到实时渲染 |
| `item_height` | `int` | — | 默认 0; 0…65535 | — | 消息最小行高；0 或省略时为 2 × bubble_padding_y + message_gap + 1 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

### 消息框（`msgbox`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `title` | `string` | — | — | — | 对话框标题 |
| `text` | `string` | — | — | 是 | 对话框正文 |
| `buttons` | `string_list` | 是 | — | — | 按钮标签 |
| `callback` | `identifier` | — | — | — | 按钮按下回调 |
| `bg_color` | `color` | — | — | 是 | 对话框背景 |
| `fg_color` | `color` | — | — | — | 对话框文字颜色 |
| `button_color` | `color` | — | — | — | 按钮填充颜色 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `hidden` | `bool` | — | 默认 `true` | 是 | 初始隐藏 |
| `dismissable` | `bool` | — | 默认 `false` | — | 点击遮罩时关闭 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

### 指针（`needle`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `angle` | `int` | — | 默认 0; 0…359 | — | 顺时针角度（0 表示向上） |
| `tail_length` | `int` | — | 默认 0; 0…65535 | — | 中心枢轴后方长度（像素） |
| `thickness` | `int` | — | 默认 0; 0…65535 | — | 描边粗细 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 页面流（`page_flow`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
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
| `items` | `string_list` | — | — | — | 条目文字（list/wheel/dropdown/tabview） |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中条目索引 |
| `item_height` | `int` | — | 默认 0; 0…65535 | — | 列表/滚轮行高 |
| `items_per_page` | `int` | — | 默认 0; 0…65535 | — | TabView 每页条目数 |
| `visible_rows` | `int` | — | 默认 0; 0…65535 | — | 可见行数（item_height 的替代配置） |
| `cyclic` | `bool` | — | 默认 `false` | — | 滚轮首尾连续循环 |
| `snap_to_item` | `bool` | — | 默认 `false` | — | 滚动结束时吸附到行边界 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `pages` | `string_list` | — | — | — | 按页面索引排列的直接子对象名称；优先于旧版 &lt;name&gt;_tabN 命名规则 |
| `page_count` | `int` | — | 1…65535 | — | 编译后的页面数量 |
| `axis` | `enum` | — | 默认 horizontal; `horizontal`, `vertical` | — | 页面运动轴向 |
| `bar_height` | `int` | — | 默认 56; 0…4096 | — | 选项卡栏高度 |
| `stop_anywhere` | `bool` | — | 默认 `false` | — | 允许 PageFlow 在页面之间回弹定位 |
| `page_extent` | `int` | — | 默认 0; 0…65535 | — | PageFlow 拖动范围 |

### 进度条（`progress`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `value` | `int` | — | — | 是 | 初始值（位于 min 到 max 范围内） |
| `min` | `int` | — | 默认 0; -2147483648…2147483647 | — | 数值范围下限 |
| `max` | `int` | — | 默认 100; -2147483648…2147483647 | — | 数值范围上限 |
| `vertical` | `bool` | — | 默认 `false` | — | 垂直方向 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 单选框（`radio`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | 默认 #55A0E8 | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `checked` | `bool` | — | 默认 `false` | 是 | 初始开关状态 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 矩形（`rect`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
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
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 形状（`shape`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `rotation` | `int` | — | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 图片绕边界框中心顺时针旋转，支持源图透明度；超出边界框的部分会被裁剪 |
| `scalable` | `bool` | — | 默认 `false` | — | 启用运行时图片缩放 |
| `scale` | `number` | — | 默认 1.0; 0.0625…16.0 | — | 运行时图片初始缩放比例 |
| `min_scale` | `number` | — | 默认 0.5; 0.0625…16.0 | — | 运行时图片最小缩放比例 |
| `max_scale` | `number` | — | 默认 4.0; 0.0625…16.0 | — | 运行时图片最大缩放比例 |
| `shape` | `enum` | — | 默认 rect; `rect`, `round_rect`, `circle`, `ellipse`, `line` | — | 形状几何类型 |
| `line_direction` | `enum` | — | 默认 tl_br; `horizontal`, `vertical`, `tl_br`, `bl_tr` | — | 线条方向 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 滑块（`slider`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `value` | `int` | — | — | 是 | 初始值（位于 min 到 max 范围内） |
| `min` | `int` | — | 默认 0; -2147483648…2147483647 | — | 数值范围下限 |
| `max` | `int` | — | 默认 100; -2147483648…2147483647 | — | 数值范围上限 |
| `vertical` | `bool` | — | 默认 `false` | — | 垂直方向 |
| `knob_color` | `color` | — | 默认 #FFFFFF | — | 滑块/开关旋钮颜色 |
| `track_size` | `int` | — | 默认 0; 0…4096 | — | 轨道粗细 |
| `knob` | `bool` | — | 默认 `true` | — | 是否显示旋钮 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 加载指示器（`spinner`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `speed` | `int` | — | 默认 1000; 100…60000 | — | 旋转一周所需时间（毫秒） |
| `thickness` | `int` | — | 默认 0; 0…65535 | — | 描边粗细 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 堆栈视图（`stackview`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 自身填充; 模板: 自身填充 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 自身填充; 模板: 自身填充 | 圆角半径（像素） |
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
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
| `page_count` | `int` | — | 1…65535 | — | 编译后的页面数量 |
| `initial_page` | `int` | — | 默认 0; 0…65534 | — | 堆栈初始根页面 |
| `capacity` | `int` | — | 默认 8; 1…8 | — | 最大堆栈深度 |
| `axis` | `enum` | — | 默认 horizontal; `horizontal`, `vertical` | — | 运动轴向 |
| `transition_ms` | `int` | — | 默认 0; 0…65535 | — | 程序调用 push/pop 的时长；0 表示沿用运行时默认值 |
| `transition_easing` | `enum` | — | 默认 linear; `linear`, `ease_out`, `ease_in_out` | — | 程序调用 push/pop 的缓动曲线 |

### 表格（`table`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | 是 | 表格背景 |
| `fg_color` | `color` | — | — | — | 表格文字颜色 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `columns` | `string_list` | 是 | — | — | 列标签 |
| `rows` | `string_matrix` | 是 | — | — | 单元格行数据 |
| `col_widths` | `number_list` | — | — | — | 各列相对宽度 |
| `row_height` | `int` | — | 默认 44; 1…65535 | — | 行高 |
| `header_color` | `color` | — | — | — | 表头填充颜色 |
| `grid_color` | `color` | — | — | — | 网格线颜色 |

### 选项卡视图（`tabview`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | 是 | TabView 背景 |
| `fg_color` | `color` | — | — | — | 选项卡标签颜色 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径 |
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏 |
| `tabs` | `string_list` | 是 | — | — | 选项卡标签 |
| `bar_height` | `int` | — | 默认 56; 1…65535 | — | 选项卡栏高度 |
| `active_color` | `color` | — | — | — | 激活指示器颜色 |
| `axis` | `enum` | — | 默认 horizontal; `horizontal`, `vertical` | — | 页面轴向 |
| `cyclic` | `bool` | — | 默认 `true` | — | 页面循环切换 |
| `stop_anywhere` | `bool` | — | 默认 `false` | — | 允许 PageFlow 在页面之间回弹定位 |
| `page_extent` | `int` | — | 默认 0; 0…65535 | — | PageFlow 拖动范围 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中的选项卡 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

### 开关（`toggle`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | 默认 #22c55e | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | 默认 #3f3f46 | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `checked` | `bool` | — | 默认 `false` | 是 | 初始开关状态 |
| `knob_color` | `color` | — | 默认 #FFFFFF | — | 滑块/开关旋钮颜色 |
| `track_size` | `int` | — | 默认 0; 0…4096 | — | 轨道粗细 |
| `knob` | `bool` | — | 默认 `true` | — | 是否显示旋钮 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |

### 滚轮选择器（`wheel`）

支持 `click`、`press`、`release`、`long` 和适用控件的 `value` 事件。

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
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
| `fg_color` | `color` | — | — | — | 前景颜色（根据控件类型用于文字、旋钮、线条或标记） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 0–255 混合透明度 |
| `bg_color` | `color` | — | — | 是 | 背景/填充颜色（#RRGGBB 或 #RRGGBBAA） |
| `bg_gradient` | `color` | — | — | — | 第二个渐变色标（与 bg_color 配合） |
| `gradient_dir` | `enum` | — | 默认 vertical; `vertical`, `horizontal` | — | 渐变方向 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |
| `shadow_color` | `color` | — | — | — | 静态硬阴影颜色 |
| `shadow_opacity` | `int` | — | 默认 96; 0…255 | — | 静态硬阴影透明度 |
| `shadow_offset_x` | `int` | — | -32768…32767 | — | 静态硬阴影 x 偏移 |
| `shadow_offset_y` | `int` | — | -32768…32767 | — | 静态硬阴影 y 偏移 |
| `shadow_spread` | `int` | — | 0…4096 | — | 静态硬阴影扩散范围（像素） |
| `shadow_radius` | `int` | — | 0…65535 | — | 静态硬阴影圆角半径 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度（需要 border_color） |
| `border_opacity` | `int` | — | 默认 255; 0…255 | — | 边框描边透明度 |
| `outline_color` | `color` | — | — | — | 外部轮廓颜色 |
| `outline_width` | `int` | — | 默认 0; 0…65535 | — | 外部轮廓宽度 |
| `outline_opacity` | `int` | — | 默认 255; 0…255 | — | 外部轮廓透明度 |
| `outline_pad` | `int` | — | 默认 0; 0…4096 | — | 元素与外部轮廓之间的间隔 |
| `text_line_space` | `int` | — | 默认 0; 0…4096 | — | 静态文字行之间的额外间距，单位像素 |
| `text_vertical_align` | `enum` | — | 默认 auto; `auto`, `top`, `center`, `bottom` | — | 静态文字块的垂直对齐；auto 保留单行居中、多行顶部对齐 |
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
| `snap_to_item` | `bool` | — | 默认 `true` | — | 滚动结束时吸附到行边界 |
| `dynamic_items` | `bool` | — | 默认 `false` | — | 预留条目更新 API，不随初始条目数量变化 |
| `cyclic` | `bool` | — | 默认 `false` | — | 选择项支持首尾连续循环 |
| `items` | `string_list` | — | — | — | 条目文字（list/wheel/dropdown/tabview） |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中条目索引 |
| `item_height` | `int` | — | 默认 0; 0…65535 | — | 列表/滚轮行高 |
| `items_per_page` | `int` | — | 默认 0; 0…65535 | — | TabView 每页条目数 |
| `visible_rows` | `int` | — | 默认 0; 0…65535 | — | 可见行数（item_height 的替代配置） |
| `row_template` | `identifier` | — | — | — | 复用的行模板 |
| `item_count` | `int` | — | 0…4294967295 | — | 动态条目初始数量 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |
| `bind` | `identifier` | — | — | — | 公开状态名称；生成 GSP_BIND_&lt;NAME&gt; |
| `bind_target` | `enum` | — | `visible`, `value`, `color`, `text`, `resource`, `data` | — | 显式绑定状态类型 |
| `callback` | `identifier` | — | — | — | 应用回调名称；生成按场景区分的事件辅助函数 |
| `events` | `action_list` | — | — | — | 输入绑定：[{event, action, ...}] |
| `template` | `identifier` | — | — | — | 将此子树声明为渲染模板 |
| `max_instances` | `int` | — | 1…65535 | — | 同时存活模板实例的最大数量；计入自动推导的资源池需求 |
| `dynamic_color` | `bool` | — | — | — | 模板成员公开每实例独立颜色槽 |
| `dynamic_image` | `bool` | — | — | — | 模板图片公开每实例独立资源槽 |
