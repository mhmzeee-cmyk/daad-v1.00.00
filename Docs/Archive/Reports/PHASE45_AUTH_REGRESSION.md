# PHASE45_AUTH_REGRESSION.md — detailed cycle (server :3200, temp DB/secrets)

register 201 · invalid-password 401 · login 200 · protected 200 · refresh#1 200 · refresh#2 (rotation) 200 · protected-after-rotation 200 · logout 200 · superseded-replay 401-blacklisted · missing-cookie 401 · no-CSRF 403 · wrong-OTP 401 · request-OTP 200 (no log leak).
Cookies: HttpOnly ✓ SameSite=Strict ✓ Secure absent (correct: test env, `secure: production||COOKIE_SECURE`) ✓ Max-Age correct ✓.
Semantics proven: rotation issues new tokens, old refresh blacklisted, version bump invalidates old access, logout clears cookies, brute-force lockout untouched.
Verdict: AUTH WORKS (was PARTIALLY/BROKEN in Phase 3).
