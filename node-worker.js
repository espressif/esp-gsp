import { parentPort } from "node:worker_threads";
import { createCompiler } from "./node.js";
import { serveCompiler } from "./web/worker-core.js";

if (!parentPort) throw new Error("The GSPC Worker entry must run in a worker thread");

serveCompiler({
  createCompiler,
  subscribe: handler => parentPort.on("message", handler),
  postMessage: (message, transfer) => parentPort.postMessage(message, transfer),
});
