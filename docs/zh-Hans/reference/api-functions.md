# 公共函数参考

本页覆盖固件组件随包发布的应用层公共函数。普通应用先包含生成的
`<symbol>_gsp.h`；只有生成 API 无法表达的运行时能力才直接调用这些函数。

函数声明和返回类型由公共头文件提取，中文摘要说明应用用途。各函数的折叠区直接展示
头文件中的完整英文契约，包含线程、所有权和错误条件，并随头文件更新。
所有未明确标注为 ISR-safe 的入口都不得从 ISR 调用；回调线程、所有权和同步限制见各组说明。

完整使用路径见 [API 参考](api.md)、[生命周期与线程](../guide/lifecycle.md)和
[应用结构与所有权](../guide/application-structures.md)。

## 生命周期与同步

实例由平台适配层创建；停止和刷新函数定义应用与渲染任务之间的同步边界。

### `esp_gsp_config_init()`

初始化一份带当前 Schema 和 ABI 信息的配置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_config_t`

```c
static inline esp_gsp_config_t esp_gsp_config_init(void);
```

### `esp_gsp_config_set()`

设置一个单实例运行策略或容量 Override。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_config_set_result_t`

```c
static inline esp_gsp_config_set_result_t esp_gsp_config_set(esp_gsp_config_t *config, esp_gsp_config_field_id_t field, uint64_t value);
```

<details><summary>完整接口契约（英文）</summary>

Sets one per-instance runtime policy or capacity. Project-wide defaults
remain in Kconfig; GSPB scene requirements are applied automatically.
Read-only ESP_GSP_BUILD_CAP_* fields are rejected.

</details>

### `esp_gsp_stop()`

停止并销毁平台创建的 UI 实例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stop(esp_gsp_handle_t gsp);
```

<details><summary>完整接口契约（英文）</summary>

Stops a platform-created UI instance. Creation belongs to a platform
backend, for example the ESP-IDF LCD backend or the SDL
adapter on host.

</details>

### `esp_gsp_flush()`

等待此前提交的命令完成一次渲染尝试和显示提交。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_flush(esp_gsp_handle_t gsp, uint32_t timeout_ms);
```

<details><summary>完整接口契约（英文）</summary>

Wait until all commands submitted before this call have been applied and
the display backend has completed the following render attempt.

This is a low-frequency synchronization fence for tests, screenshots and
orderly application state changes. Normal UI updates should remain
asynchronous. It must not be called from a render- or decode-task callback.

Parameter `timeout_ms`: Maximum total wait, including command queue admission.
Zero performs a non-blocking check/submit.
A timeout does not cancel an accepted fence.

</details>

### `esp_gsp_render_error_stats()`

读取累计渲染失败次数和最近一次引擎错误。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `void`

```c
void esp_gsp_render_error_stats(esp_gsp_handle_t gsp, uint32_t *out_failures, gsp_err_t *out_last_error);
```

<details><summary>完整接口契约（英文）</summary>

Cumulative render failures and the most recent engine error. The first
frame is strict: esp_gsp_start() returns an error instead of publishing a
handle when it cannot be rendered. Runtime failures remain recoverable and
are counted here rather than silently discarded.

</details>

## 状态与键盘

基于 Bind 的 setter 向渲染任务排队，getter 返回最近已提交的状态；普通应用优先使用生成的类型化包装函数。

### `esp_gsp_set_value()`

设置数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_value(esp_gsp_handle_t gsp, uint16_t bind, int32_t value);
```

### `esp_gsp_set_color()`

设置颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_color(esp_gsp_handle_t gsp, uint16_t bind, uint32_t color);
```

<details><summary>完整接口契约（英文）</summary>

Color in the scene's native pixel format (RGB565 scenes take
16-bit colors, RGB888/ARGB8888 scenes take 24/32-bit).

</details>

### `esp_gsp_set_visible()`

设置可见状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_visible(esp_gsp_handle_t gsp, uint16_t bind, bool visible);
```

### `esp_gsp_set_text()`

设置文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_text(esp_gsp_handle_t gsp, uint16_t bind, const char *utf8);
```

<details><summary>完整接口契约（英文）</summary>

Shapes UTF-8 on the render task. The string is copied before return;
short values stay inline in the command and longer values use temporary
framework-owned storage.

</details>

### `esp_gsp_get_value()`

读取数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_value(esp_gsp_handle_t gsp, uint16_t bind, int32_t *out_value);
```

### `esp_gsp_get_color()`

读取颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_color(esp_gsp_handle_t gsp, uint16_t bind, uint32_t *out_color);
```

### `esp_gsp_get_visible()`

读取可见状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_visible(esp_gsp_handle_t gsp, uint16_t bind, bool *out_visible);
```

### `esp_gsp_get_toggle()`

读取开关状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_get_toggle(esp_gsp_handle_t gsp, uint16_t bind, bool *out_on);
```

<details><summary>完整接口契约（英文）</summary>

Logical state of a toggle's value bind (the knob tweens 0..100;
>= 50 reads as ON so a mid-animation read reports the target
side). Checkbox/radio store plain 0/1 — read those with
esp_gsp_get_value.

</details>

### `esp_gsp_keyboard_attach()`

把编译键盘的按键事件连接到文字 Bind。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_attach(esp_gsp_handle_t gsp, uint16_t action_id, uint16_t text_bind);
```

### `esp_gsp_keyboard_attach_ex()`

按应用指定的 UTF-8 字节上限连接编译键盘。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_attach_ex(esp_gsp_handle_t gsp, uint16_t action_id, uint16_t text_bind, size_t max_bytes);
```

<details><summary>完整接口契约（英文）</summary>

Extended attachment with an application-selected UTF-8 byte limit.
Storage is allocated once during attachment and reused for editing.
max_bytes excludes the trailing NUL and may use the available address
space; allocation failure is reported as ESP_GSP_ERR_NO_MEM. It is
ignored when action_id is ESP_GSP_KEYBOARD_NONE.

</details>

### `esp_gsp_keyboard_text()`

复制当前键盘编辑缓冲区中的文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_text(esp_gsp_handle_t gsp, char *out_text, size_t capacity);
```

<details><summary>完整接口契约（英文）</summary>

Copies the attached keyboard's current text (NUL terminated).
Returns ESP_GSP_ERR_INVALID_SIZE when capacity is too small; in that
case out_text still contains a valid UTF-8 prefix.

</details>

### `esp_gsp_keyboard_text_size()`

读取复制当前键盘文字所需的缓冲区大小。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_text_size(esp_gsp_handle_t gsp, size_t *out_size);
```

<details><summary>完整接口契约（英文）</summary>

Returns the buffer size, including the trailing NUL, required by
esp_gsp_keyboard_text().

</details>

### `esp_gsp_set_cursor()`

设置输入光标。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_cursor(esp_gsp_handle_t gsp, uint16_t bind);
```

## 命名控件

控件 API 使用生成的稳定对象键和属性键；应用仍应优先使用生成的类型化包装函数。

### `esp_gsp_component_get_motion()`

读取组件已提交值、目标值和拖动或缓动状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_motion(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_component_motion_t *out_state);
```

<details><summary>完整接口契约（英文）</summary>

Query committed and target motion state for PageFlow or Drawer. Like other
component getters, use from the UI task or a caller-serialized portable app.

</details>

### `esp_gsp_component_get()`

按稳定控件键读取兼容属性。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get(esp_gsp_handle_t gsp, gsp_component_key_t key, gsp_component_prop_kind_t prop, gsp_value_t *out_value);
```

### `esp_gsp_component_set()`

按稳定控件键设置兼容属性。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set(esp_gsp_handle_t gsp, gsp_component_key_t key, gsp_component_prop_kind_t prop, const gsp_value_t *value);
```

### `esp_gsp_component_get_property()`

读取控件属性。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_property(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, gsp_value_t *out_value);
```

<details><summary>完整接口契约（英文）</summary>

Open semantic-key API for component-local properties. Generated typed
wrappers hide the key and tagged value in ordinary application code.

</details>

### `esp_gsp_component_get_property_info()`

读取控件属性元数据。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_property_info(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, gsp_property_info_t *out_info);
```

### `esp_gsp_component_get_property_info_at()`

按索引枚举控件属性元数据。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_property_info_at(esp_gsp_handle_t gsp, gsp_component_key_t component, size_t index, gsp_property_info_t *out_info);
```

### `esp_gsp_component_set_property()`

设置控件属性。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_property(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *value);
```

### `esp_gsp_component_set_many()`

把一组类型化控件更新作为一个原子事务提交。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_many(esp_gsp_handle_t gsp, const gsp_component_update_t *updates, size_t count);
```

<details><summary>完整接口契约（英文）</summary>

Queues typed updates as one atomic render-task transaction. The array is
copied before return and may be stack allocated. Every entry is validated
before the batch is queued; repeated component/property pairs are allowed
and the last value wins. Large batches use temporary framework-owned heap
storage and report ESP_GSP_ERR_NO_MEM if it cannot be allocated.

</details>

### `esp_gsp_component_set_properties()`

把一组开放属性键更新作为一个原子事务提交。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_properties(esp_gsp_handle_t gsp, const gsp_component_property_update_t *updates, size_t count);
```

<details><summary>完整接口契约（英文）</summary>

Open-key atomic batch; same validation/copy/lifetime rules as set_many.

</details>

### `esp_gsp_component_set_position()`

设置控件位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_position(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t x, int32_t y);
```

<details><summary>完整接口契约（英文）</summary>

Atomically moves a compiled component subtree. Both x and y must have
bounded dynamic declarations; a literal axis has no writable runtime
property and returns NOT_FOUND without changing either axis. Use the
individual property setter when only one axis is dynamic. X/Y are authored
scene pixels stored in runtime SRAM; compiled commands remain read-only.
esp_gsp_update_error_stats() in esp_gsp_debug.h can identify the rejected
property and asynchronous application failures.

</details>

### `esp_gsp_component_get_position()`

读取控件位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_position(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t *out_x, int32_t *out_y);
```

