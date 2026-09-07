# PHASE6_RELEASE_BLOCKERS.md — classified findings (IDs stable for Phase 7)

- RB-01 R1 — `Daad-Compiler/LICENSE` empty → action: owner supplies license text. Phase: pre-packaging.
- RB-02 R1 — No Prisma migrations (only `db push`) + no ecosystem/service file though `npm run prod` cites pm2 config → action: adopt migrations + define process mgmt. Phase: packaging.
- RB-03 R2 — `npm test` broken (jest+supertest declared, tests/ + mega script + ecosystem file absent) → action: add tests or drop scripts. Phase: packaging.
- RB-04 R2 — `.env.example` missing; DATABASE_URL empty-string behavior; ALLOWED_ORIGINS warn-only → action: template + strict prod checks. Phase: packaging.
- RB-05 R2 — Windows binaries (3× .exe + DLL) irreproducible here; tree DhadBridge STALE → action: rebuild all from source on target toolchains; review-or-drop DLL. Phase: packaging.
- RB-06 R2 — VSCode `out/` unbuilt in tree; daad-compiler PATH step undocumented → action: prepublish build + docs. Phase: packaging.
- RB-07 R3 — `openai` dep unused; test-seed creds advertised; fonts CDN offline gap; SW APP_SHELL gaps; icon.ico missing; CSRF fallback edge → actions noted per doc. Phase: packaging polish.
- RB-08 R2 — Dual frontend copies need single source of truth decision → action: owner picks bundled-vs-standalone flow. Phase: packaging.
- RB-09 R5 — DHAD_START_SERVER env consumer unconfirmed; RAILWAY/RENDER/HEROKU/AWS/LAMBDA keys are platform auto-detects (informational).
