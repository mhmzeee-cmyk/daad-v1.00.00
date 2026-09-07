# PHASE45_SECURITY_REGRESSION.md — local checks only (no external attacks)

401: invalid login, missing cookie, bad OTP, superseded refresh ✓. 403: no-CSRF refresh, student→teacher/reports/security ✓.
CSRF: token required on mutations, mismatch rejected, cookie+header pair enforced ✓.
Cookies: HttpOnly ✓ SameSite=Strict ✓ Secure per-env (absent on http-test = correct) ✓ rotation re-cookies ✓ logout clears ✓.
Crypto: bcrypt live (wrong-pass 401) ✓; OTP codes unique per user, none in logs ✓; HMAC secrets random-64hex in test ✓.
Blacklist/rotation: superseded replay 401, version bump enforced ✓ (this is the F3-01 proof).
No production attack/DoS/credential testing performed.
Verdict: SECURITY WORKS (local scope).
