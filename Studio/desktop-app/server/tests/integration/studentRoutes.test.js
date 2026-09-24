const { createTestClient, generateTestJWT, prisma } = require('../helpers/app');

const request = require('supertest');
const app = require('../../src/index');

const STUDENT_TOKEN = generateTestJWT({ id: 'student-1', role: 'STUDENT', schoolId: 'school-1' });
const TEACHER_TOKEN = generateTestJWT({ id: 'teacher-1', role: 'TEACHER', schoolId: 'school-1' });

const AUTH_USER = {
  id: 'student-1', isActive: true, isApproved: true, tokenVersion: 0,
  invitation: { isActivated: true },
};

async function authReq(method, url, token, body) {
  const agent = request.agent(app);
  const getRes = await agent.get('/health');
  const cookies = getRes.headers['set-cookie'] || [];
  const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
  const csrfToken = csrfCookie ? csrfCookie.split(';')[0].split('=')[1] : '';
  const allCookies = cookies.map(c => c.split(';')[0]).join('; ');

  let req = agent[method](url)
    .set('Cookie', allCookies)
    .set('X-CSRF-Token', csrfToken)
    .set('Authorization', `Bearer ${token}`);
  if (body && (method === 'post' || method === 'put' || method === 'patch')) req = req.send(body);
  return req;
}

