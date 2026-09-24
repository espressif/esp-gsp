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

### 图片存储与内存

`raw`（也称 `store`）将原生像素保存在 Bundle 中；使用 `mmap_direct` 时，渲染器
直接读取这些像素，不计入解码缓存。压缩图片节省 Flash，整图解码仍需要像素内存。
240×240 RGB565 图片需要 115,200 字节，独立 A8 平面再增加 57,600 字节；
行对齐和 JPEG 解码对齐可能进一步增加用量。

启用图片缓存时，每张静态压缩位图都需要放入声明的缓存预算。多张图片可通过 LRU
淘汰共用缓存，总量可以超过预算，但频繁淘汰会增加解码开销。
仅对确需常驻的资源使用 `preload`。

内存较少的目标可以对支持的无损图片使用分区解码，无需保留每张完整图片。
在 profile 中设置 `image_cache_enabled: false`，或在 Bundle 编译时使用
`--default-disable-image-cache`。JPEG 使用整图缓存。自动编码会为分区解码模式下
需要适配或旋转的图片选择原生像素；显式压缩的变换图片需要启用缓存。
直接加载二进制时，运行时配置应使用相同缓存模式；Bundle API 头中的配置函数会
应用所选模式。

`gsp_add_bundle(... IMAGE_CACHE_BYTES 262144)` 同时设置运行时缓存预算和对应的
编译预算，也适用于显式 `PROFILE`。CLI 对应参数为 `--image-cache-bytes 262144`。
单独设置 Profile 的 `image_cache_budget_bytes` 只约束编译选择，运行时仍自动推导预算。
需要两端一致时传入 `IMAGE_CACHE_BYTES`。运行时自动预算还取决于可用堆及最大连续空闲块。
固件 Flash 体积与图片解码内存应分别检查。

启用图片缓存时，运行时会在首帧前同步准备场景中已编译的可见压缩图片，
再用剩余缓存预取隐藏图片。
隐藏图片未全部预取属于正常情况，只记录调试信息。若可见图片的解码工作集超过预算，
警告会给出预算、当前常驻字节和跳过数量；被跳过的可见图片首帧可能显示场景背景色
占位。单张图片超过预算时还会给出所需解码字节。仅在堆仍有余量时增大
`IMAGE_CACHE_BYTES`，也可缩小解码尺寸，或在适用时改用 `raw`／分区解码。

多场景 Bundle 共享编码后的资源。每个场景只注册自身的编译图片资源，包含隐藏
控件和模板。仅供其他场景使用的图片不参与该场景的启动准备和缓存容量估算。
重新编译现有 Bundle 即可应用按场景注册的资源范围。

manifest 和 execution 报告分别列出注册资源、场景绘制引用和初始可见图片：

- `largest_static_decode_bytes`：场景绘制引用中最大的静态图片解码量。
- `largest_registered_static_decode_bytes`：该场景注册资源中最大的静态图片解码量。
- `initial_visible_static_decode_bytes_estimate`：应用更新前，默认状态下与屏幕和视口
  相交的静态图片解码量合计，同一资源只计一次。

绘制引用包含所有页面和模板。初始估算会考虑默认选中页、关闭的 Drawer 和隐藏组。
缓存预解码与保留图片、应用创建的实例、运行时图片以及动画/矢量缓冲区需单独预算。
运行时可用 `esp_gsp_media_stats()` 查看缓存占用与峰值。
`runtime_peak_bytes` 用于运行时实测，编译报告中为 null。

建议使用可移植的编码名称 `auto`、`speed`、`size`、`raw`、`lossless` 和 `jpeg`。
使用独立安装的 GSPC 时，先运行 `gspc compatibility`，确认
`compiler_features.image_policy_modes` 包含所需策略；当前源码工具已支持这些值。
`auto` 是默认选择，按目标能力、缓存预算和压缩收益选 STORE、QOI/RLE 或硬件 JPEG。
`speed` 偏重运行开销：小图可用 STORE；压缩收益明显的大图可用 QOI/RLE；合适的图片
可用硬件 JPEG/JPEG+A8。`size` 比较可用编码的实际字节数，也可在软件解码目标上选
JPEG。JPEG 质量默认 85，可用 `quality`（1–100）调整。要求像素完全一致时用
`lossless`，或在 Profile 设置 `image_auto_allow_lossy: false`。

