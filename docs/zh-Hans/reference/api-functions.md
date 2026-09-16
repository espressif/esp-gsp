# 公共函数参考

本页覆盖固件组件随包发布的应用层公共函数。普通应用先包含生成的
`<symbol>_gsp.h`；只有生成 API 无法表达的运行时能力才直接调用这些函数。

函数声明和返回类型由公共头文件提取，中文摘要说明应用用途；头文件仍是边界条件的最终契约。
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

### `esp_gsp_stop()`

停止并销毁平台创建的 UI 实例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_stop(esp_gsp_handle_t gsp);
```

### `esp_gsp_flush()`

等待此前提交的命令完成一次渲染尝试和显示提交。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_flush(esp_gsp_handle_t gsp, uint32_t timeout_ms);
```

### `esp_gsp_render_error_stats()`

读取累计渲染失败次数和最近一次引擎错误。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `void`

```c
void esp_gsp_render_error_stats(esp_gsp_handle_t gsp, uint32_t *out_failures, gsp_err_t *out_last_error);
```

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

### `esp_gsp_keyboard_text()`

复制当前键盘编辑缓冲区中的文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_text(esp_gsp_handle_t gsp, char *out_text, size_t capacity);
```

### `esp_gsp_keyboard_text_size()`

读取复制当前键盘文字所需的缓冲区大小。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_keyboard_text_size(esp_gsp_handle_t gsp, size_t *out_size);
```

### `esp_gsp_set_cursor()`

设置输入光标。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_cursor(esp_gsp_handle_t gsp, uint16_t bind);
```

## 命名控件

控件 API 使用生成的稳定对象键和属性键；应用仍应优先使用生成的类型化包装函数。

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

### `esp_gsp_component_set_properties()`

把一组开放属性键更新作为一个原子事务提交。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_properties(esp_gsp_handle_t gsp, const gsp_component_property_update_t *updates, size_t count);
```

### `esp_gsp_component_set_position()`

设置控件位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_position(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t x, int32_t y);
```

### `esp_gsp_component_get_position()`

读取控件位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_position(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t *out_x, int32_t *out_y);
```

### `esp_gsp_component_translate()`

相对移动一个已编译控件子树。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_translate(esp_gsp_handle_t gsp, gsp_component_key_t component, int32_t dx, int32_t dy);
```

### `esp_gsp_component_get_info()`

读取控件元数据。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_info(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_component_info_t *out_info);
```

### `esp_gsp_component_get_value()`

读取控件数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t *out_value);
```

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

### `esp_gsp_component_set_color()`

设置控件颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_color(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t native_color);
```

### `esp_gsp_component_set_color_rgb888()`

把 RGB888 色值转换为当前场景原生颜色并设置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t rgb888);
```

### `esp_gsp_component_set_property_color_rgb888()`

使用统一 RGB888 颜色设置指定颜色属性，自动转换为场景原生格式。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_set_property_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t component, gsp_property_key_t property, uint32_t rgb888);
```

### `esp_gsp_component_get_color_rgb888()`

读取控件原生颜色并转换为 RGB888。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_get_color_rgb888(esp_gsp_handle_t gsp, gsp_component_key_t key, uint32_t *out_rgb888);
```

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

## 导航与保留态容器

场景、PageFlow、StackView 和 Drawer 操作共用运行时的保留态导航与动画规则。

### `esp_gsp_page_flow_set_page()`

设置 PageFlow 当前页面。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_page_flow_set_page(esp_gsp_handle_t gsp, gsp_component_key_t key, uint16_t page, bool animated);
```

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

打开Drawer。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_drawer_open(esp_gsp_handle_t gsp, gsp_component_key_t key, bool animated);
```

### `esp_gsp_drawer_close()`

关闭Drawer。

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

### `esp_gsp_set_swipe_enabled()`

设置场景滑动开关。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_enabled(esp_gsp_handle_t gsp, bool enabled);
```

### `esp_gsp_set_swipe_transition()`

设置场景滑动效果。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_transition(esp_gsp_handle_t gsp, esp_gsp_swipe_transition_t transition);
```

### `esp_gsp_set_swipe_fade_black_point()`

设置滑动渐黑位置。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_swipe_fade_black_point(esp_gsp_handle_t gsp, uint8_t drag_percent);
```

## 运行时图片

COPY、BORROW 和 TAKE 变体具有不同的编码缓冲区所有权；释放或复用缓冲区前必须遵守函数契约。

### `esp_gsp_image_get_scale()`

