# PHASE6_RELEASE_MANIFEST.md — proposal only (no artifacts created)

```
Dhad Studio Release
├── Compiler (REQUIRED): src + include/Daad + Daad/Runtime wrapper + stdlib/** + CMakeLists + Makefile + build recipe (cmake flags) + install layout doc
├── Runtime (REQUIRED): DaadRuntime.hpp + DaadStdlib*.hpp + Math.hpp + String.hpp (part of Compiler set)
├── Server (REQUIRED): src + prisma(schema, NO migrations→adopt first) + package.json (+lock if present?) + .env.example(NEW, from env table) + EXCLUDE node_modules/seeds/dev.db/test files
├── Frontend (REQUIRED): Web tree (or Full/frontend copy — pick ONE source of truth at packaging) + host config + API-base doc + fonts decision
├── Electron (REQUIRED): main+preload+package + bundled Full subset + installed server deps + icon.ico (MISSING — create) + builder config (electron-builder scripts exist, untested)
├── Bridge (REQUIRED): sources + rebuilt binary + Qt runtime story
├── VSCode Extension (REQUIRED): package + compiled out/ + syntaxes/snippets/config + PATH doc for daad-compiler
├── CPU (REQUIRED): src+include+examples+tests+scripts; binaries: rebuild (OPTIONAL ship, EXCLUDE stale)
├── Documentation (REQUIRED): README (fix 3 notes) + guide + env docs + install steps per OS
├── Configuration templates (REQUIRED): .env.example, ecosystem/pm2 or service file (currently MISSING)
└── Legal files (REQUIRED): LICENSE text (BLOCKED: empty), third-party attributions
EXCLUDE: node_modules, build/, *.db, seeds, _backup_original?, .o/.obj intermediates, unknown DLL (until reviewed)
UNKNOWN: which frontend copy is canonical; pm2 vs systemd; installer tech choice (later phase)
```
