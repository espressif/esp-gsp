# SD / NAND 外部资源

固件内资源继续使用 `gsp_add_bundle()`；图库和动画可导出到已挂载的文件系统。
两者复用预处理和解码路径，设备无需解析 GIF/APNG 或重新合帧。启动页与占位图
可以保留在固件中，外部包按图片/帧读取，不整包搬进 RAM。

## 最小接入流程

在 `assets/assets.json` 中列出资源，路径相对于清单：

```json
{"assets":[{"name":"cover","src":"cover.png","codec":"auto",
  "target":{"scene":"../scenes/gallery.json","image":"cover"}}]}
```

`name` 是稳定名称，使用 C 标识符；`open` 为保留名称。可选 `target` 指向场景中
唯一的命名 Image（也可通过其 `bind` 匹配），推导可确定的固定宽高，检查像素/
透明度匹配，并跟踪场景和占位图依赖。Image-fit 保留源尺寸；动态或高级布局仍需
运行时验证。省略 `target` 可继续使用独立素材库和动态创建的目标，显式提供
`width`/`height`，或省略它们以保留源尺寸。

```cmake
gsp_add_bundle(${COMPONENT_LIB} PIXEL_FORMAT rgb565 IMAGE_CACHE_BYTES 262144)
gsp_add_assets(${COMPONENT_LIB} MANIFEST "../assets/assets.json" SYMBOL media)
```

外部资源默认继承同一组件先前声明的 Bundle 配置，包括像素格式、目标能力和
图片缓存预算；没有可继承的配置时使用目标默认值（RGB565）。仍可显式指定
`PIXEL_FORMAT` 或 `PROFILE`。同一组件有不同 Bundle Profile 时，须明确选择。

构建生成 `build/esp-idf/main/gsp_assets_media/media.gspb` 和 `media.h`，并自动跟踪
源素材、目标场景、占位图和 Profile。只需把 `.gspb` 复制到存储。挂载完成后，从
应用任务打开资源包并使用生成方法：

```c
#include "media.h"
esp_gsp_assets_t *assets;
ESP_ERROR_CHECK(gsp_media_open("/sdcard/media.gspb", NULL, &assets));
ESP_ERROR_CHECK(gsp_gallery_cover_set_asset(ui, assets, gsp_media_cover, false));
```

`gsp_gallery_cover_set_asset()` 对应 `gallery` 场景中可替换的命名 Image `cover`；
具体名称以生成头文件为准。`false` 跟随原动画循环次数，`true` 只播一轮。
`open()` 会读取/校验文件，应在应用任务执行，不放入渲染/解码回调。

独立导出也可使用 CLI：

```sh
gspc assets assets/assets.json --platform-soc esp32s31 --psram --hardware-jpeg --symbol media -o output/media.gspb
```

## 编码与目标兼容

栅格导入沿用编译器的 PNG/APNG、JPEG、GIF、BMP 和 WebP 能力。
`quality` 为 1–100，默认跟随 Profile；`max_fps` 限制导入动画帧率。

| `codec` | 行为 |
|---|---|
| `auto`（默认） | 静态图片沿用目标自动策略；动画使用无损 QOI 差分帧 |
| `lossless` | 无损 QOI，保留透明度 |
| `speed` / `hardware_jpeg` | 目标具有相应硬件能力时使用 JPEG/JPEG+A8，否则使用无损 QOI |
| `jpeg` | 显式 JPEG/JPEG+A8，也允许软件解码；颜色有损、透明度无损 |
| `raw` | 静态图片使用原生像素；动画沿用无损策略，可用 animation_codec 覆盖 |

目标的像素格式、透明度形式和原生尺寸必须匹配；不透明 Image-fit 允许改变源尺寸。
透明资源使用透明占位图，不匹配会报错并保留旧图，不会丢弃透明度。静态 JPEG/JPEG+A8
需要图片缓存；动画可以使用独立帧缓冲。原生 STORE 和已支持的 QOI/RLE 静态图片区域
解码路径可以不使用整图缓存。

## Row / Widget 和动态名称

模板 Resource 槽生成 Widget 和 Row 的 `*_asset()` 方法；也可使用公共入口：

