/* Developer: محمد محمود الحموز | Dhad Studio */
// ====================================================
// Dhad Studio - Production Server (Batch 10)
// ====================================================
// Features: Redis Cache, Cluster, Logging, Swagger,
//           Health, Security, Compression, Sessions
// ====================================================

require('dotenv').config();

// ── Auto-generate secrets if using defaults ────────────────────────────────────
const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

const KNOWN_INSECURE_SECRETS = [
  'c530c5a18cfee0cededce504d7ae096c397ad7050c9bd94fae318c34c1785cba24cace5cf45be770b075f9920d43b290',
  '18a488e4063f08dfed93138116e3943951c1b133b1a9d3d75f89f784965783bc',
  'change-me-in-production',
  '',
  undefined,
];

function ensureSecureSecrets() {
  const envPath = path.join(__dirname, '../.env');
  let envContent = fs.existsSync(envPath) ? fs.readFileSync(envPath, 'utf8') : '';
  let modified = false;

  if (KNOWN_INSECURE_SECRETS.includes(process.env.JWT_SECRET)) {
    const newSecret = crypto.randomBytes(64).toString('hex');
    process.env.JWT_SECRET = newSecret;
    envContent = envContent.replace(
      /JWT_SECRET="[^"]*"/,
      `JWT_SECRET="${newSecret}"`
    );
    modified = true;
    console.warn('\x1b[33m⚠ JWT_SECRET was insecure/auto-generated — new secret created.\x1b[0m');
  }

  if (KNOWN_INSECURE_SECRETS.includes(process.env.HMAC_SECRET)) {
    const newSecret = crypto.randomBytes(64).toString('hex');
    process.env.HMAC_SECRET = newSecret;
    envContent = envContent.replace(
      /HMAC_SECRET="[^"]*"/,
      `HMAC_SECRET="${newSecret}"`
    );
    modified = true;
    console.warn('\x1b[33m⚠ HMAC_SECRET was insecure/auto-generated — new secret created.\x1b[0m');
  }

  if (modified) {
    fs.writeFileSync(envPath, envContent, 'utf8');
  }
}

ensureSecureSecrets();

const express = require("express");
const helmet = require("helmet");
const compression = require("compression");

// ── Import New Middleware ─────────────────────────────────────────────────────
const { logger, requestLogger } = require("./utils/logger");
const { setupSwagger } = require("./config/swagger");
const { healthCheck, readinessCheck, livenessCheck } = require("./controllers/healthController");
const strictSecurity = require("./middlewares/strictSecurity");
const { connectRedis, disconnectRedis } = require("./utils/redis");
const { sanitizeResponse } = require("./middlewares/apiSecurity");
const { csrfProtection } = require("./middlewares/csrf");
const { caches, cacheInvalidator } = require("./middlewares/cache");
const sessionStore = require("./utils/sessionStore");

const app = express();

// ── Trust Proxy (for rate limiting behind reverse proxy) ─────────────────────
app.set('trust proxy', 1);

// ── Request Logging (Winston) ────────────────────────────────────────────────
app.use(requestLogger);

// ── STRICT Security Headers ─────────────────────────────────────────────────
app.use(strictSecurity.strictSecurityHeaders);

// ── HSTS (Production only) ──────────────────────────────────────────────────
if (process.env.NODE_ENV === 'production') {
  app.use((req, res, next) => {
    res.setHeader('Strict-Transport-Security', 'max-age=31536000; includeSubDomains; preload');
    next();
  });
}

// ── Additional Security Headers ──────────────────────────────────────────────
app.use((req, res, next) => {
  res.setHeader('X-Content-Type-Options', 'nosniff');
  res.setHeader('X-Frame-Options', 'DENY');
  res.setHeader('Referrer-Policy', 'strict-origin-when-cross-origin');
  res.setHeader('Permissions-Policy', 'camera=(), microphone=(), geolocation=(), payment=()');
  next();
});

// ── Strict Request Validation ────────────────────────────────────────────────
app.use(strictSecurity.strictRequestValidation);

