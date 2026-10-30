# 媒体与应用数据

## 选择正确的数据路径

| 内容类型 | 推荐路径 | 典型场景 |
|---|---|---|
| 固定图片与字体 | 场景相对资源 | 图标、背景、固定文案 |
| GIF/APNG 动画、本地 MP4/AVI 片段 | 构建期导入 | 加载动画、状态指示、装饰动效 |
| 偶尔变化的编码图片 | 命名 Image setter | 头像、下载图片、缩略图 |
| 相机、视频、连续像素 | Canvas | 应用持续拥有像素生产节奏 |
| 大型一维数据 | List/Wheel/Message List | 按可见行复用模板 |
| 大型二维数据 | Grid | 按可见单元复用模板 |

> GIF/APNG 是素材输入格式——构建时会转换为 GSP 内部资源。它们不是额外的控件类型，
> 也不是设备端的通用 GIF 解码器。

## 静态图片与字体

### 图片存储与内存

**`raw`（又称 `store`）** 将原生像素保存在 Bundle 中。使用 `mmap_direct` 时，
渲染器直接读取这些像素，不计入解码缓存。压缩图片节省 Flash，但整图解码仍需要
像素内存。

> 举例：240×240 RGB565 图片需要 115,200 字节，独立 A8 平面再增加 57,600 字节。
> 行对齐和 JPEG 解码对齐可能进一步增加用量。

启用图片缓存时，每张静态压缩位图都需要放入声明的缓存预算。多张图片可通过 LRU
淘汰共用缓存——总量可以超过预算，但频繁淘汰会增加解码开销。仅对确需常驻的资源
使用 `preload`。

内存较少的目标可以对支持的无损图片使用**分区解码**，无需保留每张完整图片。在
Profile 中设置 `image_cache_enabled: false`，或在 Bundle 编译时使用
`--default-disable-image-cache`。JPEG 使用整图缓存。`codec: "auto"` 或省略 codec
时，分区解码模式下需要适配或旋转的图片会选择原生像素。压缩的变换图片需要启用缓存。直接加载二进制
时，运行时配置应使用相同缓存模式。Bundle API 头中的配置函数会应用所选模式。

### 缓存预算设置

`gsp_add_bundle(... IMAGE_CACHE_BYTES 262144)` 同时设置运行时缓存预算和对应的
编译预算，也适用于显式 `PROFILE`。CLI 对应参数为 `--image-cache-bytes 262144`。

单独设置 Profile 的 `image_cache_budget_bytes` 只约束编译选择，运行时仍自动推导
预算。需要两端一致时传入 `IMAGE_CACHE_BYTES`。运行时自动预算还取决于可用堆及最大
连续空闲块。固件 Flash 体积与图片解码内存应分别检查。

运行时值既用于各场景缓存，也作为场景缓存的总保留目标。准备目标场景时，当前与
目标场景暂不参与淘汰，因此两者像素可能暂时超过该目标。收紧预算可减少常驻缓存，
但可能增加重复解码，且切页峰值不会必然同比下降。显示缓冲、任务栈和解码工作区
仍需单独预算。

RGB/DSI 示例提供 `CONFIG_EXAMPLE_LCD_FRAME_BUFFERS`（2 或 3，默认 3）。辅助层会
将两个 driver 缓冲与 `DOUBLE_PARTIAL` 配套，三个缓冲则保留 `AUTO`。两个缓冲可少占
一份 `宽 × 高 × 原生色深字节数`，但可能增加缓冲等待，应按工作负载验证后选择。
SPI/QSPI 面板的 GRAM 不受影响；这些缓冲不计入 `IMAGE_CACHE_BYTES`。

### 按工作负载选择缓存与输入所有权

`cache_key` 复用的是解码结果。图片缓存关闭时，QOI/RLE 仍按渲染区域解码；
完整帧解码计数为 0 不表示没有解码工作。COPY 即使命中解码缓存，也会复制本次编码
输入。对整个使用期都有效的 Flash 常量等不可变资源，可以使用 BORROW 配合必需的
`on_release`，避免输入复制；不能把临时接收缓冲直接改为 BORROW。

C3 的低内存 Bundle 配置通常关闭图片缓存。需要频繁 tint 或重复使用少量压缩图片时，
可在生成的配置函数之后显式启用，并按实际解码尺寸、同时在途图片和最大连续空闲块
确定预算。例如，下面的 48 KiB 是一组 96×96 矢量、80×60 图片工作负载的已测配置，
不是所有 C3 应用的默认值：

```c
esp_gsp_config_t config = gsp_bundle_config();
ESP_ERROR_CHECK(esp_gsp_config_set(&config, ESP_GSP_FIELD_ENABLE_IMAGE_CACHE, 1)
                == ESP_GSP_CONFIG_SET_OK ? ESP_OK : ESP_ERR_INVALID_ARG);
config.image_cache_bytes = 48U * 1024U;
```

缓存可以减少重复解码，也可能增加解码任务及缓存条目元数据。图片追踪载荷峰值
不能代替总 RAM 增量。一次性新图片可能因队列、发布和
完整帧分配而更慢。减少预算后应同时检查首个请求、P95、P99/最大值、缓存重新解码、
页面显隐与全刷，以及停止后的释放，不能只比较热循环 P95。编译占位资源可能被多个
控件共享；替换一个动态目标不应主动淘汰这类编译资源，其释放仍由缓存预算与场景
生命周期管理。

### 启动时的图片准备

启用图片缓存时，运行时会在首帧前同步准备场景中已编译的可见压缩图片，再用剩余
缓存预取隐藏图片。

- **隐藏图片未全部预取是正常的**，只记录调试信息。
- 若可见图片的解码工作集超过预算，警告会给出预算、当前常驻字节和跳过数量。被跳过
  的可见图片首帧可能显示场景背景色占位。
- 单张图片超过预算时还会给出所需解码字节。
- 仅在堆仍有余量时增大 `IMAGE_CACHE_BYTES`，也可缩小解码尺寸，或改用 `raw` /
  分区解码。

### 多场景资源共享

多场景 Bundle 共享编码后的资源。每个场景只注册自身的编译图片资源（包含隐藏控件和
模板）。仅供其他场景使用的图片不参与该场景的启动准备和缓存容量估算。重新编译现有
Bundle 即可应用按场景注册的资源范围。

manifest 和 execution 报告分别列出注册资源、场景绘制引用和初始可见图片：

- `largest_static_decode_bytes` — 场景绘制引用中最大的静态图片解码量。
- `largest_registered_static_decode_bytes` — 该场景注册资源中最大的静态图片解码量。
- `initial_visible_static_decode_bytes_estimate` — 应用更新前，默认状态下与屏幕
  和视口相交的静态图片解码量合计（同一资源只计一次）。

