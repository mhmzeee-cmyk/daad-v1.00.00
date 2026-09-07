# PHASE45_BEFORE_AFTER.md — Phase 3 vs Phase 4+5 (evidence only)

| Test | Phase 3 | After Repair | Result |
|---|---|---|---|
| Compiler E2E | PASS (5) | PASS (5) | SAME |
| Sandbox | 9/9 PASS | 9/9 PASS | SAME |
| Auth Login | PASS | PASS | SAME |
| Auth Refresh | FAIL (401 always) | PASS + rotation + blacklist proof | FIXED |
| Auth Logout | PASS | PASS | SAME |
| Workspace valid | PASS | PASS + reload equality | SAME+ |
| Workspace invalid FK | 500 | 404 + contract match | FIXED |
| `انتقل` | FAIL (invalid C++) | clean reject ×6, A-J intact | FIXED |
| Database | PASS | PASS (+restart persistence) | SAME |
| Redis ON | PASS | PASS | SAME |
| Redis OFF | PASS | PASS (fallback active in :3200 runs) | SAME |
| CPU | 96/96 | 96/96 | SAME |
| Bridge API | PASS | PASS (unique codes re-proven) | SAME |
| VSCode activation | PASS | STATIC VERIFIED (unchanged) | SAME |
| Electron | PARTIAL | PARTIAL (same cause) | SAME |

New regressions: NONE. All deltas are fixes or strictly-better evidence.
