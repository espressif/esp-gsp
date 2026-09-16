# UI 特效预览

本例用于直接预览和挑选特效，默认不自动切页。点击 **Next** 切换效果类别，
点击三个样式按钮选择风格；下方亮线表示当前选项。

| 页面 | 可选样式 |
|---|---|
| Charging orb | Liquid（水波）、Halo（蓝色光晕）、Plasma（彩色光晕） |
| Soft ring | Solid（实线）、Glow（柔光）、Trail（渐隐拖尾） |
| Icon carousel | Flat（平面）、Tilt（倾斜）、Depth（透视） |
| Soft pulse | Quiet、Soft、Bright（三档强度和节奏） |
| SVG morph | Slow、Normal、Fast（三档速度） |
| Highlight sweep | Linear（线性）、Soft（柔和）、Slant（斜向） |
| Touch glow | Blue、Teal、Rose（三种颜色） |
| Flip card | Flat（平面）、Soft（轻透视）、Depth（透视） |
| Frosted glass | Mist、Frost、Smoke（三种静态材质） |

充电页点击 **Stop** 关闭充电动态和粒子，保留静态光环与电量；点击 **Charge** 恢复。
Ring、脉冲和 SVG 页面使用 **Pause / Play**。轮盘由拖动或选中项驱动，支持点击和松手吸附，不显示暂停按钮。
样式切换使用批量可见性更新，避免两种样式短暂重叠。

业务接入可从[最小可运行示例](minimal/README.md)开始。

## 先用模拟器预览

使用 GSPC 0.4.1，并配套 ESP-GSP 和模拟器 1.3.1。
在仓库根目录执行：

```sh
export GSPC_EXECUTABLE="$PWD/ci/gspc-dev"
GSP_SIM_APPLICATION_SOURCE="$PWD/ci/host/effects_sim_application.c" \
  bash tools/sim/run_scene.sh examples/effects/scenes/effects_320.json
```

应用钩子与硬件示例共用按钮控制逻辑。无应用钩子时只显示初始页面。
使用模拟器预览观感、交互和像素，在目标板上测量帧率与内存。

## 充电球配置

```json
{
  "type": "charging_orb", "parent": -1, "name": "charge",
  "x": 24, "y": 24, "w": 180, "h": 180,
  "style": "halo", "value": 62, "charging": true,
  "fg_color": "#69CEF6", "period_ms": 4200,
  "strength": 85, "softness": 90, "ripple": 55, "particles": 4
}
```

- `style`：`liquid / halo / plasma`，默认 `halo`。
- `fg_color`：主色；`plasma` 叠加预设的柔和彩色变化。
- `strength`：光晕强度，0..100；`softness`：边缘柔和程度，0..100。
- `period_ms`：周期，100..60000 ms；较慢的周期适合充电状态。
- `ripple`：贴附光环的膜状波纹强度，0..100；0 关闭，40..60 较自然。
- `particles`：Halo/Plasma 的能量粒子数量，0..6，默认 4；设为 0 可关闭。
- `charging`：控制充电动态和粒子；实际充电状态由应用提供，不读取充电硬件。
- `value`：0..100。Liquid 用它控制液面；电量数字、单位使用独立 Label，自由排版。

默认在场景中配置外观；启用 `runtime_style` 后可运行时调节下文列出的参数。
进度和播放状态直接使用生成接口，例如本例：

```c
gsp_effects_charge_halo_set_value(ui, 62);
gsp_effects_level_set_text(ui, "62%");
gsp_effects_charge_halo_set_charging(ui, true);
/* Stop charging: remove moving particles and keep a static halo. */
gsp_effects_charge_halo_set_charging(ui, false);
```

`playing` 保留为通用播放接口；充电球的 `charging` 与它共享状态。
场景中同时声明两者时，以 `charging` 为准。
隐藏页面会暂停动画，不在应用中维护逐帧绘制循环。

## Ring、脉冲与轮盘

