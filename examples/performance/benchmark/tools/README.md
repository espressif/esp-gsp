# Offline hardware benchmark data tools

These Python 3.10+ tools use only the standard library. They validate saved
evidence, save validated results, and regenerate the bilingual hardware
benchmark pages. They do not build, flash, monitor, reset or restore a device.
Published versions follow the matched measurement identity. Running these tools
does not establish results for another version.

## Inputs and evidence boundary

`aggregate_matrix.py` consumes the evidence schema used by the original private
runner. That runner and its device plans are not included. A new collection must
provide the following real evidence; the tools do not manufacture it from logs.
Paths inside input JSON are absolute or relative to the command's working
directory. To move an evidence archive, update its path fields to the retained
files without changing their contents or recorded digests.

| Input | Required structure |
| --- | --- |
| Private plan JSON | `boards` array. Each included board has `label`, `chip`, private `mac`, `cpu_mhz`, `builds.native` / `builds.full` build-directory paths, `display` (`controller`, `interface`, `pixel_format`, panel `resolution_px`), and an `expected` fingerprint for logical geometry, rotation and CPU cores. `included: false` explicitly records a profile outside the run. Labels must be public-safe. |
| Runner evidence roots | Each root contains `<label>/status.json`. Exactly one status per selected label may occur across all `--runs-dir` inputs; duplicate candidates are rejected. |
| Status JSON | `board.label`, `board.chip`, and private `board.mac` must match the plan. `runs` contains per-boot `index`, `mode` (`native` / `full`), `log`, `log_sha256`, `app_sha256`, and `elf_sha256`. Two complete independent boots per mode are required. |
| Build evidence | Each build directory contains `sdkconfig` and `deployment-ready.json`: `chip`, `mode`, `elf` path, `elf_sha256`, `app_sha256`, `sdkconfig_sha256`, plus `images` with `file`, `bytes`, and `sha256` for every deployed image. Digests and file sizes are checked against the actual retained files. |
| Optional size evidence | `size-summary.json` or `size.json` in each build directory. Supported forms are the original normalized `schema: 1` (`app_binary_bytes`, `static_internal_ram_bytes`, `linked_external_ram_bytes`, `other_memory_layouts`), an ELF-bound `gsp-benchmark-size-v1` summary, or ESP-IDF size JSON / JSON2 `layout` entries (`name`, `used`). Missing or unknown regions remain unavailable. |

A completed restored transaction requires `state: "complete"` and
`restored: true`. This is an attestation from the collection workflow; an offline
tool cannot independently read back the device to prove restoration.
An explicitly authorized retained-firmware transaction instead requires
`deployment_disposition: "retained_by_user"`, `restore_required: false`,
`retention_authorized: true`, `retained_firmware_verified: true`, and four runs in
exact index/mode order `0/native`, `1/full`, `2/full`, `3/native`.
`retained_app_sha256`, `retained_elf_sha256`, and `retained_boot_log_sha256` must
match the final native run; its app and ELF must also match the current native
build. All four logs must pass validation before either mode is pooled.
Retained firmware never implies `restored: true`.

Manual IDF monitor captures can be inspected with the existing `compare_logs.py`
CLI (see `python3 compare_logs.py --help`). For matrix acceptance, each saved log
must cover one boot, its ELF fingerprint, the configuration and case markers,
58 warm-up cases, all 58 measured cases with raw counters, the final summary and
`measurement end`. The current matrix contract is protocol 30 and the adjacent
`../main/bench_cases.inc` manifest. A monitor capture alone does **not** prove
which complete image set was flashed, private board identity, restoration, or
authorization to retain firmware. Supply trustworthy build and transaction
records separately; do not fill those fields merely to make a log pass.

## Validate and export saved evidence

Run from this directory, using paths to your own retained evidence:

```sh
python3 -B -m unittest test_aggregate_matrix.py
python3 -B aggregate_matrix.py \
  --plan "$EVIDENCE/plan.json" \
  --runs-dir "$EVIDENCE/runs" \
  --output-dir "$OUTPUT/public"
```

