# 兼容性契约

ESP-GSP 与 GSPC 使用独立的产品版本。二者是否兼容，不靠比较产品版本字符串，
而是依据带版本的二进制格式、需求记录、配置和运行时 ABI 契约判断。工程配置
阶段会执行 `gspc compatibility`；所选编译器无法生成当前组件可接受的产物时，
CMake 会立即停止并给出不匹配项，不会把问题推迟到设备运行阶段。

编译后的 Bundle 属于不透明产物。应用必须使用受支持的 GSPC 版本生成 Bundle，
并通过生成头文件和公共运行时 API 使用；不支持解析、修改或自行构造其内部内容。

| 项目 | 当前值 | 链接 |
|---|---:|---|
| ESP-GSP 组件 | `1.5.0` | [Component Registry](https://components.espressif.com/components/espressif/esp-gsp/versions/1.5.0/readme) |
| ESP-GSP 模拟器 | `1.5.0` | [下载链接 (manifest)](https://dl.espressif.com/AE/gsp/sim/v1.5.0/manifest.json) |
| 推荐 GSPC | `0.6.0` | [下载链接 (manifest)](https://dl.espressif.com/AE/gsp/gspc/v0.6.0/manifest.json) |
| GSB | `2.7` |  |
| GRB | `1.4` |  |
| GFB | `1.2` |  |
| GSPB | `1.2` |  |
| GSPB Deployable | `1.4` |  |
| GSR | `1.0` |  |
| GMD | `1.2` |  |
| GSR 需求记录 | `2` |  |
| 配置 Schema / ABI | `2 / 3` |  |
| GMD 运行时 ABI | `1.0` |  |
| 模拟器 ABI | `1` |  |

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
gspc() { python -m gsp.execute --version 0.6.0 gspc "$@"; }
gsp_sim_host() { python -m gsp.execute --version 1.5.0 sim "$@"; }
```

其他 shell 可使用 `python -m gsp.execute --version VERSION gspc` 或
`python -m gsp.execute --version VERSION sim`，将 VERSION 替换为上表对应版本。
复现已有应用时，应保留该工程明确指定的版本覆盖。