硬件 JPEG 要求图片宽、高至少 64 像素。RGB888 透明图还须按 16 像素对齐，
`auto` 和 `speed` 才会自动选择 JPEG+A8；`size` 与显式 `jpeg` 可处理未对齐尺寸，
但解码时可能使用 MCU 临时缓冲。`size` 会把该临时缓冲与解码图像一起计入
候选预算；实际内存峰值还受其他资源影响。JPEG+A8 的颜色有损，A8 透明度无损。
压缩选择不会自动缩小像素尺寸；可按资源报告比较最终编码字节数和解码预算。
编译资源的 `size` 不把 PNG 纳入候选：PNG 解码需要整张图片的解压临时缓冲，
且不能按区域解码；仅以编码文件大小比较会低估设备 RAM 峰值。PNG 仍可作为运行时输入。
编译和诊断也接受已有名称 `store`（`raw`）、`qoi`（`lossless`）、
`default`（`auto`）以及显式 `rle16`（不透明 RGB565）、`rle16_a8`（透明 RGB565）和
`rle32`（RGB888）。显式 RLE 与目标像素格式不匹配时编译报错。
ARGB8888 叠加 Profile 只存储原生像素；显式请求其他编码或使用 `store_scale`
会在编译时报错。
`cache_policy` 可与三种策略一起使用。`mmap_direct` 仅适用于最终选为 STORE 的图片；
压缩图片指定 `mmap_direct` 会在编译时报错。既有场景可为 STORE 声明 `preload` 或
`decode_lru`，但 STORE 仍直接映射，不会因此进入解码缓存；新场景无需为 STORE 指定这两项。
`hardware_jpeg` 保留兼容行为：有硬件解码器时使用 JPEG，否则使用无损编码；
它不等同于显式 `jpeg`。

### 阅读场景资源预算

当前 `*.execution.json` 合同为 schema version 2，包含当前 GSPC
输出的资源可达性和可为空的解码大小字段。消费者应根据
`schema_version` 选择 schema，而不要根据 GSPC 产品版本猜测报告格式。

`*.execution.json` 的 `resource_budget` 汇总场景资源：`resource_blob_bytes` 包含
资源块及其头部，`image_payload_bytes` 与 `font_blob_bytes` 分别统计图片载荷和字体。
这些值不是固件 Flash 总量：场景 GSB、Bundle 目录／对齐和运行时代码另计；共享资源
不能跨场景报告重复累加。

`resources[].source_path` 标识资源来源。`inclusion` 区分初始绘制、绘制／模板引用和
场景可寻址资源，描述的是编译后的引用关系，不代表实际准备或常驻状态。
`source_variants` 汇总同一来源的多个编码变体，方便检查尺寸与编码成本；不同变体
可能有必要用途，不能仅凭数量判定为冗余。

`initial_static_set_exceeds_cache_budget` 只比较初始静态解码估算与明确启用的缓存预算，
自动预算或关闭缓存时为 null。超过预算可能引起淘汰，不是实测 RAM 峰值，也不代表
必然分配失败；应结合 `esp_gsp_media_stats()` 再决定是否增加内存。
`requested_codec: "generated"` 表示缺少逐资源的原始编码声明信息，全局 profile
策略单独报告，不冒充用户对每张图片的选择。

### 编译式 SVG 图片

可从 [矢量图片示例](../../../examples/usage/widgets/image/vector.json) 开始。

可在 Image 的 `image` 字段中引用 `.svg`。
GSPC 将它编译为运行时绘制的曲线资源，并记录产物所需的二进制格式版本。
工具配套关系见[兼容性契约](../reference/compatibility.md)。

Carousel、Flip Card 和特效图片集合会在编译时将可导入的 SVG 素材栅格化，产物按位图的尺寸
和缓存规则使用。需要运行时曲线缩放、改色或形变时，使用命名 Image。

SVG 保留曲线，按目标尺寸栅格化。复用大尺寸矢量时，图片缓存预算需要覆盖可见工作集：
未染色缓存每像素占 4 字节，染色遮罩每像素占 1 字节。矢量缓存与已解码图片、在途
解码共享场景图片缓存预算；位图解码需要空间时优先回收矢量缓存。预算或分配失败时，
渲染继续使用有界临时分块。

