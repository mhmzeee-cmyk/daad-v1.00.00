# PHASE2_DUPLICATE_ANALYSIS.md — sha256 re-scan (189 groups, 384 files)

## A — Unnecessary duplicate: NONE deleted this phase
(No A-class duplicates remain; phase-1 already removed `Full/compiler` + `desktop/`.)

## B — Intentional duplicates: KEEP BOTH
- 68 groups `Dhad-Studio-Full ↔ Dhad-Studio-Web`: Full/frontend-web is the bundled copy served by the Full server; Web is the standalone Netlify/Vercel target. Both deployed → KEEP.
- 14 groups within `cpu` (examples/tests mirrored under `cpu/desktop/`): desktop is a ready-to-run distribution folder → KEEP.

## C — Independent implementations: KEEP
- `server/src/utils/dhad/{lexer,parser,ast,codegen}.js` ≡ frontend `dhad-*.js`: server-side evaluator needs its own copy (runs in Node, no DOM). Do not merge.

## D — Release artifacts: KEEP
- 106 groups `Daad-Compiler/stdlib ↔ daad-studio-v1.0.0-windows-x64/stdlib`: stdlib bundled into the Windows release → KEEP.
- `cpu/*.o`, `*.exe`, `desktop_dhad_*.zip`, `Daad-Compiler/daad-compiler.exe` (hash differs from release exe → distinct pre-existing binary, unreferenced → UNKNOWN → KEEP).

## Trivial
- 1 group of three 0-byte files (`ملف_جديد.ض`, `LICENSE`, `demo.ضasm`): empty placeholders → KEEP (no impact).

## Deleted (1 item, class GENERATED)
- `Daad-Compiler/build/` (~340 files, 22M: CMakeCache/CMakeFiles/_deps/googletest incl. 16M .git): in-source build tree created during repair verification; regenerable via documented out-of-source build. Verified no project sources inside (only CMake's own CompilerId test). `rm -rf` targeted at this path only → 27M→5.3M.
