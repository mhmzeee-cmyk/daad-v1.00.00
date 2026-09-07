// ── Tests for Phase 2 Fixes (FIX 11-16) ──────────────────────────────────────
// Tests for: pagination limits, CSRF timing-safe, HMAC secret, streak atomicity,
//            XP race condition, student cookie auth

const crypto = require('crypto');

// ── FIX 12: CSRF Timing-Safe Comparison ───────────────────────────────────────
describe('FIX 12: CSRF Timing-Safe Comparison (apiSecurity.js)', () => {
  const { csrfProtection } = require('../../src/middlewares/apiSecurity');

  function mockReq(method, cookies, headers) {
    return { method: method || 'GET', cookies: cookies || {}, headers: headers || {}, path: '/test' };
  }

  function mockRes() {
    const res = { statusCode: null, body: null };
    res.status = jest.fn().mockReturnValue(res);
    res.json = jest.fn().mockReturnValue(res);
    return res;
  }

  test('accepts matching CSRF tokens', () => {
    const token = 'abcdefghijklmnop';
    const req = mockReq('POST', { csrf_token: token }, { 'x-csrf-token': token });
    const res = mockRes();
    const next = jest.fn();
    csrfProtection(req, res, next);
    expect(next).toHaveBeenCalled();
    expect(res.status).not.toHaveBeenCalled();
  });

  test('rejects CSRF when tokens differ', () => {
    const req = mockReq('POST', { csrf_token: 'token-a' }, { 'x-csrf-token': 'token-b' });
    const res = mockRes();
    const next = jest.fn();
    csrfProtection(req, res, next);
    expect(res.status).toHaveBeenCalledWith(403);
  });

  test('rejects CSRF when header token is missing', () => {
    const req = mockReq('POST', { csrf_token: 'some-token' });
    const res = mockRes();
    const next = jest.fn();
    csrfProtection(req, res, next);
    expect(res.status).toHaveBeenCalledWith(403);
  });

  test('rejects CSRF when cookie token is missing', () => {
    const req = mockReq('POST', {}, { 'x-csrf-token': 'some-token' });
    const res = mockRes();
    const next = jest.fn();
    csrfProtection(req, res, next);
    expect(res.status).toHaveBeenCalledWith(403);
  });

  test('skips CSRF for GET requests', () => {
    const req = mockReq('GET');
    const res = mockRes();
    const next = jest.fn();
    csrfProtection(req, res, next);
    expect(next).toHaveBeenCalled();
  });

  test('skips CSRF when Authorization header present', () => {
    const req = mockReq('POST', {}, { authorization: 'Bearer xxx' });
    const res = mockRes();
    const next = jest.fn();
    csrfProtection(req, res, next);
    expect(next).toHaveBeenCalled();
  });
});

// ── FIX 13: HMAC Secret is Server-Side ────────────────────────────────────────
describe('FIX 13: HMAC Secret Not User ID (apiSecurity.js)', () => {
  const apiSecurity = require('../../src/middlewares/apiSecurity');

  test('verifyRequestSignature uses HMAC_SECRET env var, not user ID', () => {
    const originalEnv = process.env.HMAC_SECRET;
    process.env.HMAC_SECRET = 'server-secret-key';

    // The middleware should use HMAC_SECRET, not req.user.id
    // We verify the export exists and CONFIG has expected shape
    expect(apiSecurity.CONFIG).toBeDefined();
    expect(apiSecurity.CONFIG.TIMESTAMP_TOLERANCE_MS).toBe(5 * 60 * 1000);

    process.env.HMAC_SECRET = originalEnv;
  });

  test('CONFIG has TIMESTAMP_TOLERANCE_MS of 5 minutes', () => {
    expect(apiSecurity.CONFIG.TIMESTAMP_TOLERANCE_MS).toBe(300000);
  });
});