图片命名后可沿用 `fit`、`rotation` 和
`scalable`。动态调整图片框时，将 `w`、`h` 声明为有界动态字段，并指定
`width`、`height` 等语义属性名，编译器会生成对应 setter。
改变宽高会改变绘制区域，`scale` 则在框内缩放，并受图片框裁剪。
宽高沿用框架现有的 0..100 有界属性插值，范围很宽时会出现像素尺寸量化。
建议按实际尺寸设置上下界；框内精细等比缩放使用 Q16.16 的 scale 接口。

可选 `tint` 将场景图片显示为单色轮廓，并生成 `set_tint()` 接口。
不设置 `tint` 时保留源颜色和透明度。模板中的 SVG 使用静态颜色和尺寸。

矢量导入支持路径、基本形状、纯色填充、填充规则、静态变换和编译期展开的描边。
准备素材时：

- 在素材编辑器中将文字和固定像素描边转为路径。
- 将绘制范围（包括描边和继承变换）保持在 SVG 视口内；越界诊断会指出元素及超出量。
- 嵌入图片、外部引用、渐变、滤镜、实际裁剪／蒙版和多次绘制的组透明度，可先导出为
  PNG 保留视觉效果，再作为位图引用。
- Image 对象的 `opacity` 保持为 255，透明效果使用源素材中的填充透明度。

编辑器元数据、非 SVG 命名空间内容和未使用的渐变定义可保留。单次绘制组的透明度
会合并到填充透明度；完全包含组内绘制范围的矩形裁剪可作为冗余裁剪移除。
其他未支持的绘制语义会给出诊断，可选择可导入的 `svg_element`，或使用准备好的位图。

`codec`、`store_scale`、`cache_policy` 用于位图输入。矢量素材通过源路径几何，以及
Image 的尺寸、旋转和缩放属性调整显示效果。

命名对象可设置 `svg_element` 选择源 SVG 中的元素或组 ID，不必拆分素材文件。
编译器保留继承变换，按该元素的绘制边界裁紧；场景中的 `x/y/w/h` 是裁紧后图片的
位置和尺寸，不再是原 SVG 整体视口。多个部分分别使用命名 Image，可以沿用位置、
旋转、显隐和颜色属性。重复 ID、空 ID 或没有可绘制内容的选择会明确报错。

组合素材建议使用 `svg_layout: "canvas"`：给各部分相同的 `x/y/w/h` 原画布矩形，
GSPC 自动定位并紧凑编码各部分，不需要手算 SVG 内部边界。运行时仍是普通 Image，
不额外保留整画布像素缓存。`x/y` 控制该部分画布位置；旋转和框内缩放作用于选中部分。
旋转/缩放按该部分的图片框裁剪。
此模式需要 `svg_element`、静态 `w/h` 和默认 `fit: "stretch"`；命中/样式范围仍是
声明的画布矩形，交互区域宜由父控件显式定义。模板图片使用 `content` 布局。
不设置该字段，或使用 `content`，保持原先按裁紧图片布局的行为。

素材导入前可运行 `gspc svg-info artwork.svg`，或加 `--element iris` 检查指定部分。
JSON 输出包括画布尺寸、元素 ID、含描边的标准化边界、路径/点数、缓存估算和不支持原因。
越界诊断会给出源文件、元素 ID（或未命名选择）、视口、选取边界、实际绘制边界及四边超出量。
整图不支持不等于每个命名部分都不支持；选择兼容部分时不会要求渲染无关的装饰组。
`supported` 表示源素材的几何内容能否编译。
默认最多列出 256 个元素，`elements_truncated` 标记截断；可用 `--element` 检查指定 ID。
它不验证场景选项、形变配对或最终目标预算，仍须编译实际场景。

