# DEEP_AUDIT_REPORT.md — Dhad Studio v1.0.0

**Method:** re-read + reproduce, old report treated as BASELINE not truth. Verdicts: CONFIRMED / REJECTED / RECLASSIFIED / NEW.

## Executive summary
21 real issues fixed; 4 old-report claims rejected or downgraded after reproduction; 0 regressions. Compiler builds (was unbuildable), full ض→C++→binary→run pipeline verified (output `5`), Sandbox blocks 7/7 attack strings, Bridge builds, CPU binary correct (10+20=30).

## Compiler
- P0 CONFIRMED: `include/` missing (11 headers reconstructed, project now builds).
- P0 CONFIRMED (worse than reported): SandboxValidator `string::find(regex)` — 15/17 patterns dead. Fixed with `std::regex_search` + indented-comment skip. Verified 9/9 (SAFE/UNSAFE/whitespace-bypass/comment/dangerous-header).
- P1 CONFIRMED: `KwBase→"this"`, `KwFunction→"void"` — but RECLASSIFIED: `cppEquivalent` is metadata-only (zero users outside Keywords.cpp, verified by grep). Set to `""` to fail visibly if ever used.
- P1 CONFIRMED: power `^` left-assoc (math-wrong for chains). Documented, NOT fixed (needs grammar decision + test vectors — future phase).
- P1 CONFIRMED: image-parse dead branch in Parser (isNextLeftParen shadows parseLoadImage…). Documented, NOT fixed (needs grammar tests first).
- P2 REJECTED: `DaadRuntime.hpp` unconditional CoreFoundation — actually `#ifdef __APPLE__` guarded. No bug.
- P2 CONFIRMED: hex/scientific numbers unsupported in Lexer. Documented, NOT fixed (grammar extension).
- NEW P0 (found during e2e): generated `#include "Daad/Runtime/..."` had no matching file. Fixed with `Daad/Runtime/DaadRuntime.hpp` wrapper → full pipeline verified.
- STUBs (700+ in DaadStdlibExt.hpp): CONFIRMED as declared; NOT removed (public surface). Mitigated by docs; real implementation is future phase.

## Server
- CRITICAL CONFIRMED + FIXED: `admin123` seed → env `SEED_ADMIN_PASSWORD` or random, min-length 8, never prints static secret.
- CRITICAL CONFIRMED + FIXED: shared batch OTP → unique OTP+hash per user, `activationCodes[]`; backward-compat singular kept for single-user onboard; Bridge updated to read array with fallback.
- HIGH CONFIRMED + FIXED: `Function()` in serverEvaluator → shunting-yard calculator (10/10 functional tests, isFinite guard, no eval).
- HIGH RECLASSIFIED → NOT A BUG: CSRF Bearer bypass is correct (Bridge is non-browser, no ambient creds; browser uses cookies+CSRF). Untouched.
- HIGH RECLASSIFIED → ACCEPTED RISK: tokens in JSON response required by Bridge (Bearer). Frontend `setToken` is no-op; PII stripped from localStorage instead.
- MEDIUM FIXED: `HMAC_CONFIRM_SECRET` auto-generate (missing/insecure/equals HMAC_SECRET), secret never fully logged.
- MEDIUM FIXED: error handler fail-closed when `NODE_ENV` unset (`isProd`).
- MEDIUM FIXED: `setUser` keeps only name/role/displayName in localStorage.
- LOW documented: in-memory audit log, `trust proxy 1`, Swagger gating — future hardening.

## Frontend
- CRASH CONFIRMED + FIXED: `new DhadAPI()` (undefined) → removed, uses global `ApiClient` (api.js loads first — verified script order).
- CRASH CONFIRMED + FIXED: 7× `Toast.show` → `toast.error/success` (toast.js loads before settings-page.js — verified).
- WARN: `new Function` in dhad.js is inherent to browser compiler; Parser is the guard. Documented.
- WARN: sw.js omits some JS from APP_SHELL but strategy is network-first → offline gap only. Documented.

## Electron / Bridge / VSCode / CPU
- Electron: contextIsolation ON, nodeIntegration OFF, IPC window-controls only — PASS, untouched. `unsafe-inline` accepted for local app.
- Bridge: `main.cpp` + `bridge.qrc` CONFIRMED missing → created minimal (qmlRegisterType DaadBridge 1.0, QApplication, qrc:/bridge_main.qml) → `cmake+make` 100% clean.
- VSCode: block comments missing CONFIRMED → added `comment.block.daad` rule, JSON valid.
- CPU: 4 simulators are INTENTIONALLY_DIFFERENT (4-bit legacy core vs 8-bit main/live/visual). `dhad_cpu` perm issue RECLASSIFIED as ENVIRONMENT (vfat USB can't store exec bit; binary verified correct from /tmp: 10+20=30). No code change.

## Remaining risks (future phases)
P1 grammar (power assoc, image dead-branch, GOTO unimplemented); STUB library implementation; in-memory audit persistence; `trust proxy` tightening; offline SW coverage; hex/scientific literals.
