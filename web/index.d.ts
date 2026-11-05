export interface Diagnostic {
  code: string;
  message: string;
  severity: "error" | "warning" | "information" | "hint";
  file: string | null;
  range: { start: { line: number; character: number }; end: { line: number; character: number } } | null;
  [key: string]: unknown;
}

export interface MemoryLimits {
  maxFiles?: number;
  maxInputBytes?: number;
  maxOutputBytes?: number;
}

export interface CompileOptions {
  scenes: string[];
  componentsDir?: string;
  /** Complete validated Profile snapshot, using the compiler's snake_case fields. */
  profile?: Record<string, unknown>;
  profilePath?: string;
  pixelFormat?: "rgb565" | "rgb888" | "argb8888";
  /** Explicit compile/runtime cache override; omission preserves runtime auto-sizing. */
  imageCacheBytes?: number;
  symbol?: string;
  deployable?: boolean;
  generateFiles?: boolean;
  debug?: boolean;
  inspect?: boolean;
  passes?: { occlusion_cull: boolean; redundant_fill_cull: boolean; rect_merge: boolean; static_bake: boolean };
  slots?: {
    list_max_slots: number; list_text_slots: number; instance_states_per_slot: number;
    max_dynamic_image_targets: number; max_font_packs: number;
  };
  projectLimits?: {
    scene_count: number; list_count: number; list_max_slots: number; list_text_slots: number;
    text_slots: number; dynamic_image_slots: number; component_instances: number;
    stack_view_max_depth: number; glyph_run_slots: number; instance_states_per_slot: number;
    asset_anims: number;
  };
}

export interface Compilation {
  success: boolean;
  sceneCount: number;
  diagnostics: Diagnostic[];
  apiJson: Record<string, unknown>;
  bundle: Uint8Array;
  files: Map<string, Uint8Array>;
}

export interface Compiler {
  readonly capabilities: {
    apiVersion: 1;
    compilerVersion: string;
    config: { schema_version: number; abi_version: number };
    memoryProjects: true; multiScene: true; generatedFiles: true; videoImport: false;
  };
  schema(): Record<string, unknown>;
  setFile(path: string, bytes: Uint8Array): void;
  setText(path: string, text: string): void;
  removeFile(path: string): boolean;
  clear(): void;
  compile(options: CompileOptions): Compilation;
  dispose(): void;
}

/** Send init before any other valid request to select limits. It returns capabilities. */
export type WorkerRequest = { id: number | string } & (
  | { method: "init"; params?: { limits?: MemoryLimits } }
  | { method: "setFile"; params: { path: string; bytes: Uint8Array } }
  | { method: "setText"; params: { path: string; text: string } }
  | { method: "removeFile"; params: { path: string } }
  | { method: "compile"; params: CompileOptions }
  | { method: "clear" | "schema" | "capabilities" }
);

export type WorkerResponse =
  | { id: number | string; ok: true; result: Compilation | boolean | Record<string, unknown> | undefined }
  | { id: number | string; ok: false; error: string };

export function createCompiler(options?: { wasm?: BufferSource | WebAssembly.Module; limits?: MemoryLimits }): Promise<Compiler>;