`morph_to` 指定结束 SVG，`morph` 为 0..100 的初始进度，默认 0。
编译后生成 `set_morph()`、`animate_morph()` 和 `animate_morph_to()`；动画沿用现有
时长、缓动、中断和批量更新机制。参见[矢量动效示例](../../../examples/usage/widgets/image/vector_motion.json)。
运行时插值曲线控制点，不解析 XML，不展开完整位图帧序列。
起止 SVG 必须具有相同视口尺寸、绘制路径顺序、填充/描边展开结果结构和绘制颜色；
各路径须保持相同轮廓、段类型、起点和方向。复制同一素材并编辑节点位置，
可保持控制点的对应关系；编译器负责检查结构一致性。
选择元素时，起止状态共用联合边界，避免形变时自动裁紧导致图片跳动。
形变用于命名的场景 Image；模板图片使用静态几何。

Image 的矢量导入上限为 2 MiB、128 次填充／描边绘制、8192 个轮廓点，支持 RGB565/RGB888 场景。
绘制耗时和内存需求随图片尺寸及复杂度增长；连续缩放、旋转和形变应在目标设备上
测量。参见[内存配置](../reference/configuration.md)。

使用执行计划中的 `vector` 摘要检查素材复杂度和内存估算；该估算不代表应用总内存。
素材应自包含，通过 GSP 属性驱动动画，并将导入效果与参考图对照。

矢量应用需要额外的 Flash 和 RAM。框架会按需提高渲染任务栈（随附构建至少为
32 KiB），应用指定的更大栈仍然有效；规划内存时应计入这部分开销。
未启用动态字体的源码构建只链接轮廓光栅器；启用动态字体（包括预编译包）时仍会
保留完整字体引擎。比较固件体积时，应使用相同的构建模式和字体能力。

### 位图与字体

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

运行时文案的词汇已知时，可在场景或文本对象上设置 `font_charset_file`，
指向 `assets/ui-words.txt` 等 UTF-8 文案文件。路径相对于场景；对象路径覆盖场景路径。
GSPC 将文件字符与 `font_charset`、静态文本合并去重，忽略文件开头的 BOM 和 CR/LF 换行符。
修改文件会触发 Bundle 重建；生成的 `set_text()` 接口直接使用烘焙字形，无需启用 FreeType。

场景级 `font_max_bytes` 可指定正整数字节上限，检查外部链接前所有字体、字号生成的 GFB
资源总大小。超出时报告所需字节数，不删字、不降低画质。该预算不包含动态字体源文件、
字形排版数据和运行时缓存。现有资源报告列出各字体大小、字形数和字符集，编译诊断指出缺失码点。

编译字体会覆盖静态文本可达的字形。运行时文字包含构建期未知字形时，再添加
`DYNAMIC_FONT`：

```cmake
gsp_add_bundle(${COMPONENT_LIB}
    SCENES "../scenes/chat.json"
    PIXEL_FORMAT rgb565
    DYNAMIC_FONT "../scenes/assets/NotoSansSC-Regular.otf")
```

动态字体是可选链接功能，预编译组件同样按需启用。`DYNAMIC_FONT` 会自动启用；
在运行时自行提供字体 Blob 的应用需在 CMake 中调用 `gsp_enable_freetype()`。
仅使用 AOT 字体的应用不会链接完整字体引擎，矢量绘制所需的轮廓栅格化仍独立可用。

启用动态字体回退时，每个静态 GFB 最多包含 32768 个字形；超过时 UI 启动明确报错。
仅使用静态字库时，每个 GFB 最多包含 65535 个字形。

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

为需要替换内容的 Image 设置稳定的 `name`、`bind` 和 `bind_target: "resource"`，
构建后使用该场景头文件生成的 `set_image()`。显式指定目标可避免对象的背景色占用
该绑定；没有背景色和文字的 Image 可以省略 `bind_target`。
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

运行时图片的像素格式由编译期占位图决定，而不是由替换图片决定。不透明 PNG 可以替换
带透明度的占位图（alpha 按 255 处理）；RGBA PNG 只有在所有像素都不透明时才能替换
不透明占位图，含透明像素时会被拒绝。需要显示透明效果时，请使用带透明度的占位图。

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

绑定可见列表后即可调用 `esp_gsp_list_fling()`；命令会先激活列表再开始滚动。
前插历史消息时保持原消息 ID，并将插入条数传给 `esp_gsp_message_list_changed()`。
框架在调整滚动锚点的同时，将复用行重新绑定到对应消息。

