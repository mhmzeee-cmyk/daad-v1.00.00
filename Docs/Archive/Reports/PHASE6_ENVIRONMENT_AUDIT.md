# PHASE6_ENVIRONMENT_AUDIT.md — 33 keys (no values recorded; no .env in tree)

| Variable | Required | Default | Prod? | Safe default? |
|---|---|---|---|---|
| NODE_ENV | yes | development(!) | production | NO → fail-closed error handler added P1; startup default still dev (doc) |
| PORT/HOST | no | 3000/0.0.0.0 | set explicitly | HOST=127.0.0.1 recommended (R4) |
| DATABASE_URL | yes | none (dies perversely if empty — prisma.js:31 `''`) | yes | must document (R2) |
| JWT_SECRET / JWT_REFRESH_SECRET | yes | auto-generate+warn | yes | yes (auto) |
| HMAC_SECRET / HMAC_CONFIRM_SECRET | yes | auto-generate (P1) | yes | yes (auto) |
| CSRF_SECRET | no | derived HMAC(JWT_SECRET‖'fallback-csrf') | set it | weak-only-if-JWT-missing (R3) |
| ENCRYPTION_KEY | feature-gated | none + CRITICAL warn, throws on use | yes if apiSecurity encryption used | fail-closed ✓ (document which feature) |
| REDIS_URL | no | unset → in-memory fallback (proven both) | recommended | yes |
| ALLOWED_ORIGINS | yes-prod | dev-open logged | yes (CORS matrix in server tests) | warn-only (R3: consider fail-closed) |
| COOKIE_SECURE | no | prod-only Secure | true behind HTTPS | correct per-env |
| HEALTH_SECRET | no | open /health (status only) | set for detail | ok |
| RATE_LIMIT_* (7) | no | built-ins | tune | ok |
| SEED_ADMIN_PASSWORD | seed-only | random+print once | n/a (local) | yes (P1) |
| JWT_ACCESS/REFRESH_EXPIRES | no | 3600/7d | tune | ok |
| LOG_LEVEL | no | info | warn+ | ok |
| WEB_CONCURRENCY/TCP_BACKLOG | no | defaults | tune | ok |
| DHAD_START_SERVER | bridge? | check packager | — | R5: confirm consumer |
| RENDER/RAILWAY/HEROKU/AWS_LAMBDA | no | platform auto-detect | n/a | informational |

Missing: `.env.example` (MISSING — R2: create from this table in packaging phase).
