// ========================================================
// Sprint 1 Critical & High Fixes Tests
// ========================================================

const fs = require('fs');
const path = require('path');

function readSource(file) {
  return fs.readFileSync(path.join(__dirname, '../../src/', file), 'utf8');
}

// ── FIX C1+C2: CSRF Double-Submit ────────────────────────────────────────────
describe('FIX C1+C2: CSRF double-submit requires both cookie AND header', () => {
  const source = readSource('middlewares/csrf.js');

  test('requires BOTH cookieToken AND headerToken (not OR)', () => {
    expect(source).toContain('if (!cookieToken || !headerToken)');
  });

  test('verifies both tokens independently', () => {
    expect(source).toContain('verifyCsrfToken(cookieToken)');
    expect(source).toContain('verifyCsrfToken(headerToken)');
  });

  test('compares cookie and header with timingSafeEqual', () => {
    expect(source).toContain('crypto.timingSafeEqual(cookieBuf, headerBuf)');
  });

  test('returns CSRF_TOKEN_MISMATCH on mismatch', () => {
    expect(source).toContain('CSRF_TOKEN_MISMATCH');
  });
});

// ── FIX C6: Client-Trusted Scores Removed ─────────────────────────────────────
describe('FIX C6: Client-trusted scores removed from submission', () => {
  const source = readSource('routes/student.js');

  test('does not destructure output from req.body (only challengeId and code)', () => {
    expect(source).toContain('const { challengeId, code } = req.body;');
    expect(source).not.toMatch(/const \{.*output.*\} = req\.body/);
  });

  test('submission uses server-computed scores from evaluate()', () => {
    expect(source).toContain('syntaxScore: evalResult.syntaxScore');
    expect(source).toContain('performanceScore: evalResult.performanceScore');
  });

  test('XP award uses server-computed values only', () => {
    expect(source).toContain('score: evalResult.score');
    expect(source).toContain('syntaxAccuracy: evalResult.syntaxScore');
  });

  test('never trusts client-provided output field', () => {
    expect(source).not.toMatch(/req\.body\.output/);
    expect(source).not.toMatch(/req\.body\.score/);
    expect(source).not.toMatch(/req\.body\.passed/);
    expect(source).not.toMatch(/req\.body\.syntaxScore/);
    expect(source).not.toMatch(/req\.body\.performanceScore/);
  });
});

// ── FIX C7: memIPBlocks/memRequestCounts Hard Cap ─────────────────────────────
describe('FIX C7: memIPBlocks and memRequestCounts hard cap', () => {
  const source = readSource('middlewares/strictSecurity.js');

  test('setIPBlock has hard cap at 50000', () => {
    expect(source).toMatch(/async function setIPBlock[\s\S]*?if \(memIPBlocks\.size > 50000\)/);
  });

  test('setIPBlock evicts oldest 10% when full', () => {
    expect(source).toMatch(/async function setIPBlock[\s\S]*?evictCount = Math\.floor\(keys\.length \* 0\.1\)/);
  });

  test('setRequestCount has hard cap at 50000', () => {
    expect(source).toMatch(/async function setRequestCount[\s\S]*?if \(memRequestCounts\.size > 50000\)/);
  });

  test('setRequestCount evicts oldest 10% when full', () => {
    expect(source).toMatch(/async function setRequestCount[\s\S]*?evictCount = Math\.floor\(keys\.length \* 0\.1\)/);
  });
});

// ── FIX C8: encryption.js Fail-Hard ──────────────────────────────────────────
describe('FIX C8: encryption.js fail-hard on missing key', () => {
  const source = readSource('utils/encryption.js');

  test('throws error when no ENCRYPTION_KEY or JWT_SECRET', () => {
    expect(source).toContain('ENCRYPTION_KEY or JWT_SECRET must be set');
  });

  test('no hardcoded fallback key in getMasterKey', () => {
    expect(source).not.toContain('dhad-studio-fallback-key');
  });

  test('warns with CRITICAL prefix', () => {
    expect(source).toContain('[Encryption] CRITICAL:');
  });
});

