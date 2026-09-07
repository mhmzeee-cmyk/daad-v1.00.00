# PHASE6_REPRODUCIBILITY_AUDIT.md — can we rebuild every binary?

| Binary | Source | Command (verified) | Deps | Result |
|---|---|---|---|---|
| daad-compiler (Linux) | Daad-Compiler/src+include | `cmake -S … -B out -DDAAD_BUILD_TESTS=OFF … && cmake --build out` | g++,cmake | PASS ×4 (incl. relocated + clean-env) |
| DhadBridge | bridge/* | `cmake -S bridge -B out && cmake --build out` | Qt5.15,cmake | PASS ×3 |
| dhad_cpu/live/vis | cpu/src | `gcc -o … src/*.c` (suite harness path) | gcc | PASS (behavior 96/96) |
| dhad_gui | cpu/gui+src | qmake+make | Qt5 dev | STATIC (not rebuilt here; sources+deps known) |
| windows *.exe, root DLL | MinGW fam. (markers) | — | Windows toolchain | NOT VERIFIED → RISK; never ship without rebuild provenance |
| VSCode out/ | src/*.ts | `tsc -p ./` | npm+typescript | PASS (in /tmp) |
| tree DhadBridge ELF | — | — | — | STALE (predates main.cpp) → refresh, don't ship |
