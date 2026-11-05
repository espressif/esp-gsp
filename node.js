import { readFile } from "node:fs/promises";
import { createCompiler as createWebCompiler } from "./web/index.js";

let wasmModule;

/** Load only the packaged compiler. Project files still use explicit uploads. */
export async function createCompiler(options = {}) {
  let wasm = options.wasm;
  if (wasm === undefined) {
    wasmModule ??= readFile(new URL("./pkg/gspc_wasm_bg.wasm", import.meta.url))
      .then(bytes => WebAssembly.compile(bytes))
      .catch(error => { wasmModule = undefined; throw error; });
    wasm = await wasmModule;
  }
  return createWebCompiler({ ...options, wasm });
}
