# 立体图标轮盘（`gsp_carousel`）

使用编译期缩略图、前后排序和吸附选择的图标轮盘。

## 适用场景

适合高效浏览、选择或展示重复的应用数据。

## 本地交互预览

先按[兼容性页的工具命令](../reference/compatibility.md#工具命令)配置 `gspc` 和
`gsp_sim_host`，再在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/carousel/carousel.json \
  --deployable -o gsp-out/widget-preview/carousel.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/carousel.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

行或单元格由场景数据渲染，并可连接滚动、选择或应用数据适配器。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "carousel_demo",
  "w": 320,
  "h": 240,
  "screen_bg": "#0B1424",
  "font": "../../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "label",
      "parent": -1,
      "x": 16,
      "y": 10,
      "w": 288,
      "h": 24,
      "text": "GSP  /  ORBIT",
      "font_size": 18,
      "fg_color": "#E8F4FF"
    },
    {
      "type": "carousel",
      "parent": -1,
      "name": "menu",
      "x": 16,
      "y": 42,
      "w": 288,
      "h": 168,
      "icons": [
        "home.svg",
        "music.svg",
        "water.svg",
        "sun.svg",
        "power.svg"
      ],
      "selected": 0
    },
    {
      "type": "label",
      "parent": -1,
      "x": 16,
      "y": 214,
      "w": 288,
      "h": 20,
      "text": "Drag to rotate  /  tap to select",
      "font_size": 14,
      "fg_color": "#9FB5CB",
      "text_align": "center"
    }
  ]
}
```

该文件来自 `examples/usage/widgets/carousel/carousel.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_carousel_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_carousel_demo_menu_animate_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_carousel_demo_menu_animate_selected_to(esp_gsp_handle_t gsp, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease)
esp_err_t gsp_carousel_demo_menu_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_carousel_demo_menu_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_carousel_demo_menu_get_selected(esp_gsp_handle_t gsp, uint32_t *out_value)
esp_err_t gsp_carousel_demo_menu_get_value(esp_gsp_handle_t gsp, int32_t *out_value)
esp_err_t gsp_carousel_demo_menu_play_selected(esp_gsp_handle_t gsp, uint32_t from, uint32_t to, const esp_gsp_animation_config_t *config)
esp_err_t gsp_carousel_demo_menu_set_selected(esp_gsp_handle_t gsp, uint32_t new_value)
esp_err_t gsp_carousel_demo_menu_set_value(esp_gsp_handle_t gsp, int32_t value)
esp_err_t gsp_carousel_demo_menu_stop_selected(esp_gsp_handle_t gsp)
esp_gsp_config_t gsp_carousel_docs_config(void)
size_t gsp_carousel_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 0…65535 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `icons` | `string_list` | 是 | — | — | 3..12 个图片路径，编译为原生格式缩略图 |
| `selected` | `int` | — | 默认 0; 0…11 | — | 初始选中的图标索引 |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
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
| `projection` | `enum` | — | 默认 perspective; `flat`, `tilt`, `perspective` | — | 轮盘投影模式 |
| `depth` | `int` | — | 默认 60; 0…100 | `runtime_style` | 轮盘立体程度，百分比 |
| `spacing` | `int` | — | 默认 50; 0…100 | `runtime_style` | 轮盘轨道间距，百分比 |
| `period_ms` | `int` | — | 默认 280; 100…60000 | `runtime_style` | 启用 runtime_style 后的轮盘吸附时长（毫秒） |
| `opacity` | `int` | — | 默认 255; 0…255 | `runtime_style` | 图标整体透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
