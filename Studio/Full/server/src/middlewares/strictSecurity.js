// ====================================================
// Dhad Studio - Strict Security Middleware
// ====================================================
// All security measures in one place
// ====================================================

const crypto = require("crypto");
const fs = require("fs");
const path = require("path");

// ── Persistent Store (Redis > Memory) ────────────────────────────────────────
const { getRedisClient, isRedisAvailable } = require("../utils/redis");
const REDIS_PREFIX = "sec:";

// In-memory fallback Maps
const memFailedAttempts = new Map();
const memIPBlocks = new Map();
const memRequestCounts = new Map();
const memActiveSessions = new Map();

// ── Redis helpers with fallback ──────────────────────────────────────────────
async function redisGet(key) {
  try {
    if (!isRedisAvailable()) return null;
    const client = getRedisClient();
    const val = await client.get(REDIS_PREFIX + key);
    return val ? JSON.parse(val) : null;
  } catch { return null; }
}

async function redisSet(key, value, ttlSeconds) {
  try {
    if (!isRedisAvailable()) return false;
    const client = getRedisClient();
    if (ttlSeconds) {
      await client.setex(REDIS_PREFIX + key, ttlSeconds, JSON.stringify(value));
    } else {
      await client.set(REDIS_PREFIX + key, JSON.stringify(value));
    }
    return true;
  } catch { return false; }
}

async function redisDel(key) {
  try {
    if (!isRedisAvailable()) return false;
    const client = getRedisClient();
    await client.del(REDIS_PREFIX + key);
    return true;
  } catch { return false; }
}

// ── failedAttempts (Redis > Memory) ──────────────────────────────────────────
async function getFailedAttempt(identifier) {
  const redis = await redisGet("fa:" + identifier);
  if (redis !== null) return redis;
  return memFailedAttempts.get(identifier) || null;
}

async function setFailedAttempt(identifier, record) {
  const ttl = Math.ceil(CONFIG.FAILED_ATTEMPT_WINDOW_MS / 1000) + 60;
  const stored = await redisSet("fa:" + identifier, record, ttl);
  if (!stored) {
    // Hard cap to prevent unbounded growth under DDoS without Redis
    if (memFailedAttempts.size > 50000) {
      // Evict oldest 10% of entries
      const keys = [...memFailedAttempts.keys()];
      const evictCount = Math.floor(keys.length * 0.1);
      for (let i = 0; i < evictCount; i++) {
        memFailedAttempts.delete(keys[i]);
      }
    }
    memFailedAttempts.set(identifier, record);
  }
}

async function deleteFailedAttempt(identifier) {
  await redisDel("fa:" + identifier);
  memFailedAttempts.delete(identifier);
}

// ── ipBlocks (Redis > Memory) ────────────────────────────────────────────────
async function getIPBlock(ip) {
  const redis = await redisGet("ip:" + ip);
  if (redis !== null) return redis;
  return memIPBlocks.get(ip) || null;
}

async function setIPBlock(ip, record) {
  const ttl = Math.ceil(CONFIG.IP_BLOCK_DURATION_MS / 1000) + 60;
  const stored = await redisSet("ip:" + ip, record, ttl);
  if (!stored) {
    if (memIPBlocks.size > 50000) {
      const keys = [...memIPBlocks.keys()];
      const evictCount = Math.floor(keys.length * 0.1);
      for (let i = 0; i < evictCount; i++) memIPBlocks.delete(keys[i]);
    }
    memIPBlocks.set(ip, record);
  }
}

async function deleteIPBlock(ip) {
  await redisDel("ip:" + ip);
  memIPBlocks.delete(ip);
}

// ── requestCounts (Redis > Memory) ───────────────────────────────────────────
async function getRequestCount(key) {
  const redis = await redisGet("rc:" + key);
  if (redis !== null) return redis;
  return memRequestCounts.get(key) || null;
}

