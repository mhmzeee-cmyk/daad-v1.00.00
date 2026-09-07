/* Developer: محمد محمود الحموز | Dhad Studio */
const jwt = require("jsonwebtoken");
const { isAccountLocked, recordFailedAttempt, clearFailedAttempts, logAudit, strictRateLimit, validatePasswordStrength } = require("../middlewares/strictSecurity");
const { logger } = require("../utils/logger");
const authService = require("../services/authService");

// ── Refresh Token Blacklist ──────────────────────────────────────────────
// Controller-level: tracks per-user token fingerprints with TTL
const REFRESH_TOKEN_FINGERPRINTS = new Map(); // userId -> { hashes: Set, timestamps: number[] }

// Cleanup old fingerprints every 5 minutes
const _fingerprintCleanupInterval = setInterval(() => {
  const maxAge = 7 * 24 * 60 * 60 * 1000; // 7 days
  const now = Date.now();
  for (const [userId, data] of REFRESH_TOKEN_FINGERPRINTS) {
    const validTimestamps = data.timestamps.filter(ts => now - ts < maxAge);
    if (validTimestamps.length === 0) {
      REFRESH_TOKEN_FINGERPRINTS.delete(userId);
    } else {
      data.timestamps = validTimestamps;
      if (data.hashes.size > 50) {
        data.hashes = new Set([...data.hashes].slice(-validTimestamps.length));
      }
    }
  }
}, 5 * 60 * 1000);

// SECURITY: Prevent interval from keeping process alive
if (_fingerprintCleanupInterval.unref) _fingerprintCleanupInterval.unref();

function blacklistRefreshToken(token, userId) {
  const hash = authService.hashToken(token);
  authService.blacklistToken(hash);
  
  if (!REFRESH_TOKEN_FINGERPRINTS.has(userId)) {
    REFRESH_TOKEN_FINGERPRINTS.set(userId, { hashes: new Set(), timestamps: [] });
  }
  const data = REFRESH_TOKEN_FINGERPRINTS.get(userId);
  data.hashes.add(hash);
  data.timestamps.push(Date.now());
}

function isRefreshTokenBlacklisted(token) {
  const hash = authService.hashToken(token);
  return authService.isTokenBlacklisted(hash);
}

// ── Cookie Helpers ───────────────────────────────────────────────────────────
const COOKIE_OPTIONS = {
  httpOnly: true,
  secure: process.env.NODE_ENV === 'production' || process.env.COOKIE_SECURE === 'true',
  sameSite: 'strict',
  path: '/',
};

function setTokenCookie(res, accessToken, refreshToken) {
  res.cookie('access_token', accessToken, { ...COOKIE_OPTIONS, maxAge: 3600 * 1000 });
  res.cookie('refresh_token', refreshToken, { ...COOKIE_OPTIONS, maxAge: 7 * 24 * 3600 * 1000 });
}

function clearTokenCookie(res) {
  res.clearCookie('access_token', { path: '/', sameSite: 'strict' });
  res.clearCookie('refresh_token', { path: '/', sameSite: 'strict' });
}