绘制引用包含所有页面和模板。初始估算考虑默认选中页、关闭的 Drawer 和隐藏组。
缓存预解码与保留图片、应用创建的实例、运行时图片以及动画/矢量缓冲区需单独预算。
运行时可用 `esp_gsp_media_stats()` 查看缓存占用与峰值。`runtime_peak_bytes` 保留给
运行时测量，在仅编译的报告中为 null。`cache_peak_bytes` 是当前存活场景缓存各自
生命周期峰值之和，属于保守上界，不是同时发生的进程峰值。

`esp_gsp_debug.h` 的 `esp_gsp_image_memory_stats()` 另提供进程级跟踪快照：默认解码
表面、COPY/TAKE 输入、缩放暂存和 PNG/JPEG scratch 的当前值、峰值及内部 RAM/PSRAM
分类。`total.peak_bytes` 是同时高水位；借用输入、自定义缓存分配器、解码库私有内存、
动画画布、区域解码缓冲、显示缓冲、任务栈和分配器开销不在其中，不能作为整机 RAM 总量。

### 编码策略选择

建议使用可移植的编码名称 `auto`、`speed`、`size`、`raw`、`lossless` 和 `jpeg`。
使用独立安装的 GSPC 时，先运行 `gspc compatibility`，确认
`compiler_features.image_policy_modes` 包含所需策略。

| 编码 | 行为 |
|---|---|
| `auto`（默认） | 按目标能力、缓存预算和压缩收益选 STORE、QOI/RLE 或硬件 JPEG |
| `speed` | 小图可用 STORE，压缩收益明显的大图用 QOI/RLE，合适的图片用硬件 JPEG/JPEG+A8 |
| `size` | 比较可用编码的实际字节数，也可在软件解码目标上选 JPEG |
| `raw` | 原生像素 |
| `lossless` | 无损 QOI |
| `jpeg` | 显式 JPEG/JPEG+A8 |

`codec: "auto"` 或省略 codec 时使用 Profile 的 `image_codec` 策略；对象上的
`speed` 和 `size` 可覆盖该策略。上表的自动选择对应 Profile `image_codec: auto`。
JPEG 质量默认 85，可用 `quality`（1–100）调整。要求保留缩放、目标像素格式转换后的
像素时用 `lossless`，或在 Profile 设置 `image_auto_allow_lossy: false`；RGB565
量化仍然存在。

### RGB565 渐变色阶

图片和编译动图可设置 `"dither": true`，减轻 RGB888 转 RGB565 时产生的可见色阶。
默认关闭；Profile 的 `image_dither: true` 可批量开启，单张图片的 `dither: false`
可关闭。使用 4×4 有序抖动，只处理平滑的不透明区域；纯色、锐利边缘、透明像素和
已有 RGB565 精度的颜色不额外加噪。RGB888/ARGB8888 跳过此处理。

处理位置由最终编码决定，与源文件是 PNG 还是 JPEG 无关：原生像素、无损编码和编译动图
在缩放后离线处理；不透明静态 JPEG 在解码、目标尺寸处理后，再抖动量化到 RGB565。
解码缓存命中时直接复用处理结果，无需每帧重做。

希望控制体积时可用 `"codec": "size"`。它比较可用编码的实际大小；缓存和目标能力允许时，
可保留 JPEG，在设备解码时抖动。`auto` 和 `speed` 保守选择离线方案，避免自动增加
解码计算。也可显式使用 `"codec": "jpeg"` 配合 `dither: true` 处理不透明静态图片。
检查执行报告中的 `dither_stage`、最终资源字节数和解码内存：离线抖动可能增大压缩素材，
解码抖动增加首次解码或缓存淘汰后的耗时及临时内存。RAW 大小不变，离线方案不增加设备抖动计算。
报告中的解码内存不包含所有后端临时缓冲，实际峰值还受目标和缓存配置影响。

GIF/APNG/WebP 和导入视频在帧合成、缩放后离线处理。整段动画共用固定的平滑区域和抖动坐标，
相同位置的相同输入像素不会因邻近内容变化而产生额外跳动，再计算差分帧。
动画中出现过半透明的坐标保持原处理，避免淡入淡出时切换抖动纹理。
帧时长、循环次数和透明度保持原语义，继续使用既有动画压缩与内存预算。

JPEG 解码抖动使用 GRB 1.9，需配套 ESP-GSP/模拟器 1.9.0；旧 runtime 会拒绝新资源。
`gspc compatibility` 的 `compiler_features.image_dither >= 3` 表示编译器支持此路径。
透明图片使用离线方案。不要同时显式请求抖动与 JPEG+A8、JPEG 动画帧或 `store_scale < 1`；
Profile 开关跳过这些组合及矢量，逐图显式请求时会得到诊断。外部图片和动画资源清单也支持 `dither`。
此选项不能恢复源图已经丢失的颜色；透明混合、运行时缩放和面板也可能影响最终色阶。

对已选用 QOI 的不透明 RGB565 静态图片，GSPC 尝试用等价颜色缩小码流，只保留更小的结果。
这会增加编译计算，解码像素和工作区保持不变。

**硬件 JPEG 要求：** 图片宽、高至少 64 像素。RGB888 透明图还须按 16 像素对齐，
`auto` 和 `speed` 才会自动选择 JPEG+A8。`size` 与显式 `jpeg` 可处理未对齐尺寸，
但解码时可能使用 MCU 临时缓冲。JPEG+A8 的颜色有损，A8 透明度无损。

压缩选择不会自动缩小像素尺寸。编译的 `size` 不把 PNG 纳入候选——PNG 解码需要整
张图片的解压临时缓冲且不能按区域解码，仅比较编码文件大小会低估设备 RAM 峰值。
PNG 仍可作为运行时输入。

P4/S31 硬件 JPEG 解码保留 JFIF 的全范围 YCbCr。符合条件的 4:2:0 图片先解码到
YUV420 scratch（每个补齐后像素 1.5 字节），再由 PPA 使用全范围矩阵写入对齐的
PSRAM RGB 目标。PPA 舍入和 RGB565 量化可能与软件解码器不同。其他采样、未对齐或
内部 RAM 目标，以及 PPA 不可用时，使用 YUV444 scratch（每个补齐后像素 3 字节）
和 CPU 转换；符合条件的 RGB888 目标可原位复用。启用加密 PSRAM 豁免区域的构建
保留 CPU 转换路径。硬件失败时仍可回退到软件。评估内存和效率时需计入共享 YUV
scratch 与转换时间，硬件 JPEG 解码计数的耗时包含转换。

缩放中间帧包含 MCU 高度及缓存对齐所需的补齐，逻辑尺寸和紧密排列的源行保持不变。
图片分配的像素起点和私有前缀按至少 64 字节及所在堆的缓存线对齐。

