const { logger } = require("../utils/logger");

// GET /api/v1/schools — list schools
async function getSchools(req, res, next) {
  try {
    const prisma = req.app.get("prisma");

    // Non-admins can only see their own school
    const where =
      req.user.role === "ADMIN"
        ? {}
        : { id: req.user.schoolId };

    const schools = await prisma.school.findMany({
      where,
      include: {
        _count: {
          select: { users: true, courses: true },
        },
      },
      orderBy: { provisionedAt: "desc" },
      take: 100
    });

    res.json({
      schools: schools.map((s) => ({
        id: s.id,
        name: s.name,
        provisionedAt: s.provisionedAt,
        userCount: s._count.users,
        courseCount: s._count.courses,
      })),
    });
  } catch (err) {
    next(err);
  }
}

// GET /api/v1/schools/:id — get school details
async function getSchoolById(req, res, next) {
  try {
    const { id } = req.params;
    const prisma = req.app.get("prisma");
    const isAdmin = req.user.role === "ADMIN";

    const school = await prisma.school.findUnique({
      where: { id },
      include: {
        users: isAdmin ? {
          select: {
            id: true,
            name: true,
            role: true,
            email: true,
            nationalId: true,
            createdAt: true,
          },
          orderBy: [{ role: "asc" }, { name: "asc" }],
        } : false,
        _count: {
          select: { users: true, courses: true },
        },
      },
    });

    if (!school) {
      return res.status(404).json({
        error: "غير موجود",
        message: "المدرسة غير موجودة",
      });
    }

    const response = {
      id: school.id,
      name: school.name,
      provisionedAt: school.provisionedAt,
      stats: {
        teachers: school._count.users,
        students: school._count.users,
        courses: school._count.courses,
      },
    };

    // Only admins see the full user list
    if (isAdmin && school.users) {
      response.teachers = school.users.filter((u) => u.role === "TEACHER");
      response.students = school.users.filter((u) => u.role === "STUDENT");
      response.stats.teachers = response.teachers.length;
      response.stats.students = response.students.length;
    }

    res.json(response);
  } catch (err) {
    next(err);
  }
}

// GET /api/v1/schools/:id/users — list users in a school
async function listSchoolUsers(req, res, next) {
  try {
    const { id } = req.params;
    const prisma = req.app.get("prisma");
    const isAdmin = req.user.role === "ADMIN";

    // Non-admins cannot list all users - only admins can
    if (!isAdmin) {
      return res.status(403).json({
        error: "محظور",
        message: "فقط المسؤولون يمكنهم عرض قائمة المستخدمين",
      });
    }

    const school = await prisma.school.findUnique({
      where: { id },
      include: {
        users: {
          select: {
            id: true,
            name: true,
            role: true,
            email: true,
            nationalId: true,
            createdAt: true,
          },
          orderBy: [{ role: "asc" }, { name: "asc" }],
        },
      },
    });

    if (!school) {
      return res.status(404).json({
        error: "غير موجود",
        message: "المدرسة غير موجودة",
      });
    }

    res.json({
      school: { id: school.id, name: school.name },
      users: school.users,
      stats: {
        teachers: school.users.filter((u) => u.role === "TEACHER").length,
        students: school.users.filter((u) => u.role === "STUDENT").length,
        total: school.users.length,
      },
    });
  } catch (err) {
    next(err);
  }
}

// PUT /api/v1/schools/:id — update school (ADMIN ONLY)
async function updateSchool(req, res, next) {
  try {
    const { id } = req.params;
    const { name } = req.body;
    const prisma = req.app.get("prisma");

    if (!name || typeof name !== "string" || name.trim().length === 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "الاسم مطلوب ويجب أن يكون نصاً غير فارغ",
      });
    }

    if (name.length > 200) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "يجب أن لا يتجاوز الاسم 200 حرف",
      });
    }

    const school = await prisma.school.findUnique({ where: { id } });
    if (!school) {
      return res.status(404).json({
        error: "غير موجود",
        message: "المدرسة غير موجودة",
      });
    }

    const updated = await prisma.school.update({
      where: { id },
      data: { name: name.trim() },
    });

    logger.info(`[Admin] School "${school.name}" renamed to "${updated.name}" by ${req.user.name}`);

    res.json({
      message: "تم تحديث المدرسة",
      school: {
        id: updated.id,
        name: updated.name,
        provisionedAt: updated.provisionedAt,
      },
    });
  } catch (err) {
    next(err);
  }
}

// DELETE /api/v1/schools/:id — delete school + cascade all users (ADMIN ONLY)
async function deleteSchool(req, res, next) {
  try {
    const { id } = req.params;
    const prisma = req.app.get("prisma");

    const school = await prisma.school.findUnique({
      where: { id },
      include: {
        _count: {
          select: { users: true, courses: true },
        },
      },
    });

    if (!school) {
      return res.status(404).json({
        error: "غير موجود",
        message: "المدرسة غير موجودة",
      });
    }

    // Prevent deleting the last school
    const totalSchools = await prisma.school.count();
    if (totalSchools <= 1) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "لا يمكن حذف المدرسة الأخيرة",
      });
    }

    await prisma.school.delete({ where: { id } });

    logger.warn(`[Admin] School "${school.name}" deleted by ${req.user.name} — ${school._count.users} users, ${school._count.courses} courses removed`);

    res.json({
      message: `School "${school.name}" deleted`,
      deleted: {
        users: school._count.users,
        courses: school._count.courses,
      },
    });
  } catch (err) {
    next(err);
  }
}

// DELETE /api/v1/schools/:id/users/:userId — delete a specific user (ADMIN ONLY)
async function deleteUser(req, res, next) {
  try {
    const { id, userId } = req.params;
    const prisma = req.app.get("prisma");

    const user = await prisma.user.findUnique({
      where: { id: userId },
      include: { school: { select: { name: true } } },
    });

    if (!user || user.schoolId !== id) {
      return res.status(404).json({
        error: "غير موجود",
        message: "المستخدم غير موجود في هذه المدرسة",
      });
    }

    // Prevent deleting yourself
    if (user.id === req.user.id) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "لا يمكنك حذف حسابك الخاص",
      });
    }

    await prisma.user.delete({ where: { id: userId } });

    logger.warn(`[Admin] User "${user.name}" (${user.role}) deleted from "${user.school.name}" by ${req.user.name}`);

    res.json({
      message: `User "${user.name}" deleted`,
    });
  } catch (err) {
    next(err);
  }
}

module.exports = {
  getSchools,
  getSchoolById,
  listSchoolUsers,
  updateSchool,
  deleteSchool,
  deleteUser,
};
