/* Developer: محمد محمود الحموز | Dhad Studio */
const jwt = require("jsonwebtoken");
const crypto = require("crypto");
const { logger } = require("../utils/logger");

// ── HMAC Telemetry Signature Validation ───────────────────────────────────────
// Validates HMAC SHA-256 signature to prevent payload spoofing
function validateHMACSignature(req, res, next) {
  const signature = req.headers['x-telemetry-signature'];
  const timestamp = req.headers['x-telemetry-timestamp'];
  
  if (!signature || !timestamp) {
    // Log potential tampering attempt
    logger.warn(`Missing HMAC signature from IP: ${req.ip}`);
    return res.status(403).json({
      error: "محظور",
      message: "توقيع القياس مفقود"
    });
  }

  // Check timestamp freshness (prevent replay attacks - 5 minute window)
  const requestTime = parseInt(timestamp);
  const currentTime = Date.now();
  if (isNaN(requestTime) || Math.abs(currentTime - requestTime) > 5 * 60 * 1000) {
    logger.warn(`Expired HMAC timestamp from IP: ${req.ip}`);
    return res.status(403).json({
      error: "محظور",
      message: "انتهت صلاحية توقيع القياس"
    });
  }

  // Get raw body for HMAC verification
  const rawBody = req.rawBody || JSON.stringify(req.body);
  
  // Create HMAC using session secret or JWT token
  const hmacSecret = process.env.HMAC_SECRET;
  if (!hmacSecret) {
    logger.error('HMAC_SECRET not configured!');
    return res.status(500).json({ error: "خطأ في إعدادات الخادم" });
  }
  const expectedSignature = crypto
    .createHmac('sha256', hmacSecret)
    .update(rawBody + timestamp)
    .digest('hex');

  // Timing-safe comparison to prevent timing attacks
  const sigBuf = Buffer.from(signature, 'hex');
  const expectedBuf = Buffer.from(expectedSignature, 'hex');
  if (sigBuf.length !== expectedBuf.length || !crypto.timingSafeEqual(sigBuf, expectedBuf)) {
    // CRITICAL: Log suspicious activity
    logger.error('HMAC SIGNATURE MISMATCH - Potential cheating attempt!', {
      ip: req.ip,
      student: req.user?.id || 'unknown',
    });
    
    return res.status(403).json({
      error: "محظور",
      message: "توقيع القياس غير صالح - تم اكتشاف تلاعب محتمل"
    });
  }

  // Signature is valid
  req.hmacValid = true;
  next();
}

// ── Suspicious Submission Detector ────────────────────────────────────────────
// Flags submissions with suspicious execution times or patterns
function detectSuspiciousActivity(submissionData, challenge) {
  const reasons = [];
  const { executionTime, code, status, score } = submissionData;
  const { tier, difficulty, estimatedTime } = challenge;

  // Check 1: Impossibly fast execution time for complex challenges
  const MIN_EXECUTION_TIMES = {
    1: 10,    // Tier 1 (Beginner): minimum 10ms
    2: 25,    // Tier 2 (Intermediate): minimum 25ms
    3: 50,    // Tier 3 (Advanced): minimum 50ms
    4: 100,   // Tier 4 (Expert): minimum 100ms
    5: 150    // Tier 5 (Master): minimum 150ms
  };

  const minTime = MIN_EXECUTION_TIMES[tier] || 10;
  if (executionTime && executionTime < minTime) {
    reasons.push({
      type: 'SUSPICIOUS_TIMING',
      detail: `Execution time ${executionTime}ms is suspiciously fast for Tier ${tier} (min: ${minTime}ms)`,
      severity: 'HIGH'
    });
  }

  // Check 2: Perfect score on first attempt for difficult challenges
  if (score === 100 && tier >= 3) {
    reasons.push({
      type: 'PERFECT_SCORE_FIRST_ATTEMPT',
      detail: `Perfect score on Tier ${tier} challenge`,
      severity: 'MEDIUM'
    });
  }

  // Check 3: Code length suspiciously short for complex output
  if (code && code.length < 20 && tier >= 2) {
    reasons.push({
      type: 'SHORT_CODE_COMPLEX_OUTPUT',
      detail: `Very short code (${code.length} chars) for Tier ${tier} challenge`,
      severity: 'MEDIUM'
    });
  }

  // Check 4: Execution time faster than estimated time by > 90%
  if (estimatedTime && executionTime) {
    const estimatedMs = estimatedTime * 60 * 1000; // Convert minutes to ms
    if (executionTime < estimatedMs * 0.1) {
      reasons.push({
        type: 'TIME_ANOMALY',
        detail: `Execution time is ${((1 - executionTime/estimatedMs) * 100).toFixed(1)}% faster than estimated`,
        severity: 'HIGH'
      });
    }
  }

  return {
    suspicious: reasons.length > 0,
    reasons: reasons,
    maxSeverity: reasons.length > 0 ? 
      reasons.reduce((max, r) => 
        ['LOW', 'MEDIUM', 'HIGH', 'CRITICAL'].indexOf(r.severity) > 
        ['LOW', 'MEDIUM', 'HIGH', 'CRITICAL'].indexOf(max) ? r.severity : max
      , 'LOW') : null
  };
}

