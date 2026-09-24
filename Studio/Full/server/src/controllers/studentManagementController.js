/* Developer: محمد محمود الحموز | Dhad Studio */
const bcrypt = require("bcryptjs");
const jwt = require("jsonwebtoken");
const { isValidEmail } = require("../middlewares/security");
const { recordFailedAttempt, clearFailedAttempts, validatePasswordStrength, isAccountLocked } = require("../middlewares/strictSecurity");
const { logger } = require("../utils/logger");

const BCRYPT_ROUNDS = 12;

// ── Teacher Creates a Student Account ──────────────────────────────────────
async function createStudent(req, res, next) {
  try {
    const { name, classroomId, nationalId, password } = req.body;
    const { schoolId, id: teacherId, role } = req.user;

    // Validation
    if (!name || name.trim().length === 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "اسم الطالب مطلوب",
      });
    }

    if (name.length > 100) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "الاسم طويل جداً (100 حرف كحد أقصى)",
      });
    }

    const prisma = req.app.get("prisma");

    // Determine classroom
    let targetClassroomId = classroomId;

    // If no classroom specified, try to find one owned by this teacher
    if (!targetClassroomId) {
      const classroom = await prisma.classroom.findFirst({
        where: {
          schoolId,
          teacherId: teacherId,
        },
        orderBy: { createdAt: "desc" },
      });

      if (classroom) {
        targetClassroomId = classroom.id;
      }
    }

    // Verify classroom belongs to same school
    if (targetClassroomId) {
      const classroom = await prisma.classroom.findUnique({
        where: { id: targetClassroomId },
      });

      if (!classroom || classroom.schoolId !== schoolId) {
        return res.status(403).json({
          error: "محظور",
          message: "لا يمكن إضافة طلاب لفصل في مدرسة أخرى",
        });
      }
    }

    // Check for duplicate name in same school/classroom
    const existing = await prisma.user.findFirst({
      where: {
        schoolId,
        role: "STUDENT",
        name: name.trim(),
        ...(targetClassroomId ? { classroomId: targetClassroomId } : {}),
      },
    });

    if (existing) {
      return res.status(409).json({
        error: "تعارض",
        message: "يوجد طالب بنفس الاسم في هذا الفصل",
      });
    }

    // Generate username: student name without spaces + school code
    const school = await prisma.school.findUnique({ where: { id: schoolId } });
    const schoolCode = school?.code || schoolId.slice(-4);
    const baseUsername = name.trim().replace(/\s+/g, "").toLowerCase();
    const username = `${baseUsername}${schoolCode}`;

    // Require password from teacher
    if (!password) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور مطلوبة للطالب",
      });
    }

    // Validate password strength
    const passwordErrors = validatePasswordStrength(password);
    if (passwordErrors.length > 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور ضعيفة جداً",
        details: passwordErrors,
      });
    }

    const passwordHash = await bcrypt.hash(password, BCRYPT_ROUNDS);

    // Create student
    const student = await prisma.user.create({
      data: {
        schoolId,
        classroomId: targetClassroomId || null,
        role: "STUDENT",
        name: name.trim(),
        nationalId: nationalId || username, // Use username as fallback
        email: null,
        passwordHash,
        isActive: true,
        isVerified: true,
        isApproved: true,
      },
      include: {
        classroom: { select: { id: true, name: true } },
      },
    });

    // Create StudentProfile
    await prisma.studentProfile.create({
      data: {
        studentId: student.id,
        totalXP: 0,
        currentLevel: 1,
        currentStreak: 0,
      },
    });

    res.status(201).json({
      success: true,
      message: `تم إنشاء حساب الطالب "${student.name}" بنجاح`,
      student: {
        id: student.id,
        name: student.name,
        username: username,
        classroom: student.classroom,
        createdAt: student.createdAt,
      },
      credentials: {
        username: username,
        note: "تم تعيين كلمة المرور من المعلم",
      },
    });
  } catch (err) {
    next(err);
  }
}

