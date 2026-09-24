const express = require("express");
const router = express.Router();
const {
  createStudent,
  bulkCreateStudents,
  getPublicClassrooms,
  getClassroomStudents,
  studentLogin,
  getTeacherStudents,
  deleteStudent,
  assignStudentToClassroom,
  getStudentActivity,
  exportStudentsReport,
} = require("../controllers/studentManagementController");
const { authenticate, requireTeacherOrAdmin, requireAdmin } = require("../middlewares/auth");
const { writeRateLimit, destructiveRateLimit } = require("../middlewares/security");
const { strictRateLimit, isAccountLocked, isProgressivelyDelayed, recordFailedAttempt } = require("../middlewares/strictSecurity");

// ═══════════════════════════════════════════════════════════════════════════
// PUBLIC ROUTES (No Authentication Required)
// ═══════════════════════════════════════════════════════════════════════════

// GET /api/v1/auth/classrooms - Get all classrooms (for student login selection)
router.get("/auth/classrooms", getPublicClassrooms);

// GET /api/v1/auth/classrooms/:classroom_id/students - Get students in a classroom
router.get("/auth/classrooms/:classroom_id/students", getClassroomStudents);

// POST /api/v1/auth/student-login - Student login with rate limiting + lockout
router.post("/auth/student-login",
  strictRateLimit("public"),
  async (req, res, next) => {
    const { studentId } = req.body || {};

    // Check account lockout
    if (studentId && await isAccountLocked(studentId)) {
      return res.status(423).json({
        error: "الحساب مقفل",
        message: "الحساب مقفل مؤقتاً بسبب محاولات كثيرة. حاول مرة أخرى بعد 15 دقيقة",
      });
    }

    // SECURITY: Return delay info to client instead of blocking event loop
    if (studentId) {
      const delay = await isProgressivelyDelayed(studentId);
      if (delay.delayed) {
        return res.status(429).json({
          error: "طلبات كثيرة جداً",
          message: `يرجى الانتظار ${Math.ceil(delay.waitMs / 1000)} ثانية قبل المحاولة التالية`,
          retryAfter: Math.ceil(delay.waitMs / 1000),
        });
      }
    }

    next();
  },
  studentLogin
);

// ═══════════════════════════════════════════════════════════════════════════
// TEACHER ROUTES (Authentication Required)
// ═══════════════════════════════════════════════════════════════════════════

// POST /api/v1/teacher/students - Create a single student (Admin only)
router.post("/teacher/students", authenticate, requireAdmin, writeRateLimit, createStudent);

// POST /api/v1/teacher/students/bulk - Create multiple students (Admin only)
router.post("/teacher/students/bulk", authenticate, requireAdmin, writeRateLimit, bulkCreateStudents);

// GET /api/v1/teacher/students - List all students
router.get("/teacher/students", authenticate, requireTeacherOrAdmin, getTeacherStudents);

// DELETE /api/v1/teacher/students/:id - Delete a student (Admin only)
router.delete("/teacher/students/:id", authenticate, requireAdmin, destructiveRateLimit, deleteStudent);

// POST /api/v1/teacher/students/assign - Assign student to classroom (Admin only)
router.post("/teacher/students/assign", authenticate, requireAdmin, writeRateLimit, assignStudentToClassroom);

// GET /api/v1/teacher/students/report/export - Export students report as JSON (fields query param)
router.get("/teacher/students/report/export", authenticate, requireTeacherOrAdmin, exportStudentsReport);

// GET /api/v1/teacher/students/:id/activity - Student activity details (must be after report/export)
router.get("/teacher/students/:id/activity", authenticate, requireTeacherOrAdmin, getStudentActivity);

module.exports = router;
