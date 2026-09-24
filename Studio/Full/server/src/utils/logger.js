// ====================================================
// Dhad Studio - Production Logger (Winston)
// ====================================================
// Features: Console + File rotation, Anti-Cheat archive
// Works on: Local, Render, Railway, AWS, Vercel
// ====================================================

const winston = require('winston');
const path = require('path');
const fs = require('fs');

// ── Detect Cloud Environment ─────────────────────────────────────────────────
const isCloudEnvironment = process.env.RENDER || 
                           process.env.RAILWAY || 
                           process.env.HEROKU || 
                           process.env.AWS_LAMBDA_FUNCTION_NAME ||
                           !fs.existsSync(path.join(__dirname, '../../logs'));

// ── Ensure logs directory exists (local only) ────────────────────────────────
const logsDir = path.join(__dirname, '../../logs');
if (!isCloudEnvironment && !fs.existsSync(logsDir)) {
  fs.mkdirSync(logsDir, { recursive: true });
}

// ── Custom Log Format ────────────────────────────────────────────────────────
const logFormat = winston.format.combine(
  winston.format.timestamp({ format: 'YYYY-MM-DD HH:mm:ss.SSS' }),
  winston.format.errors({ stack: true }),
  winston.format.json(),
  winston.format.printf(({ timestamp, level, message, ...meta }) => {
    const metaStr = Object.keys(meta).length ? ` ${JSON.stringify(meta)}` : '';
    return `${timestamp} [${level.toUpperCase()}] ${message}${metaStr}`;
  })
);

// ── Console Format (Development) ─────────────────────────────────────────────
const consoleFormat = winston.format.combine(
  winston.format.colorize(),
  winston.format.timestamp({ format: 'HH:mm:ss' }),
  winston.format.printf(({ timestamp, level, message, ...meta }) => {
    const metaStr = Object.keys(meta).length ? ` ${JSON.stringify(meta)}` : '';
    return `${timestamp} ${level} ${message}${metaStr}`;
  })
);

// ── Create Logger Instance ───────────────────────────────────────────────────
const logger = winston.createLogger({
  level: process.env.LOG_LEVEL || 'info',
  levels: {
    critical: 0,
    error: 1,
    warn: 2,
    info: 3,
    http: 4,
    debug: 5,
  },
  format: logFormat,
  defaultMeta: { service: 'daad-studio-api' },
  transports: [
    // Console transport (always active - required for cloud platforms)
    new winston.transports.Console({
      format: consoleFormat,
    }),
  ],
});

// Add file transports only in local environment
if (!isCloudEnvironment) {
  logger.add(new winston.transports.File({
    filename: path.join(logsDir, 'combined.log'),
    maxsize: 10 * 1024 * 1024,
    maxFiles: 5,
    tailable: true,
  }));

  logger.add(new winston.transports.File({
    filename: path.join(logsDir, 'error.log'),
    level: 'error',
    maxsize: 10 * 1024 * 1024,
    maxFiles: 5,
    tailable: true,
  }));

  logger.add(new winston.transports.File({
    filename: path.join(logsDir, 'security-alerts.log'),
    level: 'critical',
    maxsize: 10 * 1024 * 1024,
    maxFiles: 10,
    tailable: true,
  }));

  logger.exceptions.handle(new winston.transports.File({
    filename: path.join(logsDir, 'exceptions.log'),
    maxsize: 10 * 1024 * 1024,
    maxFiles: 3,
  }));

  logger.rejections.handle(new winston.transports.File({
    filename: path.join(logsDir, 'rejections.log'),
    maxsize: 10 * 1024 * 1024,
    maxFiles: 3,
  }));
}

// ── Security Alert Logger (Batch 8 Anti-Cheat) ──────────────────────────────
const securityLogger = {
  /**
   * Log suspicious submission activity
   */
  suspiciousSubmission: (data) => {
    const { studentId, challengeId, reasons, submissionData } = data;
    logger.critical('🚨 SUSPICIOUS SUBMISSION DETECTED', {
      type: 'SECURITY_ALERT',
      alertType: 'SUSPICIOUS_SUBMISSION',
      studentId,
      challengeId,
      reasons: reasons?.map(r => r.detail) || [],
      maxSeverity: reasons?.[0]?.severity || 'UNKNOWN',
      codeLength: submissionData?.code?.length || 0,
      executionTime: submissionData?.executionTime || 0,
      timestamp: new Date().toISOString(),
    });
  },

  /**
   * Log HMAC signature validation failure
   */
  hmacFailure: (data) => {
    const { studentId, ip, expectedSignature, receivedSignature } = data;
    logger.critical('🔐 HMAC SIGNATURE MISMATCH', {
      type: 'SECURITY_ALERT',
      alertType: 'HMAC_FAILURE',
      studentId,
      ip,
      expectedSignature: expectedSignature?.substring(0, 16) + '...',
      receivedSignature: receivedSignature?.substring(0, 16) + '...',
      timestamp: new Date().toISOString(),
    });
  },

  /**
   * Log rate limit violations
   */
  rateLimitExceeded: (data) => {
    const { ip, endpoint, userId } = data;
    logger.warn('⏱️ RATE LIMIT EXCEEDED', {
      type: 'SECURITY_EVENT',
      alertType: 'RATE_LIMIT',
      ip,
      endpoint,
      userId,
      timestamp: new Date().toISOString(),
    });
  },

  /**
   * Log authentication failures
   */
  authFailure: (data) => {
    const { username, ip, reason } = data;
    logger.warn('🔑 AUTHENTICATION FAILURE', {
      type: 'SECURITY_EVENT',
      alertType: 'AUTH_FAILURE',
      username,
      ip,
      reason,
      timestamp: new Date().toISOString(),
    });
  },
};

// ── Request Logger Middleware ────────────────────────────────────────────────
const requestLogger = (req, res, next) => {
  const start = Date.now();
  
  res.on('finish', () => {
    const duration = Date.now() - start;
    const logData = {
      method: req.method,
      path: req.path,
      status: res.statusCode,
      duration: `${duration}ms`,
      ip: req.ip || req.connection?.remoteAddress,
      userAgent: req.headers['user-agent']?.substring(0, 100),
    };

    // Skip health check logs to reduce noise
    if (req.path === '/health' || req.path.startsWith('/health/')) return;

    if (res.statusCode >= 500) {
      logger.error('HTTP Request Error', logData);
    } else if (res.statusCode >= 400) {
      logger.warn('HTTP Request Warning', logData);
    } else if (req.path.includes('/submissions') || req.path.includes('/analytics')) {
      logger.http('HTTP Request', logData);
    }
  });

  next();
};

module.exports = {
  logger,
  securityLogger,
  requestLogger,
};
