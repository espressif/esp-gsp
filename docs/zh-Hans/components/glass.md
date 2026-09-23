# 毛玻璃（`gsp_glass`）

构建期生成静态背景的磨砂裁剪，运行时使用原生图片绘制。

## 适用场景

适合传达状态、测量结果、运动过程或视觉层级。

## 本地交互预览

先按[兼容性页的工具命令](../reference/compatibility.md#工具命令)配置 `gspc` 和
`gsp_sim_host`，再在解压后的组件或公共仓库根目录运行：

```sh
mkdir -p gsp-out/widget-preview
gspc pack \
  examples/usage/widgets/glass/glass.json \
  --deployable -o gsp-out/widget-preview/glass.gspb
gsp_sim_host \
  --bundle gsp-out/widget-preview/glass.gspb
```

这些命令编译下方 JSON，并在 ESP-GSP 模拟器中打开浏览器预览。

## 运行方式

模糊、染色和圆角均在构建期生成，运行时使用原生图片绘制。`backdrop` 必须引用先声明、同一父节点下的静态不透明 Image；玻璃必须位于背景内。它不会模糊其他控件，也不会随运行时背景替换或玻璃单独移动重新取样。主题切换可预配置多种材质并切换可见性；不支持模板成员或动态几何表达式。

为需要在 C 代码中读写的对象设置稳定的 `name`。GSPC 为命名对象生成下方列出的类型化函数。

## 完整示例 JSON

```json
{
  "screen": "glass_demo",
  "w": 320,
  "h": 240,
  "font": "../../../common/fonts/DejaVuSans.ttf",
  "objects": [
    {
      "type": "image",
      "parent": -1,
      "name": "wall",
      "x": 0,
      "y": 0,
      "w": 320,
      "h": 240,
      "image": "../../effects/assets/backdrop.svg",
      "fit": "stretch"
    },
    {
      "type": "glass",
      "parent": -1,
      "name": "panel",
      "backdrop": "wall",
      "x": 40,
      "y": 45,
      "w": 240,
      "h": 150,
      "blur": 14,
      "tint_color": "#D8EAFF",
      "tint_opacity": 65,
      "radius": 18
    },
    {
      "type": "label",
      "parent": -1,
      "x": 65,
      "y": 70,
      "w": 190,
      "h": 24,
      "text": "LIVING ROOM",
      "font_size": 14,
      "fg_color": "#FFFFFF",
      "text_align": "center"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 65,
      "y": 103,
      "w": 190,
      "h": 45,
      "text": "22.6 C",
      "font_size": 34,
      "fg_color": "#FFFFFF",
      "text_align": "center"
    },
    {
      "type": "label",
      "parent": -1,
      "x": 65,
      "y": 157,
      "w": 190,
      "h": 20,
      "text": "Comfortable",
      "font_size": 12,
      "fg_color": "#E9F5FF",
      "text_align": "center"
    }
  ]
}
```

该文件来自 `examples/usage/widgets/glass/glass.json`。复制时请一并复制它引用的相对资源。

## 此示例生成的 C API

```c
const gsp_component_directory_t *const * gsp_glass_docs_component_directories(uint16_t *out_count)
esp_err_t gsp_glass_demo_panel_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_glass_demo_panel_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_err_t gsp_glass_demo_wall_get_effective_visible(esp_gsp_handle_t gsp, bool *out_visible)
esp_err_t gsp_glass_demo_wall_get_info(esp_gsp_handle_t gsp, esp_gsp_component_info_t *out_info)
esp_gsp_config_t gsp_glass_docs_config(void)
size_t gsp_glass_docs_dynamic_image_slots(void)
```

以上签名来自该 JSON 的实际编译产物。

## 示例中使用的字段

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `type` | `string` | 是 | — | — | 控件类型 |
| `parent` | `int` | 是 | 默认 -1; -1…65534 | — | 父对象索引（-1 表示屏幕根节点） |
| `x` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 x 坐标 |
| `y` | `int` | 是 | 默认 0; -32768…32767 | 场景: 不支持; 模板: 不支持 | 相对于父对象的 y 坐标 |
| `w` | `int` | 是 | 1…512 | 场景: 不支持; 模板: 不支持 | 宽度（像素） |
| `h` | `int` | 是 | 1…512 | 场景: 不支持; 模板: 不支持 | 高度（像素） |
| `name` | `identifier` | — | — | — | 稳定组件名称；生成 GSP_OBJ_KEY_&lt;NAME&gt; |
| `backdrop` | `string` | 是 | — | — | 之前声明且具有相同父节点的静态背景图片名称 |
| `blur` | `int` | — | 默认 12; 0…32 | — | 构建期模糊半径（显示像素） |
| `tint_color` | `color` | — | 默认 #D8EAFF | — | 磨砂材质染色 |
| `tint_opacity` | `int` | — | 默认 70; 0…255 | — | 染色强度 |
| `radius` | `int` | — | 默认 16; 0…128 | 场景: 不支持; 模板: 不支持 | 圆角半径（像素） |

<details><summary>查看此控件支持的其他字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `hidden` | `bool` | — | 默认 `false` | 是 | 初始隐藏（通过动作或 set_visible 显示） |
| `opacity` | `int` | — | 默认 255; 0…255 | 场景: 不支持; 模板: 不支持 | 整体透明度 |

</details>

<details><summary>查看所有组件共享字段</summary>

| 字段 | 类型 | 必填 | 默认值 / 范围 | 可运行时更新 | 编译器定义 |
|---|---|---:|---|---:|---|
| `parent_name` | `string` | — | — | — | 使用名称而不是索引指定父对象 |

</details>
