// ── Helpers ──────────────────────────────────────────────────────────────────
function startOfWeek(date) {
  const d = new Date(date);
  const day = d.getDay(); // 0=Sun
  d.setDate(d.getDate() - day);
  d.setHours(0, 0, 0, 0);
  return d;
}

function endOfWeek(date) {
  const d = startOfWeek(date);
  d.setDate(d.getDate() + 6);
  d.setHours(23, 59, 59, 999);
  return d;
}

function daysAgo(n) {
  const d = new Date();
  d.setDate(d.getDate() - n);
  d.setHours(0, 0, 0, 0);
  return d;
}

function fmtDate(date) {
  return date.toISOString().split("T")[0];
}

function fmtDayName(date) {
  const days = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
  return days[date.getDay()];
}

// ── POST /api/v1/reports/weekly — Weekly activity report for a school ────────
async function weeklyReport(req, res, next) {
  try {
    const prisma = req.app.get("prisma");
    const { role, schoolId } = req.user;
    const targetSchoolId = req.params.schoolId || schoolId;

    if (role !== "ADMIN" && targetSchoolId !== schoolId) {
      return res.status(403).json({ error: "محظور", message: "تم رفض الوصول" });
    }

    const now = new Date();
    const weekStart = startOfWeek(now);
    const weekEnd = endOfWeek(now);

    const MAX_REPORT_USERS = 5000;
    const MAX_REPORT_LOGS = 10000;

    const users = await prisma.user.findMany({
      where: { schoolId: targetSchoolId },
      select: { id: true, name: true, role: true },
      take: MAX_REPORT_USERS,
    });

    const studentIds = users.filter((u) => u.role === "STUDENT").map((u) => u.id);
    const teacherIds = users.filter((u) => u.role === "TEACHER").map((u) => u.id);
    const allIds = [...studentIds, ...teacherIds];

    if (allIds.length === 0) {
      return res.json({
        school: { id: targetSchoolId },
        period: { from: fmtDate(weekStart), to: fmtDate(weekEnd) },
        summary: { totalStudents: 0, totalTeachers: 0, activeStudents: 0, activeTeachers: 0,
          studentActivityRate: 0, teacherActivityRate: 0, totalSubmissions: 0,
          passedSubmissions: 0, passRate: 0, totalChallenges: 0, publishedChallenges: 0 },
        dailyBreakdown: [],
        userStats: [],
      });
    }

    // SECURITY: Parallel queries instead of sequential (reduces latency ~4x)
    const [loginsThisWeek, activitiesThisWeek, submissionsThisWeek, challengesThisWeek] = await Promise.all([
      prisma.loginLog.findMany({
        where: { userId: { in: allIds }, createdAt: { gte: weekStart, lte: weekEnd }, success: true },
        select: { userId: true, createdAt: true },
        take: MAX_REPORT_LOGS,
      }),
      prisma.activityLog.findMany({
        where: { userId: { in: allIds }, createdAt: { gte: weekStart, lte: weekEnd } },
        select: { userId: true, action: true, createdAt: true },
        take: MAX_REPORT_LOGS,
      }),
      prisma.submission.findMany({
        where: { studentId: { in: studentIds }, createdAt: { gte: weekStart, lte: weekEnd } },
        select: { studentId: true, passed: true, createdAt: true },
        take: MAX_REPORT_LOGS,
      }),
      prisma.challenge.findMany({
        where: { authorId: { in: teacherIds }, createdAt: { gte: weekStart, lte: weekEnd } },
        select: { id: true, published: true },
        take: MAX_REPORT_USERS,
      }),
    ]);

    // SECURITY: Pre-index data by userId/studentId for O(1) lookups instead of O(N²) filtering
    const loginsByUser = new Map();
    for (const l of loginsThisWeek) {
      if (!loginsByUser.has(l.userId)) loginsByUser.set(l.userId, []);
      loginsByUser.get(l.userId).push(l);
    }
    const activitiesByUser = new Map();
    for (const a of activitiesThisWeek) {
      if (!activitiesByUser.has(a.userId)) activitiesByUser.set(a.userId, []);
      activitiesByUser.get(a.userId).push(a);
    }
    const submissionsByUser = new Map();
    for (const s of submissionsThisWeek) {
      if (!submissionsByUser.has(s.studentId)) submissionsByUser.set(s.studentId, []);
      submissionsByUser.get(s.studentId).push(s);
    }
    const userMap = new Map(users.map(u => [u.id, u]));

    const userStats = allIds.map((uid) => {
      const user = userMap.get(uid);
      const userLogins = loginsByUser.get(uid) || [];
      const uniqueDays = new Set(userLogins.map((l) => fmtDate(l.createdAt))).size;
      const userActivities = activitiesByUser.get(uid) || [];
      const userSubmissions = submissionsByUser.get(uid) || [];

      return {
        userId: uid,
        name: user?.name,
        role: user?.role,
        loginDays: uniqueDays,
        totalLogins: userLogins.length,
        activities: userActivities.length,
        submissions: userSubmissions.length,
        passedSubmissions: userSubmissions.filter((s) => s.passed).length,
        interactionRate: Math.round((uniqueDays / 7) * 100),
      };
    });

    const students = userStats.filter((u) => u.role === "STUDENT");
    const teachers = userStats.filter((u) => u.role === "TEACHER");
    const activeStudents = students.filter((s) => s.totalLogins > 0).length;
    const activeTeachers = teachers.filter((t) => t.totalLogins > 0).length;

    // SECURITY: Pre-index by date string for O(1) lookups in dailyBreakdown
    const loginsByDate = new Map();
    const activitiesByDate = new Map();
    const submissionsByDate = new Map();
    for (const l of loginsThisWeek) { const d = fmtDate(l.createdAt); loginsByDate.set(d, (loginsByDate.get(d) || 0) + 1); }
    for (const a of activitiesThisWeek) { const d = fmtDate(a.createdAt); activitiesByDate.set(d, (activitiesByDate.get(d) || 0) + 1); }
    for (const s of submissionsThisWeek) { const d = fmtDate(s.createdAt); submissionsByDate.set(d, (submissionsByDate.get(d) || 0) + 1); }

    const dailyBreakdown = [];
    for (let i = 0; i < 7; i++) {
      const day = new Date(weekStart);
      day.setDate(day.getDate() + i);
      const dayStr = fmtDate(day);

      dailyBreakdown.push({
        date: dayStr,
        dayName: fmtDayName(day),
        logins: loginsByDate.get(dayStr) || 0,
        activities: activitiesByDate.get(dayStr) || 0,
        submissions: submissionsByDate.get(dayStr) || 0,
        uniqueUsers: 0,
      });
    }

    res.json({
      school: { id: targetSchoolId },
      period: { from: fmtDate(weekStart), to: fmtDate(weekEnd) },
      summary: {
        totalStudents: students.length,
        totalTeachers: teachers.length,
        activeStudents,
        activeTeachers,
        studentActivityRate: students.length > 0 ? Math.round((activeStudents / students.length) * 100) : 0,
        teacherActivityRate: teachers.length > 0 ? Math.round((activeTeachers / teachers.length) * 100) : 0,
        totalSubmissions: submissionsThisWeek.length,
        passedSubmissions: submissionsThisWeek.filter((s) => s.passed).length,
        passRate: submissionsThisWeek.length > 0 ? Math.round((submissionsThisWeek.filter((s) => s.passed).length / submissionsThisWeek.length) * 100) : 0,
        totalChallenges: challengesThisWeek.length,
        publishedChallenges: challengesThisWeek.filter((c) => c.published).length,
      },
      dailyBreakdown,
      userStats: userStats.sort((a, b) => b.activities - a.activities),
    });
  } catch (err) {
    next(err);
  }
}