// ── Teacher Bulk Creates Students ──────────────────────────────────────────
async function bulkCreateStudents(req, res, next) {
  try {
    const { students, classroomId, password } = req.body;
    const { schoolId, id: teacherId } = req.user;

    if (!Array.isArray(students) || students.length === 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "قائمة الطلاب مطلوبة",
      });
    }

    if (students.length > 50) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "50 طالب كحد أقصى في المرة الواحدة",
      });
    }

    const prisma = req.app.get("prisma");

    // Determine classroom
    let targetClassroomId = classroomId;
    if (!targetClassroomId) {
      const classroom = await prisma.classroom.findFirst({
        where: {
          schoolId,
          OR: [{ teacherId }, { teacherId: null }],
        },
        orderBy: { createdAt: "desc" },
      });
      if (classroom) targetClassroomId = classroom.id;
    }

    // Require password for bulk creation
    if (!password) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور مطلوبة لجميع الطلاب",
      });
    }

    // Validate password strength
    const passwordErrors = validatePasswordStrength(password);
    if (passwordErrors.length > 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور ضعيفة جداً",
        details: passwordErrors,
      });
    }

    const school = await prisma.school.findUnique({ where: { id: schoolId } });
    const schoolCode = school?.code || schoolId.slice(-4);
    const passwordHash = await bcrypt.hash(password, BCRYPT_ROUNDS);

    const results = {
      successful: [],
      failed: [],
      total: students.length,
    };

    // Use transaction for atomicity
    await prisma.$transaction(async (tx) => {
      for (const item of students) {
        const name = typeof item === "string" ? item.trim() : item.name?.trim();

        if (!name || name.length === 0) {
          results.failed.push({ name: item, reason: "اسم فارغ" });
          continue;
        }

        try {
          // Check duplicate
          const existing = await tx.user.findFirst({
            where: {
              schoolId,
              role: "STUDENT",
              name: name,
              ...(targetClassroomId ? { classroomId: targetClassroomId } : {}),
            },
          });

          if (existing) {
            results.failed.push({ name, reason: "اسم مكرر" });
            continue;
          }

          const baseUsername = name.replace(/\s+/g, "").toLowerCase();
          const username = `${baseUsername}${schoolCode}`;

          const student = await tx.user.create({
            data: {
              schoolId,
              classroomId: targetClassroomId || null,
              role: "STUDENT",
              name,
              nationalId: username,
              passwordHash,
              isActive: true,
              isVerified: true,
              isApproved: true,
            },
          });

          await tx.studentProfile.create({
            data: {
              studentId: student.id,
              totalXP: 0,
              currentLevel: 1,
              currentStreak: 0,
            },
          });

          results.successful.push({
            id: student.id,
            name: student.name,
            username,
          });
        } catch (err) {
          results.failed.push({ name, reason: err.message });
        }
      }
    });

    res.status(201).json({
      success: true,
      message: `تم إنشاء ${results.successful.length} من ${results.total} طالب`,
      results,
    });
  } catch (err) {
    next(err);
  }
}

// ── Student Lookup (for Student Login) ─────────────────────────────────────
// GET /api/v1/auth/classrooms - Get all classrooms (public for student login)
async function getPublicClassrooms(req, res, next) {
  try {
    const prisma = req.app.get("prisma");

    const classrooms = await prisma.classroom.findMany({
      take: 100, // SECURITY: Prevent unbounded enumeration
      select: {
        id: true,
        name: true,
        gradeLevel: true,
        school: {
          select: { id: true, name: true },
        },
        _count: {
          select: { students: true },
        },
      },
      orderBy: { name: "asc" },
    });

    res.json({
      success: true,
      classrooms: classrooms.map((c) => ({
        id: c.id,
        name: c.name,
        school: c.school.name,
        studentCount: c._count.students,
      })),
    });
  } catch (err) {
    next(err);
  }
}

// GET /api/v1/auth/classrooms/:classroom_id/students - Get students in classroom (for login selection)
async function getClassroomStudents(req, res, next) {
  try {
    const { classroom_id } = req.params;
    const prisma = req.app.get("prisma");

    const students = await prisma.user.findMany({
      where: {
        classroomId: classroom_id,
        role: "STUDENT",
        isActive: true,
      },
      select: {
        id: true,
        name: true,
      },
      orderBy: { name: "asc" },
      take: 100
    });

    res.json({
      success: true,
      students: students.map((s) => ({
        id: s.id,
        name: s.name,
      })),
    });
  } catch (err) {
    next(err);
  }
}

