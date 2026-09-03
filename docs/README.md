# ESP-GSP Documentation

- [English documentation](en/README.md)
- [中文文档](zh-Hans/README.md)

## Directory contract

| Path | Owner | Maintenance rule |
|---|---|---|
| `docs/en/` | English user documentation | User workflows and technical reference |
| `docs/zh-Hans/` | 中文用户文档 | 与英文 Guide、Component 和 Reference 路径一一对应 |
| `docs/_generator/` | Maintenance infrastructure | Keeps Widget, authoring, inventory and public-function references aligned with code |
| `examples/widgets/` | Executable documentation examples | Exactly one compilable JSON example for every registered Widget |
| `skills/esp-gsp-ui/` | Installable Codex workflow | Contains procedure only; reads facts from GSPC, public headers, docs and examples |

The repository README is the product landing page. This file is only the
language selector and documentation architecture contract; it does not repeat
installation or API instructions.
