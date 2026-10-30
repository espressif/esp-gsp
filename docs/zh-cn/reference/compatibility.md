# 兼容性契约

ESP-GSP 与 GSPC 使用独立的产品版本。二者是否兼容，不靠比较产品版本字符串，
而是依据带版本的二进制格式、需求记录、配置和运行时 ABI 契约判断。工程配置
阶段会执行 `gspc compatibility`；声明的基线契约不匹配时，CMake 会立即停止并给出
不匹配项。场景所需的功能变体应使用下表推荐的编译器和运行时组合。

GSPC 0.10.0 增加了 ESP-GSP 1.9.0 使用的动态下拉框文案和可选控件几何能力。
旧工具即使通过基线格式检查，也不代表支持这些编写字段。使用组件包推荐的编译器
标记；旧工程若固定了其他版本，使用新字段前应更新工程标记。

编译后的 Bundle 属于不透明产物。应用必须使用受支持的 GSPC 版本生成 Bundle，
并通过生成头文件和公共运行时 API 使用；不支持解析、修改或自行构造其内部内容。

二进制格式行在两者不同时显示**基线版本 / 最高已声明功能变体**。编译器按实际使用
的功能选择文件版本，不会将所有产物统一写成最高变体版本。

| 项目 | 当前值 | 链接 |
|---|---:|---|
| ESP-GSP 组件 | `1.9.0` | [Component Registry](https://components.espressif.com/components/espressif/esp-gsp/versions/1.9.0/readme) |
| ESP-GSP 模拟器 | `1.9.0` | [下载链接 (manifest)](https://dl.espressif.com/AE/gsp/sim/v1.9.0/manifest.json) |
| 推荐 GSPC | `0.10.0` | [下载链接 (manifest)](https://dl.espressif.com/AE/gsp/gspc/v0.10.0/manifest.json) |
| GSB | `2.7 / 2.22` |  |
| GRB | `1.4 / 1.9` |  |
| GFB | `1.2` |  |
| GSPB | `1.2 / 1.4` |  |
| GSPB Deployable | `1.4` |  |
| GSR | `1.0` |  |
| GMD | `1.2 / 1.4` |  |
| GSR 需求记录（契约 / 输出） | `2 / 3` |  |
| 配置 Schema / ABI | `2 / 3` |  |
| GMD 运行时 ABI | `1.0` |  |
| 模拟器 ABI | `1` |  |

## GSB 版本与场景兼容范围

本版 GSB 的基线是 `2.7`，由 `gspc compatibility` 返回并由 CMake 检查。
实际功能版本还包括：

- 编译器生成实例绘制顺序（`DRAW_INSTANCES`）时使用 GSB `2.20`。
- 使用动态命中几何时生成 GSB `2.21`，并包含 REQUIRED `HIT_GEOMETRY` 小节。
- 不使用这些功能的场景仍可采用基线或更早的功能变体。GSB 版本属于每个场景，同一
  bundle 内的场景可以具有不同的 minor 版本。

GSB `2.22` 增加整图矢量透明度，GRB `1.6` 增加原生动画补丁。GRB `1.8` 增加预编译 RGB565 动画 Span。需要 ESP-GSP／simulator 1.8.0 及以上、GSPC 0.9.0 及以上；1.7.0／0.8.0 组合不支持。请使用配套的编译器与运行时。

运行时继续接受此前已支持的 GSB 2.x 场景，同时校验小节、操作码、资源和目标 Profile
需求以及运行时已启用的能力。新的实例绘制顺序和动态命中场景需要配套编译器与
运行时／模拟器；旧运行时会拒绝不支持的操作码或未知 REQUIRED 小节。应面向目标
运行时重新生成受影响的场景，不要修改产物版本头。单凭基线匹配不能证明新场景可在旧运行时运行。当前编译器通过
`format_variants` 单独列出功能版本，读取端仍须支持对应的运行时能力。

复现 1.6.0 应用时，继续使用 GSPC 0.7.0 与 simulator 1.6.0。使用本版场景功能时，
按[升级说明](../../../CHANGELOG.md#upgrading-from-170)用 GSPC 0.10.0 重新生成，
并使用 ESP-GSP／simulator 1.9.0。

GSR 需求记录行区分 `gspc compatibility` 返回的基线与当前编译器生成的、包含内容
容量的记录版本。GSR 容器版本、公共 ABI 和运行时 ABI 分别独立。

## 预编译文件下载

发布流水线为以下宿主平台构建并签名归档。表中链接会直接下载对应文件；每个归档
都在相同 URL 的 `.sig` 后缀地址提供独立 Ed25519 签名。需要自动选择平台、校验签名
和缓存文件时，建议使用下文的 `esp-gsp-tools`。

| 平台 | GSPC `0.10.0` | 模拟器 `1.9.0` |
|---|---|---|
| Linux ARM64 | [gspc-0.10.0-linux-aarch64.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.10.0/gspc-0.10.0-linux-aarch64.tar.gz) | [sim-1.9.0-linux-aarch64.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.9.0/sim-1.9.0-linux-aarch64.tar.gz) |
| Linux x86_64 | [gspc-0.10.0-linux-x86_64.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.10.0/gspc-0.10.0-linux-x86_64.tar.gz) | [sim-1.9.0-linux-x86_64.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.9.0/sim-1.9.0-linux-x86_64.tar.gz) |
| Windows x86_64 | [gspc-0.10.0-windows-x86_64.zip](https://dl.espressif.com/AE/gsp/gspc/v0.10.0/gspc-0.10.0-windows-x86_64.zip) | [sim-1.9.0-windows-x86_64.zip](https://dl.espressif.com/AE/gsp/sim/v1.9.0/sim-1.9.0-windows-x86_64.zip) |
| Windows ARM64 | [gspc-0.10.0-windows-aarch64.zip](https://dl.espressif.com/AE/gsp/gspc/v0.10.0/gspc-0.10.0-windows-aarch64.zip) | [sim-1.9.0-windows-aarch64.zip](https://dl.espressif.com/AE/gsp/sim/v1.9.0/sim-1.9.0-windows-aarch64.zip) |
| macOS Universal 2 | [gspc-0.10.0-macos-universal2.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.10.0/gspc-0.10.0-macos-universal2.tar.gz) | [sim-1.9.0-macos-universal2.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.9.0/sim-1.9.0-macos-universal2.tar.gz) |

上表是当前推荐组合。安装 `esp-gsp-tools` 后可运行
`python -m gsp.execute --version '<GSPC version>' gspc compatibility` 查看机器可读
能力及构建 commit。CMake 默认读取组件的 `.gspc_version`；IDF 工程根目录下的同名文件
优先级更高，`GSPC_EXECUTABLE` 保留为手动覆盖方式。关闭 JPEG 时，CMake 还会检查
编译器是否声明 `jpeg_disable_constraint` 能力。版本字符串或二进制格式匹配不代表支持
`--disable-jpeg`；不支持该能力的旧编译器会在配置阶段被明确拒绝。

Registry 组件中的每个预编译库都带有 `prebuilt/<variant>/build-info.json`。
该机器可读文件记录组件源码、ESP-IDF 与编译器版本、目标芯片 revision 范围、实际
优化选项和库文件 SHA-256，供包审计和兼容性诊断使用，应用运行时无需解析它。

## 工具命令

控件指南使用 `gspc` 和 `gsp_sim_host` 命令。PATH 中已有兼容可执行文件时可直接使用；
否则在 POSIX shell 中复制执行一次下列配置，函数会为当前终端选择上表配套版本：

```sh
python -m pip install -U esp-gsp-tools
gspc() { python -m gsp.execute --version 0.10.0 gspc "$@"; }
gsp_sim_host() { python -m gsp.execute --version 1.9.0 sim "$@"; }
```

其他 shell 可使用 `python -m gsp.execute --version VERSION gspc` 或
`python -m gsp.execute --version VERSION sim`，将 VERSION 替换为上表对应版本。
复现已有应用时，应保留该工程明确指定的版本覆盖。