读取图片缩放比例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_image_get_scale(esp_gsp_handle_t gsp, gsp_component_key_t image, uint32_t *out_scale_q16);
```

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

### `esp_gsp_set_image_ex()`

按照指定所有权和完成回调提交编码图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_ex(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size, const esp_gsp_image_options_t *options);
```

### `esp_gsp_set_image_borrowed()`

借用不可变编码图片，直到释放回调归还缓冲区。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_borrowed(esp_gsp_handle_t gsp, uint16_t bind, const void *data, size_t size, esp_gsp_image_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_set_image_owned()`

把 malloc 兼容的编码图片所有权转交给框架。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_image_owned(esp_gsp_handle_t gsp, uint16_t bind, void *data, size_t size);
```

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

### `esp_gsp_assets_show()`

异步显示资源包中的图片或逐帧播放动画；加载失败保留旧图。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, esp_gsp_asset_ref_t asset, bool once);
```

### `esp_gsp_assets_show_name()`

按清单名称异步显示资源，支持独立更新资源包中新增的素材。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, const char *name, bool once);
```

### `esp_gsp_assets_show_row()`

异步加载 Row 图片，校验复用行令牌。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_show_row(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t slot, esp_gsp_asset_ref_t asset, bool once);
```

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

### `esp_gsp_assets_status()`

读取目标最近的加载结果和是否仍在处理。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `gsp_err_t`

```c
gsp_err_t esp_gsp_assets_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind, bool *out_pending);
```

### `esp_gsp_assets_stop()`

停止目标的后续帧调度，保留已显示的图片。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_stop(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind);
```

### `esp_gsp_assets_close()`

最多等待五秒关闭资源包；超时保留有效句柄，恢复处理后重试。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_close(esp_gsp_assets_t *assets);
```

### `esp_gsp_assets_close_wait()`

在指定超时内关闭服务；超时后保留有效句柄供重试。

- **头文件:** `include/esp_gsp_assets.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_assets_close_wait(esp_gsp_assets_t *assets, uint32_t timeout_ms);
```

### `esp_gsp_font_file_open()`

在显式文件大小上限内加载动态字体或预编译字体目录包。

- **头文件:** `include/esp_gsp_font_file.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_font_file_open(const char *path, size_t max_bytes, esp_gsp_font_file_t **out_font);
```

### `esp_gsp_font_file_apply()`

将已加载字体应用到启动前的 UI 配置，保留另一类字体来源。

- **头文件:** `include/esp_gsp_font_file.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_font_file_apply(const esp_gsp_font_file_t *font, esp_gsp_config_t *config);
```

### `esp_gsp_font_file_close()`

在所有引用它的 UI 停止后释放字体文件及目录。

- **头文件:** `include/esp_gsp_font_file.h`
- **返回类型:** `void`

```c
void esp_gsp_font_file_close(esp_gsp_font_file_t *font);
```

## 事件、输入与覆盖层

回调必须遵守公共头文件标明的执行上下文；在渲染任务中执行的回调应保持短小且不阻塞。

### `esp_gsp_on_event()`

注册应用事件回调。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_event(esp_gsp_handle_t gsp, esp_gsp_event_cb_t cb, void *user_ctx);
```

### `esp_gsp_on_pinch()`

注册由应用决定是否接管的双点 Pinch 回调。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_on_pinch(esp_gsp_handle_t gsp, esp_gsp_pinch_cb_t cb, void *user_ctx);
```

### `esp_gsp_set_pointer_observer()`

注册映射后的指针采样观察器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_pointer_observer(esp_gsp_handle_t gsp, esp_gsp_pointer_observer_cb_t cb, void *user_ctx);
```

### `esp_gsp_set_overlay_contributor()`

设置覆盖层贡献者。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_overlay_contributor(esp_gsp_handle_t gsp, esp_gsp_overlay_contributor_cb_t cb, void *user_ctx);
```

### `esp_gsp_set_input_interceptor()`

设置输入拦截器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_input_interceptor(esp_gsp_handle_t gsp, esp_gsp_input_interceptor_cb_t cb, void *user_ctx);
```

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

## 列表、网格与消息

集合 API 只保留可见行或单元格；Binder 回调在渲染任务执行，回调结束后不得继续持有 Row Token。

### `esp_gsp_grid_bind_component()`

按稳定控件键绑定生成的 Grid。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_grid_t`

```c
esp_gsp_grid_t esp_gsp_grid_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, esp_gsp_grid_bind_cb_t bind_item, void *user_ctx);
```

### `esp_gsp_grid_cell_set_image()`

设置Grid单元格图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_grid_cell_set_image(esp_gsp_handle_t gsp, esp_gsp_grid_cell_t cell, const void *data, size_t size);
```

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

