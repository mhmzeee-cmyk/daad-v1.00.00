// ========================================================
// Sprint 4 Fixes Tests — XSS, Server Hardening, Frontend
// ========================================================

const fs = require('fs');
const path = require('path');

function readSource(file) {
  return fs.readFileSync(path.join(__dirname, '../../src/', file), 'utf8');
}

function readFrontend(file) {
  return fs.readFileSync(path.join(__dirname, '../../../frontend-web/', file), 'utf8');
}

// ── XSS: student-dashboard.html ─────────────────────────────────────────────
describe('XSS FIX: student-dashboard uses escapeHtml', () => {
  const source = readFrontend('js/student-dashboard.js');

  test('activity challengeTitle is escaped', () => {
    expect(source).toContain("escapeHtml(details.challengeTitle)");
  });

  test('activity renderActionName is escaped', () => {
    expect(source).toContain("escapeHtml(renderActionName(a.action))");
  });

  test('leaderboard studentName is escaped', () => {
    expect(source).toContain("escapeHtml(item.studentName)");
  });

  test('assessment title is escaped', () => {
    expect(source).toContain("escapeHtml(a.title)");
  });
});

// ── XSS: assessments.html ────────────────────────────────────────────────────
describe('XSS FIX: assessments uses escapeHtml', () => {
  const source = readFrontend('js/assessments-page.js');

  test('instructions are escaped before innerHTML', () => {
    expect(source).toContain("escapeHtml(instructions).replace");
  });

  test('editor.value fallback uses escapeHtml', () => {
    expect(source).toContain("escapeHtml(editor.value)");
  });
});

// ── XSS: activate.html ──────────────────────────────────────────────────────
describe('XSS FIX: activate.html email is escaped', () => {
  const source = readFrontend('activate.html');

  test('email is escaped in innerHTML', () => {
    expect(source).toContain("escapeHtml(email)");
  });

  test('no template literal injection', () => {
    expect(source).not.toMatch(/innerHTML\s*=\s*`[^`]*\$\{email\}/);
  });
});

// ── XSS: reports.html ───────────────────────────────────────────────────────
describe('XSS FIX: reports progress values are escaped', () => {
  const source = readFrontend('js/reports-page.js');

  test('averageProgress is escaped', () => {
    expect(source).toContain("escapeHtml(String(data.averageProgress");
  });

  test('averageXP is escaped', () => {
    expect(source).toContain("escapeHtml(String(data.averageXP");
  });

  test('studentCount is escaped', () => {
    expect(source).toContain("escapeHtml(String(data.studentCount");
  });
});

// ── XSS: dhad.js highlight fallback ──────────────────────────────────────────
describe('XSS FIX: dhad.js highlight escapes when library unavailable', () => {
  const source = readFrontend('js/dhad.js');

  test('highlight fallback uses escapeHtml', () => {
    expect(source).toContain("escapeHtml(code)");
  });

  test('highlight fallback has inline escaping', () => {
    expect(source).toContain("map[ch]");
  });
});

// ── Service Worker: no API caching ───────────────────────────────────────────
describe('SECURITY FIX: Service Worker does not cache API responses', () => {
  const source = readFrontend('sw.js');

  test('API requests use network-only (no cache.put)', () => {
    const apiSection = source.substring(
      source.indexOf("url.pathname.startsWith('/api/')"),
      source.indexOf("cacheFirst(request)")
    );
    expect(apiSection).not.toContain('cache.put');
  });

  test('API offline returns 503 without caching', () => {
    expect(source).toContain("status: 503");
  });
});

// ── Server: listChallenges has take limit ────────────────────────────────────
describe('FIX: listChallenges has take limit', () => {
  const source = readSource('controllers/challengeController.js');

  test('findMany has take limit', () => {
    const fn = source.substring(
      source.indexOf('async function listChallenges'),
      source.indexOf('async function dailyChallenge')
    );
    expect(fn).toMatch(/take:\s*\d+/);
  });
});

// ── Server: challengeController ownership check ──────────────────────────────
describe('FIX: challengeController system-wide challenge ownership', () => {
  const source = readSource('controllers/challengeController.js');

  test('non-admins cannot modify system-wide challenges', () => {
    expect(source).toContain("existing.schoolId === null && req.user.role !== 'ADMIN'");
  });

  test('system-wide modification returns 403', () => {
    expect(source).toContain('يمكن للمسؤول فقط تعديل تحديات النظام');
  });
});

// ── Server: reportsController parallel queries ───────────────────────────────
describe('FIX: reportsController uses Promise.all for parallel queries', () => {
  const source = readSource('controllers/reportsController.js');

  test('uses Promise.all for 4 queries', () => {
    expect(source).toContain('await Promise.all([');
  });

  test('uses Map for O(1) lookups', () => {
    expect(source).toContain('new Map()');
    expect(source).toContain('loginsByUser');
    expect(source).toContain('activitiesByUser');
    expect(source).toContain('submissionsByUser');
  });

  test('dailyBreakdown uses pre-indexed data', () => {
    expect(source).toContain('loginsByDate');
    expect(source).toContain('activitiesByDate');
    expect(source).toContain('submissionsByDate');
  });
});

// ── Server: unhandledRejection exits ─────────────────────────────────────────
describe('FIX: unhandledRejection exits process', () => {
  const source = readSource('index.js');

  test('unhandledRejection calls process.exit(1)', () => {
    const fn = source.substring(
      source.indexOf("process.on('unhandledRejection'"),
      source.indexOf("process.on('uncaughtException'")
    );
    expect(fn).toContain('process.exit(1)');
  });

  test('redis disconnect has error logging', () => {
    expect(source).toContain('Redis disconnect failed during shutdown');
  });
});

// ── Server: schoolController uses logger ─────────────────────────────────────
describe('FIX: schoolController uses logger instead of console.log', () => {
  const source = readSource('controllers/schoolController.js');

  test('imports logger', () => {
    expect(source).toContain("require(\"../utils/logger\")");
    expect(source).toContain('logger');
  });

  test('no console.log in delete operations', () => {
    const deleteSection = source.substring(source.indexOf('async function deleteSchool'));
    expect(deleteSection).not.toContain('console.log');
  });

  test('no console.log in user delete', () => {
    const deleteUserSection = source.substring(source.indexOf('async function deleteUser'));
    expect(deleteUserSection).not.toContain('console.log');
  });
});

// ── Server: redis disconnect error handling ───────────────────────────────────
describe('FIX: redis disconnect has try/catch', () => {
  const source = readSource('utils/redis.js');

  test('disconnectRedis wraps quit in try/catch', () => {
    const fn = source.substring(
      source.indexOf('const disconnectRedis'),
      source.indexOf('const isRedisAvailable')
    );
    expect(fn).toContain('try');
    expect(fn).toContain('catch');
  });
});
