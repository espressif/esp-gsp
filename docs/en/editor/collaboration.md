# Agent Collaboration

ESP-GSP Studio supports a file-based collaboration protocol that lets an AI
agent (or external script) edit native GSP scene files while the editor
provides validation, preview and recovery.

The editor coordinates a project-wide **handoff**: it flushes local drafts,
pauses human content editing, lets the external writer modify files, then
validates and reloads the complete batch when the writer finishes.

## Prerequisites

- ESP-GSP Studio must be running with the target project open.
- Node.js 22 or later.
- The collaboration CLI script `gsp-agent.cjs`. It is available from:
  - **Installed application** — shipped in the app's `resources/` directory.
  - **Source checkout** — `scripts/gsp-agent.cjs` in the editor repository.
  - **Standalone download** — [Download collaboration scripts](index.md#download).

## Quick start

```sh
# 1. Check editor status
node /path/to/gsp-agent.cjs status /path/to/project

# 2. Begin a collaboration session
node /path/to/gsp-agent.cjs begin /path/to/project
# The command prints a batch ID on success.

# 3. Edit native JSON and resource files directly.
#    (The editor pauses human content editing during the session.)

# 4. When all writes are done, mark the batch as ready for review
node /path/to/gsp-agent.cjs ready /path/to/project <batch-id>

# 5. Finish: validate and reload
node /path/to/gsp-agent.cjs finish /path/to/project <batch-id>
```

## CLI commands

### `status`

```sh
node gsp-agent.cjs status /path/to/project
```

Reports the current project state, selected Bundle/toolchain and whether an
active collaboration session exists.

### `begin`

```sh
node gsp-agent.cjs begin /path/to/project
```

Starts a new collaboration session. The editor flushes any pending human
drafts and enters read-only mode for content. Returns a **batch ID** that
must be used in subsequent commands.

- Retrying `begin` with the same `requestId` returns the original result
  without repeating the snapshot.
- An existing active or interrupted batch must be resolved before starting a
  new one.

### `ready`

```sh
node gsp-agent.cjs ready /path/to/project <batch-id>
```

Declares that the external writer has stopped modifying files and captures
the current file state for review. After `ready`, the editor can display
before/after diffs and previews.

Use `resume` to continue writing if more changes are needed.

### `resume`

```sh
node gsp-agent.cjs resume /path/to/project <batch-id>
```

Resumes an unfinished batch after `ready`, allowing additional file writes
before calling `ready` or `finish` again.

### `finish`

```sh
node gsp-agent.cjs finish /path/to/project <batch-id>
```

Validates the complete project using the selected GSPC and Bundle context,
then reloads the editor.

- `success: true` means the compiler accepted the batch.
- Check `reload.status` separately — a pending or failed reload does not
  undo acceptance. Use `retry_reload` if needed.
- A validation failure keeps the batch and its diagnostics. Fix the source
  files and call `finish` again with a **new** `requestId`.

### `cancel`

```sh
# Keep modified files on disk:
node gsp-agent.cjs cancel /path/to/project <batch-id>

# Restore pre-batch files:
node gsp-agent.cjs cancel /path/to/project <batch-id> --restore
```

Without `--restore`, modified files remain on disk. With `--restore`, the
editor restores the snapshot taken at `begin`.

### `restore`

```sh
node gsp-agent.cjs restore /path/to/project <completed-batch-id>
```

Restores files from a previously completed batch. Refuses if any affected
files have been modified since the batch.

### `batch_status`

Query the state of a batch by ID after a timeout or interruption. Terminal
results remain readable even after `session.json` is removed.

## Batch scope and limits

- The batch covers regular source files inside the project root.
- **Excluded**: generated/build/cache directories, symbolic links, external
  sources and resources.
- **Snapshot limit**: 128 MiB.
- Shared assets outside the project root can be referenced but must not be
  edited as part of this project-local batch.

## Recovery

If the editor or the agent process is interrupted:

1. Reopening the project in Studio shows the interrupted session in
   **read-only** mode.
2. Use the editor's **Collaboration History and Recovery**
   (协作记录与恢复…) entry to inspect, finish or restore the batch.
3. The CLI `restore` command can also recover a completed batch, as long as
   no files have been modified after the batch.

Recovery checks all changed source versions before restoring anything.
Progress is journaled so a resumed restore can continue from where it left
off.

## MCP connection

For AI agents that support MCP (Model Context Protocol), Studio also provides
an MCP stdio adapter:

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

The adapter script is available from:
- **Installed application** — `resources/gsp-editor-mcp.cjs`.
- **Source checkout** — `scripts/gsp-editor-mcp.cjs`.
- **Standalone download** — [Download collaboration scripts](https://github.com/espressif/esp-gsp/releases) (placeholder).

MCP tools include:

| Tool | Purpose |
|---|---|
| `get_editor_state` | Project state, toolchain, active document |
| `get_diagnostics` | Compiler diagnostics for the current scene |
| `screenshot_preview` | Capture a rendered preview PNG |
| `wait_for_preview` | Wait until the preview finishes rendering |
| `select` | Select a widget by index (requires current document version) |
| `reveal` | Scroll the canvas to reveal a widget |
| `begin_batch` | Start a collaboration session |
| `ready_batch` | Declare writes complete for review |
| `resume_batch` | Resume writing after `ready` |
| `finish_batch` | Validate and accept the batch |
| `cancel_batch` | Cancel the session |
| `restore_batch` | Restore files from a completed batch |

The adapter shares the authenticated local editor service with the CLI.
The private HTTP bridge is not a public MCP HTTP endpoint.

## Collaboration skill

A distributable collaboration skill is included at
`skills/gsp-editor-collaboration/SKILL.md` and is shipped in packaged
applications. Install it in your agent's skill directory to provide the
editor handoff and evidence workflow:

```sh
mkdir -p ~/.codex/skills
cp -R /path/to/gsp-editor/skills/gsp-editor-collaboration ~/.codex/skills/
```

## Editor banner

While a collaboration session is active, the editor displays a banner
indicating the batch state. The banner provides buttons for:

- **Finish** — validate and accept.
- **Cancel** — abort and optionally restore.
- **Recovery** — inspect history and restore past batches.

## Best practices

1. Always call `begin` and wait for success before writing files.
2. Preserve unknown JSON fields and existing resource paths when editing.
3. Use `ready` before `finish` so the editor can capture review snapshots.
4. On validation failure, inspect diagnostics, fix files, then `finish` with
   a new `requestId` — do not retry unchanged failures repeatedly.
5. For large changes, consider a separate worktree with its own build context.
6. Do not edit files outside the project root as part of a batch.
