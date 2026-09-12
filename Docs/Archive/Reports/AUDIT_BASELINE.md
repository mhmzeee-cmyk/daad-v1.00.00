# AUDIT_BASELINE.md — Dhad Studio v1.0.0 (post-cleanup)

**Date:** 2026-09-06 · **Scope:** inventory only, no code changes · **Repo:** not a git repo · **Backup:** `/tmp/Dhad-Studio-backup-before-cleanup/`

## Components (7 dirs, ~827 files + reports)

| Component | Files | Entry point | Build system | Deps | Tests |
|---|---|---|---|---|---|
| Daad-Compiler | 284 | src/main.cpp (CLI `daad-compiler`) | CMake 3.21+/C++20, Makefile (`-Iinclude`) | none mandatory (GTest opt, Qt5 opt) | none (tests/ missing) |
| Dhad-Studio-Full/server | ~110 | src/index.js (Express) | npm (package.json) | express, prisma, ioredis, bcrypt, jsonwebtoken, helmet, winston, swagger | none |
| Dhad-Studio-Full/frontend | 70 | index.html + js/*.js | none (vanilla JS, Netlify _redirects) | none (CDN fonts, local html2canvas/jspdf) | none (SmartEvaluator is runtime check) |
| Dhad-Studio-Full/bridge | 6+2 new | main.cpp (new) + bridge_main.qml | CMake/Qt5.15 (Core Quick Qml Network Widgets) | Qt5 | none |
| Dhad-Studio-Web | 70 | index.html (mirror of Full frontend) | none (Vercel/Netlify) | same as Full frontend | none |
| Dhad-Studio-Electron | 527 (incl. node_modules) | main.js (spawns server, loads login.html) | npm (electron) | electron | none |
| VSCode-Extension | 6 | src/extension.ts (`daad.compileCurrentFile`, F9) | tsc (tsconfig, ES2020) | vscode API | none |
| cpu | 154 | src/main.c (`dhad_cpu`), gui/main.cpp, dhad_live.cpp, visual_sim.c | qmake (GUI), manual gcc for sims | Qt5 (GUI), pthread | run_tests.sh (~96 tests) |
| daad-studio-v1.0.0-windows-x64 | 153 | bin/daad-compiler.exe | prebuilt release | — | — |

## APIs (server, base /api/v1)
auth (login/register/refresh/logout/OTP/student-login), challenges, student (profile/roadmap/submit/assessments/workspace), teacher (students/invitations), analytics, assessments, schools, reports, onboard-school(+/files), security, courses, health, api-docs.

## Databases
- prisma/schema.prisma (SQLite dev, `dev.db`), schema.postgresql.prisma (prod). 25+ models: School, User (ADMIN/TEACHER/STUDENT), Classroom, Challenge, Submission, Assessment, StudentProfile, Achievement, Attendance, ActivityLog, Invitation, CloudWorkspace, Course, Lesson, ChallengeVerification…
- Redis (ioredis, optional, in-memory fallback) for leaderboard/cache/rate-limit counters.

## Config / runtime
- Server env: DATABASE_URL, JWT_SECRET, HMAC_SECRET (+HMAC_CONFIRM_SECRET new), ALLOWED_ORIGINS, NODE_ENV, REDIS_URL, SEED_ADMIN_PASSWORD (new).
- Dhad-Studio.bat: npm install → prisma generate/db push → start browser → node src/index.js (NODE_ENV=production).

## Cross-component deps
- Electron → server/src/index.js (spawn) + frontend-web/login.html.
- Bridge → server REST (/auth/login, /onboard-school/files, /auth/request-otp, /auth/verify-otp).
- VSCode ext → external `daad-compiler` binary in PATH.
- Full frontend ≡ Web frontend (identical files, two deploy targets). Server bundles its own copy of dhad lexer/parser/ast/codegen for server-side eval.
- Generated C++ needs `-I<Daad-Compiler>` (resolves `Daad/Runtime/...` via new wrapper + `stdlib/...`).