<details><summary>完整接口契约（英文）</summary>

Reads the last committed authored position.

</details>

### `esp_gsp_component_translate()`

相对移动一个已编译控件子树。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_translate(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t dx, int32_t dy);
```

<details><summary>完整接口契约（英文）</summary>

Queues a relative move from the last committed authored position.

</details>

### `esp_gsp_component_get_info()`

读取控件元数据。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_info(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_component_info_t *out_info);
```

<details><summary>完整接口契约（英文）</summary>

Queries kind and available properties without reading component state.

</details>

### `esp_gsp_component_get_value()`

读取控件数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t *out_value);
```

<details><summary>完整接口契约（英文）</summary>

Strongly typed convenience APIs over esp_gsp_component_get/set().

</details>

### `esp_gsp_component_set_value()`

设置控件数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t value);
```

### `esp_gsp_component_get_color()`

读取控件颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_color(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t *out_native_color);
```

<details><summary>完整接口契约（英文）</summary>

Native scene color: RGB565 uses packed 16-bit values, RGB888 uses 0xRRGGBB.

</details>

### `esp_gsp_component_set_color()`

设置控件颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_color(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t native_color);
```

<details><summary>完整接口契约（英文）</summary>

Sets the component's primary color in the scene's native format.

</details>

### `esp_gsp_component_set_color_rgb888()`

把 RGB888 色值转换为当前场景原生颜色并设置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t rgb888);
```

<details><summary>完整接口契约（英文）</summary>

Sets the primary color from 0xRRGGBB, converting to the scene format.
Requires a canonical color property or a legacy color binding. For named
colors such as fg_color, use the generated property-specific helper.
RGB565 uses 5/6/5-bit truncation; ARGB8888 receives opaque alpha.
Values above 0xFFFFFF are rejected. Existing native-color APIs are unchanged.

</details>

### `esp_gsp_component_set_property_color_rgb888()`

使用统一 RGB888 颜色设置指定颜色属性，自动转换为场景原生格式。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_property_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, uint32_t rgb888);
```

<details><summary>完整接口契约（英文）</summary>

Sets a named COLOR property from 0xRRGGBB with scene-format conversion.
Uses the same validation, queue and transaction path as set_property().
Does not change component opacity or accept an alpha byte.

</details>

### `esp_gsp_component_get_color_rgb888()`

读取控件原生颜色并转换为 RGB888。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t *out_rgb888);
```

<details><summary>完整接口契约（英文）</summary>

Gets the primary color as 0xRRGGBB.

</details>

### `esp_gsp_component_get_visible()`

读取控件可见状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_visible(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_visible);
```

### `esp_gsp_component_set_visible()`

设置控件可见状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_visible(esp_gsp_handle_t gsp, gsp_component_key_t key, bool visible);
```

### `esp_gsp_component_set_text()`

设置控件文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_text(esp_gsp_handle_t gsp, gsp_component_key_t key, const char *text);
```

<details><summary>完整接口契约（英文）</summary>

Structured content setters resolve the generated property bind by key.

</details>

### `esp_gsp_component_set_image()`

设置控件图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_image(esp_gsp_handle_t gsp, gsp_component_key_t key, const void *data, size_t size);
```

### `esp_gsp_component_get_checked()`

读取控件选中状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_checked(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_checked);
```

<details><summary>完整接口契约（英文）</summary>

Toggle on/off by key — CHECKED semantics, no bind ids, no 0..100
rail convention.

</details>

### `esp_gsp_component_set_checked()`

设置控件选中状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_checked(esp_gsp_handle_t gsp, gsp_component_key_t key, bool checked);
```

### `esp_gsp_component_get_enabled()`

读取控件启用状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_enabled(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_enabled);
```

<details><summary>完整接口契约（英文）</summary>

Enables or disables a component that opted into authored `enabled`.

</details>

### `esp_gsp_component_set_enabled()`

设置控件启用状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_enabled(esp_gsp_handle_t gsp, gsp_component_key_t key, bool enabled);
```

### `esp_gsp_component_play_animation()`

播放可延迟、重复和往返的属性动画，隐藏时暂停。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_play_animation(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *from, const gsp_value_t *to, const esp_gsp_animation_config_t *config);
```

### `esp_gsp_component_stop_animation()`

停止指定属性动画并保留当前值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_stop_animation(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property);
```

### `esp_gsp_component_stop_position_animation()`

停止控件位置动画。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_stop_position_animation(esp_gsp_handle_t gsp, gsp_component_key_t component);
```

<details><summary>完整接口契约（英文）</summary>

Stops the component's synchronized position tween, if active. The
component stays where the last committed step put it.

</details>

## 导航与保留态容器

场景、PageFlow、StackView 和 Drawer 操作共用运行时的保留态导航与动画规则。

### `esp_gsp_page_flow_set_page()`

设置 PageFlow 当前页面。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_set_page(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t page, bool animated);
```

<details><summary>完整接口契约（英文）</summary>

Standard PageFlow navigation. Tab clicks and pointer settle use the same
driver state machine as this API.

</details>

### `esp_gsp_page_flow_get_page()`

读取 PageFlow 当前页面。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_get_page(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t *out_page);
```

### `esp_gsp_page_flow_get_offset()`

读取 PageFlow 沿主轴的当前偏移。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_get_offset(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t *out_offset_px);
```

<details><summary>完整接口契约（英文）</summary>

Current transform offset in pixels along the PageFlow axis. Unlike the
selected page, this remains precise while dragging and for stop-anywhere.

</details>

### `esp_gsp_page_flow_is_dragging()`

读取 PageFlow 当前是否正在拖动。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_is_dragging(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_dragging);
```

### `esp_gsp_stack_view_push()`

推送栈视图。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_push(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t page, bool animated);
```

<details><summary>完整接口契约（英文）</summary>

In-scene StackView navigation. Push/pop requests are rejected while an
animation is active; the root page cannot be popped.

</details>

### `esp_gsp_stack_view_pop()`

弹出栈视图。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_pop(esp_gsp_handle_t gsp, gsp_component_key_t key, bool animated);
```

### `esp_gsp_stack_view_get_top()`

读取栈视图栈顶。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_get_top(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t *out_page);
```

### `esp_gsp_stack_view_is_animating()`

读取 StackView 当前是否正在执行动画。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stack_view_is_animating(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_animating);
```

### `esp_gsp_drawer_open()`

打开 Drawer；动画中可从当前位置反向，重复当前目标不会重启动画。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_drawer_open(esp_gsp_handle_t gsp, gsp_component_key_t key, bool animated);
```

<details><summary>完整接口契约（英文）</summary>

Overlay Drawer state. Gesture and Close-button actions use this same
settle state machine. Animated requests can reverse an active settle from
its current position; repeating its target does not restart the animation.

</details>

### `esp_gsp_drawer_close()`

关闭 Drawer；与打开操作共用同一动画状态机。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_drawer_close(esp_gsp_handle_t gsp, gsp_component_key_t key, bool animated);
```

### `esp_gsp_drawer_is_open()`

读取 Drawer 当前是否打开。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_drawer_is_open(esp_gsp_handle_t gsp, gsp_component_key_t key, bool *out_open);
```

### `esp_gsp_goto_scene()`

切换到指定场景并选择转场方式。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_goto_scene(esp_gsp_handle_t gsp, uint16_t scene_id, esp_gsp_transition_t transition);
```

<details><summary>完整接口契约（英文）</summary>

Switches scenes (multi-scene bundles) with an optional transition.
Use ESP_GSP_CROSS_FADE to blend overlapping scenes, or
ESP_GSP_FADE_THROUGH_BLACK to dim the old scene fully to black
before revealing the new scene. Strip displays can render
fade-through-black incrementally without full-screen snapshots;
cross-fade still requires both scene snapshots. If no visual path
is available, the scene still switches safely. Safe from any task;
also invoked
automatically by `goto` actions and swipe navigation.

</details>

### `esp_gsp_set_swipe_enabled()`

设置场景滑动开关。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_enabled(esp_gsp_handle_t gsp, bool enabled);
```

<details><summary>完整接口契约（英文）</summary>

Enables or disables horizontal swipe scene navigation at runtime.
Multi-scene bundles enable swipes by default (`disable_swipe=false`
in esp_gsp_config_t); this mirrors that flag. The default classifier locks
only when horizontal travel clearly dominates vertical travel. Release
commits after a one-fifth-screen pull, or after a shorter deliberate fling;
tiny fast movements remain below the fling-distance guard. Safe from any
task.

</details>

### `esp_gsp_set_swipe_transition()`

设置场景滑动效果。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_transition(esp_gsp_handle_t gsp, esp_gsp_swipe_transition_t transition);
```

<details><summary>完整接口契约（英文）</summary>

Selects the visual that follows horizontal scene-swipe progress.
The default is ESP_GSP_SWIPE_SLIDE. The slide-fade option moves the
pages directly from gesture distance; the transition layer derives
brightness independently from that same raw distance, so no
effect-specific calculation lives in the touch path. Safe from any
task.

</details>

### `esp_gsp_set_swipe_fade_black_point()`

设置滑动渐黑位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_fade_black_point(esp_gsp_handle_t gsp, uint8_t drag_percent);
```

<details><summary>完整接口契约（英文）</summary>

Sets where a fade-through-black swipe reaches full black.
drag_percent is the horizontal drag distance as a percentage of
screen width (1..99). The default is 50. A smaller value dims the
current scene faster; the target scene still reaches full brightness
at 100%. This changes only brightness response, never page position,
gesture recognition or commit thresholds. Safe from any task.

</details>

## 运行时图片

COPY、BORROW 和 TAKE 变体具有不同的编码缓冲区所有权；释放或复用缓冲区前必须遵守函数契约。

### `esp_gsp_image_get_scale()`

读取图片缩放比例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_image_get_scale(esp_gsp_handle_t gsp, gsp_component_key_t image, uint32_t *out_scale_q16);
```

<details><summary>完整接口契约（英文）</summary>

Reads or updates a named scalable image's unsigned Q16.16 scale.

</details>

