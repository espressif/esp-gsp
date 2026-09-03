# 媒体与应用数据

## 选择正确的数据路径

| 内容 | 推荐路径 | 适用场景 |
|---|---|---|
| 固定图片与字体 | 场景相对资源 | 图标、背景、固定文案 |
| GIF/APNG 动画素材 | 构建期导入 | 加载、状态与装饰动画 |
| 偶尔变化的编码图片 | 命名 Image setter | 头像、下载图片、缩略图 |
| 相机、视频、连续像素 | Canvas | 应用持续拥有像素生产节奏 |
| 大型一维数据 | List/Wheel/Message List | 按可见行复用模板 |
| 大型二维数据 | Grid | 按可见单元复用模板 |

GIF/APNG 是素材输入，构建时会转换为 GSP 资源；它们不是额外控件，也不是设备端
通用 GIF 解码器。

## 静态图片与字体

静态资源路径相对于场景 JSON。场景或引用资源变化时，Bundle 会随构建重新生成。

```json
{
  "type": "image",
  "parent": -1,
  "x": 24,
  "y": 24,
  "w": 64,
  "h": 64,
  "image": "assets/status.png",
  "codec": "auto"
}
```

编译字体会覆盖静态文本可达的字形。运行时文字包含构建期未知字形时，再添加
`DYNAMIC_FONT`：

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../scenes/chat.json"
    PIXEL_FORMAT rgb565
    DYNAMIC_FONT "../scenes/assets/NotoSansSC-Regular.otf")
```

动态字体会增加代码、Heap 和任务栈消耗，应按实际字符集和负载测量，不要为构建期
已经确定的文字默认启用。

## 动态文字与标量状态

给应用需要修改的对象设置稳定 `name`，重新构建后调用生成辅助函数：

```c
ESP_ERROR_CHECK(gsp_status_temperature_set_text(ui, "24.5 C"));
ESP_ERROR_CHECK(gsp_status_battery_set_value(ui, 82));
```

确切函数取决于控件类型和动态属性。多个标量必须原子可见时，使用公共批量属性 API，
不要依赖多个异步 Setter 的调用顺序。

## 运行时图片

为需要替换内容的 Image 设置稳定 `name`，构建后使用该场景头文件提供的图片 setter。
编码格式、目标能力、缓存策略和 COPY/BORROW/TAKE 所有权必须同时满足。发布新图片前
确认旧借用缓冲区是否仍被运行时引用。

通用接口提供 `esp_gsp_set_image()`、`esp_gsp_set_image_borrowed()`、
`esp_gsp_set_image_owned()` 与 `esp_gsp_set_image_ex()`；普通命名对象仍优先使用生成的
Image Setter。

图片缓存按解码后的同时可见内容消耗预算，不按素材文件大小计算。先使用默认策略；
只有诊断或实测表明预算不合适时再调整。

替换图片解码期间，当前图片继续显示；替换解码失败时仍保留当前图片。BORROW/TAKE
提交成功后，编码负载可能在该图片保持当前内容期间一直被引用，因为解码像素可以被
缓存逐出并再次解码。API 立即返回错误时所有权仍归调用方。

## Canvas

Canvas 适合外部生产者拥有的帧：相机、视频、软件生成图或高频栅格图。根据生产者选择：

- 提交整帧；
- 提交脏区；
- 在渲染任务中执行 Direct Draw。

Direct Draw 回调不得阻塞，也不得在内部等待另一个持有同一缓冲区的任务。

## List、Grid、Wheel 与 Message List

集合控件按可见窗口复用行或单元格，不为数据集每条记录创建一个对象：

1. JSON 定义行/单元模板与同时存活上限。
2. 应用数据源提供总数、可见记录及文字/图片。
3. 数据变化后调用对应 `refresh()` 或总数 setter。
4. 回调只读取稳定快照，不在框架任务中访问慢速存储或网络。

Slot 数量按同时可见内容、Overscan 和并发控件计算，而不是按数据库总记录数计算。

## 图片 Grid 与图库

在 Grid 行模板内的 Image 设置 `"dynamic_image": true`。Binder 只向可见和保留的
备用 Cell 发布内容，不会为数据集每项创建图片目标。

不可变编码内容跨 Cell 复用时，为 `esp_gsp_grid_cell_set_image_ex()` 提供稳定的非零
`cache_key`；内容字节改变时必须换 Key。`ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS`
按同时活动的可见 Cell 加 Overscan 计算，不按图库总数计算。

## Message List

聊天式、可变高度内容使用 `message_list`。消息存储留在应用中；追加、前插、替换或
刷新数据时通知框架，不重建场景结构。Row 发布运行在框架回调上下文，只发布已准备好
的数据；存储、网络和慢速解码交给应用任务。

## 容量与内存规则

- `ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES` 覆盖同时存活的模板实例和复用行。
- `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS` 覆盖同时活动的运行时图片目标。
- List Slot 和单行文字容量是有界配置，能够推导时会在场景生成阶段检查。
- `image_cache_bytes` 限制已解码图片缓存，不包含应用拥有的编码输入缓冲区。
- 禁用图片缓存会改变可用的运行时图片路径；无 PSRAM 目标调整前先读[配置参考](../reference/configuration.md)。

完整函数、回调和停止边界见[函数级 API 参考](../reference/api-functions.md)与
[生命周期](lifecycle.md)。
