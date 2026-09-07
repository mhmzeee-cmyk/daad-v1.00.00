const crypto = require("crypto");
const bcrypt = require("bcryptjs");
const prisma = require("../utils/prisma");
const { logger } = require("../utils/logger");

const OTP_EXPIRY_MINUTES = 10;
const BCRYPT_ROUNDS = 12;

// ── Import Students (Bulk Invitation) ──────────────────────────────────────
async function importStudents(studentList, schoolId, classroomId) {
  if (!Array.isArray(studentList) || studentList.length === 0) {
    throw new Error("قائمة الطلاب يجب أن تكون مصفوفة غير فارغة");
  }

  if (studentList.length > 100) {
    throw new Error("الحد الأقصى 100 طالب لكل استيراد");
  }

  const school = await prisma.school.findUnique({ where: { id: schoolId } });
  if (!school) {
    throw new Error("المدرسة غير موجودة");
  }

  const results = {
    successful: [],
    failed: [],
    total: studentList.length,
  };

  for (const item of studentList) {
    const name = typeof item === "string" ? item.trim() : item.name?.trim();
    const email =
      typeof item === "string"
        ? `${item.trim().replace(/\s+/g, "").toLowerCase()}@dhad.local`
        : item.email?.trim();

    if (!name || name.length === 0) {
      results.failed.push({ name: item, reason: "اسم فارغ" });
      continue;
    }

    if (!email || email.length === 0) {
      results.failed.push({ name, reason: "بريد إلكتروني فارغ" });
      continue;
    }

    try {
      // Check if already invited in this school
      const existing = await prisma.invitation.findUnique({
        where: {
          schoolId_studentEmail: {
            schoolId,
            studentEmail: email,
          },
        },
      });

      if (existing) {
        results.failed.push({ name, email, reason: "مدعو بالفعل" });
        continue;
      }

      // Generate OTP
      const otp = generateOTP();
      const otpHash = await bcrypt.hash(otp, BCRYPT_ROUNDS);
      const expiry = new Date(Date.now() + OTP_EXPIRY_MINUTES * 60 * 1000);

      const invitation = await prisma.invitation.create({
        data: {
          studentEmail: email,
          studentName: name,
          classroomId: classroomId || null,
          schoolId,
          schoolName: school.name,
          token: otpHash,
          tokenExpiry: expiry,
          isActivated: false,
        },
      });

      results.successful.push({
        id: invitation.id,
        name,
        email,
        // SECURITY: OTP is never returned in API response — admin shares it out-of-band
      });
    } catch (err) {
      results.failed.push({ name, email, reason: err.message });
    }
  }

  logger.info(`Import: ${results.successful.length}/${results.total} successful for school ${schoolId}`);

  return results;
}

// ── Generate 6-Digit OTP ───────────────────────────────────────────────────
function generateOTP() {
  return crypto.randomInt(100000, 999999).toString();
}

// ── Request OTP (Student or Teacher) ────────────────────────────────────────
async function requestOTP(identifier) {
  if (!identifier || typeof identifier !== "string") {
    throw new Error("البريد الإلكتروني أو الرقم الوطني مطلوب");
  }

  const trimmed = identifier.trim().toLowerCase();

  // 1. Check invitations (existing flow — by email)
  const invitation = await prisma.invitation.findFirst({
    where: { studentEmail: trimmed, isActivated: false },
  });

  if (invitation) {
    if (invitation.tokenExpiry && invitation.tokenExpiry > new Date(Date.now() - 60000)) {
      return { success: true, message: "تم إرسال رمز التحقق مسبقاً." };
    }
    const otp = generateOTP();
    const otpHash = await bcrypt.hash(otp, BCRYPT_ROUNDS);
    const expiry = new Date(Date.now() + OTP_EXPIRY_MINUTES * 60 * 1000);
    await prisma.invitation.update({ where: { id: invitation.id }, data: { token: otpHash, tokenExpiry: expiry } });
    logger.info(`OTP requested for ${trimmed} via invitation`);
    return { success: true, message: "تم إرسال رمز التحقق." };
  }

  // 2. Check onboard-created users — smart lookup by input type
  const isEmail = trimmed.includes("@");
  const userWhere = isEmail
    ? { email: trimmed, passwordHash: null, isActive: false }
    : { nationalId: trimmed, passwordHash: null, isActive: false };
  const user = await prisma.user.findFirst({ where: userWhere });

  if (!user) {
    return { success: true, message: "إذا كان لهذا الحساب رمز تحقق، فقد تم إرساله." };
  }

  // Onboard users already have a batch OTP in resetToken — don't overwrite it
  // Just confirm it exists and is not expired
  if (user.resetToken && user.resetTokenExpiry && user.resetTokenExpiry < new Date()) {
    throw new Error("انتهت صلاحية رمز التحقق. يرجى التواصل مع الإدارة.");
  }

  logger.info(`OTP requested for ${trimmed} (onboard user ${user.id})`);

  return { success: true, message: "تم إرسال رمز التحقق." };
}

