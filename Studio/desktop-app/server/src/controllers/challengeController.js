// ==============================================================================
// Dhad Studio Challenge Controller
// Challenge CRUD, Submissions, Daily Challenge, HMAC Verification
// Developer: محمد محمود الحموز | Dhad Studio
// ==============================================================================

const crypto = require('crypto');
const prisma = require('../utils/prisma');
const { logger } = require('../utils/logger');
const { isAccountLocked } = require('../middlewares/strictSecurity');

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Challenge Verification Controller                     │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function verifyChallengeCompletion(req, res, next) {
  try {
    // Security: Check HMAC_SECRET is configured
    if (!process.env.HMAC_SECRET) {
      logger.error('HMAC_SECRET not configured');
      return res.status(500).json({
        error: "خطأ في الإعدادات",
        message: "HMAC_SECRET غير مُعد. يرجى الإعداد في ملف البيئة."
      });
    }

    // Security: Check account lockout first (highest priority)
    if (await isAccountLocked(req.body.studentId)) {
      logger.warn('Challenge verification blocked - account locked', {
        studentId: req.body.studentId,
        ip: req.ip,
        reason: 'Account lockout due to failed attempts'
      });
      return res.status(423).json({
        error: "الحساب مقفل",
        message: "حساب الطالب مقفل بسبب محاولات فاشلة متعددة. يرجى المحاولة لاحقًا."
      });
    }

    const { studentId, taskId, timestamp, signature } = req.body;

    // SECURITY: Verify studentId matches authenticated user
    if (studentId !== req.user.id) {
      return res.status(403).json({
        error: "محظور",
        message: "studentId لا يتطابق مع المستخدم المُصادق عليه"
      });
    }

    // Validate required fields
    if (!studentId || !taskId || !timestamp || !signature) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "يجب إرسال studentId، taskId، timestamp، و signature معًا."
      });
    }

    // Validate timestamp format and prevent replay attacks
    const now = Date.now();
    const timestampMs = parseInt(timestamp);
    if (isNaN(timestampMs)) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "الطابع الزمني غير صالح."
      });
    }

    const fiveMinutesAgo = now - (5 * 60 * 1000);
    if (timestampMs < fiveMinutesAgo) {
      logger.warn('Challenge verification rejected - timestamp too old', {
        studentId,
        taskId,
        timestamp,
        receivedAt: new Date().toISOString(),
        serverTime: now,
        ageMinutes: Math.round((now - timestampMs) / 60000)
      });
      return res.status(403).json({
        error: "Token Expired",
        message: "الطابع الزمني منتهي الصلاحية. يرجى إعادة الاختبار."
      });
    }

    // Verify HMAC signature
    const message = `${studentId}:${taskId}:${timestamp}`;
    const expectedSignature = crypto
      .createHmac('sha256', process.env.HMAC_SECRET)
      .update(message)
      .digest('hex');

    // Constant-time comparison to prevent timing attacks
    const sigBuf = Buffer.from(signature, 'hex');
    const expectedBuf = Buffer.from(expectedSignature, 'hex');
    if (sigBuf.length !== expectedBuf.length) {
      return res.status(403).json({
        error: "محظور",
        message: "Signatures length mismatch"
      });
    }
    const isValidSignature = crypto.timingSafeEqual(sigBuf, expectedBuf);

    if (!isValidSignature) {
      logger.error('Challenge verification failed - invalid signature', {
        studentId,
        taskId,
        timestamp,
      });
      return res.status(403).json({
        error: "محظور",
        message: "التوقيع غير صالح. تم اكتشاف محاولة غش!"
      });
    }

    // Generate confirmation token for the verified completion
    const confirmationToken = crypto
      .createHmac('sha256', process.env.HMAC_CONFIRM_SECRET || process.env.HMAC_SECRET)
      .update(`${studentId}:${taskId}:${timestamp}:${expectedSignature}`)
      .digest('hex');

    // Log successful verification
    logger.info('Challenge verification succeeded', {
      studentId,
      taskId,
      timestamp,
      ip: req.ip,
      userAgent: req.headers['user-agent']
    });

    res.json({
      success: true,
      confirmationToken,
      message: "تم التحقق من اكتمال التحدي بنجاح!"
    });

  } catch (error) {
    logger.error('Challenge verification error', {
      error: error.message,
      stack: error.stack,
      studentId: req.body.studentId,
      taskId: req.body.taskId
    });
    next(error);
  }
}

