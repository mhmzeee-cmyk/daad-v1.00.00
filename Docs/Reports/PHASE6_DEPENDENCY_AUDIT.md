# PHASE6_DEPENDENCY_AUDIT.md — complete map (versions pinned by ^ ranges; nothing upgraded)

- server (node≥18): express 4.21, @prisma/client+prisma 5.22, jsonwebtoken 9, bcryptjs 2.4, ioredis 5, helmet 8, winston 3, multer 2, xlsx 0.18, openai 6.46 (**imported NOWHERE in src → dead weight + surface; R3 remove-candidate, NOT removed**), msgpack, compression, cookie-parser, dotenv, swagger×2. devDeps: jest 30 + supertest 7 (NO tests/ dir → `npm test` broken; scripts also cite missing ecosystem.config.js, Dockerfile, mega simulator → R3). postinstall: `prisma generate` (needs engine download = network at install).
- Electron: electron ^33.4.11 (2 deps/2 devDeps); needs FULL BUILD (build:linux/mac scripts present, never run here).
- VSCode ext: 0 deps; devDeps typescript ^5 (+@types). engines vscode ^1.80.
- Web frontend: 0 deps (vanilla; html2canvas/jspdf vendored in js/lib).
- Compiler: no third-party (ex-GoogleTest fetch only when tests ON; system g++/cmake).
- Qt5.15 (bridge+cpu GUI), g++, cmake, qmake, python3 (dev/test scripts only).
- Native modules: none (bcryptjs pure-JS ✓ — no node-gyp risk).
- No curl|sh installers found. No dependency changed this phase.
