# PHASE3_INTEGRATION_REPORT.md — cross-component scenarios

## S1 User→Frontend→Login→Server→DB→Evaluator→Result: WORKS
register 201 → login 200 (JWT cookies) → profile/challenges 200 → submit correct → passed:true xp:100 → leaderboard shows XP:100 rank:1 (persisted across restart). Wrong-creds 401, no-cookie 401, student→teacher 403.

## S2 Electron→Full Server→Full Frontend: PARTIALLY
Main process+splash+window code run 40s+ headless; resolved server+login paths exist; BLOCKED only by unshipped `server/node_modules` (proven: same dir boots with deps). Backend spawn design verified by control experiment.

## S3 Bridge→Server API: WORKS
login→onboard(201, unique codes live)→request-otp(200)→verify-otp rejects bad (401); correct-code path needs mail trap. QML app itself: offscreen survival, resource bundle verified.

## Data integrity notes
- All TEST data in /tmp/test.db; production `dev.db` untouched. No secrets recorded (random 64-hex env, OTP codes in report are throwaway test values on temp DB — bt/BS accounts).
- Refresh-token rotation: BROKEN (FOUND bug, authController.js:579 missing await) — logout verified independently (200).
- workspace/save without valid FK → 500 instead of 404 (robustness note).
