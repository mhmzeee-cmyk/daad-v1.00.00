const { logger } = require("../utils/logger");

function errorHandler(err, req, res, _next) {
  // Prevent double response
  if (res.headersSent) {
    logger.error("Headers already sent", { error: err.message, route: req.originalUrl });
    return req.destroy();
  }

  // Structured logging with full context
  logger.error("Unhandled error", {
    message: err.message,
    stack: err.stack,
    code: err.code,
    meta: err.meta,
    name: err.name,
    route: req.originalUrl,
    method: req.method,
    userId: req.user?.id,
    ip: req.ip,
  });

  // Prisma unique constraint violation — never expose field names
  if (err.code === "P2002") {
    return res.status(409).json({
      error: "تعارض",
      message: "القيمة المدخلة موجودة مسبقاً",
    });
  }

  // Prisma record not found
  if (err.code === "P2025") {
    return res.status(404).json({
      error: "غير موجود",
      message: "المرجع المطلوب غير موجود",
    });
  }

  // Prisma connection/lock errors (SQLite BUSY, pool exhausted)
  if (err.code === "P1002" || err.code === "P1017" || err.message?.includes("SQLITE") || err.message?.includes("timeout")) {
    return res.status(503).json({
      error: "Service Unavailable",
      message: "قاعدة البيانات مشغولة حالياً. حاول مرة أخرى.",
    });
  }

  // JWT errors
  if (err.name === "JsonWebTokenError") {
    return res.status(401).json({
      error: "غير مصرح",
      message: "الرمز غير صالح",
    });
  }

  if (err.name === "TokenExpiredError") {
    return res.status(401).json({
      error: "غير مصرح",
      message: "انتهت صلاحية الرمز",
    });
  }

  // TypeError from NoSQL injection (object passed where string expected)
  if (err instanceof TypeError) {
    return res.status(400).json({
      error: "خطأ في الطلب",
      message: "نوع الإدخال غير صالح",
    });
  }

  // SyntaxError from malformed JSON
  if (err instanceof SyntaxError && err.status === 400) {
    return res.status(400).json({
      error: "خطأ في الطلب",
      message: "صيغة JSON غير صحيحة في جسم الطلب",
    });
  }

  // SECURITY: Never expose internal error details to client
  const statusCode = err.statusCode || 500;
  res.status(statusCode).json({
    error: statusCode === 500 ? "خطأ داخلي في الخادم" : "خطأ في الطلب",
    message: statusCode === 500
      ? "حدث خطأ غير متوقع"
      : "حدث خطأ في معالجة طلبك",
  });
}

module.exports = { errorHandler };
