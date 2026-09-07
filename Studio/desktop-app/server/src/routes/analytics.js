const express = require("express");
const router = express.Router();
const {
  getLeaderboard,
  getAttendanceSummary,
  getClassProgress,
  getTestCases,
  getChallengeTestCases,
  getChallenges,
  getTeacherDashboard,
  getTeacherClassrooms,
  createClassroom,
  getTeacherAssessments,
  markAttendance,
  getAttendanceHistory
} = require("../controllers/analyticsController");
const { authenticate, requireTeacherOrAdmin, requireAdmin } = require("../middlewares/auth");
const { writeRateLimit } = require("../middlewares/security");

// All analytics routes require authentication
router.use(authenticate);

// ── Analytics Dashboard ──────────────────────────────────────────────────────
router.get("/leaderboard", requireTeacherOrAdmin, getLeaderboard);
router.get("/attendance-summary", requireTeacherOrAdmin, getAttendanceSummary);
router.get("/attendance-history", requireTeacherOrAdmin, getAttendanceHistory);
router.post("/attendance", requireTeacherOrAdmin, writeRateLimit, markAttendance);
router.get("/progress", requireTeacherOrAdmin, getClassProgress);

// ── Teacher Dashboard & Classrooms ───────────────────────────────────────────
router.get("/dashboard", requireTeacherOrAdmin, getTeacherDashboard);
router.get("/classrooms", requireTeacherOrAdmin, getTeacherClassrooms);
router.post("/classrooms", requireTeacherOrAdmin, writeRateLimit, createClassroom);
router.get("/assessments", requireTeacherOrAdmin, getTeacherAssessments);

// ── Content Delivery ────────────────────────────────────────────────────────────
router.get("/challenges", getChallenges);
router.get("/lessons/:lessonId/tests", requireTeacherOrAdmin, getTestCases);
router.get("/challenges/tier/:tierId", requireTeacherOrAdmin, getChallengeTestCases);

module.exports = router;