编译和诊断也接受已有名称 `store`（`raw`）、`qoi`（`lossless`）、`default`（`auto`）
以及显式 `rle16`（不透明 RGB565）、`rle16_a8`（透明 RGB565）和 `rle32`（RGB888）。
显式 RLE 与目标像素格式不匹配时编译报错。ARGB8888 叠加 Profile 只存储原生像素；
显式请求其他编码或使用 `store_scale` 会在编译时报错。

`cache_policy` 可与三种策略一起使用。`mmap_direct` 仅适用于最终选为 STORE 的图片，
压缩图片指定 `mmap_direct` 会在编译时报错。既有场景可为 STORE 声明 `preload` 或
`decode_lru`，但 STORE 仍直接映射不会因此进入解码缓存。新场景无需为 STORE 指定
这两项。`hardware_jpeg` 保留兼容行为：有硬件解码器时使用 JPEG，否则使用无损编码；
它不等同于显式 `jpeg`。

### 阅读场景资源预算

编译器进程另有编码缓存，默认最多保留 128 个请求键和 32 MiB 唯一载荷。可用全局
`--encoding-cache-bytes` 参数或 `GSPC_IMAGE_CACHE_BYTES` 设置宿主预算；设为 0 时
关闭缓存持有，编译仍正常完成。daemon 的 `cache/imageStats` 和
`cache/configureImages` 可查询占用并立即调整预算。这些设置影响编译复用，目标
Profile 的 `image_cache_budget_bytes` 则控制设备解码缓存。宿主预算小于工作集时，
可能增加重复编码，应结合命中、未命中和淘汰计数调整。
这些控制要求编译器的 compatibility 或 doctor 报告声明
`compiler_features.image_encoding_cache: 1`；旧工具可继续编译，无需使用这些选项。

当前 `*.execution.json` 合同为 schema version 2，包含当前 GSPC 输出的资源可达性
和可为空的解码大小字段。消费者应根据 `schema_version` 选择 schema，而不要根据
GSPC 的产品版本选择。

严格 Schema 接受 Profile 的 `default`、`auto`、`speed`、`size` 四种策略和
`animation_speed_hardware_jpeg` 开关。资源和来源变体的 `uses` 包含可为空的创作对象
索引、名称、编码请求及初始可见性；程序化图片的初始选择无法确定时，由
`image_usage.initial_visibility_unknown_images` 计数。跨度动画报告
`persistent_span_decode`，禁用缓存的图片报告 `region_decode`。动画的可选
`scratch_bytes` 是持久 `frame_bytes` 之外的工作区。这些字段说明能力和预算，
性能结论仍需设备实测。

`*.execution.json` 的 `resource_budget` 汇总场景资源：

- `resource_blob_bytes` — 资源块及其头部。
- `image_payload_bytes` — 图片载荷。
- `font_blob_bytes` — 字体。

这些值不是固件 Flash 总量：场景 GSB、Bundle 目录/对齐和运行时代码另计。共享资源
不能跨场景报告重复累加。

`resources[].source_path` 标识资源来源。`inclusion` 区分初始绘制、绘制/模板引用
和场景可寻址资源——描述的是编译后的引用关系，不代表实际准备或常驻状态。
`source_variants` 汇总同一来源的多个编码变体，方便检查尺寸与编码成本。不同变体
可能有必要用途，不能仅凭数量判定为冗余。

`initial_static_set_exceeds_cache_budget` 只比较初始静态解码估算与明确启用的缓存
预算，自动预算或关闭缓存时为 null。超过预算可能引起淘汰，不是实测 RAM 峰值，
也不代表必然分配失败。应结合 `esp_gsp_media_stats()` 再决定是否增加内存。
`requested_codec: "generated"` 表示缺少逐资源的原始编码声明信息，全局 profile
策略单独报告，不冒充用户对每张图片的选择。

### 编译式 SVG 图片

可从 [矢量图片示例](../../../examples/usage/widgets/image/vector.json) 开始。

在 Image 的 `image` 字段中引用 `.svg`，GSPC 将它编译为运行时绘制的曲线资源。
编译器会记录产物所需的二进制格式版本——工具配套关系见
[兼容性契约](../reference/compatibility.md)。

Carousel、Flip Card 和特效图片集合会在编译时将可导入的 SVG 素材栅格化，产物按
位图的尺寸和缓存规则使用。需要运行时曲线缩放、改色或形变时，使用命名 Image。

SVG 保留曲线，按目标尺寸栅格化。复用大尺寸矢量时，图片缓存预算需要覆盖可见工作
集：未染色缓存每像素 4 字节，染色遮罩每像素 1 字节。矢量缓存与已解码图片、在途
解码共享场景图片缓存预算；位图解码需要空间时优先回收矢量缓存。预算或分配失败时，
渲染继续使用有界临时分块。

缓存目录最多容纳八个矢量变体，供独立素材共享；同一来源仍最多保留四个变体。
目录分配失败时先回退为四项，两次失败后使用有界分块。首次分配大目录会多用四个
元数据项（32 位目标为 256 字节）。更多像素表面可驻留在原有字节预算内，因此实际
RAM 占用可能增加，换取减少重复栅格化；配置的像素预算不会自动增大。

图片命名后可沿用 `fit`、`rotation` 和 `scalable`。动态调整图片框时，将 `w`、`h`
声明为有界动态字段并指定 `width`、`height` 等语义属性名，编译器会生成对应 setter。
改变宽高改变绘制区域，`scale` 则在框内缩放并受图片框裁剪。宽高沿用框架现有的
0..100 有界属性插值——范围很宽时会出现像素尺寸量化。建议按实际尺寸设置上下界。
框内精细等比缩放使用 Q16.16 的 scale 接口。

可选 `tint` 将场景图片显示为单色轮廓，并生成 `set_tint()` 接口。
不设置 `tint` 时保留源颜色和透明度。场景 Image 的 `image_opacity` 是直接的 0..255
图片透明度；声明 `"image_opacity": {"default":255,"min":0,"max":255}` 后生成
`set_image_opacity()`，小于 255 的字面量也会建立该控制。现有对象字面量 `opacity`
作为固定乘数，背景和边框保留原有样式语义。透明度在整张图片合成完成后统一应用，
不会因重叠路径而重复叠加；修改透明度复用已有矢量缓存。
整图透明度需要配套的新编译器／运行时及 GSB 2.22；普通不透明矢量仍使用 GSB 2.8。
模板矢量仍要求组合透明度为 255，颜色和尺寸保持静态。

矢量导入支持路径、基本形状、纯色填充、填充规则、静态变换和编译期展开的描边。
描边的虚线数组、偏移、端点和连接样式在编译期展开为轮廓；虚线越密，轮廓点数越多，
仍受相同复杂度上限约束。`symbol/use` 和可展开为普通路径的 marker 也在编译期处理，
不增加设备侧 SVG 解析。marker 若依赖实际裁剪，仍会明确拒绝；可在编辑器中展开成路径。
准备素材时：

