/* Developer: محمد محمود الحموز | Dhad Studio */
// ====================================================
// Dhad Studio - API Security Middleware
// ====================================================
// Request signing, replay protection, and data encryption
// ====================================================

const crypto = require("crypto");
const { createHMAC, verifyHMAC, generateToken } = require("../utils/encryption");

// ── Configuration ────────────────────────────────────────────────────────────
const CONFIG = {
  TIMESTAMP_TOLERANCE_MS: 5 * 60 * 1000,
  MAX_NONCES: 10000,
  NONCE_EXPIRY_MS: 10 * 60 * 1000,
  SENSITIVE_FIELDS: ["passwordHash", "password", "nationalId", "email"],
  MASKED_FIELDS: ["password", "passwordHash", "nationalId", "email", "token"],
};

// ── Nonce Store ──────────────────────────────────────────────────────────────
const nonces = new Map();

// ── Prevent Replay ──────────────────────────────────────────────────────────
function preventReplay(req, res, next) {
  if (!req.path.startsWith("/api/")) return next();

  const timestamp = req.headers["x-request-timestamp"];
  const nonce = req.headers["x-request-nonce"];

  if (!timestamp || !nonce) {
    return res.status(400).json({
      error: "خطأ في الطلب",
      message: "Timestamp and nonce required",
    });
  }

  const requestTime = parseInt(timestamp);
  if (isNaN(requestTime) || Math.abs(Date.now() - requestTime) > CONFIG.TIMESTAMP_TOLERANCE_MS) {
    return res.status(401).json({
      error: "غير مصرح",
      message: "Request timestamp expired or invalid",
    });
  }

  if (nonces.has(nonce)) {
    return res.status(401).json({
      error: "غير مصرح",
      message: "Replay detected",
    });
  }

  nonces.set(nonce, Date.now());
  next();
}

// ── Strict Replay Protection ────────────────────────────────────────────────
function strictReplayProtection(req, res, next) {
  if (!req.path.startsWith("/api/")) return next();

  const timestamp = req.headers["x-request-timestamp"];
  const nonce = req.headers["x-request-nonce"];

  if (!timestamp || !nonce) {
    return res.status(400).json({
      error: "خطأ في الطلب",
      message: "Timestamp and nonce required for this operation",
    });
  }

  const requestTime = parseInt(timestamp);
  if (isNaN(requestTime) || Math.abs(Date.now() - requestTime) > CONFIG.TIMESTAMP_TOLERANCE_MS) {
    return res.status(401).json({
      error: "غير مصرح",
      message: "Request timestamp expired or invalid",
    });
  }

  if (nonces.has(nonce)) {
    return res.status(401).json({
      error: "غير مصرح",
      message: "Replay detected",
    });
  }

  nonces.set(nonce, Date.now());
  next();
}

// ── Data Protection Middleware ───────────────────────────────────────────────
function sanitizeObject(obj, sensitiveFields = CONFIG.SENSITIVE_FIELDS) {
  if (!obj || typeof obj !== "object") return obj;

  if (Array.isArray(obj)) {
    return obj.map((item) => sanitizeObject(item, sensitiveFields));
  }

  const sanitized = { ...obj };
  for (const field of sensitiveFields) {
    if (sanitized[field]) {
      sanitized[field] = "[REDACTED]";
    }
  }
  return sanitized;
}

function sanitizeResponse(sensitiveFields = CONFIG.SENSITIVE_FIELDS) {
  return (req, res, next) => {
    const originalJson = res.json.bind(res);

    res.json = function (data) {
      if (data && typeof data === "object") {
        data = sanitizeObject(data, sensitiveFields);
      }
      return originalJson(data);
    };

    next();
  };
}

// ── CSRF Protection ──────────────────────────────────────────────────────────
function csrfProtection(req, res, next) {
  if (req.headers.authorization) {
    return next();
  }

  if (["GET", "HEAD", "OPTIONS"].includes(req.method)) {
    return next();
  }

  if (req.path.includes("/auth/login") || req.path.includes("/auth/register")) {
    return next();
  }

  const token = req.headers["x-csrf-token"] || req.body?._csrf;
  const cookieToken = req.cookies?.csrf_token || req.cookies?._csrf_dhad;

  if (!token || !cookieToken) {
    return res.status(403).json({
      error: "CSRF",
      message: "CSRF token missing",
    });
  }

  const tokenBuf = Buffer.from(token, "utf8");
  const cookieBuf = Buffer.from(cookieToken, "utf8");

  if (tokenBuf.length !== cookieBuf.length || !crypto.timingSafeEqual(tokenBuf, cookieBuf)) {
    return res.status(403).json({
      error: "CSRF",
      message: "CSRF token mismatch",
    });
  }

  next();
}

// ── HMAC Verification Middleware ────────────────────────────────────────────
function verifyHMACMiddleware(req, res, next) {
  const secret = process.env.HMAC_SECRET;
  if (!secret) {
    return res.status(500).json({ error: "HMAC secret not configured" });
  }

  const signature = req.headers["x-hmac-signature"];
  const timestamp = req.headers["x-request-timestamp"];

  if (!signature || !timestamp) {
    return res.status(400).json({ error: "Missing HMAC signature" });
  }

  const data = `${req.method}:${req.path}:${timestamp}:${JSON.stringify(req.body || {})}`;
  const expected = createHMAC(data, secret);
  const sigBuf = Buffer.from(signature, "utf8");
  const expectedBuf = Buffer.from(expected, "utf8");

  if (sigBuf.length !== expectedBuf.length || !crypto.timingSafeEqual(sigBuf, expectedBuf)) {
    return res.status(401).json({ error: "Invalid HMAC signature" });
  }

  next();
}

// ── Request Signature Verification (for Bridge) ────────────────────────────
function verifyRequestSignature(req, res, next) {
  const signature = req.headers["x-request-signature"];
  const timestamp = req.headers["x-request-timestamp"];

  if (!signature || !timestamp) {
    return res.status(401).json({ error: "توقيع الطلب مطلوب" });
  }

  const secret = process.env.HMAC_SECRET;
  if (!secret) {
    return res.status(500).json({ error: "HMAC secret not configured" });
  }

  const expected = createHMAC(JSON.stringify(req.body) + timestamp, secret);
  const sigBuf = Buffer.from(signature, "hex");
  const expectedBuf = Buffer.from(expected, "hex");

  if (sigBuf.length !== expectedBuf.length || !crypto.timingSafeEqual(sigBuf, expectedBuf)) {
    return res.status(401).json({ error: "توقيع الطلب غير صالح" });
  }

  next();
}

// ── Exports ──────────────────────────────────────────────────────────────────
module.exports = {
  CONFIG,
  preventReplay,
  strictReplayProtection,
  sanitizeResponse,
  sanitizeObject,
  csrfProtection,
  verifyRequestSignature,
  verifyHMACMiddleware,
  createHMAC,
  generateToken,
};