// Helper: Store verification record for auditing
async function storeChallengeVerification(verificationData) {
  const prisma = require('../utils/prisma');
  
  try {
    await prisma.challengeVerification.create({
      data: {
        studentId: verificationData.studentId,
        taskId: verificationData.taskId,
        timestamp: new Date(parseInt(verificationData.timestamp)),
        signature: verificationData.signature,
        ip: verificationData.ip,
        userAgent: verificationData.userAgent,
        createdAt: new Date()
      }
    });
  } catch (err) {
    logger.error('Failed to store challenge verification record', {
      error: err.message,
      studentId: verificationData.studentId,
      taskId: verificationData.taskId
    });
    // Non-blocking - continue without storing
  }
}

// Middleware: Challenge verification security checks
async function validateChallengeSecurity(req, res, next) {
  try {
    const { studentId, taskId, timestamp, signature } = req.body;

    if (!studentId || !taskId || !timestamp || !signature) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "حقول التحقق من التحدي تتطلب: studentId، taskId، timestamp، signature"
      });
    }

    // Verify HMAC signature format
    if (!/^[0-9a-f]{64}$/i.test(signature)) {
      return res.status(400).json({
        error: "خطأ في الطلب", 
        message: "تنسيق التوقيع غير صالح."
      });
    }

    // Validate timestamp
    const timestampMs = parseInt(timestamp);
    if (isNaN(timestampMs) || timestampMs <= 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "الطابع الزمني غير صالح."
      });
    }

    const now = Date.now();
    const fiveMinutesAgo = now - (5 * 60 * 1000);
    if (timestampMs < fiveMinutesAgo) {
      return res.status(403).json({
        error: "Token Expired",
        message: "الطابع الزمني منتهي الصلاحية (أكثر من 5 دقائق)."
      });
    }

    next();
  } catch (error) {
    logger.error('Challenge security validation error', { error: error.message });
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  List Challenges                                                          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function listChallenges(req, res, next) {
  try {
    const { role } = req.user;
    const { difficulty, courseId, tier, published } = req.query;

    const whereClause = {};

    // SECURITY: Non-admins only see their school's challenges + system challenges
    if (role !== 'ADMIN') {
      whereClause.published = true;
      whereClause.OR = [
        { schoolId: req.user.schoolId },
        { schoolId: null },
      ];
    } else if (published !== undefined) {
      whereClause.published = published === 'true';
    }

    if (courseId) {
      whereClause.courseId = courseId;
    } else if (courseId === '') {
      whereClause.courseId = null;
    }

    if (difficulty && difficulty !== 'all') {
      const valid = ['BEGINNER', 'INTERMEDIATE', 'ADVANCED', 'EXPERT'];
      if (valid.includes(difficulty)) {
        whereClause.difficulty = difficulty;
      }
    }

    if (tier) {
      whereClause.tier = parseInt(tier);
    }

    const challenges = await prisma.challenge.findMany({
      where: whereClause,
      select: {
        id: true,
        title: true,
        description: true,
        starterCode: true,
        requirements: true,
        difficulty: true,
        challengeType: true,
        points: true,
        xpReward: true,
        tier: true,
        order: true,
        estimatedTime: true,
        isSystem: true,
        published: true,
        dailyDate: true,
        popularity: true,
        courseId: true,
        createdAt: true,
        ...(role !== 'STUDENT' ? { expectedOutput: true, dynamicOutput: true } : {}),
      },
      orderBy: [{ tier: 'asc' }, { order: 'asc' }, { points: 'asc' }],
      take: 210,
    });

    res.json({
      success: true,
      challenges,
      total: challenges.length,
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Daily Challenge                                                          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function dailyChallenge(req, res, next) {
  try {
    const today = new Date().toISOString().split('T')[0];
    const { role, schoolId } = req.user;

    const baseWhere = { dailyDate: today, published: true };
    if (role !== 'ADMIN') {
      baseWhere.OR = [
        { schoolId: schoolId },
        { schoolId: null },
      ];
    }

    let challenge = await prisma.challenge.findFirst({
      where: baseWhere,
      select: {
        id: true,
        title: true,
        description: true,
        starterCode: true,
        difficulty: true,
        challengeType: true,
        points: true,
        tier: true,
        estimatedTime: true,
        dailyDate: true,
      },
    });

    if (!challenge) {
      const fallbackWhere = { published: true, dailyDate: null };
      if (role !== 'ADMIN') {
        fallbackWhere.OR = [
          { schoolId: schoolId },
          { schoolId: null },
        ];
      }
      challenge = await prisma.challenge.findFirst({
        where: fallbackWhere,
        orderBy: { popularity: 'desc' },
        select: {
          id: true,
          title: true,
          description: true,
          starterCode: true,
          difficulty: true,
          challengeType: true,
          points: true,
          tier: true,
          estimatedTime: true,
          dailyDate: true,
        },
      });
    }

    if (!challenge) {
      return res.status(404).json({
        error: 'غير موجود',
        message: 'لا يوجد تحدي يومي متاح حالياً',
      });
    }

    res.json({ success: true, challenge });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Create Challenge                                                         │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function createChallenge(req, res, next) {
  try {
    const { id: authorId, schoolId } = req.user;
    const {
      title, description, starterCode, expectedOutput, dynamicOutput,
      requirements, difficulty, challengeType, points, tier, order,
      estimatedTime, courseId,
    } = req.body;

    if (!title || !description) {
      return res.status(400).json({
        error: 'خطأ في الطلب',
        message: 'العنوان والوصف مطلوبان',
      });
    }

    if (title.length > 200) {
      return res.status(400).json({
        error: 'خطأ في الطلب',
        message: 'يجب أن لا يتجاوز العنوان 200 حرف',
      });
    }

    const validDifficulties = ['BEGINNER', 'INTERMEDIATE', 'ADVANCED', 'EXPERT'];
    if (difficulty && !validDifficulties.includes(difficulty)) {
      return res.status(400).json({
        error: 'خطأ في الطلب',
        message: `يجب أن يكون الصعوبة أحد: ${validDifficulties.join(', ')}`,
      });
    }

    const validTypes = ['BUG_FIX', 'CODE_COMPLETION', 'FREE_FORM', 'QUIZ'];
    if (challengeType && !validTypes.includes(challengeType)) {
      return res.status(400).json({
        error: 'خطأ في الطلب',
        message: `يجب أن يكون نوع التحدي أحد: ${validTypes.join(', ')}`,
      });
    }

    const challenge = await prisma.challenge.create({
      data: {
        authorId,
        schoolId: schoolId || null,
        title,
        description,
        starterCode: starterCode || '',
        expectedOutput: expectedOutput || '',
        dynamicOutput: dynamicOutput || '',
        requirements: requirements ? JSON.stringify(requirements) : '[]',
        difficulty: difficulty || 'BEGINNER',
        challengeType: challengeType || 'BUG_FIX',
        points: points || 100,
        tier: tier || 1,
        order: order || 1,
        estimatedTime: estimatedTime || null,
        courseId: courseId || null,
      },
      select: { id: true, title: true, difficulty: true, points: true, tier: true, createdAt: true },
    });

    logger.info(`Challenge created: "${challenge.title}" by ${req.user.name}`);

    res.status(201).json({
      success: true,
      message: 'تم إنشاء التحدي بنجاح',
      challenge,
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Update Challenge                                                         │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function updateChallenge(req, res, next) {
  try {
    const { id } = req.params;
    const { schoolId } = req.user;

    const existing = await prisma.challenge.findUnique({ where: { id } });
    if (!existing) {
      return res.status(404).json({
        error: 'غير موجود',
        message: 'التحدي غير موجود',
      });
    }

    if (existing.schoolId && existing.schoolId !== schoolId) {
      return res.status(403).json({
        error: 'محظور',
        message: 'ليس لديك صلاحية تعديل هذا التحدي',
      });
    }

    const {
      title, description, starterCode, expectedOutput, dynamicOutput,
      requirements, difficulty, challengeType, points, tier, order,
      estimatedTime, courseId,
    } = req.body;

    const updateData = {};
    if (title !== undefined) updateData.title = title;
    if (description !== undefined) updateData.description = description;
    if (starterCode !== undefined) updateData.starterCode = starterCode;
    if (expectedOutput !== undefined) updateData.expectedOutput = expectedOutput;
    if (dynamicOutput !== undefined) updateData.dynamicOutput = dynamicOutput;
    if (requirements !== undefined) updateData.requirements = JSON.stringify(requirements);
    if (difficulty !== undefined) updateData.difficulty = difficulty;
    if (challengeType !== undefined) updateData.challengeType = challengeType;
    if (points !== undefined) updateData.points = points;
    if (tier !== undefined) updateData.tier = tier;
    if (order !== undefined) updateData.order = order;
    if (estimatedTime !== undefined) updateData.estimatedTime = estimatedTime;
    if (courseId !== undefined) updateData.courseId = courseId;

    const challenge = await prisma.challenge.update({
      where: { id },
      data: updateData,
      select: { id: true, title: true, difficulty: true, points: true, tier: true, updatedAt: true },
    });

    res.json({
      success: true,
      message: 'تم تحديث التحدي بنجاح',
      challenge,
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Delete Challenge                                                         │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function deleteChallenge(req, res, next) {
  try {
    const { id } = req.params;
    const { schoolId } = req.user;

    const existing = await prisma.challenge.findUnique({ where: { id } });
    if (!existing) {
      return res.status(404).json({
        error: 'غير موجود',
        message: 'التحدي غير موجود',
      });
    }

    // SECURITY: Prevent non-admins from modifying system-wide or other school's challenges
    if (existing.schoolId !== null && existing.schoolId !== schoolId) {
      return res.status(403).json({
        error: 'محظور',
        message: 'ليس لديك صلاحية تعديل هذا التحدي',
      });
    }
    if (existing.schoolId === null && req.user.role !== 'ADMIN') {
      return res.status(403).json({
        error: 'محظور',
        message: 'يمكن للمسؤول فقط تعديل تحديات النظام',
      });
    }

    await prisma.challenge.delete({ where: { id } });

    logger.info(`Challenge deleted: "${existing.title}" by ${req.user.name}`);

    res.json({
      success: true,
      message: 'تم حذف التحدي بنجاح',
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Publish Challenge                                                        │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function publishChallenge(req, res, next) {
  try {
    const { id } = req.params;
    const { schoolId } = req.user;

    const existing = await prisma.challenge.findUnique({ where: { id } });
    if (!existing) {
      return res.status(404).json({
        error: 'غير موجود',
        message: 'التحدي غير موجود',
      });
    }

    if (existing.schoolId && existing.schoolId !== schoolId) {
      return res.status(403).json({
        error: 'محظور',
        message: 'ليس لديك صلاحية نشر هذا التحدي',
      });
    }

    const challenge = await prisma.challenge.update({
      where: { id },
      data: { published: !existing.published },
      select: { id: true, title: true, published: true },
    });

    res.json({
      success: true,
      message: challenge.published ? 'تم نشر التحدي' : 'تم إلغاء نشر التحدي',
      challenge,
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Set Daily Challenge                                                      │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function setDaily(req, res, next) {
  try {
    const { id } = req.params;
    const { date } = req.body;
    const { schoolId } = req.user;

    const existing = await prisma.challenge.findUnique({ where: { id } });
    if (!existing) {
      return res.status(404).json({
        error: 'غير موجود',
        message: 'التحدي غير موجود',
      });
    }

    if (existing.schoolId && existing.schoolId !== schoolId) {
      return res.status(403).json({
        error: 'محظور',
        message: 'ليس لديك صلاحية تعديل هذا التحدي',
      });
    }

    const dailyDate = date || new Date().toISOString().split('T')[0];

    await prisma.challenge.updateMany({
      where: { dailyDate, id: { not: id } },
      data: { dailyDate: null },
    });

    const challenge = await prisma.challenge.update({
      where: { id },
      data: { dailyDate, published: true },
      select: { id: true, title: true, dailyDate: true, published: true },
    });

    res.json({
      success: true,
      message: `تم تعيين التحدي كتحدي يومي ليوم ${dailyDate}`,
      challenge,
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  Submit Solution                                                          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function submitSolution(req, res, next) {
  try {
    const studentId = req.user.id;
    const { challengeId, code, language, output, executionTime, memoryUsage } = req.body;

    if (!challengeId || !code) {
      return res.status(400).json({
        error: 'خطأ في الطلب',
        message: 'معرف التحدي والكود مطلوبان',
      });
    }

    const challenge = await prisma.challenge.findUnique({ where: { id: challengeId } });
    if (!challenge) {
      return res.status(404).json({
        error: 'غير موجود',
        message: 'التحدي غير موجود',
      });
    }

    // SECURITY: Verify school isolation — student can only submit to own school's or system challenges
    if (challenge.schoolId !== null && challenge.schoolId !== req.user.schoolId) {
      return res.status(403).json({
        error: 'محظور',
        message: 'ليس لديك صلاحية للتقدم لهذا التحدي',
      });
    }

    // SECURITY: Never trust client-provided 'passed'. Verify server-side.
    const normalize = (s) => {
      return (s || '')
        .replace(/\s+/g, ' ')
        .replace(/[\u0610-\u061A\u064B-\u065F\u0670\u06D6-\u06DC\u06DF-\u06E4\u06E7\u06E8\u06EA-\u06ED]/g, '')
        .replace(/[\u200F\u200E\u2066-\u2069]/g, '')
        .trim();
    };

    let isPassed = false;
    const hasExpectedOutput = challenge.expectedOutput && challenge.expectedOutput.trim().length > 0;

    if (hasExpectedOutput) {
      const expected = normalize(challenge.expectedOutput);
      const actual = normalize(output);
      isPassed = expected === actual && code.trim().length > 0;
    } else {
      // No expected output: pass if code is non-empty and contains a print statement
      const hasPrint = /print\s*\(/.test(code) || /طباعة\s*\(/.test(code);
      isPassed = hasPrint && code.trim().length > 0;
    }

    // SECURITY: executionTime and memoryUsage from client are untrusted — store as-is but don't use for grading
    const submission = await prisma.submission.create({
      data: {
        challengeId,
        studentId,
        code,
        language: language || 'daad',
        output: output || '',
        passed: isPassed,
        // SECURITY: Store client-reported metrics but mark as untrusted
        executionTime: executionTime ? Math.min(Number(executionTime), 30000) : null,
        memoryUsage: memoryUsage ? Math.min(Number(memoryUsage), 512 * 1024 * 1024) : null,
      },
      select: { id: true, passed: true, createdAt: true },
    });

    if (isPassed) {
      await prisma.challenge.update({
        where: { id: challengeId },
        data: { popularity: { increment: 1 } },
      });
    }

    res.status(201).json({
      success: true,
      message: isPassed ? 'تم اجتياز التحدي بنجاح!' : 'تم تسجيل المحاولة',
      submission,
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │  List Submissions                                                         │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function listSubmissions(req, res, next) {
  try {
    const { role, id: userId, schoolId } = req.user;
    const { challengeId, studentId, passed } = req.query;

    const whereClause = {};

    if (role === 'STUDENT') {
      whereClause.studentId = userId;
    } else if (role === 'TEACHER' && schoolId) {
      // SECURITY: Teacher can only see submissions from their school
      whereClause.challenge = { schoolId: schoolId };
    }

    if (challengeId) whereClause.challengeId = challengeId;
    if (studentId && role !== 'STUDENT') whereClause.studentId = studentId;
    if (passed !== undefined) whereClause.passed = passed === 'true';

    const submissions = await prisma.submission.findMany({
      where: whereClause,
      select: {
        id: true,
        challengeId: true,
        studentId: true,
        passed: true,
        language: true,
        executionTime: true,
        createdAt: true,
        challenge: { select: { id: true, title: true, difficulty: true } },
        student: { select: { id: true, name: true } },
      },
      orderBy: { createdAt: 'desc' },
      take: 100,
    });

    res.json({
      success: true,
      submissions,
      total: submissions.length,
    });
  } catch (error) {
    next(error);
  }
}

module.exports = {
  listChallenges,
  dailyChallenge,
  createChallenge,
  updateChallenge,
  deleteChallenge,
  publishChallenge,
  setDaily,
  submitSolution,
  listSubmissions,
  verifyChallengeCompletion,
  validateChallengeSecurity,
  storeChallengeVerification,
};