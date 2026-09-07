# PHASE6_FILE_CLASSIFICATION.md — verdicts (nothing deleted this phase)

- SOURCE: all src/, include/, server/src, js/, qml, SchoolOnboarder.*, extension.ts, cpu/src+include+gui.
- RUNTIME_REQUIRED: stdlib/**, Daad/Runtime wrapper, frontend html/js/css/icons, syntaxes+snippets+lang-config, prisma/schema, bridge qml/qrc.
- BUILD_REQUIRED: CMakeLists, Makefile, qmake .pro, package.jsons, tsconfig.
- DOCUMENTATION: README.txt, guide, book.html, ARCHITECTURE/REPORT md, cpu docs.
- TEST/TEST_FIXTURE: cpu/tests+examples, Daad examples/conversions, server seed_*.js (DEV-ONLY, EXCLUDE from release).
- DEVELOPMENT_ONLY: src/_backup_original, Electron splash/titlebar dev files.
- GENERATED: *.o, *.exe/ELF in tree, desktop/*, zips in cpu/, dev.db*, node_modules.
- OPTIONAL: book.html, solutions pages, extra themes, zombie example.
- LEGACY: none confirmed (desktop/ already removed Phase 1).
- UNKNOWN (keep + review): root `__------…` DLL (5.9M, i386, std Win32 imports + odd internal names — supply-chain REVIEW, never executed); `Daad-Compiler/daad-compiler.exe` (distinct MinGW build, unreferenced); empty `Daad-Compiler/LICENSE`; root user photos `/1 /2 /3.jpeg…` (out of scope, untouched).