### `esp_gsp_image_set_scale()`

设置图片缩放比例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_image_set_scale(esp_gsp_handle_t gsp, gsp_component_key_t image, uint32_t scale_q16);
```

### `esp_gsp_set_image()`

复制编码图片并异步提交到图片 Bind。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size);
```

<details><summary>完整接口契约（英文）</summary>

Shows a runtime-provided encoded image (JPEG/PNG/QOI, e.g. a network
download) on an image bind. The container is sniffed from the data —
no metadata needed. The encoded payload is copied before this function
returns and decoded by a background worker, so the caller may immediately
release or reuse `data`.

</details>

### `esp_gsp_set_image_ex()`

按照指定所有权和完成回调提交编码图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_ex(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size, const esp_gsp_image_options_t *options);
```

<details><summary>完整接口契约（英文）</summary>

Complete runtime-image submission contract. NULL options select COPY.
BORROW returns the immutable source through on_release. TAKE transfers the
source only when this function returns ESP_GSP_OK; on immediate submission
failure the caller still owns it. on_complete is called only for accepted
submissions.

</details>

### `esp_gsp_set_image_borrowed()`

借用不可变编码图片，直到释放回调归还缓冲区。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_borrowed(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Borrowing variant of esp_gsp_set_image(). The encoded bytes are never
copied; `data` must remain immutable until `on_release` is called.
Replacement, cancellation, decode failure and shutdown all release the
payload exactly once. `on_release` is required.

Note that a published image holds its payload for as long as it is the one
on screen, not just until it decodes: the decode cache may drop the pixels
under memory pressure and re-decode from these bytes. So a borrowed buffer
comes back when the next image for that bind is adopted, or at shutdown —
budget for one outstanding buffer per image bind.

This removes only the encoded-input copy. Decode and framebuffer composition
still follow the target image/cache policy.

</details>

### `esp_gsp_set_image_owned()`

把 malloc 兼容的编码图片所有权转交给框架。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_owned(esp_gsp_handle_t gsp, uint16_t bind, void *data, size_t size);
```

<details><summary>完整接口契约（英文）</summary>

Transfers malloc-compatible encoded storage without copying. Ownership is
transferred only when ESP_GSP_OK is returned; the framework calls free()
once the image is replaced, cancelled or shut down — same lifetime as the
borrowed variant, without the callback.

</details>

### `esp_gsp_asset_image_target()`

构造页面 Image 目标，用于状态和停止操作。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_asset_target_t`

```c
static inline esp_gsp_asset_target_t esp_gsp_asset_image_target(uint16_t bind);
```

### `esp_gsp_asset_row_target()`

构造携带复用令牌的 Row 图片目标。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_asset_target_t`

```c
static inline esp_gsp_asset_target_t esp_gsp_asset_row_target(esp_gsp_row_t row, uint16_t slot);
```

### `esp_gsp_asset_widget_target()`

构造 Widget 图片资源槽目标。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_asset_target_t`

```c
static inline esp_gsp_asset_target_t esp_gsp_asset_widget_target(esp_gsp_widget_t widget, uint16_t slot);
```

### `esp_gsp_assets_open()`

打开已挂载文件系统中的预处理资源包，校验索引并创建后台读取任务。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_open(const char *path, uint32_t index_crc, const esp_gsp_assets_config_t *config, esp_gsp_assets_t **out_assets);
```

<details><summary>完整接口契约（英文）</summary>

Open a mounted SD/NAND filesystem package using the generated open helper.
index_crc comes from its generated header. A matching index takes the fast
path; a replacement package (or index_crc=0) is fully CRC-checked in bounded
chunks at open. Each requested member is CRC-checked when read. No whole-pack
RAM copy. Call from an application task; open performs IO.
File contents must stay immutable until close. Close before unmount/update.
NULL config selects defaults. On failure *out_assets is NULL.

</details>

### `esp_gsp_assets_show()`

异步显示资源包中的图片或逐帧播放动画；加载失败保留旧图。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, esp_gsp_asset_ref_t asset, bool once);
```

<details><summary>完整接口契约（英文）</summary>

Queue an image or animation for an authored Image bind. Asset names come
from the generated header. Submission returns without file IO or decoding.
once plays one animation cycle; false follows the authored loop count.
Existing content stays visible until a complete replacement is ready.
Each target has at most one in-flight frame. New requests coalesce per bind.
Animation memory follows ANIM_FRAME_MEMORY / ANIM_MAX_FRAME_BYTES and is
separate from the encoded read budget. JPEG animations also work with the
image cache disabled; measure playback performance on the target device.
The target placeholder must match the exported pixel/alpha form and native
dimensions (opaque image-fit binds can vary dimensions). Export to the same
RGB565/RGB888 profile as the UI.
Show targets the active scene. Switching scenes cancels subsequent frames;
resubmit when returning to that scene.
Hiding a page/group does not stop file IO or decoding. Stop its targets
when hidden and show them again when visible; show restarts playback.
gsp must remain alive until this service is closed. Use one service owner
per target; do not concurrently replace it with another image producer.

</details>

### `esp_gsp_assets_show_name()`

按清单名称异步显示资源，支持独立更新资源包中新增的素材。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, const char *name, bool once);
```

<details><summary>完整接口契约（英文）</summary>

Resolve an asset by its manifest name, including newly added assets.
Names are copied as a stable key; the caller may release name on return.
Same asynchronous result and lifetime contract as esp_gsp_assets_show().

</details>

### `esp_gsp_assets_show_row()`

异步加载 Row 图片，校验复用行令牌。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_row(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t slot, esp_gsp_asset_ref_t asset, bool once);
```

<details><summary>完整接口契约（英文）</summary>

Same ownership and scene rules as show; a recycled row token is checked
again on the UI task, so delayed data cannot replace a newly assigned row.

</details>

### `esp_gsp_assets_show_widget()`

异步加载 Widget 图片资源槽。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_widget(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, esp_gsp_asset_ref_t asset, bool once);
```

### `esp_gsp_assets_show_target()`

通过目标描述异步加载外部资源。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target, esp_gsp_asset_ref_t asset, bool once);
```

<details><summary>完整接口契约（英文）</summary>

Generic target forms for Image, Row and Widget destinations.

</details>

### `esp_gsp_assets_show_target_name()`

通过清单名称向指定目标异步加载外部资源。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_target_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target, const char *name, bool once);
```

### `esp_gsp_assets_get_status()`

读取请求编号、处理阶段、错误原因和所需字节数。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_get_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target, esp_gsp_asset_status_t *out_status);
```

### `esp_gsp_assets_stop_target()`

停止指定目标的后续帧，保留已显示内容。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_stop_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_asset_target_t target);
```

### `esp_gsp_assets_get_stats()`

按需读取编码缓冲、解码缓存和加载开销概况。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_get_stats(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_assets_stats_t *out_stats);
```

<details><summary>完整接口契约（英文）</summary>

On-demand overview: encoded bytes belong to this package; optional gsp
contributes all live scene decoded caches, including non-external images.
Decoded counters also include this service's animation canvases/patch scratch.
Peaks are conservative sums, not simultaneous samples. Display buffers and
codec-internal scratch are not included.

</details>

### `esp_gsp_assets_status()`

读取目标最近的加载结果和是否仍在处理。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t esp_gsp_assets_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, bool *out_pending);
```

<details><summary>完整接口契约（英文）</summary>

Last load/decode result for this target; out_pending reports unfinished work.
Returns GSP_ERR_NOT_FOUND before the target has been submitted.

</details>

### `esp_gsp_assets_stop()`

停止目标的后续帧调度，保留已显示的图片。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_stop(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind);
```

<details><summary>完整接口契约（英文）</summary>

Stop scheduling this target; its last published image remains visible.
An already accepted frame may finish. Call show again to restart/replace.

</details>

### `esp_gsp_assets_close()`

最多等待五秒关闭资源包；超时保留有效句柄，恢复处理后重试。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_close(esp_gsp_assets_t *assets);
```

<details><summary>完整接口契约（英文）</summary>

Stop new work and wait at most 5000 ms. On ESP_GSP_ERR_TIMEOUT the handle
remains valid and closing; resume UI/IO progress and retry close. Never
unmount storage or destroy the UI until close returns ESP_GSP_OK.

</details>

### `esp_gsp_assets_close_wait()`

