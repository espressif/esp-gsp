# 问题排查

从最早失败的层级开始：组件解析、场景生成、生成 API 编译、Target 链接、运行时启动、
输入或视觉输出。后续症状通常不能准确定位更早的配置错误。

## 组件与工具环境

### `gsp_add_bundle: standalone GSPC is required`

推荐先安装工具链管理器，让 CMake 根据 `.gspc_version` 自动解析 GSPC：

```sh
python -m pip install -U esp-gsp-tools
idf.py build
```

IDF 工程根目录下的 `.gspc_version` 优先于组件包中的编译器版本标记。如需选择其他
编译器版本，可手动创建工程根目录标记。如果没有可用标记，
请显式设置 `GSPC_EXECUTABLE`。如果无法使用 `esp-gsp-tools`，请从
[ESP-GSP Releases](https://github.com/espressif/esp-gsp/releases) 下载匹配主机的资产，
校验发布校验值，再指定该可执行文件。

CMake 会执行 `gspc compatibility` 兼容性检查并验证生成格式、需求、配置契约和 GMD 运行时 ABI。不要复制
其他构建的生成文件绕过错误，应使用匹配的工具版本。当前值见[兼容性契约](../reference/compatibility.md)。

### 原生模拟器无法启动

从终端执行 `gsp_sim --capabilities` 以保留 Loader 错误。确认资产匹配主机平台，并只
安装发布说明列出的运行库。无法满足原生依赖时可使用浏览器/WASM 资产。显示硬件的配置步骤见[显示集成](display.md)。

### 构建使用了错误的 ESP-GSP 副本

通过正常 ESP-IDF 依赖流程清理过期的工程依赖状态，再重新配置。查看
`build/project_description.json` 中当前选中的组件路径，不要假设附近的组件目录一定
覆盖 Managed Component。

## 场景生成

### JSON 报告未知字段或控件

以当前组件版本的[场景编写参考](../reference/authoring.md)和[控件库](../components/index.md)
为准，选择该版本支持的字段。检查诊断给出的完整 JSON Path；
模板和组件配置会让出错叶子字段离根因较远。

### 找不到场景

默认模式要求至少存在一个 `PROJECT_DIR/scenes/*.json`。创建该目录，或在
`gsp_add_bundle(SCENES ...)` 中提供显式列表。构建日志会说明当前使用的模式。

### 场景超过容量

构建会把选中 Target 的容量传给编译器。对比诊断中的 Required 与 Configured：减少
同时存在的场景需求，或提高对应 Kconfig/运行时 Pool。List/Grid 按可见项加 Overscan
计算，不按数据集总量计算；动态 Template 用 `max_instances` 表示同时存活上限。

### 找不到图片或字体

资源路径相对于场景 JSON，不相对于工程根目录或 Build 目录。把资源放在场景附近或
使用正确相对路径。场景或引用资源变化后应自动触发 Bundle 重新生成。

## 生成 API

### 找不到预期辅助函数

1. 为对象添加稳定 `name`。
2. 确认所需属性是该控件的动态属性。
3. 需要事件判断函数时添加 `callback`。
4. 重新构建 Bundle。
5. 检查 `<symbol>_gsp.h` 及其包含的场景头文件。

使用生成头文件中的接口声明和编辑器补全。

### 旧的生成函数仍然存在

确认修改的 JSON 属于活动 `gsp_add_bundle()` 的 `SCENES`，且应用包含同一个 `SYMBOL`
的头文件。组件路径或 CMake 所有权发生变化后执行重新配置。

## 显示输出

### 场景与面板不匹配

同时检查：

- 场景 `w`、`h`；
- 旋转后的逻辑面板宽高；
- `gsp_add_bundle(PIXEL_FORMAT ...)`；
- 显示目标像素宽度和字节序；
- BSP 物理旋转与 Framebuffer 配置。

非方形屏幕物理旋转 90°/270° 时，逻辑宽高需要交换。应用缓冲区保持 ESP-GSP 原生
像素布局；面板需要字节交换时在 BSP/显示目标配置，不要预交换所有应用像素。

### 颜色交换或显示错误

ESP-GSP 运行时 Surface 使用小端 RGB565，或 RGB888 的 B、G、R 紧凑字节顺序。单独
确认面板 Wire Byte Order；需要大端 RGB565 的面板应在显示目标启用字节交换。

### 更新撕裂或只有局部刷新

确认 BSP 准确提供面板类别、Framebuffer 数量、TE 信号、Draw Buffer 内存与完成回调。
保持 `ESP_DISPLAY_PRESENT_MODE_AUTO`，只有真实测量支持时才覆盖。详见[显示集成](display.md)。

### Arc、线条或裁剪与之前不同

锁定同一场景、GSPC/组件版本、像素格式、显示 Profile 和 Target。先比较原生/WASM
预览的几何，再测试开发板。Arc 粗细、裁剪和加速差异应作为框架或显示链路回归处理，
不要用应用层覆盖掩盖。

### 升级后绑定文本的位置上移

左对齐的绑定 Label 在高度足够容纳两行时，从首帧起使用多行布局，即使初始内容只有一行。
这样初始文本与后续 `set_text()` 更新的位置保持一致。若需要在较大的控件中居中显示单行
标题，将 Label 高度设为一行，并调整文本框在控件内的位置。

## 状态与生命周期

### Setter 成功但屏幕尚未变化

ESP-IDF 上 Setter 是异步的；成功表示更新进入命令路径。只有测试、截图或其他确定性
边界需要观察已提交状态时，才调用 `esp_gsp_flush()`。

### Getter 返回旧值

Getter 读取已提交状态。另一个任务发出的 Setter 仍可能排队。保持单一应用状态拥有者，
或在确需确定性 Read-after-write 时使用低频 Flush。

### 停止超时或缓冲区仍被占用

从应用任务调用 `esp_gsp_stop()`，不要从框架回调调用。Canvas Stop 是异步的，释放其
上下文和已接受的借用帧前先 Flush。运行时图片按 COPY/BORROW/TAKE 模式等待对应释放
回调。停止超时后 Handle 仍归应用所有，可以重试。

## 输入

### 触控无响应

确认：

- 启动前已设置 `lcd.touch`；
- 触控坐标符合配置的逻辑方向；
- 目标可见且没有被后绘制对象覆盖；
- 子控件或视口没有合理捕获该手势；
- 显式 Interrupt 模式拥有可用且未占用的中断源。

AUTO 输入模式在中断不可用时回退轮询；显式 Interrupt 模式保持严格失败。

### Pinch 没有缩放图片

Pinch 由应用处理。注册 `esp_gsp_on_pinch()`，在 `ESP_GSP_PINCH_BEGIN` 返回 `true`
捕获手势，再把 `relative_scale_q16` 应用到生成的 Image Scale 函数。ESP-GSP 只报告
手势，不自动缩放或平移控件；`MAX_TOUCH_POINTS=1` 会禁用第二触点和 Pinch。

## List、Grid 与运行时媒体

### 循环 Wheel 滚到边界后停止或没有显示行

在 Wheel 上设置 `cyclic: true`。有限的 `items` 数组是逻辑数据集，并不限制界面可以
循环多少次。循环 Wheel 会生成 Bind Helper；只需用生成函数和空回调绑定一次，例如
`gsp_widget_wheel_city_bind(ui, NULL, NULL)`；运行时会自动提供编译好的条目文字并复用
Row Template，应用不需要处理 Slot ID。只有数据由应用动态维护时，才需要 Callback、
`set_total()` 和 `refresh()`。

条目会在运行时变化时，声明 `dynamic_items: true`。无论初始数组为空、较短还是超过
视口，都会生成同一套 `bind`、`set_total()`、`refresh()` API。绑定一次并保存 Handle。
使用编译好的条目文字时传 `NULL` 回调；由应用提供数据时使用回调，并通过
`font_charset` 声明可能出现的字符。

调试预编译 Runtime 时，先用相互匹配的组件、GSPC 与 Simulator 版本复现，并记录
`gspc compatibility` 与固件启动摘要。Simulator
日志面板可查看 Host 与 Runtime 诊断，API Bridge 也支持同一个生成式 Wheel Helper。
设备端可用 `esp_gsp_set_pointer_observer()` 确认手势路由前的映射按下/抬起样本，并用
`esp_gsp_render_error_stats()`、`esp_gsp_service_stats()` 查看渲染失败与服务队列工作量；
这些公开诊断不要求取得 Runtime 源码。

### List 绑定最终返回 `ESP_GSP_LIST_NONE`

每个逻辑 List/Wheel 在 UI 生命周期内只绑定一次并保存 Handle。不要在每次 Scene Ready
事件中重复绑定。总数变化时用生成的 `set_total()`，可见数据变化时用 `refresh()`。

### 复用行显示过期的异步内容

Binder 返回后不要保存 Row Handle。异步结果应关联 Row Token，并只在它仍标识同一个
List Instance 和 Item 时发布。只有不可变编码内容才能复用稳定图片 `cache_key`。

### 运行时图片在内存压力下为空

检查解码缓存预算、最大可分配连续块、活动 Dynamic Image Slot，以及图片缓存是否被
禁用。无 PSRAM 构建可能选择不支持运行时 PNG/JPEG 替换的有界区域解码路径。详见
[配置参考](../reference/configuration.md)与[媒体和应用数据](media-and-data.md)。

## 报告问题

报告问题时至少提供：

- ESP-GSP 组件版本和实际组件路径；
- ESP-IDF 版本与 `IDF_TARGET`；
- 场景文件、逻辑分辨率和像素格式；
- 面板接口、旋转、Framebuffer 数量和 TE 配置；
- 第一条编译器/运行时错误，而不只是后续级联信息；
- 问题是否在主机预览、Target 构建、真实开发板或人工视觉检查中复现。
