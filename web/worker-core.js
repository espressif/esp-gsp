// One compiler and serial request queue per Worker. The transport supplies
// message delivery; compiler ownership and the wire protocol stay shared.
export function serveCompiler({ createCompiler, subscribe, postMessage }) {
  let compiler;
  let queue = Promise.resolve();
  const handlers = {
    setFile: (instance, params) => instance.setFile(params.path, params.bytes),
    setText: (instance, params) => instance.setText(params.path, params.text),
    removeFile: (instance, params) => instance.removeFile(params.path),
    clear: instance => instance.clear(),
    compile: (instance, params) => instance.compile(params),
    schema: instance => instance.schema(),
    capabilities: instance => instance.capabilities,
  };
  const initialize = options => {
    compiler ??= createCompiler(options).catch(error => { compiler = undefined; throw error; });
    return compiler;
  };
  subscribe(data => {
    queue = queue.then(async () => {
      const id = data?.id;
      try {
        if (!data || typeof data.method !== "string") throw new TypeError("request must contain a method");
        const { method, params = {} } = data;
        let result;
        if (method === "init") {
          if (!params || typeof params !== "object" || Array.isArray(params) || Object.keys(params).some(key => key !== "limits")) {
            throw new TypeError("init params must be an object containing only limits");
          }
          if (compiler) throw new Error("Worker is already initialized; recreate it to change limits");
          result = (await initialize({ limits: params.limits })).capabilities;
        } else {
          if (!Object.hasOwn(handlers, method)) throw new Error(`Unknown compiler method: ${method}`);
          result = handlers[method](await initialize(), params);
        }
        const transfer = result?.bundle ? [result.bundle.buffer, ...[...result.files.values()].map(bytes => bytes.buffer)] : [];
        postMessage({ id, ok: true, result }, transfer);
      } catch (error) {
        postMessage({ id, ok: false, error: String(error.message || error) });
      }
    });
  });
}
