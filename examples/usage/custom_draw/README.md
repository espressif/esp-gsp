# ESP-GSP Custom Draw

This example renders a CPU-generated waveform into a named Canvas image. The
draw callback writes only the supplied native RGB565 or RGB888 region. A
render-task timer advances the phase and invalidates the Canvas; the draw
callback never changes GSP state and never logs. The waveform uses a hard
pixel threshold with no antialiasing, so diagonal edges can appear stepped.

The example also installs the per-instance extension observer and defines all
six global extension hooks. The observer records the tagged CHECK and SET_DRAW
results, subscribing only to those operations with event/operation masks.
Global hooks remain independent and still count all Canvas operations.
`gsp_add_hooks()` compiles the hook source in the application target and retains
it with an automatically generated strong anchor; no manual linker option is
needed. The application task logs sparse atomic diagnostics, including
whether the Canvas installation was actually accepted by the UI core. A
successful esp_gsp_esp_lcd_start_prepared() call proves runtime startup; it
does not certify prepare submissions or Canvas admission.

The example requires the public esp_gsp_extension.h header, the prepared start
API in esp_gsp_esp_lcd.h, and the generated bundle_gsp.h.


From this example directory, build the selected target:

```sh
idf.py -B build_esp32s3 -D SDKCONFIG="$PWD/build_esp32s3/sdkconfig" \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32s3" \
  set-target esp32s3 build
```

For native RGB888, use a MIPI-DSI or 24-bit RGB profile:

```sh
idf.py --preview -B build_rgb888 -D SDKCONFIG="$PWD/build_rgb888/sdkconfig" \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32s31_rgb888" \
  -D GSP_CUSTOM_DRAW_RGB888=ON \
  set-target esp32s31 build
```

The scene is configured in the build directory. The opaque 200x100 placeholder
supplies native dimensions and belongs to this example's only Image target.
The renderer owns temporary destination pixels; the
application owns drawing state and uses one producer per bind. Direct draw runs
on the render task, so it must not block, log, call setters, flush, stop, or
retain the surface pointer. The extension observer is observation-only; handle
follow-up work in the application task.

## ESP-GSP 自定义绘制

这个示例把 CPU 生成的波形绘制到命名 Canvas 图片中。波形采用无抗锯齿的逐像素阈值绘制，斜边可能呈阶梯状。绘制回调只写入框架
提供的 RGB565/RGB888 原生区域；渲染任务定时器推进相位并请求重绘，绘制回调
本身不修改 GSP 状态，也不输出日志。

示例安装实例级 extension observer，并定义六个全局 extension hook。observer
记录带 request ID 的 CHECK 和 SET_DRAW 结果；实例观察者只订阅这两种操作，
全局钩子仍独立统计全部 Canvas 操作。
`gsp_add_hooks()` 在应用目标内编译钩子源码，并自动生成保留用的强锚点，无需手写链接选项。
esp_gsp_esp_lcd_start_prepared() 成功只表示运行时启动成功，不能据此宣称 prepare 中的
提交全部成功或 Canvas 已被接纳。

使用与当前组件配套的 GSPC，并确保包含公开的
esp_gsp_extension.h、prepared start API 和同一次编译生成的 bundle_gsp.h。
scene 在构建目录中配置；200x100 不透明占位图提供原生尺寸，仅供示例中的一个 Image
目标使用。Canvas 回调运行在渲染任务，
不能阻塞、打印日志、调用状态修改 API、flush/stop 或保留 surface 指针；后续工作交给应用任务。
