import { createCompiler } from "./index.js";
import { serveCompiler } from "./worker-core.js";

serveCompiler({
  createCompiler,
  subscribe: handler => self.addEventListener("message", ({ data }) => handler(data)),
  postMessage: (message, transfer) => self.postMessage(message, transfer),
});
