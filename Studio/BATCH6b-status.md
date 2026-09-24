# BATCH 6b — Server Dedup Sorting Report
**Date:** 2026-09-15
**Scope:** `Studio/Full/server` ↔ `Studio/desktop-app/server`
**Risk:** HIGH — Sort and prepare only. No final merge without decision.

---

## 1. Diff Summary

| Item | Result |
|------|--------|
| `src/` directories | **IDENTICAL** (diff -rq EXIT:0) |
| `package.json` | **IDENTICAL** (diff -q no output) |
| `Dockerfile` | **IDENTICAL** (diff -q no output) |
| `docker-compose.yml` | **IDENTICAL** |
| `render.yaml` | **IDENTICAL** |
| `ecosystem.config.js` | **IDENTICAL** |
| `prisma/` schema/migrations | **IDENTICAL** (excluding dev.db WAL/SHM runtime artifacts) |

### Only Differences (non-src):
- `Studio/Full/server/aws/` — AWS deployment configs (ALB, ECS, IAM, etc.)
- `Studio/Full/server/.env.archived` — Archived env file
- `Studio/desktop-app/server/prisma/dev.db-shm` + `dev.db-wal` — SQLite runtime artifacts

### Git-tracked files:
- `Studio/Full/server`: **120 files**
- `Studio/desktop-app/server`: **112 files** (8 fewer — likely the `aws/` dir + archived)

---

## 2. How desktop-app Spawns Server

**File:** `Studio/desktop-app/main.js` (Electron main process)

```js
// Line 4
const { spawn } = require("child_process");

// Line 39-41
const serverPath = path.join(__dirname, "server", "src", "index.js");
serverProcess = spawn(process.execPath, [serverPath], {
  cwd: path.join(__dirname, "server"),
});
```

**Key:** Desktop-app already uses a **direct spawn** of its own `server/src/index.js` with `cwd` set to `server/`. No import/require aliasing. Self-contained.

---

## 3. External References to Servers

| File | References |
|------|-----------|
| `Demo/run-demo.sh:91` | `desktop-app/server` |
| `Demo/start-server.sh:9` | `desktop-app/server` |
| `Tests/run_all_tests.sh` | Lines 460, 477, 491, 549, 574 → all `desktop-app/server` |

**No references to `Full/server`** found in shell scripts or test harnesses. Full/server is referenced only in `aws/` config files (self-referential).

---

## 4. Prisma / DB

Both servers use identical Prisma setup (line 285-287 in both `index.js`):
```js
const prisma = require("./utils/prisma");
app.set("prisma", prisma);
```
Same disconnect handler at lines 438-440. Schema and migrations identical.

---

## 5. Syntax Check

`node --check` on both `index.js` files: **PASS** (no errors, no output = clean).

---

## 6. Backup / Manifest

- **Path:** `/tmp/opencode/BATCH6b-backup/server-double-20260915.tar.gz`
- **SHA256:** `d1664dd4002c1e7ed16acdd247bd9f6050c268f02fc4c667f6705f96d849de39`
- **Contents:** `src/` + `package.json` from both servers

---

## 7. Canonical Recommendation

### ✅ Canonical = `Studio/Full/server`

**Reasons:**
1. Contains `aws/` deployment configs (Full deployment path)
2. Contains `.env.archived` (history)
3. Same code, same schema, same Dockerfile
4. Name "Full" implies it's the primary deployment target

### ✅ Recommended Binding: **Direct Spawn (current approach)**

Desktop-app already spawns `server/src/index.js` directly — no copy needed. Two options:

#### Option A: Symlink (recommended for dev)
```bash
# Remove desktop-app/server/src (identical copy)
rm -rf Studio/desktop-app/server/src

# Create symlink to canonical
ln -s ../../Full/server/src Studio/desktop-app/server/src
```
- **Pro:** Zero code duplication, single source of truth
- **Con:** Requires same `node_modules` / `package.json` (already identical)
- **Risk:** Git tracks symlink target, not content — review carefully

#### Option B: Keep Copy, Sync Script (recommended for prod)
```bash
# Create a sync script: scripts/sync-server.sh
rsync -av --delete Studio/Full/server/src/ Studio/desktop-app/server/src/
```
- **Pro:** Desktop-app remains fully self-contained (offline Electron builds)
- **Con:** Manual sync step, risk of drift
- **Use if:** Electron app needs to ship standalone with embedded server

#### Option C: Monorepo with Shared Package (future)
```json
// Studio/shared-server/package.json → symlink or workspace
// Both servers depend on @dhad/server-core
```
- **Pro:** Clean separation
- **Con:** Refactor needed — defer to separate decision

---

## 8. Deferred (Awaiting Decision)

| Item | Status |
|------|--------|
| Actual merge/deletion | **BLOCKED** — awaiting decision |
| Symlink vs copy-sync approach | Requires orchestrator decision |
| Update Demo/Tests refs if canonical changes | Pending |
| .env handling for both environments | Pending |

---

## 9. Files Touched

- `Studio/BATCH6b-status.md` — This report (NEW)

**No files modified, deleted, or committed.**