// ── Student Login by Classroom + Name ──────────────────────────────────────
async function studentLogin(req, res, next) {
  try {
    const { classroomId, studentId, password } = req.body;

    if (!classroomId || !studentId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "classroomId و studentId مطلوبان",
      });
    }

    if (!password) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور مطلوبة",
      });
    }

    const prisma = req.app.get("prisma");
    const ip = req.ip || req.connection?.remoteAddress || "unknown";

    // Find student with school isolation
    const student = await prisma.user.findFirst({
      where: {
        id: studentId,
        classroomId: classroomId,
        role: "STUDENT",
      },
      include: {
        school: { select: { id: true, name: true } },
        classroom: { select: { id: true, name: true, schoolId: true } },
      },
    });

    if (!student) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "الطالب غير موجود",
      });
    }

    // Verify classroom belongs to the same school
    if (student.classroom && student.classroom.schoolId !== student.schoolId) {
      return res.status(401).json({
        error: "غير مصرح",
        message: "الفصل غير متوافق مع المدرسة",
      });
    }

    // Check if student is active
    if (!student.isActive) {
      return res.status(403).json({
        error: "محظور",
        message: "الحساب غير مفعل. يرجى تفعيل الحساب أولاً",
      });
    }

    // Check if account is locked
    const accountLocked = await isAccountLocked(studentId);
    if (accountLocked) {
      return res.status(423).json({
        error: "محظور",
        message: "الحساب مقفل بسبب محاولات كثيرة. يرجى المحاولة لاحقاً",
      });
    }

    // Verify password
    const validPassword = await bcrypt.compare(password, student.passwordHash);
    if (!validPassword) {
      await recordFailedAttempt(studentId);
      return res.status(401).json({
        error: "غير مصرح",
        message: "كلمة المرور غير صحيحة",
      });
    }

    // Clear failed attempts on successful login
    await clearFailedAttempts(studentId);

    // Increment tokenVersion to invalidate old sessions (same as admin login)
    const updatedStudent = await prisma.user.update({
      where: { id: student.id },
      data: { tokenVersion: { increment: 1 } },
      select: { tokenVersion: true }
    });

    // Log login, activity, and mark attendance in parallel
    await Promise.all([
      prisma.loginLog
        .create({
          data: {
            userId: student.id,
            ip: ip,
            userAgent: req.headers["user-agent"] || "",
            success: true,
          },
        })
        .catch(() => {}),
      prisma.activityLog.create({
        data: {
          userId: student.id,
          action: 'LOGIN',
          details: JSON.stringify({ ip: ip || "" })
        }
      }).catch(() => {})
    ]);

    // Auto-mark attendance for today
    const todayStart = new Date();
    todayStart.setHours(0, 0, 0, 0);
    const todayEnd = new Date();
    todayEnd.setHours(23, 59, 59, 999);
    await prisma.attendance.upsert({
      where: {
        studentId_date: {
          studentId: student.id,
          date: todayStart,
        },
      },
      update: { status: "PRESENT" },
      create: {
        studentId: student.id,
        schoolId: student.schoolId,
        date: todayStart,
        status: "PRESENT",
      },
    }).catch(() => {});

    const tokenPayload = {
      id: student.id,
      schoolId: student.schoolId,
      classroomId: student.classroomId,
      role: student.role,
      name: student.name,
      tokenVersion: updatedStudent.tokenVersion,
    };

    // Access token (short-lived)
    const accessToken = jwt.sign(tokenPayload, process.env.JWT_SECRET, {
      algorithm: "HS256",
      expiresIn: process.env.JWT_ACCESS_EXPIRES || "1h",
    });

    // Refresh token (long-lived) - uses separate secret
    const refreshToken = jwt.sign(
      { id: student.id, type: "refresh", tokenVersion: updatedStudent.tokenVersion },
      process.env.JWT_REFRESH_SECRET || process.env.JWT_SECRET,
      {
        algorithm: "HS256",
        expiresIn: process.env.JWT_REFRESH_EXPIRES || "7d",
      }
    );

    // Set httpOnly cookies (same as admin login)
    const cookieOpts = {
      httpOnly: true,
      secure: process.env.NODE_ENV === "production",
      sameSite: "lax",
      path: "/",
    };
    res.cookie("access_token", accessToken, { ...cookieOpts, maxAge: 3600 * 1000 });
    res.cookie("refresh_token", refreshToken, { ...cookieOpts, maxAge: 7 * 24 * 3600 * 1000 });

    res.json({
      success: true,
      expiresIn: 3600,
      userId: student.id,
      username: student.name,
      role: student.role,
      profile: {
        id: student.id,
        name: student.name,
        role: student.role,
        classroom: student.classroom,
        school: student.school,
      },
    });
  } catch (err) {
    next(err);
  }
}

