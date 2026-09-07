# PHASE6_RELEASE_INVENTORY.md — per-component release roles

| Component | Purpose | Entry | Build | Runtime deps | Build deps | Config | Assets | Generated | Req/Opt/Dev-only |
|---|---|---|---|---|---|---|---|---|---|
| Daad-Compiler | ض→C++20 | src/main.cpp | CMake 3.21+/C++20, Makefile | none (binary+headers+stdlib at user compile time: g++) | g++,cmake | none | none | bin/daad-compiler | src+include+stdlib+Daad wrapper REQUIRED; examples/docs OPTIONAL; _backup_original DEV-ONLY |
| Full/server | REST API+eval | src/index.js (cluster.js alt) | npm scripts | node≥18, node_modules(485 pkgs), SQLite file, Redis opt | npm, prisma engines (network at install) | 33 env keys (see ENV audit) | none | prisma client, *.db* | src+prisma+package.json REQUIRED; tests/ absent though scripted; seeds DEV-ONLY |
| Full/frontend = Web | static UI | index.html | none (static host) | browser; API base localhost:3000 default; fonts CDN | none | none | icons.svg, css, js/lib | none | all REQUIRED except book.html OPTIONAL |
| Electron | desktop shell | main.js | electron-builder scripts present (build/build:mac/build:linux) | electron runtime, Full tree alongside, server node_modules | npm | none | splash/titlebar (icon.ico MISSING) | none | main+preload+package REQUIRED |
| Bridge | Qt onboard tool | main.cpp+qml | CMake/Qt5.15 Core/Quick/Qml/Network/Widgets | Qt5 runtime, server URL (default localhost:3000) | Qt5 dev, cmake | serverUrl in-app | qml+qrc | DhadBridge (tree copy STALE) | sources REQUIRED |
| VSCode ext | editor support | out/extension.js (tsc) | tsc ^5 | vscode ^1.80, external daad-compiler on PATH | npm+tsc | package.json contributes | syntaxes/snippets/lang-config | out/ (absent in tree) | package+out+syntaxes REQUIRED |
| cpu | simulators (intentional variants) | src/main.c, gui/main.cpp, live, visual | qmake(GUI)/gcc(manual) | Qt5 (GUI only), libc (sims) | Qt5 dev, gcc | none | examples/tests | *.o,*.exe,desktop/*,zips | src+include+examples+tests REQUIRED; build outputs OPTIONAL |
| windows-x64 | compiler release bundle | bin/daad-compiler.exe | prebuilt (MinGW fam.) | Windows x64 | — | none | examples/include/stdlib copies | — | REQUIRED as bundle |