// ── GET /api/v1/reports/activity/:userId — Detailed user activity log ────────
async function userActivity(req, res, next) {
  try {
    const prisma = req.app.get("prisma");
    const { role, schoolId } = req.user;
    const { userId } = req.params;

    const targetUser = await prisma.user.findUnique({
      where: { id: userId },
      select: { schoolId: true, name: true, role: true },
    });

    if (!targetUser) {
      return res.status(404).json({ error: "غير موجود", message: "المستخدم غير موجود" });
    }

    if (role !== "ADMIN" && targetUser.schoolId !== schoolId) {
      return res.status(403).json({ error: "محظور", message: "تم رفض الوصول" });
    }

    if (role === "STUDENT" && userId !== req.user.id) {
      return res.status(403).json({ error: "محظور", message: "تم رفض الوصول" });
    }

    const days = parseInt(req.query.days) || 30;
    const since = daysAgo(days);

    const logins = await prisma.loginLog.findMany({
      where: { userId, createdAt: { gte: since }, success: true },
      orderBy: { createdAt: "desc" },
      take: 200,
    });

    const activities = await prisma.activityLog.findMany({
      where: { userId, createdAt: { gte: since } },
      orderBy: { createdAt: "desc" },
      take: 200,
    });

    const submissions = await prisma.submission.findMany({
      where: { studentId: userId, createdAt: { gte: since } },
      include: { challenge: { select: { id: true, title: true } } },
      orderBy: { createdAt: "desc" },
      take: 200,
    });

    const activeDays = new Set(logins.map((l) => fmtDate(l.createdAt))).size;

    const actionBreakdown = {};
    for (const a of activities) {
      actionBreakdown[a.action] = (actionBreakdown[a.action] || 0) + 1;
    }

    res.json({
      user: { id: userId, name: targetUser.name, role: targetUser.role },
      period: { days, since: fmtDate(since) },
      stats: {
        activeDays,
        totalLogins: logins.length,
        totalActivities: activities.length,
        totalSubmissions: submissions.length,
        passedSubmissions: submissions.filter((s) => s.passed).length,
        activityRate: Math.round((activeDays / days) * 100),
      },
      actionBreakdown,
      recentLogins: logins.slice(0, 50),
      recentActivities: activities.slice(0, 50),
      recentSubmissions: submissions.slice(0, 50),
    });
  } catch (err) {
    next(err);
  }
}

