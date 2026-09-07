// ========================================================
// Sprint 3 High Fixes Tests
// ========================================================

const fs = require('fs');
const path = require('path');

function readSource(file) {
  return fs.readFileSync(path.join(__dirname, '../../src/', file), 'utf8');
}

// ── FIX H3: getClassProgress query limits ────────────────────────────────────
describe('FIX H3: getClassProgress bounded queries to prevent OOM', () => {
  const source = readSource('controllers/analyticsController.js');

  test('students query has take limit', () => {
    const fn = source.substring(
      source.indexOf('async function getClassProgress'),
      source.indexOf('async function getTestCases')
    );
    expect(fn).toMatch(/students = await prisma\.user\.findMany[\s\S]*take:\s*\d+/);
  });

  test('submissions use SQL aggregation (groupBy) instead of findMany', () => {
    const fn = source.substring(
      source.indexOf('async function getClassProgress'),
      source.indexOf('async function getTestCases')
    );
    expect(fn).toContain('submission.groupBy');
  });

  test('submissions filtered by exercise IDs to avoid loading all', () => {
    const fn = source.substring(
      source.indexOf('async function getClassProgress'),
      source.indexOf('async function getTestCases')
    );
    expect(fn).toContain('challengeId: { in: exerciseIds }');
  });

  test('uses aggregate instead of loading all profiles', () => {
    const fn = source.substring(
      source.indexOf('async function getClassProgress'),
      source.indexOf('async function getTestCases')
    );
    expect(fn).toContain('studentProfile.aggregate');
    expect(fn).toContain('_avg: { totalXP: true }');
  });
});

// ── FIX H11: Leaderboard Redis cache path bounded ───────────────────────────
describe('FIX H11: Leaderboard Redis cache path bounded', () => {
  const source = readSource('controllers/analyticsController.js');

  test('cached leaderboard is capped before enrichment', () => {
    expect(source).toContain('const cappedLeaderboard = cachedLeaderboard.slice(0, Math.min(limit + 10, 200))');
  });

  test('enrichment query has take limit', () => {
    expect(source).toMatch(/students = await prisma\.user\.findMany[\s\S]*take:\s*200/);
  });

  test('enriched leaderboard filters from capped list', () => {
    expect(source).toContain('const enrichedLeaderboard = cappedLeaderboard');
  });
});