async function setRequestCount(key, record) {
  const ttl = Math.ceil(CONFIG.RATE_LIMIT_WINDOW_MS / 1000) + 60;
  const stored = await redisSet("rc:" + key, record, ttl);
  if (!stored) {
    if (memRequestCounts.size > 50000) {
      const keys = [...memRequestCounts.keys()];
      const evictCount = Math.floor(keys.length * 0.1);
      for (let i = 0; i < evictCount; i++) memRequestCounts.delete(keys[i]);
    }
    memRequestCounts.set(key, record);
  }
}

// ── activeSessions (Redis > Memory) ──────────────────────────────────────────
async function getActiveSessions(userId) {
  const redis = await redisGet("sess:" + userId);
  if (redis !== null) return new Set(redis);
  const mem = memActiveSessions.get(userId);
  return mem ? new Set(mem) : new Set();
}

async function setActiveSessions(userId, sessionsSet) {
  const arr = [...sessionsSet];
  const ttl = 24 * 60 * 60; // 24h
  const stored = await redisSet("sess:" + userId, arr, ttl);
  if (!stored) memActiveSessions.set(userId, sessionsSet);
}

async function deleteActiveSessions(userId) {
  await redisDel("sess:" + userId);
  memActiveSessions.delete(userId);
}

// ── Cleanup: Periodic cleanup of in-memory stores ───────────────────────────
const MEMORY_SESSION_MAX = 10000;
setInterval(async () => {
  // Clean up memActiveSessions
  if (memActiveSessions.size > MEMORY_SESSION_MAX) {
    const keys = [...memActiveSessions.keys()];
    const evictCount = Math.floor(keys.length * 0.1);
    for (let i = 0; i < evictCount; i++) memActiveSessions.delete(keys[i]);
  }
}, 60 * 1000).unref();

// ── Configuration ────────────────────────────────────────────────────────────
const CONFIG = {
  // Rate Limiting
  RATE_LIMIT_WINDOW_MS: 15 * 60 * 1000,    // 15 minutes
  MAX_REQUESTS_PER_WINDOW: {
    public: parseInt(process.env.RATE_LIMIT_PUBLIC) || 30,
    authenticated: parseInt(process.env.RATE_LIMIT_AUTHENTICATED) || 60,
    admin: parseInt(process.env.RATE_LIMIT_ADMIN) || 100,
    onboard: parseInt(process.env.RATE_LIMIT_ONBOARD) || 5,
    submission: parseInt(process.env.RATE_LIMIT_SUBMISSION) || 10,
    auth: parseInt(process.env.RATE_LIMIT_AUTH) || 20,
    "otp-request": parseInt(process.env.RATE_LIMIT_OTP_REQUEST) || 3,
    "otp-verify": parseInt(process.env.RATE_LIMIT_OTP_VERIFY) || 5,
  },

  // Account Lockout
  MAX_FAILED_ATTEMPTS: 5,
  LOCKOUT_DURATION_MS: 15 * 60 * 1000,  // 15 minutes
  FAILED_ATTEMPT_WINDOW_MS: 15 * 60 * 1000,

  // IP Blocking
  MAX_IP_FAILURES: 20,
  IP_BLOCK_DURATION_MS: 60 * 60 * 1000,  // 1 hour

  // Request Limits
  MAX_BODY_SIZE: 1 * 1024 * 1024,  // 1MB
  MAX_URL_LENGTH: 2048,
  MAX_HEADER_SIZE: 8192,
  MAX_FIELD_LENGTH: 500,

  // Password Policy
  MIN_PASSWORD_LENGTH: 8,
  REQUIRE_UPPERCASE: true,
  REQUIRE_LOWERCASE: true,
  REQUIRE_NUMBER: true,

  // Session
  TOKEN_EXPIRY_HOURS: 24,
  MAX_SESSIONS_PER_USER: 3,
};

