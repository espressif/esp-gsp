import initialize, { Compiler, capabilitiesJson, schemaJson } from "../pkg/gspc_wasm.js";

let initialization;

/** Initialize once per JS realm. Each returned compiler owns an isolated project.
 * Supply wasm bytes/Module explicitly in Node; browsers use the adjacent file. */
export async function createCompiler({ wasm, limits } = {}) {
  if (limits !== undefined && (limits === null || typeof limits !== "object" || Array.isArray(limits))) {
    throw new TypeError("compiler limits must be an object");
  }
  initialization ??= initialize(wasm === undefined ? undefined : { module_or_path: wasm })
    .catch(error => { initialization = undefined; throw error; });
  await initialization;
  const compiler = limits ? Compiler.withLimits(JSON.stringify(limits)) : new Compiler();
  const maxInputBytes = limits?.maxInputBytes ?? 64 * 1024 * 1024;
  let disposed = false;
  const alive = () => { if (disposed) throw new Error("GSPC compiler has been disposed"); };
  const pathArgument = path => {
    if (typeof path !== "string" || path.length > 4096) throw new TypeError("file path must be a string of at most 4096 bytes");
  };
  const store = (path, bytes) => {
    pathArgument(path);
    if (bytes.byteLength > maxInputBytes) throw new RangeError("file exceeds the input-byte budget");
    compiler.validateFile(path, bytes.byteLength);
    compiler.setFile(path, bytes);
  };
  return {
    capabilities: JSON.parse(capabilitiesJson()),
    schema: () => { alive(); return JSON.parse(schemaJson()); },
    setFile(path, bytes) {
      alive();
      if (!(bytes instanceof Uint8Array)) throw new TypeError("file content must be Uint8Array");
      store(path, bytes);
    },
    setText(path, text) {
      alive();
      if (typeof text !== "string") throw new TypeError("file content must be a string");
      if (text.length > maxInputBytes) throw new RangeError("file exceeds the input-byte budget");
      store(path, new TextEncoder().encode(text));
    },
    removeFile(path) { alive(); pathArgument(path); return compiler.removeFile(path); },
    clear() { alive(); compiler.clear(); },
    compile(options) {
      alive();
      const result = compiler.compile(JSON.stringify(options));
      try {
        const summary = JSON.parse(result.summary);
        const files = new Map();
        for (let i = 0; i < result.fileCount; i++) files.set(result.filePath(i), result.takeFile(i));
        return { ...summary, bundle: result.takeBundle(), files };
      } finally { result.free(); }
    },
    dispose() { if (!disposed) { compiler.free(); disposed = true; } },
  };
}