// ── Teacher: List Students in Classroom ────────────────────────────────────
async function getTeacherStudents(req, res, next) {
  try {
    const { schoolId, id: teacherId } = req.user;
    const { classroomId } = req.query;
    const prisma = req.app.get("prisma");

    // Get the teacher's own classrooms
    const teacherClassrooms = await prisma.classroom.findMany({
      take: 50,
      where: { teacherId, schoolId },
      select: { id: true },
    });
    const teacherClassroomIds = teacherClassrooms.map((c) => c.id);

    // If a specific classroom filter is requested, verify it belongs to the teacher
    if (classroomId && !teacherClassroomIds.includes(classroomId)) {
      return res.status(403).json({
        success: false,
        message: "لا يمكن الوصول لهذا الفصل",
      });
    }

    const where = {
      schoolId,
      role: "STUDENT",
      isActive: true,
      classroomId: { in: teacherClassroomIds },
    };

    if (classroomId) {
      where.classroomId = classroomId;
    }

    const students = await prisma.user.findMany({
      take: 200,
      where,
      select: {
        id: true,
        name: true,
        nationalId: true,
        classroomId: true,
        isActive: true,
        createdAt: true,
        classroom: { select: { id: true, name: true } },
        studentProfile: {
          select: { totalXP: true, currentLevel: true, currentStreak: true },
        },
      },
      orderBy: { name: "asc" },
    });

    res.json({
      success: true,
      students: students.map((s) => ({
        id: s.id,
        name: s.name,
        classroom: s.classroom,
        xp: s.studentProfile?.totalXP || 0,
        level: s.studentProfile?.currentLevel || 1,
        streak: s.studentProfile?.currentStreak || 0,
        isActive: s.isActive,
        createdAt: s.createdAt,
      })),
      total: students.length,
    });
  } catch (err) {
    next(err);
  }
}

// ── Teacher: Delete Student ────────────────────────────────────────────────
async function deleteStudent(req, res, next) {
  try {
    const { id } = req.params;
    const { schoolId, id: teacherId } = req.user;
    const prisma = req.app.get("prisma");

    // Get teacher's classrooms
    const teacherClassrooms = await prisma.classroom.findMany({
      take: 50,
      where: { teacherId, schoolId },
      select: { id: true },
    });
    const teacherClassroomIds = teacherClassrooms.map((c) => c.id);

    const student = await prisma.user.findUnique({
      where: { id },
      select: { schoolId: true, role: true, name: true, classroomId: true },
    });

    if (!student) {
      return res.status(404).json({
        error: "غير موجود",
        message: "الطالب غير موجود",
      });
    }

    if (student.schoolId !== schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "لا يمكن حذف طالب من مدرسة أخرى",
      });
    }

    if (!teacherClassroomIds.includes(student.classroomId)) {
      return res.status(403).json({
        error: "محظور",
        message: "لا يمكنك حذف طالب من فصل يخص معلماً آخر",
      });
    }

    // Soft delete
    await prisma.user.update({
      where: { id },
      data: { isActive: false },
    });

    logger.info(`Student "${student.name}" deactivated by ${req.user.name}`);

    res.json({
      success: true,
      message: `تم حذف الطالب "${student.name}"`,
    });
  } catch (err) {
    next(err);
  }
}

// ── Teacher: Assign Student to Classroom ───────────────────────────────────
async function assignStudentToClassroom(req, res, next) {
  try {
    const { studentId, classroomId } = req.body;
    const { schoolId, id: teacherId } = req.user;
    const prisma = req.app.get("prisma");

    if (!studentId || !classroomId) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "studentId و classroomId مطلوبان",
      });
    }

    // Verify student belongs to same school
    const student = await prisma.user.findUnique({
      where: { id: studentId },
      select: { schoolId: true, name: true },
    });

    if (!student || student.schoolId !== schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "الطالب غير موجود في مدرستك",
      });
    }

    // Verify classroom belongs to this teacher
    const classroom = await prisma.classroom.findUnique({
      where: { id: classroomId },
      select: { schoolId: true, name: true, teacherId: true },
    });

    if (!classroom || classroom.schoolId !== schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "الفصل غير موجود في مدرستك",
      });
    }

    if (classroom.teacherId !== teacherId) {
      return res.status(403).json({
        error: "محظور",
        message: "لا يمكنك نقل طالب إلى فصل يخص معلماً آخر",
      });
    }

    await prisma.user.update({
      where: { id: studentId },
      data: { classroomId },
    });

    res.json({
      success: true,
      message: `تم نقل الطالب "${student.name}" إلى فصل "${classroom.name}"`,
    });
  } catch (err) {
    next(err);
  }
}

