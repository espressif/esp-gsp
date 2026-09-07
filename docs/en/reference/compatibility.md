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
| ESP-GSP component | `1.2.0` | [Component Registry](https://components.espressif.com/components/espressif/esp-gsp/versions/1.2.0/readme) |
| ESP-GSP simulator | `1.2.0` | [Binary (manifest)](https://dl.espressif.com/AE/gsp/sim/v1.2.0/manifest.json) |
| Recommended GSPC | `0.3.0` | [Binary (manifest)](https://dl.espressif.com/AE/gsp/gspc/v0.3.0/manifest.json) |
| GSB | `2.7` |  |
| GRB | `1.4` |  |
| GFB | `1.2` |  |
| GSPB | `1.2` |  |
| GSPB deployable | `1.4` |  |
| GSR | `1.0` |  |
| GMD | `1.1` |  |
| GSR requirements record | `2` |  |
| Configuration Schema / ABI | `2 / 3` |  |
| GMD runtime ABI | `1.0` |  |
| Simulator ABI | `1` |  |

Run `python -m gsp.execute --version '<GSPC version>' gspc compatibility` after
installing `esp-gsp-tools` to inspect the selected compiler. The command returns
machine-readable JSON including its build commit. CMake selects the version in
the component `.gspc_version`, unless the IDF project root contains an
overriding marker; `GSPC_EXECUTABLE` remains the manual override.
