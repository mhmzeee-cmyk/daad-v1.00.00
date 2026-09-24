const express = require("express");
const router = express.Router();
const {
  requestOtp,
  verifyOtp,
} = require("../controllers/otpController");
const { strictRateLimit } = require("../middlewares/strictSecurity");

// POST /api/v1/auth/request-otp
// Student requests OTP for account activation (strict rate limit: 3/min)
router.post("/request-otp", strictRateLimit("otp-request"), requestOtp);

// POST /api/v1/auth/verify-otp
// Student verifies OTP and activates account (strict rate limit: 5/min — brute-force protection)
router.post("/verify-otp", strictRateLimit("otp-verify"), verifyOtp);

module.exports = router;
