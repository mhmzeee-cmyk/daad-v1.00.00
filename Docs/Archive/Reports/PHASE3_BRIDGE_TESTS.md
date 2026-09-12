# PHASE3_BRIDGE_TESTS.md — build + offscreen + API (server :3000)

- Build: `cmake -S bridge -B /tmp/...` 0, `cmake --build` 0 → `DhadBridge` ELF 152K.
- Runtime (QT_QPA_PLATFORM=offscreen): survives 8s and 25s (exit 124 = event loop alive; instant exit -1 on QML failure by main.cpp design). Binary embeds bridge_main.qml resources (165 string hits). Real window interaction NOT VERIFIED (headless).
- API integration (TEST data, admin user in temp DB):
  1. POST /auth/login (admin) → 200.
  2. POST /onboard-school JSON (1 teacher + 1 student, no passwords) → 201, `activationCodes: [{bt…806152},{P3N1…666713}]` — TWO DIFFERENT codes = Phase-1 unique-OTP fix verified LIVE.
  3. POST /auth/request-otp → 200 (works without ENCRYPTION_KEY; no code in logs = no leak).
  4. POST /auth/verify-otp wrong code → 401 OTP_INVALID ✓ (correct-code path needs mail trap → NOT VERIFIED).
- Verdict: BUILD WORKS, API WORKS, RUNTIME PARTIALLY (offscreen survival).
