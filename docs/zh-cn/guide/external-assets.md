# SD / NAND 外部资源

固件内资源继续使用 `gsp_add_bundle()`。大型图库和动画可以导出到已挂载的文件系统，
两者复用相同的预处理和解码路径——设备不需要解析 GIF/APNG 或重新合帧。启动页和
占位图可以保留在固件中，外部包按图片/帧读取，不会整包搬进 RAM。

## 最小接入流程

在 `assets/assets.json` 中列出资源，路径相对于清单文件：

```json
{"assets":[{"name":"cover","src":"cover.png","codec":"auto",
  "target":{"scene":"../scenes/gallery.json","image":"cover"}}]}
```

**字段说明：**

- `name` — 稳定名称，需符合 C 标识符规则。`open` 是保留名称。
- `target`（可选）— 指向场景中唯一的命名 Image（也可通过 `bind` 匹配），用于
  推导固定宽高、检查像素/透明度匹配，并跟踪场景和占位图依赖。Image-fit 保留源
  尺寸；动态或高级布局仍需运行时验证。
- 省略 `target` 可继续使用独立素材库和动态创建的目标——显式提供 `width`/`height`，
  或省略它们以保留源尺寸。

```cmake
gsp_add_bundle(${COMPONENT_LIB} PIXEL_FORMAT rgb565 IMAGE_CACHE_BYTES 262144)
gsp_add_assets(${COMPONENT_LIB} MANIFEST "../assets/assets.json" SYMBOL media)
```

外部资源默认继承同一组件先前声明的 Bundle 配置（像素格式、目标能力和图片缓存
预算）。没有可继承的配置时使用目标默认值（RGB565）。仍可显式指定 `PIXEL_FORMAT`
或 `PROFILE`。同一组件有不同 Bundle Profile 时，须明确选择。

构建后生成 `build/esp-idf/main/gsp_assets_media/media.gspb` 和 `media.h`，并自动
跟踪源素材、目标场景、占位图和 Profile。只需把 `.gspb` 复制到存储。挂载完成后，
从应用任务打开资源包并使用生成方法：

```c
#include "media.h"
esp_gsp_assets_t *assets;
ESP_ERROR_CHECK(gsp_media_open("/sdcard/media.gspb", NULL, &assets));
ESP_ERROR_CHECK(gsp_gallery_cover_set_asset(ui, assets, gsp_media_cover, false));
```

`gsp_gallery_cover_set_asset()` 对应 `gallery` 场景中可替换的命名 Image `cover`，
具体名称以生成头文件为准。`false` 跟随原动画循环次数，`true` 只播一轮。
`open()` 会读取并校验文件，应在应用任务执行，不要放入渲染/解码回调。

独立导出也可使用 CLI：

```sh
gspc assets assets/assets.json --platform-soc esp32s31 --psram --hardware-jpeg --symbol media -o output/media.gspb
```

## 获取二进制资源包

`gsp_add_assets()` 会随 `idf.py build` 生成资源包。完成项目配置后，也可单独重建资源：

```sh
cmake --build build --target gsp_assets_media
```

目标后缀来自 `SYMBOL`。`main` 组件的 `SYMBOL media` 默认输出到
`build/esp-idf/main/gsp_assets_media/media.gspb`；其他目录以构建日志为准。
复制 `.gspb` 到设备挂载路径即可；`.h` 用于编译固件，`.deps.json` 和 `.d` 用于构建依赖。
资源 `.gspb` 不是固件 `.bin` 或文件系统镜像，`idf.py flash` 不会将它复制到 SD 卡。
替换正在使用的包前，先按下文关闭流程停止文件访问。

## 编码与目标兼容

栅格导入沿用编译器的 PNG/APNG、JPEG、GIF、BMP 和 WebP 能力。
`quality` 为 1–100，默认跟随 Profile；`max_fps: 0` 不限速，1–120 限制导入
动画帧率。导出要求 RGB565 或 RGB888 Profile。

