/* tslint:disable */
/* eslint-disable */

export class CompiledProject {
    private constructor();
    free(): void;
    [Symbol.dispose](): void;
    filePath(index: number): string;
    /**
     * Transfer the owned bundle to JavaScript. Subsequent calls return empty.
     */
    takeBundle(): Uint8Array;
    /**
     * Transfer one artifact to JavaScript, without retaining another copy.
     */
    takeFile(index: number): Uint8Array;
    readonly fileCount: number;
    readonly summary: string;
}

export class Compiler {
    free(): void;
    [Symbol.dispose](): void;
    clear(): void;
    /**
     * Syntax/semantic errors belong to Compilation.diagnostics. Malformed
     * compiler options throw before invoking the compiler.
     */
    compile(options_json: string): CompiledProject;
    constructor();
    removeFile(path: string): boolean;
    setFile(path: string, bytes: Uint8Array): void;
    validateFile(path: string, byte_len: number): void;
    static withLimits(limits_json: string): Compiler;
}

export function capabilitiesJson(): string;

export function schemaJson(): string;

export type InitInput = RequestInfo | URL | Response | BufferSource | WebAssembly.Module;

export interface InitOutput {
    readonly memory: WebAssembly.Memory;
    readonly __wbg_compiledproject_free: (a: number, b: number) => void;
    readonly __wbg_compiler_free: (a: number, b: number) => void;
    readonly capabilitiesJson: (a: number) => void;
    readonly compiledproject_fileCount: (a: number) => number;
    readonly compiledproject_filePath: (a: number, b: number, c: number) => void;
    readonly compiledproject_summary: (a: number, b: number) => void;
    readonly compiledproject_takeBundle: (a: number, b: number) => void;
    readonly compiledproject_takeFile: (a: number, b: number, c: number) => void;
    readonly compiler_clear: (a: number) => void;
    readonly compiler_compile: (a: number, b: number, c: number, d: number) => void;
    readonly compiler_new: () => number;
    readonly compiler_removeFile: (a: number, b: number, c: number, d: number) => void;
    readonly compiler_setFile: (a: number, b: number, c: number, d: number, e: number, f: number) => void;
    readonly compiler_validateFile: (a: number, b: number, c: number, d: number, e: number) => void;
    readonly compiler_withLimits: (a: number, b: number, c: number) => void;
    readonly schemaJson: (a: number) => void;
    readonly __wbindgen_add_to_stack_pointer: (a: number) => number;
    readonly __wbindgen_export: (a: number, b: number, c: number) => void;
    readonly __wbindgen_export2: (a: number, b: number) => number;
    readonly __wbindgen_export3: (a: number, b: number, c: number, d: number) => number;
}

export type SyncInitInput = BufferSource | WebAssembly.Module;

/**
 * Instantiates the given `module`, which can either be bytes or
 * a precompiled `WebAssembly.Module`.
 *
 * @param {{ module: SyncInitInput }} module - Passing `SyncInitInput` directly is deprecated.
 *
 * @returns {InitOutput}
 */
export function initSync(module: { module: SyncInitInput } | SyncInitInput): InitOutput;

/**
 * If `module_or_path` is {RequestInfo} or {URL}, makes a request and
 * for everything else, calls `WebAssembly.instantiate` directly.
 *
 * @param {{ module_or_path: InitInput | Promise<InitInput> }} module_or_path - Passing `InitInput` directly is deprecated.
 *
 * @returns {Promise<InitOutput>}
 */
export default function __wbg_init (module_or_path?: { module_or_path: InitInput | Promise<InitInput> } | InitInput | Promise<InitInput>): Promise<InitOutput>;
