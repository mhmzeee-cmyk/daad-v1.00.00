# PHASE2_INVENTORY.md — Engineering Cleanup inventory (2026-09-06)

**Root:** USB mount `UBUNTU 26_0/<NBSP>/` (vfat — no exec bits, no git repo; no `git init` per rules).
**Backup phase2:** `/tmp/Dhad-Studio-backup-phase2/` (71M). Old backup kept.

## Sizes (before phase2 actions)
- Total: ~79M (incl. node_modules + in-source build tree + root user files).
- Daad-Compiler 27M (incl. `build/` 22M) · Full 28M · Web 2.7M · Electron 7.1M · VSCode 80K · cpu 5.7M · windows-x64 2.2M.

## Suspicious extensions found
- `Daad-Compiler/build/` (CMakeCache/CMakeFiles/_deps/googletest+16M .git) → GENERATED, in-source → DELETED.
- `cpu/*.o`, `cpu/desktop/*.o`, `cpu/desktop/dhad_cpu.exe`, `Daad-Compiler/daad-compiler.exe`, `windows-x64/bin/daad-compiler.exe` → pre-existing build/dist artifacts → KEPT (see duplicate analysis).
- `cpu/desktop_dhad_v2.0.zip`, `desktop_dhad_v3.0.zip` → pre-existing cpu distribution archives → KEPT.
- No `.tar/.7z/.log/.tmp/.cache`, no `dist/`, no `CMakeCache.txt` outside `build/`.
- `node_modules` only in `Dhad-Studio-Electron/` (incl. nested) → runtime deps → KEPT, size recorded (7.1M total dir).
- Root `/1 /2 /3.jpeg…/14.png` (Sep 5, user photos) → OUT OF SCOPE, untouched.
- Root `__------….dll` (5.9M, unknown Windows DLL) → UNKNOWN → KEPT.

## Counts
- Files (excl. trash/sys, incl. node_modules): 862 hashed. Dirs (excl. trash/sys): 318.
