# 兼容性契约

ESP-GSP 与 GSPC 使用独立的产品版本。二者是否兼容，不靠比较产品版本字符串，
而是依据带版本的二进制格式、需求记录、配置和运行时 ABI 契约判断。工程配置
阶段会执行 `gspc compatibility`；所选编译器无法生成当前组件可接受的产物时，
CMake 会立即停止并给出不匹配项，不会把问题推迟到设备运行阶段。

编译后的 Bundle 属于不透明产物。应用必须使用受支持的 GSPC 版本生成 Bundle，
并通过生成头文件和公共运行时 API 使用；不支持解析、修改或自行构造其内部内容。

| 项目 | 当前值 | 链接 |
|---|---:|---|
| ESP-GSP 组件 | `1.1.0` | [Component Registry](https://components.espressif.com/components/espressif/esp-gsp/versions/1.1.0/readme) |
| ESP-GSP 模拟器 | `1.1.0` | [下载链接 (manifest)](https://dl.espressif.com/AE/gsp/sim/v1.1.0/manifest.json) |
| 推荐 GSPC | `0.2.8` | [下载链接 (manifest)](https://dl.espressif.com/AE/gsp/gspc/v0.2.8/manifest.json) |
| GSB | `2.6` |  |
| GRB | `1.4` |  |
| GFB | `1.2` |  |
| GSPB | `1.2` |  |
| GSPB Deployable | `1.4` |  |
| GSR | `1.0` |  |
| GMD | `1.1` |  |
| GSR 需求记录 | `2` |  |
| 配置 Schema / ABI | `2 / 3` |  |
| GMD 运行时 ABI | `1.0` |  |
| 模拟器 ABI | `1` |  |

上表是当前推荐组合。安装 `esp-gsp-tools` 后可运行
`python -m gsp.execute --version '<GSPC version>' gspc compatibility` 查看机器可读
能力及构建 commit。CMake 默认读取组件的 `.gspc_version`；IDF 工程根目录下的同名文件
优先级更高，`GSPC_EXECUTABLE` 保留为手动覆盖方式。
