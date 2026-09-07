# PHASE3_BASELINE.md — Full Runtime Verification (2026-09-06)

- Date: 2026-09-06 · Root: USB `UBUNTU 26_0/<NBSP>/` (vfat; NOT A GIT REPOSITORY, no init).
- Phase 2 status: SUCCESS (re-verified, not assumed).
- Test root: `/tmp/dhad-phase3/` (compiler-build, bridge-build, generated, server, db, logs, tests, artifacts). Zero project modifications (read-only + localhost + /tmp).
- Tools: Linux 7.0.0-31 x86_64 · node v22.22.1 · npm 9.2.0 · g++ 15.2.0 · cmake 4.2.3 · qmake 3.1 (Qt 5.15) · python3 3.14.4 · VSCode 1.136.1 · DISPLAY=:0 (+Xvfb) · `import` (screenshot tool) ✓ · electron: MISSING binary (installed 33.x into /tmp for test) · redis-server PRESENT, 127.0.0.1:6379 OPEN · network: AVAILABLE (registry 200).
- No ENVIRONMENT BLOCKERs except: no tsc in tree (worked around via /tmp install), no electron binary in tree (worked around via /tmp install).