// ── Strict Rate Limiter ──────────────────────────────────────────────────────
function strictRateLimit(category = "authenticated") {
  return async (req, res, next) => {
    const key = `${req.ip}:${category}`;
    const now = Date.now();
    const windowMs = CONFIG.RATE_LIMIT_WINDOW_MS;
    const maxRequests = CONFIG.MAX_REQUESTS_PER_WINDOW[category] || 60;

    let record = await getRequestCount(key);
    if (!record || now - record.windowStart > windowMs) {
      record = { count: 0, windowStart: now };
    }

    record.count++;

    await setRequestCount(key, record);

    // Set rate limit headers
    res.setHeader("X-RateLimit-Limit", maxRequests);
    res.setHeader("X-RateLimit-Remaining", Math.max(0, maxRequests - record.count));
    res.setHeader("X-RateLimit-Reset", Math.ceil((record.windowStart + windowMs) / 1000));

    if (record.count > maxRequests) {
      logAudit("RATE_LIMIT_EXCEEDED", { ip: req.ip, path: req.path, category, count: record.count });
      return res.status(429).json({
        error: "طلبات كثيرة جداً",
        message: `Rate limit exceeded. Max ${maxRequests} requests per minute.`,
        retryAfter: Math.ceil((record.windowStart + windowMs - now) / 1000),
      });
    }

    next();
  };
}

// ── Account Lockout ──────────────────────────────────────────────────────────
async function recordFailedAttempt(identifier) {
  const now = Date.now();
  let record = await getFailedAttempt(identifier);

  if (!record || now - record.lastAttempt > CONFIG.FAILED_ATTEMPT_WINDOW_MS) {
    record = { count: 0, lastAttempt: now, blockedUntil: null, delayUntil: null };
  }

  record.count++;
  record.lastAttempt = now;

  // Progressive delay: 1s, 2s, 4s, 8s, 16s after each failure
  if (record.count >= 2 && record.count < CONFIG.MAX_FAILED_ATTEMPTS) {
    const delayMs = Math.min(1000 * Math.pow(2, record.count - 1), 16000);
    record.delayUntil = now + delayMs;
    logAudit("LOGIN_PROGRESSIVE_DELAY", { identifier, attempts: record.count, delayMs });
  }

  if (record.count >= CONFIG.MAX_FAILED_ATTEMPTS) {
    record.blockedUntil = now + CONFIG.LOCKOUT_DURATION_MS;
    logAudit("ACCOUNT_LOCKED", { identifier, attempts: record.count });
  }

  await setFailedAttempt(identifier, record);
  return record;
}

async function clearFailedAttempts(identifier) {
  await deleteFailedAttempt(identifier);
}

async function isAccountLocked(identifier) {
  const record = await getFailedAttempt(identifier);
  if (!record) return false;

  const now = Date.now();
  if (record.blockedUntil && now < record.blockedUntil) {
    return true;
  }

  // Reset if window expired
  if (now - record.lastAttempt > CONFIG.FAILED_ATTEMPT_WINDOW_MS) {
    await deleteFailedAttempt(identifier);
    return false;
  }

  return false;
}

// Check if request should be delayed due to progressive delay
async function isProgressivelyDelayed(identifier) {
  const record = await getFailedAttempt(identifier);
  if (!record) return { delayed: false, waitMs: 0 };

  const now = Date.now();
  if (record.delayUntil && now < record.delayUntil) {
    return { delayed: true, waitMs: Math.ceil((record.delayUntil - now) / 1000) };
  }

  return { delayed: false, waitMs: 0 };
}

// ── IP Blocking ──────────────────────────────────────────────────────────────
async function recordIPFailure(ip) {
  let count = await getIPBlock(ip);
  if (!count) {
    count = { failures: 0, blockedUntil: null };
  }

  count.failures++;
  count.lastFailure = Date.now();

  if (count.failures >= CONFIG.MAX_IP_FAILURES) {
    count.blockedUntil = Date.now() + CONFIG.IP_BLOCK_DURATION_MS;
    logAudit("IP_BLOCKED", { ip, failures: count.failures });
  }

  await setIPBlock(ip, count);
}

async function isIPBlocked(ip) {
  const record = await getIPBlock(ip);
  if (!record) return false;

  if (record.blockedUntil && Date.now() < record.blockedUntil) {
    return true;
  }

  if (record.blockedUntil && Date.now() >= record.blockedUntil) {
    await deleteIPBlock(ip);
    return false;
  }

  return false;
}

