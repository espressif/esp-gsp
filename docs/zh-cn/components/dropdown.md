# 下拉选择（`gsp_dropdown`）

展开选项面板的紧凑型单选控件。

## 适用场景

适合直接使用一个现成控件来提供常见交互。

## 本地交互预览

先按[兼容性页的工具命令](../reference/compatibility.md#工具命令)配置 `gspc` 和 `gsp_sim_host`，再在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/dropdown/dropdown.json \
  --deployable -o gsp-out/widget-preview/dropdown.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/dropdown.gspb --frames 0
```

这些命令会编译当前示例，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

GSPC 会将组合组件编译为原生图元。应用代码需要控制某个对象时，为它设置稳定的 `name`。

`selected` 是零基初始索引，缺省为 0。生成的 `set_selected` 更新当前场景中控件的运行时索引和显示文字，不修改 JSON。切到对应场景后再恢复应用保存的选择。点击回调的新选择读 `event->arg`；回调内 getter 可能仍读到本次输入提交前的值。

如果应用代码需要读写某个对象，请为它设置稳定的 `name`。GSPC 会为命名对象生成下方列出的类型化函数。

## 运行时选项文字

切换语言时，可显式设置 `dynamic_options: true`。
`option_text_max_bytes` 缺省为每项 128 个 UTF-8 字节，不含 NUL，范围为 1–65535。
该编写能力需要 GSPC 0.10.0 和 ESP-GSP 1.9.0 及以上版本。

```json
{"type":"dropdown", "name":"sleep", "x":20, "y":20, "w":180, "h":32,
 "options":["10 sec", "Never"], "dynamic_options":true, "option_text_max_bytes":32}
```

先配置覆盖译文的字体，并在场景根节点用 `font_charset`（如 `"10 秒永不"`）或
`font_charset_file` 纳入译文字符，也可配置运行时字体提供器。
回读正确并不证明字库覆盖完整。进入对应场景后调用生成接口（以下假设场景名为 `settings`）：

```c
const char *const zh[] = {"10 秒", "永不"};
ESP_ERROR_CHECK(gsp_settings_sleep_set_options_text(ui, zh, 2));
```

每次替换整组选项文字，数量和顺序固定。选中索引、回调索引和应用业务值保持原有语义。
SDK 在返回前复制字符串，准备好所有选项行和收起标签的字形后再整体发布。
UTF-8 非法、文字超长、分配失败或队列请求所属的场景访问已过期时，保留原文案。
`ESP_OK` 表示已接受请求；异步应用或丢弃错误通过 `esp_gsp_update_error_stats()`
读取，`component` 指向下拉控件，`property=0` 表示整组选项文字。

启用后还会生成 `get_option_count` 和 `get_option_text`。后者支持传入
`NULL, 0, &required` 查询包含 NUL 的所需字节数；缓冲区不足时返回
`ESP_ERR_INVALID_SIZE`，不截断、不改写缓冲区。回读反映已提交文字，需在 UI
任务中调用，或由调用者与绘制、销毁操作串行化。文案在应用实例存活期间跨场景保留；
持久化策略由应用负责。

选项行尺寸固定，沿用普通文本的裁剪和省略规则。
每个启用的选项行会新增一个文字绑定，纳入编译器的槽位需求检查。替换后运行时保留
UTF-8 副本及每项的展开、收起两份字形；准备新内容时新旧两组会短暂共存。
静态控件不分配选项文字控制器，字节预算应按实际译文设置。
未启用该字段时，生成配置及选择行为保持静态，替换接口返回 `ESP_ERR_NOT_SUPPORTED`。

资源需使用配套编译器和 SDK 重新生成。动态元数据要求 GMD 1.4，旧运行时会拒绝其
必需段。PC C Backend 使用同一组生成接口；模拟器客户端可通过
`dropdown_get_option_count` / `dropdown_get_option_text` 回读已提交文案。

## 场景示例

```json
{"screen":"widget_dropdown","w":480,"h":320,"screen_bg":"#101827","font":"../../../common/fonts/DejaVuSans.ttf","font_charset":" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~","objects":[{"type":"dropdown","parent":-1,"name":"metric","x":70,"y":55,"w":340,"h":48,"options":["Frame throughput","Memory budget","Media decode","Input latency"],"selected":1,"open_direction":"down","font_size":16,"bg_color":"#172235","panel_color":"#1D2D44","fg_color":"#E2E8F0","border_color":"#334A68","border_width":1,"radius":10},{"type":"label","parent":-1,"x":70,"y":130,"w":340,"h":60,"text":"Tap the field, then choose an option from the panel.","font_size":16,"fg_color":"#94A3B8"}]}
```

规范示例来自 [`dropdown.json`](../../../examples/usage/widgets/dropdown/dropdown.json)。复制时请一并复制它引用的相对资源。

## 此场景生成的 C API

```c
const gsp_component_directory_t *const * gsp_dropdown_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_widget_dropdown___metric_label_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown___metric_panel_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown_metric_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_widget_dropdown_metric_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_widget_dropdown_metric_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_widget_dropdown_metric_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_gsp_config_t gsp_dropdown_docs_config(void)
size_t gsp_dropdown_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 此示例使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `bg_color` | `color` | — | — | 是 | 下拉控件背景 |
| `fg_color` | `color` | — | — | — | 下拉控件文字颜色 |
| `border_color` | `color` | — | — | — | 边框描边颜色 |
| `border_width` | `int` | — | 0…65535 | — | 边框描边宽度 |
| `radius` | `int` | — | 默认 0; 0…65535 | 场景: 不支持; 模板: 不支持 | 圆角半径 |
| `font_size` | `int` | — | 1…255 | — | 单个对象的字体像素尺寸 |
| `options` | `string_list` | 是 | — | — | 选项标签 |
| `open_direction` | `enum` | — | 默认 down; `down`, `up` | — | 选项面板展开方向 |
| `selected` | `int` | — | 默认 0; 0…65535 | 是 | 初始选中索引 |
| `panel_color` | `color` | — | — | — | 面板填充颜色 |

<details>
<summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `grow` | `int` | — | 默认 0; 0…100 | — | 自动布局扩展权重 |
| `font` | `path` | — | — | — | 单个对象的 TTF/OTF 字体覆盖路径 |
| `dynamic_options` | `bool` | — | 默认 `false` | — | 允许在运行时整体替换选项文字 |
| `option_text_max_bytes` | `int` | — | 默认 128; 1…65535 | — | 每项动态文字的 UTF-8 字节上限，不含 NUL |
| `items` | `string_list` | — | — | — | 规范化后的下拉选项标签 |
| `item_height` | `int` | — | 0…65535 | — | 条目行高 |
| `callback` | `identifier` | — | — | — | 选中状态回调 |
| `enabled` | `bool` | — | — | — | 初始交互状态；设置后会公开可由后代控件继承的运行时 enabled 属性 |
| `disabled_color` | `color` | — | 默认 #808080 | — | 禁用态覆盖颜色 |
| `disabled_opacity` | `int` | — | 默认 112; 0…255 | — | 禁用态覆盖透明度 |

</details>

<details>
<summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