// ── Strict Input Sanitization ────────────────────────────────────────────────
app.use(strictSecurity.strictSanitizeMiddleware);

// ── Response Sanitization ────────────────────────────────────────────────────
app.use(sanitizeResponse());

// ── Helmet (minimal, our CSP is stricter) ────────────────────────────────────
app.use(helmet({
  contentSecurityPolicy: false,
  frameguard: false, // We set X-Frame-Options: DENY
  xssFilter: false,  // We set X-XSS-Protection
  noSniff: true,
  hidePoweredBy: true,
  crossOriginEmbedderPolicy: false,
}));

// ── Compression Middleware (Gzip) ─────────────────────────────────────────
app.use(compression({
  level: 4,              // Faster compression (was 6)
  threshold: 1024,       // Only compress responses > 1KB (was 512)
  filter: (req, res) => {
    if (req.headers['x-no-compression']) return false;
    const contentType = res.getHeader('content-type') || '';
    if (contentType.includes('image/')) return false;
    return compression.filter(req, res);
  },
  memLevel: 6,
}));

// ── Body Parsing ─────────────────────────────────────────────────────────────
app.use(express.json({
  limit: '5mb',
  strict: true,
  reviver: (key, value) => {
    if (typeof value === 'string' && value.length > 1000000) {
      throw new Error('String value too large');
    }
    return value;
  },
}));

app.use(express.urlencoded({ extended: true, limit: '1mb' }));

// ── Cookie Parser (for httpOnly JWT cookies) ──────────────────────────────────
const cookieParser = require('cookie-parser');
app.use(cookieParser());

// ── CSRF Protection (Double Submit Cookie) ─────────────────────────────────────
app.use(csrfProtection);

// ── MessagePack Content-Type Validation ──────────────────────────────────────
app.use('/api/v1/submissions/bulk-report', (req, res, next) => {
  if (req.headers['content-type'] && 
      !req.headers['content-type'].includes('application/json') &&
      !req.headers['content-type'].includes('application/msgpack') &&
      !req.headers['content-type'].includes('application/x-www-form-urlencoded')) {
    return res.status(415).json({
      error: "UNSUPPORTED_MEDIA_TYPE",
      message: "This endpoint supports application/json and application/msgpack",
      supportedFormats: ["application/json", "application/msgpack"]
    });
  }
  next();
});

// ── CORS ─────────────────────────────────────────────────────────────────────
let _corsWarned = false;
app.use((req, res, next) => {
  const envOrigins = process.env.ALLOWED_ORIGINS;
  const isProduction = process.env.NODE_ENV === 'production';
  
  let allowedOrigins = envOrigins
    ? envOrigins.split(',').map(o => o.trim()).filter(Boolean)
    : ['http://localhost:3000', 'http://localhost:5173', 'http://localhost:5174'];

  // SECURITY: In production with no ALLOWED_ORIGINS, log warning once
  if (isProduction && !envOrigins && !_corsWarned) {
    _corsWarned = true;
    logger.warn('CORS: No ALLOWED_ORIGINS set in production — using localhost defaults');
  }

  const origin = req.headers.origin;
  const isAllowed = origin && allowedOrigins.includes(origin);

  if (isAllowed) {
    res.setHeader('Access-Control-Allow-Origin', origin);
    res.setHeader('Access-Control-Allow-Credentials', 'true');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization, X-Requested-With, X-CSRF-Token');
    res.setHeader('Access-Control-Max-Age', '7200');
    res.setHeader('Access-Control-Expose-Headers', 'X-RateLimit-Limit, X-RateLimit-Remaining, X-RateLimit-Reset');
  }

  if (req.method === 'OPTIONS') {
    if (!isAllowed) {
      return res.status(403).json({ error: 'CORS', message: 'Origin not allowed' });
    }
    return res.sendStatus(204);
  }

  next();
});