| `codec` | 行为 |
|---|---|
| `auto`（默认） | 使用 Profile 策略；自动动画策略比较 QOI／原生补丁与符合条件的 JPEG 帧 |
| `speed` | 静态图片选择 STORE、QOI/RLE 或硬件 JPEG；动画使用 QOI／原生补丁，符合条件的密集帧且 Profile 有板级实测提示时可选硬件 JPEG |
| `size` | 静态图片取可用格式中体积更小者；动画比较 QOI／原生差分与 JPEG/JPEG+A8 整段大小 |
| `lossless` | 无损 QOI，保留透明度 |
| `qoi` / `rle16` / `rle16_a8` / `rle32` | 显式选择静态无损编码，RLE 须匹配像素格式与透明度；动画输入使用无损 QOI 补丁 |
| `hardware_jpeg` | 目标具有相应硬件能力时使用 JPEG/JPEG+A8，否则使用无损 QOI |
| `jpeg` | 显式 JPEG/JPEG+A8，也允许软件解码；颜色有损、透明度无损 |
| `raw` | 静态图片使用原生像素；动画沿用无损策略，可用 `animation_codec` 覆盖 |

外部资源的编码像素格式和 alpha 模式必须与占位图完全相同。清单带 `target` 提示时
在导出阶段检查；省略提示后，运行时发布仍会检查，不匹配时保留旧图。因此不透明外部
资源不能替换透明占位图。原生尺寸也必须匹配，支持 Image-fit 的目标可接纳不同源尺寸。
这与通过 [`set_image()`](media-and-data.md#运行时图片) 提交原始 PNG/JPEG 字节不同：
后者可向透明目标填入 alpha 255。外部目标需要透明度时，使用模式匹配的透明资源。

静态 JPEG/JPEG+A8 需要图片缓存；动画可以使用独立帧缓冲。原生 STORE 和已支持的
QOI/RLE 静态图片区域解码路径可以不使用整图缓存。

## Row / Widget 和动态名称

模板 Resource 槽生成 Widget 和 Row 的 `*_asset()` 方法。也可使用公共入口：

```c
ESP_ERROR_CHECK(esp_gsp_assets_show_row(assets, ui, row, image_slot, gsp_media_cover, false));
ESP_ERROR_CHECK(esp_gsp_assets_show_widget(assets, ui, widget, image_slot, gsp_media_cover, false));
ESP_ERROR_CHECK(esp_gsp_assets_show_target_name(assets, ui,
    esp_gsp_asset_row_target(row, image_slot), record_asset_name, false));
```

Row 使用 binder 提供的令牌，槽位使用生成常量。读取结束后再次校验行/实例是否有效，
旧令牌的查询或停止操作不会影响新条目。普通 Image 可用 `show_name()` 按名称显示。

请求会记录当前场景。切换场景后后续帧会被取消，避免写入另一场景中同编号的绑定。
返回场景时重新提交。一个目标应由一个资源服务/图片生产者负责。

> **隐藏控件或父容器不会自动暂停文件读取和解码。** 隐藏时调用
> `esp_gsp_assets_stop()` 或 `esp_gsp_assets_stop_target()`，再次显示时重新
> `show()`——动画从头播放。

视频输入使用与[场景视频导入](media-and-data.md#视频导入)相同的 `video_*` 字段和 `max_fps` 行为。省略尺寸时，使用经过旋转和像素宽高比修正后的显示尺寸。

## 编码与存储分离

内嵌与外部资源共用栅格预处理、原生像素转换和 JPEG 策略，保留透明度、累计帧时长与
循环信息。外部动画拆成可独立读取的帧成员：自动策略可使用 QOI 或原生像素补丁，
但不会导出内嵌动画的 SPANS 与分组 DEFLATE。因此相同输入及 Profile 的内嵌 Bundle
和外部包可能有不同编码大小。

清单支持与场景 Image 相同的 `animation_codec`：`lossless`、`jpeg`、
`hardware_jpeg`，优先于 `codec` 的动画策略。`auto` 或未指定编码时在未校准目标上
默认无损差分。`auto` 和 `speed` 使用板级实测提示，`size` 比较编码字节数，
`hardware_jpeg` 按目标能力选择。静态图片仍由 `codec` 控制。动画上的
`codec: "raw"` 与场景保持一致，使用无损动画路径。画布大小需符合 Profile 的动画帧
预算。

错误动画帧不会修改已显示图片。新请求从首帧开始。关闭图片缓存时也支持 JPEG
动画，但应在目标设备上测量播放性能。

动画内存由 `ANIM_FRAME_MEMORY` 和 `ANIM_MAX_FRAME_BYTES` 控制，与编码读取预算
分开。需计入完整解码帧及播放峰值内存。存储延迟会影响帧率。使用新导出的动画前，
应配套更新 GSPC 和 ESP-GSP。旧资源包仍可读取，重新导出后可使用新的编码优化。

## 状态与内存诊断

`esp_gsp_assets_show()` 及生成的资源 setter 返回成功只表示请求已接纳，不代表文件
已加载或图片已解码。在应用任务调用 `esp_gsp_assets_get_status()`，获取请求编号、
处理阶段、错误和 `pending` 状态：

| 阶段 | 含义 |
|---|---|
| `QUEUED` | 已排队，包括队列拥塞时等待重试 |
| `READING` | 正在定位、读取或校验资源 |
| `DECODING` | 已提交，等待 UI 解码/发布完成 |
| `READY` | 当前图片已就绪；动画可继续调度后续帧 |
| `FINISHED` | 动画播放结束，最后一帧仍显示 |
| `FAILED` | 查看 `last_error`、`failed_stage`、`system_errno`、`required_bytes` |
| `STOPPING` / `STOPPED` | 正在停止 / 后续帧已停止 |

`pending` 覆盖所有尚未完成的工作，不会把排队当成完成。多个目标公平轮转。队列
拥塞时复用已读取的编码缓冲；预算/堆不足时可回收其他尚未提交的缓冲，不回收已
显示的图。

- `failed_stage` 区分文件读取与 UI 解码/发布。
- `required_bytes` 是编码缓冲需求，不要用它代替解码预算。
- 文件错误保留 `errno`，失败停止该次请求。恢复后重新提交。
- `stop()` 保留最后一张图，再次 `show()` 从头开始。
- 旧的简单 `status()` 查询仍可使用。

用 `esp_gsp_assets_get_publication()` 判断是否可以结束加载占位：

```c
esp_gsp_asset_publication_t publication;
if (esp_gsp_assets_get_publication(assets, ui,
        esp_gsp_asset_image_target(image_bind), &publication) == ESP_GSP_OK &&
        publication.has_frame) {
    // 当前请求已将一张完整图片发布到 UI。
}
```

查询一次返回当前 `request_id` 和 `has_frame`。首次成功后，后续帧读取期间仍为
true。后续帧失败而保留旧图时也保持 true。新的 show、stop 或场景切换会清除该标记。
它表示 UI 发布完成——LCD 实际扫描显示时间需单独测量。

**内存预算：** 每包一个内部栈读取任务，默认 4 KiB。读取不阻塞渲染任务。读取预算
默认取可用 PSRAM 的四分之一（无 PSRAM 时取可用内部 RAM 的四分之一），上限 4 MiB。
这是编码数据上限，不是预分配，也不是整个 GSP 的 RAM 配额。替换时需要旧、新编码
数据与解码像素共存。可测量后覆盖：

```c
esp_gsp_assets_config_t config = { .read_budget_bytes = 512 * 1024 };
ESP_ERROR_CHECK(gsp_media_open("/sdcard/media.gspb", &config, &assets));
```

`esp_gsp_assets_get_stats(assets, ui, &stats)` 按需给出编码数据当前值/峰值/预算、
索引缓存、解码内存当前值/峰值、文件字节数、加载耗时和重试次数。

> **注意：** 解码内存包含本服务的动画画布/补丁暂存，以及传入 UI 的场景缓存。
> 各类峰值相加是保守上界，不是同时采样。编码统计包括正在读取的缓冲；加载耗时包含
> 查找、CRC 和分配；解码统计包含 UI 的其他图片。这些值不是同时采样的整机内存
> 账本，也不含显示、DMA、任务和所有临时内存。

小型索引自动尝试缓存在 PSRAM（最多 64 KiB），没有 PSRAM 或分配困难时分块读取。
图片分配可以回收可选索引缓存。播放吞吐量取决于存储读取延迟、解码和显示提交耗时，
应使用目标存储和实际素材测量各阶段开销。

## 关闭、更新和存储边界

关闭时**必须检查返回结果**，默认最多等待 5 秒，也可指定等待预算：

```c
esp_gsp_err_t ret = esp_gsp_assets_close_wait(assets, 1000);
if (ret == ESP_GSP_OK) {
    assets = NULL;
    esp_gsp_stop(ui);
    // 此后才卸载或更新文件系统。
} else if (ret == ESP_GSP_ERR_TIMEOUT) {
    // 保留 assets、UI 和挂载，恢复处理后再次 close_wait()。
}
```

超时不会强制终止驱动读取或释放在途数据。关闭中的句柄拒绝新 show 请求，但仍可
查询和重试关闭。成功关闭前保持 UI 处理命令/解码，其他调用者应结束对该句柄的使用。
关闭后，已显示的源数据仍在 UI 替换/销毁时释放。调整读取预算同样通过关闭、重新
打开完成。

**文件更新规则：**

- 同名且目标契约兼容的素材可独立更新。新增名称可用 `show_name()` 或目标名称
  入口，不依赖旧帧数/位置。
- 文件使用期间保持不变——关闭后替换再打开。
- 匹配构建快速校验索引，合法替换包分块校验全包一次，随后按成员校验。连续校验
  避免每个小块重新定位，并定期让出 CPU。首次验证仍有存储读取成本。CRC 不提供来源
  认证。

导出逐文件原子替换，资源包最后提交。写失败不会截断已有资源包——但这不是跨文件
事务，构建成功后再部署。

当前接入 ESP-IDF 已挂载的文件系统，SD/NAND 共用接口。裸 NAND 的 ECC、坏块和
磨损管理由存储驱动负责。文件大小受平台 `fseek/ftell` 范围限制（32 位 `long` 下
需小于 2 GiB）。每包还限制为最多 65,535 个成员，所有静态图片及动画帧表项合计；
更大的素材库需拆包。该文件服务通过 ESP-IDF API 使用，PC 桥接没有对应端点。

`examples/usage/external_assets` 提供 SDSPI 挂载、板级引脚配置、BSP 已挂载
SDMMC/NAND 入口及内置占位图。挂载失败不会格式化存储。工具配套关系见
[兼容性契约](../reference/compatibility.md)。

## SD 字体

字符集已知时，优先由 GSPC 烘焙为 GFB 字形。运行时修改文本，只要字符仍在该
字符集内，就不需要原始字体或 FreeType 动态回退。

| 需求 | 字体路径 |
|---|---|
| 字符已知，包括运行时变化的文本 | 将所需字符烘焙进 bundle |
| 固定字形不进入固件镜像 | 使用 `gspc font-link`，加载其 GFB 目录 |
| 需要编译字符集之外的字符 | 显式提供原始字体，由 FreeType 补齐缺字 |

CFF 支持用于从含 CFF 轮廓的原始字体补字。GSPC 已将 CFF 源字体烘焙成 GFB 时，
加载这些 GFB 字形无需运行时 CFF driver。应根据字符覆盖和资源预算选择动态回退，
无需仅因文本会变化或源字体是 OTF 就启用它。

`esp_gsp_font_file.h` 提供启动前的字体文件加载。动态 TTF/OTF/TTC 与
`gspc font-link` 生成的字体目录包共用 `open → apply → close` 流程，按文件签名
识别类型。

`max_bytes` 必须非零——超过上限在分配前拒绝。文件整体驻留 RAM，ESP-IDF 优先使用
PSRAM。**这不是按需读取字形**——上限只约束文件字节数，仍需预留对齐、句柄、
FreeType 和字形缓存开销。

动态字体的源码和预编译组件工程均需先调用 `gsp_enable_freetype()`，再从已挂载的
文件系统加载：

```c
#include "esp_gsp_font_file.h"

esp_gsp_font_file_t *font = NULL;
esp_gsp_config_t app = gsp_bundle_config();
if (esp_gsp_font_file_open("/sdcard/ui.ttf", 8 * 1024 * 1024, &font) == ESP_GSP_OK) {
    ESP_ERROR_CHECK(esp_gsp_font_file_apply(font, &app));
}
// On load failure, app keeps its embedded fonts.
// Use app to start the UI; keep font alive until esp_gsp_stop(ui) succeeds.
```

加载失败时可保留固件内字体继续启动。启动失败，或所有引用该字体的 UI 停止成功后，
调用 `esp_gsp_font_file_close(font)`。停止超时时保留句柄继续等待。释放后的旧配置
不可直接复用。`apply()` 只准备启动配置，不热替换运行中的字体。动态字体在此检查
签名，完整字体解析由 UI 启动时的 FreeType 完成，TTC 使用第一个字体面。目录包在
加载时校验 CRC。文件加载或 UI 启动成功不等于动态补字已启用：部分 FreeType 初始化
失败会记录警告，并继续使用烘焙字形。应检查 `Dynamic text ready`，再用烘焙字符集
之外的字符验证。缺少 CFF driver 会与其他字体数据或 FreeType 错误分别诊断。

### 外置固定字形

场景使用 `"font_link":"external"`，再链接资源包：

```sh
gspc pack scenes/app.json -o build/app.gspb
gspc font-link build/app.gspb --output-dir build/linked --catalog build/fonts.gspb
```

应用使用 `build/linked/app.gspb`，将 `fonts.gspb` 放入 SD 卡，并用同一组字体
文件接口加载、应用目录包。原始未链接包仍包含字形，不能据此认为字体已外置。

目录包设置 `app.font_catalog`，动态字体设置 `app.ttf`/`ttf_size`。可以分别加载
两个句柄并依次应用，共同提供固定字形和动态补字。两类句柄都需保持到 UI 停止。

字体解析分为两步：启动时，场景的每个 FONT 引用优先选择内嵌 GFB；缺少内嵌资产时，
才按相同的资产/content ID 从目录取得 GFB。必需资产缺失会导致启动失败，即使同时
提供动态 TTF 也不能替代，因为 GFB 还提供编译期布局元数据。排版时优先使用这份
已解析 GFB 中的字形，再由动态字体按其编译字号补齐缺字。目录不会按码点、字体名称
或字号搜索，因此给未链接 bundle 挂目录不会扩大字符集，也不会覆盖内嵌字体。

### 模拟器与 PC bridge

使用 capabilities 报告 `font_catalog_version: 1` 的模拟器构建：

```sh
gsp_sim_host --bundle build/linked/app.gspb --font-catalog build/fonts.gspb \
  --dynamic-font assets/ui.ttf --headless --frames 2
```

`--font-catalog` 优先于 `GSP_FONT_CATALOG` 环境变量。bridge runner 接收相同选项
和环境变量，在 host 创建 UI 前传入；原生 C 模拟器也支持两者。GSPB 字体目录应传给
这个选项，不能传给 `--dynamic-font`。目录内容与 CRC 在启动前校验；整个目录文件和
动态字体在 reset 期间保持驻留 RAM，UI 销毁后释放。此路径验证目录解析及其与动态字体
的共存，不验证 SD 挂载或设备堆分配位置。

PC Backend 接收的 UI handle 已经打开，bridge archive 不实现
`esp_gsp_font_file_open/apply/close`，应在启动 host 时配置字体。RPC 不热替换字体。
缺少此入口的旧发行版需要更新 host 及其内嵌运行时，仅重编 Backend 不会增加该能力。

> 字体 `open()` 返回时文件已经关闭。如果没有图片资源服务等其他使用者，随后可以
> 卸载 SD；UI 继续使用 RAM 中的字体。这与图片/动画的逐帧文件读取生命周期不同。
