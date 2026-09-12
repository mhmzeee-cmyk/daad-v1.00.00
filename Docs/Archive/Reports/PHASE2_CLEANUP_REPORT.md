# PHASE2_CLEANUP_REPORT.md

## Before
- Files: 862 hashed (incl. node_modules) · Dirs: 318 · Size: ~79M (22M = in-source `Daad-Compiler/build/`).

## Deleted (1 path, ~340 files, ~22M)
- `Daad-Compiler/build/` — GENERATED in-source CMake tree (CMakeCache/CMakeFiles/_deps/googletest+16M .git). Evidence: file list is build outputs only; regenerable via `cmake -S Daad-Compiler -B <out>`. Targeted `rm -rf` on this path only.

## Kept duplicates (intentional, §3B/C/D)
- Full/frontend ≡ Web (68 groups): bundled vs standalone deploy targets.
- server dhad utils ≡ frontend dhad (independent Node implementation).
- windows-x64 stdlib (106 groups): release bundle.
- cpu desktop mirrors (14 groups): runnable distribution folder.
- Empty-file trio + unknown DLL/exe/zip artifacts: UNKNOWN → kept.

## Modified (2 files)
- `Dhad-Studio-Electron/main.js`: 4 path strings `..` → `../Dhad-Studio-Full/...` (server spawn+cwd, login, nav). Reason: old paths matched NO layout in repo (pre-existing breakage); new paths verified to exist.
- `دليل-التثبيت-والتشغيل.txt`: removed 2 stale ZIP lines from tree.

## Build
- Compiler: cmake exit 0, build exit 0 (`libDaadCore.a`, `bin/daad-compiler`).
- Bridge: cmake exit 0, build exit 0 (`DhadBridge`, incl. compat code).
- CPU: no rebuild (sources untouched); smoke via /tmp copy (vfat has no exec bit): 10+20=30 ✓.
- Server: `node --check` index.js/cluster.js/router OK (+4 modified files OK in Phase 1).
- VSCode: `npm run compile` NOT VERIFIED (no node_modules/tsc offline); change was JSON-only, validated by parse.

## Regression
- Compiler e2e (fresh binary): `صحيح س = 5؛ طباعة(س)؛` → run prints **5** ✓.
- Sandbox: 9/9 ✓. Evaluator vectors: 10/10 (Phase 1, unchanged since).
- Frontend: no code touched this phase.

## Remaining unknowns
- `__------….dll` (5.9M), `Daad-Compiler/daad-compiler.exe` (distinct pre-existing binary), `icon.ico` (Electron asset), empty `LICENSE` placeholder, server live-boot/DB/Redis, VSCode tsc build.
