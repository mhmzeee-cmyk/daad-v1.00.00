const express = require("express");
const router = express.Router();
const {
  importStudentsHandler,
  getInvitationsHandler,
} = require("../controllers/otpController");
const { authenticate, requireTeacherOrAdmin } = require("../middlewares/auth");
const { writeRateLimit } = require("../middlewares/security");

// POST /api/v1/teacher/invitations/import
// Import students (creates invitations with OTPs)
router.post(
  "/teacher/invitations/import",
  authenticate,
  requireTeacherOrAdmin,
  writeRateLimit,
  importStudentsHandler
);

// GET /api/v1/teacher/invitations
// View all invitations for the school
router.get(
  "/teacher/invitations",
  authenticate,
  requireTeacherOrAdmin,
  getInvitationsHandler
);

module.exports = router;
