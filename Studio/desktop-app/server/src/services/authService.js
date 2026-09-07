const bcrypt = require("bcryptjs");
const jwt = require("jsonwebtoken");
const crypto = require("crypto");

const BCRYPT_ROUNDS = 12;

// ── Refresh Token Blacklist (Redis-first, in-memory fallback) ──────────────
const refreshTokenBlacklist = new Map();

function getRedis() {
  try {
    const { getRedisClient, isRedisAvailable } = require("../utils/redis");
    if (isRedisAvailable()) return getRedisClient();
  } catch (_) {}
  return null;
}

function hashToken(token) {
  // SECURITY: Use full SHA-256 hash (64 hex chars) for collision resistance
  return crypto.createHash("sha256").update(token).digest("hex");
}

// ── 1. validatePassword ───────────────────────────────────────────────────────
function validatePassword(password) {
  const errors = [];

  if (password.length < 8) {
    errors.push("Password must be at least 8 characters");
  }

  if (!/[A-Z]/.test(password)) {
    errors.push("يجب أن تحتوي كلمة المرور على حرف كبير واحد على الأقل");
  }

  if (!/[a-z]/.test(password)) {
    errors.push("يجب أن تحتوي كلمة المرور على حرف صغير واحد على الأقل");
  }

  if (!/[0-9]/.test(password)) {
    errors.push("يجب أن تحتوي كلمة المرور على رقم واحد على الأقل");
  }

  return errors;
}

// ── 2. hashPassword ───────────────────────────────────────────────────────────
async function hashPassword(password) {
  return bcrypt.hash(password, BCRYPT_ROUNDS);
}

// ── 3. comparePassword ────────────────────────────────────────────────────────
async function comparePassword(password, hash) {
  return bcrypt.compare(password, hash);
}

// ── 4. generateTokens ─────────────────────────────────────────────────────────
function generateTokens(user) {
  const tokenPayload = {
    id: user.id,
    schoolId: user.schoolId,
    classroomId: user.classroomId || null,
    role: user.role,
    name: user.name,
    tokenVersion: user.tokenVersion || 0,
  };

  const accessToken = jwt.sign(tokenPayload, process.env.JWT_SECRET, {
    algorithm: "HS256",
    expiresIn: process.env.JWT_ACCESS_EXPIRES || "1h",
  });

  const refreshToken = jwt.sign(
    { id: user.id, type: "refresh", tokenVersion: user.tokenVersion || 0 },
    process.env.JWT_REFRESH_SECRET || process.env.JWT_SECRET,
    {
      algorithm: "HS256",
      expiresIn: process.env.JWT_REFRESH_EXPIRES || "7d",
    }
  );

  return { accessToken, refreshToken };
}

// ── 5. verifyAccessToken ──────────────────────────────────────────────────────
function verifyAccessToken(token) {
  return jwt.verify(token, process.env.JWT_SECRET, {
    algorithms: ["HS256"],
  });
}

// ── 6. verifyRefreshToken ─────────────────────────────────────────────────────
function verifyRefreshToken(token) {
  return jwt.verify(token, process.env.JWT_REFRESH_SECRET || process.env.JWT_SECRET, {
    algorithms: ["HS256"],
  });
}

// ── 7. isTokenBlacklisted ─────────────────────────────────────────────────────
async function isTokenBlacklisted(tokenHash) {
  const redis = getRedis();
  if (redis) {
    try {
      const exists = await redis.exists(`blacklist:${tokenHash}`);
      return exists === 1;
    } catch (_) {}
  }
  // Fallback to in-memory
  const expiry = refreshTokenBlacklist.get(tokenHash);
  if (!expiry) return false;
  if (Date.now() > expiry) {
    refreshTokenBlacklist.delete(tokenHash);
    return false;
  }
  return true;
}

// ── 8. blacklistToken ─────────────────────────────────────────────────────────
async function blacklistToken(tokenHash) {
  const EXPIRY_SECONDS = 7 * 24 * 60 * 60; // 7 days
  const redis = getRedis();
  if (redis) {
    try {
      await redis.setex(`blacklist:${tokenHash}`, EXPIRY_SECONDS, '1');
    } catch (_) {}
  }
  // Also store in-memory as fallback
  const expiry = Date.now() + EXPIRY_SECONDS * 1000;
  refreshTokenBlacklist.set(tokenHash, expiry);

  // Cleanup old entries periodically
  if (refreshTokenBlacklist.size > 10000) {
    const now = Date.now();
    for (const [key, exp] of refreshTokenBlacklist) {
      if (exp < now) refreshTokenBlacklist.delete(key);
    }
  }
}

// ── 9. validateEmail ──────────────────────────────────────────────────────────
function validateEmail(email) {
  if (!email || typeof email !== "string") return false;
  if (email.length > 254) return false;
  if (email.includes("\n") || email.includes("\r") || email.includes("\0")) return false;
  const re = /^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$/;
  return re.test(email);
}

// ── 10. sanitizeInput ─────────────────────────────────────────────────────────
function sanitizeInput(input) {
  if (typeof input !== "string") return input;
  return input
    .replace(/\0/g, "")
    .replace(/[\x00-\x08\x0B\x0C\x0E-\x1F]/g, "")
    .replace(/\r/g, "")
    .trim();
}

module.exports = {
  validatePassword,
  hashPassword,
  comparePassword,
  generateTokens,
  verifyAccessToken,
  verifyRefreshToken,
  isTokenBlacklisted,
  blacklistToken,
  validateEmail,
  sanitizeInput,
  hashToken,
};
