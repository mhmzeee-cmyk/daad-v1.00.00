const rateLimit = require("express-rate-limit");

// ── Input Sanitizer ─────────────────────────────────────────────────────────
// Strips XSS, null bytes, control characters, trims whitespace
function sanitizeString(str) {
  if (typeof str !== "string") return str;
  return str
    .replace(/\0/g, "")                           // null bytes
    .replace(/[\x00-\x08\x0B\x0C\x0E-\x1F]/g, "") // control chars except \n\t
    .replace(/\r/g, "")                             // carriage return
    .trim();
}

// ── Deep Sanitize Middleware ─────────────────────────────────────────────────
// Sanitizes all string values in req.body recursively with depth limit
function deepSanitize(obj, depth = 0, maxDepth = 10, seen = new WeakSet()) {
  if (depth > maxDepth) return "[Object too deep]";
  if (typeof obj === "string") return sanitizeString(obj);
  if (Array.isArray(obj)) {
    if (obj.length > 1000) return obj.slice(0, 1000).map(v => deepSanitize(v, depth + 1, maxDepth, seen));
    return obj.map(v => deepSanitize(v, depth + 1, maxDepth, seen));
  }
  if (obj && typeof obj === "object") {
    if (seen.has(obj)) return "[Circular]";
    seen.add(obj);
    const clean = {};
    const keys = Object.keys(obj);
    if (keys.length > 1000) return "[Object too large]";
    for (const key of keys) {
      clean[key] = deepSanitize(obj[key], depth + 1, maxDepth, seen);
    }
    return clean;
  }
  return obj;
}

function sanitizeMiddleware(req, res, next) {
  if (req.body && typeof req.body === "object") {
    req.body = deepSanitize(req.body);
  }
  if (req.query && typeof req.query === "object") {
    req.query = deepSanitize(req.query);
  }
  if (req.params && typeof req.params === "object") {
    req.params = deepSanitize(req.params);
  }
  next();
}

// ── Email Validator ─────────────────────────────────────────────────────────
function isValidEmail(email) {
  if (!email || typeof email !== "string") return false;
  // RFC 5322 simplified — blocks injection, spaces, control chars
  const re = /^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$/;
  if (email.length > 254) return false;
  if (email.includes("\n") || email.includes("\r") || email.includes("\0")) return false;
  return re.test(email);
}

// ── Submission Rate Limiters ─────────────────────────────────────────────────
// Training submissions: 10/hour per student (prevents brute-forcing solutions)
const trainingSubmissionRateLimit = rateLimit({
  windowMs: 60 * 60 * 1000, // 1 hour
  max: 10,
  standardHeaders: true,
  legacyHeaders: false,
  message: {
    error: "طلبات كثيرة جداً",
    message: "تم تجاوز حد محاولات التدريب (10/ساعة). حاول مرة أخرى لاحقاً.",
  },
  keyGenerator: (req) => req.user?.id || req.ip,
});

// Exam submissions: 50/hour per student per exam (allows active exam work)
const examSubmissionRateLimit = rateLimit({
  windowMs: 60 * 60 * 1000, // 1 hour
  max: 50,
  standardHeaders: true,
  legacyHeaders: false,
  message: {
    error: "طلبات كثيرة جداً",
    message: "تم تجاوز حد إرسالات الامتحان (50/ساعة).",
  },
  keyGenerator: (req) => {
    const examId = req.body?.assessmentId || req.params?.assessmentId || 'unknown';
    return `${req.user?.id || req.ip}:${examId}`;
  },
});

// Exam auto-save: 120/hour per student per exam (frequent saves during exam)
const examAutoSaveRateLimit = rateLimit({
  windowMs: 60 * 60 * 1000, // 1 hour
  max: 120,
  standardHeaders: true,
  legacyHeaders: false,
  message: {
    error: "طلبات كثيرة جداً",
    message: "تم تجاوز حد حفظ الامتحان التلقائي (120/ساعة).",
  },
  keyGenerator: (req) => {
    const examId = req.body?.assessmentId || req.params?.assessmentId || 'unknown';
    return `${req.user?.id || req.ip}:${examId}`;
  },
});

// Legacy alias for backward compatibility
const submissionRateLimit = trainingSubmissionRateLimit;

// ── Read Rate Limiter (anti-scraping) ────────────────────────────────────────
// Prevents bulk scraping of challenge/submission data: 60/min per user
const readRateLimit = rateLimit({
  windowMs: 60 * 1000, // 1 minute
  max: 60,
  standardHeaders: true,
  legacyHeaders: false,
  message: {
    error: "طلبات كثيرة جداً",
    message: "تم تجاوز حد الطلبات. حاول مرة أخرى لاحقاً.",
  },
  keyGenerator: (req) => req.user?.id || req.ip,
});

// ── Write Rate Limiter (teacher/admin mutations) ─────────────────────────────
// Prevents abuse of create/update operations: 30/min per user
const writeRateLimit = rateLimit({
  windowMs: 60 * 1000, // 1 minute
  max: 30,
  standardHeaders: true,
  legacyHeaders: false,
  message: {
    error: "طلبات كثيرة جداً",
    message: "تم تجاوز حد عمليات الكتابة (30/دقيقة). حاول مرة أخرى لاحقاً.",
  },
  keyGenerator: (req) => req.user?.id || req.ip,
});

// ── Destructive Rate Limiter (DELETE operations) ─────────────────────────────
// Prevents abuse of delete/hard-reset operations: 5/min per user
const destructiveRateLimit = rateLimit({
  windowMs: 60 * 1000, // 1 minute
  max: 5,
  standardHeaders: true,
  legacyHeaders: false,
  message: {
    error: "طلبات كثيرة جداً",
    message: "تم تجاوز حد العمليات التدميرية (5/دقيقة). حاول مرة أخرى لاحقاً.",
  },
  keyGenerator: (req) => req.user?.id || req.ip,
});

module.exports = {
  sanitizeMiddleware,
  sanitizeString,
  isValidEmail,
  submissionRateLimit,
  trainingSubmissionRateLimit,
  examSubmissionRateLimit,
  examAutoSaveRateLimit,
  readRateLimit,
  writeRateLimit,
  destructiveRateLimit,
};