// ── Security Middleware ──────────────────────────────────────────────────────
app.use((req, res, next) => {
  const contentLength = req.headers['content-length'];
  if (contentLength && parseInt(contentLength) > 5 * 1024 * 1024) {
    return res.status(413).json({
      error: "PAYLOAD_TOO_LARGE",
      message: "الحمولة كبيرة جداً (الحد الأقصى 5 ميغابايت)",
      limit: "5MB"
    });
  }
  
  if (req.headers['user-agent'] && req.headers['user-agent'].length > 500) {
    return res.status(400).json({
      error: "INVALID_USER_AGENT",
      message: "ترويسة User-Agent طويلة جداً"
    });
  }

  // Block path traversal attempts in URL
  const decodedUrl = decodeURIComponent(req.url);
  if (
    decodedUrl.includes('..') ||
    decodedUrl.includes('%2e') ||
    decodedUrl.includes('%252e') ||
    decodedUrl.includes('%2E') ||
    decodedUrl.includes('\\') ||
    decodedUrl.includes('\0')
  ) {
    return res.status(400).json({
      error: "BAD_REQUEST",
      message: "مسار غير صالح"
    });
  }
  
  next();
});

// ── Health Endpoints (No Rate Limiting) ──────────────────────────────────────
app.get('/health', healthCheck);
app.get('/health/ready', readinessCheck);
app.get('/health/live', livenessCheck);

// ── Swagger Documentation (Development Only) ──────────────────────────────
if (process.env.NODE_ENV !== 'production') {
  setupSwagger(app);
}

// ── Prisma Client (available to all routes via req.app.get("prisma")) ────────
const prisma = require("./utils/prisma");
app.set("prisma", prisma);

// ── Import Routes ────────────────────────────────────────────────────────────
const router = require("./router/index");

app.use('/api/v1', router);

// ── Static Files (Frontend) ──────────────────────────────────────────────────
const frontendPath = path.join(__dirname, "../../frontend-web");

// Static files with caching for production
app.use(express.static(frontendPath, {
  dotfiles: 'deny',
  index: false,
  maxAge: process.env.NODE_ENV === 'production' ? '1d' : 0, // Cache for 1 day in production
  etag: true,
  lastModified: true,
  setHeaders: function(res, filePath) {
    if (filePath.endsWith('.html') || filePath.endsWith('.htm')) {
      res.setHeader('Content-Type', 'text/html; charset=utf-8');
    }
    // Cache static assets aggressively
    if (filePath.endsWith('.css') || filePath.endsWith('.js')) {
      if (process.env.NODE_ENV === 'production') {
        res.setHeader('Cache-Control', 'public, max-age=31536000, immutable');
      } else {
        res.setHeader('Cache-Control', 'no-store, no-cache, must-revalidate');
        res.setHeader('Pragma', 'no-cache');
      }
    } else if (filePath.endsWith('.svg') || filePath.endsWith('.png') || filePath.endsWith('.jpg')) {
      res.setHeader('Cache-Control', 'public, max-age=86400');
    } else {
      res.setHeader('Cache-Control', 'no-store, no-cache, must-revalidate');
      res.setHeader('Pragma', 'no-cache');
    }
  }
}));

// ── 404 Handler ──────────────────────────────────────────────────────────────
app.use((req, res) => {
  // Block path traversal attempts in URL
  if (req.originalUrl.includes('..') || req.originalUrl.includes('%2e') || req.originalUrl.includes('%252e')) {
    return res.status(400).json({
      error: 'BAD_REQUEST',
      message: 'Invalid path',
    });
  }

  // API routes always get JSON 404
  if (req.path.startsWith('/api')) {
    return res.status(404).json({
      error: 'NOT_FOUND',
      message: 'Route not found',
      documentation: '/api-docs',
    });
  }

  // Known frontend pages get HTML
  const knownPages = ['/', '/login.html', '/register.html', '/index.html', '/activate.html'];
  if (knownPages.includes(req.path)) {
    return res.sendFile(path.join(frontendPath, req.path === '/' ? 'index.html' : req.path));
  }

  // Everything else gets 404
  res.status(404).json({
    error: 'NOT_FOUND',
    message: 'Route not found',
  });
});

// ── Error Handling ───────────────────────────────────────────────────────────
const { errorHandler } = require("./middlewares/errorHandler");
app.use(errorHandler);