将 `esp_gsp_message_source_t` 清零初始化，并设置 `struct_size`。默认
`flags = 0` 保留文本 hash 校验，即使漏更新 revision，也能检测文本变化。
不含 `flags` 字段的旧结构继续使用这一行为。

能可靠维护版本的数据源可以设置
`flags = ESP_GSP_MESSAGE_SOURCE_TRUST_REVISION`。当 `id`/`revision` 不变时，
框架直接复用文字测量结果，不再扫描该条文本。ID 应标识消息本身，而不是数组索引；
前插和重排时保持 ID，文本或装饰数据变化时更新 revision。若不同内容复用了同一组
ID/revision，画面可能保持旧内容。此模式仍读取每条消息的元数据，并非范围更新 API。

一次处理期间应保持数据源一致。可在渲染任务发布变化，或为读取方保留不可变快照；
revision 标志不负责同步后台生产者。返回的文本须至少保持有效到下一次 `get()`。
追加、前插通知和失败重试继续遵循 `esp_gsp_message_list_changed()` 的现有语义。

## 容量与内存规则

- `ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES` 覆盖同时存活的模板实例和复用行。
- `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS` 覆盖同时活动的运行时图片目标。
- List Slot 和单行文字容量是有界配置，能够推导时会在场景生成阶段检查。
- `image_cache_bytes` 限制已解码图片缓存，不包含应用拥有的编码输入缓冲区。
- 禁用图片缓存会改变可用的运行时图片路径；无 PSRAM 目标调整前先读[配置参考](../reference/configuration.md)。
  无缓存模式下更新运行时 QOI，应使用固定尺寸、`fit: "stretch"` 的占位图，
  并将替换图片编码为相同的原生尺寸。对编码格式的运行时替换图片做适配和缩放，需要先通过图片缓存获得解码像素。
  setter 返回成功只代表提交成功；通过 `esp_gsp_set_image_ex()` 的完成回调确认
  图片已发布，并诊断被拒绝的替换请求。

完整函数、回调和停止边界见[函数级 API 参考](../reference/api-functions.md)与
[生命周期](lifecycle.md)。


## JPEG 动画帧

GIF/APNG 的 `animation_codec` 可以选择：

| 值 | 编译行为 |
|---|---|
| `lossless` | 强制 QOI 增量帧；未校准目标的 `auto` 默认采用此策略 |
| `jpeg` | 完整 JPEG 序列帧，不因体积更大而退回 QOI |
| `hardware_jpeg` | 目标 profile 声明 `hardware_jpeg: true` 时使用 JPEG，否则使用 QOI |

```json
{"type":"image","name":"motion","x":0,"y":0,"w":256,"h":256,
 "image":"assets/motion.png","animation_codec":"hardware_jpeg","quality":85}
```

不透明动画编码为 JPEG；任一帧含透明度时，整段动画编码为 JPEG+A8，颜色有损、A8 无损，不会静默丢弃透明区域。GIF/APNG 先合成为完整画布帧，再编码，保留帧时长和循环次数。`max_fps` 仍可限制导入帧率。显式 `codec: "jpeg"` 也适用于动画；若同时提供 `animation_codec`，后者优先。使用 `animation_codec` 时，未写 `quality` 则使用 profile 的 `jpeg_quality`。

JPEG 模式使用整帧解码和整帧刷新，适合愿意用资源空间换取硬件解码机会的场景；稀疏变化的 UI 动画仍可能更适合 QOI 增量帧。`animation_frame_budget_bytes` 限制解码帧缓冲预算，编码后的 Flash 大小应查看资源报告。报告区分 `anim_qoi`、`anim_jpeg`、`anim_jpeg_a8`。

未指定 `animation_codec` 时，`size` 比较整段 QOI 差分与 JPEG/JPEG+A8 的字节数。
`auto` 和 `speed` 仅在 Profile 设置 `animation_speed_hardware_jpeg: true`、帧变化密集、
画面不透明且尺寸按 16 对齐时考虑硬件 JPEG。`auto` 还要求 JPEG 至少节省 20% 空间；
`speed` 允许最多增加 10%。这个板级设置默认关闭，应根据实际显示路径测量后启用。
在已验证的板级 Profile 中设置 `hardware_jpeg: true` 与
`animation_speed_hardware_jpeg: true`，即可让上述规则参与编译。
透明动画自动保留 QOI；需要 JPEG+A8 时显式设置 `animation_codec: hardware_jpeg`。
显式 `animation_codec` 始终优先。
动画使用独立帧缓冲；对动画设置静态图片的 `cache_policy` 或 `store_scale` 会报错。