Ring 使用 `type: "effect"`、`effect: "ring"`，通过
`ring_style: "solid" / "glow" / "trail"` 选择实线、柔光或渐隐拖尾。
`strength`、`softness`、`period_ms` 调整光晕和节奏；Ring 保持圆弧轮廓，不产生波纹；`value` 调整活动弧段长度。
脉冲使用 `effect: "pulse"`，相同参数控制柔和呼吸。

轮盘使用 `type: "carousel"` 和 3..12 个 `icons` 图片路径：

```json
{
  "type": "carousel", "parent": -1, "name": "menu",
  "x": 16, "y": 40, "w": 288, "h": 168,
  "icons": ["home.svg", "music.svg", "water.svg"],
  "projection": "perspective", "depth": 60, "spacing": 45,
  "selected": 0, "callback": "selected"
}
```

`projection` 为 `flat / tilt / perspective`。`depth` 与 `spacing` 范围均为 0..100。
倾斜模式使用较低开销的采样，透视模式使用透明边缘双线性采样。
图标在构建期生成缩略图；运行时复用它们，并按投影轮廓进行点击测试。
吸附复用框架动画池，通过有限时长缓动结束；新输入可中断。

## 硬件配置

本例提供 S3 SPI、C3 SPI、S3 QSPI、P4 MIPI-DSI、S31 RGB565/RGB888 配置。
构建命令见[英文说明](README.md#hardware)。每个配置使用独立的 build/sdkconfig。
S31 使用 `idf.py --preview`，RGB888 增加 `-D GSP_EFFECTS_RGB888=ON`。

`GSP_EFFECTS_AUTOPLAY=ON` 开启自动预览：每十秒切页，每两秒切换样式。
默认 OFF，方便手动观察。C3 SPI 与 S31 RGB888 的共享板级实现没有触摸，可用自动模式预览。

特效宽高上限 512；充电球、光环、脉冲使用正方形。轮盘最小边长 32，缩略图最大边长
128。特效声明为场景控件；透视用于轮盘图标和卡片，玻璃模糊在构建期烘焙，
粒子数量由效果参数指定。
RGB565 使用稳定的空间抖动减轻光晕色阶，但仍受面板色深和采样分辨率限制。

串口报告实际时间窗口内的帧数、平均帧生产耗时、光栅化/提交时间、错误与内部堆。
平均 FPS 包含空闲时间；测量轮盘运动时，应选取动画活动时间窗口。

## 外观与开销的取舍

充电球、Ring、脉冲和扫光由运行时计算；轮盘和翻转卡片只预处理静态图片，
投影和动画在运行时计算，不存储整段动画帧。毛玻璃的模糊材质则完全在构建期生成。

外观参数默认在场景中设置；启用 `runtime_style` 后可动态调节。
运行时也可直接更新电量、充电状态、选中项等属性。
切换完整风格可参考本例：预配置不同样式，通过容器可见性批量切换。
轮盘只需替换 `icons` 中的图片路径；同尺寸、同内容缩略图在资源包中去重。
示例图标采用统一圆角、顶部高光和清晰符号。SVG 在构建期栅格化；纹理的颜色、透明边缘仍会影响运行时采样与混合成本。

自己的图片可通过轮盘的 `icons`、卡片的 `front` / `back` 或玻璃背景 Image 的 `image`
路径导入，路径相对场景 JSON。支持 PNG、JPEG 和 GSP 支持的自包含 SVG 子集；
单张源文件不超过 2 MiB。编译器自动缩放，无需手动制作每帧图片。
轮盘和卡片的图片随资源包编译，当前没有从 SD 卡或网络运行时替换这些图片的专用接口。
充电球通过参数调整外观，不提供球体贴图皮肤接口。

- 充电球先从 120..180 px 开始，根据目标板实测再放大；重绘面积随边长平方增长。
- `particles: 0` 可去掉粒子，`ripple: 0` 可去掉膜状波纹；光晕本身仍有逐像素开销。
- 轮盘优先使用 5 个图标；`tilt` 提供较低采样成本，`perspective` 的透明边缘更平滑。
- 图标缩略图边长最多 128。原始像素资源约为 `数量 × 边长² × 3` 字节（RGB565+A8）
  或 `数量 × 边长² × 4` 字节（ARGB8888），另有行对齐与资源元数据；这不是每帧新增堆分配。
- 暂停、隐藏和吸附结束后停止连续动画刷新。放慢 `period_ms` 只改变动作速度，不等同于降低刷新频率。

效果渲染使用有界行缓冲，不分配整幅模糊中间图；仍须为渲染任务保留充足栈空间。
RGB565 的有限色深仍可能显露轻微色阶，不能保证与 RGB888 完全相同的观感。

预览控制与统计使用 GSP 定时回调，与触摸事件串行执行；主任务初始化后退出，不保留独立的轮播任务。

## 新增预览：扫光、触摸柔光、翻转与毛玻璃

Next 依次切换九类预览。新增页面提供以下选择：

| 页面 | 样式 | 操作 |
|---|---|---|
| Highlight sweep | Linear / Soft / Slant | 暂停或播放扫光 |
| Touch glow | Blue / Teal / Rose | 轻点 TOUCH，释放后柔光淡出 |
| Flip card | Flat / Soft / Depth | 点击卡片或 Flip 翻面，可在途中反向 |
| Frosted glass | Mist / Frost / Smoke | 切换静态玻璃材质 |

柔和扫光使用 `effect: "shimmer"`，设置 `shimmer_style: "soft"` 或
`"diagonal"`。`strength`、`softness`、`period_ms` 控制亮度、宽度和周期；
未指定样式时保留原有 `linear` 行为。

触摸柔光由 Button 与暂停的 Pulse 组成，通过下文的 `play_glow` 动作关联，无需应用点击回调。
按钮按住时仍使用原生按下反馈，柔光在点击释放后触发。

### 翻转卡片

```json
{
  "type": "flip_card", "name": "card", "parent": -1,
  "x": 20, "y": 30, "w": 200, "h": 140,
  "front": "front.svg", "back": "back.png",
  "flipped": false, "depth": 60, "period_ms": 600
}
```

卡片默认可点击翻面，也可调用生成的 `set_flipped(ui, true/false)` 控制。`enabled: false` 禁用点击；可选 `callback` 在点击翻面后通知应用（显式事件使用 `event: "value"`）。`period_ms` 是一次翻转的时间，
`depth` 范围 0..100。翻转复用动画池，不需要应用逐帧更新；再次设置目标会从当前姿态反向；隐藏时直接落到目标面，不保留不可见的翻转动画。
图片在构建期缩放为最大边长 256 的透明缩略图，支持 32..512 的控件边界；建议从
160..220 px 的卡片宽度开始测量，再根据目标板的帧耗时和内存余量调整尺寸。

### 静态背景毛玻璃

```json
{
  "screen": "glass_demo", "w": 320, "h": 240,
  "objects": [
    {"type": "image", "name": "wall", "parent": -1,
     "x": 0, "y": 0, "w": 320, "h": 240, "image": "wallpaper.png", "fit": "cover"},
    {"type": "glass", "parent": -1, "backdrop": "wall",
     "x": 40, "y": 50, "w": 240, "h": 140,
     "blur": 14, "tint_color": "#D8EAFF", "tint_opacity": 65, "radius": 16}
  ]
}
```

背景图片必须先声明，与玻璃具有相同父节点，裁剪范围须位于背景内；背景须不透明，
采用居中对齐，不使用旋转、SVG 元素选择或变形。`blur` 为构建期模糊半径（0..32），
`tint_opacity` 为染色强度（0..255），`radius` 为圆角（0..128）。
背景最大 2048×2048，玻璃最大 512×512；源图片各不超过 2 MiB。

模糊、染色和圆角在构建时完成，运行时只绘制原生透明图片。玻璃不会自动模糊其后的
其他控件，也不会随背景图片的运行时替换更新。需要换主题时，可以像本例一样预配置材质并切换可见性。
RGB565 对新烘焙材质的渐变采用空间抖动，色深限制仍可能产生轻微纹理。

新翻转卡片和柔和扫光使用 GSB 2.11；已有特效仍保持 GSB 2.10，普通图片/SVG 场景不升级格式。
静态玻璃本身使用已有图片命令。Halo 同时关闭 `ripple` 和 `particles` 后停止无意义的相位推进。

毛玻璃的不透明主体与圆角边缘分开编码，减少透明通道存储，并复用原生不透明拷贝路径。

玻璃单独移动、缩放或背景内容变化时，不会重新取样；如需移动整组内容，可移动其共同父容器。

## 运行时调节外观与图片组

对需要调节外观的 `effect`、`charging_orb`、`carousel` 或 `flip_card` 设置
`runtime_style: true` 和稳定的 `name`，GSPC 会生成对应 getter/setter。
未启用的控件不分配这些额外属性；完整风格类型仍在构建期选择。

| 控件 | 可运行时调节的外观 |
|---|---|
| 充电球 Halo / Plasma | `fg_color`、`strength`、`softness`、`ripple`、`particles`、`opacity`、`period_ms` |
| 脉冲、柔光 Ring、柔和／斜向扫光 | `fg_color`、`strength`、`softness`、`opacity`、`period_ms` |
| Liquid、实线 Ring、线性扫光 | `fg_color`、`opacity`、`period_ms` |
| 轮盘 | `depth`、`spacing`、`opacity`、`period_ms`（吸附时间） |
| 翻转卡片 | `depth`、`opacity`、`period_ms`（翻转时间） |

参数范围与构建期相同，周期为 100..60000 ms。新的周期用于之后启动的卡片／轮盘过渡；
已有过渡完成后保持选中状态。新增 `set_<属性>_rgb888()` 统一接收 `0xRRGGBB`，
自动适配场景格式；特效前景色使用 `set_fg_color_rgb888()`。旧 setter/getter 仍使用
原生颜色值。透明度单独设置，新接口不接受 alpha 字节。背景颜色仍是构建期配置。

例如本例的脉冲页用一个实例切换三种风格：

```c
ESP_ERROR_CHECK(gsp_effects_pulse_set_strength(ui, 45));
ESP_ERROR_CHECK(gsp_effects_pulse_set_period_ms(ui, 4500));
ESP_ERROR_CHECK(gsp_effects_card_set_depth(ui, 80));
ESP_ERROR_CHECK(gsp_effects_card_set_image_set(ui, 1));
```

需要主题换图时，在 `icons` 或 `front` / `back` 之外增加 `image_sets`：

```json
{
  "type": "flip_card", "name": "card", "w": 200, "h": 140,
  "front": "front.png", "back": "back.png", "runtime_style": true,
  "image_sets": [["front_dark.png", "back_dark.png"]], "image_set": 0
}
```

第 0 组是原始图片；最多追加三组，每组图片数量必须与原始组相同。
`image_set` 可设置初始组，`set_image_set()` 在运行时切换，不改变选中项或翻转进度。
换图本身不要求 `runtime_style`。编译器使用相同尺寸生成各组纹理，并复用重复资源。
额外图片增加资源包和资源描述表大小，运行时不重新解码、不分配过渡帧。
`image_sets` 切换构建期打包的图库和主题皮肤。

点击柔光不再需要应用回调，在按钮上配置：

```json
{
  "type": "button", "w": 100, "h": 36, "text": "TOUCH",
  "bg_color": "#244766", "fg_color": "#E8FAFF",
  "events": [{"event": "click", "action": "play_glow", "target_name": "feedback", "arg": 850}]
}
```

`feedback` 是已声明且有名称的 Pulse，设为 `playing: false, value: 0`。
`arg` 是可选的渐隐时长（默认 850 ms，范围 100..60000）；重复点击重新触发，
隐藏后自动归零并释放动画槽，按钮禁用时不会触发。按住按钮仍使用原生按下反馈。
本例点击光效页面直接使用该动作；脉冲、点击柔光、翻转页均只保留一个实例。

这些控制能力使用 GSB 2.12，需配套 GSPC 0.4.1 和 ESP-GSP 1.3.1。