- 在素材编辑器中将文字和固定像素描边转为路径。
  `vector-effect: non-scaling-stroke` 在属性、行内样式和样式表中均不支持，工具会给出诊断。
- 将绘制范围（包括描边和继承变换）保持在 SVG 视口内；越界诊断会指出元素及超出量。
- 静态渐变、裁剪、蒙版、图案、组透明度和支持的滤镜，可显式选择下面的
  `svg_mode: "raster"`，或导出 PNG。
- 嵌入图片和外部引用先导出 PNG；文字先转路径。

编辑器元数据、非 SVG 命名空间内容和未使用的渐变定义可保留。单次绘制组的透明度
会合并到填充透明度。完全包含组内绘制范围的矩形裁剪可作为冗余裁剪移除。其他
未支持的绘制语义会给出诊断，可选择可导入的 `svg_element`，或使用准备好的位图。

`codec`、`store_scale`、`cache_policy` 用于位图输入。矢量素材通过源路径几何及
Image 的尺寸、旋转和缩放属性调整显示效果。

#### 显式静态 SVG 烘焙

Image 的 `svg_mode` 默认为 `"vector"`。对于使用静态特效的固定装饰，可设置
`"svg_mode": "raster"`，在编译期按固定 `w/h` 将自包含的 SVG 画布烘焙为静态位图。
支持静态线性／径向渐变、裁剪、蒙版、图案、多路径组透明度及 `resvg` 支持的静态滤镜；
`fit` 和 `position_x/y` 一并烘焙。

```json
{"type":"image", "name":"decoration", "w":96, "h":96,
 "image":"decoration.svg", "svg_mode":"raster", "codec":"store"}
```

可通过 `svg_element` 烘焙命名部件，保留祖先变换、透明度、裁剪、蒙版和滤镜，
并在源画布内按内容边界裁切。选择会隔离兄弟绘制节点；效果需要多个部件共同参与时，
应选择包含这些部件的组。

在 RGB565/RGB888 场景中，烘焙后的位图支持旋转及有界动态旋转，
Alpha 和解码缓存要求与 PNG 相同。
旋转围绕图框中心，并受图框裁剪；应为素材保留足够空间。
需要运行时旋转控制时，使用有界 rotation 声明。
参见 [SVG 位图示例](../../../examples/usage/widgets/image/raster.json)。

光栅模式要求固定烘焙尺寸、scale 1 和 `store_scale: 1`。形变、染色、`svg_layout`、
动态宽高或动态 `image_opacity`、运行时缩放使用矢量模式。矢量模式也不接受
`store_scale` 覆盖；需要改变存储尺寸时，调整烘焙 `w/h` 或源几何。
文字、嵌入图片、脚本、动画和外部资源仍不支持。

| 限制项 | 上限 |
|---|---|
| 源 SVG 文件大小 | 2 MiB |
| 输出 RGBA／原生像素／编码资源 | 各 4 MiB |
| 归一化节点数／路径点数 | 4,096 节点／131,072 路径点 |
| 组嵌套深度／滤镜原语数 | 32 层／64 个原语 |
| 图层与滤镜工作量估算 | 64 MiB |
| `svg_element` 隔离及样式表展开后的 XML | 64 MiB |

图层工作量估算与生成 XML 上限分别约束不同构建阶段，都不是编译进程峰值内存的硬上限。
普通位图缓存预算仍然生效。

`codec: "store"` 免解码缓存但占用更多 Flash；压缩资源需计入 profile 的解码缓存预算。
烘焙 SVG 在 `auto`、`speed`、`size` 策略下保持无损，仅在显式指定 `codec: "jpeg"` 时使用有损编码。

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
若修改后的 viewBox 能通过普通矢量编译，诊断还会给出该值。修改 viewBox 会改变画布映射，
重新编译后应检查预期位置。
整图不支持不等于每个命名部分都不支持；选择兼容部分时不会要求渲染无关的装饰组。
`supported` 表示源素材的几何内容能否编译。
默认最多列出 256 个元素，`elements_truncated` 标记截断；可用 `--element` 检查指定 ID。
它不验证场景选项、形变配对或最终目标预算，仍须编译实际场景。

`morph_to` 指定结束 SVG，`morph` 为 0..100 的初始进度，默认 0。
编译后生成 `set_morph()`、`animate_morph()` 和 `animate_morph_to()`；动画沿用现有
时长、缓动、中断和批量更新机制。参见[矢量动效示例](../../../examples/usage/widgets/image/vector_motion.json)。
运行时插值曲线控制点，不解析 XML，不展开完整位图帧序列。
起止 SVG 必须具有相同视口尺寸、绘制路径顺序、填充/描边展开结果结构和绘制颜色；
各路径须保持相同轮廓、段类型、起点和方向。需要统一曲线阶数时，应在编辑器中
转换对应曲线段并检查两个状态的外观。复制同一素材并编辑节点位置，
可保持控制点的对应关系；编译器负责检查结构一致性。
选择元素时，起止状态共用联合边界，避免形变时自动裁紧导致图片跳动。
形变用于命名的场景 Image；模板图片使用静态几何。

Image 的矢量导入上限为 2 MiB、128 次填充／描边绘制、8192 个轮廓点，支持 RGB565/RGB888 场景。
绘制耗时和内存需求随图片尺寸及复杂度增长；连续缩放、旋转和形变应在目标设备上
测量。参见[内存配置](../reference/configuration.md)。

使用执行计划中的 `vector` 摘要检查素材复杂度和内存估算；该估算不代表应用总内存。
素材应自包含，通过 GSP 属性驱动动画，并将导入效果与参考图对照。

矢量应用需要额外的 Flash 和 RAM。框架会按需提高渲染任务栈（随附构建至少为
32 KiB），应用指定的更大栈仍然有效。未启用动态字体的源码构建只链接轮廓光栅器；
启用动态字体（包括预编译包）时仍会保留完整字体引擎。比较固件体积时，应使用相同
的构建模式和字体能力。

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

运行时文案的词汇已知时，可在场景或文本对象上设置 `font_charset_file`，指向
`assets/ui-words.txt` 等 UTF-8 文案文件。路径相对于场景；对象路径覆盖场景路径。
GSPC 将文件字符与 `font_charset`、静态文本合并去重，忽略文件开头的 BOM 和 CR/LF
换行符。修改文件会触发 Bundle 重建。生成的 `set_text()` 接口直接使用烘焙字形，
无需启用 FreeType。

