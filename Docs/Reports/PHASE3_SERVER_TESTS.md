# PHASE3_SERVER_TESTS.md — runtime on 127.0.0.1 (3100, then 3000)

Setup: server copied to /tmp, `npm install` 485 pkgs exit 0; `prisma validate` 0; `prisma generate` 0; `db push` 0 → test.db 622K. Boot env: NODE_ENV=test, temp random JWT/HMAC (64 hex), ALLOWED_ORIGINS localhost. Boot log: `Dhad Studio Server started port 3100 address 127.0.0.1`, WAL on, `Redis not configured, running without cache` (fallback by design).

## Health: /health, /health/ready, /health/live → 200 (13ms).
## Auth (TEST user only, no secrets recorded)
- register STUDENT+schoolId → 201 (missing schoolId → 400 ✓).
- login username/password → 200 + JWT cookies; wrong password → 401 ✓.
- logout → 200 ✓ (earlier 401 was test-sequencing artifact: refresh clears cookies).
- **refresh → 401 ALWAYS (FOUND CODE BUG)**: `isRefreshTokenBlacklisted()` returns async Promise, tested truthy at authController.js:579-580. Single-site, precisely characterized. NOT fixed (Phase 3 rule).
## API smoke (student / teacher / anon)
- 200: student/profile, challenges, student/courses, student/leaderboard, workspace/list, teacher/students (teacher), analytics/dashboard (teacher).
- 403 RBAC: student→teacher/students, →reports/school-overview, →security/dashboard ✓. Teacher register w/o auth → 401 ✓.
- 404: submit bad challengeId (clean Arabic error) ✓. 401: no-cookie profile ✓.
- workspace/save bad FK → 500 (missing pre-check → robustness note, not data bug); valid id → 200 ✓.
- submit correct → passed:true score:100 xpAwarded:100 ✓; repeat → alreadyPassed idempotency ✓.
- teacher created via prisma (test DB): teacher endpoints 200 ✓.
## DB/Redis
- SQLite: generate/push/query/upsert verified live; XP persisted across restart (leaderboard totalXP:100 ranking:1).
- Redis: server ran WITHOUT (fallback leaderboard 200 ✓); restarted WITH REDIS_URL → `Redis connected/ready`, leaderboard 200 with persisted XP ✓. Both paths WORKS.