// ── JWT Authentication ───────────────────────────────────────────────────────
// Verifies the JWT token and attaches decoded user to req.user
// Supports: Authorization Bearer header OR httpOnly cookie `access_token`
async function authenticate(req, res, next) {
  // Try Authorization header first, then cookie
  let token = null;
  const header = req.headers.authorization;

  if (header && header.startsWith("Bearer ")) {
    token = header.split(" ")[1];
  } else if (req.cookies && req.cookies.access_token) {
    token = req.cookies.access_token;
  }

  if (!token || token.length === 0) {
    return res.status(401).json({
      error: "غير مصرح",
      message: "الرمز مفقود. قدم ترويسة Authorization: Bearer أو ملف تعريف الارتباط access_token",
    });
  }

  try {
    const decoded = jwt.verify(token, process.env.JWT_SECRET, {
      algorithms: ["HS256"], // Explicitly require HS256 — prevent algorithm confusion
      maxAge: process.env.JWT_ACCESS_EXPIRES || "1h",
    });

    // Validate token payload structure
    if (!decoded.id || !decoded.role) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "محتوى الرمز غير صالح",
      });
    }

    // schoolId required for non-admin users; admins may be super-admins without a school
    if (decoded.role !== "ADMIN" && !decoded.schoolId) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "محتوى الرمز غير صالح - schoolId مفقود",
      });
    }

    // Attach user info to request
    req.user = {
      id: decoded.id,
      schoolId: decoded.schoolId,
      classroomId: decoded.classroomId || null,
      role: decoded.role,
      name: decoded.name,
      tokenVersion: decoded.tokenVersion || 1,
    };

    // ── Combined DB Check: Activation + Token Revocation ────────────────────
    // SECURITY: Single query for both checks halves DB round-trips per request
    const prisma = req.app.get("prisma");
    if (!prisma) {
      return res.status(503).json({
        error: "Service Unavailable",
        message: "Unable to verify account status. Please try again.",
      });
    }

    try {
      const selectFields = {
        isActive: true,
        tokenVersion: true,
      };
      if (decoded.role === "STUDENT") {
        selectFields.isApproved = true;
        selectFields.invitation = { select: { isActivated: true } };
      }

      const user = await prisma.user.findUnique({
        where: { id: decoded.id },
        select: selectFields,
      });

      if (!user) {
        return res.status(401).json({
          error: "غير مصرح",
          message: "المستخدم غير موجود",
        });
      }

      // Student activation check
      if (decoded.role === "STUDENT") {
        if (!user.isActive) {
          return res.status(403).json({
            error: "محظور",
            message: "الحساب غير نشط. تواصل مع المعلم لتفعيل حسابك.",
          });
        }
        if (user.invitation && !user.invitation.isActivated) {
          return res.status(403).json({
            error: "محظور",
            message: "الحساب لم يتم تفعيله بعد. تحقق من بريدك الإلكتروني لتفعيل الحساب.",
          });
        }
      }

      // Token revocation check
      if (user.tokenVersion !== (decoded.tokenVersion || 0)) {
        return res.status(401).json({
          error: "غير مصرح",
          message: "تم سحب الرمز. يرجى تسجيل الدخول مرة أخرى.",
        });
      }
    } catch (err) {
      logger.error('Auth verification failed', { error: err.message });
      return res.status(503).json({
        error: "Service Unavailable",
        message: "Unable to verify account status. Please try again.",
      });
    }

    next();
  } catch (err) {
    if (err.name === "TokenExpiredError") {
      return res.status(401).json({
        error: "غير مصرح",
        message: "انتهت صلاحية الرمز",
      });
    }
    if (err.name === "JsonWebTokenError") {
      return res.status(401).json({
        error: "غير مصرح",
        message: "الرمز غير صالح",
      });
    }
    return res.status(401).json({
      error: "غير مصرح",
      message: "فشل التحقق من الرمز",
    });
  }
}

