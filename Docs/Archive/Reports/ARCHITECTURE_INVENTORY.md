# ARCHITECTURE_INVENTORY.md — per-folder roles (Phase 2)

| Dir | Purpose | Entry | Build | Runtime deps | Generated | Tests | Docs | Internal deps | Distribution role |
|---|---|---|---|---|---|---|---|---|---|
| Daad-Compiler | ض→C++20 compiler | src/main.cpp | CMake/C++20, Makefile `-Iinclude` | none | bin/daad-compiler (out-of-tree) | none | ARCH n/a | — | canonical source |
| Full/server | REST+eval backend | src/index.js (`cluster.js` alt) | npm start/build(prisma) | node18, SQLite/PG, Redis opt | prisma client, dev.db* | none | swagger /api-docs | frontend bundle, bridge REST | self-contained stack |
| Full/frontend | bundled web UI | index.html | none (static) | browser | — | SmartEvaluator (runtime) | book.html | server API | served by server |
| Full/bridge | Qt onboarding uploader | main.cpp+qml | CMake/Qt5.15 | Qt5 | DhadBridge (out-of-tree) | none | — | server REST | admin tool |
| Dhad-Studio-Web | standalone web UI | index.html | none (static) | browser | — | SmartEvaluator | same book | server API | Netlify/Vercel target |
| Electron | desktop shell | main.js → spawns server, loads Full login | npm/electron | node runtime | — | none | — | Full server+frontend | desktop app (paths fixed) |
| VSCode-Extension | editor support | src/extension.ts (F9 compile) | tsc | vscode API, external daad-compiler | out/ (absent) | none | — | compiler binary | marketplace ext |
| cpu | Arabic CPU sims (4-bit legacy core, 8-bit main/live/visual, Qt GUI, desktop dist) | src/main.c, gui/main.cpp | qmake(GUI), gcc(manual) | Qt5 GUI, pthread | *.o, *.exe, .bin | run_tests.sh (~96) | ARCHITECTURE.md, REPORT.md, READMEs | — | distro incl. desktop/ |
| windows-x64 | compiler release | bin/daad-compiler.exe | prebuilt | Windows | — | — | README | Daad-Compiler | release bundle |