在指定超时内关闭服务；超时后保留有效句柄供重试。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_close_wait(esp_gsp_assets_t *assets, uint32_t timeout_ms);
```

<details><summary>完整接口契约（英文）</summary>

Explicit wait budget; zero polls. Does not forcibly cancel a driver read.
On timeout status/stats and another close are allowed; show is rejected.
Call from an application task, not a render/decode callback. Other callers
must be quiescent before successful close frees the handle. Published source
buffers survive close until UI replacement/shutdown. NULL succeeds.

</details>

### `esp_gsp_font_file_open()`

在显式文件大小上限内加载动态字体或预编译字体目录包。

- **头文件:** `include/esp_gsp_font_file.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_font_file_open(const char *path, size_t max_bytes, esp_gsp_font_file_t **out_font);
```

<details><summary>完整接口契约（英文）</summary>

Loads a TTF/OTF/TTC or a GSPB font catalog from a mounted filesystem.
max_bytes is a required, nonzero limit on the file size. Oversized files
fail before allocation. The entire file stays in RAM (PSRAM preferred on
ESP-IDF); this is not on-demand glyph IO. Catalogs are CRC-checked; dynamic
fonts are checked by FreeType when the UI starts. Their signature is checked
here. TTC uses its first face. Dynamic fonts require gsp_enable_freetype()
in source and prebuilt builds. With dynamic fallback, each static GFB must
contain at most 32768 glyphs; UI startup rejects larger packs with NOT_SUPPORTED.
Call from an application task: open performs blocking file IO. On failure
*out_font is NULL. Glyph caches and FreeType working memory are separate.

</details>

### `esp_gsp_font_file_apply()`

将已加载字体应用到启动前的 UI 配置，保留另一类字体来源。

- **头文件:** `include/esp_gsp_font_file.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_font_file_apply(const esp_gsp_font_file_t *font, esp_gsp_config_t *config);
```

<details><summary>完整接口契约（英文）</summary>

Applies a loaded font to an initialized configuration before UI startup.
Sets ttf/ttf_size for a dynamic font, or font_catalog for a linked catalog;
the other font source is preserved. No ownership is transferred. Keep the
handle alive until every UI using this configuration has stopped successfully.
Changing this configuration does not replace a font in a running UI.

</details>

### `esp_gsp_font_file_close()`

在所有引用它的 UI 停止后释放字体文件及目录。

- **头文件:** `include/esp_gsp_font_file.h`
- **返回类型:** `void`

```c
void esp_gsp_font_file_close(esp_gsp_font_file_t *font);
```

<details><summary>完整接口契约（英文）</summary>

Frees a loaded font after all borrowing UIs have stopped, or startup failed.
NULL is accepted. Configurations that borrowed it must not be reused without
applying another font or clearing the corresponding font source fields.

</details>

## 事件、输入与覆盖层

回调必须遵守公共头文件标明的执行上下文；在渲染任务中执行的回调应保持短小且不阻塞。

### `esp_gsp_on_event()`

注册应用事件回调。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_event(esp_gsp_handle_t gsp, esp_gsp_event_cb_t cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Registers the application event callback (render-task context;
keep it short, use esp_gsp_set_* freely inside).

</details>

### `esp_gsp_on_pinch()`

注册由应用决定是否接管的双点 Pinch 回调。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_pinch(esp_gsp_handle_t gsp, esp_gsp_pinch_cb_t cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Registers the independent pinch observer through the render-task queue.

BEGIN is emitted once with scale 1.0. Return true to accept the pinch and
cancel the current single-pointer route; false leaves that touch sequence on
the single-pointer route. Later accepted samples emit UPDATE. END means the
contacts were released normally and CANCEL means recognition was
interrupted. Passing NULL unregisters the observer. The callback runs in
render-task context and must not block.

</details>

### `esp_gsp_set_pointer_observer()`

注册映射后的指针采样观察器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_pointer_observer(esp_gsp_handle_t gsp, esp_gsp_pointer_observer_cb_t cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Registers a pointer observer (render-task context). Pass `NULL` to
clear.

</details>

### `esp_gsp_set_overlay_contributor()`

设置覆盖层贡献者。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_overlay_contributor(esp_gsp_handle_t gsp, esp_gsp_overlay_contributor_cb_t cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Registers one application-owned overlay contributor through the
render-task queue (like esp_gsp_on_pinch): the registration and the
resulting overlay rebuild are applied on the render task, never inline
in the calling task. Passing NULL clears it. The callback runs on the
render task during overlay rebuild.

</details>

### `esp_gsp_set_input_interceptor()`

设置输入拦截器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_input_interceptor(esp_gsp_handle_t gsp, esp_gsp_input_interceptor_cb_t cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Registers one top-level input interceptor through the render-task
queue. Passing NULL clears it. The callback runs in render-task
context and must not block.

</details>

### `esp_gsp_overlay_builder_round_rect()`

向当前覆盖层追加一个圆角矩形。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_overlay_builder_round_rect(esp_gsp_overlay_builder_t *builder, int32_t x, int32_t y, uint16_t width, uint16_t height, uint32_t color, uint16_t radius, uint8_t opacity);
```

### `esp_gsp_overlay_builder_glyph_a8()`

向当前覆盖层追加一个 A8 字形。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_overlay_builder_glyph_a8(esp_gsp_overlay_builder_t *builder, int32_t x, int32_t y, uint16_t width, uint16_t height, uint32_t color, const uint8_t *a8, size_t a8_size, size_t stride_bytes);
```

<details><summary>完整接口契约（英文）</summary>

The `a8` bitmap is borrowed, not copied: it must stay alive until
the next overlay rebuild (any state change that repaints chrome, or
the next esp_gsp_set_overlay_contributor call).

</details>

## 列表、网格与消息

集合 API 只保留可见行或单元格；Binder 回调在渲染任务执行，回调结束后不得继续持有 Row Token。

### `esp_gsp_grid_bind_component()`

按稳定控件键绑定生成的 Grid。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_grid_t`

```c
esp_gsp_grid_t esp_gsp_grid_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_grid_bind_cb_t bind_item, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Binds a generated Grid component. The framework owns row/column mapping,
recycling, tail cells and scrolling; the callback only fills one item.

</details>

### `esp_gsp_grid_cell_set_image()`

设置Grid单元格图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const void *data, size_t size);
```

<details><summary>完整接口契约（英文）</summary>

Slot-free convenience setters for the Grid cell's first dynamic image or
text field. They return ESP_GSP_ERR_NOT_SUPPORTED when the template has no
matching dynamic field.

</details>

### `esp_gsp_grid_cell_set_image_ex()`

按照指定所有权设置 Grid 单元格图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image_ex(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const void *data, size_t size, const esp_gsp_image_options_t *options);
```

### `esp_gsp_grid_cell_set_image_borrowed()`

设置 Grid 单元格借用图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image_borrowed(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_grid_cell_set_image_owned()`

把 Grid 单元格图片所有权交给框架。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image_owned(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, void *data, size_t size);
```

### `esp_gsp_grid_cell_set_text()`

设置Grid单元格文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_text(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const char *utf8);
```

### `esp_gsp_grid_set_total()`

设置Grid总数。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_set_total(esp_gsp_handle_t gsp, esp_gsp_grid_t grid, uint32_t total);
```

<details><summary>完整接口契约（英文）</summary>

Grid-named aliases keep application code at the cell abstraction.

</details>

### `esp_gsp_grid_refresh()`

刷新Grid。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_refresh(esp_gsp_handle_t gsp, esp_gsp_grid_t grid);
```

### `esp_gsp_row_text()`

设置回收行中的第一个文字 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_text(esp_gsp_handle_t gsp, esp_gsp_row_t row, const char *utf8);
```

<details><summary>完整接口契约（英文）</summary>

Shapes UTF-8 into the row's TEXT slot (FreeType fallback and
multi-line wrap apply as for esp_gsp_set_text).

</details>

### `esp_gsp_row_set_text()`

设置行文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_text(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t text_slot, const char *utf8);
```

<details><summary>完整接口契约（英文）</summary>

Shapes UTF-8 into an explicit TEXT slot of a composite row.

</details>

### `esp_gsp_row_value()`

设置回收行中的第一个数值 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_value(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t value);
```

<details><summary>完整接口契约（英文）</summary>

Drives the row's first VALUE slot (0..100).

</details>

### `esp_gsp_row_set_value()`

设置行数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_value(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t value_slot, uint32_t value);
```

<details><summary>完整接口契约（英文）</summary>

Drives an explicit VALUE slot of a composite row.

</details>

### `esp_gsp_row_color()`

设置回收行中的第一个颜色 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_color(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t color);
```

<details><summary>完整接口契约（英文）</summary>

Drives the row's first COLOR slot (native format).

</details>

### `esp_gsp_row_set_color()`

设置行颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_color(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t color_slot, uint32_t color);
```

<details><summary>完整接口契约（英文）</summary>

Drives an explicit COLOR slot of a composite row.

</details>

### `esp_gsp_row_set_image()`

设置行图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, const void *data, size_t size);
```

<details><summary>完整接口契约（英文）</summary>

Replaces one RESOURCE slot in a recycled row from encoded JPEG/PNG/QOI
bytes. The input is copied before return. A stale row token cancels the
accepted request before publication, so an asynchronous loader cannot
publish into a slot that has since been recycled for another item.

</details>

### `esp_gsp_row_set_image_ex()`

按照指定所有权设置回收行图片 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image_ex(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, const void *data, size_t size, const esp_gsp_image_options_t *options);
```

### `esp_gsp_row_set_image_borrowed()`

设置回收行的借用图片 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image_borrowed(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Borrowed/owned variants of esp_gsp_row_set_image().

</details>

### `esp_gsp_row_set_image_owned()`

把回收行图片 Slot 的所有权交给框架。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image_owned(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, void *data, size_t size);
```

### `esp_gsp_list_bind_component()`

按稳定控件键绑定生成的 List 或 Wheel。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_list_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_list_bind_cb_t bind_item, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Creates the list/wheel by its stable component key: template,
viewport and initial total (the authored item count) come from the
generated directory — nothing to re-pair by hand. Requires
config.directories. A NULL `bind_item` serves the authored item
texts directly (fixed lists: zero application assembly). Adjust
the count later with esp_gsp_list_set_total when data is dynamic. The
returned binding remains valid until the UI instance is destroyed.

</details>

### `esp_gsp_bind_list()`

绑定List。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_bind_list(esp_gsp_handle_t gsp, uint16_t template_id, int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t total, esp_gsp_list_bind_cb_t bind_item, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

ADVANCED: manual template/viewport pairing (prefer the
component-key form above).

</details>

### `esp_gsp_list_set_total()`

设置List总数。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_set_total(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t total);
```

### `esp_gsp_list_refresh()`

刷新List。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_refresh(esp_gsp_handle_t gsp, esp_gsp_list_t list);
```

<details><summary>完整接口契约（英文）</summary>

Re-binds visible rows after the backing data changed.

</details>

### `esp_gsp_list_set_item_heights()`

原子替换可变高度列表的完整高度索引。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_set_item_heights(esp_gsp_handle_t gsp, esp_gsp_list_t list, const uint16_t *heights, uint32_t count, uint32_t prepended_count, bool stick_to_end);
```

<details><summary>完整接口契约（英文）</summary>

Enables variable-height rows and atomically replaces the complete height
index. There is no logical item limit other than memory and the signed
31-bit pixel extent. Existing fixed-height lists do not allocate this
index and keep their arithmetic fast path.

`prepended_count` maps the old first visible item onto its new index when
older records were inserted at the front. If `stick_to_end` is true, a
viewport already at the end remains attached to the end (new live messages
do not pull a user who is browsing older history).

</details>

### `esp_gsp_message_list_bind_component()`

绑定生成的 Message List 和应用数据源。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_message_list_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, const esp_gsp_message_source_t *source);
```

<details><summary>完整接口契约（英文）</summary>

Binds a generated message_list component and loads its current source.
The source is application-owned; its callbacks execute on the render task.

</details>

### `esp_gsp_message_list_changed()`

重新读取 Message List 数据源并更新布局。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_message_list_changed(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t prepended_count);
```