场景级 `font_max_bytes` 可指定正整数字节上限，检查外部链接前所有字体、字号生成
的 GFB 资源总大小。超出时报告所需字节数，不删字、不降低画质。该预算不包含动态
字体源文件、字形排版数据和运行时缓存。

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

动态字体会增加代码、Heap 和任务栈消耗。字形缓存像素区按实例实际生效的
`条目数 × 单边最大像素数²` 计算，元数据另计。默认值来自
`CONFIG_ESP_GSP_FREETYPE_CACHE_GLYPHS` 和 `CONFIG_ESP_GSP_FREETYPE_GLYPH_MAX_PX`，
实例级容量覆盖同样会改变这项成本。超过缓存槽位的
大字形使用一块可复用 Heap 工作区，重复使用时仍需重新栅格化；缓存尺寸不是可渲染字号的
上限。FreeType 工作内存及这块工作区均在缓存预算之外。应按实际字符集和负载测量。
CFF 支持、启动诊断和字体缓冲区生命周期见[字体加载](external-assets.md#sd-字体)。

## 动态文字与标量状态

给应用需要修改的对象设置稳定的 `name`，重新构建后调用生成的辅助函数：

```c
ESP_ERROR_CHECK(gsp_status_temperature_set_text(ui, "24.5 C"));
ESP_ERROR_CHECK(gsp_status_battery_set_value(ui, 82));
```

具体函数取决于控件类型和动态属性。多个标量必须原子可见时，使用公共批量属性 API，
不要依赖多个异步 Setter 的调用顺序。

## 运行时图片

为需要替换内容的 Image 设置稳定的 `name`、`bind` 和 `bind_target: "resource"`，
构建后使用该场景头文件生成的 `set_image()`。显式指定 `bind_target` 可避免对象的
背景色占用该绑定；没有背景色和文字的 Image 可以省略 `bind_target`。

编码格式、目标能力、缓存策略和 COPY/BORROW/TAKE 所有权必须同时满足。发布新图片
前确认旧借用缓冲区是否仍被运行时引用。

通用接口的所有权约定如下；普通命名对象仍优先使用生成的 Image Setter：

| API | 编码缓冲区所有权 |
|---|---|
| `esp_gsp_set_image()` | 返回前复制输入字节 |
| `esp_gsp_set_image_borrowed()` | 借用至 release 回调 |
| `esp_gsp_set_image_owned()` | 提交成功后接管与 `free()` 兼容的分配 |
| `esp_gsp_set_image_ex()` | 显式选择 COPY、BORROW 或 TAKE |

图片缓存按驻留的解码像素消耗预算，包括预取和保留的图片，不按素材文件大小计算。先使用默认策略；
只有诊断或实测表明预算不合适时再调整。

替换图片解码期间，当前图片继续显示。替换解码失败时仍保留当前图片。BORROW/TAKE
提交成功后，编码负载可能在该图片保持当前内容期间一直被引用（解码像素可以被缓存
逐出并再次解码）。API 立即返回错误时所有权仍归调用方。

完成和释放回调可以提交下一张图片，但不得在回调中等待提交完成。
运行时在调用应用代码前提交或分离原请求，每个已接受的请求只完成一次。
被替代请求的取消通知在替换请求安装后交付；取消回调中提交的请求仍是较新的请求。

运行时图片的像素格式由**编译期占位图**决定，而不是由替换图片决定。不透明 PNG
或 JPEG 可以替换带透明度的占位图（alpha 按 255 处理）。JPEG 本身没有 alpha；
替换图片需要独立且无损的透明度平面时使用 JPEG+A8。RGBA 和 RGB+tRNS PNG 只有在
所有像素都不透明时才能替换不透明占位图。需要显示透明效果时，请使用带透明度的占位图。
运行时 PNG 支持非交错的 8 位 RGB/RGBA，以及 RGB 的 `tRNS` 透明色。
透明色在原生颜色量化前比较，生成 0 或 255 的 alpha；透明键未使用的高位按 PNG 规范屏蔽。
调色板、灰度、16 位和交错 PNG 不属于运行时支持范围，可由编译器转换支持的源文件。

## Canvas

GSP 原生缓冲区中的 RGB888 像素按 B、G、R 字节排列；RGB565 使用原生 16 位打包值。
借用帧和 Direct Draw 都需要遵循此布局。

Canvas 适合外部生产者拥有的帧：相机、视频、软件生成图或高频栅格图。根据生产者
特点选择：

- 提交整帧；
- 提交脏区；
- 在渲染任务中执行 Direct Draw。

在命名场景 Image 上显式声明生产方式：

```json
{
  "type": "image", "w": 200, "h": 100,
  "image": "assets/placeholder.png", "codec": "raw",
  "bind": "preview", "canvas": "draw"
}
```

`canvas: "draw"` 在构建期检查 Direct Draw 条件；`canvas: "push"` 检查借用帧
占位图条件，并允许已有的图片变换和 blit 透明度。两者都生成独立可变资源视图，相同
不可变图片数据仍共享；普通 Image 保持原有去重行为。声明不会注册生产者、分配帧
缓冲区或改变运行时 API。需使用在 `gspc schema --authoring` 中暴露
`canvas_resource_views` 能力的编译器；未提供该能力的编译器会拒绝这个新字段。

占位图必须是 RGB565/RGB888 场景中的静态不透明位图，并使用资源 bind
（省略 `bind_target` 或设为 `"resource"`）。draw 模式还要求固定透明度 255、
`fit: "stretch"`／`"fill"`、scale/store_scale 为 1、scalable 为 false，并省略
rotation 或使用固定的 `rotation: 0`；draw 不生成运行时旋转 setter，需要运行时旋转时
使用 push 模式。push 模式沿用现有变换和缓存条件，输入帧匹配编译后占位图的原生尺寸；
启用适配或缩放时，该尺寸可能不同于对象框。透明、SVG、动画占位图、ARGB8888
场景及列表／模板实例会被拒绝。`esp_gsp_canvas_check()` 检查 Direct Draw 兼容
条件，因此可以拒绝带变换的 push 目标。未声明 `canvas` 的旧场景维持原行为，
仍使用运行时接纳检查。

独立视图仍受 Canvas 槽位容量约束，当前默认同时容纳 2 个生产者。按同时存活数量
设置 `ESP_GSP_CANVAS_SLOTS` 或实例级 `ESP_GSP_FIELD_CANVAS_SLOTS` override
（当前范围 1–8）。声明本身不占槽位，安装绘制回调或提交帧才会占用。入队成功与
Canvas 接纳不同，需观察 Canvas result／release 回调。

> Direct Draw 回调不得阻塞，也不得在内部等待另一个持有同一缓冲区的任务。

## List、Grid、Wheel 与 Message List

集合控件按可见窗口复用行或单元格，**不会**为数据集每条记录创建一个对象：

1. 在 JSON 中定义行/单元模板与同时存活上限。
2. 应用数据源提供总数、可见记录及文字/图片。
3. 数据变化后调用对应 `refresh()` 或总数 setter。
4. 回调只读取稳定快照——不在框架任务中访问慢速存储或网络。

Slot 数量按同时可见内容、Overscan 和并发控件计算，**不是**按数据库总记录数计算。

## 图片 Grid 与图库

在 Grid 行模板内的 Image 设置 `"dynamic_image": true`。Binder 只向可见和保留的
备用 Cell 发布内容，不会为数据集每项创建图片目标。

不可变编码内容跨 Cell 复用时，为 `esp_gsp_grid_cell_set_image_ex()` 提供稳定的
非零 `cache_key`。内容字节改变时必须换 Key。
`ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS` 按同时活动的可见 Cell 加 Overscan
计算，不按图库总数计算。

## Message List

聊天式、可变高度内容使用 `message_list`。消息存储留在应用中。追加、前插、替换或
刷新数据时通知框架，不需要重建场景结构。Row 发布运行在框架回调上下文——只发布已
准备好的数据，存储、网络和慢速解码交给应用任务。

绑定可见列表后即可调用 `esp_gsp_list_fling()`——命令会先激活列表再开始滚动。
前插历史消息时保持原消息 ID，并将插入条数传给 `esp_gsp_message_list_changed()`。
框架在调整滚动锚点的同时将复用行重新绑定到对应消息。

将 `esp_gsp_message_source_t` 清零初始化并设置 `struct_size`。默认 `flags = 0`
保留文本 hash 校验——即使漏更新 revision，也能检测文本变化。不含 `flags` 字段的
旧结构继续使用这一行为。

能可靠维护版本的数据源可以设置
`flags = ESP_GSP_MESSAGE_SOURCE_TRUST_REVISION`：当 `id`/`revision` 不变时，
框架直接复用文字测量结果而不再扫描文本。ID 应标识消息本身而非数组索引。前插和重排
时保持 ID，文本或装饰数据变化时更新 revision。若不同内容复用了同一组 ID/revision，
画面可能保持旧内容。此模式仍读取每条消息的元数据，并非范围更新 API。

一次处理期间应保持数据源一致。可在渲染任务发布变化，或为读取方保留不可变快照。
revision 标志不负责同步后台生产者。返回的文本须至少保持有效到下一次 `get()`。
追加、前插通知和失败重试继续遵循 `esp_gsp_message_list_changed()` 的现有语义。

## 容量与内存规则

- `ESP_GSP_FIELD_CONTEXT_DEFAULT_INSTANCES` — 覆盖同时存活的模板实例和复用行。
- `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS` — 覆盖同时活动的运行时图片目标。
- List Slot 和单行文字容量是有界配置，能推导时会在场景生成阶段检查。
- `image_cache_bytes` — 限制已解码图片缓存，不包含应用拥有的编码输入缓冲区。
- 禁用图片缓存会改变可用的运行时图片路径。无 PSRAM 目标调整前先读
  [配置参考](../reference/configuration.md)。无缓存模式下更新运行时 QOI，应使用
  固定尺寸、`fit: "stretch"` 的占位图，并将替换图片编码为相同的原生尺寸。对编码
  格式的运行时替换图片做适配和缩放，需要先通过图片缓存获得解码像素。setter 返回
  成功只代表提交成功——通过 `esp_gsp_set_image_ex()` 的完成回调确认图片已发布，
  并诊断被拒绝的替换请求。

完整函数、回调和停止边界见 [函数级 API 参考](../reference/api-functions.md) 与
[生命周期](lifecycle.md)。

## 视频导入

本地 MP4、AVI、MOV、WebM 和 MKV 可直接使用同一个 `image` 字段。独立安装编译器时，先检查
`gspc compatibility` 中的 `compiler_features.video_import: 1`。GSPC 按需调用
主机端 `ffmpeg` 和 `ffprobe`；安装 FFmpeg，或通过 `GSPC_FFMPEG` 和
`GSPC_FFPROBE` 指定可执行文件路径。普通图片和 GIF/APNG/WebP 动图无需这些工具。
要求 FFmpeg 4.4 或更新版本，实际容器及视频编码兼容范围取决于所用 FFmpeg 构建。

```json
{"type":"image","name":"intro","x":0,"y":0,"w":240,"h":240,
 "image":"assets/intro.mp4","video_start_ms":1000,"video_duration_ms":3000}
```

视频在构建时转换为普通 GSP 动画资源，只导入视频画面，排除音频、字幕及封面。
设备复用现有帧编码和播放器，无需 FFmpeg 或 MP4/AVI 解码器。源视频压缩不会保留，
所以小 MP4 也可能生成较大的 Bundle。现有 `codec`、`animation_codec`、`quality`
和目标 Profile 仍决定输出编码，不会因输入是视频就强制 JPEG。
`dither: true` 沿用无损动画的 RGB565 离线处理；显式同时请求 JPEG 动画帧会在编译时报错。

| 字段 | 视频行为 |
|---|---|
| `max_fps` | 省略时规范化到最多 30 fps，不提高源标称帧率；正数指定上限；显式 `0` 保留源显示时间戳，包括可变帧率。 |
| `video_start_ms` / `video_duration_ms` | 默认从 0 开始；省略时长则导入到结尾。空片段或起点越界会报错。 |
| `video_fit` | 默认 `contain`，以不透明黑色补边；`cover` 居中裁剪；`stretch` 拉伸。未覆盖时继承 Image 显式设置的 `fit`。先烘焙到控件宽高，再应用运行时变换。 |
| `video_loop_count` | 默认 `1`，播放一遍；`0` 无限循环。不改变已有动图的循环默认值。 |
| `video_max_decoded_bytes` | 主机保留的 RGBA 像素预算，默认 268435456 字节（256 MiB）。 |
| `video_max_frames` | 合并相同帧之前的解码帧数上限，默认 4096，最大 65535。 |

导入会处理旋转元数据和非方形像素，再执行尺寸适配。累计时间戳统一量化到毫秒，
避免逐帧取整造成非整数帧率的时长漂移。HDR（PQ/HLG）需要先显式转为 SDR；不支持
或损坏的输入会返回 FFmpeg 诊断。

已知时长会在解码前检查导入预算，实际输出在流式读取时再次检查。超过预算或单次
工具调用超过 120 秒会报错并提示调整方法，不会静默截短片段。RGBA 预算不是主机
进程总内存上限：解码器、RGB565 规范化和编码候选还会占用内存。设备画布、解码及
显示缓冲需要另行计算，并满足目标 Profile 和运行时配置。

[外部资源清单](external-assets.md) 使用相同字段。构建依赖包含视频和实际使用的
工具可执行文件；通过环境变量或 `PATH` 改换工具后应重新配置 CMake。导入帧率不
代表设备播放 FPS，播放性能需要在目标设备上测量。这些选项仅适用于视频输入。

## JPEG 动画帧

动图与导入视频的 `animation_codec` 可以选择：

| 值 | 编译行为 |
|---|---|
| `lossless` | 强制 QOI 增量帧；默认 profile 保持此行为 |
| `jpeg` | 完整 JPEG 序列帧，不因体积更大而退回 QOI |
| `hardware_jpeg` | 目标 Profile 声明 `hardware_jpeg: true` 时使用 JPEG，否则使用 QOI |

```json
{"type":"image","name":"motion","x":0,"y":0,"w":256,"h":256,
 "image":"assets/motion.png","animation_codec":"hardware_jpeg","quality":85}
```

不透明动画编码为 JPEG；任一帧含透明度时，整段动画编码为 JPEG+A8（颜色有损、A8
无损），不会静默丢弃透明区域。GIF/APNG 先合成为完整画布帧再编码，保留帧时长和
循环次数。`max_fps` 仍可限制导入帧率。显式 `codec: "jpeg"` 也适用于动画；若同时
提供 `animation_codec`，后者优先。使用 `animation_codec` 时，未写 `quality` 则
使用 Profile 的 `jpeg_quality`。

超过 65,535 ms 的停留拆成兼容的帧表项，共用压缩 patch 数据。合并相同帧和
`max_fps` 降帧会保留累计时长。超过容器的 65,535 个帧表项、65,535 次有限循环或
32 位负载偏移时，编译失败并报告限制，不会静默截短。资源报告按每次重放计入 patch 工作量。

长停留增加条目时，请配套重新生成外部素材包与对应头文件。

未指定 `animation_codec` 时，`auto`、`speed`、`size` 先比较 QOI 与原生像素补丁（GRB 1.6）。
原生补丁保留 RGB565/RGB888 和完整 A8，直接复制到现有画布而不增加帧缓冲；当整段动画
至少节省 2 KiB（含补丁头与表开销）且满足 `auto`/`speed` 的 `image_auto_min_savings_percent` 时自动选用。

对于不透明 RGB565 内嵌动图，上述自动策略还会比较预编译 Span（GRB 1.8）。
编译器保留帧时序与脏区，在适用时使用共享调色板，并预先规划像素复用；播放器在现有画布上
执行，不增加第二个帧缓冲或逐帧分配。选择时按每个场景保留的去重资源比较最终大小，
计入已启用的压缩。`auto`/`speed` 要求每个替换资源至少节省
`max(10%, image_auto_min_savings_percent)`，`size` 要求载荷更小；对齐后的资源合计
至少节省 8 KiB，作为该场景共享解码器的选择余量。这不是链接器实测代码体积，也不跨场景
汇总余量。预计算会增加编译工作和临时内存，应同时评估编译成本与设备播放。
显式 `animation_codec`、默认 QOI 策略、透明动画、
RGB888 与外置流式资源保留既有资源编码。资源报告以 `anim_spans` 标识选中的表示。

内嵌动图需要进一步缩减体积时，可启用 `CONFIG_ESP_GSP_ENABLE_ANIM_DEFLATE=y`，并选择
Profile `image_codec: size` 或 Image `codec: "size"`。当节省空间达到 8 KiB 和 10%，且
画布与工作区满足场景动画预算时，GSPC 会将最多 4 帧分组压缩为 zlib 流。普通补丁
每组最多 32 KiB 解压数据（GRB 1.7），Span 每组最多 64 KiB（GRB 1.8）。独立 GSPC 可传入 `--animation-deflate` 或在 Profile 中设置 `animation_deflate: true`。

该选项默认关闭，不影响显式 `animation_codec`、JPEG 与外置流式资源。报告通过 `scratch_bytes`
给出工作区大小，播放器在打开时分配一次并跨组复用。有 PSRAM 时，`AUTO` 要求在 PSRAM
中分配，不回退内部 RAM；无 PSRAM 时需计入 SRAM 预算。压缩会增加解码开销，尤其是切组时；
应配套使用支持对应格式的运行时，并检查实际帧耗时。若单帧编码块超过对应分组上限，
编译器会保留未压缩的表示，并报告 `GSPC-ANIMATION-DEFLATE-SKIPPED`；
仅提高动画内存预算不会改变这些格式上限。

JPEG 模式使用整帧解码和整帧刷新，适合愿意用资源空间换取硬件解码机会的场景。
稀疏变化的 UI 动画仍可能更适合 QOI 增量帧。`animation_frame_budget_bytes` 限制
编译动画画布和可选分组工作区预算。编码后的 Flash 大小应查看资源报告。报告区分 `anim_qoi`、
`anim_jpeg`、`anim_jpeg_a8`、`anim_spans`。

`animation_max_players` 是 Profile 中限制每个场景独立编译动画资源数的单独上限，
默认值为 4。可在通过 `--profile` 或 `gsp_add_backend(... PROFILE ...)` 选择的
YAML Profile 中提高该值。设备端 `CONFIG_ESP_GSP_MAX_ASSET_ANIMS=0` 会根据 Bundle
需求推导运行时容量。显式工程或实例容量仍必须覆盖这些资源。隐藏动画可以暂停，但
编译出的资源仍计入播放器上限。

未指定 `animation_codec` 时，`size` 先比较 QOI／原生无损结果与 JPEG/JPEG+A8 的字节数，
再对保留的无损结果选择 Span 和可选分组压缩。
要求颜色无损时，设置 Profile `image_auto_allow_lossy: false`。
`auto` 和 `speed` 仅在 Profile 设置 `animation_speed_hardware_jpeg: true`、首帧之后
无损补丁工作量达到对应整帧工作量的至少 70%、画面不透明、宽高各至少 128 像素且按
16 对齐时考虑硬件 JPEG。`auto` 还要求 JPEG 至少
节省 20% 空间；`speed` 允许最多增加 10%。这个板级设置默认关闭，应根据实际显示
路径测量后启用。

`auto` 和 `speed` 对透明动画保留无损补丁。允许有损且目标支持 JPEG+A8 时，
`size` 可以选择更小的 JPEG+A8；其颜色有损、透明度无损。
显式 `animation_codec` 始终优先。动画使用独立帧缓冲。对动画设置静态图片的
`cache_policy` 或 `store_scale` 会报错。

无损编译动画使用一块原生像素画布。首帧在发布前完整解码；后续帧首次更新前，
运行时会只读检查该帧的 patch 或 Span 数据。非法内容使播放停在最后有效帧，像素、帧号和
时钟不变。GSP 按帧表顺序播放，用播放器内的校验进度标记复用检查结果，
无需分配额外缓存或第二块像素画布。
借用的动画字节必须保持不变，直到关闭播放器。
首次遍历包含帧数据校验，测量播放延迟时应计入这项首次开销。

QOI 区域选择增加有数量上限的行段候选，以保留细线和不规则变化区域。默认只有在
存储字节、像素工作量和原有区域调用成本代理都不增加时，才替换原候选。已有
`speed` 图像策略在额外区域能节省足够像素时，允许每帧最多增加 10% 字节，包含
patch 表；仍使用原 QOI 动画格式。更多区域也可能增加显示窗口开销，应结合目标
设备的解码、渲染和提交共同测量。

硬件与软件 JPEG 解码器的色彩转换可能不同。图标、品牌色等要求颜色一致的 UI
素材应使用 `lossless`。

选型时可分别以 `lossless` 和 `jpeg` 编译，对比生成的 `*.execution.json`：
`resources[].encoded_bytes` 是整段动画的编码大小，`animation.frame_bytes` 是单帧像素缓冲，
`animation.patch_ratio` 是写入像素工作量与整帧解码的比值；先复制再替换像素时可超过 1。
动画的 `flash_savings_bytes` 与 `flash_savings_percent` 为 null，因为单帧画布不能作为
整段动画的 Flash 基准。应比较不同构建的实际编码大小。报告不包含实际 SD 吞吐或
完整播放耗时，最终以目标板测量为准。

SD/NAND 中的预处理资源与逐帧动画见 [外部资源](external-assets.md)。
SD 卡字体可用 [字体文件接口](external-assets.md#sd-字体) 加载，显式设置大小上限
并管理其生命周期。

## 透明位图、预览与资源预算

在 RGB565/RGB888 场景中，RGB565+A8 和 ARGB8888 图片资源可使用 `fit`、`rotation`
及有界动态旋转，模板图片也适用。变换按图片框裁剪，源图透明度参与混合。
对象自身的 `opacity` 仍须为 255。固定且无资源绑定的静态位图可将字面量
`image_opacity` 烘焙进源 alpha：要求 codec 省略、为 `auto` 或 `store`，且
`store_scale` 为 1；产物使用 STORE，不会为位图生成运行时透明度控制。
透明变换使用软件采样。`codec: raw` 不需要解码缓存；压缩资源须有足够解码缓存——
编码体积不能代替运行时像素内存预算。

相机或解码器推帧使用带 `bind` 的不透明 Image 占位对象，再调用
`esp_gsp_canvas_push()` / `esp_gsp_canvas_try_push()`。输入必须匹配声明的尺寸与
目标 RGB565/RGB888 格式。至少准备双缓冲，并在释放回调后复用旧帧。偶尔替换图片
使用 Image Setter。文件资源接入见 [外部资源](external-assets.md)。

跨芯片场景优先 `codec: auto`。按资源侧重点可改用 `speed` 或 `size`。例如
`{"type":"image","image":"assets/photo.png","w":240,"h":240,"codec":"size","quality":75}`
将这张图片的候选 JPEG 质量设为 75。显式 `jpeg` 是严格要求，不支持的目标会指出
替代方式。

`store_scale < 1` 会自动选择适配声明图片框的 fit 绘制路径，无需额外开启
`scalable`。`codec: auto` 或 `speed` 配合缩小存储采用 QOI；`size` 则比较缩小后
的 QOI 与可用 JPEG。显式 `raw` 不支持此存储选项，会在编译期指出对象路径和替代
编码。压缩缩略资源仍需要解码缓存。

迁移时优先使用原始 PNG 和 TTF/OTF。只有 LVGL C 数组或私有二进制时，必须先确定
LVGL 版本、像素格式、通道顺序、Stride、Alpha 是否预乘及压缩格式，再转换为编译器
接受的资源。32 位像素不能仅凭位数判断通道顺序；私有 `.bin` 也不是统一格式。

## 从固件中移除 JPEG

JPEG 解码默认开启。如果应用完全不使用 JPEG，可以将解码代码从固件中裁掉：

```ini
CONFIG_ESP_GSP_ENABLE_JPEG=n
```

已有工程可在 `idf.py menuconfig` 的 **ESP-GSP** 菜单中关闭
**Link the JPEG decoder**。新工程可将该设置放入 `sdkconfig.defaults`。该选项适用
于源码和预编译组件，在链接阶段生效。组件构建文件与预编译库应来自同一发布版本。

### 资源编译

`gsp_add_bundle()` 和 `gsp_add_assets()` 会向 GSPC 传入 `--disable-jpeg`。
显式 `PROFILE` 仍可使用——编译器排除 JPEG 但保留其他能力与自定义预算。若 Profile
只允许 JPEG，则会报配置冲突。

- 静态图片可使用 `codec: auto` 选择无损编码。显式 `codec: "jpeg"` 会报
  `GSPC-RS-CAPABILITY`。
- 动画可使用 `animation_codec: "lossless"`。显式 `animation_codec: "jpeg"`
  会报同类错误。关闭 JPEG 时 `animation_codec: "hardware_jpeg"` 会选择无损帧。

### 运行时图片与外部资源

已接受的 JPEG 运行时图片请求会在分配解码面前以 `GSP_ERR_UNSUPPORTED` 完成。
通过完成回调获取结果，缓冲区所有权和释放规则保持不变。

Host 和 WASM/WASI 模拟器保留自身的编码支持——预览成功并不代表关闭 JPEG 的设备
固件能够显示相同的外部 JPEG 数据。

## 软件 JPEG 与缩放动画

ESP-IDF 在 ESP32-S3 等目标上已有 `esp_new_jpeg` 软件解码路径。
`gsp_add_bundle`/`gsp_add_assets` 分别识别软件依赖与硬件 JPEG 引擎。静态 JPEG
还要求可用的解码图片缓存。独立平台编译可通过 `--software-jpeg` 声明软件解码能力。
它放行显式 `codec: "jpeg"`，不会仅因软件解码器存在就让 `auto` 改选有损 JPEG。

命名且可缩放的 Image 可用 `animate_scale_q16` 和 `animate_scale_q16_to` 设置
缩放动画。65536 表示 1×，131072 表示 2×。在图片所属场景调用，目标值使用声明的
缩放范围。

## 替换图片

为场景图片添加资源 `bind`（例如 `name: "cover", bind: "cover_image"`），即可
生成替换接口。模板图片使用 `dynamic_image: true`。应用创建模板图片时，按同时活跃
的图片目标数配置 `ESP_GSP_FIELD_DEFAULT_DYNAMIC_IMAGE_SLOTS`。

使用 `gsp_add_assets()` 构建预编译素材包，再通过目标图片的生成 `set_asset()`
helper 显示素材。选择图片时使用生成的素材引用，其中包含接口所需的素材包信息。
