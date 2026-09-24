const express = require("express");
const multer = require("multer");
const router = express.Router();
const { onboardSchool, onboardSchoolFiles } = require("../controllers/onboardController");
const { authenticate, requireAdmin } = require("../middlewares/auth");
const { strictRateLimit } = require("../middlewares/strictSecurity");

// Multer for file uploads (max 5MB per file, 2 files max)
const upload = multer({
  storage: multer.memoryStorage(),
  limits: { fileSize: 5 * 1024 * 1024, files: 2 },
  fileFilter: (req, file, cb) => {
    const allowed = [
      "text/csv",
      "application/vnd.ms-excel",
      "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
    ];
    if (allowed.includes(file.mimetype) || file.originalname.match(/\.(csv|xlsx?)$/i)) {
      cb(null, true);
    } else {
      cb(new Error("Only CSV and Excel files are allowed"));
    }
  },
});

// POST /api/v1/onboard-school — JSON body (Qt Bridge v1)
router.post("/onboard-school", strictRateLimit("onboard"), authenticate, requireAdmin, onboardSchool);

// POST /api/v1/onboard-school/files — multipart file upload (Bridge v2 with Excel support)
router.post(
  "/onboard-school/files",
  strictRateLimit("onboard"),
  authenticate,
  requireAdmin,
  upload.array("files", 2),
  onboardSchoolFiles
);

module.exports = router;