// ── Teacher: Get Student Activity Details ──────────────────────────────────
async function getStudentActivity(req, res, next) {
  try {
    const { id } = req.params;
    const { schoolId } = req.user;
    const prisma = req.app.get("prisma");

    const student = await prisma.user.findFirst({
      where: { id, schoolId, role: "STUDENT" },
      select: {
        id: true,
        name: true,
        nationalId: true,
        classroomId: true,
        classroom: { select: { id: true, name: true } },
        studentProfile: {
          select: { totalXP: true, currentLevel: true, currentStreak: true, longestStreak: true, lastActivityDate: true },
        },
      },
    });

    if (!student) {
      return res.status(404).json({ error: "غير موجود", message: "الطالب غير موجود" });
    }

    const totalSubmissions = await prisma.submission.count({ where: { studentId: id } });
    const passedSubmissions = await prisma.submission.count({ where: { studentId: id, passed: true } });

    // Attendance this month
    const monthStart = new Date();
    monthStart.setDate(1); monthStart.setHours(0,0,0,0);
    const attendanceCount = await prisma.attendance.count({
      where: { studentId: id, date: { gte: monthStart }, status: "PRESENT" },
    });
    const totalAttendance = await prisma.attendance.count({
      where: { studentId: id, date: { gte: monthStart } },
    });

    // Areas worked (courses from submissions via challenges)
    const courseWork = await prisma.submission.findMany({
      take: 200,
      where: { studentId: id },
      distinct: ['challengeId'],
      select: {
        challenge: {
          select: { courseId: true, challengeType: true, tier: true },
        },
      },
    });

    const courseIds = [...new Set(courseWork.filter(s => s.challenge?.courseId).map(s => s.challenge.courseId))];
    const courses = courseIds.length > 0
      ? await prisma.course.findMany({ take: 200, where: { id: { in: courseIds } }, select: { id: true, title: true } })
      : [];
    const challengeTypes = [...new Set(courseWork.filter(s => s.challenge?.challengeType).map(s => s.challenge.challengeType))];
    const tiers = [...new Set(courseWork.filter(s => s.challenge?.tier).map(s => s.challenge.tier))];

    res.json({
      success: true,
      student: {
        id: student.id,
        name: student.name,
        nationalId: student.nationalId,
        classroom: student.classroom,
        xp: student.studentProfile?.totalXP || 0,
        level: student.studentProfile?.currentLevel || 1,
        streak: student.studentProfile?.currentStreak || 0,
        longestStreak: student.studentProfile?.longestStreak || 0,
        lastActive: student.studentProfile?.lastActivityDate || null,
        submissions: { total: totalSubmissions, passed: passedSubmissions, passRate: totalSubmissions > 0 ? Math.round((passedSubmissions / totalSubmissions) * 100) : 0 },
        attendance: totalAttendance > 0 ? Math.round((attendanceCount / totalAttendance) * 100) : 0,
        areas: {
          courses: courses.map(c => ({ id: c.id, title: c.title })),
          challengeTypes,
          tiers,
        },
      },
    });
  } catch (err) {
    next(err);
  }
}