```c
ESP_ERROR_CHECK(esp_gsp_assets_show_row(assets, ui, row, image_slot, gsp_media_cover, false));
ESP_ERROR_CHECK(esp_gsp_assets_show_widget(assets, ui, widget, image_slot, gsp_media_cover, false));
ESP_ERROR_CHECK(esp_gsp_assets_show_target_name(assets, ui,
    esp_gsp_asset_row_target(row, image_slot), record_asset_name, false));
```

Row 使用 binder 提供的令牌，槽位使用生成常量。读取结束后再次校验行/实例是否有效，
旧令牌的查询或停止操作不会影响新条目。普通 Image 可用 `show_name()` 按名称显示。

请求记录当前场景；切换后取消后续帧，避免写入另一场景中同编号的绑定。返回场景时
重新提交。一个目标应由一个资源服务/图片生产者负责。
页内隐藏控件或父容器不会自动暂停文件读取和解码；隐藏时调用 `esp_gsp_assets_stop()`
或 `esp_gsp_assets_stop_target()`，再次显示时重新 `show()`，动画从头播放。

## 编码与存储分离

相同素材、尺寸、目标 Profile 和编码配置下，Flash 与外部资源使用相同的编码策略。
导出保留透明度、帧时长和循环信息。

清单支持与场景 Image 相同的 `animation_codec`：`lossless`、`jpeg`、`hardware_jpeg`，
优先于 `codec` 的动画策略。默认无损差分；`hardware_jpeg` 按目标能力选择。`speed`
保留为旧清单的便捷别名。静态图片仍由 `codec` 控制，动画上的 `codec: "raw"` 与场景
保持一致，使用无损动画路径。画布大小需符合 Profile 的动画帧预算。

错误动画帧不会修改已显示图片；新请求从首帧开始。关闭图片缓存时也支持 JPEG
动画，应在目标设备上测量播放性能。

动画内存由 `ANIM_FRAME_MEMORY` 和 `ANIM_MAX_FRAME_BYTES` 控制，与编码读取预算
分开。需计入完整解码帧及播放峰值内存；存储延迟会影响帧率。使用新导出的动画前，
应配套更新 GSPC 和 ESP-GSP；旧资源包仍可读取，重新导出后可使用新的编码优化。

## 状态与内存诊断

`esp_gsp_assets_get_status()` 返回请求编号、处理阶段、错误和 `pending`：

| 阶段 | 含义 |
|---|---|
| `QUEUED` | 已排队，包括队列拥塞时等待重试 |
| `READING` | 正在定位、读取或校验资源 |
| `DECODING` | 已提交，等待 UI 解码/发布完成 |
| `READY` | 当前图片已就绪；动画可继续调度后续帧 |
| `FINISHED` | 动画播放结束，最后一帧仍显示 |
| `FAILED` | 查看 `last_error`、`failed_stage`、`system_errno`、`required_bytes` |
| `STOPPING` / `STOPPED` | 正在停止 / 后续帧已停止 |

`pending` 覆盖所有尚未完成的工作，不把排队当成完成。多个目标公平轮转。队列拥塞
时复用已读取的编码缓冲；预算/堆不足时可回收其他尚未提交的缓冲，不回收已显示图。
`failed_stage` 区分文件读取与 UI 解码/发布，`required_bytes` 是编码缓冲需求，
不要用它代替解码预算。文件错误保留 `errno`，失败停止该次请求；恢复后重新提交。`stop()` 保留最后一张图，
再次 `show()` 从头开始。旧的 `status()` 简单查询仍可使用。

每包一个内部栈读取任务，默认 4 KiB；读取不阻塞渲染任务。读取预算默认取可用
PSRAM 的四分之一，无 PSRAM 时取可用内部 RAM 的四分之一，上限 4 MiB。这是编码
数据上限，不是预分配，也不是整个 GSP 的 RAM 配额。替换时需要旧、新编码数据
与解码像素共存；可测量后覆盖：

```c
esp_gsp_assets_config_t config = { .read_budget_bytes = 512 * 1024 };
ESP_ERROR_CHECK(gsp_media_open("/sdcard/media.gspb", &config, &assets));
```