硬件与软件 JPEG 解码器的色彩转换可能不同。图标、品牌色等要求颜色一致的 UI
素材应使用 `lossless`；JPEG+A8 的透明度无损，RGB 颜色使用有损压缩。

内置与文件系统图片省略 `quality` 时均使用 Profile 的 `jpeg_quality`，显式设置
也适用于自动选择 JPEG 的情况。Profile 不具备硬件 JPEG 能力时，`hardware_jpeg`
回退为无损编码。

JPEG 解码器按尺寸、布局和平台能力选择硬件或软件路径；较小图像可能走软件，MCU 未对齐时可能使用临时缓冲。硬件解码不等于整段播放必然更快，还应测量读帧、透明度处理和显示提交耗时。启用 JPEG 动画前须同步更新 GSPC 和运行时，旧运行时会拒绝新增的帧格式；默认 QOI 动画格式不变。

选型时可分别以 `lossless` 和 `jpeg` 编译，对比生成的 `*.execution.json`：
`resources[].encoded_bytes` 是编码大小，`animation.frame_bytes` 是单帧像素缓冲，
`animation.patch_ratio` 是差分像素占比。占比小通常更适合保留差分；接近 1 时再评估
硬件 JPEG。报告不包含实际 SD 吞吐或完整播放耗时，最终以目标板测量为准。

SD/NAND 中的预处理资源与逐帧动画见[外部资源](external-assets.md)。

