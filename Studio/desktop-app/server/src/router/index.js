const express = require("express");
const router = express.Router();
const rateLimit = require("express-rate-limit");

// Import route modules
const authRoutes = require("../routes/auth");
const otpRoutes = require("../routes/otp");
const invitationsRoutes = require("../routes/invitations");
const analyticsRoutes = require("../routes/analytics");
const teacherRoutes = require("../routes/teacher");
const studentRoutes = require("../routes/student");
const studentManagementRoutes = require("../routes/studentManagement");
const onboardRoutes = require("../routes/onboard");
const securityRoutes = require("../routes/security");
const challengeRoutes = require("../routes/challenges");
const courseRoutes = require("../routes/courses");
const schoolRoutes = require("../routes/schools");
const reportsRoutes = require("../routes/reports");

// Import security middleware
const { sanitizeMiddleware } = require("../middlewares/security");

// Apply global security middleware
router.use(sanitizeMiddleware);

// ── Route-Level Rate Limiters ────────────────────────────────────────────────
// Analytics export: 60 req/min per user
const analyticsRateLimit = rateLimit({
  windowMs: 60 * 1000,
  max: 60,
  standardHeaders: true,
  legacyHeaders: false,
  message: { error: "RATE_LIMIT_EXCEEDED", message: "تم تجاوز حد الطلبات للتحليلات (60/دقيقة)." },
  keyGenerator: (req) => req.user?.id || req.ip,
});

// Student API: 30 req/min per user (dashboard, submissions)
const studentRateLimit = rateLimit({
  windowMs: 60 * 1000,
  max: 30,
  standardHeaders: true,
  legacyHeaders: false,
  message: { error: "RATE_LIMIT_EXCEEDED", message: "تم تجاوز حد الطلبات للطلاب (30/دقيقة)." },
  keyGenerator: (req) => req.user?.id || req.ip,
});

// Mount routes (order matters - public routes first)
router.use("/auth", otpRoutes);    // OTP routes (public: request-otp, verify-otp)
router.use("/auth", authRoutes);   // Auth routes (login, register)
router.use("/", studentManagementRoutes); // Student management (has public classroom routes)
router.use("/", invitationsRoutes); // Teacher invitations (authenticated)
router.use("/analytics", analyticsRateLimit, analyticsRoutes);
router.use("/", teacherRoutes);     // Teacher routes (assessments, security alerts)
router.use("/", studentRateLimit, studentRoutes);    // Student routes with 30/min global limit
router.use("/", onboardRoutes);    // School onboard (admin only)
router.use("/", securityRoutes);   // Security dashboard (admin only)
router.use("/", challengeRoutes);  // HMAC challenge verification
router.use("/", courseRoutes);     // Additional courses
router.use("/", schoolRoutes);     // School management
router.use("/", reportsRoutes);    // Reports and analytics

// 404 catch-all for unmatched /api/v1/* routes
router.use((req, res) => {
  res.status(404).json({
    error: 'NOT_FOUND',
    message: 'Route not found',
  });
});

module.exports = router;