`esp_gsp_assets_get_stats(assets, ui, &stats)` 按需给出编码数据当前值/峰值/预算、
索引缓存、解码内存当前值/峰值、文件字节数、加载耗时和重试次数。解码内存包含本服务
的动画画布/补丁暂存，以及传入 UI 的场景缓存；峰值相加是保守上界，不是同时采样。编码统计包括
正在读取的缓冲；加载耗时包含查找、CRC 和分配；解码统计含 UI 的其他图片。
这些值不是同时采样的整机内存账本，也不含显示、DMA、任务和所有临时内存。

小型索引自动尝试缓存在 PSRAM（最多 64 KiB），没有 PSRAM 或分配困难时分块读取。
图片分配可以回收可选索引缓存。播放吞吐量取决于存储读取延迟、解码和显示提交耗时，
应使用目标存储和实际素材测量各阶段开销。

## 关闭、更新和存储边界

关闭必须检查返回结果，默认最多等待 5 秒，或指定等待预算：

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

超时不强制终止驱动读取或释放在途数据。关闭中的句柄拒绝新显示请求，仍可查询和
重试关闭；成功关闭前保持 UI 处理命令/解码，其他调用者应结束对该句柄的使用。
关闭包后，已显示源数据仍在 UI 替换/销毁时释放。调整读取预算同样通过关闭、重新
打开完成。

同名且目标契约兼容的素材可独立更新；新增名称可用 `show_name()` 或目标名称入口，
不依赖旧帧数/位置。文件使用期间保持不变，关闭后替换再打开。匹配构建快速校验
索引，合法替换包分块校验全包一次，随后按成员校验。连续校验避免每个小块重新
定位，并定期让出 CPU；首次验证仍有存储读取成本。CRC 不提供来源认证。

导出逐文件原子替换，资源包最后提交，写失败不会截断已有资源包；这不是跨文件
事务，构建成功后再部署。

当前接入 ESP-IDF 已挂载的文件系统，SD/NAND 共用接口；裸 NAND 的 ECC、坏块和
磨损管理由存储驱动负责。文件大小受平台 `fseek/ftell` 范围限制，32 位 long 下需
小于 2 GiB，可拆包。该文件服务通过 ESP-IDF API 使用，PC 桥接没有对应端点。

`examples/external_assets` 提供 SDSPI 挂载、板级引脚配置、BSP 已挂载 SDMMC/NAND
入口及内置占位图。挂载失败不会格式化存储。使用 GSPC 0.4.1 并配套 ESP-GSP 1.3.1。

## SD 字体

`esp_gsp_font_file.h` 提供启动前的字体文件加载。动态 TTF/OTF/TTC 与 `gspc font-link`
生成的字体目录包共用 `open → apply → close`，按文件签名识别。`max_bytes` 必须非零，
超过上限在分配前拒绝；文件整体驻留 RAM，ESP-IDF 优先使用 PSRAM。这不是按需读取字形，
上限只约束文件字节数，仍需预留对齐、句柄、FreeType 和字形缓存开销。

动态字体的源码工程先调用 `gsp_enable_freetype()`，再从已挂载的文件系统加载：

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
调用 `esp_gsp_font_file_close(font)`；停止超时时保留句柄继续等待。释放后的旧配置不可
直接复用。`apply()` 只准备启动配置，不热替换运行中的字体。动态字体在此检查签名，
完整字体解析由 UI 启动时的 FreeType 完成，TTC 使用第一个字面；目录包在加载时校验 CRC。

固定字形需要外置时，场景使用 `"font_link":"external"`，再链接资源包：

```sh
gspc pack scenes/app.json -o build/app.gspb
gspc font-link build/app.gspb --output-dir build/linked --catalog build/fonts.gspb
```

应用使用 `build/linked/app.gspb`，将 `fonts.gspb` 放入 SD 卡，并用同一组字体文件接口
加载、应用目录包。原始未链接包仍包含字形，不能据此认为字体已外置。目录包设置
`app.font_catalog`，动态字体设置 `app.ttf/ttf_size`；可以分别加载两个句柄，依次应用，
共同提供固定字形和动态补字。两类句柄都需保持到 UI 停止。

字体 `open()` 返回时文件已经关闭。若没有图片资源服务等其他使用者，随后可以卸载 SD；
UI 继续使用 RAM 中的字体。与图片／动画的逐帧文件读取生命周期不同。
