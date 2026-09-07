/* Developer: محمد محمود الحموز | Dhad Studio */
// ==============================================================================
// Teacher Analytics Engine - Batch 11 (Redis Optimized)
// ==============================================================================

const prisma = require("../utils/prisma");
const { leaderboardCache } = require("../utils/redis");
const { logger } = require("../utils/logger");

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Honor Roll Leaderboard - Top Students (Redis Cached)          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getLeaderboard(req, res, next) {
  try {
    const { role, schoolId } = req.user;
    const targetSchoolId = req.query.schoolId || schoolId;
    const classroomId = req.query.classroomId;
    const limit = parseInt(req.query.limit) || 50;
    const forceRefresh = req.query.refresh === 'true';

    // Authorization check
    if (role !== "ADMIN" && targetSchoolId !== schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط الوصول لبيانات مدرستك"
      });
    }

    // Try Redis cache first (unless force refresh)
    if (!forceRefresh) {
      const cachedLeaderboard = await leaderboardCache.getTop(limit);
      
      if (cachedLeaderboard && cachedLeaderboard.length > 0) {
        // SECURITY: Cap cached leaderboard to prevent OOM
        const cappedLeaderboard = cachedLeaderboard.slice(0, Math.min(limit + 10, 200));
        const studentIds = cappedLeaderboard.map(e => e.studentId);
        const students = await prisma.user.findMany({
          where: { id: { in: studentIds } },
          select: { id: true, name: true, schoolId: true },
          take: 200
        });
        
        const studentMap = {};
        students.forEach(s => { studentMap[s.id] = s; });
        
        // Filter by school and enrich
        const enrichedLeaderboard = cappedLeaderboard
          .filter(entry => studentMap[entry.studentId]?.schoolId === targetSchoolId)
          .map((entry, index) => ({
            ...entry,
            rank: index + 1,
            studentName: studentMap[entry.studentId]?.name || 'Unknown',
          }))
          .slice(0, limit);

        if (enrichedLeaderboard.length > 0) {
          logger.debug('📊 Leaderboard served from Redis cache', {
            schoolId: targetSchoolId,
            count: enrichedLeaderboard.length,
          });

          return res.json({
            school: { id: targetSchoolId },
            leaderboard: enrichedLeaderboard,
            generatedAt: new Date().toISOString(),
            totalStudents: enrichedLeaderboard.length,
            source: 'cache',
            analyticsPeriod: {
              type: "all_time",
              from: "2026-01-01",
              to: new Date().toISOString().split('T')[0]
            }
          });
        }
      }
    }

    // Cache miss or force refresh - query database
    logger.debug('📊 Leaderboard cache miss, querying database', { schoolId: targetSchoolId });

    const leaderboard = await prisma.user.findMany({
      where: {
        schoolId: targetSchoolId,
        role: "STUDENT",
        ...(classroomId ? { classroomId } : {}),
      },
      take: Math.min(limit + 10, 200), // SECURITY: Cap query size to prevent OOM
      select: {
        id: true,
        name: true,
        studentProfile: {
          select: {
            totalXP: true,
            currentLevel: true,
            currentStreak: true,
          }
        },
        _count: {
          select: {
            submissions: { where: { passed: true } },
            projects: true
          }
        }
      }
    });

    // Calculate rankings based on profile + counts (no need to load all submissions)
    const rankedStudents = leaderboard.map(student => {
      const profile = student.studentProfile;
      const passedChallenges = student._count?.submissions || 0;
      const totalProjects = student._count?.projects || 0;

      // XP from profile
      const totalXP = profile?.totalXP || (passedChallenges * 100);

      // Simplified honor score from available metrics
      const honorScore = (
        (passedChallenges * 10) +        // Challenge contributions
        (totalProjects * 15) +            // Project contributions
        (totalXP / 100) * 0.2            // XP contribution
      );

      return {
        studentId: student.id,
        studentName: student.name,
        totalXP,
        currentLevel: profile?.currentLevel || 1,
        currentStreak: profile?.currentStreak || 0,
        ranking: 0, // Will be set after sorting
        totalChallenges: passedChallenges,
        passedChallenges,
        challengeCompletionRate: 0,
        avgChallengeScore: 0,
        totalProjects,
        avgProjectScore: 0,
        honorScore: Math.round(honorScore)
      };
    })
      .filter(student => student.totalChallenges > 0 || student.totalProjects > 0)
      .sort((a, b) => b.honorScore - a.honorScore)
      .slice(0, limit)
      .map((student, index) => ({
        ...student,
        ranking: index + 1
      }));

    // Update Redis cache with fresh data
    const cacheData = rankedStudents.map(s => ({
      studentId: s.studentId,
      totalXP: s.totalXP,
    }));
    
    await leaderboardCache.bulkUpdate(cacheData);

    logger.info('📊 Leaderboard refreshed from database', {
      schoolId: targetSchoolId,
      count: rankedStudents.length,
    });

    res.json({
      school: { id: targetSchoolId },
      leaderboard: rankedStudents,
      generatedAt: new Date().toISOString(),
      totalStudents: rankedStudents.length,
      source: 'database',
      analyticsPeriod: {
        type: "all_time",
        from: "2026-01-01",
        to: new Date().toISOString().split('T')[0]
      }
    });
  } catch (error) {
    logger.error('Leaderboard query failed', { error: error.message });
    // Return empty leaderboard instead of 500 on DB contention
    return res.json({
      school: { id: req.query.schoolId || req.user?.schoolId },
      leaderboard: [],
      generatedAt: new Date().toISOString(),
      totalStudents: 0,
      source: 'fallback',
      analyticsPeriod: {
        type: "all_time",
        from: "2026-01-01",
        to: new Date().toISOString().split('T')[0]
      }
    });
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Live Attendance Summary                                │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getAttendanceSummary(req, res, next) {
  try {
    const { role, schoolId } = req.user;
    const targetSchoolId = req.query.schoolId || schoolId;
    const classroomId = req.query.classroomId;
    const today = new Date().toISOString().split('T')[0];
    const todayStart = new Date(today + "T00:00:00.000Z");
    const todayEnd = new Date(today + "T23:59:59.999Z");

    // Authorization check
    if (role !== "ADMIN" && targetSchoolId !== schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط الوصول لبيانات مدرستك"
      });
    }

    const studentWhere = { schoolId: targetSchoolId, role: "STUDENT" };
    if (classroomId) studentWhere.classroomId = classroomId;

    const totalStudents = await prisma.user.count({ where: studentWhere });

    // SECURITY: Cap at 200 students + parallel queries
    const studentIds = (await prisma.user.findMany({ where: studentWhere, select: { id: true }, take: 200 })).map(s => s.id);

    const yesterday = new Date();
    yesterday.setDate(yesterday.getDate() - 1);
    const yesterdayStr = yesterday.toISOString().split('T')[0];
    const yesterdayStart = new Date(yesterdayStr + "T00:00:00.000Z");
    const yesterdayEnd = new Date(yesterdayStr + "T23:59:59.999Z");

    // SECURITY: Use count()+groupBy instead of loading all records
    const [activeToday, activeYesterday, statusBreakdown] = await Promise.all([
      prisma.attendance.count({
        where: {
          studentId: { in: studentIds },
          date: { gte: todayStart, lte: todayEnd }
        }
      }),
      prisma.attendance.count({
        where: {
          studentId: { in: studentIds },
          date: { gte: yesterdayStart, lte: yesterdayEnd }
        }
      }),
      prisma.attendance.groupBy({
        by: ['status'],
        where: {
          studentId: { in: studentIds },
          date: { gte: todayStart, lte: todayEnd }
        },
        _count: { status: true }
      })
    ]);

    const activeStudentsToday = activeToday;
    const activeStudentsYesterday = activeYesterday;

    // Calculate rates
    const attendanceRateToday = totalStudents > 0 ? (activeStudentsToday / totalStudents) * 100 : 0;
    const attendanceRateYesterday = totalStudents > 0 ? (activeStudentsYesterday / totalStudents) * 100 : 0;

    // Status breakdown for today (from groupBy)
    const attendanceStatusBreakdown = {};
    for (const row of statusBreakdown) {
      attendanceStatusBreakdown[row.status] = row._count.status;
    }

    res.json({
      school: { id: targetSchoolId },
      period: {
        today: today,
        yesterday: yesterdayStr,
        comparisonDay: "relative"
      },
      attendance: {
        today: {
          totalPresent: activeStudentsToday,
          totalExpected: totalStudents,
          rate: Math.round(attendanceRateToday),
          statusBreakdown: attendanceStatusBreakdown
        },
        yesterday: {
          totalPresent: activeStudentsYesterday,
          totalExpected: totalStudents,
          rate: Math.round(attendanceRateYesterday),
          statusBreakdown: {}
        },
        trend: {
          absoluteChange: activeStudentsToday - activeStudentsYesterday,
          percentageChange: attendanceRateToday - attendanceRateYesterday,
          trend: attendanceRateToday > attendanceRateYesterday ? 'IMPROVING' :
                  attendanceRateToday < attendanceRateYesterday ? 'DECLINING' : 'STABLE'
        }
      },
      lastUpdated: new Date().toISOString()
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Class Progress Matrix                                    │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getClassProgress(req, res, next) {
  try {
    const { role, schoolId } = req.user;
    const targetSchoolId = req.query.schoolId || schoolId;
    const classroomId = req.query.classroomId;

    // Authorization check
    if (role !== "ADMIN" && targetSchoolId !== schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "يمكنك فقط الوصول لبيانات مدرستك"
      });
    }

    const now = new Date();
    const weekAgo = new Date(now.setDate(now.getDate() - 7));

    const studentWhere = { schoolId: targetSchoolId, role: "STUDENT" };
    if (classroomId) studentWhere.classroomId = classroomId;

    // SECURITY: Cap at 200 students to prevent OOM
    const students = await prisma.user.findMany({
      where: studentWhere,
      select: { id: true, name: true },
      take: 200
    });

    // Get all challenges for the first 5 lessons
    const firstFiveLessons = await prisma.lesson.findMany({
      where: {
        schoolId: targetSchoolId,
        order: { lte: 5 }
      },
      select: { id: true },
      take: 100
    });

    const lessonIds = firstFiveLessons.map(lesson => lesson.id);

    // Get challenges (exercises) for the school
    const exercises = await prisma.challenge.findMany({
      where: {
        schoolId: targetSchoolId,
        published: true,
      },
      select: { id: true, title: true, points: true, tier: true },
      take: 200
    });
    // SECURITY: Cap submissions query to prevent OOM with large datasets
    const studentIds = students.map(s => s.id);
    const exerciseIds = exercises.map(e => e.id);

    // Use SQL aggregation instead of O(n×m) in-memory grouping
    const [passCounts, attemptCounts] = await Promise.all([
      prisma.submission.groupBy({
        by: ['studentId', 'challengeId'],
        where: {
          studentId: { in: studentIds },
          challengeId: { in: exerciseIds },
          passed: true,
        },
        _count: { id: true },
      }),
      prisma.submission.groupBy({
        by: ['studentId', 'challengeId'],
        where: {
          studentId: { in: studentIds },
          challengeId: { in: exerciseIds },
        },
        _count: { id: true },
      })
    ]);

    // Build lookup maps: { studentId: { challengeId: count } }
    const passMap = {};
    for (const row of passCounts) {
      if (!passMap[row.studentId]) passMap[row.studentId] = {};
      passMap[row.studentId][row.challengeId] = row._count.id;
    }
    const attemptMap = {};
    for (const row of attemptCounts) {
      if (!attemptMap[row.studentId]) attemptMap[row.studentId] = {};
      attemptMap[row.studentId][row.challengeId] = row._count.id;
    }

    const exerciseLookup = {};
    for (const ex of exercises) {
      exerciseLookup[ex.id] = ex;
    }

    const progressMatrix = [];

    for (const student of students) {
      const studentPasses = passMap[student.id] || {};
      const studentAttempts = attemptMap[student.id] || {};

      const exerciseProgress = {};
      for (const exercise of exercises) {
        const completed = studentPasses[exercise.id] || 0;
        const total = studentAttempts[exercise.id] || 0;
        const completionRate = total > 0 ? (completed / total) * 100 : 0;

        exerciseProgress[exercise.id] = {
          exerciseId: exercise.id,
          exerciseTitle: exercise.title,
          completed,
          total,
          completionRate: Math.round(completionRate),
          pointsEarned: completed * exercise.points,
          totalPossiblePoints: total * exercise.points
        };
      }

      const totalCompleted = Object.values(exerciseProgress).reduce((sum, ex) => sum + ex.completed, 0);
      const totalPossible = Object.values(exerciseProgress).reduce((sum, ex) => sum + ex.total, 0);
      const overallProgress = totalPossible > 0 ? (totalCompleted / totalPossible) * 100 : 0;

      progressMatrix.push({
        studentId: student.id,
        studentName: student.name,
        overallProgress: Math.round(overallProgress),
        exercisesCompleted: totalCompleted,
        totalExercises: totalPossible,
        exerciseProgress: Object.values(exerciseProgress)
      });
    }

    // Find most failed exercise
    const exerciseFailureCounts = {};
    for (const student of progressMatrix) {
      for (const exercise of student.exerciseProgress) {
        if (exercise.completionRate === 0) {
          if (!exerciseFailureCounts[exercise.exerciseId]) {
            exerciseFailureCounts[exercise.exerciseId] = {
              exerciseId: exercise.exerciseId,
              exerciseTitle: exercise.exerciseTitle,
              failureCount: 0,
              totalStudents: 0
            };
          }
          exerciseFailureCounts[exercise.exerciseId].failureCount++;
        }
        exerciseFailureCounts[exercise.exerciseId].totalStudents++;
      }
    }

    const mostFailedExercises = Object.values(exerciseFailureCounts)
      .sort((a, b) => b.failureCount - a.failureCount)
      .slice(0, 5);

    const avgProgress = progressMatrix.length > 0
      ? Math.round(progressMatrix.reduce((sum, s) => sum + s.overallProgress, 0) / progressMatrix.length)
      : 0;

    // SECURITY: Use aggregated query instead of loading all profiles
    const xpResult = await prisma.studentProfile.aggregate({
      where: { studentId: { in: studentIds } },
      _avg: { totalXP: true },
    });
    const avgXP = Math.round(xpResult._avg?.totalXP || 0);

    res.json({
      progress: {
        averageProgress: avgProgress,
        averageXP: avgXP,
        studentCount: students.length,
        totalChallenges: exercises.length
      },
      school: { id: targetSchoolId },
      analytics: {
        period: {
          lessons: "Lessons 1-5",
          timeframe: "All Time",
          totalExercises: exercises.length,
          totalStudents: students.length
        },
        classProgress: {
          overallCompletionRate: avgProgress,
          studentsByProgress: {
            "0-20%": progressMatrix.filter(s => s.overallProgress <= 20).length,
            "21-40%": progressMatrix.filter(s => s.overallProgress > 20 && s.overallProgress <= 40).length,
            "41-60%": progressMatrix.filter(s => s.overallProgress > 40 && s.overallProgress <= 60).length,
            "61-80%": progressMatrix.filter(s => s.overallProgress > 60 && s.overallProgress <= 80).length,
            "81-100%": progressMatrix.filter(s => s.overallProgress > 80).length
          }
        },
        progressMatrix: progressMatrix.map(s => ({
          studentId: s.studentId,
          studentName: s.studentName,
          overallProgress: s.overallProgress,
          exercisesCompleted: s.exercisesCompleted,
          totalExercises: s.totalExercises
        }))
      },
      mostFailedExercises,
      lastUpdated: new Date().toISOString()
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test Cases Delivery (Secure JSON)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getTestCases(req, res, next) {
  try {
    const { lessonId } = req.params;

    // Verify lesson exists and belongs to authenticated teacher's school
    const lesson = await prisma.lesson.findUnique({
      where: { id: lessonId },
    });

    if (!lesson) {
      return res.status(404).json({
        error: "غير موجود",
        message: "الدرس غير موجود"
      });
    }

    // Check authorization
    if (req.user.role !== "ADMIN" && req.user.schoolId !== lesson.schoolId) {
      return res.status(403).json({
        error: "محظور",
        message: "تم رفض الوصول لحالات اختبار هذا الدرس"
      });
    }

    res.json({
      success: true,
      lessonId,
      lessonTitle: lesson.title,
      message: "نقطة نهاية اختبارات الدرس نشطة"
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Challenge Tests Delivery                                │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getChallengeTestCases(req, res, next) {
  try {
    const { tierId } = req.params;
    const { role, schoolId } = req.user;
    const difficultyMap = {
      '1': 'BEGINNER',
      '2': 'INTERMEDIATE',
      '3': 'ADVANCED',
      '4': 'EXPERT'
    };

    const difficulty = difficultyMap[tierId];
    if (!difficulty) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "معرف المستوى غير صالح"
      });
    }

    // SECURITY: Add school isolation - teachers can only see their school's challenges
    const whereClause = {
      tier: parseInt(tierId),
      published: true
    };

    // Non-admin users are restricted to their school's challenges
    if (role !== "ADMIN") {
      whereClause.schoolId = schoolId;
    }

    // Fetch challenges with test cases
    const challenges = await prisma.challenge.findMany({
      where: whereClause,
      take: 50,
      select: {
        id: true,
        title: true,
        description: true,
        difficulty: true,
        challengeType: true,
        points: true,
        tier: true
      }
    });

    // Prepare secure delivery of test data
    const secureData = {
      tier: difficulty,
      challenges: challenges.map(challenge => ({
        ...challenge,
        testCases: [],
        downloadUrl: `/api/v1/challenges/tier/${tierId}/${challenge.id}/test-cases`,
        accessLevel: "AUTHORIZED_TEACHER"
      }))
    };

    res.json({
      tier: difficulty,
      challengesCount: challenges.length,
      data: secureData,
      generatedAt: new Date().toISOString(),
      securityNote: "هذه البيانات مشفرة أثناء النقل ومحكومة بالوصول"
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Challenges Listing (Authenticated)                     │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getChallenges(req, res, next) {
  try {
    const { role } = req.user;
    const { difficulty, courseId } = req.query;

    const whereClause = { published: true };

    // Filter: no courseId param → main challenges only; specific courseId → that course's challenges
    if (courseId) {
      whereClause.courseId = courseId;
    } else {
      whereClause.courseId = null;
    }

    if (difficulty && difficulty !== 'all') {
      const validDifficulties = ['BEGINNER', 'INTERMEDIATE', 'ADVANCED', 'EXPERT'];
      if (validDifficulties.includes(difficulty)) {
        whereClause.difficulty = difficulty;
      }
    }

    const challenges = await prisma.challenge.findMany({
      where: whereClause,
      take: 200,
      select: {
        id: true,
        title: true,
        description: true,
        difficulty: true,
        challengeType: true,
        points: true,
        tier: true
      },
      orderBy: [{ tier: "asc" }, { points: "asc" }]
    });

    res.json({
      success: true,
      challenges: challenges.map(c => ({
        ...c,
        xpReward: c.points,
        category: c.difficulty
      })),
      total: challenges.length
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Teacher Dashboard (Aggregated Stats)                   │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getTeacherDashboard(req, res, next) {
  try {
    const { id: teacherId, schoolId, role } = req.user;

    // Get teacher's classrooms
    const classrooms = await prisma.classroom.findMany({
      where: { teacherId, schoolId },
      select: { id: true, name: true, gradeLevel: true, subject: true, isActive: true, inviteCode: true }
    });

    const classroomIds = classrooms.map(c => c.id);

    // Count total students across all classrooms
    const totalStudents = await prisma.user.count({
      where: { classroomId: { in: classroomIds }, role: "STUDENT" }
    });

    // Count active students (submitted in last 7 days)
    const weekAgo = new Date();
    weekAgo.setDate(weekAgo.getDate() - 7);
    const activeStudents = await prisma.user.count({
      where: {
        classroomId: { in: classroomIds },
        role: "STUDENT",
        submissions: { some: { createdAt: { gte: weekAgo } } }
      }
    });

    // Get total assessments for teacher's classrooms
    const totalAssessments = await prisma.assessment.count({
      where: { classroomId: { in: classroomIds } }
    });

    // Get recent submissions
    const recentSubmissions = await prisma.submission.findMany({
      where: {
        student: { classroomId: { in: classroomIds } }
      },
      select: {
        id: true,
        passed: true,
        syntaxScore: true,
        createdAt: true,
        student: { select: { name: true } },
        challenge: { select: { title: true, difficulty: true } }
      },
      orderBy: { createdAt: "desc" },
      take: 10
    });

    // Get leaderboard top 5
    const topStudents = await prisma.user.findMany({
      where: { classroomId: { in: classroomIds }, role: "STUDENT" },
      select: {
        id: true,
        name: true,
        studentProfile: { select: { totalXP: true, currentLevel: true } }
      },
      orderBy: { studentProfile: { totalXP: "desc" } },
      take: 5
    });

    res.json({
      success: true,
      dashboard: {
        classrooms: {
          total: classrooms.length,
          active: classrooms.filter(c => c.isActive).length,
          list: classrooms
        },
        students: {
          total: totalStudents,
          active: activeStudents,
          inactive: totalStudents - activeStudents
        },
        assessments: {
          total: totalAssessments
        },
        recentSubmissions: recentSubmissions.map(s => ({
          id: s.id,
          studentName: s.student.name,
          challengeTitle: s.challenge.title,
          passed: s.passed,
          score: s.syntaxScore,
          createdAt: s.createdAt
        })),
        topStudents: topStudents.map((s, i) => ({
          rank: i + 1,
          name: s.name,
          xp: s.studentProfile?.totalXP || 0,
          level: s.studentProfile?.currentLevel || 1
        }))
      }
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Teacher Classrooms List                                │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getTeacherClassrooms(req, res, next) {
  try {
    const { id: teacherId, schoolId } = req.user;

    const classrooms = await prisma.classroom.findMany({
      where: { teacherId, schoolId },
      take: 50,
      include: {
        _count: { select: { students: true } },
        assessments: {
          select: { id: true },
          take: 5,
          orderBy: { createdAt: "desc" }
        },
        students: {
          select: {
            studentProfile: { select: { totalXP: true } }
          }
        }
      },
      orderBy: { createdAt: "desc" }
    });

    res.json({
      success: true,
      classrooms: classrooms.map(c => {
        const xpValues = c.students
          .map(s => s.studentProfile?.totalXP)
          .filter(xp => xp != null);
        const averageXP = xpValues.length > 0
          ? Math.round(xpValues.reduce((a, b) => a + b, 0) / xpValues.length)
          : 0;

        return {
          id: c.id,
          name: c.name,
          description: c.description,
          gradeLevel: c.gradeLevel,
          subject: c.subject,
          maxStudents: c.maxStudents,
          isActive: c.isActive,
          studentCount: c._count.students,
          averageXP,
          recentAssessments: c.assessments.length,
          createdAt: c.createdAt
        };
      }),
      total: classrooms.length
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Create Classroom (Teacher Only)                        │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function createClassroom(req, res, next) {
  try {
    const { id: teacherId, schoolId, role } = req.user;
    const { name, subject, gradeLevel, description, maxStudents } = req.body;

    if (role !== "TEACHER" && role !== "ADMIN") {
      return res.status(403).json({
        error: "محظور",
        message: "يمكن للمعلمين فقط إنشاء الفصول"
      });
    }

    if (!name || !name.trim()) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "اسم الفصل مطلوب"
      });
    }

    // Check for duplicate name in same school
    const existing = await prisma.classroom.findFirst({
      where: { schoolId, name: name.trim() }
    });

    if (existing) {
      return res.status(409).json({
        error: "تعارض",
        message: "يوجد فصل بهذا الاسم بالفعل"
      });
    }

    const crypto = require("crypto");
    function generateInviteCode() {
      return crypto.randomBytes(3).toString("hex").toUpperCase(); // 6-char hex code
    }
    let inviteCode = generateInviteCode();
    // Ensure uniqueness
    while (await prisma.classroom.findUnique({ where: { inviteCode } })) {
      inviteCode = generateInviteCode();
    }

    const classroom = await prisma.classroom.create({
      data: {
        schoolId,
        teacherId,
        name: name.trim(),
        description: description || "",
        subject: subject || "",
        gradeLevel: gradeLevel || "",
        maxStudents: maxStudents || 40,
        isActive: true,
        inviteCode
      }
    });

    logger.info(`Classroom created: "${name}" by teacher ${teacherId}`);

    res.status(201).json({
      success: true,
      message: "تم إنشاء الفصل بنجاح",
      classroom: {
        id: classroom.id,
        name: classroom.name,
        subject: classroom.subject,
        gradeLevel: classroom.gradeLevel,
        maxStudents: classroom.maxStudents,
        isActive: classroom.isActive,
        inviteCode: classroom.inviteCode
      }
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Teacher Assessments (All Classrooms)                    │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getTeacherAssessments(req, res, next) {
  try {
    const { id: teacherId, schoolId } = req.user;

    const classrooms = await prisma.classroom.findMany({
      where: { teacherId, schoolId },
      take: 50,
      select: { id: true }
    });

    const classroomIds = classrooms.map(c => c.id);

    const assessments = await prisma.assessment.findMany({
      where: { classroomId: { in: classroomIds } },
      take: 50,
      include: {
        classroom: { select: { id: true, name: true } },
        _count: { select: { results: true } }
      },
      orderBy: { createdAt: "desc" }
    });

    res.json({
      success: true,
      assessments: assessments.map(a => ({
        id: a.id,
        title: a.title,
        description: a.description,
        classroomId: a.classroomId,
        classroom: a.classroom,
        assessmentType: a.assessmentType,
        allowedTime: a.allowedTime,
        startTime: a.startTime,
        endTime: a.endTime,
        totalPoints: a.totalPoints,
        passingScore: a.passingScore,
        isPublished: a.isPublished,
        isActive: new Date() >= a.startTime && new Date() <= a.endTime,
        questionCount: a.challengeIds.length,
        resultCount: a._count.results,
      })),
      total: assessments.length
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Mark Attendance for Classroom                        │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function markAttendance(req, res, next) {
  try {
    const { role, id: teacherId, schoolId } = req.user;
    const { classroomId, date, records } = req.body;

    if (role !== "ADMIN" && role !== "TEACHER") {
      return res.status(403).json({ error: "محظور", message: "يمكن للمعلمين فقط تسجيل الحضور" });
    }

    if (!classroomId || !records || !Array.isArray(records)) {
      return res.status(400).json({ error: "خطأ في الطلب", message: "معرف الفصل ومصفوفة السجلات مطلوبة" });
    }

    const targetDate = date ? new Date(date + "T00:00:00.000Z") : new Date();
    targetDate.setHours(0, 0, 0, 0);

    const students = await prisma.user.findMany({
      where: { classroomId, role: "STUDENT", schoolId },
      select: { id: true, name: true },
      take: 100
    });

    const studentIds = new Set(students.map(s => s.id));

    const results = [];
    for (const record of records) {
      if (!studentIds.has(record.studentId)) continue;

      const attendance = await prisma.attendance.upsert({
        where: { studentId_date: { studentId: record.studentId, date: targetDate } },
        update: {
          status: record.status || "PRESENT",
          notes: record.notes || "",
          recordedBy: teacherId
        },
        create: {
          studentId: record.studentId,
          schoolId,
          date: targetDate,
          status: record.status || "PRESENT",
          notes: record.notes || "",
          recordedBy: teacherId
        }
      });
      results.push(attendance);
    }

    res.json({
      success: true,
      message: "Attendance recorded for " + results.length + " students",
      count: results.length,
      date: targetDate.toISOString().split('T')[0]
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Attendance History for Classroom                   │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getAttendanceHistory(req, res, next) {
  try {
    const { role, schoolId } = req.user;
    const { classroomId, days } = req.query;
    const numDays = parseInt(days) || 30;

    if (!classroomId) {
      return res.status(400).json({ error: "خطأ في الطلب", message: "معرف الفصل مطلوب" });
    }

    const students = await prisma.user.findMany({
      where: { classroomId, role: "STUDENT", schoolId },
      select: { id: true },
      take: 100
    });

    const studentIds = students.map(s => s.id);
    const totalStudents = studentIds.length;

    const since = new Date();
    since.setDate(since.getDate() - numDays);
    since.setHours(0, 0, 0, 0);

    const attendanceRecords = await prisma.attendance.findMany({
      where: {
        studentId: { in: studentIds },
        date: { gte: since }
      },
      orderBy: { date: "desc" },
      take: 500
    });

    const byDate = {};
    for (const record of attendanceRecords) {
      const dateKey = new Date(record.date).toISOString().split('T')[0];
      if (!byDate[dateKey]) byDate[dateKey] = { present: 0, absent: 0, late: 0, total: 0 };
      byDate[dateKey].total++;
      if (record.status === "PRESENT") byDate[dateKey].present++;
      else if (record.status === "ABSENT") byDate[dateKey].absent++;
      else if (record.status === "LATE") byDate[dateKey].late++;
    }

    const history = Object.entries(byDate).map(([date, data]) => ({
      date,
      present: data.present,
      absent: totalStudents - data.present,
      late: data.late,
      total: totalStudents,
      percentage: totalStudents > 0 ? Math.round((data.present / totalStudents) * 100) : 0
    })).sort((a, b) => b.date.localeCompare(a.date));

    res.json({ success: true, attendance: history, totalStudents });
  } catch (error) {
    next(error);
  }
}

module.exports = {
  getLeaderboard,
  getAttendanceSummary,
  getClassProgress,
  getTestCases,
  getChallengeTestCases,
  getChallenges,
  getTeacherDashboard,
  getTeacherClassrooms,
  createClassroom,
  getTeacherAssessments,
  markAttendance,
  getAttendanceHistory
};