// ── FIX H13: Schools routes have rate limits ────────────────────────────────
describe('FIX H13: Schools routes have rate limits', () => {
  const source = readSource('routes/schools.js');

  test('PUT school has writeRateLimit', () => {
    expect(source).toMatch(/router\.put\(.*writeRateLimit/);
  });

  test('DELETE school has destructiveRateLimit', () => {
    expect(source).toMatch(/router\.delete\(.*destructiveRateLimit/);
  });

  test('DELETE user has destructiveRateLimit', () => {
    expect(source).toMatch(/router\.delete\(\s*\n\s*"\/schools\/:id\/users\/:userId"[\s\S]*destructiveRateLimit/);
  });

  test('imports writeRateLimit and destructiveRateLimit', () => {
    expect(source).toContain('writeRateLimit');
    expect(source).toContain('destructiveRateLimit');
  });
});

// ── FIX H17: getClassroomStudents has take limit ────────────────────────────
describe('FIX H17: getClassroomStudents has take limit', () => {
  const source = readSource('controllers/studentManagementController.js');

  test('getClassroomStudents has take limit', () => {
    const fn = source.substring(
      source.indexOf('async function getClassroomStudents'),
      source.indexOf('async function studentLogin')
    );
    expect(fn).toContain('take: 100');
  });
});

// ── FIX H18: Challenges routes have rate limits ─────────────────────────────
describe('FIX H18: Challenges routes have write rate limits', () => {
  const source = readSource('routes/challenges.js');

  test('POST challenges has writeRateLimit', () => {
    expect(source).toMatch(/router\.post\(.*"\/challenges".*writeRateLimit/);
  });

  test('PUT challenges has writeRateLimit', () => {
    expect(source).toMatch(/router\.put\(.*writeRateLimit/);
  });

  test('DELETE challenges has writeRateLimit', () => {
    expect(source).toMatch(/router\.delete\(.*writeRateLimit/);
  });

  test('POST publish has writeRateLimit', () => {
    expect(source).toMatch(/router\.post\(.*"\/challenges\/:id\/publish".*writeRateLimit/);
  });

  test('POST set-daily has writeRateLimit', () => {
    expect(source).toMatch(/router\.post\(.*"\/challenges\/:id\/set-daily".*writeRateLimit/);
  });

  test('imports writeRateLimit', () => {
    expect(source).toContain('writeRateLimit');
  });
});

// ── FIX H22: getModelFromRoute uses path segment matching ───────────────────
describe('FIX H22: getModelFromRoute uses path segment matching', () => {
  const source = readSource('middlewares/auth.js');

  test('splits URL into segments', () => {
    expect(source).toContain("url.split('/').filter(Boolean)");
  });

  test('uses object map for segment matching', () => {
    expect(source).toContain("'challenges': 'challenge'");
    expect(source).toContain("'submissions': 'submission'");
    expect(source).toContain("'schools': 'school'");
    expect(source).toContain("'courses': 'course'");
    expect(source).toContain("'users': 'user'");
    expect(source).toContain("'classrooms': 'classroom'");
    expect(source).toContain("'assessments': 'assessment'");
  });

  test('does not use substring matching', () => {
    expect(source).not.toMatch(/url\.includes\(pattern\)/);
  });
});

// ── FIX H23: Auth middleware single DB query ─────────────────────────────────
describe('FIX H23: Auth middleware merges activation + token check into single query', () => {
  const source = readSource('middlewares/auth.js');

  test('single findUnique query in authenticate', () => {
    const fn = source.substring(
      source.indexOf('async function authenticate'),
      source.indexOf('function requireRole')
    );
    // Should have exactly 1 findUnique call (the merged one)
    const matches = fn.match(/prisma\[?\.?user\]?\.findUnique/g);
    expect(matches).toHaveLength(1);
  });

  test('combined query fetches isActive and tokenVersion', () => {
    const fn = source.substring(
      source.indexOf('async function authenticate'),
      source.indexOf('function requireRole')
    );
    expect(fn).toContain('isActive: true');
    expect(fn).toContain('tokenVersion: true');
  });

  test('student query includes invitation', () => {
    const fn = source.substring(
      source.indexOf('async function authenticate'),
      source.indexOf('function requireRole')
    );
    expect(fn).toContain("selectFields.isApproved = true");
    expect(fn).toContain("selectFields.invitation = { select: { isActivated: true } }");
  });

  test('checks tokenVersion from single query result', () => {
    const fn = source.substring(
      source.indexOf('async function authenticate'),
      source.indexOf('function requireRole')
    );
    expect(fn).toContain('user.tokenVersion !== (decoded.tokenVersion || 0)');
  });
});

// ── Security.js exports ─────────────────────────────────────────────────────
describe('FIX H13+H18: security.js exports rate limiters', () => {
  const source = readSource('middlewares/security.js');

  test('writeRateLimit is defined and exported', () => {
    expect(source).toContain('writeRateLimit');
    expect(source).toMatch(/module\.exports[\s\S]*writeRateLimit/);
  });

  test('destructiveRateLimit is defined and exported', () => {
    expect(source).toContain('destructiveRateLimit');
    expect(source).toMatch(/module\.exports[\s\S]*destructiveRateLimit/);
  });

  test('writeRateLimit max is 30', () => {
    const fn = source.substring(
      source.indexOf('writeRateLimit = rateLimit'),
      source.indexOf('destructiveRateLimit = rateLimit')
    );
    expect(fn).toContain('max: 30');
  });

  test('destructiveRateLimit max is 5', () => {
    const fn = source.substring(
      source.indexOf('destructiveRateLimit = rateLimit'),
      source.indexOf('module.exports')
    );
    expect(fn).toContain('max: 5');
  });
});