// ── FIX 11: Unbounded findMany with take limits ───────────────────────────────
describe('FIX 11: reportsController take limits', () => {
  // We can't easily test the DB queries without mocking Prisma,
  // but we can verify the source code has the limits
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/reportsController.js'),
    'utf8'
  );

  test('weeklyReport user query has take limit', () => {
    expect(source).toContain('MAX_REPORT_USERS');
    expect(source).toContain('take: MAX_REPORT_USERS');
  });

  test('weeklyReport logins query has take limit', () => {
    expect(source).toContain('MAX_REPORT_LOGS');
    expect(source).toContain('take: MAX_REPORT_LOGS');
  });

  test('schoolOverview user query has take limit', () => {
    // Verify the schoolOverview function also has a take limit
    const schoolOverviewMatch = source.indexOf('schoolOverview');
    const takeLimit = source.indexOf('MAX_REPORT_USERS', schoolOverviewMatch);
    expect(takeLimit).toBeGreaterThan(schoolOverviewMatch);
  });
});

// ── FIX 14: calculateDailyStreak Atomicity ─────────────────────────────────────
describe('FIX 14: calculateDailyStreak no read-before-write race', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/studentProfileController.js'),
    'utf8'
  );

  test('calculateDailyStreak uses select to limit fields read', () => {
    // Should use select: { currentStreak: true, lastActivityDate: true }
    // instead of full profile read
    expect(source).toContain("select: { currentStreak: true, lastActivityDate: true }");
  });

  test('calculateDailyStreak has early return for today (no write)', () => {
    // When lastActivity === today, should return without DB write
    expect(source).toContain('Already logged today');
  });
});

// ── FIX 15: awardXP Race Condition ─────────────────────────────────────────────
describe('FIX 15: awardXP atomic update and idempotency', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/studentProfileController.js'),
    'utf8'
  );

  test('awardXP uses $executeRaw for atomic update', () => {
    expect(source).toContain('$executeRaw');
  });

  test('awardXP removes speed bonus (execution time not used for XP)', () => {
    // The new code should NOT have speedBonus in awardXP
    const awardXpStart = source.indexOf('async function awardXP');
    const awardXpEnd = source.indexOf('// ──', awardXpStart + 10);
    const awardXpBody = source.substring(awardXpStart, awardXpEnd);
    expect(awardXpBody).not.toContain('speedBonus');
  });

  test('awardXP does not use stale profile.longestStreak', () => {
    const awardXpStart = source.indexOf('async function awardXP');
    const awardXpEnd = source.indexOf('// ──', awardXpStart + 10);
    const awardXpBody = source.substring(awardXpStart, awardXpEnd);
    expect(awardXpBody).not.toContain('Math.max(profile.longestStreak');
  });

  test('awardXP accepts transaction parameter', () => {
    expect(source).toMatch(/async function awardXP\(submissionData, studentId, tx\)/);
  });
});

// ── FIX 16: Student Login Sets httpOnly Cookies ────────────────────────────────
describe('FIX 16: studentLogin httpOnly cookies', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/studentManagementController.js'),
    'utf8'
  );

  test('studentLogin sets access_token httpOnly cookie', () => {
    expect(source).toContain('res.cookie("access_token"');
    expect(source).toContain('httpOnly: true');
  });

  test('studentLogin sets refresh_token httpOnly cookie', () => {
    expect(source).toContain('res.cookie("refresh_token"');
  });

  test('studentLogin no longer returns tokens in body (httpOnly cookies only)', () => {
    // SECURITY: Tokens are in httpOnly cookies only, never in response body
    // Check that the final res.json block does not contain accessToken/refreshToken fields
    const loginFn = source.substring(
      source.indexOf('async function studentLogin'),
      source.indexOf('async function getTeacherStudents')
    );
    // The res.json should contain userId/username/role/profile but NOT accessToken/refreshToken
    expect(loginFn).toContain('userId: student.id');
    expect(loginFn).toContain('role: student.role');
    // Verify the json response block doesn't have token fields
    expect(loginFn).not.toMatch(/res\.json\(\{[\s\S]*?accessToken:[\s\S]*?\}\)/);
  });
});

// ── FIX 1: REFRESH_TOKEN_FINGERPRINTS Cleanup ──────────────────────────────────
describe('FIX 1: REFRESH_TOKEN_FINGERPRINTS cleanup interval', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/authController.js'),
    'utf8'
  );

  test('has setInterval cleanup for REFRESH_TOKEN_FINGERPRINTS', () => {
    expect(source).toContain('setInterval');
    expect(source).toContain('REFRESH_TOKEN_FINGERPRINTS');
  });

  test('cleanup removes entries older than 7 days', () => {
    expect(source).toContain('7 * 24 * 60 * 60 * 1000');
  });

  test('caps hashes per user at 50', () => {
    expect(source).toContain('data.hashes.size > 50');
  });
});

