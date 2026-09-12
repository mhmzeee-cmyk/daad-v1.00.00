# PHASE45_REPAIR_PLAN.md — order F3-01 → F3-02 → F3-03 (one at a time)

## F3-01 refresh 401 (P0)
Root cause (verified): `isRefreshTokenBlacklisted()` sync wrapper returns async Promise → always truthy at authController.js:580. Related sites audited: rotation blacklist+version bump (correct), logout blacklist (body-only, fine), no other callers of wrapper.
Patch (2 lines): `async function isRefreshTokenBlacklisted` + `await` at call site. No API/contract change.
Tests: full cycle register→login→refresh→refresh→logout→revoked + invalid/missing-cookie + rotation supersede + cookie flags.

## F3-02 `انتقل` bad codegen (P1)
Trace: Lexer KwGoto ✓ → Parser NO branch (falls through, emits stray identifier) → no AST node → no CodeGen. Smallest correct point: explicit Parser rejection (labels don't exist; goto support = new syntax, forbidden). Patch: branch after توقف/استمر reporting unsupported + skip-to-semicolon recovery + nullptr (existing hasErrors→exit 1 path).
Tests: 6 jump variants (label/block/cond/loop/multi/undef) → clean reject, no crash; A-J still 10/10.

## F3-03 workspace FK 500 (P2)
Root cause: upsert without existence check (submit route HAS the check → 404 contract exists). Patch: same `findUnique`→404 `{"error":"التحدي غير موجود"}` before upsert. Other DB errors still reach error middleware (no masking).
Tests: valid 200+persisted, invalid 404, malformed 400, no-auth 403/401, reload equality.
