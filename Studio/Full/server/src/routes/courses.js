const express = require("express");
const router = express.Router();
const { getCourses, enrollCourse, getCourseRoadmap } = require("../controllers/courseController");
const { authenticate, requireStudent } = require("../middlewares/auth");
const { writeRateLimit } = require("../middlewares/security");

router.get("/student/courses", authenticate, requireStudent, getCourses);
router.post("/student/courses/:courseId/enroll", authenticate, requireStudent, writeRateLimit, enrollCourse);
router.get("/student/courses/:courseId/roadmap", authenticate, requireStudent, getCourseRoadmap);

module.exports = router;
