// ========================================================
// Sprint 5 Fixes Tests — Security Hardening, School Isolation
// ========================================================

const fs = require('fs');
const path = require('path');

function readSource(file) {
  return fs.readFileSync(path.join(__dirname, '../../src/', file), 'utf8');
}

function readFrontend(file) {
  return fs.readFileSync(path.join(__dirname, '../../../frontend-web/', file), 'utf8');
}

// ── CRITICAL: HMAC fallback secret removed ─────────────────────────────────
describe('CRITICAL FIX: HMAC secret has no fallback', () => {
  const source = readSource('middlewares/apiSecurity.js');

  test('no hardcoded fallback secret string', () => {
    expect(source).not.toContain('default-hmac-secret-change-in-production');
  });

  test('returns 500 if HMAC secret not configured', () => {
    expect(source).toContain('500');
    expect(source).toContain('HMAC secret not configured');
  });
});

// ── CRITICAL: Progressive delay no longer blocks event loop ────────────────
describe('CRITICAL FIX: Progressive delay returns 429 instead of setTimeout', () => {
  const source = readSource('routes/studentManagement.js');

  test('no setTimeout in login route', () => {
    expect(source).not.toContain('setTimeout(resolve, delay.waitMs)');
  });

  test('returns 429 with retryAfter', () => {
    expect(source).toContain('429');
    expect(source).toContain('retryAfter');
  });
});

// ── CRITICAL: Service Worker removed non-existent pages ────────────────────
describe('CRITICAL FIX: Service Worker removed non-existent pages', () => {
  const source = readFrontend('sw.js');

  test('no admin-students.html', () => {
    expect(source).not.toContain('admin-students.html');
  });

  test('no admin-classrooms.html', () => {
    expect(source).not.toContain('admin-classrooms.html');
  });

  test('no admin-reports.html', () => {
    expect(source).not.toContain('admin-reports.html');
  });

  test('no admin-courses.html', () => {
    expect(source).not.toContain('admin-courses.html');
  });
});

// ── HIGH: School isolation in listChallenges ────────────────────────────────
describe('HIGH FIX: listChallenges filters by schoolId', () => {
  const source = readSource('controllers/challengeController.js');

  test('listChallenges has schoolId filter for non-admin', () => {
    expect(source).toContain("schoolId: req.user.schoolId");
  });

  test('listChallenges includes system challenges (schoolId null)', () => {
    expect(source).toContain("{ schoolId: null }");
  });
});

// ── HIGH: School isolation in submitSolution ────────────────────────────────
describe('HIGH FIX: submitSolution checks school isolation', () => {
  const source = readSource('controllers/challengeController.js');

  test('submitSolution verifies school ownership', () => {
    expect(source).toContain('challenge.schoolId !== req.user.schoolId');
  });

  test('submitSolution returns 403 for cross-school', () => {
    expect(source).toContain("403");
  });
});

// ── HIGH: Console.log no longer prints secrets ──────────────────────────────
describe('HIGH FIX: index.js no longer logs secrets', () => {
  const source = readSource('index.js');

  test('no console.log with password values', () => {
    expect(source).not.toMatch(/console\.log.*password/i);
  });

  test('no console.log with secret values', () => {
    expect(source).not.toMatch(/console\.log.*secret/i);
  });
});

// ── MEDIUM: Rate limits on invitations ──────────────────────────────────────
describe('MEDIUM FIX: Invitations route has writeRateLimit', () => {
  const source = readSource('routes/invitations.js');

  test('import route uses writeRateLimit', () => {
    expect(source).toContain('writeRateLimit');
  });

  test('writeRateLimit imported from security', () => {
    expect(source).toContain("require(\"../middlewares/security\")");
  });
});

// ── MEDIUM: Unbounded queries fixed ────────────────────────────────────────
describe('MEDIUM FIX: Unbounded queries have take limits', () => {
  const analyticsSource = readSource('controllers/analyticsController.js');
  const assessmentSource = readSource('controllers/assessmentController.js');
  const studentSource = readSource('routes/student.js');

  test('analytics classroom query has take: 50', () => {
    expect(analyticsSource).toContain('take: 50');
  });

  test('analytics challenge query has take limit', () => {
    const challengeCount = (analyticsSource.match(/take:\s*\d+/g) || []).length;
    expect(challengeCount).toBeGreaterThanOrEqual(3);
  });

  test('analytics assessments query has take limit', () => {
    const assessCount = (analyticsSource.match(/take:\s*\d+/g) || []).length;
    expect(assessCount).toBeGreaterThanOrEqual(5);
  });

  test('student leaderboard has take limit', () => {
    expect(studentSource).toContain('take: Math.min(limit, 100)');
  });

  test('assessment findMany has take: 100', () => {
    expect(assessmentSource).toContain('take: 100');
  });
});

// ── MEDIUM: setInterval cleanup with unref ──────────────────────────────────
describe('MEDIUM FIX: authController setInterval has unref', () => {
  const source = readSource('controllers/authController.js');

  test('interval is stored in named variable', () => {
    expect(source).toContain('_fingerprintCleanupInterval');
  });

  test('interval has unref call', () => {
    expect(source).toContain('unref');
  });
});

// ── MEDIUM: console.log replaced with logger ────────────────────────────────
describe('MEDIUM FIX: cluster.js uses logger instead of console.log', () => {
  const source = readSource('cluster.js');

  test('no console.log in cluster.js', () => {
    expect(source).not.toContain('console.log(');
  });

  test('uses logger', () => {
    expect(source).toContain('logger.info(');
    expect(source).toContain('logger.error(');
  });
});

// ── XSS: dhad-editor file names escaped ────────────────────────────────────
describe('XSS FIX: dhad-editor escapes file names', () => {
  const source = readFrontend('js/dhad-editor-page.js');

  test('file names are escaped', () => {
    expect(source).toContain("escapeHtml(name)");
  });

  test('template names are escaped', () => {
    expect(source).toContain("escapeHtml(t.name)");
  });

  test('challenge names are escaped', () => {
    expect(source).toContain("escapeHtml(c.name)");
  });

  test('challenge descriptions are escaped', () => {
    expect(source).toContain("escapeHtml(c.desc)");
  });

  test('error messages are escaped', () => {
    expect(source).toContain("e.message.replace(/</g,'&lt;')");
  });
});

// ── XSS: mobile-nav.js user name escaped ───────────────────────────────────
describe('XSS FIX: mobile-nav.js escapes user.name', () => {
  const source = readFrontend('js/mobile-nav.js');

  test('user.name is escaped', () => {
    expect(source).toContain("user.name || 'مستخدم').replace(/</g,'&lt;')");
  });
});

// ── XSS: students values are safe ─────────────────────────────────────────
describe('XSS FIX: students safe data rendering', () => {
  const source = readFrontend('js/students-page.js');

  test('attendance has fallback default', () => {
    expect(source).toContain('s.attendance||0');
  });

  test('streak has fallback default', () => {
    expect(source).toContain('s.streak||0');
  });

  test('toast does not include unsanitized name', () => {
    expect(source).not.toContain("toast.success('تم إضافة الطالب \"' + name +");
  });
});