// POST /api/v1/auth/login — authenticate user, return JWT
async function login(req, res, next) {
  try {
    const { username, password } = req.body;

    // Type guard: username must be a string
    if (typeof username !== "string" || typeof password !== "string") {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "يجب أن يكون اسم المستخدم وكلمة المرور نصوصاً",
      });
    }

    if (!username || !password) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "اسم المستخدم وكلمة المرور مطلوبان",
      });
    }

    // Reject extremely long inputs
    if (username.length > 254 || password.length > 128) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "بيانات الدخول غير صحيحة",
      });
    }

    // Check account lockout BEFORE rate limiting
    if (await isAccountLocked(username)) {
      logAudit("LOGIN_BLOCKED_LOCKED_ACCOUNT", { username, ip: req.ip });
      return res.status(423).json({
        error: "الحساب مقفل",
        message: "الحساب مقفل بسبب محاولات فاشلة كثيرة. حاول مرة أخرى لاحقاً.",
      });
    }

    const prisma = req.app.get("prisma");

    // Try email first (teachers/admins), then nationalId (students)
    let user = await prisma.user.findFirst({
      where: { email: username },
      include: { school: true },
    });

    if (!user) {
      user = await prisma.user.findFirst({
        where: { nationalId: username },
        include: { school: true },
      });
    }

    if (!user) {
      await recordFailedAttempt(username);
      logAudit("LOGIN_USER_NOT_FOUND", { username, ip: req.ip });
      return res.status(401).json({
        error: "غير مصرح",
        message: "بيانات الدخول غير صحيحة",
      });
    }

    const validPassword = await authService.comparePassword(password, user.passwordHash);
    if (!validPassword) {
      // Record failed attempt
      await recordFailedAttempt(username);

      // Log failed attempt
      await prisma.loginLog.create({
        data: {
          userId: user.id,
          ip: req.ip || "",
          userAgent: req.headers["user-agent"] || "",
          success: false,
        },
      }).catch(() => {}); // non-blocking

      logAudit("LOGIN_INVALID_PASSWORD", { userId: user.id, username, ip: req.ip });

      return res.status(401).json({
        error: "غير مصرح",
        message: "بيانات الدخول غير صحيحة",
      });
    }

    // Clear failed attempts on successful login
    await clearFailedAttempts(username);
    logAudit("LOGIN_SUCCESS", { userId: user.id, username, ip: req.ip });

    // Invalidate previous sessions by incrementing tokenVersion
    const newTokenVersion = (user.tokenVersion || 0) + 1;
    await prisma.user.update({
      where: { id: user.id },
      data: { tokenVersion: newTokenVersion },
    }).catch(() => {}); // non-blocking

    // Log successful login
    await prisma.loginLog.create({
      data: {
        userId: user.id,
        ip: req.ip || "",
        userAgent: req.headers["user-agent"] || "",
        success: true,
      },
    }).catch(() => {}); // non-blocking

    // Log activity for student dashboard
    await prisma.activityLog.create({
      data: {
        userId: user.id,
        action: 'LOGIN',
        details: JSON.stringify({ ip: req.ip || "" })
      }
    }).catch(() => {}); // non-blocking

    // Generate tokens with new tokenVersion to invalidate previous sessions
    user.tokenVersion = newTokenVersion;
    const { accessToken, refreshToken } = authService.generateTokens(user);

    // Set httpOnly cookies — tokens are NOT returned in response body
    setTokenCookie(res, accessToken, refreshToken);

    res.json({
      accessToken,
      expiresIn: 3600,
      userId: user.id,
      username: user.name,
      role: user.role,
      tokenVersion: newTokenVersion,
      profile: {
        id: user.id,
        name: user.name,
        email: user.email,
        role: user.role,
        school: {
          id: user.school.id,
          name: user.school.name,
        },
      },
    });
  } catch (err) {
    next(err);
  }
}