// ── GET /api/v1/reports/school-overview — Full school overview ───────────────
async function schoolOverview(req, res, next) {
  try {
    const prisma = req.app.get("prisma");
    const { role, schoolId } = req.user;
    const targetSchoolId = req.params.schoolId || schoolId;

    if (role !== "ADMIN" && targetSchoolId !== schoolId) {
      return res.status(403).json({ error: "محظور", message: "تم رفض الوصول" });
    }

    const todayStart = new Date();
    todayStart.setHours(0, 0, 0, 0);
    const weekAgo = daysAgo(7);
    const monthAgo = daysAgo(30);

    const MAX_REPORT_USERS = 5000;

    const users = await prisma.user.findMany({
      where: { schoolId: targetSchoolId },
      select: { id: true, name: true, role: true },
      take: MAX_REPORT_USERS,
    });

    const studentIds = users.filter((u) => u.role === "STUDENT").map((u) => u.id);
    const teacherIds = users.filter((u) => u.role === "TEACHER").map((u) => u.id);
    const allIds = [...studentIds, ...teacherIds];

    const [todayLogins, weekLogins, weekSubmissions, weekPassed, monthLogins, monthSubmissions, totalChallenges, publishedChallenges] = await Promise.all([
      prisma.loginLog.count({ where: { userId: { in: allIds }, createdAt: { gte: todayStart }, success: true } }),
      prisma.loginLog.count({ where: { userId: { in: allIds }, createdAt: { gte: weekAgo }, success: true } }),
      prisma.submission.count({ where: { studentId: { in: studentIds }, createdAt: { gte: weekAgo } } }),
      prisma.submission.count({ where: { studentId: { in: studentIds }, createdAt: { gte: weekAgo }, passed: true } }),
      prisma.loginLog.count({ where: { userId: { in: allIds }, createdAt: { gte: monthAgo }, success: true } }),
      prisma.submission.count({ where: { studentId: { in: studentIds }, createdAt: { gte: monthAgo } } }),
      prisma.challenge.count({ where: { authorId: { in: teacherIds } } }),
      prisma.challenge.count({ where: { authorId: { in: teacherIds }, published: true } }),
    ]);

    const mostActive = await prisma.activityLog.groupBy({
      by: ["userId"],
      where: { userId: { in: allIds }, createdAt: { gte: weekAgo } },
      _count: { id: true },
      orderBy: { _count: { id: "desc" } },
      take: 10,
    });

    const activeUserIds = mostActive.map((a) => a.userId);
    const activeUsers = await prisma.user.findMany({
      take: 50,
      where: { id: { in: activeUserIds } },
      select: { id: true, name: true, role: true },
    });

    const topUsers = mostActive.map((a) => {
      const user = activeUsers.find((u) => u.id === a.userId);
      return { userId: a.userId, name: user?.name || "غير معروف", role: user?.role || "غير معروف", actions: a._count.id };
    });

    res.json({
      school: { id: targetSchoolId },
      totals: { students: studentIds.length, teachers: teacherIds.length, challenges: totalChallenges, publishedChallenges },
      today: { logins: todayLogins },
      thisWeek: { logins: weekLogins, submissions: weekSubmissions, passedSubmissions: weekPassed, passRate: weekSubmissions > 0 ? Math.round((weekPassed / weekSubmissions) * 100) : 0 },
      thisMonth: { logins: monthLogins, submissions: monthSubmissions },
      topUsers,
    });
  } catch (err) {
    next(err);
  }
}

module.exports = { weeklyReport, userActivity, schoolOverview };
