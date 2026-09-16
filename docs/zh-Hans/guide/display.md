# 显示集成

## BSP 的职责

ESP-GSP 消费 `esp_display_present_target_config_t`，不会猜测面板时序、总线引脚、物理旋转、Framebuffer 所有权或字节序。这些值由 BSP 配置，并提供逻辑尺寸与编译场景一致的显示目标。

例程覆盖 SPI、QSPI、RGB 和 MIPI-DSI 等显示链路。使用时根据开发板原理图配置引脚、时序和面板参数。

## 面板类别

帧内存位置决定 Presenter 策略：

| 面板类别 | 帧内存 | AUTO 行为 |
|---|---|---|
| RGB / MIPI-DSI | 面板持续扫描 Host Framebuffer | 三个 Buffer 时三缓冲局部渲染；否则双缓冲 Direct 回退 |
| 带 TE 的 SPI / QSPI | 面板内部 GRAM | 脏区传输与 TE 同步 |
| 不带 TE 的 SPI / QSPI | 面板内部 GRAM | 自由运行的脏区传输 |

应用应准确描述硬件并让 Presenter 选择策略。不能根据 SoC 推断面板接口，同一个 SoC
可以搭配多种显示链路。

## 像素格式与分辨率

`gsp_add_bundle()` 默认 RGB565。只有真实显示/Presenter 链路使用 RGB888 时才选择 RGB888：

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../scenes/dashboard.json"
    PIXEL_FORMAT rgb888)
```

场景尺寸是 BSP 选定旋转后的逻辑尺寸。在开发板上检查画面方向和触控坐标映射。

## 触控

可传入 `esp_lcd_touch_handle_t`。AUTO 模式在中断可用时采用中断，否则轮询；强制中断模式在无有效中断链路时明确失败。触控坐标应由 BSP 转换到逻辑显示方向。

## 内存受限的 GRAM 面板

分区渲染通过 `target.drawbuf` 共用一个 Draw Buffer Pool。`lines` 控制条带高度，零值
从 32 KiB 默认预算推导；`buffers` 对自由运行 GRAM 默认两个，对 TE 同步和 Framebuffer
修复路径默认一个。产品需要不同内存上限时显式设置 `lines`，只有传输链路能消费
PSRAM 时才设置 `in_psram`。条带高度超过逻辑屏幕或 Buffer 数超过两个会被拒绝。

RGB/MIPI-DSI 使用 Framebuffer；`target.drawbuf` 只影响其可选修复路径分区缓冲区。

## 旋转

场景始终使用逻辑坐标。Presenter 根据目标旋转把逻辑脏区和像素映射到物理面板。
场景分辨率必须匹配旋转后的逻辑方向；非方形屏幕旋转 90°/270° 时宽高需要交换。

## 提交与硬件加速

普通场景使用 `ESP_DISPLAY_PRESENT_MODE_AUTO`，运行时会选择脏区渲染、硬件加速或软件路径。需要调优时，在目标板上比较帧生成和显示提交耗时，再调整显示策略。

## 转场

场景转场通常保留快照。内存受限时可在启动前用 `esp_gsp_config_set()` 将
`ESP_GSP_FIELD_ENABLE_TRANSITION_SNAPSHOTS` 设为零，请求低内存路径。Slide 和
Fade-through-black 可在显示契约允许时直接按分区渲染；Cross-fade 没有两份快照时
回退为零快照 Fade-through-black，但最终场景状态不能丢失。

## 渲染与 SDK 配套

绘制优化自动生效，包括原生 RGB888 图片透明度处理，无需新增应用 API。
比较性能时，分别检查帧生成和显示提交耗时。

P4 使用预编译库时，应在 ESP-IDF 中设置与开发板一致的芯片修订版本。
`CONFIG_ESP32P4_SELECTS_REV_LESS_V3` 选择 v3 之前的库，其他 P4 配置选择 v3 及之后
的库；修改修订版本后重新配置并构建。DMA2D 适配代码随消费端 ESP-IDF 编译，
使驱动配置结构与所用 SDK 保持一致。

## 所有权边界

应用/BSP 拥有硬件描述和产品策略，ESP-GSP 拥有逻辑脏区、命令回放、转场和帧规划，
Presenter 拥有目标分类、Framebuffer 一致性、物理变换、Cache 同步、TE/DMA/PPA
传输和完成栅栏。应用与框架不得绕过 Presenter 重复注册 Panel Callback 或自行修复
Framebuffer；自定义板级集成只需提供一个准确的显示目标。

## 检查显示效果

先验证准确板级 Profile，再分别观察颜色通道、旋转、裁剪、撕裂、转场、Arc/线条粗细和触控边缘。导航行为见[导航、视口与手势](navigation.md)。