// POST /api/v1/auth/register — admin creates a new user account
async function register(req, res, next) {
  try {
    const { name, email, nationalId: rawNationalId, role, schoolId, password } = req.body;
    const nationalId = rawNationalId ? rawNationalId.trim() : rawNationalId;

    // ── Validation ──────────────────────────────────────────────────────
    if (!name || !role) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "الاسم والدور مطلوبان",
      });
    }

    if (name.length > 100) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "يجب أن لا يتجاوز الاسم 100 حرف",
      });
    }

    const allowedRoles = ["STUDENT", "TEACHER", "ADMIN"];
    if (!allowedRoles.includes(role)) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: `يجب أن يكون الدور أحد: ${allowedRoles.join(", ")}`,
      });
    }

    // Teachers and admins must have an email
    if ((role === "TEACHER" || role === "ADMIN") && !email) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "البريد الإلكتروني مطلوب للمعلمين والإداريين",
      });
    }

    if (email && email.length > 254) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "البريد الإلكتروني طويل جداً",
      });
    }

    // Validate email format
    if (email && !authService.validateEmail(email)) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "صيغة البريد الإلكتروني غير صحيحة",
      });
    }

    // Strip control chars from email (header injection protection)
    if (email) {
      req.body.email = email.replace(/[\r\n\0]/g, "").trim();
    }

    // Students must have email (for login)
    if (role === "STUDENT" && !email) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "البريد الإلكتروني مطلوب للطلاب",
      });
    }

    // National ID is optional for students
    if (nationalId && nationalId.length > 20) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "يجب أن لا يتجاوز الرقم الوطني 20 حرف",
      });
    }

    // Password validation — enforce strong password policy
    if (!password) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور مطلوبة",
      });
    }

    const passwordErrors = validatePasswordStrength(password);
    if (passwordErrors.length > 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: passwordErrors.join(". "),
      });
    }

    if (password.length > 128) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "يجب أن لا تتجاوز كلمة المرور 128 حرف",
      });
    }

    const prisma = req.app.get("prisma");

    // Determine target school
    // For public student registration, use the first available school
    let targetSchoolId = schoolId;
    if (!targetSchoolId) {
      if (req.user && req.user.schoolId) {
        // Authenticated user — use their school
        targetSchoolId = req.user.schoolId;
      } else {
        // Public registration — use the first school
        const defaultSchool = await prisma.school.findFirst();
        if (defaultSchool) {
          targetSchoolId = defaultSchool.id;
        }
      }
    }

    if (!targetSchoolId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف المدرسة مطلوب",
      });
    }

    // Role-based authorization checks (only for authenticated users)
    if (req.user) {
      // Teachers can only create users in their own school
      if (req.user.role === "TEACHER" && targetSchoolId !== req.user.schoolId) {
        return res.status(403).json({
          error: "محظور",
          message: "يمكنك فقط إنشاء مستخدمين في مدرستك",
        });
      }

      // Teachers cannot create ADMIN accounts (privilege escalation prevention)
      if (req.user.role === "TEACHER" && role === "ADMIN") {
        return res.status(403).json({
          error: "محظور",
          message: "المعلمون لا يمكنهم إنشاء حسابات إدارية",
        });
      }

      // Students cannot create any accounts
      if (req.user.role === "STUDENT") {
        return res.status(403).json({
          error: "محظور",
          message: "الطلاب لا يمكنهم إنشاء حسابات مستخدمين",
        });
      }
    }

    // Check for duplicate email or nationalId within the school
    if (email) {
      const existing = await prisma.user.findFirst({
        where: { schoolId: targetSchoolId, email },
      });
      if (existing) {
        return res.status(409).json({
          error: "تعارض",
          message: "يوجد مستخدم بهذا البريد الإلكتروني في هذه المدرسة بالفعل",
        });
      }
    }

    if (nationalId) {
      const existing = await prisma.user.findFirst({
        where: { schoolId: targetSchoolId, nationalId },
      });
      if (existing) {
        return res.status(409).json({
          error: "تعارض",
          message: "يوجد مستخدم بهذا الرقم الوطني في هذه المدرسة بالفعل",
        });
      }
    }

    // ── Hash password ───────────────────────────────────────────────────
    const passwordHash = await authService.hashPassword(password);

    // ── Create user ─────────────────────────────────────────────────────
    const user = await prisma.user.create({
      data: {
        schoolId: targetSchoolId,
        role,
        name,
        email: email || null,
        nationalId: nationalId || null,
        passwordHash,
      },
      include: {
        school: { select: { id: true, name: true } },
      },
    });

    logger.info(`User created: ${user.name} (${user.role}) in school "${user.school.name}" by ${req.user ? req.user.name : 'public registration'}`);

    res.status(201).json({
      message: `User "${user.name}" created successfully`,
      user: {
        id: user.id,
        name: user.name,
        email: user.email,
        nationalId: user.nationalId,
        role: user.role,
        school: user.school,
        createdAt: user.createdAt,
      },
    });
  } catch (err) {
    next(err);
  }
}

