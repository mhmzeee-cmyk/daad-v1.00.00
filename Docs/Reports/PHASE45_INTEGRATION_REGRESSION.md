# PHASE45_INTEGRATION_REGRESSION.md — S1/S2/S3 re-run

## S1 Frontend→Login→Server→DB→Evaluator→XP→Leaderboard: PASS
register→login→submit(passed,xp100)→leaderboard(XP:100,rank 1)→restart(redis ON)→leaderboard persists. RBAC/401 gates hold throughout.
## S2 Electron→Full Server→Full Frontend: BLOCKED (unchanged)
Main process + resolved paths re-verified; backend needs `npm install` (documented step, by design). No masking.
## S3 Bridge→Server→Login→Onboarding→OTP: PASS
login 200, onboard 201 (unique codes), request-otp 200, wrong-otp 401. Correct-code path still needs mail trap.
## Unicode end-to-end: PASS (`يونيكود.ض` filename, Arabic ids/strings/`؛` → مرحبا + 5, no mojibake).
