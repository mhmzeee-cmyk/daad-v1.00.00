// ====================================================
// Dhad Studio — Shared Rate Limit Store + Strong Key
// ====================================================
// H8-ADD: Redis-backed store for express-rate-limit v7
//         with in-memory fallback + strong identity key.
//
// When REDIS_URL is set and Redis is reachable:
//   - Keys: rl:<prefix>:<key> with TTL = windowMs
//   - Atomic INCR + PEXPIRE via Lua
//   - On Redis failure: silent fallback to per-store MemoryMap
//
// When REDIS_URL is absent: express-rate-limit default MemoryStore
// (no change to single-worker behaviour).
// ====================================================

const crypto = require("crypto");
const { getRedisClient, isRedisAvailable } = require("../utils/redis");

// ── Lua: atomic INCR + PEXPIRE ───────────────────────────────────────────────
// Returns {totalHits, ttlMs}
const LUA_INCR_EXPIRE = `
local key = KEYS[1]
local windowMs = tonumber(ARGV[1])
local current = redis.call('INCR', key)
if current == 1 then
  redis.call('PEXPIRE', key, windowMs)
end
local ttl = redis.call('PTTL', key)
if ttl < 0 then
  redis.call('PEXPIRE', key, windowMs)
  ttl = windowMs
end
return {current, ttl}
`;

// ── Lua: atomic SETNX for nonces (returns 1 if set, 0 if exists) ─────────────
const LUA_SETNX_EXPIRE = `
local key = KEYS[1]
local ttlMs = tonumber(ARGV[1])
local result = redis.call('SET', key, '1', 'PX', ttlMs, 'NX')
if result then
  return 1
else
  return 0
end
`;

// ── Strong Identity Key ──────────────────────────────────────────────────────
// Authenticated → user.id (best: per-account, not per-IP)
// Unauthenticated → ip + SHA-256(UA[:64]) — thwarts direct-client XFF spoof
//   because trust proxy loopback gives real IP, and UA fingerprint adds
//   a second dimension so NAT-shared IPs don't merge counters.
function strongKey(req, ...extras) {
  let base;
  if (req.user?.id) {
    base = `u:${req.user.id}`;
  } else {
    const ip = req.ip || "0.0.0.0";
    const ua = (req.headers["user-agent"] || "").substring(0, 64);
    const uaHash = crypto
      .createHash("sha256")
      .update(ua)
      .digest("hex")
      .substring(0, 16);
    base = `ip:${ip}:ua:${uaHash}`;
  }
  if (extras.length > 0) {
    return `${base}:${extras.join(":")}`;
  }
  return base;
}

// ── Shared Rate Limit Store (express-rate-limit v7 compatible) ────────────────
// Returns a store object with { increment, decrement, resetKey, shutdown }.
// Each call creates a separate instance (avoids unsharedStore validation).
function getSharedStore(prefix, windowMs) {
  // Express-rate-limit v7 store interface:
  //   increment(key) → { totalHits, resetTime }
  //   decrement(key) → void
  //   resetKey(key)  → void
  //   shutdown()     → void (optional)

  const RL_PREFIX = `rl:${prefix}:`;
  const memFallback = new Map();

  // Memory fallback: periodic cleanup to prevent unbounded growth
  const memCleanup = setInterval(() => {
    const now = Date.now();
    for (const [k, v] of memFallback) {
      if (now - v.start > windowMs * 2) memFallback.delete(k);
    }
  }, windowMs).unref();

  const store = {
    localKeys: false,          // shared when Redis is up
    prefix: RL_PREFIX,

    increment: async (key) => {
      // ── Try Redis first ──────────────────────────────────────────
      if (isRedisAvailable()) {
        try {
          const client = getRedisClient();
          const result = await client.eval(LUA_INCR_EXPIRE, 1, RL_PREFIX + key, windowMs);
          const totalHits = result[0];
          const ttlMs = result[1];
          const resetTime = new Date(Date.now() + Math.max(ttlMs, 0));
          // Clear memory fallback entry on Redis success
          memFallback.delete(key);
          return { totalHits, resetTime };
        } catch {
          // Redis failure → fall through to memory
        }
      }

      // ── Memory fallback ──────────────────────────────────────────
      const now = Date.now();
      let entry = memFallback.get(key);
      if (!entry || now - entry.start >= windowMs) {
        entry = { count: 0, start: now };
      }
      entry.count++;
      memFallback.set(key, entry);
      return {
        totalHits: entry.count,
        resetTime: new Date(entry.start + windowMs),
      };
    },

    decrement: async (key) => {
      if (isRedisAvailable()) {
        try {
          const client = getRedisClient();
          await client.decr(RL_PREFIX + key);
          return;
        } catch { /* fall through */ }
      }
      const entry = memFallback.get(key);
      if (entry && entry.count > 0) entry.count--;
    },

    resetKey: async (key) => {
      memFallback.delete(key);
      if (isRedisAvailable()) {
        try {
          const client = getRedisClient();
          await client.del(RL_PREFIX + key);
        } catch { /* best-effort */ }
      }
    },

    shutdown: async () => {
      clearInterval(memCleanup);
      memFallback.clear();
    },
  };

  return store;
}

// ── Nonce Store (Redis > Memory) for apiSecurity.js ─────────────────────────
// Returns { track, has } where:
//   track(nonce, ttlMs) → records nonce; returns true on success
//   has(nonce)          → true if nonce already seen
function getNonceStore(maxSize = 10000, defaultTtlMs = 10 * 60 * 1000) {
  const memNonces = new Map();

  // Memory-only cleanup (lazy expiry)
  function memTrack(nonce, ttlMs) {
    const now = Date.now();
    if (memNonces.size >= maxSize) {
      for (const [k, ts] of memNonces) {
        if (now - ts > ttlMs) memNonces.delete(k);
      }
      if (memNonces.size >= maxSize) {
        memNonces.delete(memNonces.keys().next().value);
      }
    }
    memNonces.set(nonce, now);
  }

  return {
    track: async (nonce, ttlMs = defaultTtlMs) => {
      if (isRedisAvailable()) {
        try {
          const client = getRedisClient();
          await client.eval(LUA_SETNX_EXPIRE, 1, `sec:nonce:${nonce}`, ttlMs);
          memNonces.delete(nonce); // clean memory on Redis success
          return true;
        } catch { /* fall through */ }
      }
      memTrack(nonce, ttlMs);
      return true;
    },

    has: async (nonce) => {
      if (isRedisAvailable()) {
        try {
          const client = getRedisClient();
          const exists = await client.exists(`sec:nonce:${nonce}`);
          if (exists === 1) return true;
          // Not in Redis — check memory too (was stored before Redis came up)
          return memNonces.has(nonce);
        } catch { /* fall through */ }
      }
      return memNonces.has(nonce);
    },

    cleanup: () => {
      memNonces.clear();
    },
  };
}

module.exports = {
  getSharedStore,
  getNonceStore,
  strongKey,
};
