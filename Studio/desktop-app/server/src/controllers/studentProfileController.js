// ========================================================
// Student Progression Engine - Batch 7 Core (Refactored)
// ========================================================

const prisma = require("../utils/prisma");
const { logger } = require("../utils/logger");

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   XP Calculation Constants                       │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

const XP_TABLE = {
  BEGINNER: 100,
  INTERMEDIATE: 200,
  ADVANCED: 300,
  EXPERT: 500
};

const LEVEL_THRESHOLDS = [
  0, 100, 250, 500, 1000, 1500, 2500, 3500, 5000, 7000,
  9000, 11500, 14000, 17000, 20000, 23500, 27500, 32000, 37000, 42500,
  48500, 55000, 62000, 70000, 78500, 87500, 97000, 107000, 118000, 130000,
  142500, 155500, 169000, 183000, 198000, 214000, 231000, 249000, 268000, 288000,
  310000, 333000, 358000, 384000, 412000, 442000, 474000, 508000, 544000, 582000
];

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get or Create Student Profile                    │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getOrCreateProfile(studentId) {
  let profile = await prisma.studentProfile.findUnique({
    where: { studentId },
    include: {
      student: {
        select: { id: true, name: true, schoolId: true }
      }
    }
  });

  if (!profile) {
    profile = await prisma.studentProfile.create({
      data: {
        studentId,
        totalXP: 0,
        currentLevel: 1,
        currentStreak: 0,
        longestStreak: 0,
        lastActive: new Date(),
        lastXPAwarded: new Date(),
        lastActivityDate: new Date().toISOString().split('T')[0]
      },
      include: {
        student: {
          select: { id: true, name: true, schoolId: true }
        }
      }
    });
  }

  return profile;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Persistent XP Awarding System                  │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function awardXP(submissionData, studentId, tx) {
  try {
    const {
      challengeId,
      status,
      score,
    } = submissionData;

    // SECURITY: XP is only awarded from trusted sources (server-side evaluation)
    if (status !== "PASS" && status !== "PASSED") {
      return { success: false, message: "يُمنح نقاط الخبرة فقط للإرسالات الناجحة" };
    }

    // Get challenge info for XP calculation
    const challenge = await (tx || prisma).challenge.findUnique({
      where: { id: challengeId }
    });

    if (!challenge) {
      return { success: false, message: "التحدي غير موجود" };
    }

    // ═══════════════════════════════════════════════════════════════
    // SECURITY: XP = baseXP only (no speed bonus, no performance bonus)
    // Speed bonus removed: execution time varies by environment
    // Performance bonus removed: server calculates score, not separate bonuses
    // ═══════════════════════════════════════════════════════════════
    const baseXP = XP_TABLE[challenge.difficulty] || XP_TABLE.BEGINNER;
    const totalXP = baseXP;

    const today = new Date().toISOString().split('T')[0];

    // Get or create profile
    const profile = await getOrCreateProfile(studentId);

    // Calculate level from current XP + pending XP
    const newTotalXP = profile.totalXP + totalXP;
    const newLevel = calculateLevel(newTotalXP);
    const leveledUp = newLevel > profile.currentLevel;

    // ═══════════════════════════════════════════════════════════════
    // SECURITY: Use provided transaction or fallback to raw query
    // Prevents race conditions in concurrent submissions
    // ═══════════════════════════════════════════════════════════════
    const db = tx || prisma;
    await db.$executeRaw`
      UPDATE StudentProfile
      SET totalXP = totalXP + ${totalXP},
          currentLevel = ${newLevel},
          lastActive = ${new Date()},
          lastXPAwarded = ${new Date()},
          lastActivityDate = ${today}
      WHERE studentId = ${studentId}
    `;

    // Fetch updated profile for response
    const refreshed = await (tx || prisma).studentProfile.findUnique({
      where: { studentId },
      select: { currentStreak: true, longestStreak: true, totalXP: true, currentLevel: true }
    });

    // Update daily XP record
    await (tx || prisma).dailyXP.upsert({
      where: { studentId_date: { studentId, date: today } },
      update: {
        totalXP: { increment: totalXP },
        submissionsCount: { increment: 1 }
      },
      create: {
        studentId,
        date: today,
        totalXP: totalXP,
        submissionsCount: 1
      }
    });

    // Log XP award activity
    await (tx || prisma).activityLog.create({
      data: {
        userId: studentId,
        action: "xp_awarded",
        details: JSON.stringify({
          challengeId,
          challengeTitle: challenge.title,
          status,
          baseXP,
          totalXP: totalXP,
          difficulty: challenge.difficulty,
          previousLevel: profile.currentLevel,
          newLevel,
          leveledUp
        })
      }
    });

    // Trigger achievement check
    await checkAndAwardAchievements(studentId, tx);

    logger.info(
      `Student ${studentId} awarded ${totalXP} XP for ${challenge.difficulty} challenge` +
      (leveledUp ? ` - LEVEL UP! Level ${newLevel}` : '')
    );

    return {
      success: true,
      xpAwarded: totalXP,
      previousLevel: profile.currentLevel,
      newLevel,
      leveledUp,
      totalXP: refreshed ? refreshed.totalXP : profile.totalXP + totalXP,
      currentStreak: refreshed ? refreshed.currentStreak : 0
    };
  } catch (error) {
    logger.error('Failed to award XP:', error.message);
    return { success: false, error: error.message };
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Persistent Streak Calculator (Day-Based)              │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function calculateDailyStreak(studentId) {
  try {
    const today = new Date().toISOString().split('T')[0];

    const profile = await prisma.studentProfile.findUnique({
      where: { studentId },
      select: { currentStreak: true, lastActivityDate: true }
    });

    if (!profile) {
      return 0;
    }

    const lastActivity = profile.lastActivityDate;
    if (!lastActivity) {
      // First activity ever — set streak to 1
      await prisma.studentProfile.update({
        where: { studentId },
        data: { currentStreak: 1, lastActivityDate: today, lastActive: new Date() }
      }).catch(() => {});
      return 1;
    }

    // Already logged today — return current streak (no write, no increment)
    // SECURITY: Multiple submissions same day should NOT increase streak
    if (lastActivity === today) {
      return profile.currentStreak;
    }

    // Check if activity was yesterday
    const yesterday = new Date();
    yesterday.setDate(yesterday.getDate() - 1);
    const yesterdayStr = yesterday.toISOString().split('T')[0];

    if (lastActivity === yesterdayStr) {
      // Streak continues — atomic increment
      const updated = await prisma.studentProfile.update({
        where: { studentId },
        data: {
          currentStreak: { increment: 1 },
          lastActivityDate: today,
          lastActive: new Date()
        },
        select: { currentStreak: true, longestStreak: true }
      }).catch(() => null);
      if (updated) {
        if (updated.currentStreak > updated.longestStreak) {
          await prisma.studentProfile.update({
            where: { studentId },
            data: { longestStreak: updated.currentStreak }
          }).catch(() => {});
        }
        return updated.currentStreak;
      }
      return profile.currentStreak + 1;
    }

    // Streak broken (missed more than 1 day) — reset to 1
    const reset = await prisma.studentProfile.update({
      where: { studentId },
      data: { currentStreak: 1, lastActivityDate: today, lastActive: new Date() },
      select: { longestStreak: true }
    }).catch(() => null);
    return 1;
  } catch (error) {
    logger.error('Failed to calculate streak:', error.message);
    return 0;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Level Calculator (1-50 XP System)               │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function calculateLevel(totalXP) {
  for (let i = LEVEL_THRESHOLDS.length - 1; i >= 0; i--) {
    if (totalXP >= LEVEL_THRESHOLDS[i]) {
      return i + 1;
    }
  }
  return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Achievement System - Badge Awarding              │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function checkAndAwardAchievements(studentId, tx) {
  try {
    const db = tx || prisma;
    const profile = await db.studentProfile.findUnique({
      where: { studentId },
      include: {
        userAchievements: {
          include: { achievement: true }
        }
      }
    });

    if (!profile) return;

    const awardedIds = profile.userAchievements.map(ua => ua.achievementId);

    // Get all achievements to check
    const allAchievements = await db.achievement.findMany({
      where: { isSystem: true },
      take: 200
    });

    for (const achievement of allAchievements) {
      if (awardedIds.includes(achievement.id)) continue;

      let qualifies = false;
      let criteria;
      try { criteria = JSON.parse(achievement.criteria); } catch(e) { continue; }

      switch (criteria.type) {
        case 'xp_threshold':
          qualifies = profile.totalXP >= criteria.value;
          break;
        case 'streak':
          qualifies = profile.currentStreak >= criteria.value;
          break;
        case 'level_up':
          qualifies = profile.currentLevel >= criteria.value;
          break;
        case 'longest_streak':
          qualifies = profile.longestStreak >= criteria.value;
          break;
      }

      if (qualifies) {
        // SECURITY: Use upsert to prevent duplicate achievements
        await db.userAchievement.upsert({
          where: {
            userId_achievementId: {
              userId: studentId,
              achievementId: achievement.id
            }
          },
          update: {},
          create: {
            userId: studentId,
            achievementId: achievement.id,
            metadata: {
              reason: `${criteria.type}: ${criteria.value}`,
              previousValue: profile[criteria.type] || 0,
              newValue: profile[criteria.type] || 0
            }
          }
        });

        logger.info(`Achievement awarded: "${achievement.name}" to student ${studentId}`);
      }
    }
  } catch (error) {
    logger.error('Failed to check achievements:', error.message);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Student Profile (Optimized)                   │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// Simple in-memory cache for school rankings (30-second TTL)
const schoolRankingCache = new Map();
const SCHOOL_RANKING_TTL = 30 * 1000;

function getCacheKey(prefix, id) {
  return `${prefix}:${id}`;
}

async function getStudentProfile(req, res, next) {
  try {
    const { id: studentId } = req.user;

    const profile = await getOrCreateProfile(studentId);

    if (!profile) {
      return res.status(404).json({
        error: "غير موجود",
        message: "ملف الطالب غير موجود"
      });
    }

    const schoolId = profile.student.schoolId;

    // Run independent queries in parallel
    const [
      schoolStudents,
      recentActivities,
      userAchievements,
      todayStreak,
      todayXP,
      totalChallenges
    ] = await Promise.all([
      // School ranking (cached 30s)
      (async () => {
        const cacheKey = getCacheKey('rank', schoolId);
        const cached = schoolRankingCache.get(cacheKey);
        if (cached && Date.now() - cached.time < SCHOOL_RANKING_TTL) {
          return cached.data;
        }
        const result = await prisma.studentProfile.findMany({
          where: {
            student: { schoolId, role: "STUDENT" }
          },
          select: { studentId: true, totalXP: true },
          orderBy: { totalXP: "desc" },
          take: 200
        });
        schoolRankingCache.set(cacheKey, { data: result, time: Date.now() });
        return result;
      })(),

      // Recent activity
      prisma.activityLog.findMany({
        where: { userId: studentId },
        select: { id: true, action: true, createdAt: true, details: true },
        orderBy: { createdAt: "desc" },
        take: 10
      }),

      // User achievements
      prisma.userAchievement.findMany({
        where: { userId: studentId },
        include: {
          achievement: {
            select: { id: true, name: true, description: true, icon: true, points: true }
          }
        },
        orderBy: { awardedAt: "desc" },
        take: 20
      }),

      // Daily streak
      calculateDailyStreak(studentId),

      // Today's XP
      prisma.dailyXP.findUnique({
        where: { studentId_date: { studentId, date: new Date().toISOString().split('T')[0] } }
      }),

      // Total passed challenges
      prisma.submission.count({
        where: { studentId, passed: true }
      })
    ]);

    const rank = schoolStudents.findIndex(s => s.studentId === studentId) + 1;

    // Calculate level progress
    const currentLevel = profile.currentLevel;
    const nextLevelXP = currentLevel < LEVEL_THRESHOLDS.length ? LEVEL_THRESHOLDS[currentLevel] : LEVEL_THRESHOLDS[LEVEL_THRESHOLDS.length - 1];
    const currentLevelXP = currentLevel > 1 ? LEVEL_THRESHOLDS[currentLevel - 1] : 0;
    const levelProgress = ((profile.totalXP - currentLevelXP) / (nextLevelXP - currentLevelXP)) * 100;

    res.json({
      success: true,
      profile: {
        studentId: profile.studentId,
        studentName: profile.student.name,
        schoolId: profile.student.schoolId,
        
        // Gamification stats (PERSISTED)
        totalXP: profile.totalXP,
        currentLevel: profile.currentLevel,
        currentStreak: profile.currentStreak,
        longestStreak: profile.longestStreak,
        lastActive: profile.lastActive,
        todayXP: todayXP?.totalXP || 0,
        todaySubmissions: todayXP?.submissionsCount || 0,
        totalChallenges: totalChallenges,
        
        // Ranking
        rank: rank,
        totalStudents: schoolStudents.length,
        
        // Achievements
        achievements: userAchievements.map(ua => ({
          ...ua.achievement,
          awardedAt: ua.awardedAt
        })),
        totalAchievements: userAchievements.length,
        
        // Recent activity (safe JSON parse)
        recentActivities: recentActivities.map(activity => {
          let details = null;
          try {
            details = activity.details ? JSON.parse(activity.details) : null;
          } catch (e) {
            logger.warn('Invalid activity.details JSON', {
              activityId: activity.id,
              details: activity.details?.substring(0, 100)
            });
            details = null;
          }
          return {
            action: activity.action,
            createdAt: activity.createdAt,
            details
          };
        }),
        
        // Level progress
        currentLevelXP: currentLevelXP,
        nextLevelXP: nextLevelXP,
        levelProgress: Math.min(Math.max(levelProgress, 0), 100),
        xpToNextLevel: Math.max(nextLevelXP - profile.totalXP, 0)
      }
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Student Roadmap (Progress Gates)            │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getStudentRoadmap(req, res, next) {
  try {
    const { id: studentId } = req.user;

    // Get main course Lessons only (no courseId)
    const lessons = await prisma.lesson.findMany({
      where: { courseId: null },
      select: {
        id: true,
        title: true,
        order: true,
        contentSlug: true,
      },
      orderBy: { order: "asc" },
      take: 100
    });

    // Get main course Challenges only (no courseId)
    const challenges = await prisma.challenge.findMany({
      where: { courseId: null },
      select: {
        id: true,
        title: true,
        points: true,
        xpReward: true,
        tier: true,
        order: true,
        difficulty: true,
        submissions: {
          where: { studentId },
          select: { passed: true, createdAt: true }
        }
      },
      orderBy: [{ tier: "asc" }, { order: "asc" }],
      take: 210
    });

    // Get student name for dynamic output
    const student = await prisma.user.findUnique({ where: { id: studentId }, select: { name: true } });
    const studentName = student ? student.name : "";

    // Get all Projects for this student
    const projects = await prisma.project.findMany({
      where: { studentId },
      select: {
        id: true,
        title: true,
        description: true,
        lessonId: true,
        status: true,
        score: true,
      },
      take: 100
    });

    // Calculate progress for each lesson
    const lessonsWithProgress = lessons.map(lesson => {
      const lessonChallenges = challenges.filter(c => c.tier === lesson.order);
      const totalExercises = lessonChallenges.length;
      const completedExercises = lessonChallenges.filter(c =>
        c.submissions.some(s => s.passed)
      ).length;
      const completionRate = totalExercises > 0 ? (completedExercises / totalExercises) * 100 : 0;
      const earnedPoints = lessonChallenges.reduce((sum, c) => {
        return sum + (c.submissions.some(s => s.passed) ? c.points : 0);
      }, 0);
      const totalPoints = lessonChallenges.reduce((sum, c) => sum + c.points, 0);

      let status = "LOCKED";
      if (lesson.order === 1) {
        status = "UNLOCKED";
      } else {
        const previousLesson = lessons.find(l => l.order === lesson.order - 1);
        if (previousLesson) {
          const prevChallenges = challenges.filter(c => c.tier === previousLesson.order);
          const prevCompleted = prevChallenges.filter(c =>
            c.submissions.some(s => s.passed)
          ).length;
          const prevCompletionRate = prevChallenges.length > 0 ? (prevCompleted / prevChallenges.length) * 100 : 0;
          status = prevCompletionRate >= 80 ? "UNLOCKED" : "LOCKED";
        }
      }

      if (completionRate >= 100) {
        status = "COMPLETED";
      }

      return {
        ...lesson, status, totalExercises, completedExercises,
        completionRate: Math.round(completionRate),
        earnedPoints, totalPoints,
        challenges: lessonChallenges.sort((a, b) => a.order - b.order).map((c, idx) => {
          const passed = c.submissions.some(s => s.passed);
          // Sequential locking: first challenge always unlocked, rest require previous to be passed
          let challengeStatus = "LOCKED";
          if (idx === 0) {
            challengeStatus = status === "LOCKED" ? "LOCKED" : "UNLOCKED";
          } else {
            const prevChallenge = lessonChallenges.sort((a, b) => a.order - b.order)[idx - 1];
            const prevPassed = prevChallenge.submissions.some(s => s.passed);
            if (status === "LOCKED") {
              challengeStatus = "LOCKED";
            } else if (passed) {
              challengeStatus = "COMPLETED";
            } else if (prevPassed) {
              challengeStatus = "UNLOCKED";
            } else {
              challengeStatus = "LOCKED";
            }
          }
          if (passed && challengeStatus !== "LOCKED") challengeStatus = "COMPLETED";
          
          // SECURITY: expectedOutput is NEVER sent to students
          return {
            id: c.id,
            title: c.title,
            points: c.points,
            xpReward: c.xpReward || c.points,
            difficulty: c.difficulty,
            order: c.order,
            passed,
            submitted: c.submissions.length > 0,
            status: challengeStatus,
            isPractice: /المثال/.test(c.title),
          };
        }),
        eligibleForNext: status === "COMPLETED" || (
          status === "UNLOCKED" && lesson.order < lessons.length
        )
      };
    });

    // Calculate progress for each project
    const projectsWithProgress = projects.map(project => {
      const completed = project.status === "COMPLETED" || project.status === "SUBMITTED";
      const status = completed ? "COMPLETED" : "LOCKED";

      return {
        ...project,
        status,
        submitted: project.status === "SUBMITTED",
        completed,
        eligibleForNext: status === "COMPLETED" && (
          projects.findIndex(p => p.id === project.id) < projects.length - 1
        )
      };
    });

    // Recommendations
    const nextUnlockedLesson = lessonsWithProgress.find(l => l.status === "UNLOCKED");
    const nextLockedLesson = lessonsWithProgress.find(l => l.status === "LOCKED");

    res.json({
      success: true,
      roadmap: {
        lessons: lessonsWithProgress,
        projects: projectsWithProgress,
        
        nextSteps: {
          nextLesson: nextUnlockedLesson?.title || "",
          recommendedProjects: projectsWithProgress.filter(p => p.eligibleForNext).slice(0, 3),
          timeUntilNextUnlock: nextLockedLesson ? 
            "Based on prerequisite completion" : "All lessons unlocked"
        },
        
        overallProgress: {
          totalLessons: lessons.length,
          completedLessons: lessonsWithProgress.filter(l => l.status === "COMPLETED").length,
          totalProjects: projects.length,
          completedProjects: projectsWithProgress.filter(p => p.status === "COMPLETED").length,
          avgCompletionRate: (
            lessonsWithProgress.reduce((sum, l) => sum + l.completionRate, 0) / lessons.length
          ) || 0,
          totalPoints: lessonsWithProgress.reduce((sum, l) => sum + l.totalPoints, 0),
          earnedPoints: lessonsWithProgress.reduce((sum, l) => sum + l.earnedPoints, 0),
          totalChallenges: lessonsWithProgress.reduce((sum, l) => sum + l.totalExercises, 0),
          completedChallenges: lessonsWithProgress.reduce((sum, l) => sum + l.completedExercises, 0)
        },
        
        lastUpdated: new Date().toISOString()
      }
    });
  } catch (error) {
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Handle XP Awarding Trigger (Judge Service Call) │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function handleXPAwarding(submissionData) {
  try {
    const studentId = submissionData.studentId || 
                     (submissionData.submission?.studentId);
    
    if (!studentId) {
      logger.warn("لم يتم العثور على معرف الطالب في بيانات الإرسال");
      return { success: false, message: "Missing studentId" };
    }

    const result = await awardXP(submissionData, studentId);
    
    return result;
  } catch (error) {
    logger.error('Failed to handle XP awarding:', error.message);
    return { success: false, error: error.message };
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Bulk Telemetry Sync (Offline-First)             │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function processBulkTelemetry(telemetryArray, studentId) {
  const results = {
    total: telemetryArray.length,
    successful: 0,
    failed: 0,
    xpAwarded: 0,
    errors: []
  };

  // SECURITY: Never trust client-provided telemetry for XP/grading
  // Telemetry is recorded but NOT used for XP or score calculations
  // XP must come from verified judge callbacks only

  // Use Prisma transaction for atomicity
  await prisma.$transaction(async (tx) => {
    for (const item of telemetryArray) {
      try {
        // Validate each item
        if (!item.challengeId || !item.exerciseId) {
          results.failed++;
          results.errors.push({
            index: results.successful + results.failed - 1,
            error: 'Missing required fields'
          });
          continue;
        }

        // Create submission record (NEVER trust client for passed/score)
        const submission = await tx.submission.create({
          data: {
            challengeId: item.challengeId,
            studentId: studentId,
            code: item.code || '',
            language: item.language || 'daad',
            passed: false, // SECURITY: Always false until server verification
            output: '', // SECURITY: Never store client-provided output
            executionTime: 0, // SECURITY: Never trust client execution time
            syntaxScore: 0, // SECURITY: Never trust client scores
            performanceScore: 0 // SECURITY: Never trust client scores
          }
        });

        // Create student submission
        await tx.studentSubmission.create({
          data: {
            submissionId: submission.id,
            score: 0, // SECURITY: Never trust client scores
            completionTime: 0,
            tokenCount: item.code ? Math.floor(item.code.length / 4) : 0,
            astNodesCount: 0,
            errorCount: 0,
            warningCount: 0,
            metadata: { originalTelemetry: item } // Store original for audit
          }
        });

        // SECURITY: NO XP awarded from client telemetry
        // XP must come from verified judge callbacks only

        // Log activity
        await tx.activityLog.create({
          data: {
            userId: studentId,
            action: 'bulk_submission_recorded',
            details: JSON.stringify({
              exerciseId: item.exerciseId,
              challengeId: item.challengeId,
              status: 'recorded_not_verified'
            })
          }
        });

        results.successful++;
      } catch (error) {
        results.failed++;
        results.errors.push({
          index: results.successful + results.failed - 1,
          error: error.message
        });
      }
    }
  });

  return results;
}

module.exports = {
  getStudentProfile,
  getStudentRoadmap,
  handleXPAwarding,
  processBulkTelemetry,
  calculateLevel,
  awardXP,
  checkAndAwardAchievements
};