<details><summary>完整接口契约（英文）</summary>

Re-reads count/messages and updates layout. Pass the number inserted before
the previous item zero to preserve the visible history anchor. Appends keep
a viewport already at the end attached to the new end, without pulling a
user who is browsing older history.

</details>

### `esp_gsp_list_snap()`

启用或关闭 List/Wheel 的行边界吸附。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_snap(esp_gsp_handle_t gsp, esp_gsp_list_t list, bool enable);
```

<details><summary>完整接口契约（英文）</summary>

Wheel-style behavior: released drags settle on a row boundary.

</details>

### `esp_gsp_list_fling()`

以指定像素速度启动列表惯性滚动。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_fling(esp_gsp_handle_t gsp, esp_gsp_list_t list, int32_t velocity_px_s);
```

<details><summary>完整接口契约（英文）</summary>

Programmatic momentum: starts coasting at `velocity_px_s` (signed;
positive scrolls toward higher items). Friction and row snapping
behave exactly as a released drag. A newly bound visible list is activated
when this command is applied, so callers need not wait for a UI tick
between binding and requesting the fling.

</details>

### `esp_gsp_list_scroll_to()`

跳转到列表的绝对像素偏移。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_scroll_to(esp_gsp_handle_t gsp, esp_gsp_list_t list, int32_t offset_px);
```

<details><summary>完整接口契约（英文）</summary>

Jumps to an absolute scroll offset in pixels (clamped to content;
cancels any coasting). Fixed-height row N starts at N * row_height;
variable-height lists use cumulative item heights.

</details>

### `esp_gsp_list_fade()`

设置 Wheel 风格的上下边缘淡出带。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_fade(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t native_color, bool enable);
```

<details><summary>完整接口契约（英文）</summary>

Roller-style fade: translucent `native_color` bands dim the rows
toward the viewport's top and bottom edges.
Pass the wheel's background color. The bands render above the row
instances, which disables the scroll-blit shortcut over this
viewport — scrolling repaints the viewport instead.

</details>

### `esp_gsp_on_scroll()`

注册指定矩形内的纵向滚动回调。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_scroll(esp_gsp_handle_t gsp, gsp_rect_t region, esp_gsp_scroll_cb_t cb, void *user_ctx);
```

## Canvas

Canvas 帧采用借用语义；每个已接受帧恰好收到一次释放回调，立即返回错误时所有权仍属于调用者。

### `esp_gsp_canvas_set_draw_cb()`

注册直接绘制回调并请求首次完整重绘。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_set_draw_cb(esp_gsp_handle_t gsp, uint16_t bind, esp_gsp_canvas_draw_cb_t draw_cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Registers a direct draw callback and requests the first full repaint.
The authored bind must target an unscaled profile-native opaque image.
Existing esp_gsp_canvas_push() users are unaffected; pushing a frame to
the same bind replaces the callback. user_ctx must remain valid until the
callback is replaced or Canvas stop/destruction has completed; use
esp_gsp_flush() before releasing it after esp_gsp_canvas_stop(). A repaint
may invoke the callback more than once; render one coherent generation for
all regions belonging to that repaint.

</details>

### `esp_gsp_canvas_invalidate()`

请求重绘Canvas。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_invalidate(esp_gsp_handle_t gsp, uint16_t bind);
```

<details><summary>完整接口契约（英文）</summary>

Requests a full repaint of a callback-backed Canvas.

</details>

### `esp_gsp_canvas_invalidate_dirty()`

请求重绘Canvas脏区。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_invalidate_dirty(esp_gsp_handle_t gsp, uint16_t bind, gsp_rect_t dirty);
```

<details><summary>完整接口契约（英文）</summary>

Requests repaint of a non-empty Canvas-local rectangle.

</details>

### `esp_gsp_canvas_push()`

发布一个借用的 Canvas 完整帧。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_push(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Publishes a borrowed frame. Eligible renderer copies automatically use the
available PPA/DMA2D backend and otherwise fall back to software.

</details>

### `esp_gsp_canvas_try_push()`

以非阻塞方式尝试发布 Canvas 完整帧。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_try_push(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Non-blocking variant of esp_gsp_canvas_push(). Returns
ESP_GSP_ERR_TIMEOUT when the render queue cannot accept the frame
immediately; ownership remains with the caller in that case.

</details>

### `esp_gsp_canvas_push_dirty()`

发布借用帧并只重绘指定脏区。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_push_dirty(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Publishes a borrowed Canvas frame while repainting only `dirty`, in
resource-local pixels with an exclusive x2/y2 edge. Pixels outside dirty
must match the previously displayed frame. The dirty rectangle must be
non-empty and fit the authored placeholder. Ownership and release ordering
are identical to esp_gsp_canvas_push().

</details>

### `esp_gsp_canvas_try_push_dirty()`

以非阻塞方式尝试发布 Canvas 脏区帧。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_try_push_dirty(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Non-blocking variant of esp_gsp_canvas_push_dirty().

</details>

### `esp_gsp_canvas_stop()`

停止Canvas。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_stop(esp_gsp_handle_t gsp, uint16_t bind);
```

<details><summary>完整接口契约（英文）</summary>

Queues restoration of the placeholder image. The release callback is the
completion signal for each borrowed frame; do not free accepted frames
immediately after this function returns.

</details>

## 定时器、模板与动画

运行时对象消耗有界资源池并在渲染任务应用；模板配额来自场景需求和工程覆盖值。

### `esp_gsp_on_component_event()`