`--runs-dir` may be repeated for separate batches, and `--label` may be repeated
to select a publication subset. `--parser-dir` defaults to this directory's
`compare_logs.py`, which also uses the adjacent manifest and `vector_cases.py`.
Both tools require an explicit output directory outside the tools source tree.

The evidence exporter writes `matrix.json`, `groups.csv`, `runs.csv`, `cases.csv`,
`transitions.csv`, `startup-memory.csv`, and `failures.csv`. It exits zero only
when every included group passes. Missing or failed repeats retain explicit
reasons and are never filled with zero; valid individual repeats can remain
visible while a group has no pooled result. Device identities, private paths,
ports, dates, and arbitrary exception text are not copied into public output.

FPS and repeat ranges use raw frame/time ratios. CPU pools raw all-core capacity,
idle and explicit per-window idle adjustments, rather than averaging percentages.
Firmware-rounded FPS / CPU remain separate fields. Active FPS uses render-path
busy time and is distinct from wall FPS and CPU utilization. Transition P95 is a
transition-completion statistic, not a per-frame latency percentile. Startup
heap decreases and sampled case increases are distinct from ELF allocations and
total or exact peak usage; linked external RAM may include XIP code / rodata.
The saved JSON includes the full definitions. These detailed acquisition files
are inputs to publication, rather than files to copy into the documentation site.

## Regenerate the measured bilingual pages

The renderer requires the complete declared publication matrix and checks raw
FPS / CPU arithmetic and repeat pooling again. Its default is the published
12-profile set, including S31 QSPI; `--include-mosaico` is a compatible explicit
alias. `--exclude-mosaico` selects the 11-profile set without S31 QSPI. It does not
accept arbitrary partial tables as publication pages.

Measured versions come from `measurement.json` beside the matrix. Use
`--measurement PATH` only when that attachment is stored elsewhere. Its complete
board/mode build set and app, ELF and configuration SHA-256 hashes must match the
matrix before either language is written. GSP/GSPC versions, the IDF revision and
versioned download paths follow this matched identity; there is no version-label
override. This cross-check ties the attachments together, but does not recreate
or independently attest their original firmware acquisition.

To generate pages from your saved validated evidence, run from the repository root:

```sh
python3 -B examples/performance/benchmark/tools/render_hardware_docs.py \
  --matrix "$OUTPUT/public/matrix.json" \
  --exclude-mosaico \
  --output-dir "$OUTPUT/hardware-docs"
diff -u docs/src/content/docs/guide/hardware-benchmark.md \
  "$OUTPUT/hardware-docs/en/guide/hardware-benchmark.md"
diff -u docs/src/content/docs/zh-cn/guide/hardware-benchmark.md \
  "$OUTPUT/hardware-docs/zh-cn/guide/hardware-benchmark.md"
```

The renderer writes `en/guide/hardware-benchmark.md` and
`zh-cn/guide/hardware-benchmark.md` beneath the requested output directory. It
also writes compact `cases.csv` and `resources.csv` under
`assets/performance/gsp-VERSION-benchmark/`. Per-case FPS and CPU use pooled
counters; min/max columns retain the two boot observations. Sampled heap
increases retain the larger observation and are not exact allocation peaks.
Resource columns use bytes and distinguish ELF allocation, startup increase
and free memory after initialization. Copy only these two CSV files into the
site's corresponding `docs/public/assets/performance/` directory.

The renderer prepares and validates both languages and downloads before writing.
`--outline` instead of
`--matrix` writes visibly unpublished `hardware-benchmark.outline.md` files.
Outlines need no measurement attachment and contain no measured version or
download links. Generated publication links retain the documentation-relative
asset layout, with the GSP version taken from the measurement attachment.

Keep `measurement.json`, the full matrix and acquisition records with the saved
evidence. These tools do not independently attest source/compiler freeze or
device transactions. Public downloads contain current results and resource
costs; they do not include historical datasets, build inventories or frame traces.