// POST /api/v1/auth/change-password — authenticated user changes their own password
async function changePassword(req, res, next) {
  try {
    const { currentPassword, newPassword } = req.body;

    if (!currentPassword || !newPassword) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور الحالية والجديدة مطلوبتان",
      });
    }

    // Enforce strong password policy
    const passwordErrors = validatePasswordStrength(newPassword);
    if (passwordErrors.length > 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: passwordErrors.join(". "),
      });
    }

    // Check account lockout
    if (await isAccountLocked(req.user.id)) {
      logAudit("PASSWORD_CHANGE_BLOCKED_LOCKED", { userId: req.user.id, ip: req.ip });
      return res.status(423).json({
        error: "الحساب مقفل",
        message: "الحساب مقفل. لا يمكن تغيير كلمة المرور.",
      });
    }

    const prisma = req.app.get("prisma");
    const user = await prisma.user.findUnique({
      where: { id: req.user.id },
    });

    if (!user) {
      return res.status(404).json({
        error: "غير موجود",
        message: "المستخدم غير موجود",
      });
    }

    const validPassword = await authService.comparePassword(currentPassword, user.passwordHash);
    if (!validPassword) {
      await recordFailedAttempt(req.user.id);
      logAudit("PASSWORD_CHANGE_INVALID_PASSWORD", { userId: req.user.id, ip: req.ip });
      return res.status(401).json({
        error: "غير مصرح",
        message: "كلمة المرور الحالية غير صحيحة",
      });
    }

    // Clear failed attempts on successful password change
    await clearFailedAttempts(req.user.id);

    const passwordHash = await authService.hashPassword(newPassword);
    await prisma.user.update({
      where: { id: req.user.id },
      data: { passwordHash },
    });

    logAudit("PASSWORD_CHANGED", { userId: req.user.id, ip: req.ip });

    res.json({
      message: "تم تغيير كلمة المرور بنجاح",
    });
  } catch (err) {
    next(err);
  }
}

// POST /api/v1/auth/reset-password — admin resets a user's password
async function resetPassword(req, res, next) {
  try {
    const { userId, newPassword } = req.body;

    if (!userId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "userId is required",
      });
    }

    const prisma = req.app.get("prisma");
    const targetUser = await prisma.user.findUnique({
      where: { id: userId },
      include: { school: { select: { name: true } } },
    });

    if (!targetUser) {
      return res.status(404).json({
        error: "غير موجود",
        message: "المستخدم غير موجود",
      });
    }

    // Teachers can only reset passwords in their own school
    if (req.user.role === "TEACHER" && targetUser.schoolId !== req.user.schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط إعادة تعيين كلمات المرور للمستخدمين في مدرستك",
      });
    }

    // Require a new password
    if (!newPassword) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور الجديدة مطلوبة",
      });
    }

    // Enforce password policy
    const passwordErrors = validatePasswordStrength(newPassword);
    if (passwordErrors.length > 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: passwordErrors.join(". "),
      });
    }

    const passwordHash = await authService.hashPassword(newPassword);

    await prisma.user.update({
      where: { id: userId },
      data: { passwordHash },
    });

    // Clear any lockout on the target user
    await clearFailedAttempts(targetUser.email || targetUser.nationalId || userId);

    logAudit("PASSWORD_RESET", {
      adminId: req.user.id,
      targetUserId: userId,
      targetUserName: targetUser.name,
      ip: req.ip,
    });

    res.json({
      message: `Password reset for "${targetUser.name}"`,
    });
  } catch (err) {
    next(err);
  }
}