// ── Role-Based Authorization ─────────────────────────────────────────────────
// Accepts one or more roles: requireRole("ADMIN"), requireRole("TEACHER", "ADMIN")
function requireRole(...roles) {
  return (req, res, next) => {
    if (!req.user) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "المصادقة مطلوبة",
      });
    }

    if (!roles.includes(req.user.role)) {
      return res.status(403).json({
        error: "محظور",
        message: `This action requires one of the following roles: ${roles.join(", ")}`,
      });
    }

    next();
  };
}

// ── Shorthand Role Guards ────────────────────────────────────────────────────
const requireAdmin = requireRole("ADMIN");
const requireTeacher = requireRole("TEACHER", "ADMIN");
const requireStudent = requireRole("STUDENT");
const requireTeacherOrAdmin = requireRole("TEACHER", "ADMIN");

// ── Ownership Check ──────────────────────────────────────────────────────────
// Ensures the user can only access their own school's data, unless they're an admin
function requireSchoolAccess(req, res, next) {
  if (!req.user) {
    return res.status(401).json({
      error: "غير مصرح",
      message: "المصادقة مطلوبة",
    });
  }

  // Admins can access any school
  if (req.user.role === "ADMIN") {
    return next();
  }

  // Teachers and students can only access their own school
  const targetSchoolId = req.params.id || req.params.schoolId || req.query.schoolId || req.body.schoolId;

  if (targetSchoolId && targetSchoolId !== req.user.schoolId) {
    return res.status(403).json({
      error: "محظور",
      message: "يمكنك فقط الوصول لبيانات مدرستك",
    });
  }

  next();
}

// ── Resource Ownership Check ─────────────────────────────────────────────────
// Ensures the user owns the resource, unless they're an admin
// Usage: router.delete("/challenges/:id", authenticate, requireOwnership("authorId"), deleteChallenge)
function requireOwnership(resourceIdParam = "id", ownershipField = "authorId") {
  return async (req, res, next) => {
    if (!req.user) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "المصادقة مطلوبة",
      });
    }

    // Admins can modify any resource
    if (req.user.role === "ADMIN") {
      return next();
    }

    const resourceId = req.params[resourceIdParam];
    if (!resourceId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "Resource ID is required",
      });
    }

    const prisma = req.app.get("prisma");

    try {
      // Determine which model to query based on the route
      const model = getModelFromRoute(req.originalUrl);
      
      // SECURITY: Fail-closed - deny access if model cannot be determined
      if (model === "unknown") {
        return res.status(403).json({
          error: "محظور",
          message: "Unable to verify resource ownership",
        });
      }

      const resource = await prisma[model].findUnique({
        where: { id: resourceId },
        select: { [ownershipField]: true, schoolId: true },
      });

      if (!resource) {
        return res.status(404).json({
          error: "غير موجود",
          message: "Resource not found",
        });
      }

      // Check ownership
      if (resource[ownershipField] !== req.user.id) {
        return res.status(403).json({
          error: "محظور",
          message: "You don't have permission to modify this resource",
        });
      }

      next();
    } catch (err) {
      next(err);
    }
  };
}

// Helper: determine Prisma model from URL path
function getModelFromRoute(url) {
  // SECURITY: Use path segment matching, not substring matching
  // to prevent model confusion attacks (e.g., /users-challenges matching "challenge")
  const segments = url.split('/').filter(Boolean);
  const routeModelMap = {
    'challenges': 'challenge',
    'submissions': 'submission',
    'schools': 'school',
    'courses': 'course',
    'users': 'user',
    'classrooms': 'classroom',
    'assessments': 'assessment',
  };

  for (const segment of segments) {
    if (routeModelMap[segment]) return routeModelMap[segment];
  }

  return "unknown";
}

// ── Token Revocation Check ───────────────────────────────────────────────────
// Compare tokenVersion in JWT against a stored version in DB
async function checkTokenRevocation(req, res, next) {
  try {
    const prisma = req.app.get("prisma");
    const user = await prisma.user.findUnique({
      where: { id: req.user.id },
      select: { tokenVersion: true },
    });

    if (!user || user.tokenVersion !== req.user.tokenVersion) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "تم سحب الرمز. يرجى تسجيل الدخول مرة أخرى.",
      });
    }

    next();
  } catch (err) {
    next(err);
  }
}

module.exports = {
  // Rate limiters
  // Authentication
  authenticate,
  checkTokenRevocation,

  // Role-based authorization
  requireRole,
  requireAdmin,
  requireTeacher,
  requireStudent,
  requireTeacherOrAdmin,

  // Ownership/school access
  requireSchoolAccess,
  requireOwnership,

  // Security
  validateHMACSignature,
  detectSuspiciousActivity,
};