SD 卡字体可用[字体文件接口](external-assets.md#sd-字体)加载，显式设置大小上限并管理其生命周期。

## 透明位图、预览与资源预算

RGB565+A8 和 ARGB8888 位图可使用 `fit`、`rotation` 及有界动态旋转，模板图片也适用。
变换按图片框裁剪，源图透明度参与混合；对象自身的 `opacity` 仍须为 255。
透明变换使用软件采样。`codec: raw` 不需要解码缓存；压缩资源须有
足够解码缓存，编码体积不能代替运行时像素内存预算。

相机或解码器推帧使用带 `bind` 的不透明 Image 占位对象，再调用
`esp_gsp_canvas_push()`／`esp_gsp_canvas_try_push()`；输入必须匹配声明的尺寸与目标
RGB565/RGB888 格式。至少准备双缓冲，并在释放回调后复用旧帧。偶尔替换图片使用
Image Setter；文件资源接入见[外部资源](external-assets.md)。

跨芯片场景优先 `codec: auto`；按资源侧重点可改用 `speed` 或 `size`。
例如 `{"type":"image","image":"assets/photo.png","w":240,"h":240,"codec":"size","quality":75}`
将这张图片的候选 JPEG 质量设为 75。显式 `jpeg` 是严格要求，不支持的目标会指出替代方式。
无 JPEG 目标可使用现有 QOI/RLE 无损编码，并对允许降低细节的图片显式配置
`store_scale`。缩小存储需要支持缩放的容器编码及足够缓存，不能作为无 RAM 成本的
Flash 优化。检查资源报告的图片、字体与 Bundle 字节数，对照应用分区预算；工具不会
静默降低图片分辨率。字体缺字报告列出完整码点集合，并为文本使用位置提供对象路径；
没有问号字形的自定义字体会烘焙可见替代框，避免显示不相关的第一个图标。

`store_scale < 1` 会自动选择适配声明图片框的 fit 绘制路径，无需额外开启 `scalable`。
`codec: auto` 或 `speed` 配合缩小存储采用 QOI；`size` 则比较缩小后的 QOI 与可用
JPEG。显式 `raw` 不支持此存储选项，会在编译期指出对象
路径和替代编码。普通图片与模板图片复用同一套编码选项解析。压缩缩略资源仍需要解码缓存。

迁移时优先使用原始 PNG 和 TTF/OTF。只有 LVGL C 数组或私有二进制时，必须先确定
LVGL 版本、像素格式、通道顺序、Stride、Alpha 是否预乘及压缩格式，再转换为编译器
接受的资源。32 位像素不能仅凭位数判断通道顺序；私有 `.bin` 也不是统一格式。

## 从固件中移除 JPEG

JPEG 解码默认开启，支持编译资源、运行时图片、外部资源包及硬件到软件的回退。
应用完全不使用 JPEG 时，可将对应解码代码从固件中裁掉：

```ini
CONFIG_ESP_GSP_ENABLE_JPEG=n
```

已有工程可在 `idf.py menuconfig` 的 **ESP-GSP** 菜单中关闭
**Link the JPEG decoder**；新工程可将该设置放入 `sdkconfig.defaults`。
该选项适用于源码和预编译组件，在链接阶段生效。组件构建文件与预编译库应来自
同一发布版本。

### 资源编译

`gsp_add_bundle()` 和 `gsp_add_assets()` 会向 GSPC 传入 `--disable-jpeg`。
显式 `PROFILE` 仍可使用：编译器排除 JPEG，保留其他能力与自定义预算。
若 profile 只允许 JPEG，则会报配置冲突；请声明所需的非 JPEG 编码或保留 JPEG。
独立编译资源时应传入同一参数。CMake 会在配置阶段检查编译器是否支持该能力，
并为不兼容的编译器给出更新方法。

- 静态图片可使用 `codec: auto` 选择无损编码。显式 `codec: "jpeg"` 会报
  `GSPC-RS-CAPABILITY`，也适用于透明图片派生的 `jpeg_a8` 输出。
- 动画可使用 `animation_codec: "lossless"`。显式 `animation_codec: "jpeg"`
  会报同类错误，仅修改 `codec` 不会覆盖它。关闭 JPEG 时，
  `animation_codec: "hardware_jpeg"` 会选择无损帧。

PNG、原生像素、支持的无损位图、编译式 SVG 和 QOI 动画仍可使用，容器探测也能
识别 JPEG。解码能力由配置明确选择，因此没有内置 JPEG 的应用仍可保留解码器，
用于日后从运行时输入的 JPEG 图片。

### 运行时图片与外部资源

已接受的 JPEG 运行时图片请求会在分配解码面前以 `GSP_ERR_UNSUPPORTED` 完成。
通过完成回调获取结果，缓冲区所有权和释放规则保持不变。由其他工程编译的 Bundle
或资源包在使用其 JPEG 资源时检查能力。编译式 JPEG 动画在打开时失败，外部 JPEG
逐帧流在分配帧画布前失败。

软件解码器依赖仍参与组件解析；应用中的其他组件使用 JPEG 时，相应代码仍可能
被链接。Host 和 WASM/WASI 模拟器保留自身的编码支持，预览成功并不代表关闭 JPEG
的设备固件能够显示相同的外部 JPEG 数据。

## 软件 JPEG 与缩放动画

ESP-IDF 在 ESP32-S3 等目标上已有 `esp_new_jpeg` 软件解码路径。
`gsp_add_bundle`/`gsp_add_assets` 分别识别软件依赖与硬件 JPEG 引擎；静态 JPEG
还要求可用的解码图片缓存。独立平台编译可通过 `--software-jpeg` 声明软件解码能力。
它放行显式 `codec: "jpeg"`，不会仅因软件解码器存在就让 `auto` 改选有损 JPEG。
有限缓存预算必须容纳含对齐、alpha 存储的解码后像素面；JPEG 节省 Flash，不节省
该像素内存。动画帧缓冲使用独立预算。

命名且可缩放的 Image 可用 `animate_scale_q16` 和 `animate_scale_q16_to` 设置缩放动画。
65536 表示 1×，131072 表示 2×；在图片所属场景调用，目标值使用声明的缩放范围。

## 替换图片

为场景图片添加资源 `bind`，例如 `name: "cover", bind: "cover_image"`，即可生成
替换接口。模板图片使用 `dynamic_image: true`。应用创建模板图片时，按同时活跃的
图片目标数配置 `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS`。

使用 `gsp_add_assets()` 构建预编译素材包，再通过目标图片的生成 `set_asset()` helper
显示素材。选择图片时使用生成的素材引用，其中包含接口所需的素材包信息。
