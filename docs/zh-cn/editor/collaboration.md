# Agent 协作

ESP-GSP Studio 支持基于文件的协作协议，允许 AI Agent（或外部脚本）编辑原生
GSP 场景文件，同时由编辑器提供验证、预览和恢复功能。

编辑器协调工程级的 **交接**（handoff）：它会刷新本地草稿、暂停人类内容编辑、
让外部写入方修改文件，然后在写入方完成后验证并重新加载完整的批次。

## 前提条件

- ESP-GSP Studio 必须正在运行，且已打开目标工程。
- Node.js 22 或更高版本。
- 协作 CLI 脚本 `gsp-agent.cjs`，可从以下位置获取：
  - **已安装的应用** — 位于应用的 `resources/` 目录中。
  - **源码仓库** — 编辑器仓库中的 `scripts/gsp-agent.cjs`。
  - **独立下载** — [下载协作脚本包](https://dl.espressif.com/AE/gsp/gsp-editor/)。

> [!NOTE]
> 如果当前发行版未包含独立脚本包，请使用已安装应用中附带的副本或源码仓库中的
> 脚本。未来版本将提供单独的可下载脚本包。

## 快速上手

```sh
# 1. 检查编辑器状态
node /path/to/gsp-agent.cjs status /path/to/project

# 2. 开始协作会话
node /path/to/gsp-agent.cjs begin /path/to/project
# 成功后打印 batch ID。

# 3. 直接编辑原生 JSON 和资源文件。
#   （协作期间编辑器暂停人类内容编辑。）

# 4. 所有写入完成后，标记批次为可审查状态
node /path/to/gsp-agent.cjs ready /path/to/project <batch-id>

# 5. 完成：验证并重新加载
node /path/to/gsp-agent.cjs finish /path/to/project <batch-id>
```

## CLI 命令

### `status`

```sh
node gsp-agent.cjs status /path/to/project
```

报告当前工程状态、选中的 Bundle/工具链以及是否存在活跃的协作会话。

### `begin`

```sh
node gsp-agent.cjs begin /path/to/project
```

开始新的协作会话。编辑器会刷新所有待保存的人类草稿并进入内容只读模式。
返回一个 **batch ID**，后续命令必须使用该 ID。

- 使用相同的 `requestId` 重试 `begin` 会返回原始结果，不会重复快照。
- 已有的活跃或中断的批次必须先解决后才能开始新批次。

### `ready`

```sh
node gsp-agent.cjs ready /path/to/project <batch-id>
```

声明外部写入方已停止修改文件，并捕获当前文件状态用于审查。`ready` 后编辑器
可以显示修改前后的差异和预览。

如果需要继续修改，使用 `resume` 恢复写入。

### `resume`

```sh
node gsp-agent.cjs resume /path/to/project <batch-id>
```

在 `ready` 后恢复未完成的批次，允许在再次调用 `ready` 或 `finish` 前进行
额外的文件写入。

### `finish`

```sh
node gsp-agent.cjs finish /path/to/project <batch-id>
```

使用选中的 GSPC 和 Bundle 上下文验证完整工程，然后重新加载编辑器。

- `success: true` 表示编译器接受了该批次。
- 需单独检查 `reload.status` — 挂起或失败的重新加载不会撤销已接受的结果。
  如需要可使用 `retry_reload`。
- 验证失败会保留批次及其诊断信息。修复源文件后使用**新的** `requestId` 再次
  调用 `finish`。

### `cancel`

```sh
# 保留修改后的文件：
node gsp-agent.cjs cancel /path/to/project <batch-id>

# 恢复批次前的文件：
node gsp-agent.cjs cancel /path/to/project <batch-id> --restore
```

不带 `--restore` 时，修改后的文件保留在磁盘上。带 `--restore` 时，编辑器
恢复到 `begin` 时的快照。

### `restore`

```sh
node gsp-agent.cjs restore /path/to/project <completed-batch-id>
```

从之前完成的批次恢复文件。如果受影响的文件在批次之后又被修改，则拒绝恢复。

### `batch_status`

在超时或中断后按 batch ID 查询批次状态。即使 `session.json` 已被移除，
终端结果仍可读取。

## 批次范围与限制

- 批次覆盖工程根目录下的常规源文件。
- **排除**：生成的/构建/缓存目录、符号链接、外部源码和资源。
- **快照限制**：128 MiB。
- 工程根目录外的共享资源可以被引用，但不能作为本工程批次的一部分进行编辑。

## 恢复

如果编辑器或 Agent 进程中断：

1. 在 Studio 中重新打开工程，会以 **只读** 模式显示中断的会话。
2. 使用编辑器的 **协作记录与恢复…** 入口检查、完成或恢复批次。
3. CLI 的 `restore` 命令也可以恢复已完成的批次，前提是批次之后没有文件被修改。

恢复前会检查所有已变更的源文件版本。进度有日志记录，恢复中断后可以从断点继续。

## MCP 连接

对于支持 MCP（Model Context Protocol）的 AI Agent，Studio 还提供 MCP stdio
适配器：

```json
{
  "mcpServers": {
    "gsp-editor": {
      "command": "node",
      "args": [
        "/path/to/gsp-editor-mcp.cjs",
        "/path/to/project"
      ]
    }
  }
}
```

适配器脚本可从以下位置获取：
- **已安装的应用** — `resources/gsp-editor-mcp.cjs`。
- **源码仓库** — `scripts/gsp-editor-mcp.cjs`。
- **独立下载** — [下载协作脚本包](https://github.com/espressif/esp-gsp/releases)（占位链接）。

MCP 工具包括：

| 工具 | 用途 |
|---|---|
| `get_editor_state` | 工程状态、工具链、当前文档 |
| `get_diagnostics` | 当前场景的编译器诊断 |
| `screenshot_preview` | 捕获渲染预览 PNG |
| `wait_for_preview` | 等待预览渲染完成 |
| `select` | 按索引选择控件（需要当前文档版本） |
| `reveal` | 滚动画布以显示某个控件 |
| `begin_batch` | 开始协作会话 |
| `ready_batch` | 声明写入完成以供审查 |
| `resume_batch` | 在 `ready` 后恢复写入 |
| `finish_batch` | 验证并接受批次 |
| `cancel_batch` | 取消会话 |
| `restore_batch` | 从已完成的批次恢复文件 |

适配器与 CLI 共享经过认证的本地编辑器服务。私有 HTTP 桥接不是公共 MCP HTTP
端点。

## 协作 Skill

可分发的协作 Skill 包含在 `skills/gsp-editor-collaboration/SKILL.md` 中，
也随打包应用一同发布。将其安装到你的 Agent Skill 目录即可使用编辑器交接和
证据检查工作流：

```sh
mkdir -p ~/.codex/skills
cp -R /path/to/gsp-editor/skills/gsp-editor-collaboration ~/.codex/skills/
```

## 编辑器横幅

协作会话活跃期间，编辑器显示横幅指示批次状态。横幅提供以下按钮：

- **完成** — 验证并接受。
- **取消** — 中止并可选恢复。
- **恢复** — 检查历史并恢复过去的批次。

## 最佳实践

1. 始终先调用 `begin` 并等待成功后再写入文件。
2. 编辑时保留未知的 JSON 字段和已有的资源路径。
3. 在 `finish` 前使用 `ready`，以便编辑器捕获审查快照。
4. 验证失败时，检查诊断信息、修复文件，然后使用新的 `requestId` 调用
   `finish` — 不要反复重试未变更的失败。
5. 对于大规模变更，考虑使用独立的 worktree 及其自身的构建上下文。
6. 不要将工程根目录外的文件作为批次的一部分进行编辑。
