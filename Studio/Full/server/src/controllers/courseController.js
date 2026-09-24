const prisma = require("../utils/prisma");
const { logger } = require("../utils/logger");

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get All Additional Courses                                         │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getCourses(req, res, next) {
  try {
    const { id: studentId, schoolId } = req.user;

    const courses = await prisma.course.findMany({
      where: { published: true, category: { not: 'MAIN' } },
      take: 200,
      select: {
        id: true,
        title: true,
        description: true,
        icon: true,
        category: true,
        order: true,
        unlockTier: true,
        _count: { select: { lessons: true } }
      },
      orderBy: { order: 'asc' }
    });

    const enrollments = await prisma.studentCourseEnrollment.findMany({
      where: { studentId },
      select: { courseId: true, enrolledAt: true }
    });
    const enrolledIds = new Set(enrollments.map(e => e.courseId));

    // Get student's max completed tier from main course
    const mainChallenges = await prisma.challenge.findMany({
      where: { courseId: null },
      take: 210,
      select: { tier: true, submissions: { where: { studentId, passed: true }, select: { id: true } } }
    });
    const completedTiers = new Set();
    mainChallenges.forEach(c => { if (c.submissions.length > 0) completedTiers.add(c.tier); });
    const maxCompletedTier = completedTiers.size > 0 ? [...completedTiers].reduce((a, b) => Math.max(a, b), -Infinity) : 0;

    const result = courses.map(c => {
      const enrolled = enrolledIds.has(c.id);
      const unlocked = maxCompletedTier >= c.unlockTier;
      return {
        ...c,
        lessonCount: c._count.lessons,
        enrolled,
        unlocked,
        status: enrolled ? 'ENROLLED' : unlocked ? 'UNLOCKED' : 'LOCKED'
      };
    });

    res.json({ success: true, courses: result });
  } catch (error) {
    logger.error('getCourses error:', error);
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Enroll in a Course                                                  │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function enrollCourse(req, res, next) {
  try {
    const { id: studentId } = req.user;
    const { courseId } = req.params;

    const course = await prisma.course.findUnique({ where: { id: courseId } });
    if (!course) return res.status(404).json({ success: false, message: 'الدورة غير موجودة' });

    // Check unlock requirement
    const mainChallenges = await prisma.challenge.findMany({
      where: { courseId: null },
      take: 210,
      select: { tier: true, submissions: { where: { studentId, passed: true }, select: { id: true } } }
    });
    const completedTiers = new Set();
    mainChallenges.forEach(c => { if (c.submissions.length > 0) completedTiers.add(c.tier); });
    const maxCompletedTier = completedTiers.size > 0 ? [...completedTiers].reduce((a, b) => Math.max(a, b), -Infinity) : 0;

    if (maxCompletedTier < course.unlockTier) {
      return res.status(403).json({
        success: false,
        message: `يجب إكمال المستوى ${course.unlockTier} أولاً`
      });
    }

    const existing = await prisma.studentCourseEnrollment.findUnique({
      where: { studentId_courseId: { studentId, courseId } }
    });
    if (existing) {
      return res.json({ success: true, message: 'مسجّل مسبقاً', enrollment: existing });
    }

    const enrollment = await prisma.studentCourseEnrollment.create({
      data: { studentId, courseId }
    });

    res.json({ success: true, enrollment });
  } catch (error) {
    logger.error('enrollCourse error:', error);
    next(error);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Get Course Roadmap (Lessons + Challenges)                          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

async function getCourseRoadmap(req, res, next) {
  try {
    const { id: studentId } = req.user;
    const { courseId } = req.params;

    const course = await prisma.course.findUnique({ where: { id: courseId } });
    if (!course) return res.status(404).json({ success: false, message: 'الدورة غير موجودة' });

    // Check enrollment
    const enrollment = await prisma.studentCourseEnrollment.findUnique({
      where: { studentId_courseId: { studentId, courseId } }
    });
    if (!enrollment) return res.status(403).json({ success: false, message: 'غير مسجّل في الدورة' });

    // Get lessons for this course
    const lessons = await prisma.lesson.findMany({
      where: { courseId },
      take: 100,
      select: { id: true, title: true, order: true, contentSlug: true },
      orderBy: { order: 'asc' }
    });

    // Get challenges for this course
    const challenges = await prisma.challenge.findMany({
      where: { courseId },
      take: 200,
      select: {
        id: true, title: true, description: true, starterCode: true,
        points: true, xpReward: true,
        tier: true, order: true, difficulty: true,
        submissions: { where: { studentId }, select: { passed: true } }
      },
      orderBy: [{ tier: 'asc' }, { order: 'asc' }]
    });

    // Build lessons with progress
    const lessonsWithProgress = lessons.map(lesson => {
      const lessonChallenges = challenges.filter(c => c.tier === lesson.order);
      const completedCount = lessonChallenges.filter(c => c.submissions.some(s => s.passed)).length;
      const totalExercises = lessonChallenges.length;
      const completionRate = totalExercises > 0 ? Math.round((completedCount / totalExercises) * 100) : 0;
      const earnedPoints = lessonChallenges.reduce((sum, c) => {
        return sum + (c.submissions.some(s => s.passed) ? (c.xpReward || c.points || 0) : 0);
      }, 0);
      const totalPoints = lessonChallenges.reduce((sum, c) => sum + (c.xpReward || c.points || 0), 0);

      return {
        id: lesson.id, title: lesson.title, order: lesson.order,
        contentSlug: lesson.contentSlug,
        totalExercises, completedExercises: completedCount,
        completionRate, earnedPoints, totalPoints,
        challenges: lessonChallenges.map(c => ({
          id: c.id, title: c.title, description: c.description,
          starterCode: c.starterCode, points: c.points, xpReward: c.xpReward,
          tier: c.tier, order: c.order, difficulty: c.difficulty,
          passed: c.submissions.some(s => s.passed),
          submitted: c.submissions.length > 0
        }))
      };
    });

    // Locking logic
    lessonsWithProgress.forEach((lesson, idx) => {
      if (idx === 0) {
        lesson.status = 'UNLOCKED';
      } else {
        const prev = lessonsWithProgress[idx - 1];
        lesson.status = prev.completionRate >= 80 ? 'UNLOCKED' : 'LOCKED';
      }
      if (lesson.completionRate >= 100) lesson.status = 'COMPLETED';

      // Challenge locking within lesson
      lesson.challenges.forEach((ch, ci) => {
        if (lesson.status === 'LOCKED') {
          ch.status = 'LOCKED';
        } else if (ci === 0) {
          ch.status = ch.passed ? 'COMPLETED' : 'UNLOCKED';
        } else {
          const prev = lesson.challenges[ci - 1];
          ch.status = prev.passed ? (ch.passed ? 'COMPLETED' : 'UNLOCKED') : 'LOCKED';
          if (ch.passed) ch.status = 'COMPLETED';
        }
      });
    });

    const completedLessons = lessonsWithProgress.filter(l => l.status === 'COMPLETED').length;
    const totalEarned = lessonsWithProgress.reduce((s, l) => s + l.earnedPoints, 0);
    const totalPoints = lessonsWithProgress.reduce((s, l) => s + l.totalPoints, 0);
    const totalChallenges = challenges.length;
    const completedChallenges = challenges.filter(c => c.submissions.some(s => s.passed)).length;

    res.json({
      success: true,
      roadmap: {
        course: { id: course.id, title: course.title, description: course.description, icon: course.icon, category: course.category },
        lessons: lessonsWithProgress,
        overallProgress: {
          totalLessons: lessons.length, completedLessons,
          totalChallenges, completedChallenges,
          totalPoints, earnedPoints: totalEarned,
          avgCompletionRate: lessons.length > 0 ? Math.round(lessonsWithProgress.reduce((s, l) => s + l.completionRate, 0) / lessons.length) : 0
        }
      }
    });
  } catch (error) {
    logger.error('getCourseRoadmap error:', error);
    next(error);
  }
}

module.exports = { getCourses, enrollCourse, getCourseRoadmap };
