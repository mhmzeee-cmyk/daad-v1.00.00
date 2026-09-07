# PHASE2_REFERENCE_AUDIT.md — cross-component reference graph

Method: `grep -R` for dir names across js/json/ts/html/cmake/bat/sh/pro + relative-path review of entry points.

## Result: components are self-contained (zero dir-name cross-refs)
No file outside its own top dir references another top dir by name.

## Real edges (verified)
- Electron → Full: RUNTIME-ONLY via `main.js` (spawn `../Dhad-Studio-Full/server/src/index.js`, load `../Dhad-Studio-Full/frontend/frontend-web/*.html`) + `http://localhost:*`. **Was BROKEN** (`../server`, `../frontend-web` matched no layout anywhere) → FIXED (4 path strings, verified resolvable + `node --check`).
- Electron → self: splash.html, preload.js, titlebar (same dir) ✓.
- Dhad-Studio.bat → `server/` sibling ✓ (consistent with Full layout, untouched).
- Bridge → server REST only (`/auth/login`, `/onboard-school/files`, `/auth/request-otp`, `/auth/verify-otp`) — no path coupling ✓.
- VSCode ext → `daad-compiler` on PATH (external, documented) ✓.
- Frontend → server REST `/api/v1/*` only ✓. Server → DB/Redis via env ✓.
- Generated C++ → `Daad/Runtime/...` + `stdlib/...` (resolved by `-I<Daad-Compiler>` + wrapper) ✓.

## Broken refs fixed this phase
- `Dhad-Studio-Electron/main.js` lines 38/40/88/176 (server + login + nav paths). Missing `icon.ico` recorded as UNKNOWN asset (non-fatal; needs design file in packaging phase).

## Stale doc refs fixed
- `دليل-التثبيت-والتشغيل.txt` tree listed deleted `Dhad-Studio-v1.0.0-All.zip` + `Dhad-Studio-v1.0.0-Windows.zip` → 2 lines removed. Cleanup/audit reports intentionally still mention deleted paths (historical record, untouched).