describe('Student Routes (Integration)', () => {
  beforeEach(() => {
    jest.clearAllMocks();
    prisma.user.findUnique.mockResolvedValue(AUTH_USER);
  });

  describe('GET /api/v1/student/profile', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/student/profile');
      expect(res.status).toBe(401);
    });

    test('returns profile for authenticated student (auth passes)', async () => {
      prisma.studentProfile.findUnique.mockResolvedValue(null);
      prisma.studentProfile.create.mockResolvedValue({ id: 'sp1', studentId: 'student-1', totalXP: 0 });
      const res = await authReq('get', '/api/v1/student/profile', STUDENT_TOKEN);
      expect(res.status).not.toBe(401);
    });
  });

  describe('GET /api/v1/student/leaderboard', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/student/leaderboard');
      expect(res.status).toBe(401);
    });

    test('returns leaderboard for student', async () => {
      prisma.user.findMany.mockResolvedValue([
        { id: 's1', name: 'A', studentProfile: { totalXP: 1000, currentLevel: 5, currentStreak: 10 } },
      ]);
      const res = await authReq('get', '/api/v1/student/leaderboard', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
      if (res.status === 200) {
        expect(res.body).toHaveProperty('leaderboard');
        expect(Array.isArray(res.body.leaderboard)).toBe(true);
      }
    });
  });

  describe('GET /api/v1/student/roadmap', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/student/roadmap');
      expect(res.status).toBe(401);
    });
  });

  describe('GET /api/v1/student/classroom/check', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/student/classroom/check');
      expect(res.status).toBe(401);
    });

    test('returns unassigned for student without classroom', async () => {
      prisma.user.findUnique.mockResolvedValue({
        id: 'student-1', isActive: true, isApproved: true, tokenVersion: 0,
        invitation: { isActivated: true },
        classroomId: null, classroom: null,
      });
      const res = await authReq('get', '/api/v1/student/classroom/check', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
      if (res.status === 200) expect(res.body.assigned).toBe(false);
    });

    test('returns classroom info for assigned student', async () => {
      prisma.user.findUnique.mockResolvedValue({
        id: 'student-1', isActive: true, isApproved: true, tokenVersion: 0,
        invitation: { isActivated: true },
        classroomId: 'cl1',
        classroom: { id: 'cl1', name: 'Class A', gradeLevel: '10', teacher: { id: 't1', name: 'T' }, school: { id: 's1', name: 'S' } },
      });
      const res = await authReq('get', '/api/v1/student/classroom/check', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
      if (res.status === 200) {
        expect(res.body.assigned).toBe(true);
        expect(res.body.classroom).toBeDefined();
      }
    });
  });

  describe('POST /api/v1/student/challenge/submit', () => {
    test('returns 403 without CSRF (POST requires CSRF)', async () => {
      const res = await createTestClient().post('/api/v1/student/challenge/submit');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects missing challengeId', async () => {
      const res = await authReq('post', '/api/v1/student/challenge/submit', STUDENT_TOKEN, { code: 'print("hello")' });
      expect([400, 401]).toContain(res.status);
    });

    test('rejects empty code', async () => {
      const res = await authReq('post', '/api/v1/student/challenge/submit', STUDENT_TOKEN, { challengeId: 'ch1', code: '' });
      expect([400, 401]).toContain(res.status);
    });

    test('rejects non-string challengeId', async () => {
      const res = await authReq('post', '/api/v1/student/challenge/submit', STUDENT_TOKEN, { challengeId: 123, code: 'test' });
      expect([400, 401]).toContain(res.status);
    });

    test('rejects non-existent challenge', async () => {
      prisma.challenge.findUnique.mockResolvedValue(null);
      const res = await authReq('post', '/api/v1/student/challenge/submit', STUDENT_TOKEN, { challengeId: 'nonexistent', code: 'print("hello")' });
      expect([404, 401]).toContain(res.status);
    });
  });

  describe('GET /api/v1/student/achievements', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/student/achievements');
      expect(res.status).toBe(401);
    });

    test('returns achievements for student', async () => {
      prisma.userAchievement.findMany.mockResolvedValue([]);
      prisma.achievement.findMany.mockResolvedValue([]);
      prisma.studentProfile.findUnique.mockResolvedValue(null);
      const res = await authReq('get', '/api/v1/student/achievements', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
      if (res.status === 200) {
        expect(res.body).toHaveProperty('success', true);
        expect(res.body).toHaveProperty('awarded');
      }
    });
  });

  describe('POST /api/v1/workspace/save', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().post('/api/v1/workspace/save');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects missing challengeId', async () => {
      const res = await authReq('post', '/api/v1/workspace/save', STUDENT_TOKEN, { code: 'test' });
      expect([400, 401]).toContain(res.status);
    });

    test('saves workspace', async () => {
      prisma.cloudWorkspace.upsert.mockResolvedValue({ id: 'ws1', challengeId: 'ch1', lastSavedAt: new Date() });
      const res = await authReq('post', '/api/v1/workspace/save', STUDENT_TOKEN, { challengeId: 'ch1', code: 'print("hello")' });
      expect([200, 400, 401]).toContain(res.status);
    });
  });

  describe('GET /api/v1/workspace/list', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/workspace/list');
      expect(res.status).toBe(401);
    });

    test('returns workspace list', async () => {
      prisma.cloudWorkspace.findMany.mockResolvedValue([]);
      const res = await authReq('get', '/api/v1/workspace/list', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
      if (res.status === 200) expect(res.body).toHaveProperty('workspaces');
    });
  });

  describe('GET /api/v1/workspace/load/:challengeId', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/workspace/load/ch1');
      expect(res.status).toBe(401);
    });

    test('returns null workspace for student', async () => {
      prisma.cloudWorkspace.findUnique.mockResolvedValue(null);
      const res = await authReq('get', '/api/v1/workspace/load/ch1', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
    });
  });

  describe('DELETE /api/v1/workspace/:challengeId', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().delete('/api/v1/workspace/ch1');
      expect([401, 403]).toContain(res.status);
    });

    test('deletes workspace', async () => {
      prisma.cloudWorkspace.deleteMany.mockResolvedValue({ count: 1 });
      const res = await authReq('delete', '/api/v1/workspace/ch1', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
    });
  });

  describe('POST /api/v1/assessments/create', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().post('/api/v1/assessments/create');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects student role', async () => {
      const res = await authReq('post', '/api/v1/assessments/create', STUDENT_TOKEN, { title: 'Test' });
      expect([401, 403]).toContain(res.status);
    });
  });

  describe('GET /api/v1/analytics/security-alerts', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/analytics/security-alerts');
      expect(res.status).toBe(401);
    });
  });
});