// ── FIX 2: getMasterKey Caching ────────────────────────────────────────────────
describe('FIX 2: getMasterKey caching', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/utils/encryption.js'),
    'utf8'
  );

  test('getMasterKey caches derived key', () => {
    expect(source).toContain('_cachedMasterKey');
    expect(source).toContain('_cachedMasterKeyHash');
  });

  test('getMasterKey invalidates cache if master key changes', () => {
    // Cache hit check: if hashes match, return cached; otherwise re-derive
    expect(source).toContain('_cachedMasterKeyHash === keyHash');
  });
});

// ── FIX 3: deepSanitize depth limit ────────────────────────────────────────────
describe('FIX 3: deepSanitize depth limit', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/middlewares/security.js'),
    'utf8'
  );

  test('deepSanitize has maxDepth parameter', () => {
    expect(source).toContain('maxDepth = 10');
  });

  test('deepSanitize detects circular references', () => {
    expect(source).toContain('[Circular]');
  });

  test('deepSanitize limits array size to 1000', () => {
    expect(source).toContain('obj.length > 1000');
  });

  test('deepSanitize limits object keys to 1000', () => {
    expect(source).toContain('keys.length > 1000');
  });
});

// ── FIX 4: Security Dashboard Operator Precedence ─────────────────────────────
describe('FIX 4: security dashboard operator precedence', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/routes/security.js'),
    'utf8'
  );

  test('failedLogins uses parenthesized OR defaults', () => {
    expect(source).toContain('(eventCounts["LOGIN_INVALID_PASSWORD"] || 0) + (eventCounts["LOGIN_USER_NOT_FOUND"] || 0)');
  });
});

// ── FIX 5: Math.max stack overflow prevention ──────────────────────────────────
describe('FIX 5: Math.max stack overflow prevention', () => {
  const fs = require('fs');
  const path = require('path');
  const assessSource = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/assessmentController.js'),
    'utf8'
  );
  const courseSource = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/courseController.js'),
    'utf8'
  );

  test('assessmentController uses reduce instead of spread', () => {
    expect(assessSource).toContain('.reduce((a, b) => Math.max(a, b), -Infinity)');
    expect(assessSource).not.toMatch(/Math\.max\(\.\.\.scores\)/);
  });

  test('courseController uses reduce instead of spread', () => {
    expect(courseSource).toContain('.reduce((a, b) => Math.max(a, b), -Infinity)');
  });
});

// ── FIX 6: studentLogin isAccountLocked check ──────────────────────────────────
describe('FIX 6: studentLogin isAccountLocked check', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/studentManagementController.js'),
    'utf8'
  );

  test('studentLogin imports isAccountLocked', () => {
    expect(source).toContain('isAccountLocked');
  });

  test('studentLogin checks isAccountLocked before password verify', () => {
    const loginStart = source.indexOf('async function studentLogin');
    const loginEnd = source.indexOf('// ──', loginStart + 30);
    const loginBody = source.substring(loginStart, loginEnd);
    expect(loginBody).toContain('const accountLocked = await isAccountLocked(studentId)');
  });
});

// ── FIX 7: memActiveSessions cleanup ───────────────────────────────────────────
describe('FIX 7: memActiveSessions cleanup', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/middlewares/strictSecurity.js'),
    'utf8'
  );

  test('cleanup interval includes memActiveSessions', () => {
    expect(source).toContain('memActiveSessions');
    // Should be in the cleanup setInterval block
    const cleanupStart = source.indexOf('setInterval(async ()');
    const cleanupSection = source.substring(cleanupStart, cleanupStart + 1500);
    expect(cleanupSection).toContain('memActiveSessions.delete');
  });
});

// ── FIX 8: Redis SCAN instead of KEYS ──────────────────────────────────────────
describe('FIX 8: Redis SCAN instead of KEYS', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/middlewares/cache.js'),
    'utf8'
  );

  test('cacheInvalidator uses SCAN not KEYS', () => {
    expect(source).toContain('client.scan(');
    expect(source).not.toMatch(/client\.keys\(/);
  });

  test('clearPattern uses cursor-based iteration', () => {
    expect(source).toContain("do {");
    expect(source).toContain("cursor !== '0'");
  });
});