### `esp_gsp_row_set_text()`

设置行文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_text(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t text_slot, const char *utf8);
```

### `esp_gsp_row_value()`

设置回收行中的第一个数值 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_value(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t value);
```

### `esp_gsp_row_set_value()`

设置行数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_value(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t value_slot, uint32_t value);
```

### `esp_gsp_row_color()`

设置回收行中的第一个颜色 Slot。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_color(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint32_t color);
```

### `esp_gsp_row_set_color()`

设置行颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_color(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t color_slot, uint32_t color);
```

### `esp_gsp_row_set_image()`

设置行图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_row_set_image(esp_gsp_handle_t gsp, esp_gsp_row_t row, uint16_t resource_slot, const void *data, size_t size);
```

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

### `esp_gsp_bind_list()`

绑定List。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_bind_list(esp_gsp_handle_t gsp, uint16_t template_id, int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t total, esp_gsp_list_bind_cb_t bind_item, void *user_ctx);
```

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

### `esp_gsp_list_set_item_heights()`

原子替换可变高度列表的完整高度索引。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_set_item_heights(esp_gsp_handle_t gsp, esp_gsp_list_t list, const uint16_t *heights, uint32_t count, uint32_t prepended_count, bool stick_to_end);
```

### `esp_gsp_message_list_bind_component()`

绑定生成的 Message List 和应用数据源。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_list_t`

```c
esp_gsp_list_t esp_gsp_message_list_bind_component(esp_gsp_handle_t gsp, gsp_component_key_t key, const esp_gsp_message_source_t *source);
```

### `esp_gsp_message_list_changed()`

重新读取 Message List 数据源并更新布局。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_message_list_changed(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t prepended_count);
```

### `esp_gsp_list_snap()`

启用或关闭 List/Wheel 的行边界吸附。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_snap(esp_gsp_handle_t gsp, esp_gsp_list_t list, bool enable);
```

### `esp_gsp_list_fling()`

以指定像素速度启动列表惯性滚动。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_fling(esp_gsp_handle_t gsp, esp_gsp_list_t list, int32_t velocity_px_s);
```

### `esp_gsp_list_scroll_to()`

跳转到列表的绝对像素偏移。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_scroll_to(esp_gsp_handle_t gsp, esp_gsp_list_t list, int32_t offset_px);
```

### `esp_gsp_list_fade()`

设置 Wheel 风格的上下边缘淡出带。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_list_fade(esp_gsp_handle_t gsp, esp_gsp_list_t list, uint32_t native_color, bool enable);
```

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

### `esp_gsp_canvas_invalidate()`

请求重绘Canvas。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_invalidate(esp_gsp_handle_t gsp, uint16_t bind);
```

### `esp_gsp_canvas_invalidate_dirty()`

请求重绘Canvas脏区。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_invalidate_dirty(esp_gsp_handle_t gsp, uint16_t bind, gsp_rect_t dirty);
```

### `esp_gsp_canvas_push()`

