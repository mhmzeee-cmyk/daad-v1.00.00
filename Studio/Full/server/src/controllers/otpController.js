const { requestOTP, verifyOTP, importStudents, getInvitations } = require("../services/adminService");

// POST /api/v1/auth/request-otp
// Student requests OTP to activate their account (email OR nationalId)
async function requestOtp(req, res, next) {
  try {
    const { email, identifier } = req.body;
    const id = identifier || email;

    if (!id || typeof id !== "string" || id.trim().length === 0) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "البريد الإلكتروني أو الرقم الوطني مطلوب",
      });
    }

    const result = await requestOTP(id.trim());

    res.json({
      success: true,
      message: result.message,
    });
  } catch (err) {
    next(err);
  }
}

// POST /api/v1/auth/verify-otp
// Student verifies OTP and activates account (email OR nationalId)
async function verifyOtp(req, res, next) {
  try {
    const { email, identifier, otp, password } = req.body;
    const id = identifier || email;

    if (!id || !otp) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "المعرف ورمز التحقق مطلوبان",
      });
    }

    if (!password) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "كلمة المرور مطلوبة",
      });
    }

    if (typeof otp !== "string" || otp.trim().length !== 6) {
      return res.status(400).json({
        error: "خطأ في الطلب",
        message: "رمز التحقق يجب أن يكون 6 أرقام",
      });
    }

    const result = await verifyOTP(id.trim(), otp, password);

    res.json(result);
  } catch (err) {
    if (err.message.includes("expired") || err.message.includes("انتهت")) {
      return res.status(400).json({ error: "OTP_EXPIRED", message: err.message });
    }
    if (err.message.includes("Invalid") || err.message.includes("غير صالح")) {
      return res.status(401).json({ error: "OTP_INVALID", message: err.message });
    }
    if (err.message.includes("No pending") || err.message.includes("لا يوجد")) {
      return res.status(404).json({ error: "NOT_FOUND", message: err.message });
    }
    if (err.message.includes("Password") || err.message.includes("كلمة المرور")) {
      return res.status(400).json({ error: "WEAK_PASSWORD", message: err.message });
    }
    next(err);
  }
}

// POST /api/v1/teacher/invitations/import
async function importStudentsHandler(req, res, next) {
  try {
    const { students, classroomId } = req.body;
    const { schoolId } = req.user;

    if (!Array.isArray(students) || students.length === 0) {
      return res.status(400).json({ error: "خطأ في الطلب", message: "قائمة الطلاب مطلوبة" });
    }

    const result = await importStudents(students, schoolId, classroomId);

    res.status(201).json({
      success: true,
      message: `تم إنشاء ${result.successful.length} من ${result.total} دعوة`,
      results: result,
    });
  } catch (err) {
    next(err);
  }
}

// GET /api/v1/teacher/invitations
async function getInvitationsHandler(req, res, next) {
  try {
    const { schoolId } = req.user;
    const { isActivated, classroomId, limit, offset } = req.query;

    const options = {};
    if (isActivated !== undefined) options.isActivated = isActivated === "true";
    if (classroomId) options.classroomId = classroomId;
    if (limit) options.limit = Math.min(Math.max(parseInt(limit) || 50, 1), 200);
    if (offset) options.offset = Math.max(parseInt(offset) || 0, 0);

    const result = await getInvitations(schoolId, options);

    res.json({ success: true, ...result });
  } catch (err) {
    next(err);
  }
}

module.exports = {
  requestOtp,
  verifyOtp,
  importStudentsHandler,
  getInvitationsHandler,
};