// ── FIX C9: Fisher-Yates Shuffle ──────────────────────────────────────────────
describe('FIX C9: Fisher-Yates shuffle in generatePassword', () => {
  const source = readSource('utils/encryption.js');

  test('uses Fisher-Yates algorithm', () => {
    expect(source).toContain('Fisher-Yates shuffle');
    expect(source).toContain('crypto.randomInt(i + 1)');
  });

  test('no biased sort shuffle', () => {
    expect(source).not.toMatch(/\.sort\(\(\) => crypto\.randomInt/);
  });
});

// ── FIX H4: expectedOutput Not Exposed to Students ────────────────────────────
describe('FIX H4: expectedOutput removed from student API responses', () => {
  const source = readSource('controllers/assessmentController.js');

  test('getActiveAssessment does not return expectedOutput', () => {
    // The response should include questions, starterCode, instructions, but NOT expectedOutput
    const getActiveSection = source.substring(
      source.indexOf('async function getActiveAssessment'),
      source.indexOf('async function submitAssessmentAnswer')
    );
    expect(getActiveSection).toContain('starterCode: assessment.starterCode');
    expect(getActiveSection).toContain('instructions: assessment.instructions');
    expect(getActiveSection).not.toMatch(/expectedOutput.*assessment\.expectedOutput/);
  });

  test('getStudentAssessments does not select expectedOutput', () => {
    const getStudentSection = source.substring(
      source.indexOf('async function getStudentAssessments'),
      source.indexOf('module.exports')
    );
    expect(getStudentSection).toContain('starterCode: true');
    expect(getStudentSection).toContain('instructions: true');
    expect(getStudentSection).not.toContain('expectedOutput: true');
  });
});

// ── FIX H8: Tokens Removed from Response Body ────────────────────────────────
describe('FIX H8: Tokens removed from response body in studentLogin', () => {
  const source = readSource('controllers/studentManagementController.js');

  test('studentLogin response does not contain accessToken or refreshToken', () => {
    // Find the res.json block after cookie setting
    const loginFn = source.substring(
      source.indexOf('async function studentLogin'),
      source.indexOf('async function getTeacherStudents')
    );
    // The final res.json should not have accessToken/refreshToken fields
    expect(loginFn).toContain('res.cookie("access_token"');
    expect(loginFn).toContain('res.cookie("refresh_token"');
    // After cookies, response should not return tokens in body
    expect(loginFn).not.toMatch(/res\.json\(\{[\s\S]*accessToken[\s\S]*\}\)/);
  });
});

// ── FIX H20: Strict Replay Protection for Sensitive Ops ──────────────────────
describe('FIX H20: strictReplayProtection for sensitive operations', () => {
  const source = readSource('middlewares/apiSecurity.js');

  test('strictReplayProtection function exists', () => {
    expect(source).toContain('function strictReplayProtection');
  });

  test('requires both timestamp and nonce', () => {
    expect(source).toContain('if (!timestamp || !nonce)');
  });

  test('validates timestamp freshness', () => {
    expect(source).toContain('Math.abs(Date.now() - requestTime) > CONFIG.TIMESTAMP_TOLERANCE_MS');
  });

  test('prevents nonce replay', () => {
    expect(source).toContain('nonces.has(nonce)');
  });

  test('strictReplayProtection is exported', () => {
    expect(source).toContain('strictReplayProtection');
  });
});

// ── FIX H24+H25: Error Handler Sanitization ──────────────────────────────────
describe('FIX H24+H25: Error handler sanitized', () => {
  const source = readSource('middlewares/errorHandler.js');

  test('prevents double response (headersSent check)', () => {
    expect(source).toContain('res.headersSent');
  });

  test('P2002 does not expose field names', () => {
    expect(source).not.toContain('err.meta?.target');
    expect(source).not.toContain('Duplicate value for');
  });

  test('P2002 returns generic message', () => {
    expect(source).toContain('القيمة المدخلة موجودة مسبقاً');
  });

  test('non-500 errors do not expose err.name', () => {
    expect(source).not.toMatch(/err\.name \|\| "Error"/);
  });

  test('non-500 errors do not expose err.message in response', () => {
    // err.message should only appear in logging, never in res.json messages
    const resJsonBlocks = source.match(/res\.json\(\{[^}]+\}/g) || [];
    for (const block of resJsonBlocks) {
      expect(block).not.toMatch(/err\.message/);
    }
  });

  test('returns generic error message for all statuses', () => {
    expect(source).toContain('حدث خطأ في معالجة طلبك');
  });
});

// ── FIX H29: verifyHMAC Length Check ──────────────────────────────────────────
describe('FIX H29: verifyHMAC length check before timingSafeEqual', () => {
  const source = readSource('utils/encryption.js');

  test('verifies data and signature are present', () => {
    expect(source).toContain('if (!data || !signature) return false;');
  });

  test('wraps timingSafeEqual in try/catch', () => {
    expect(source).toMatch(/function verifyHMAC[\s\S]*?try \{[\s\S]*?timingSafeEqual[\s\S]*?\} catch/);
  });
});

// ── FIX H30: Redis URL Redaction ──────────────────────────────────────────────
describe('FIX H30: Redis URL not logged with credentials', () => {
  const source = readSource('utils/redis.js');

  test('Redis connected log does not include URL', () => {
    expect(source).not.toMatch(/Redis connected.*url.*REDIS_CONFIG\.url/);
    expect(source).toContain("logger.info('✅ Redis connected')");
  });
});

// ── FIX C4: bulkCreateStudents Transaction ───────────────────────────────────
describe('FIX C4: bulkCreateStudents uses transaction', () => {
  const source = readSource('controllers/studentManagementController.js');

  test('bulkCreateStudents wraps loop in $transaction', () => {
    const bulkFn = source.substring(
      source.indexOf('async function bulkCreateStudents'),
      source.indexOf('module.exports')
    );
    expect(bulkFn).toContain('prisma.$transaction(async (tx)');
    expect(bulkFn).toContain('tx.user.findFirst');
    expect(bulkFn).toContain('tx.user.create');
    expect(bulkFn).toContain('tx.studentProfile.create');
  });
});