async function unblockIPAdmin(ip) {
  if (!ip) return false;
  const hadBlock = await isIPBlocked(ip);
  await deleteIPBlock(ip);
  logAudit("IP_UNBLOCKED", { ip });
  return hadBlock;
}

// ── Request Validation ───────────────────────────────────────────────────────
function strictRequestValidation(req, res, next) {
  // Check URL length
  if (req.originalUrl.length > CONFIG.MAX_URL_LENGTH) {
    return res.status(414).json({ error: "URI Too Long" });
  }

  // Check content-length
  const contentLength = parseInt(req.headers["content-length"] || "0");
  if (contentLength > CONFIG.MAX_BODY_SIZE) {
    return res.status(413).json({ error: "الحمولة كبيرة جداً", limit: "1MB" });
  }

  // Check for suspicious headers
  const suspiciousHeaders = ["x-forwarded-for", "x-real-ip", "x-cluster-client-ip"];
  for (const header of suspiciousHeaders) {
    const value = req.headers[header];
    if (value && value.length > 500) {
      logAudit("SUSPICIOUS_HEADER", { ip: req.ip, header, length: value.length });
      return res.status(400).json({ error: "Invalid Headers" });
    }
  }

  // Check User-Agent (only validate if present — API clients may omit it)
  const ua = req.headers["user-agent"];
  if (ua) {
    if (ua.length > 500) {
      return res.status(400).json({ error: "Invalid User-Agent" });
    }

    // Block known bad user agents
    const badAgents = ["sqlmap", "nikto", "nessus", "openvas", "burp", "dirbuster", "gobuster"];
    if (badAgents.some(bad => ua.toLowerCase().includes(bad))) {
      logAudit("BLOCKED_SCANNER", { ip: req.ip, ua });
      return res.status(403).json({ error: "محظور" });
    }
  }

  next();
}

// ── Input Sanitization (Strict) ──────────────────────────────────────────────
function strictSanitize(obj, depth = 0) {
  if (depth > 10) return "[MAX_DEPTH]";  // Prevent deep nesting attacks

  if (typeof obj === "string") {
    // Remove null bytes
    let s = obj.replace(/\0/g, "");
    // Remove control characters (except newlines and tabs)
    s = s.replace(/[\x01-\x08\x0B\x0C\x0E-\x1F\x7F]/g, "");
    // Limit length
    if (s.length > CONFIG.MAX_FIELD_LENGTH) {
      s = s.substring(0, CONFIG.MAX_FIELD_LENGTH);
    }
    return s;
  }

  if (Array.isArray(obj)) {
    if (obj.length > 1000) return obj.slice(0, 1000);  // Limit array size
    return obj.map(item => strictSanitize(item, depth + 1));
  }

  if (obj && typeof obj === "object") {
    const clean = {};
    let keys = Object.keys(obj);
    if (keys.length > 100) keys = keys.slice(0, 100);  // Limit object keys

    for (const key of keys) {
      const sanitizedKey = strictSanitize(key, depth + 1);
      clean[sanitizedKey] = strictSanitize(obj[key], depth + 1);
    }
    return clean;
  }

  return obj;
}

function strictSanitizeMiddleware(req, res, next) {
  if (req.body && typeof req.body === "object") {
    req.body = strictSanitize(req.body);
  }
  if (req.query && typeof req.query === "object") {
    req.query = strictSanitize(req.query);
  }
  if (req.params && typeof req.params === "object") {
    req.params = strictSanitize(req.params);
  }
  next();
}

