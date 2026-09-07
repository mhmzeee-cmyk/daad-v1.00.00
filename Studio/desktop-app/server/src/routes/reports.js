const express = require("express");
const { authenticate, requireRole } = require("../middlewares/auth");
const { writeRateLimit } = require("../middlewares/security");
const { weeklyReport, userActivity, schoolOverview } = require("../controllers/reportsController");

const router = express.Router();

// ── Reports ─────────────────────────────────────────────────────────────────
// GET /api/v1/reports/school-overview — school overview stats
router.get("/reports/school-overview", authenticate, requireRole("TEACHER", "ADMIN"), schoolOverview);
router.get("/reports/school-overview/:schoolId", authenticate, requireRole("ADMIN"), schoolOverview);

// POST /api/v1/reports/weekly — weekly activity report
router.post("/reports/weekly", authenticate, requireRole("TEACHER", "ADMIN"), writeRateLimit, weeklyReport);
router.post("/reports/weekly/:schoolId", authenticate, requireRole("ADMIN"), writeRateLimit, weeklyReport);

// GET /api/v1/reports/activity/:userId — user activity details
router.get("/reports/activity/:userId", authenticate, requireRole("TEACHER", "ADMIN"), userActivity);

module.exports = router;