// ── Verify OTP & Activate Account ──────────────────────────────────────────
async function verifyOTP(email, otp, password) {
  if (!email || !otp) {
    throw new Error("البريد الإلكتروني ورمز التحقق مطلوبان");
  }

  if (!password) {
    throw new Error("كلمة المرور مطلوبة");
  }

  const { validatePasswordStrength } = require("../middlewares/strictSecurity");
  const passwordErrors = validatePasswordStrength(password);
  if (passwordErrors.length > 0) {
    throw new Error("كلمة المرور ضعيفة جداً: " + passwordErrors.join(". "));
  }

  const trimmedEmail = email.trim().toLowerCase();
  const trimmedOtp = otp.trim();
  const passwordHash = await bcrypt.hash(password, 12);

  // 1. Check invitations (existing flow)
  const invitation = await prisma.invitation.findFirst({
    where: { studentEmail: trimmedEmail, isActivated: false },
  });

  if (invitation) {
    if (!invitation.tokenExpiry || invitation.tokenExpiry < new Date()) {
      throw new Error("انتهت صلاحية رمز التحقق. يرجى طلب رمز جديد.");
    }
    const validOtp = await bcrypt.compare(trimmedOtp, invitation.token);
    if (!validOtp) throw new Error("رمز التحقق غير صالح");

    const user = await prisma.user.create({
      data: {
        schoolId: invitation.schoolId,
        classroomId: invitation.classroomId || null,
        role: "STUDENT",
        name: invitation.studentName,
        email: invitation.studentEmail,
        nationalId: invitation.studentEmail.replace(/@.*/, "").replace(/\s+/g, ""),
        passwordHash,
        isActive: true, isVerified: true, isApproved: true, isOnboarded: true,
      },
    });

    await prisma.studentProfile.create({
      data: { studentId: user.id, totalXP: 0, currentLevel: 1, currentStreak: 0 },
    });

    await prisma.invitation.update({
      where: { id: invitation.id },
      data: { isActivated: true, userId: user.id, activatedAt: new Date(), token: null, tokenExpiry: null },
    });

    const jwt = require("jsonwebtoken");
    const token = jwt.sign(
      { id: user.id, schoolId: user.schoolId, role: user.role, name: user.name, tokenVersion: 1 },
      process.env.JWT_SECRET,
      { expiresIn: "1h" }
    );

    return { success: true, message: "تم تفعيل الحساب بنجاح!", accessToken: token, userId: user.id };
  }

  // 2. Check onboard-created users — smart lookup by input type
  const isEmail = trimmedEmail.includes("@");
  const onboardWhere = isEmail
    ? { email: trimmedEmail, passwordHash: null, isActive: false }
    : { nationalId: trimmedEmail, passwordHash: null, isActive: false };
  const user = await prisma.user.findFirst({ where: onboardWhere });

  if (!user) {
    throw new Error("لا يوجد حساب معلق لهذا البريد الإلكتروني");
  }

  if (!user.resetToken || !user.resetTokenExpiry) {
    throw new Error("لم يتم طلب رمز تحقق. يرجى طلب رمز أولاً.");
  }
  if (user.resetTokenExpiry < new Date()) {
    throw new Error("انتهت صلاحية رمز التحقق. يرجى طلب رمز جديد.");
  }

  const validOtp = await bcrypt.compare(trimmedOtp, user.resetToken);
  if (!validOtp) throw new Error("رمز التحقق غير صالح");

  // Activate the user
  await prisma.user.update({
    where: { id: user.id },
    data: {
      passwordHash,
      isActive: true,
      isVerified: true,
      isApproved: true,
      resetToken: null,
      resetTokenExpiry: null,
    },
  });

  const jwt = require("jsonwebtoken");
  const token = jwt.sign(
    { id: user.id, schoolId: user.schoolId, role: user.role, name: user.name, tokenVersion: 1 },
    process.env.JWT_SECRET,
    { expiresIn: "1h" }
  );

  logger.info(`Account activated via OTP: ${trimmedEmail} (${user.role})`);

  return { success: true, message: "تم تفعيل الحساب بنجاح!", accessToken: token, userId: user.id };
}

// ── Get Invitations for School ─────────────────────────────────────────────
async function getInvitations(schoolId, options = {}) {
  const { isActivated, classroomId, limit = 50, offset = 0 } = options;

  const where = { schoolId };

  if (isActivated !== undefined) {
    where.isActivated = isActivated;
  }

  if (classroomId) {
    where.classroomId = classroomId;
  }

  const invitations = await prisma.invitation.findMany({
    where,
    orderBy: { createdAt: "desc" },
    take: limit,
    skip: offset,
    select: {
      id: true,
      studentEmail: true,
      studentName: true,
      classroomId: true,
      schoolName: true,
      isActivated: true,
      userId: true,
      activatedAt: true,
      createdAt: true,
    },
  });

  const total = await prisma.invitation.count({ where });

  return { invitations, total };
}

module.exports = {
  importStudents,
  requestOTP,
  verifyOTP,
  getInvitations,
};
