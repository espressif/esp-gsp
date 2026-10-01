# 可复用项目组件模式

这个 480×320 场景通过 `--components-dir` 加载五个可编辑项目组件：三个设置项行
变体、一个图标按钮和一个角标。它们展开为普通 Container、Label、Button、Image
和 Toggle 对象，不会新增运行时控件类型。

使用与此组件配套的 GSPC，在组件根目录运行。


```sh
python -m pip install -U esp-gsp-tools
gspc diagnose \
  examples/usage/component_patterns/settings.json \
  --components-dir examples/usage/component_patterns/components
mkdir -p gsp-out/component-patterns
gspc pack \
  examples/usage/component_patterns/settings.json \
  --components-dir examples/usage/component_patterns/components \
  --api-header gsp-out/component-patterns/settings.h \
  --deployable -o gsp-out/component-patterns/settings.gspb
python -m gsp.execute --version 1.6.0 sim \
  --bundle gsp-out/component-patterns/settings.gspb \
  --api-json gsp-out/component-patterns/settings.api.json
```

`params` 数组中的元素是必填参数名。这些定义使用完整的 `$param` 字符串替换来保留
实参的 JSON 类型。GSPC 也支持[带前缀文本替换](../../../docs/zh-cn/guide/scenes.md#可复用项目组件)。
参数替换不计算表达式，也不会补齐缺少的实参。根对象的 `name` 在每个 `use` 使用处
提供。行内部使用局部数字父索引以缩短使用处参数；只有应用可能读取或更新的尾部数值
和开关才使用显式的唯一名称参数。

复制到应用时要保持场景的相对素材目录，或相应修改 `settings.json` 中的两个路径：

```sh
mkdir -p my_app/ui/components my_app/ui/scenes my_app/common/fonts my_app/ui/widgets/image
cp examples/usage/component_patterns/settings.json \
  my_app/ui/scenes/
cp examples/usage/component_patterns/components/*.json \
  my_app/ui/components/
cp examples/common/fonts/DejaVuSans.ttf my_app/common/fonts/
cp examples/usage/widgets/image/vector_icon.svg \
  my_app/ui/widgets/image/
```

使用 `--components-dir my_app/ui/components` 编译 `my_app/ui/scenes/settings.json`；上述目录
结构会让场景中的相对字体和图标路径解析到复制后的文件。

角标接收应用已经格式化好的短文本。数值范围或文本长度限制由应用负责；JSON 组件只
负责布局并在边界处裁剪文本。这里的布局在编译期确定，运行时改变父对象宽度不会重新
排布兄弟设置项。

数值行和导航行使用按钮的原生按压反馈。开关行只有开关自身可交互，点击标题不会
改变开关或触发其回调。示例会发出 `settings_info`、`settings_network`、
`settings_appearance` 和 `settings_notifications`；导航和持久化由应用处理。
在回调中读取开关的 checked 属性，不要把回调参数直接当作开关状态。

生成的头文件提供 `network_value`、`status_badge` 的文字设置接口和
`notifications_toggle` 的 checked 读写接口。Badge 必须传入 `name`，该名称也作为
文字绑定名。数量上限由应用格式化，例如 `99+`。动态文字使用声明的预编译
`font_charset`；其他语言或符号需要相应修改字符集和字体。

图标按钮通过 `icon_size` 和 `gap` 将图标排在文字前方。按钮宽度应容纳内边距、
图标、间距和文字；其他排列方式可修改组件中的行顺序。仅当多个实例确实共享动作时，
才让它们使用相同回调。

这些模式复用已有渲染操作，但展开的控件及可变文字仍占用状态、文字槽位、字体和
Bundle 容量。不需要运行时更新的静态标签可在复制的定义中去掉绑定。
没有新增设备端布局引擎或全局组件缓存。