// ── Start Server ─────────────────────────────────────────────────────────────
const PORT = process.env.PORT || 3000;
const HOST = process.env.HOST || '0.0.0.0';
const BACKLOG = parseInt(process.env.TCP_BACKLOG) || 2048;

let server;

function startServer() {
  connectRedis().catch(() => {});

  server = app.listen(PORT, HOST, BACKLOG, async () => {
    const serverInfo = server.address();

    // Tune TCP for high concurrency
    server.keepAliveTimeout = 30000;   // 30s keep-alive
    server.headersTimeout = 60000;     // 60s headers
    server.requestTimeout = 60000;     // 60s request
    server.timeout = 120000;           // 120s socket

    logger.info('Dhad Studio Server started', {
      port: serverInfo.port,
      address: serverInfo.address,
      backlog: BACKLOG,
      pid: process.pid,
      environment: process.env.NODE_ENV || 'development',
      nodeVersion: process.version,
    });

    // ── Startup Security Summary ────────────────────────────────────────────
    const isProd = process.env.NODE_ENV === 'production';
    const envOrigins = process.env.ALLOWED_ORIGINS;
    const jwtLen = process.env.JWT_SECRET ? process.env.JWT_SECRET.length : 0;
    const hmacLen = process.env.HMAC_SECRET ? process.env.HMAC_SECRET.length : 0;

    const secLines = [];
    secLines.push('═══════════════════════════════════════════════════');
    secLines.push('  Dhad Studio — Security Status');
    secLines.push('═══════════════════════════════════════════════════');
    secLines.push(`  JWT key length: ${jwtLen >= 32 ? 'OK' : 'TOO SHORT'} (${jwtLen} chars)`);
    secLines.push(`  HMAC key length: ${hmacLen >= 32 ? 'OK' : 'TOO SHORT'} (${hmacLen} chars)`);
    secLines.push(`  CORS: ${isProd ? (envOrigins ? 'configured' : 'WARN: no origins') : 'dev mode'}`);
    secLines.push(`  Helmet: enabled`);
    secLines.push(`  CSRF: enabled`);
    secLines.push(`  Environment: ${process.env.NODE_ENV || 'development'}`);
    secLines.push('═══════════════════════════════════════════════════');
    logger.info(secLines.join('\n'));

    // Signal cluster master that this worker is ready
    if (process.send) process.send('ready');
  });

  // Handle connection errors (ECONNRESET from client disconnects)
  server.on('clientError', (err, socket) => {
    if (err.code === 'ECONNRESET' || err.code === 'ECONNABORTED') return;
    logger.warn('Client error', { code: err.code });
  });
}

if (require.main === module || process.env.DHAD_START_SERVER === '1') {
  startServer();
}

// ── Graceful Shutdown ────────────────────────────────────────────────────────
const gracefulShutdown = (signal) => {
  logger.info(`Received ${signal}. Starting graceful shutdown...`);

  if (server) {
    server.close(async () => {
      logger.info('HTTP server closed');

      try { await disconnectRedis(); } catch (e) { logger.warn('Redis disconnect failed during shutdown', { error: e.message }); }

      const prisma = app.get("prisma");
      if (prisma) {
        await prisma.$disconnect().catch(() => {});
      }
      process.exit(0);
    });
  } else {
    process.exit(0);
  }

  // Force shutdown after 30 seconds
  setTimeout(() => {
    logger.error('Forced shutdown due to timeout');
    process.exit(1);
  }, 30000);
};

process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));
process.on('SIGINT', () => gracefulShutdown('SIGINT'));

// ── Unhandled Rejections ─────────────────────────────────────────────────────
process.on('unhandledRejection', (reason, promise) => {
  logger.error('Unhandled Rejection — treating as fatal', { reason: reason?.toString(), promise });
  process.exit(1);
});

process.on('uncaughtException', (error) => {
  logger.error('Uncaught Exception', { error: error.message, stack: error.stack });
  process.exit(1);
});

module.exports = app;
module.exports.server = server || null;