// ── Password Strength Validator ──────────────────────────────────────────────
function validatePasswordStrength(password) {
  const errors = [];

  if (password.length < CONFIG.MIN_PASSWORD_LENGTH) {
    errors.push(`Password must be at least ${CONFIG.MIN_PASSWORD_LENGTH} characters`);
  }

  if (CONFIG.REQUIRE_UPPERCASE && !/[A-Z]/.test(password)) {
    errors.push("يجب أن تحتوي كلمة المرور على حرف كبير واحد على الأقل");
  }

  if (CONFIG.REQUIRE_LOWERCASE && !/[a-z]/.test(password)) {
    errors.push("يجب أن تحتوي كلمة المرور على حرف صغير واحد على الأقل");
  }

  if (CONFIG.REQUIRE_NUMBER && !/[0-9]/.test(password)) {
    errors.push("يجب أن تحتوي كلمة المرور على رقم واحد على الأقل");
  }

  // Check for common passwords
  const commonPasswords = ["password", "123456", "12345678", "qwerty", "admin", "letmein", "welcome"];
  if (commonPasswords.includes(password.toLowerCase())) {
    errors.push("كلمة المرور شائعة جداً");
  }

  return errors;
}

// ── Audit Logger ─────────────────────────────────────────────────────────────
const auditLog = [];
function logAudit(action, data = {}) {
  const entry = {
    timestamp: new Date().toISOString(),
    action,
    ip: data.ip || "unknown",
    userId: data.userId || null,
    details: data,
  };

  auditLog.push(entry);

  // Keep only last 10000 entries using circular buffer (O(1) instead of O(n) shift)
  if (auditLog.length > 10000) {
    auditLog.splice(0, auditLog.length - 10000);
  }
}

function getAuditLog(options = {}) {
  const { limit = 100, action, userId, ip } = options;
  let filtered = [...auditLog];

  if (action) filtered = filtered.filter(e => e.action === action);
  if (userId) filtered = filtered.filter(e => e.userId === userId);
  if (ip) filtered = filtered.filter(e => e.ip === ip);

  return filtered.slice(-limit);
}

// ── Security Headers (Strict) ────────────────────────────────────────────────
function strictSecurityHeaders(req, res, next) {
  // Prevent clickjacking
  res.setHeader("X-Frame-Options", "DENY");

  // Prevent MIME sniffing
  res.setHeader("X-Content-Type-Options", "nosniff");

  // XSS Protection
  res.setHeader("X-XSS-Protection", "1; mode=block");

  // Strict Transport Security (HSTS)
  res.setHeader("Strict-Transport-Security", "max-age=31536000; includeSubDomains; preload");

  // Content Security Policy (Strict)
  // Allow inline styles/scripts for frontend HTML pages
  const isApiRequest = req.path.startsWith('/api/');
  const scriptSrc = isApiRequest ? "'self'" : "'self' 'unsafe-inline' 'unsafe-eval'";
  const styleSrc = isApiRequest ? "'self'" : "'self' 'unsafe-inline' https://fonts.googleapis.com";

  res.setHeader("Content-Security-Policy",
    "default-src 'self'; " +
    `script-src ${scriptSrc}; ` +
    `style-src ${styleSrc}; ` +
    "img-src 'self' data: https:; " +
    "font-src 'self' https://fonts.gstatic.com; " +
    "connect-src 'self' ws: wss: https://fonts.googleapis.com https://fonts.gstatic.com; " +
    "frame-ancestors 'none'; " +
    "form-action 'self'; " +
    "base-uri 'self'; " +
    "object-src 'none'; " +
    "media-src 'none'; " +
    "child-src 'none'; " +
    "worker-src 'self';"
  );

  // Referrer Policy
  res.setHeader("Referrer-Policy", "strict-origin-when-cross-origin");

  // Permissions Policy
  res.setHeader("Permissions-Policy", "camera=(), microphone=(), geolocation=()");

  // Remove server identification
  res.removeHeader("X-Powered-By");

  next();
}

// ── Exports ──────────────────────────────────────────────────────────────────
module.exports = {
  CONFIG,
  strictRateLimit,
  recordFailedAttempt,
  clearFailedAttempts,
  isAccountLocked,
  isProgressivelyDelayed,
  recordIPFailure,
  isIPBlocked,
  unblockIPAdmin,
  strictRequestValidation,
  strictSanitize,
  strictSanitizeMiddleware,
  validatePasswordStrength,
  logAudit,
  getAuditLog,
  strictSecurityHeaders,
};
