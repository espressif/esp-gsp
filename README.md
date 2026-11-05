# gspc-wasm (WebAssembly)

[GitHub](https://github.com/espressif/esp-gsp)

ESP-GSP scene compiler for browsers and Node.js. Compiles scene JSON, images,
fonts, SVG and component definitions into GSPB bundles and matching C
headers/reports. This package shares the same Rust compiler, resource encoders,
multi-scene linker and code generators as the native GSPC CLI.

> **Note:** This package does **not** build ESP-IDF firmware. Browser and Node
> memory projects have no host filesystem fallback or process execution.

## Installation

```sh
npm install gspc-wasm
```

Requires Node.js 18.20 or later. No native dependencies or Emscripten needed.

## Quick start (Node.js ESM)

```js
import { createCompiler } from "gspc-wasm";

const compiler = await createCompiler();
try {
  compiler.setText("main.json", JSON.stringify({
    screen: "main", w: 32, h: 32, objects: [],
  }));
  const result = compiler.compile({ scenes: ["main.json"] });
  if (result.success) {
    // result.bundle: Uint8Array — pass to the simulator or save as .gspb
    // result.files: Map<string, Uint8Array> — generated headers and sidecars
  } else {
    console.error(result.diagnostics);
  }
} finally {
  compiler.dispose();
}
```

## Browser usage

Browser bundles use the same `createCompiler` import. The package `exports`
map selects the browser entry automatically when using a bundler.

## Compile an uploaded project

```js
import { createCompiler } from "gspc-wasm";

const compiler = await createCompiler();
try {
  compiler.setText("scenes/main.json", sceneText);
  compiler.setFile("assets/ui.ttf", new Uint8Array(await fontFile.arrayBuffer()));
  compiler.setFile("assets/icon.png", new Uint8Array(await imageFile.arrayBuffer()));
  const result = compiler.compile({
    scenes: ["scenes/main.json"],
    pixelFormat: "rgb888",
    symbol: "my_app",
  });
  if (result.success) {
    // result.bundle: Uint8Array; pass to the simulator or save as my_app.gspb.
    // result.files: Map<string, Uint8Array>, including my_app.h and sidecars.
  } else {
    console.error(result.diagnostics);
  }
} finally {
  compiler.dispose();
}
```

Input paths are project-relative UTF-8 paths using `/`. An authored
`../assets/icon.png` resolves relative to `scenes/main.json`. Paths cannot
escape the project, name host files, or overwrite generated artifacts.

Upload the complete input set before compiling. `setFile` replaces one file
atomically; `removeFile` and `clear` support edits. Each compiler instance
owns its own files. Returned JS buffers survive later compiles and disposal.
Malformed API options throw; authored compilation errors return
`success: false`, diagnostics, an empty bundle and no stale generated files.

## Compile options

| Option | Description |
| --- | --- |
| `scenes` | Scene file paths in application order (required). |
| `componentsDir` | Uploaded directory of external component JSON files. |
| `pixelFormat` | One of `"rgb565"`, `"rgb888"` or `"argb8888"`. |
| `profilePath` | Uploaded YAML profile and its capability/calibration files. |
| `profile` | A complete `Profile.snapshot()` object (snake_case fields). |
| `imageCacheBytes` | Explicit compile/runtime cache override; `0` retains auto-sizing. |
| `symbol` | Output symbol prefix for generated C headers. |
| `deployable` | Whether the bundle targets device deployment (default `true`). |
| `generateFiles` | Generate C headers and reports (default `true`). Set `false` for preview-only. |
| `inspect` | Include source/geometry inspection in API sidecars. |

Choose exactly one of `pixelFormat`, `profilePath` or `profile`. The default
is the host RGB565 profile. See `index.d.ts` for the complete options and
response types, including `passes`, `slots` and `projectLimits`.

## Worker hosting

For interactive editors, run the compiler in a Worker to avoid blocking the
main thread. Import the `gspc-wasm/worker` export:

**Browser (Vite):**

```js
import CompilerWorker from "gspc-wasm/worker?worker";
const worker = new CompilerWorker();
```

**Node.js:**

```js
import { Worker } from "node:worker_threads";
const worker = new Worker(new URL(import.meta.resolve("gspc-wasm/worker")));
```

### Worker protocol

Send `{id, method, params}` messages; responses are `{id, ok, result}` or
`{id, ok: false, error}`.

Available methods: `init`, `setFile`, `setText`, `removeFile`, `clear`,
`compile`, `schema`, `capabilities`.

- File methods use `{path, bytes}` or `{path, text}`.
- `compile` uses compile options as params.
- The worker serializes edits and builds, and transfers bundle/generated-file
  buffers to the caller.

Send `init` before any other request to configure limits:

```js
worker.postMessage({ id: 1, method: "init", params: {
  limits: {
    maxFiles: 4096,
    maxInputBytes: 32 * 1024 * 1024,
    maxOutputBytes: 64 * 1024 * 1024,
  },
} });
```

`init` returns capabilities. Omitting it retains the default limits. Recreate
the Worker to change limits after initialization. Typed request/response
envelopes are exported from `index.d.ts`.

Compilation is synchronous inside a Worker. Terminate and recreate the Worker
to cancel a running build. After recreating, initialize it and upload the
complete project again. `clear` removes files but retains the configured
limits and WASM instance.

## Memory limits

`createCompiler({ limits })` accepts:

| Limit | Default | Description |
| --- | --- | --- |
| `maxFiles` | 4 096 | Maximum number of uploaded files. |
| `maxInputBytes` | 64 MiB | Cumulative uploaded input bytes. |
| `maxOutputBytes` | 128 MiB | Bundle plus generated files. |

These bound retained inputs and produced artifacts, not decoded image/font
working memory, caches or total browser/process RAM. Input decoding remains
governed by the compiler/profile limits. Larger projects should choose budgets
and measure their peak memory and compile latency.

Node may supply WASM bytes directly via `createCompiler({ wasm })` instead of
fetching the adjacent file.

## Capabilities

```js
compiler.capabilities.compilerVersion   // native GSPC product version
compiler.capabilities.config            // { schema_version, abi_version }
compiler.capabilities.apiVersion        // 1
```

## TypeScript

Full type definitions are included at `gspc-wasm/index.d.ts` (resolved via
the `types` field in `package.json`). See `CompileOptions`, `Compilation`,
`Compiler`, `WorkerRequest` and `WorkerResponse`.

## Exports

| Specifier | Description |
| --- | --- |
| `gspc-wasm` | Main entry — `createCompiler()` |
| `gspc-wasm/worker` | Worker entry for both browser and Node.js |
| `gspc-wasm/wasm` | Raw WASM asset for custom loading |
| `gspc-wasm/package.json` | Package metadata |

## License

See the `LICENSE` file included in the package. Third-party notices are in
`THIRD_PARTY_LICENSES.txt`.
