// ========================================================
// Sprint 2 High Fixes Tests
// ========================================================

const fs = require('fs');
const path = require('path');

function readSource(file) {
  return fs.readFileSync(path.join(__dirname, '../../src/', file), 'utf8');
}

// ── FIX H2: Leaderboard Query Capped ─────────────────────────────────────────
describe('FIX H2: Leaderboard query capped to prevent OOM', () => {
  const source = readSource('controllers/analyticsController.js');

  test('leaderboard query has take limit', () => {
    expect(source).toMatch(/take:\s*Math\.min\(limit\s*\+\s*10,\s*200\)/);
  });

  test('leaderboard uses _count instead of loading all submissions', () => {
    expect(source).toContain('_count: {');
    expect(source).toContain('submissions: { where: { passed: true } }');
  });
});

// ── FIX H6: completeAssessment Classroom Check ────────────────────────────────
describe('FIX H6: completeAssessment classroom ownership check', () => {
  const source = readSource('controllers/assessmentController.js');

  test('completeAssessment verifies classroom ownership', () => {
    const fn = source.substring(
      source.indexOf('async function completeAssessment'),
      source.indexOf('async function getAssessmentResults')
    );
    expect(fn).toContain('classroomId: true');
    expect(fn).toContain('assessment.classroomId !== req.user.classroomId');
  });
});

// ── FIX H10: getPublicClassrooms Pagination ──────────────────────────────────
describe('FIX H10: getPublicClassrooms has pagination limit', () => {
  const source = readSource('controllers/studentManagementController.js');

  test('getPublicClassrooms has take limit', () => {
    const fn = source.substring(
      source.indexOf('async function getPublicClassrooms'),
      source.indexOf('async function getClassroomStudents')
    );
    expect(fn).toContain('take: 100');
  });
});

// ── FIX H15: checkAccountLocked Server-Side ID ───────────────────────────────
describe('FIX H15: checkAccountLocked uses server-side ID', () => {
  const source = readSource('routes/challenges.js');

  test('checkAccountLocked uses req.user.id, not req.body.studentId', () => {
    const fn = source.substring(
      source.indexOf('async function checkAccountLocked'),
      source.indexOf('next();') + 10
    );
    expect(fn).toContain('req.user?.id');
    expect(fn).not.toMatch(/req\.body\.studentId/);
  });
});

// ── FIX H21: getModelFromRoute Explicit Mapping ──────────────────────────────
describe('FIX H21: getModelFromRoute uses explicit path matching', () => {
  const source = readSource('middlewares/auth.js');

  test('getModelFromRoute uses path segment matching', () => {
    const fnStart = source.indexOf('function getModelFromRoute');
    const fnEnd = source.indexOf('return "unknown"', fnStart);
    const fn = source.substring(fnStart, fnEnd + 20);
    expect(fn).toContain("'challenges': 'challenge'");
    expect(fn).toContain("'submissions': 'submission'");
    expect(fn).toContain("'schools': 'school'");
  });

  test('getModelFromRoute returns unknown for unmatched routes', () => {
    expect(source).toContain('return "unknown"');
  });
});

// ── FIX H28: Session Store Cleanup ────────────────────────────────────────────
describe('FIX H28: In-memory session store has cleanup', () => {
  const source = readSource('utils/sessionStore.js');

  test('has MEMORY_SESSION_MAX constant', () => {
    expect(source).toContain('MEMORY_SESSION_MAX');
  });

  test('has cleanup interval for in-memory sessions', () => {
    expect(source).toContain('setInterval');
  });

  test('evicts expired sessions', () => {
    expect(source).toContain('createdAt');
  });

  test('hard cap evicts oldest 10%', () => {
    expect(source).toContain('evictCount');
    expect(source).toContain('0.1');
  });
});

// ── FIX H32: OTP Not Returned in Response ────────────────────────────────────
describe('FIX H32: OTP not returned in API response', () => {
  const source = readSource('services/adminService.js');

  test('importStudents does not include OTP in success response', () => {
    const importFn = source.substring(
      source.indexOf('results.successful.push'),
      source.indexOf('results.successful.push') + 200
    );
    expect(importFn).not.toMatch(/otp,?\s*\/\//);
    expect(importFn).not.toMatch(/otp\s*}/);
  });
});

// ── FIX H33: Token Hash Full Length ──────────────────────────────────────────
describe('FIX H33: Token hash uses full SHA-256', () => {
  const source = readSource('services/authService.js');

  test('hashToken does not truncate', () => {
    expect(source).not.toMatch(/\.substring\(0,\s*16\)/);
  });

  test('hashToken uses full sha256 digest', () => {
    expect(source).toContain('crypto.createHash("sha256").update(token).digest("hex")');
  });
});

// ── FIX H5: exportClassroomReport Batch Queries ──────────────────────────────
describe('FIX H5: exportClassroomReport uses batch aggregation', () => {
  const source = readSource('controllers/assessmentController.js');

  test('exportClassroomReport uses groupBy for submissions', () => {
    const fn = source.substring(
      source.indexOf('async function exportClassroomReport'),
      source.indexOf('module.exports')
    );
    expect(fn).toContain('submission.groupBy');
    expect(fn).toContain('attendance.groupBy');
    expect(fn).toContain('studentAssessmentResult.groupBy');
  });

  test('exportClassroomReport does not load all child records', () => {
    const fn = source.substring(
      source.indexOf('async function exportClassroomReport'),
      source.indexOf('// Build CSV')
    );
    expect(fn).not.toContain('submissions: {');
    expect(fn).not.toContain('attendances: {');
    expect(fn).not.toContain('assessmentResults: {');
  });
});

// ── FIX H9: exportStudentsReport Batch Queries ──────────────────────────────
describe('FIX H9: exportStudentsReport uses batch queries', () => {
  const source = readSource('controllers/studentManagementController.js');

  test('exportStudentsReport uses groupBy for submissions', () => {
    const fn = source.substring(
      source.indexOf('async function exportStudentsReport'),
      source.indexOf('module.exports')
    );
    expect(fn).toContain('submission.groupBy');
  });

  test('exportStudentsReport uses Promise.all for parallel batch queries', () => {
    const fn = source.substring(
      source.indexOf('async function exportStudentsReport'),
      source.indexOf('// Index batch results')
    );
    expect(fn).toContain('Promise.all');
  });

  test('exportStudentsReport row building is synchronous (no async/await in map)', () => {
    const fn = source.substring(
      source.indexOf('const rows = students.map'),
      source.indexOf('res.json({ success: true, rows')
    );
    expect(fn).not.toMatch(/students\.map\(async/);
  });
});
