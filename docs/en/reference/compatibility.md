# Compatibility contract

ESP-GSP and GSPC have independent product versions. Compatibility is decided
from the versioned binary-format, requirements, configuration, and runtime ABI
contract instead of comparing those product-version strings. CMake runs
`gspc compatibility` during project configuration and stops immediately when
the selected executable cannot generate artifacts accepted by this component.

Compiled bundles are opaque artifacts. Applications must create them with the
supported GSPC release and consume them through generated headers and public
runtime APIs; parsing, editing, or constructing bundle internals is unsupported.

| Item | Current value | Link |
|---|---:|---|
| ESP-GSP component | `1.6.0` | [Component Registry](https://components.espressif.com/components/espressif/esp-gsp/versions/1.6.0/readme) |
| ESP-GSP simulator | `1.6.0` | [Binary (manifest)](https://dl.espressif.com/AE/gsp/sim/v1.6.0/manifest.json) |
| Recommended GSPC | `0.7.0` | [Binary (manifest)](https://dl.espressif.com/AE/gsp/gspc/v0.7.0/manifest.json) |
| GSB | `2.7` |  |
| GRB | `1.4` |  |
| GFB | `1.2` |  |
| GSPB | `1.2` |  |
| GSPB deployable | `1.4` |  |
| GSR | `1.0` |  |
| GMD | `1.2` |  |
| GSR requirements record | `2` |  |
| Configuration Schema / ABI | `2 / 3` |  |
| GMD runtime ABI | `1.0` |  |
| Simulator ABI | `1` |  |

## Prebuilt downloads

The release pipeline builds and signs the following host archives. These links
download the archive directly. Each archive has a detached Ed25519 signature at
the same URL with a `.sig` suffix; use `esp-gsp-tools` as shown below when you
want automatic platform selection, signature verification, and caching.

| Platform | GSPC `0.7.0` | Simulator `1.6.0` |
|---|---|---|
| Linux ARM64 | [gspc-0.7.0-linux-aarch64.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.7.0/gspc-0.7.0-linux-aarch64.tar.gz) | [sim-1.6.0-linux-aarch64.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.6.0/sim-1.6.0-linux-aarch64.tar.gz) |
| Linux x86_64 | [gspc-0.7.0-linux-x86_64.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.7.0/gspc-0.7.0-linux-x86_64.tar.gz) | [sim-1.6.0-linux-x86_64.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.6.0/sim-1.6.0-linux-x86_64.tar.gz) |
| Windows x86_64 | [gspc-0.7.0-windows-x86_64.zip](https://dl.espressif.com/AE/gsp/gspc/v0.7.0/gspc-0.7.0-windows-x86_64.zip) | [sim-1.6.0-windows-x86_64.zip](https://dl.espressif.com/AE/gsp/sim/v1.6.0/sim-1.6.0-windows-x86_64.zip) |
| Windows ARM64 | [gspc-0.7.0-windows-aarch64.zip](https://dl.espressif.com/AE/gsp/gspc/v0.7.0/gspc-0.7.0-windows-aarch64.zip) | [sim-1.6.0-windows-aarch64.zip](https://dl.espressif.com/AE/gsp/sim/v1.6.0/sim-1.6.0-windows-aarch64.zip) |
| macOS Universal 2 | [gspc-0.7.0-macos-universal2.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.7.0/gspc-0.7.0-macos-universal2.tar.gz) | [sim-1.6.0-macos-universal2.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.6.0/sim-1.6.0-macos-universal2.tar.gz) |

Run `python -m gsp.execute --version '<GSPC version>' gspc compatibility` after
installing `esp-gsp-tools` to inspect the selected compiler. The command returns
machine-readable JSON including its build commit. CMake selects the version in
the component `.gspc_version`, unless the IDF project root contains an
overriding marker; `GSPC_EXECUTABLE` remains the manual override. When JPEG is
disabled, CMake additionally requires the compiler's `jpeg_disable_constraint`
feature. A matching version or binary-format contract alone does not imply
support for `--disable-jpeg`; an older compiler is rejected during configuration.

Each Registry prebuilt library includes `prebuilt/<variant>/build-info.json`.
This machine-readable file identifies the component source, ESP-IDF and
compiler revisions, target revision range, effective optimization flags, and
archive SHA-256. Applications do not parse it at runtime; it is available for
package audits and compatibility diagnosis.

## Tool commands

The widget guides use `gspc` and `gsp_sim_host`. If compatible executables are
already on PATH, use them directly. Otherwise copy this setup once into a POSIX
shell; these functions select the versions in the table above for this session:

```sh
python -m pip install -U esp-gsp-tools
gspc() { python -m gsp.execute --version 0.7.0 gspc "$@"; }
gsp_sim_host() { python -m gsp.execute --version 1.6.0 sim "$@"; }
```

In other shells, use `python -m gsp.execute --version VERSION gspc` or
`python -m gsp.execute --version VERSION sim`, substituting the respective
version from the table. Keep project-specific version overrides when reproducing
an existing application.