// ── Teacher: Export Students Report ─────────────────────────────────────────
async function exportStudentsReport(req, res, next) {
  try {
    const { schoolId, id: teacherId } = req.user;
    const { classroomId, fields } = req.query;
    const prisma = req.app.get("prisma");

    const teacherClassrooms = await prisma.classroom.findMany({
      take: 50,
      where: { teacherId, schoolId },
      select: { id: true },
    });
    const teacherClassroomIds = teacherClassrooms.map((c) => c.id);

    if (classroomId && !teacherClassroomIds.includes(classroomId)) {
      return res.status(403).json({ error: "محظور", message: "لا يمكن الوصول لهذا الفصل" });
    }

    const where = { schoolId, role: "STUDENT", isActive: true, classroomId: { in: teacherClassroomIds } };
    if (classroomId) where.classroomId = classroomId;

    const students = await prisma.user.findMany({
      take: 200,
      where,
      select: {
        id: true,
        name: true,
        nationalId: true,
        email: true,
        createdAt: true,
        classroom: { select: { name: true } },
        studentProfile: { select: { totalXP: true, currentLevel: true, currentStreak: true, longestStreak: true, lastActive: true } },
      },
      orderBy: { name: "asc" },
    });

    const selectedFields = fields ? fields.split(",") : ["name", "username", "classroom", "xp", "level", "streak"];

    // SECURITY: Batch all DB queries upfront instead of N+1 per student
    const needsSubmissionData = selectedFields.some(f => ["submissions", "passed", "passedRate", "avgScore"].includes(f));
    const needsAttendanceData = selectedFields.some(f => ["attendance", "attendanceDays", "absenceDays"].includes(f));

    const studentIds = students.map(s => s.id);

    // Parallel batch queries
    const [submissionCounts, attendanceCounts] = await Promise.all([
      needsSubmissionData ? prisma.submission.groupBy({
        by: ['studentId', 'passed'],
        where: { studentId: { in: studentIds } },
        _count: true,
      }).catch(() => []) : Promise.resolve([]),
      needsAttendanceData ? prisma.attendance.groupBy({
        by: ['studentId', 'status'],
        where: {
          studentId: { in: studentIds },
          date: { gte: new Date(new Date().getFullYear(), new Date().getMonth(), 1) }
        },
        _count: true,
      }).catch(() => []) : Promise.resolve([]),
    ]);

    // Index batch results by studentId
    const subIndex = {};
    for (const row of submissionCounts) {
      if (!subIndex[row.studentId]) subIndex[row.studentId] = { total: 0, passed: 0 };
      subIndex[row.studentId].total += row._count;
      if (row.passed) subIndex[row.studentId].passed = row._count;
    }
    const attIndex = {};
    for (const row of attendanceCounts) {
      if (!attIndex[row.studentId]) attIndex[row.studentId] = { total: 0, present: 0 };
      attIndex[row.studentId].total += row._count;
      if (row.status === "PRESENT") attIndex[row.studentId].present = row._count;
    }

    const rows = students.map((s) => {
      const row = {};
      const sub = subIndex[s.id] || { total: 0, passed: 0 };
      const att = attIndex[s.id] || { total: 0, present: 0 };

      for (const f of selectedFields) {
        switch (f) {
          case "name": row["الاسم"] = s.name; break;
          case "username": row["اسم المستخدم"] = s.nationalId || ""; break;
          case "email": row["البريد الإلكتروني"] = s.email || ""; break;
          case "classroom": row["الفصل"] = s.classroom?.name || ""; break;
          case "xp": row["إجمالي النقاط"] = s.studentProfile?.totalXP || 0; break;
          case "level": row["المستوى"] = s.studentProfile?.currentLevel || 1; break;
          case "streak": row["التسلسل"] = s.studentProfile?.currentStreak || 0; break;
          case "longestStreak": row["أطول سلسلة"] = s.studentProfile?.longestStreak || 0; break;
          case "lastActive": row["آخر نشاط"] = s.studentProfile?.lastActive ? new Date(s.studentProfile.lastActive).toLocaleDateString("ar-SA") : ""; break;
          case "registeredAt": row["تاريخ التسجيل"] = s.createdAt ? new Date(s.createdAt).toLocaleDateString("ar-SA") : ""; break;
          case "submissions":
            row["إجمالي المحاولات"] = sub.total;
            break;
          case "passed":
            row["المحاولات الناجحة"] = sub.passed;
            break;
          case "passedRate":
            row["نسبة النجاح"] = sub.total > 0 ? Math.round((sub.passed / sub.total) * 100) + "%" : "0%";
            break;
          case "attendance":
            row["الحضور %"] = att.total > 0 ? Math.round((att.present / att.total) * 100) + "%" : "0%";
            break;
          case "attendanceDays":
            row["أيام الحضور"] = att.present;
            break;
          case "absenceDays":
            row["أيام الغياب"] = att.total - att.present;
            break;
          case "achievements":
            row["الإنجازات"] = 0; // Pre-fetched if needed
            break;
          case "avgScore":
            row["متوسط الدرجات"] = 0; // Pre-fetched if needed
            break;
        }
      }
      return row;
    });

    res.json({ success: true, rows, total: rows.length });
  } catch (err) {
    next(err);
  }
}

module.exports = {
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
};
