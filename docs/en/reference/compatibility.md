# Compatibility contract

ESP-GSP and GSPC have independent product versions. Compatibility is decided
from the versioned binary-format, requirements, configuration, and runtime ABI
contract instead of comparing those product-version strings. During project
configuration, CMake runs `gspc compatibility` and rejects a mismatch in the
declared baseline contract. Select the recommended compiler/runtime pair below
for the feature variants used by the scene.

Compiled bundles are opaque artifacts. Applications must create them with the
supported GSPC release and consume them through generated headers and public
runtime APIs; parsing, editing, or constructing bundle internals is unsupported.

Binary-format rows show **baseline / highest declared feature variant** when
these differ. The compiler selects the file version from the features used;
it does not write the highest variant into every artifact.

| Item | Current value | Link |
|---|---:|---|
| ESP-GSP component | `1.8.0` | [Component Registry](https://components.espressif.com/components/espressif/esp-gsp/versions/1.8.0/readme) |
| ESP-GSP simulator | `1.8.0` | [Binary (manifest)](https://dl.espressif.com/AE/gsp/sim/v1.8.0/manifest.json) |
| Recommended GSPC | `0.9.0` | [Binary (manifest)](https://dl.espressif.com/AE/gsp/gspc/v0.9.0/manifest.json) |
| GSB | `2.7 / 2.22` |  |
| GRB | `1.4 / 1.8` |  |
| GFB | `1.2` |  |
| GSPB | `1.2 / 1.4` |  |
| GSPB deployable | `1.4` |  |
| GSR | `1.0` |  |
| GMD | `1.2 / 1.3` |  |
| GSR requirements record (contract / output) | `2 / 3` |  |
| Configuration Schema / ABI | `2 / 3` |  |
| GMD runtime ABI | `1.0` |  |
| Simulator ABI | `1` |  |

## GSB versions and scene compatibility

For this contract, GSB `2.7` is the baseline reported by
`gspc compatibility` and checked by CMake. It is not the feature ceiling:

- GSB `2.20` is selected for compiler-emitted instance paint order
  (`DRAW_INSTANCES`).
- GSB `2.21` is selected for dynamic hit geometry and includes the
  REQUIRED `HIT_GEOMETRY` section.
- Scenes without these features can retain the baseline or an earlier feature
  variant. The GSB version belongs to each scene, so one bundle can contain
  scenes with different minor versions.

GSB `2.22` adds whole-vector opacity; GRB `1.6` adds native animation patches. GRB `1.8` adds precompiled RGB565 animation spans. They require ESP-GSP/simulator 1.8.0 or later and GSPC 0.9.0 or later; the 1.7.0/0.8.0 pair does not support them. Use a matched compiler/runtime pair.

The runtime continues to accept earlier supported GSB 2.x scenes, subject to
their sections, opcodes, resource/profile requirements and enabled runtime
capabilities. New instance-order and dynamic-hit scenes require the matching
compiler and runtime/simulator; earlier runtimes reject unsupported opcodes or
unknown REQUIRED sections. Regenerate affected scenes for the intended runtime
instead of changing their version header. Baseline fields alone do not prove that a newer scene will work
on an older runtime. Current compilers list feature versions separately in
`format_variants`; the reader still needs the corresponding runtime capability.

For a 1.6.0 application, keep GSPC 0.7.0 and simulator 1.6.0 when reproducing
existing behavior. To use this release's scene features, regenerate with GSPC
0.9.0 and use ESP-GSP/simulator 1.8.0, as described in
the [upgrade notes](../../../CHANGELOG.md#upgrading-from-170).

The GSR record row distinguishes the baseline reported by `gspc compatibility`
from the record emitted by this compiler, which includes content capacities.
The GSR container version and public/runtime ABI versions remain independent.

## Prebuilt downloads

The release pipeline builds and signs the following host archives. These links
download the archive directly. Each archive has a detached Ed25519 signature at
the same URL with a `.sig` suffix; use `esp-gsp-tools` as shown below when you
want automatic platform selection, signature verification, and caching.

| Platform | GSPC `0.9.0` | Simulator `1.8.0` |
|---|---|---|
| Linux ARM64 | [gspc-0.9.0-linux-aarch64.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.9.0/gspc-0.9.0-linux-aarch64.tar.gz) | [sim-1.8.0-linux-aarch64.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.8.0/sim-1.8.0-linux-aarch64.tar.gz) |
| Linux x86_64 | [gspc-0.9.0-linux-x86_64.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.9.0/gspc-0.9.0-linux-x86_64.tar.gz) | [sim-1.8.0-linux-x86_64.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.8.0/sim-1.8.0-linux-x86_64.tar.gz) |
| Windows x86_64 | [gspc-0.9.0-windows-x86_64.zip](https://dl.espressif.com/AE/gsp/gspc/v0.9.0/gspc-0.9.0-windows-x86_64.zip) | [sim-1.8.0-windows-x86_64.zip](https://dl.espressif.com/AE/gsp/sim/v1.8.0/sim-1.8.0-windows-x86_64.zip) |
| Windows ARM64 | [gspc-0.9.0-windows-aarch64.zip](https://dl.espressif.com/AE/gsp/gspc/v0.9.0/gspc-0.9.0-windows-aarch64.zip) | [sim-1.8.0-windows-aarch64.zip](https://dl.espressif.com/AE/gsp/sim/v1.8.0/sim-1.8.0-windows-aarch64.zip) |
| macOS Universal 2 | [gspc-0.9.0-macos-universal2.tar.gz](https://dl.espressif.com/AE/gsp/gspc/v0.9.0/gspc-0.9.0-macos-universal2.tar.gz) | [sim-1.8.0-macos-universal2.tar.gz](https://dl.espressif.com/AE/gsp/sim/v1.8.0/sim-1.8.0-macos-universal2.tar.gz) |

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
gspc() { python -m gsp.execute --version 0.9.0 gspc "$@"; }
gsp_sim_host() { python -m gsp.execute --version 1.8.0 sim "$@"; }
```

In other shells, use `python -m gsp.execute --version VERSION gspc` or
`python -m gsp.execute --version VERSION sim`, substituting the respective
version from the table. Keep project-specific version overrides when reproducing
an existing application.
