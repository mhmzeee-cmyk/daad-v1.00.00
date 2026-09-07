const express = require("express");
const router = express.Router();
const {
  getSchools,
  getSchoolById,
  updateSchool,
  deleteSchool,
  listSchoolUsers,
  deleteUser,
} = require("../controllers/schoolController");
const {
  authenticate,
  requireAdmin,
  requireSchoolAccess,
} = require("../middlewares/auth");
const { writeRateLimit, destructiveRateLimit } = require("../middlewares/security");

// ── Read Routes (Authenticated users can view their own school) ──────────────
// GET /api/v1/schools — list all schools (admin) or own school (teacher/student)
router.get("/schools", authenticate, getSchools);

// GET /api/v1/schools/:id — get school details
router.get("/schools/:id", authenticate, requireSchoolAccess, getSchoolById);

// GET /api/v1/schools/:id/users — list users in a school (ADMIN ONLY)
router.get("/schools/:id/users", authenticate, requireAdmin, listSchoolUsers);

// ── Write Routes (ADMIN ONLY) ────────────────────────────────────────────────
// PUT /api/v1/schools/:id — update school name
router.put("/schools/:id", authenticate, requireAdmin, writeRateLimit, updateSchool);

// DELETE /api/v1/schools/:id — delete school and all its users
router.delete("/schools/:id", authenticate, requireAdmin, destructiveRateLimit, deleteSchool);

// DELETE /api/v1/schools/:id/users/:userId — delete a specific user
router.delete(
  "/schools/:id/users/:userId",
  authenticate,
  requireAdmin,
  destructiveRateLimit,
  deleteUser
);

module.exports = router;
