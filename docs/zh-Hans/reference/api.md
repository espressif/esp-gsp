# 公共 API 参考

普通应用首先包含生成的 `<symbol>_gsp.h`。它是当前 Bundle 的准确接口：场景 ID、事件判断、模板描述和命名控件类型化函数都由 JSON 推导。

## 推荐接口层

| 层级 | 头文件 | 用途 |
|---|---|---|
| 生成 Bundle | `<symbol>_gsp.h` | 启动配置和普通类型化应用接口 |
| 应用 Facade | `esp_gsp.h` | 动态媒体、列表、导航、定时器和数据驱动集成 |
| ESP-LCD 生命周期 | `esp_gsp_esp_lcd.h` | 启动、暂停、临时应用交接、恢复和会话销毁 |
| 诊断 | `esp_gsp_debug.h` | 计数、时序、媒体遥测和测试输入注入 |
| 框架集成 | `esp_gsp_advanced.h` | 访问底层渲染上下文；不是普通应用路径 |

## 应用 Facade 分组

`esp_gsp.h` 中的声明与所有权注释是最终依据。下面用于快速定位功能，不重复维护
全部函数原型。每个应用层公共函数的准确声明、返回类型、所属头文件和使用契约见
[公共函数参考](api-functions.md)。

| 分组 | 主要入口 |
|---|---|
| 同步与错误 | `esp_gsp_flush()`、`esp_gsp_render_error_stats()` |
| 通用状态 | `esp_gsp_set_*()`、`esp_gsp_get_*()` |
| 组件 | `esp_gsp_component_get_*()`、`esp_gsp_component_set_*()`、批处理、位置和动画 API |
| 导航 | `esp_gsp_goto_scene()`、滑动策略、PageFlow、StackView、Drawer API |
| 事件与输入 | `esp_gsp_on_event()`、`esp_gsp_on_pinch()`、`esp_gsp_set_pointer_observer()` |
| 运行时图片 | `esp_gsp_set_image*()` 的 COPY、BORROW、TAKE 变体 |
| List 与 Wheel | 绑定、刷新、吸附、Fling、滚动、渐隐和行发布 API |
| Canvas | 全帧/脏区提交、区域直接绘制和 `esp_gsp_canvas_stop()` |
| 模板控件 | 创建、销毁、内容、可见性、位置和动画 API |
| 键盘 | `esp_gsp_keyboard_attach()`、`esp_gsp_keyboard_text()` |
| 定时器与动画 | 渲染任务定时器、数值/颜色/属性/位置动画 API |

Setter 在 ESP-IDF 上异步执行。COPY/BORROW/TAKE 所有权、回调上下文、同步与停止规则见[运行时与生成 API](../guide/runtime-api.md)。

`esp_gsp_on_pinch()` 上报由应用处理的双点手势；回调必须接受 `BEGIN` 后框架才会捕获，缩放和位置更新仍由应用决定。

## 生成 API

生成函数遵循：

```text
gsp_<scene>_<control>_<operation>()
```

具体操作由命名组件和动态属性决定。不要根据其他控件猜函数；重新构建并查看 `<symbol>_gsp.h`。每个[控件页面](../components/index.md)都会实际编译示例并列出生成签名。

### Dropdown 选择状态

为 Dropdown 设置稳定 `name`，`options` 定义索引到文字的映射，`selected` 定义初始索引：

```json
{
  "type": "dropdown",
  "parent": 0,
  "name": "display_mode",
  "x": 20, "y": 20, "w": 180, "h": 40,
  "options": ["Standard", "Vivid", "Reading"],
  "selected": 0
}
```

应用从 NVS 恢复状态时使用生成函数：

```c
ESP_ERROR_CHECK(gsp_settings_display_mode_set_selected(ui, saved_mode));
```

Setter 会拒绝超出 `options` 的索引，并同步保留态选择与显示文字。用户选择回调也使用同一个从零开始的索引；应用应持久化该参数，不要覆盖额外 Label 或直接操作编译器生成的 `__...` 对象。

只有高级数据驱动集成确实需要 Raw bind/action/object/property/template ID 时，才在包含生成头文件前定义 `GSP_BUNDLE_ENABLE_RAW_IDS`。

## 键盘流程

编译 `keyboard` 会生成一个按键回调，把它连接到已编写文字输入的 bind：

```c
#define GSP_BUNDLE_ENABLE_RAW_IDS 1
#include "bundle_gsp.h"

ESP_ERROR_CHECK(esp_gsp_keyboard_attach(
    ui, GSP_MAIN_ACT_ID_SEARCH_KEY, GSP_MAIN_BIND_SEARCH_TEXT));
```

字符键和删除键更新框架持有的编辑缓冲；应用收到 OK 动作时，最终文字已经可通过 `esp_gsp_keyboard_text()` 读取。必须使用当前场景生成的 action/bind 符号，不要复制例子中的名称。

## 诊断接口

`esp_gsp_debug.h` 是可选观测工具，不是应用状态。它提供帧、渲染、转场、区域、服务和媒体统计，以及测试输入注入。产品决策应依赖应用状态和正常错误返回，不能依赖诊断计数器。
