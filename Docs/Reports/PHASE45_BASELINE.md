# PHASE45_BASELINE.md — pre-repair state (Phase 3 verdicts, re-verified where cheap)

Source of truth: 11 PHASE3_*.md + Phase1/2 reports (all read). Spot re-verified: refresh-401 reproduced live before patch; `انتقل` bad-codegen reproduced; workspace-500 reproduced. Assumed-true items (matrix 10/10, 96/96, builds) re-run in full below.
Known pre-state: Compiler/Sandbox/Server/DB/Redis/CPU WORKS; Frontend/Bridge/Electron/VSCode PARTIAL; refresh BROKEN; `انتقل` BROKEN; workspace-FK 500.
Env: Linux x86_64, node22/npm9, g++15, cmake4.2, Qt5.15, python3, VSCode 1.136.1, Xvfb, redis :6379, network OK, vfat USB (no exec bit → /tmp execution), no git.
Backup: /tmp/Dhad-Studio-phase4-before-repair/ (51M) taken BEFORE first edit. Work root: /tmp/dhad-phase4-5/.