注册 PageFlow 和 Drawer 状态及运动完成事件回调。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_component_event(esp_gsp_handle_t gsp, esp_gsp_component_event_cb_t cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Subscribe to PageFlow/Drawer changes without polling. Registration is
serialized like setters; NULL unsubscribes. No initial event is emitted.
Notifications are coalesced per component per UI step and dispatched after
driver updates, outside driver iteration. The event contains final committed
state; intermediate requests within a step are not an event history.
MOTION_FINISHED also covers a return to the original value and an effective
non-animated change. An idle same-value request emits nothing. Scene teardown
discards pending notifications. Existing esp_gsp_on_event is unaffected.
Callbacks may use setters but must not block, flush or destroy the app.
With a continuously attached ESP-IDF runtime, an external task can unsubscribe
and successfully flush before freeing user_ctx. Do not concurrently tear down
or replace the runtime. Portable apps require caller serialization instead.

</details>

### `esp_gsp_scale_q16_from_percent()`

把整数百分比转换为无符号 Q16.16 缩放值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_from_percent(uint32_t percent);
```

<details><summary>完整接口契约（英文）</summary>

Converts an integer percentage to unsigned Q16.16 scale.

</details>

### `esp_gsp_scale_q16_multiply()`

组合图片缩放值与相对 Pinch 缩放值，并进行饱和处理。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_multiply(uint32_t scale_q16, uint32_t relative_q16);
```

<details><summary>完整接口契约（英文）</summary>

Combines an image scale and a relative pinch scale, with saturation.

</details>

### `esp_gsp_scale_q16_clamp()`

把 Q16.16 缩放值限制在指定范围内。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_clamp(uint32_t scale_q16, uint32_t minimum_q16, uint32_t maximum_q16);
```

<details><summary>完整接口契约（英文）</summary>

Clamps an unsigned Q16.16 scale to an authored or application range.

</details>

### `esp_gsp_chart_set_series()`

按业务数值一次提交整条曲线，返回前复制输入数组。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_chart_set_series(esp_gsp_handle_t gsp, const esp_gsp_chart_series_t *series, const int32_t *values, size_t count);
```

<details><summary>完整接口契约（英文）</summary>

Replaces every point in a fixed-capacity series in one render transaction.
Values use the authored business range and are copied before return. count
must equal the series capacity (at most ESP_GSP_CHART_MAX_POINTS). Small
batches stay inline; larger batches use temporary framework-owned storage
and return ESP_GSP_ERR_NO_MEM if allocation fails.

</details>

### `esp_gsp_chart_append()`

丢弃最旧采样点并按顺序追加一个业务数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_chart_append(esp_gsp_handle_t gsp, const esp_gsp_chart_series_t *series, int32_t value);
```

<details><summary>完整接口契约（英文）</summary>

Appends one business value, dropping the oldest point from the fixed
window. Consecutive queued appends are applied in order on the render task.

</details>

### `esp_gsp_set_press_feedback_enabled()`

启用或关闭默认按下遮罩，不改变点击路由。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_press_feedback_enabled(esp_gsp_handle_t gsp, bool enabled);
```

<details><summary>完整接口契约（英文）</summary>

Enable/disable the default press shade; processed on the render task.

</details>

### `esp_gsp_query_visibility()`

查询活动场景中对象布局与屏幕及父视口的有效可见交集。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_query_visibility(esp_gsp_handle_t gsp, const esp_gsp_visibility_target_t *target, bool *out_visible);
```

<details><summary>完整接口契约（英文）</summary>

True when the active scene, visibility gates and transformed layout bounds
intersect the screen/ancestor viewport. Does not test pixel alpha or occlusion
by unrelated siblings. Prefer the generated get_effective_visible() helper.

</details>

### `esp_gsp_timer_create()`

创建定时器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `void *`

```c
void *esp_gsp_timer_create(esp_gsp_handle_t gsp, uint32_t period_ms, esp_gsp_timer_cb_t cb, void *user_ctx);
```

<details><summary>完整接口契约（英文）</summary>

Periodic callback in render-task context (lv_timer equivalent).
Returns a handle usable with esp_gsp_timer_delete; NULL on error.
With the ESP-IDF runtime attached, creation and deletion may be called from
application tasks or timer callbacks, but not from an ISR. Keep the app and
runtime alive throughout these calls. Unattached portable apps require
caller serialization with app stepping and other timer operations.
The caller owns user_ctx and must keep it alive until callbacks have exited.

</details>

### `esp_gsp_timer_delete()`

删除定时器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_timer_delete(esp_gsp_handle_t gsp, void *timer);
```

<details><summary>完整接口契约（英文）</summary>

Stops future scheduling; a callback already selected for execution may
still run. This call does not wait for that callback to return.
The handle must belong to this app and must not be used after deletion
(timer slots may be reused). Callbacks may delete their own timer.

An external task using a continuously attached ESP-IDF runtime can wait
for an in-flight callback by successfully deleting the timer and then
successfully calling esp_gsp_flush(). Keep user_ctx alive if flush fails
or times out. Do not use this wait from a timer callback, concurrently with
app/runtime teardown or replacement, or while holding a lock needed by the
callback. This does not cover asynchronous work launched by the callback or
other timers sharing/re-registering user_ctx. Unattached portable apps do
not provide this cross-task flush barrier.

</details>

### `esp_gsp_widget_create()`

在当前场景创建一个已声明模板的运行时实例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_widget_t`

```c
esp_gsp_widget_t esp_gsp_widget_create(esp_gsp_handle_t gsp, uint16_t template_id, int16_t x, int16_t y);
```

<details><summary>完整接口契约（英文）</summary>

Instantiates `template_id` at (`x`, `y`) in the CURRENT scene.
Creation is asynchronous; the handle is immediately usable with the
widget setters. Returns ESP_GSP_WIDGET_NONE when the widget quota
is used up.

</details>

### `esp_gsp_widget_create_template()`

创建模板实例模板。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_widget_t`

```c
static inline esp_gsp_widget_t esp_gsp_widget_create_template(esp_gsp_handle_t gsp, const esp_gsp_template_desc_t *template_desc, int16_t x, int16_t y);
```

<details><summary>完整接口契约（英文）</summary>

Named-descriptor form; avoids exposing the compiled template id.

</details>

### `esp_gsp_widget_destroy()`

销毁模板实例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_destroy(esp_gsp_handle_t gsp, esp_gsp_widget_t widget);
```

### `esp_gsp_widget_set_value()`

设置模板实例数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_value(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, uint32_t value);
```

<details><summary>完整接口契约（英文）</summary>

Drives a VALUE slot (progress members), value in 0..100.

</details>

### `esp_gsp_widget_set_color()`

设置模板实例颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_color(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, uint32_t color);
```

<details><summary>完整接口契约（英文）</summary>

Drives a COLOR slot, native pixel format (see esp_gsp_set_color).

</details>

### `esp_gsp_widget_set_text()`

设置模板实例文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_text(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, const char *utf8);
```

<details><summary>完整接口契约（英文）</summary>

Shapes UTF-8 into a TEXT slot; the string is copied before return.

</details>

### `esp_gsp_widget_set_image()`

设置模板实例图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_image(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot, const void *data, size_t size);
```

<details><summary>完整接口契约（英文）</summary>

Replaces a template RESOURCE slot from copied encoded image bytes.

</details>

### `esp_gsp_widget_set_image_borrowed()`

设置模板实例的借用图片 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_image_borrowed(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_widget_set_image_owned()`

把模板实例图片 Slot 的所有权交给框架。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_image_owned(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot, void *data, size_t size);
```

### `esp_gsp_widget_value()`

设置模板实例的第一个数值 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_value(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t value);
```

### `esp_gsp_widget_color()`

设置模板实例的第一个颜色 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_color(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint32_t color);
```

### `esp_gsp_widget_text()`

设置模板实例的第一个文字 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_text(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, const char *utf8);
```

### `esp_gsp_widget_set_visible()`

设置模板实例可见状态。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_visible(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, bool visible);
```

### `esp_gsp_widget_set_position()`

设置模板实例位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_position(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, int16_t x, int16_t y);
```

### `esp_gsp_play()`

在图片占位资源上播放 EAF 帧动画。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_play(esp_gsp_handle_t gsp, const void *eaf, size_t size, bool once);
```

<details><summary>完整接口契约（英文）</summary>

Plays an EAF frame animation into a profile-native RGB565/RGB888
placeholder image resource. The framework ticks it on the render task —
no app-side loop. `eaf` is NOT copied. once = stop on the last frame.

</details>

### `esp_gsp_animate()`

执行动画。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_animate(esp_gsp_handle_t gsp, uint16_t bind, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

<details><summary>完整接口契约（英文）</summary>

Animates a value bind from `from` to `to` (0..100 domain) over
`duration_ms`. A new animation on the same bind replaces the
running one.

</details>

### `esp_gsp_animate_color()`

执行动画颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_animate_color(esp_gsp_handle_t gsp, uint16_t bind, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

<details><summary>完整接口契约（英文）</summary>

Tweens a color bind between two native colors (per-channel
interpolation, RGB565 scenes).

</details>

### `esp_gsp_component_animate_value()`

执行动画控件数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

<details><summary>完整接口契约（英文）</summary>

Component-key animations use semantic VALUE/COLOR properties and the
same authored-unit conversion as component get/set.

</details>

### `esp_gsp_component_animate_value_to()`

执行动画控件数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_value_to(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_color()`

执行动画控件颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_color(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_color_to()`

执行动画控件颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_color_to(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_property()`

执行动画控件属性。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_property(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *from, const gsp_value_t *to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

<details><summary>完整接口契约（英文）</summary>

Open-key animation for any scalar semantic property.

</details>

### `esp_gsp_component_animate_property_to()`

执行动画控件属性。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_property_to(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, const gsp_value_t *to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_position_to()`

执行动画控件位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_position_to(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t x, int32_t y, uint32_t duration_ms, esp_gsp_ease_t ease);
```

<details><summary>完整接口契约（英文）</summary>

Starts one synchronized x/y tween from the committed position. Both
axes are committed together on every step, so no frame shows the
component at a position between the two. Animating x and y as two
separate properties does not give that guarantee.

</details>

### `esp_gsp_widget_animate()`

执行动画模板实例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_animate(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

<details><summary>完整接口契约（英文）</summary>

Same, driving a widget's first VALUE slot.

</details>

## ESP-LCD 生命周期

这些函数管理 ESP-IDF 渲染任务，并协调显示提交、暂停、挂起和保留 Session 的切换。

### `esp_gsp_esp_lcd_config_init()`

初始化一份 ESP-LCD 平台配置。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_gsp_esp_lcd_config_t`

```c
static inline esp_gsp_esp_lcd_config_t esp_gsp_esp_lcd_config_init(void);
```

### `esp_gsp_esp_lcd_pause()`

静默活动 UI，并等待共享显示提交完成。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_pause(esp_gsp_handle_t gsp, uint32_t timeout_ms, esp_gsp_esp_lcd_pause_t **out_pause);
```

<details><summary>完整接口契约（英文）</summary>

Quiesce the active GSP render task without destroying its UI state.

On success no new GSP frame can be submitted until
esp_gsp_esp_lcd_resume_paused() is called. The function also waits for the
shared presenter transfer/present fences to retire.

</details>

### `esp_gsp_esp_lcd_resume_paused()`

恢复 Pause Token 对应的同一个 UI。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_resume_paused(esp_gsp_esp_lcd_pause_t *pause, esp_gsp_handle_t *out_gsp);
```

<details><summary>完整接口契约（英文）</summary>

Resume the exact UI paused by esp_gsp_esp_lcd_pause(), including a temporary
app running on a retained hub session. The pause token is consumed on
success and the first resumed frame is forced to redraw in full.

</details>

### `esp_gsp_esp_lcd_suspend()`

挂起 UI，同时保留场景和控件状态。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_suspend(esp_gsp_handle_t hub, esp_gsp_esp_lcd_session_t **out_session);
```

<details><summary>完整接口契约（英文）</summary>

Pause a UI while retaining its scenes and component state. Its render task
and LCD host remain resident; decoded image surfaces are released.

</details>

### `esp_gsp_esp_lcd_start_on_session()`

在挂起 Hub 的显示宿主上启动应用 Bundle。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start_on_session(esp_gsp_esp_lcd_session_t *session, const esp_gsp_config_t *app_config, esp_gsp_handle_t *out_gsp);
```

<details><summary>完整接口契约（英文）</summary>

Start an app bundle on a suspended hub's LCD host. The bundle must match
the display width, height and pixel format. A startup failure resumes the
hub and consumes the session.

</details>

### `esp_gsp_esp_lcd_start_on_session_prepared()`

启动应用，并在第一帧前由驻留渲染任务执行准备回调。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start_on_session_prepared(esp_gsp_esp_lcd_session_t *session, const esp_gsp_config_t *app_config, esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx, esp_gsp_handle_t *out_gsp);
```

<details><summary>完整接口契约（英文）</summary>

Start an app and invoke `prepare` on the resident render task before the
first frame. This is intended for installing generic runtime contributors,
event handlers and timers without racing the initial render.

</details>

### `esp_gsp_esp_lcd_replace_on_session_prepared()`

替换临时应用，期间不重新显示 Hub。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_replace_on_session_prepared(esp_gsp_esp_lcd_session_t *session, esp_gsp_handle_t current_app, const esp_gsp_config_t *next_config, esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx, esp_gsp_handle_t *out_gsp);
```

<details><summary>完整接口契约（英文）</summary>

Replace the active temporary app while retaining the suspended hub.
Unlike esp_gsp_esp_lcd_resume(), this never reactivates or presents the
hub between the two Apps. On startup failure the hub is resumed and the
session is consumed.

</details>

### `esp_gsp_esp_lcd_resume()`

停止临时应用并恢复保留的 Hub。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_resume(esp_gsp_esp_lcd_session_t *session, esp_gsp_handle_t app, esp_gsp_handle_t *out_hub);
```

<details><summary>完整接口契约（英文）</summary>

Stop the temporary app, then resume the retained hub and force its first
frame to redraw. The session is consumed on success.

</details>

### `esp_gsp_esp_lcd_session_destroy()`

销毁没有活动临时应用的挂起 Session。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_session_destroy(esp_gsp_esp_lcd_session_t *session);
```

<details><summary>完整接口契约（英文）</summary>

Destroy a suspended hub and its LCD host when no temporary app is active.

</details>

### `esp_gsp_esp_lcd_start()`

创建 ESP-LCD 后端并启动 UI。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start(const esp_gsp_config_t *app_config, const esp_gsp_esp_lcd_config_t *esp_config, esp_gsp_handle_t *out_gsp);
```

<details><summary>完整接口契约（英文）</summary>

Creation and resource validation use the caller's stack before the render
task starts. See getting-started: the example starts with a 20 KiB main stack;
measure the caller's high-water mark with the actual asset/font workload.

</details>

### `esp_gsp_esp_lcd_start_prepared()`

启动 LCD 应用并在渲染任务首帧前执行准备回调。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start_prepared(const esp_gsp_config_t *app_config, const esp_gsp_esp_lcd_config_t *esp_config, esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx, esp_gsp_handle_t *out_gsp);
```

<details><summary>完整接口契约（英文）</summary>

Like start(), with an optional callback on the render task before its first
frame. Use it to install contributors, event handlers and timers without
racing the initial render. A NULL callback is equivalent to start().
If invoked, the callback completes before this function returns, including
first-frame failure. Its void return cannot report preparation failures;
the returned error covers framework startup and the first render attempt.
A context retained by a registered callback must outlive that registration.

</details>

## Deployable Bundle

Deployable 元数据从借用的 GSPB 字节解析；关闭元数据句柄之前，这些字节必须保持有效。

### `esp_gsp_deployable_bundle_open()`

解析自描述 Deployable GSPB 元数据。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_open(const void *data, size_t size, bool verify_crc, esp_gsp_deployable_bundle_t **out_bundle);
```

<details><summary>完整接口契约（英文）</summary>

Opens a borrowed GSPB containing one GMD member for every GSB scene.

The input bytes must be 64-byte aligned and remain valid until the handle
is closed. When `verify_crc` is false, structural validation still runs.

</details>

### `esp_gsp_deployable_bundle_get_info()`

读取 Deployable Bundle 的尺寸、像素格式和场景信息。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_get_info(const esp_gsp_deployable_bundle_t *bundle, esp_gsp_deployable_info_t *out_info);
```

<details><summary>完整接口契约（英文）</summary>

Returns immutable package requirements discovered during open.

</details>

### `esp_gsp_deployable_bundle_make_config()`

从 Deployable Bundle 元数据构造运行时配置。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_make_config(const esp_gsp_deployable_bundle_t *bundle, esp_gsp_config_t *out_config);
```

<details><summary>完整接口契约（英文）</summary>

Builds an application configuration backed by the opened bundle.

The deployable handle and its borrowed bytes must outlive the UI instance.
Stop the UI before calling esp_gsp_deployable_bundle_close().

</details>

### `esp_gsp_deployable_bundle_action_key()`

把场景内数字 CALL ID 解析为稳定动作键。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_action_key(const esp_gsp_deployable_bundle_t *bundle, uint16_t scene_id, uint16_t action_id, uint32_t *out_key);
```

<details><summary>完整接口契约（英文）</summary>

Resolves a scene-local numeric CALL id to its stable authored-name key.

</details>

### `esp_gsp_deployable_bundle_close()`

释放解析后的 Deployable 元数据。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `void`

```c
void esp_gsp_deployable_bundle_close(esp_gsp_deployable_bundle_t *bundle);
```

<details><summary>完整接口契约（英文）</summary>

Releases parsed metadata. The borrowed GSPB bytes are never freed.

</details>

## 诊断

诊断计数器用于分析渲染、输入和资源使用；产品状态由应用代码维护。

### `esp_gsp_heap_stats()`

按需读取设备内部 RAM 与 PSRAM 的空闲字节和最大连续块；不支持时返回 false 并清零。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `bool`

```c
bool esp_gsp_heap_stats(esp_gsp_heap_stats_t *out_stats);
```

<details><summary>完整接口契约（英文）</summary>

Read device-wide internal/PSRAM heaps on demand, including non-GSP users.
Does not allocate or change budgets. Call from task context, not an ISR or
every frame: heap inspection traverses allocator metadata. The two heaps
are sampled separately; concurrent allocations can change the result.
Returns false and zeroes the output on hosts without heap capabilities;
also returns false for NULL. An absent PSRAM heap has zero values.
These values are not DMA guarantees or a complete scene memory budget.

</details>

### `esp_gsp_update_error_stats()`

读取组件批量更新的失败计数及最近保留的失败阶段、对象和属性；无需开启 profiling，并发遇忙时返回 false 且不修改输出。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `bool`

```c
bool esp_gsp_update_error_stats(esp_gsp_handle_t gsp, esp_gsp_update_error_stats_t *out_stats, size_t stats_size);
```

<details><summary>完整接口契约（英文）</summary>

Snapshot set_many/set_properties failures and wrappers routed through them
(including set_position), including asynchronous apply/drop errors. Legacy
bind setters and other API families are not included.
Available in ordinary builds without profiling. Successful setters do not
clear the last error. A property absent from the compiled runtime directory
cannot be distinguished from an unsupported property: consult the compiler's
schema and declare supported x/y fields dynamic before calling set_position.

The detail record is coherent and best-effort: concurrent readers/writers
never wait; a contending writer increments details_dropped instead. Counters
are atomic but can include failures newer than the retained detail. This is
diagnostic history, not an acknowledgement for a particular command or task.
esp_gsp_flush() still waits for a render attempt, not successful updates.

Pass sizeof(*out_stats). On success the known prefix is copied, any extra
bytes are zeroed, and struct_size is the copied size. Returns false without
touching the output for NULL, a size below sizeof(uint32_t), or a busy record
(retry later). Call while the handle is alive, outside ISR context.

</details>

### `esp_gsp_frame_count()`

读取启动后的非空闲渲染帧数。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `uint32_t`

```c
uint32_t esp_gsp_frame_count(esp_gsp_handle_t gsp);
```

<details><summary>完整接口契约（英文）</summary>

Monotonic count of rendered non-idle frames since startup.

</details>

### `esp_gsp_render_stats()`

读取累计渲染帧数和忙碌时间。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_render_stats(esp_gsp_handle_t gsp, uint32_t *out_frames, uint64_t *out_busy_us);
```

<details><summary>完整接口契约（英文）</summary>

Read rendered frames and cumulative time spent producing frames.

</details>

### `esp_gsp_transition_stats()`

读取各转场路径的累计统计。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_transition_stats(esp_gsp_handle_t gsp, esp_gsp_transition_stats_t *out_stats);
```

### `esp_gsp_drag_snapshot_stats()`

读取拖拽快照启用、复用及各类跳过原因的累计计数。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_drag_snapshot_stats(esp_gsp_handle_t gsp, esp_gsp_drag_snapshot_stats_t *out_stats);
```

<details><summary>完整接口契约（英文）</summary>

Cumulative List/Grid/MessageList and viewport drag snapshot accounting.
Read from a serialized app callback, like the other debug counters.

</details>

### `esp_gsp_region_stats()`

读取脏区规划和合并统计。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_region_stats(esp_gsp_handle_t gsp, esp_gsp_region_stats_t *out_stats);
```

### `esp_gsp_render_phases()`

读取累计光栅化和显示提交时间。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_render_phases(esp_gsp_handle_t gsp, uint64_t *out_render_us, uint64_t *out_submit_us);
```

<details><summary>完整接口契约（英文）</summary>

Cumulative rasterization and presentation time.

</details>

### `esp_gsp_service_stats()`

读取渲染任务服务循环统计。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_service_stats(esp_gsp_handle_t gsp, uint32_t *out_iterations, uint64_t *out_service_us, uint32_t *out_commands);
```

<details><summary>完整接口契约（英文）</summary>

Render-task service-loop profile. Values are zero unless the integration
enables the lightweight GSP_PROFILE_SERVICE build instrumentation.

</details>

### `esp_gsp_property_stats()`

读取组件属性更新的批次、条目与分层计时统计。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `bool`

```c
bool esp_gsp_property_stats(esp_gsp_handle_t gsp, esp_gsp_property_stats_t *out_stats, size_t stats_size);
```

<details><summary>完整接口契约（英文）</summary>

Snapshot the component update counters. Pass
sizeof(esp_gsp_property_stats_t) as `stats_size`; the library fills the
common prefix and reports it in struct_size, so a caller built against a
different header revision still reads the fields it knows.

Returns true only when this library carries the GSP_PROFILE_SERVICE
instrumentation and `gsp` is valid. An ordinary build has no counters at
all: the call then writes `stats_size` zeroed bytes, sets struct_size,
leaves counters_available at 0 and returns false, so a zeroed reading is
never mistaken for measured traffic.

Two cases write nothing at all and return false: `out_stats` is NULL, or
`stats_size` is under sizeof(uint32_t) and so cannot hold even the
struct_size and availability prefix. In both the caller's buffer is left
exactly as it was. Every other `stats_size` is fully written.

Counters are cumulative and wrap; take two snapshots and subtract. Every
field is read with a single atomic load, so snapshots do not race with
counter updates or report torn values. Fields are sampled one after
another and may belong to slightly different moments; this is not an
atomic snapshot of the entire pipeline.

Atomic operations use the target toolchain's implementation. On targets
without native support, including 64-bit totals on supported 32-bit SoCs,
ESP-IDF helpers use brief interrupt-disabled critical sections or spinlocks.
Reads and updates can therefore delay another task; the overhead depends
on the target and contention. These helpers do not acquire UI/state
mutexes, and this cost only exists in an instrumented build.

</details>

### `esp_gsp_media_stats()`

读取图片、动画和 Canvas 流水线统计。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_media_stats(esp_gsp_handle_t gsp, esp_gsp_media_stats_t *out_stats);
```

### `esp_gsp_inject_touch()`

注入一条逻辑指针样本，仅用于测试和诊断。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_inject_touch(esp_gsp_handle_t gsp, int16_t x, int16_t y, bool pressed);
```

<details><summary>完整接口契约（英文）</summary>

Inject one logical pointer sample. Intended for tests and diagnostics.

</details>

## 高级集成

高级入口用于定制渲染集成，可访问底层渲染状态。

### `esp_gsp_context()`

取得当前底层渲染上下文。

- **头文件:** `include/esp_gsp_advanced.h`
- **返回类型:** `gsp_context_t *`

```c
gsp_context_t *esp_gsp_context(esp_gsp_handle_t gsp);
```

<details><summary>完整接口契约（英文）</summary>

Return the active low-level rendering context.

</details>

## 可移植 Core 与框架集成

这些底层头文件面向平台适配器、Launcher 和生成的控件目录；普通 ESP-IDF 应用应使用 `esp_gsp.h`。

### `gsp_app_create()`

创建一个可移植 GSP 应用实例。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_app_create(const gsp_app_config_t *config, gsp_app_t **out_app);
```

### `gsp_app_effective_config()`

读取应用解析完成后的不可变配置。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `const esp_gsp_effective_config_t *`

```c
const esp_gsp_effective_config_t *gsp_app_effective_config(const gsp_app_t *app);
```

### `gsp_app_destroy()`

销毁可移植 GSP 应用实例。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `void`

```c
void gsp_app_destroy(gsp_app_t *app);
```

### `gsp_app_apply()`

向可移植应用提交一条命令。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_app_apply(gsp_app_t *app, const gsp_app_command_t *command);
```

### `gsp_app_step()`

让可移植应用推进到指定时间。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `void`

```c
void gsp_app_step(gsp_app_t *app, int64_t now_us);
```

### `gsp_app_next_deadline_us()`

读取应用下一次必须处理的时间点。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `int64_t`

```c
int64_t gsp_app_next_deadline_us(const gsp_app_t *app);
```

### `gsp_app_feed_pointer()`

向可移植应用注入单指指针状态。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `void`

```c
void gsp_app_feed_pointer(gsp_app_t *app, int32_t x, int32_t y, bool pressed);
```

<details><summary>完整接口契约（英文）</summary>

Scripted pointer injection (tests, simulators, demos): feeds the
classifier directly, bypassing the input interceptor and observer
that platform runners route through.

</details>

### `gsp_app_feed_touch()`

向可移植应用输入一个多点触摸 Contact。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `void`

```c
void gsp_app_feed_touch(gsp_app_t *app, int32_t id, int32_t x, int32_t y, bool pressed);
```

<details><summary>完整接口契约（英文）</summary>

Feeds one contact from a multi-touch source. The portable app keeps the
active contact set and routes its centroid through the pointer classifier;
platform pinch-aware builds may consume the same contact stream directly.

</details>

### `gsp_app_current_scene()`

读取应用当前场景 ID。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `uint16_t`

```c
uint16_t gsp_app_current_scene(const gsp_app_t *app);
```

### `gsp_app_scene_count()`

读取应用包含的场景数量。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `uint16_t`

```c
uint16_t gsp_app_scene_count(const gsp_app_t *app);
```

### `gsp_app_current_context()`

取得当前场景的底层 Context。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `gsp_context_t *`

```c
gsp_context_t *gsp_app_current_context(gsp_app_t *app);
```

### `gsp_app_scene_context()`

取得指定场景的底层 Context。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `gsp_context_t *`

```c
gsp_context_t *gsp_app_scene_context(gsp_app_t *app, uint16_t scene);
```

### `gsp_app_transition()`

读取当前活动转场状态。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `bool`

```c
bool gsp_app_transition(const gsp_app_t *app, gsp_app_transition_t *out_transition);
```

### `gsp_drawer_axis_is_y()`

判断 Drawer 是否沿 Y 轴移动。

- **头文件:** `include/gsp/gsp_component.h`
- **返回类型:** `bool`

```c
static inline bool gsp_drawer_axis_is_y(const gsp_drawer_config_t *config);
```

<details><summary>完整接口契约（英文）</summary>

True when the drawer travels along Y, so callers know which axis of
the transform group carries its offset.

</details>

### `gsp_drawer_closed_offset()`

计算 Drawer 靠指定边缘关闭时的偏移。

- **头文件:** `include/gsp/gsp_component.h`
- **返回类型:** `int32_t`

```c
static inline int32_t gsp_drawer_closed_offset(const gsp_drawer_config_t *config);
```

<details><summary>完整接口契约（英文）</summary>

Transform-group offset that parks the drawer off-screen against its
edge; open is always offset zero.

The compiler bakes this same value into the scene's initial state and
the gesture layer measures drag progress against it, so it has to be
one definition — a driver and an input layer that disagree about
where "closed" is produce a drawer that snaps to the wrong end.

</details>

### `gsp_component_find()`

在生成目录中按稳定键解析控件 View。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_find(const gsp_component_directory_t *directory, gsp_runtime_t *runtime, gsp_component_key_t key, gsp_component_view_t *out_view);
```

### `gsp_component_get()`

从底层控件 View 读取兼容属性。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_get(const gsp_component_view_t *view, gsp_component_prop_kind_t prop, gsp_value_t *out_value);
```

### `gsp_component_get_property()`

读取控件属性。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_get_property(const gsp_component_view_t *view, gsp_property_key_t property, gsp_value_t *out_value);
```

### `gsp_component_get_property_info()`

读取控件属性元数据。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_get_property_info(const gsp_component_view_t *view, gsp_property_key_t property, gsp_property_info_t *out_info);
```

### `gsp_component_get_property_info_at()`

按索引枚举底层控件属性元数据。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_get_property_info_at(const gsp_component_view_t *view, size_t index, gsp_property_info_t *out_info);
```

### `gsp_component_set()`

在更新事务中设置底层控件兼容属性。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_set(gsp_update_transaction_t *transaction, const gsp_component_view_t *view, gsp_component_prop_kind_t prop, const gsp_value_t *value);
```

### `gsp_component_set_property()`

设置控件属性。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_set_property(gsp_update_transaction_t *transaction, const gsp_component_view_t *view, gsp_property_key_t property, const gsp_value_t *value);
```

### `gsp_component_set_many()`

在一个底层更新事务中批量设置兼容属性。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_set_many(gsp_update_transaction_t *transaction, const gsp_component_directory_t *directory, const gsp_component_update_t *updates, size_t count);
```

### `gsp_component_set_properties()`

在一个底层更新事务中批量设置开放属性键。

- **头文件:** `include/gsp/gsp_component_advanced.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_component_set_properties(gsp_update_transaction_t *transaction, const gsp_component_directory_t *directory, const gsp_component_property_update_t *updates, size_t count);
```

### `gsp_font_catalog_create()`

创建字体目录。

- **头文件:** `include/gsp/gsp_font_catalog.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_font_catalog_create(const void *data, size_t size, bool verify_crc, gsp_font_catalog_t **out_catalog);
```

<details><summary>完整接口契约（英文）</summary>

Opens a borrowed GSPB containing GFB members keyed by content_id.

</details>

### `gsp_font_catalog_destroy()`

销毁字体目录。

- **头文件:** `include/gsp/gsp_font_catalog.h`
- **返回类型:** `void`

```c
void gsp_font_catalog_destroy(gsp_font_catalog_t *catalog);
```

### `gsp_font_catalog_find()`

在外部字体目录中查找指定字体。

- **头文件:** `include/gsp/gsp_font_catalog.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t gsp_font_catalog_find(const gsp_font_catalog_t *catalog, uint32_t asset_id, const void **out_data, size_t *out_size);
```

<details><summary>完整接口契约（英文）</summary>

Finds a GFB member by Asset ID (GSP_ERR_NOT_FOUND when absent).

</details>

### `gsp_renderer_profile_snapshot()`

读取当前渲染器 Profile 快照。

- **头文件:** `include/gsp/gsp_render_profile.h`
- **返回类型:** `void`

```c
void gsp_renderer_profile_snapshot(uint64_t *out_us, uint32_t *out_calls, size_t cap);
```

<details><summary>完整接口契约（英文）</summary>

Copies the per-opcode cumulative render time / call counters
(zeros unless the benchmark enables GSP_PROFILE_RENDER).
Index = opcode value.

</details>

### `gsp_esp_touch_interrupt_available()`

判断 ESP-IDF Touch 适配器能否使用中断。

- **头文件:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **返回类型:** `bool`

```c
static inline bool gsp_esp_touch_interrupt_available(esp_lcd_touch_handle_t handle);
```

### `gsp_esp_touch_init()`

初始化 ESP-IDF Touch 适配器。

- **头文件:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **返回类型:** `void`

```c
void gsp_esp_touch_init(gsp_esp_touch_t *touch, esp_lcd_touch_handle_t handle, bool interrupt_mode, bool interrupt_required, uint8_t release_confirm_polls, gsp_esp_touch_wake_from_isr_t external_wake_from_isr, void *external_wake_ctx);
```

### `gsp_esp_touch_start()`

启动ESP-IDF Touch 适配器。

- **头文件:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t gsp_esp_touch_start(gsp_esp_touch_t *touch, gsp_esp_touch_wake_from_isr_t wake_from_isr, void *wake_ctx);
```

### `gsp_esp_touch_stop()`

停止ESP-IDF Touch 适配器。

- **头文件:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **返回类型:** `void`

```c
void gsp_esp_touch_stop(gsp_esp_touch_t *touch);
```

### `gsp_esp_touch_poll_frame()`

轮询一帧多点触摸样本。

- **头文件:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t gsp_esp_touch_poll_frame(gsp_esp_touch_t *touch, gsp_platform_touch_frame_t *out_frame, bool *out_has_sample);
```

### `gsp_esp_touch_poll()`

轮询一个兼容单指触摸样本。

- **头文件:** `include/gsp/platform/esp_gsp_idf_touch.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t gsp_esp_touch_poll(gsp_esp_touch_t *touch, gsp_platform_pointer_t *out_pointer, bool *out_has_sample);
```
