// ========================================================
// Assessment Controller - High-Stakes Exam Engine
// ========================================================

const prisma = require("../utils/prisma");
const { logger } = require("../utils/logger");

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Create Assessment (Teacher Only)                   │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function createAssessment(req, res, next) {
  try {
    const { role, schoolId, id: teacherId } = req.user;
    const {
      classroomId,
      title,
      description,
      allowedTime,
      durationMinutes,
      startTime,
      endTime,
      totalPoints,
      passingScore,
      challengeIds,
      lockdownMode,
      assessmentType,
      questions,
      starterCode,
      expectedOutput,
      instructions
    } = req.body;

    const effectiveAllowedTime = allowedTime || durationMinutes;

    if (role !== "TEACHER" && role !== "ADMIN") {
      return res.status(403).json({
        error: "محظور",
        message: "يمكن للمعلمين فقط إنشاء التقييمات"
      });
    }

    if (!classroomId || !title || !effectiveAllowedTime || !startTime) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف الفصل، العنوان، الوقت المسموح/المدة بالدقائق، ووقت البداية مطلوبة"
      });
    }

    // Validate assessment type
    const VALID_TYPES = ["CODING", "MULTIPLE_CHOICE", "AI_GRADED"];
    if (assessmentType && !VALID_TYPES.includes(assessmentType)) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: `assessmentType must be one of: ${VALID_TYPES.join(', ')}`
      });
    }

    // Validate time range (5 min - 300 min = 5 hours)
    if (effectiveAllowedTime < 5 || effectiveAllowedTime > 300) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "الوقت المسموح يجب أن يكون بين 5 و 300 دقيقة"
      });
    }

    // Validate classroom exists and teacher has access
    const classroom = await prisma.classroom.findUnique({
      where: { id: classroomId },
      include: { school: true }
    });

    if (!classroom) {
      return res.status(404).json({
        error: "غير موجود",
        message: "الفصل غير موجود"
      });
    }

    if (classroom.schoolId !== schoolId && role !== "ADMIN") {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط إنشاء تقييمات لمدرستك"
      });
    }

    if (classroom.teacherId !== teacherId && role !== "ADMIN") {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط إنشاء تقييمات لأفصالك"
      });
    }

    // RATE LIMIT: Max 1 assessment per classroom per 6 hours
    const sixHoursAgo = new Date(Date.now() - 6 * 60 * 60 * 1000);
    const recentCount = await prisma.assessment.count({
      where: {
        classroomId,
        createdAt: { gte: sixHoursAgo }
      }
    });
    if (recentCount >= 1) {
      return res.status(429).json({
        error: "Rate Limit",
        message: "يمكن إنشاء اختبار واحد فقط كل 6 ساعات لكل فصل"
      });
    }

    // Validate dates
    const start = new Date(startTime);
    const computedEndTime = endTime ? new Date(endTime) : new Date(start.getTime() + effectiveAllowedTime * 60000);
    
    if (computedEndTime <= start) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "وقت النهاية يجب أن يكون بعد وقت البداية"
      });
    }

    // Create assessment
    const assessment = await prisma.assessment.create({
      data: {
        classroomId: String(classroomId),
        title: title.trim(),
        description: (description || "").trim(),
        assessmentType: assessmentType || "CODING",
        allowedTime: effectiveAllowedTime,
        startTime: start,
        endTime: computedEndTime,
        totalPoints: totalPoints || 100,
        passingScore: passingScore || 60,
        challengeIds: JSON.stringify(challengeIds || []),
        questions: JSON.stringify(questions || []),
        starterCode: starterCode || "",
        expectedOutput: expectedOutput || "",
        instructions: instructions || "",
        lockdownMode: role === "ADMIN" ? lockdownMode !== false : true,
        isPublished: true
      }
    });

    // Log activity
    await prisma.activityLog.create({
      data: {
        userId: teacherId,
        action: "create_assessment",
        details: JSON.stringify({
          assessmentId: assessment.id,
          classroomId,
          title,
          startTime,
          endTime
        })
      }
    });

    logger.info(`Assessment created: "${title}" for classroom ${classroomId}`);

    res.status(201).json({
      success: true,
      message: "تم إنشاء التقييم بنجاح",
      assessment: {
        id: assessment.id,
        title: assessment.title,
        classroomId: assessment.classroomId,
        assessmentType: assessment.assessmentType,
        allowedTime: assessment.allowedTime,
        startTime: assessment.startTime,
        endTime: assessment.endTime,
        totalPoints: assessment.totalPoints,
        passingScore: assessment.passingScore,
        challengeCount: (challengeIds || []).length,
        questionCount: (questions || []).length,
        isPublished: assessment.isPublished
      }
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Active Assessment for Student                   │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getActiveAssessment(req, res, next) {
  try {
    const { id: studentId, classroomId } = req.user;
    const now = new Date();

    if (!classroomId) {
      return res.json({
        success: true,
        active: false,
        message: "لم يتم تعيين فصل"
      });
    }

    // Find active assessment for student's classroom
    const assessment = await prisma.assessment.findFirst({
      where: {
        classroomId,
        isPublished: true,
        startTime: { lte: now },
        endTime: { gte: now }
      },
      include: {
        classroom: {
          select: { name: true, gradeLevel: true }
        }
      }
    });

    if (!assessment) {
      return res.json({
        success: true,
        active: false,
        message: "لا يوجد تقييم نشط"
      });
    }

    // Check if student already has a result
    let result = await prisma.studentAssessmentResult.findUnique({
      where: {
        assessmentId_studentId: {
          assessmentId: assessment.id,
          studentId
        }
      }
    });

    // Create result if not exists
    if (!result) {
      result = await prisma.studentAssessmentResult.create({
        data: {
          assessmentId: assessment.id,
          studentId,
          totalPointsPossible: assessment.totalPoints,
          status: "IN_PROGRESS"
        }
      });
    }

    // Fetch challenge details
    const challengeIdsArray = JSON.parse(assessment.challengeIds || '[]');
    const challenges = await prisma.challenge.findMany({
      take: 500,
      where: {
        id: { in: challengeIdsArray }
      },
      select: {
        id: true,
        title: true,
        description: true,
        starterCode: true,
        difficulty: true,
        points: true,
        estimatedTime: true
      }
    });

    // Calculate time remaining
    const timeRemaining = Math.max(0, assessment.endTime.getTime() - now.getTime());
    const timeRemainingMinutes = Math.floor(timeRemaining / (1000 * 60));

    // Check if time expired
    if (timeRemaining <= 0 || result.status === "TIMED_OUT") {
      await prisma.studentAssessmentResult.update({
        where: { id: result.id },
        data: { status: "TIMED_OUT" }
      });

      return res.json({
        success: true,
        active: false,
        message: "انتهى وقت التقييم",
        result: {
          id: result.id,
          score: result.score,
          totalPointsEarned: result.totalPointsEarned,
          challengesAttempted: result.challengesAttempted,
          challengesPassed: result.challengesPassed,
          status: "TIMED_OUT"
        }
      });
    }

    // SECURITY: Block if already completed
    if (result.status === "COMPLETED") {
      return res.json({
        success: true,
        active: false,
        message: "لقد أنهيت هذا الامتحان مسبقاً",
        result: {
          id: result.id,
          score: result.score,
          totalPointsEarned: result.totalPointsEarned,
          challengesAttempted: result.challengesAttempted,
          challengesPassed: result.challengesPassed,
          status: "COMPLETED"
        }
      });
    }

    res.json({
      success: true,
      active: true,
      assessment: {
        id: assessment.id,
        title: assessment.title,
        description: assessment.description,
        assessmentType: assessment.assessmentType,
        allowedTime: assessment.allowedTime,
        startTime: assessment.startTime,
        endTime: assessment.endTime,
        totalPoints: assessment.totalPoints,
        passingScore: assessment.passingScore,
        lockdownMode: assessment.lockdownMode,
        questions: (assessment.questions || []).map(function(q) {
          var safe = Object.assign({}, q);
          delete safe.correctIndex;
          return safe;
        }),
        starterCode: assessment.starterCode,
        instructions: assessment.instructions,
        classroom: assessment.classroom
      },
      challenges,
      result: {
        id: result.id,
        startedAt: result.startedAt,
        timeRemaining: timeRemainingMinutes,
        status: result.status
      },
      lockdownInstructions: assessment.lockdownMode ? {
        disableCopyPaste: true,
        disableTabSwitch: true,
        fullscreenMode: true,
        disableRightClick: true,
        showTimer: true,
        autoSubmit: true
      } : null
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Submit Assessment Answer                            │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function submitAssessmentAnswer(req, res, next) {
  try {
    const { id: studentId } = req.user;
    const { assessmentId, challengeId, code } = req.body;

    if (!assessmentId || !challengeId || code === undefined) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف التقييم، معرف التحدي، والكود مطلوبة"
      });
    }

    // SECURITY: Never trust client for passed/output/executionTime/syntaxScore
    // Server executes code in the ISOLATED worker thread (C2-full), same as submissions
    const { executeDhadIsolated } = require("../utils/dhadSandbox");
    const { evaluate } = require("../utils/serverEvaluator");
    const challenge = await prisma.challenge.findUnique({ where: { id: challengeId }, select: { expectedOutput: true, tier: true, title: true, allowedCommands: true, validationRules: true, testCases: true, starterCode: true, description: true, type: true, points: true, inputs: true } });
    // Item-2: same stdin queue as challenge submissions (server-side only, never trust client).
    let assessmentInputs = [];
    try {
      const parsed = JSON.parse((challenge && challenge.inputs) || '[]');
      if (Array.isArray(parsed)) assessmentInputs = parsed.slice(0, 1000);
    } catch (_) {}
    const executionResult = await executeDhadIsolated(code, assessmentInputs);
    const evalResult = evaluate(code, challenge || {}, "", executionResult);
    const passed = evalResult.passed;
    const output = executionResult.stdout ? executionResult.stdout.substring(0, 10000) : "";
    const executionTime = evalResult.executionTime || 0;
    const syntaxScore = evalResult.syntaxScore || 0;

    // Verify assessment is active
    const assessment = await prisma.assessment.findUnique({
      where: { id: assessmentId }
    });

    if (!assessment) {
      return res.status(404).json({
        error: "غير موجود",
        message: "التقييم غير موجود"
      });
    }

    const now = new Date();
    if (now < assessment.startTime || now > assessment.endTime) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "التقييم غير نشط حالياً"
      });
    }

    // Get or create result
    let result = await prisma.studentAssessmentResult.findUnique({
      where: {
        assessmentId_studentId: {
          assessmentId,
          studentId
        }
      }
    });

    if (!result) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "لم تبدأ هذا التقييم"
      });
    }

    if (result.status === "COMPLETED" || result.status === "TIMED_OUT") {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "تم إكمال التقييم مسبقاً"
      });
    }

    // SECURITY: Limit submissions per assessment (max 20)
    if (result.submissionIds && result.submissionIds.length >= 20) {
      return res.status(429).json({
        error: "Rate Limit",
        message: "تم الوصول للحد الأقصى من المحاولات (20)"
      });
    }

    // SECURITY: Rate limit per minute (max 5 submissions in 60s)
    const oneMinuteAgo = new Date(Date.now() - 60000);
    const recentCount = await prisma.submission.count({
      where: {
        studentId,
        challengeId,
        createdAt: { gte: oneMinuteAgo }
      }
    });
    if (recentCount >= 5) {
      return res.status(429).json({
        error: "Rate Limit",
        message: "محاولات كثيرة جداً. انتظر دقيقة قبل المحاولة مرة أخرى."
      });
    }

    // Create submission with server-verified results
    const submission = await prisma.submission.create({
      data: {
        challengeId,
        studentId,
        code,
        language: "daad",
        passed: passed,
        output: output,
        executionTime: executionTime,
        syntaxScore: syntaxScore,
        performanceScore: evalResult.performanceScore || 0
      }
    });

    // Calculate points for this challenge
    const challengePoints = challenge ? challenge.points : 10;
    const safeScore = typeof evalResult.score === 'number' && !isNaN(evalResult.score) ? evalResult.score : 0;
    const pointsEarned = passed ? Math.round(challengePoints * (safeScore / 100)) : 0;

    // Update result atomically with transaction to prevent race conditions
    const updatedResult = await prisma.$transaction(async (tx) => {
      const freshResult = await tx.studentAssessmentResult.findUnique({
        where: { id: result.id }
      });
      const newScore = Math.round(
        ((freshResult.totalPointsEarned + pointsEarned) / freshResult.totalPointsPossible) * 100
      );
      return tx.studentAssessmentResult.update({
        where: { id: result.id },
        data: {
          totalPointsEarned: { increment: pointsEarned },
          challengesAttempted: { increment: 1 },
          challengesPassed: passed ? { increment: 1 } : undefined,
          submissionIds: { push: submission.id },
          score: newScore
        }
      });
    });

    res.json({
      success: true,
      message: "تم إرسال الإجابة",
      submission: {
        id: submission.id,
        passed: submission.passed,
        pointsEarned
      },
      result: {
        totalPointsEarned: updatedResult.totalPointsEarned,
        challengesAttempted: updatedResult.challengesAttempted,
        challengesPassed: updatedResult.challengesPassed,
        currentScore: updatedResult.score
      }
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Start Assessment Attempt (Student - creates result record)                │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function startAssessmentAttempt(req, res, next) {
  try {
    const { id: studentId, classroomId } = req.user;
    const { assessmentId } = req.body;

    if (!assessmentId) {
      return res.status(400).json({ error: "خطأ في الطلب", message: "معرف التقييم مطلوب" });
    }

    const assessment = await prisma.assessment.findUnique({ where: { id: assessmentId } });
    if (!assessment) {
      return res.status(404).json({ error: "غير موجود", message: "التقييم غير موجود" });
    }

    if (assessment.classroomId !== classroomId) {
      return res.status(403).json({ error: "محظور", message: "هذا التقييم ليس لفصلك" });
    }

    const now = new Date();
    if (now < assessment.startTime || now > assessment.endTime) {
      return res.status(400).json({ error: "خطأ في الطلب", message: "التقييم غير نشط حالياً" });
    }

    const existing = await prisma.studentAssessmentResult.findUnique({
      where: { assessmentId_studentId: { assessmentId, studentId } }
    });

    if (existing && (existing.status === "COMPLETED" || existing.status === "TIMED_OUT")) {
      return res.status(403).json({ error: "محظور", message: "لقد أنهيت هذا الامتحان مسبقاً" });
    }

    if (existing) {
      return res.json({ success: true, result: { id: existing.id, status: existing.status } });
    }

    let result;
    try {
      result = await prisma.studentAssessmentResult.create({
        data: {
          assessmentId,
          studentId,
          totalPointsPossible: assessment.totalPoints,
          status: "IN_PROGRESS"
        }
      });
    } catch (err) {
      if (err.code === 'P2002') {
        result = await prisma.studentAssessmentResult.findUnique({
          where: { assessmentId_studentId: { assessmentId, studentId } }
        });
        if (result) {
          return res.json({ success: true, result: { id: result.id, status: result.status } });
        }
      }
      throw err;
    }

    res.json({ success: true, result: { id: result.id, status: result.status } });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Complete Assessment (Student)                                             │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function completeAssessment(req, res, next) {
  try {
    const { id: studentId } = req.user;
    const { assessmentId, mcAnswers } = req.body;

    if (!assessmentId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف التقييم مطلوب"
      });
    }

    const result = await prisma.studentAssessmentResult.findUnique({
      where: {
        assessmentId_studentId: {
          assessmentId,
          studentId
        }
      }
    });

    if (!result) {
      return res.status(404).json({
        error: "غير موجود",
        message: "نتيجة التقييم غير موجودة"
      });
    }

    if (result.status === "COMPLETED" || result.status === "TIMED_OUT") {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: result.status === "COMPLETED" ? "تم إكمال التقييم مسبقاً" : "انتهى وقت التقييم"
      });
    }

    // Fetch assessment
    const assessment = await prisma.assessment.findUnique({
      where: { id: assessmentId },
      select: { passingScore: true, classroomId: true, questions: true, endTime: true, totalPoints: true }
    });

    // SECURITY: Verify classroom
    if (assessment && assessment.classroomId !== req.user.classroomId) {
      return res.status(403).json({ error: "محظور", message: "هذا التقييم ليس لفصلك" });
    }

    // SECURITY: Enforce time window
    if (assessment && assessment.endTime && new Date() > new Date(assessment.endTime)) {
      return res.status(400).json({ error: "خطأ في الطلب", message: "انتهى وقت التقييم" });
    }

    // SERVER-SIDE MC GRADING
    let mcPointsEarned = 0;
    if (mcAnswers && Array.isArray(mcAnswers) && assessment && assessment.questions) {
      const questions = assessment.questions;
      const pointsPerQ = Math.round((assessment.totalPoints || 100) / questions.length);
      for (var qi = 0; qi < questions.length; qi++) {
        if (mcAnswers[qi] !== undefined && mcAnswers[qi] === questions[qi].correctIndex) {
          mcPointsEarned += pointsPerQ;
        }
      }
    }

    const totalEarned = result.totalPointsEarned + mcPointsEarned;
    const totalPossible = result.totalPointsPossible || assessment?.totalPoints || 100;
    const finalScore = totalPossible > 0 ? Math.round((totalEarned / totalPossible) * 100) : 0;

    const completedResult = await prisma.studentAssessmentResult.update({
      where: { id: result.id },
      data: {
        score: finalScore,
        totalPointsEarned: { increment: mcPointsEarned },
        completedAt: new Date(),
        status: "COMPLETED"
      }
    });

    res.json({
      success: true,
      message: "تم إكمال التقييم",
      result: {
        id: completedResult.id,
        score: finalScore,
        totalPointsEarned: completedResult.totalPointsEarned,
        totalPointsPossible: totalPossible,
        challengesAttempted: completedResult.challengesAttempted,
        challengesPassed: completedResult.challengesPassed,
        timeSpent: completedResult.timeSpent,
        status: completedResult.status,
        passed: finalScore >= (assessment?.passingScore || 60)
      }
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Assessment Results (Teacher View)               │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getAssessmentResults(req, res, next) {
  try {
    const { role, schoolId, id: userId } = req.user;
    const { assessmentId } = req.params;

    if (!assessmentId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف التقييم مطلوب"
      });
    }

    // Get assessment with classroom
    const assessment = await prisma.assessment.findUnique({
      where: { id: assessmentId },
      include: {
        classroom: {
          include: {
            school: { select: { id: true, name: true } }
          }
        }
      }
    });

    if (!assessment) {
      return res.status(404).json({
        error: "غير موجود",
        message: "التقييم غير موجود"
      });
    }

    // Check access
    if (assessment.classroom.schoolId !== schoolId && role !== "ADMIN") {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط مشاهدة النتائج لمدرستك"
      });
    }

    // Get all results
    const [results, totalResults] = await Promise.all([
      prisma.studentAssessmentResult.findMany({
        where: { assessmentId },
        take: 200,
        skip: parseInt(req.query.offset) || 0,
        include: {
          student: {
            select: {
              id: true,
              name: true,
              email: true,
              nationalId: true
            }
          }
        },
        orderBy: { score: 'desc' }
      }),
      prisma.studentAssessmentResult.count({ where: { assessmentId } })
    ]);

    // Calculate statistics
    const scores = results.map(r => r.score).filter(s => s !== null);
    const avgScore = scores.length > 0 ? Math.round(scores.reduce((a, b) => a + b, 0) / scores.length) : 0;
    const passCount = scores.filter(s => s >= assessment.passingScore).length;
    const passRate = scores.length > 0 ? Math.round((passCount / scores.length) * 100) : 0;

    res.json({
      success: true,
      assessment: {
        id: assessment.id,
        title: assessment.title,
        classroom: assessment.classroom,
        totalPoints: assessment.totalPoints,
        passingScore: assessment.passingScore,
        startTime: assessment.startTime,
        endTime: assessment.endTime
      },
      statistics: {
        totalStudents: totalResults,
        averageScore: avgScore,
        highestScore: scores.length > 0 ? scores.reduce((a, b) => Math.max(a, b), -Infinity) : 0,
        lowestScore: scores.length > 0 ? scores.reduce((a, b) => Math.min(a, b), Infinity) : 0,
        passCount,
        passRate
      },
      results: results.map(r => ({
        id: r.id,
        student: r.student,
        score: r.score,
        totalPointsEarned: r.totalPointsEarned,
        challengesAttempted: r.challengesAttempted,
        challengesPassed: r.challengesPassed,
        status: r.status,
        startedAt: r.startedAt,
        completedAt: r.completedAt
      }))
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Export Classroom Report (CSV)                       │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function exportClassroomReport(req, res, next) {
  try {
    const { role, schoolId } = req.user;
    const { classId } = req.params;

    if (!classId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف الفصل مطلوب"
      });
    }

    // Get classroom with students
    const classroom = await prisma.classroom.findUnique({
      where: { id: classId },
      include: {
        school: { select: { id: true, name: true } },
        teacher: { select: { id: true, name: true } }
      }
    });

    if (!classroom) {
      return res.status(404).json({
        error: "غير موجود",
        message: "الفصل غير موجود"
      });
    }

    if (classroom.schoolId !== schoolId && role !== "ADMIN") {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط تصدير التقارير لمدرستك"
      });
    }

    // Get all students in classroom (lightweight — no nested includes)
    const students = await prisma.user.findMany({
      take: 200,
      where: {
        classroomId: classId,
        role: "STUDENT"
      },
      select: {
        id: true,
        name: true,
        email: true,
        nationalId: true,
        createdAt: true,
        studentProfile: {
          select: {
            totalXP: true,
            currentLevel: true,
            currentStreak: true
          }
        },
      }
    });

    // SECURITY: Batch aggregation queries instead of loading all child records
    const studentIds = students.map(s => s.id);
    const [submissionStats, attendanceStats, assessmentStats, securityAlerts] = await Promise.all([
      prisma.submission.groupBy({
        by: ['studentId', 'passed'],
        where: { studentId: { in: studentIds } },
        _count: true,
        _avg: { syntaxScore: true },
      }).catch(() => []),
      prisma.attendance.groupBy({
        by: ['studentId', 'status'],
        where: { studentId: { in: studentIds } },
        _count: true,
      }).catch(() => []),
      prisma.studentAssessmentResult.groupBy({
        by: ['studentId'],
        where: { studentId: { in: studentIds } },
        _avg: { score: true },
      }).catch(() => []),
      prisma.securityAlert.findMany({
        take: 200,
        where: { studentId: { in: studentIds }, resolved: false },
        select: { studentId: true },
      }).catch(() => []),
    ]);

    // Index batch results by studentId
    const subStats = {};
    for (const row of submissionStats) {
      if (!subStats[row.studentId]) subStats[row.studentId] = { total: 0, passed: 0, totalScore: 0, scoreCount: 0 };
      subStats[row.studentId].total += row._count;
      if (row.passed) subStats[row.studentId].passed = row._count;
      if (row._avg.syntaxScore) {
        subStats[row.studentId].totalScore += row._avg.syntaxScore * row._count;
        subStats[row.studentId].scoreCount += row._count;
      }
    }
    const attStats = {};
    for (const row of attendanceStats) {
      if (!attStats[row.studentId]) attStats[row.studentId] = { total: 0, present: 0 };
      attStats[row.studentId].total += row._count;
      if (row.status === "PRESENT") attStats[row.studentId].present = row._count;
    }
    const assessStats = {};
    for (const row of assessmentStats) {
      assessStats[row.studentId] = Math.round(row._avg.score || 0);
    }
    const alertCount = {};
    for (const alert of securityAlerts) {
      alertCount[alert.studentId] = (alertCount[alert.studentId] || 0) + 1;
    }

    // Build CSV data
    const csvRows = [];
    csvRows.push([
      'Student ID',
      'Student Name',
      'Email',
      'National ID',
      'Total XP',
      'Current Level',
      'Current Streak',
      'Total Submissions',
      'Passed Submissions',
      'Pass Rate (%)',
      'Average Score',
      'Attendance Rate (%)',
      'Assessment Average',
      'Security Alerts'
    ].join(','));

    for (const student of students) {
      const profile = student.studentProfile || { totalXP: 0, currentLevel: 1, currentStreak: 0 };
      const sub = subStats[student.id] || { total: 0, passed: 0, totalScore: 0, scoreCount: 0 };
      const att = attStats[student.id] || { total: 0, present: 0 };
      const assessmentAvg = assessStats[student.id] || 0;
      const alerts = alertCount[student.id] || 0;
      
      const passRate = sub.total > 0 ? Math.round((sub.passed / sub.total) * 100) : 0;
      const avgScore = sub.scoreCount > 0 ? Math.round(sub.totalScore / sub.scoreCount) : 0;
      const attendanceRate = att.total > 0 ? Math.round((att.present / att.total) * 100) : 100;

      // Sanitize CSV fields to prevent injection
      const sanitizeCSV = (val) => {
        if (typeof val !== 'string') return val;
        // Escape double quotes and prefix dangerous characters
        const escaped = val.replace(/"/g, '""');
        if (/^[=+\-@\t\r]/.test(escaped)) {
          return `"'"${escaped}"`;
        }
        return `"${escaped}"`;
      };

      csvRows.push([
        student.id,
        sanitizeCSV(student.name),
        sanitizeCSV(student.email || ''),
        student.nationalId || '',
        profile.totalXP,
        profile.currentLevel,
        profile.currentStreak,
        sub.total,
        sub.passed,
        passRate,
        avgScore,
        attendanceRate,
        assessmentAvg,
        alerts
      ].join(','));
    }

    const csvContent = csvRows.join('\n');

    // Log export activity
    await prisma.activityLog.create({
      data: {
        userId: req.user.id,
        action: "export_classroom_report",
        details: JSON.stringify({
          classroomId: classId,
          studentCount: students.length,
          format: 'CSV'
        })
      }
    });

    res.setHeader('Content-Type', 'text/csv; charset=utf-8');
    res.setHeader('Content-Disposition', `attachment; filename="classroom_report_${classroom.name}_${new Date().toISOString().split('T')[0]}.csv"`);
    res.send(csvContent);
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Classroom Assessments (Teacher View)            │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getClassroomAssessments(req, res, next) {
  try {
    const { role, schoolId, id: userId } = req.user;
    const { classId } = req.params;

    if (!classId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف الفصل مطلوب"
      });
    }

    // Verify access
    const classroom = await prisma.classroom.findUnique({
      where: { id: classId }
    });

    if (!classroom) {
      return res.status(404).json({
        error: "غير موجود",
        message: "الفصل غير موجود"
      });
    }

    if (classroom.schoolId !== schoolId && role !== "ADMIN") {
      return res.status(403).json({
        error: "محظور",
        message: "تم رفض الوصول"
      });
    }

    // Get assessments
    const assessments = await prisma.assessment.findMany({
      take: 200,
      where: { classroomId: classId },
      include: {
        results: {
          select: {
            id: true,
            score: true,
            status: true
          }
        }
      },
      orderBy: { startTime: 'desc' }
    });

    // Calculate statistics for each assessment
    const assessmentsWithStats = assessments.map(a => {
      const scores = a.results.map(r => r.score).filter(s => s !== null);
      const completedCount = a.results.filter(r => r.status === 'COMPLETED').length;
      
      return {
        id: a.id,
        title: a.title,
        description: a.description,
        allowedTime: a.allowedTime,
        startTime: a.startTime,
        endTime: a.endTime,
        totalPoints: a.totalPoints,
        passingScore: a.passingScore,
        isPublished: a.isPublished,
        challengeCount: a.challengeIds.length,
        statistics: {
          totalStudents: a.results.length,
          completedCount,
          averageScore: scores.length > 0 ? Math.round(scores.reduce((a, b) => a + b, 0) / scores.length) : 0,
          passRate: scores.length > 0 ? 
            Math.round((scores.filter(s => s >= a.passingScore).length / scores.length) * 100) : 0
        }
      };
    });

    res.json({
      success: true,
      classroom: {
        id: classroom.id,
        name: classroom.name,
        gradeLevel: classroom.gradeLevel
      },
      assessments: assessmentsWithStats,
      total: assessments.length
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Student Assessments (All assessments for student's classroom)        │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getStudentAssessments(req, res, next) {
  try {
    const { id: studentId, classroomId } = req.user;

    if (!classroomId) {
      return res.json({ success: true, assessments: [], total: 0 });
    }

    const now = new Date();

    const assessments = await prisma.assessment.findMany({
      where: {
        classroomId,
        isPublished: true,
      },
      take: 100,
      select: {
        id: true,
        title: true,
        description: true,
        assessmentType: true,
        allowedTime: true,
        startTime: true,
        endTime: true,
        totalPoints: true,
        passingScore: true,
        isPublished: true,
        questions: true,
        starterCode: true,
        instructions: true,
      },
      orderBy: { startTime: 'desc' }
    });

    // Get student's existing results for these assessments
    const assessmentIds = assessments.map(a => a.id);
    const existingResults = await prisma.studentAssessmentResult.findMany({
      take: 100,
      where: {
        assessmentId: { in: assessmentIds },
        studentId
      },
      select: {
        assessmentId: true,
        status: true,
        score: true,
        totalPointsEarned: true
      }
    });
    const resultMap = {};
    existingResults.forEach(r => { resultMap[r.assessmentId] = r; });

    const assessmentsWithStatus = assessments.map(a => {
      let status = 'upcoming';
      if (now >= a.startTime && now <= a.endTime) status = 'active';
      else if (now > a.endTime) status = 'completed';
      const result = resultMap[a.id] || null;
      const hasSubmitted = !!(result && (result.status === 'COMPLETED' || result.status === 'TIMED_OUT'));
      return { ...a, status, hasSubmitted, studentResult: result };
    });

    res.json({
      success: true,
      assessments: assessmentsWithStatus,
      total: assessments.length
    });
  } catch (error) {
    next(error);
  }
}

module.exports = {
  createAssessment,
  getActiveAssessment,
  getStudentAssessments,
  submitAssessmentAnswer,
  startAssessmentAttempt,
  completeAssessment,
  getAssessmentResults,
  exportClassroomReport,
  getClassroomAssessments
};
