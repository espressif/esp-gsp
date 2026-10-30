# Compiler Stack Evidence

Release builds can export compiler stack frames and call graphs alongside the
prebuilt archive. Source maintainers can request the same data on demand:

```sh
GSP_STACK_EVIDENCE=1 bash ci/build_prebuilt_library.sh esp32s31
```

An active matching ESP-IDF/toolchain environment is required. Ordinary builds
keep compiler caching; evidence builds disable cache hits so `.su` and `.ci`
sidecars are present. Tagged release builds collect them automatically.

Outputs under `artifacts/stack-evidence/<target>/` include `status.json`,
`compiler-stack.json.gz` and `synchronous-entry-points.json.gz`. The status
records whether collection ran. Evidence binds the archive SHA-256, compiler,
SDK, configuration, source and object identities. A build against another SDK
is a reference build until its machine code has been verified equivalent.

The analyzer reports per-call-site known stack contributions and unresolved
terms. Sequential calls take a maximum; nested calls add their live prefixes.
Unresolved assembly/ROM frames, indirect targets, weak imports, dynamic frames
and recursive components without a proven bound remain explicit. They are not counted as
zero. Scoped assumptions must identify their evidence and matching input hash.
Each root also includes `call_sites`: one row per compiler call site with its
target set, target-set completeness, callee maximum, live-caller-plus-callee
maximum and recursive-component IDs. Open downstream terms keep numeric maxima
null even when the immediate target set is complete. Source locations are
compiler locations; final instruction addresses require the linked ELF.

`open_frontiers` traces one compiler-graph path attaining each unresolved term's
maximum known prefix, including its caller, location, candidate targets and
recursive-component ID. The unknown subtree is excluded from that prefix;
a compiler cycle alone does not prove unbounded recursion. Export as CSV with:

```sh
python3 ci/stack_usage.py analyze --input compiler-stack.json.gz \
  --root esp_gsp_stop --output analysis.json.gz --frontiers-csv frontiers.csv
```

Use distinct paths for the input, assumptions and JSON/CSV outputs.

In `transitive_bounds` assumptions, `bytes: null` keeps a whole function subtree
symbolic instead of expanding it. This supports modular proofs such as a caller
prefix plus a separately proved allocator bound. It never means zero or makes
the result numerically complete; a numeric bound still requires evidence.

Use the reports together with the final application ELF/map, task entry,
application callback targets and the applicable port/interrupt reserve.
FreeType, presenter, IDF, ROM and libc ownership must be established from the
actual linked artifacts; an undefined import in GSP is not evidence that its
implementation is inside the GSP archive.

A largest observed or resolved path is not a task-wide worst-case bound.
`task_stack_bound_bytes` remains null while application and external frontiers
are open. The application's task budget and GSP's render-worker stack policy
are separate contracts. Runtime stack watermarks complement static evidence;
they do not close unexecuted paths.
