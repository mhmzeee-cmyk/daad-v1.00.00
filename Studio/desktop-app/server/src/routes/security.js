const express = require("express");
const router = express.Router();
const { authenticate, requireAdmin } = require("../middlewares/auth");
const { getAuditLog, logAudit, strictRateLimit, unblockIPAdmin } = require("../middlewares/strictSecurity");
const { logger } = require("../utils/logger");

// In-memory counters for dashboard
let dashboardFailedAttempts = 0;
let dashboardBlockedIPs = 0;

// GET /api/v1/security/dashboard — Admin security dashboard
router.get("/security/dashboard", strictRateLimit("admin"), authenticate, requireAdmin, (req, res) => {
  try {
    const audit = getAuditLog({ limit: 100 });

    // Count events by type
    const eventCounts = {};
    for (const entry of audit) {
      eventCounts[entry.action] = (eventCounts[entry.action] || 0) + 1;
    }

    // Count security events
    const failedLogins = (eventCounts["LOGIN_INVALID_PASSWORD"] || 0) + (eventCounts["LOGIN_USER_NOT_FOUND"] || 0);
    const lockedAccounts = eventCounts["ACCOUNT_LOCKED"] || 0;
    const blockedIPs = eventCounts["IP_BLOCKED"] || 0;
    const blockedScanners = eventCounts["BLOCKED_SCANNER"] || 0;

    res.json({
      success: true,
      summary: {
        totalEvents: audit.length,
        eventCounts,
        security: {
          failedLogins,
          lockedAccounts,
          blockedIPs,
          blockedScanners,
        },
      },
      recentEvents: audit.slice(-50),
    });
  } catch (err) {
    logger.error(`Security dashboard error: ${err.message}`);
    res.status(500).json({ error: "خطأ داخلي في الخادم" });
  }
});

// GET /api/v1/security/audit — Get audit log
router.get("/security/audit", strictRateLimit("admin"), authenticate, requireAdmin, (req, res) => {
  const { limit = 100, action, userId, ip } = req.query;
  const log = getAuditLog({
    limit: parseInt(limit) || 100,
    action,
    userId,
    ip,
  });

  res.json({
    success: true,
    count: log.length,
    entries: log,
  });
});

// POST /api/v1/security/unblock-ip — Unblock an IP
router.post("/security/unblock-ip", strictRateLimit("admin"), authenticate, requireAdmin, async (req, res) => {
  const { ip } = req.body;
  if (!ip) {
    return res.status(400).json({ error: "عنوان IP مطلوب" });
  }

  const wasBlocked = await unblockIPAdmin(ip);
  if (wasBlocked) {
    logAudit("IP_UNBLOCKED", { ip, by: req.user.id });
    return res.json({ success: true, message: `IP ${ip} unblocked` });
  }

  res.json({ success: true, message: `IP ${ip} was not blocked` });
});

module.exports = router;