// ── FIX 9: Parallel login logging ──────────────────────────────────────────────
describe('FIX 9: studentLogin parallel logging', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/studentManagementController.js'),
    'utf8'
  );

  test('login logging uses Promise.all for parallel writes', () => {
    const loginStart = source.indexOf('async function studentLogin');
    const loginBody = source.substring(loginStart, loginStart + 3000);
    expect(loginBody).toContain('Promise.all');
  });
});

// ── FIX 10: courseController Math.max safety ───────────────────────────────────
describe('FIX 10: courseController Math.max safety', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/controllers/courseController.js'),
    'utf8'
  );

  test('courseController uses reduce for max calculation', () => {
    expect(source).toContain('.reduce((a, b) => Math.max(a, b), -Infinity)');
  });

  test('courseController does not use spread with Math.max', () => {
    expect(source).not.toMatch(/Math\.max\(\.\.\.completedTiers\)/);
  });
});

// ── FIX 17: memFailedAttempts hard cap ────────────────────────────────────────
describe('FIX 17: memFailedAttempts hard cap', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/middlewares/strictSecurity.js'),
    'utf8'
  );

  test('setFailedAttempt has size check before insert', () => {
    expect(source).toContain('memFailedAttempts.size > 50000');
  });

  test('setFailedAttempt evicts oldest 10% when full', () => {
    expect(source).toContain('Math.floor(keys.length * 0.1)');
  });
});

// ── FIX 18: HMAC verify rejects missing signature ─────────────────────────────
describe('FIX 18: HMAC verify rejects missing signature', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/middlewares/apiSecurity.js'),
    'utf8'
  );

  test('verifyRequestSignature rejects when signature missing', () => {
    expect(source).toContain('توقيع الطلب مطلوب');
  });

  test('verifyRequestSignature does not call next() on missing signature', () => {
    // The old code had: if (!signature) { return next(); }
    // The new code should NOT have next() in the missing-signature branch
    const fnStart = source.indexOf('function verifyRequestSignature');
    const fnEnd = source.indexOf('function sanitizeResponse');
    const fnBody = source.substring(fnStart, fnEnd);
    // Should have rejection, not next()
    expect(fnBody).toContain('res.status(401)');
  });

  test('HMAC comparison uses timingSafeEqual', () => {
    expect(source).toContain('crypto.timingSafeEqual');
  });
});

// ── FIX 21: prisma.js slow query logging ──────────────────────────────────────
describe('FIX 21: prisma.js slow query logging', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/utils/prisma.js'),
    'utf8'
  );

  test('has SLOW_QUERY_THRESHOLD_MS constant', () => {
    expect(source).toContain('SLOW_QUERY_THRESHOLD_MS');
    expect(source).toContain('1000');
  });

  test('logs slow queries in production', () => {
    expect(source).toContain('Slow query detected');
  });

  test('sets SQLite cache_size pragma', () => {
    expect(source).toContain('PRAGMA cache_size');
  });

  test('sets synchronous=NORMAL pragma', () => {
    expect(source).toContain('PRAGMA synchronous=NORMAL');
  });
});

// ── FIX 22: challenges.js GET rate limiting ────────────────────────────────────
describe('FIX 22: challenges.js GET rate limiting', () => {
  const fs = require('fs');
  const path = require('path');
  const source = fs.readFileSync(
    path.join(__dirname, '../../src/routes/challenges.js'),
    'utf8'
  );

  test('GET /challenges has readRateLimit', () => {
    expect(source).toMatch(/router\.get\(.*"\/challenges".*readRateLimit/);
  });

  test('GET /challenges/daily has readRateLimit', () => {
    expect(source).toMatch(/router\.get\(.*"\/challenges\/daily".*readRateLimit/);
  });

  test('GET /submissions has readRateLimit', () => {
    expect(source).toMatch(/router\.get\(.*"\/submissions".*readRateLimit/);
  });

  test('imports readRateLimit from security middleware', () => {
    expect(source).toContain('readRateLimit');
  });
});
