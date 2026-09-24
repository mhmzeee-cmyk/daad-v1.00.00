const express = require("express");
const { authenticate, requireRole } = require("../middlewares/auth");
const { trainingSubmissionRateLimit, readRateLimit, writeRateLimit } = require("../middlewares/security");
const { isAccountLocked } = require("../middlewares/strictSecurity");
const {
  listChallenges,
  dailyChallenge,
  createChallenge,
  updateChallenge,
  deleteChallenge,
  publishChallenge,
  setDaily,
  submitSolution,
  listSubmissions,
  validateChallengeSecurity,
  verifyChallengeCompletion,
} = require("../controllers/challengeController");

const router = express.Router();

// ── Challenge Verify Rate Limit ─────────────────────────────────────────────
const challengeVerifyRateLimit = require("express-rate-limit")({
  windowMs: 60 * 1000,
  max: 60,
  standardHeaders: true,
  legacyHeaders: false,
  message: { error: "RATE_LIMIT_EXCEEDED", message: "تم تجاوز حد طلبات التحقق من التحديات (60/دقيقة)." },
  keyGenerator: (req) => req.user?.id || req.ip,
});

async function checkAccountLocked(req, res, next) {
  const identifier = req.user?.id || req.ip;
  if (await isAccountLocked(identifier)) {
    return res.status(423).json({ error: "الحساب مقفل", message: "حساب الطالب مقفل بسبب محاولات فاشلة متعددة." });
  }
  next();
}

// ── Challenges ───────────────────────────────────────────────────────────────
router.get("/challenges", authenticate, readRateLimit, listChallenges);
router.get("/challenges/daily", authenticate, readRateLimit, dailyChallenge);
router.post("/challenges", authenticate, requireRole("TEACHER", "ADMIN"), writeRateLimit, createChallenge);
router.put("/challenges/:id", authenticate, requireRole("TEACHER", "ADMIN"), writeRateLimit, updateChallenge);
router.delete("/challenges/:id", authenticate, requireRole("TEACHER", "ADMIN"), writeRateLimit, deleteChallenge);
router.post("/challenges/:id/publish", authenticate, requireRole("TEACHER", "ADMIN"), writeRateLimit, publishChallenge);
router.post("/challenges/:id/set-daily", authenticate, requireRole("TEACHER", "ADMIN"), writeRateLimit, setDaily);

// ── Submissions ──────────────────────────────────────────────────────────────
// DEPRECATED: Old /submit-solution endpoint — was vulnerable to client-trusted output
// Students must use /api/v1/student/challenge/submit (sandboxed server-side evaluation)
router.post("/submit-solution", authenticate, requireRole("STUDENT"), (req, res) => {
  res.status(410).json({
    error: "تم إلغاء هذا المendpoints",
    message: "استخدم POST /api/v1/student/challenge/submit بدلاً من ذلك",
  });
});
router.get("/submissions", authenticate, readRateLimit, listSubmissions);

// ── Challenge Verification (HMAC - Secure) ──────────────────────────────────
router.post("/challenge/verify",
  authenticate,
  challengeVerifyRateLimit,
  checkAccountLocked,
  validateChallengeSecurity,
  verifyChallengeCompletion
);

module.exports = router;