发布一个借用的 Canvas 完整帧。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_push(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_try_push()`

以非阻塞方式尝试发布 Canvas 完整帧。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_try_push(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_push_dirty()`

发布借用帧并只重绘指定脏区。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_push_dirty(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_try_push_dirty()`

以非阻塞方式尝试发布 Canvas 脏区帧。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_try_push_dirty(esp_gsp_handle_t gsp, uint16_t bind, const void *pixels, size_t stride_bytes, gsp_rect_t dirty, esp_gsp_canvas_release_cb_t on_release, void *release_ctx);
```

### `esp_gsp_canvas_stop()`

停止Canvas。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_canvas_stop(esp_gsp_handle_t gsp, uint16_t bind);
```

## 定时器、模板与动画

运行时对象消耗有界资源池并在渲染任务应用；模板配额来自场景需求和工程覆盖值。

### `esp_gsp_scale_q16_from_percent()`

把整数百分比转换为无符号 Q16.16 缩放值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_from_percent(uint32_t percent);
```

### `esp_gsp_scale_q16_multiply()`

组合图片缩放值与相对 Pinch 缩放值，并进行饱和处理。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_multiply(uint32_t scale_q16, uint32_t relative_q16);
```

### `esp_gsp_scale_q16_clamp()`

把 Q16.16 缩放值限制在指定范围内。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `uint32_t`

```c
static inline uint32_t esp_gsp_scale_q16_clamp(uint32_t scale_q16, uint32_t minimum_q16, uint32_t maximum_q16);
```

### `esp_gsp_chart_set_series()`

按业务数值一次提交整条曲线，返回前复制输入数组。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_chart_set_series(esp_gsp_handle_t gsp, const esp_gsp_chart_series_t *series, const int32_t *values, size_t count);
```

### `esp_gsp_chart_append()`

丢弃最旧采样点并按顺序追加一个业务数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_chart_append(esp_gsp_handle_t gsp, const esp_gsp_chart_series_t *series, int32_t value);
```

### `esp_gsp_set_press_feedback_enabled()`

启用或关闭默认按下遮罩，不改变点击路由。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_set_press_feedback_enabled(esp_gsp_handle_t gsp, bool enabled);
```

### `esp_gsp_query_visibility()`

查询活动场景中对象布局与屏幕及父视口的有效可见交集。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_query_visibility(esp_gsp_handle_t gsp, const esp_gsp_visibility_target_t *target, bool *out_visible);
```

### `esp_gsp_timer_create()`

创建定时器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `void *`

```c
void *esp_gsp_timer_create(esp_gsp_handle_t gsp, uint32_t period_ms, esp_gsp_timer_cb_t cb, void *user_ctx);
```

### `esp_gsp_timer_delete()`

删除定时器。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_timer_delete(esp_gsp_handle_t gsp, void *timer);
```

### `esp_gsp_widget_create()`

在当前场景创建一个已声明模板的运行时实例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_widget_t`

```c
esp_gsp_widget_t esp_gsp_widget_create(esp_gsp_handle_t gsp, uint16_t template_id, int16_t x, int16_t y);
```

### `esp_gsp_widget_create_template()`

创建模板实例模板。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_widget_t`

```c
static inline esp_gsp_widget_t esp_gsp_widget_create_template(esp_gsp_handle_t gsp, const esp_gsp_template_desc_t *template_desc, int16_t x, int16_t y);
```

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

### `esp_gsp_widget_set_color()`

设置模板实例颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_color(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, uint32_t color);
```

### `esp_gsp_widget_set_text()`

设置模板实例文字。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_text(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t slot, const char *utf8);
```

### `esp_gsp_widget_set_image()`

设置模板实例图片。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_set_image(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, uint16_t resource_slot, const void *data, size_t size);
```

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

### `esp_gsp_animate()`

执行动画。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_animate(esp_gsp_handle_t gsp, uint16_t bind, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_animate_color()`

执行动画颜色。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_animate_color(esp_gsp_handle_t gsp, uint16_t bind, uint32_t from, uint32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

### `esp_gsp_component_animate_value()`

执行动画控件数值。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_component_animate_value(esp_gsp_handle_t gsp, gsp_component_key_t key, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

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

### `esp_gsp_widget_animate()`

执行动画模板实例。

- **头文件:** `include/esp_gsp.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_widget_animate(esp_gsp_handle_t gsp, esp_gsp_widget_t widget, int32_t from, int32_t to, uint32_t duration_ms, esp_gsp_ease_t ease);
```

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

### `esp_gsp_esp_lcd_resume_paused()`

恢复 Pause Token 对应的同一个 UI。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_resume_paused(esp_gsp_esp_lcd_pause_t *pause, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_suspend()`

挂起 UI，同时保留场景和控件状态。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_suspend(esp_gsp_handle_t hub, esp_gsp_esp_lcd_session_t **out_session);
```

### `esp_gsp_esp_lcd_start_on_session()`

在挂起 Hub 的显示宿主上启动应用 Bundle。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start_on_session(esp_gsp_esp_lcd_session_t *session, const esp_gsp_config_t *app_config, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_start_on_session_prepared()`

启动应用，并在第一帧前由驻留渲染任务执行准备回调。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start_on_session_prepared(esp_gsp_esp_lcd_session_t *session, const esp_gsp_config_t *app_config, esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_replace_on_session_prepared()`

替换临时应用，期间不重新显示 Hub。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_replace_on_session_prepared(esp_gsp_esp_lcd_session_t *session, esp_gsp_handle_t current_app, const esp_gsp_config_t *next_config, esp_gsp_esp_lcd_prepare_cb_t prepare, void *prepare_ctx, esp_gsp_handle_t *out_gsp);
```

### `esp_gsp_esp_lcd_resume()`

停止临时应用并恢复保留的 Hub。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_resume(esp_gsp_esp_lcd_session_t *session, esp_gsp_handle_t app, esp_gsp_handle_t *out_hub);
```

### `esp_gsp_esp_lcd_session_destroy()`

销毁没有活动临时应用的挂起 Session。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_session_destroy(esp_gsp_esp_lcd_session_t *session);
```

### `esp_gsp_esp_lcd_start()`

创建 ESP-LCD 后端并启动 UI。

- **头文件:** `include/esp_gsp_esp_lcd.h`
- **返回类型:** `esp_err_t`

```c
esp_err_t esp_gsp_esp_lcd_start(const esp_gsp_config_t *app_config, const esp_gsp_esp_lcd_config_t *esp_config, esp_gsp_handle_t *out_gsp);
```

## Deployable Bundle

Deployable 元数据从借用的 GSPB 字节解析；关闭元数据句柄之前，这些字节必须保持有效。

### `esp_gsp_deployable_bundle_open()`

解析自描述 Deployable GSPB 元数据。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_open(const void *data, size_t size, bool verify_crc, esp_gsp_deployable_bundle_t **out_bundle);
```

### `esp_gsp_deployable_bundle_get_info()`

读取 Deployable Bundle 的尺寸、像素格式和场景信息。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_get_info(const esp_gsp_deployable_bundle_t *bundle, esp_gsp_deployable_info_t *out_info);
```

### `esp_gsp_deployable_bundle_make_config()`

从 Deployable Bundle 元数据构造运行时配置。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_make_config(const esp_gsp_deployable_bundle_t *bundle, esp_gsp_config_t *out_config);
```

### `esp_gsp_deployable_bundle_action_key()`

把场景内数字 CALL ID 解析为稳定动作键。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `esp_gsp_err_t`

```c
esp_gsp_err_t esp_gsp_deployable_bundle_action_key(const esp_gsp_deployable_bundle_t *bundle, uint16_t scene_id, uint16_t action_id, uint32_t *out_key);
```

### `esp_gsp_deployable_bundle_close()`

释放解析后的 Deployable 元数据。

- **头文件:** `include/esp_gsp_deployable.h`
- **返回类型:** `void`

```c
void esp_gsp_deployable_bundle_close(esp_gsp_deployable_bundle_t *bundle);
```

## 诊断

诊断计数器用于分析渲染、输入和资源使用；产品状态由应用代码维护。

### `esp_gsp_heap_stats()`

按需读取设备内部 RAM 与 PSRAM 的空闲字节和最大连续块；不支持时返回 false 并清零。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `bool`

```c
bool esp_gsp_heap_stats(esp_gsp_heap_stats_t *out_stats);
```

### `esp_gsp_frame_count()`

读取启动后的非空闲渲染帧数。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `uint32_t`

```c
uint32_t esp_gsp_frame_count(esp_gsp_handle_t gsp);
```

### `esp_gsp_render_stats()`

读取累计渲染帧数和忙碌时间。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_render_stats(esp_gsp_handle_t gsp, uint32_t *out_frames, uint64_t *out_busy_us);
```

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

### `esp_gsp_service_stats()`

读取渲染任务服务循环统计。

- **头文件:** `include/esp_gsp_debug.h`
- **返回类型:** `void`

```c
void esp_gsp_service_stats(esp_gsp_handle_t gsp, uint32_t *out_iterations, uint64_t *out_service_us, uint32_t *out_commands);
```

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

## 高级集成

高级入口用于定制渲染集成，可访问底层渲染状态。

### `esp_gsp_context()`

取得当前底层渲染上下文。

- **头文件:** `include/esp_gsp_advanced.h`
- **返回类型:** `gsp_context_t *`

```c
gsp_context_t *esp_gsp_context(esp_gsp_handle_t gsp);
```

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

### `gsp_app_feed_touch()`

向可移植应用输入一个多点触摸 Contact。

- **头文件:** `include/gsp/gsp_app.h`
- **返回类型:** `void`

```c
void gsp_app_feed_touch(gsp_app_t *app, int32_t id, int32_t x, int32_t y, bool pressed);
```

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

### `gsp_drawer_closed_offset()`

计算 Drawer 靠指定边缘关闭时的偏移。

- **头文件:** `include/gsp/gsp_component.h`
- **返回类型:** `int32_t`

```c
static inline int32_t gsp_drawer_closed_offset(const gsp_drawer_config_t *config);
```

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

### `gsp_renderer_profile_snapshot()`

读取当前渲染器 Profile 快照。

- **头文件:** `include/gsp/gsp_render_profile.h`
- **返回类型:** `void`

```c
void gsp_renderer_profile_snapshot(uint64_t *out_us, uint32_t *out_calls, size_t cap);
```

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