// ── Refresh Token ─────────────────────────────────────────────────────────
async function refreshToken(req, res, next) {
  try {
    // Support both body and cookie for refresh
    const token = req.body.refreshToken || (req.cookies && req.cookies.refresh_token);

    if (!token) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "رمز التحديث مطلوب (في الجسم أو ملف تعريف الارتباط)",
      });
    }

    // Check if token is blacklisted (already used)
    if (isRefreshTokenBlacklisted(token)) {
      clearTokenCookie(res);
      return res.status(401).json({
        error: "غير مصرح",
        message: "تم استخدام هذا الرمز مسبقاً. يرجى تسجيل الدخول مرة أخرى.",
      });
    }

    const prisma = req.app.get("prisma");

    // Verify refresh token
    let decoded;
    try {
      decoded = authService.verifyRefreshToken(token);
    } catch (err) {
      clearTokenCookie(res);
      return res.status(401).json({
        error: "غير مصرح",
        message: "رمز التحديث غير صالح أو منتهي الصلاحية",
      });
    }

    // Check token type
    if (decoded.type !== "refresh") {
      clearTokenCookie(res);
      return res.status(401).json({
        error: "غير مصرح",
        message: "نوع الرمز غير صالح",
      });
    }

    // Get user from DB
    const user = await prisma.user.findUnique({
      where: { id: decoded.id },
      select: {
        id: true,
        name: true,
        email: true,
        role: true,
        schoolId: true,
        classroomId: true,
        isActive: true,
        tokenVersion: true,
      },
    });

    if (!user || !user.isActive) {
      clearTokenCookie(res);
      return res.status(401).json({
        error: "غير مصرح",
        message: "المستخدم غير موجود أو غير نشط",
      });
    }

    // Check token version
    if (decoded.tokenVersion !== undefined && decoded.tokenVersion !== user.tokenVersion) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "تم سحب الرمز",
      });
    }

    // Blacklist the old refresh token (prevent reuse)
    blacklistRefreshToken(token, user.id);

    // Increment tokenVersion to invalidate all previous access tokens
    const newTokenVersion = (user.tokenVersion || 0) + 1;
    await prisma.user.update({
      where: { id: user.id },
      data: { tokenVersion: newTokenVersion },
    }).catch(() => {});

    // Generate new tokens with updated version (rotation)
    user.tokenVersion = newTokenVersion;
    const { accessToken: newAccessToken, refreshToken: newRefreshToken } = authService.generateTokens(user);

    // Rotate httpOnly cookies as well — tokens are NOT returned in response body
    setTokenCookie(res, newAccessToken, newRefreshToken);

    res.json({
      success: true,
      expiresIn: 3600,
    });
  } catch (err) {
    next(err);
  }
}

// ── Logout ────────────────────────────────────────────────────────────────
async function logout(req, res, next) {
  try {
    const prisma = req.app.get("prisma");
    const userId = req.user?.id;

    // Blacklist the refresh token if provided (verify ownership first)
    const refreshToken = req.body.refreshToken;
    if (refreshToken && userId) {
      try {
        const decoded = jwt.decode(refreshToken);
        if (decoded && decoded.id === userId) {
          blacklistRefreshToken(refreshToken, userId);
        }
      } catch (e) {
        // Token may be malformed, still proceed with logout
      }
    }

    // Increment tokenVersion to revoke all existing JWTs for this user
    if (userId && prisma) {
      await prisma.user.update({
        where: { id: userId },
        data: { tokenVersion: { increment: 1 } },
      }).catch(() => {});
    }

    logAudit("LOGOUT", {
      userId: userId || "unknown",
      ip: req.ip,
    });

    // Clear auth cookies
    clearTokenCookie(res);

    res.json({
      success: true,
      message: "تم تسجيل الخروج بنجاح",
    });
  } catch (err) {
    next(err);
  }
}

module.exports = { login, register, changePassword, resetPassword, refreshToken, logout };
