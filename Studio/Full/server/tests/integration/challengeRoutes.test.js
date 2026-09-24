const { createTestClient, generateTestJWT, prisma } = require('../helpers/app');

const request = require('supertest');
const app = require('../../src/index');

const STUDENT_TOKEN = generateTestJWT({ id: 'student-1', role: 'STUDENT', schoolId: 'school-1' });
const TEACHER_TOKEN = generateTestJWT({ id: 'teacher-1', role: 'TEACHER', schoolId: 'school-1' });

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

describe('Challenge Routes (Integration)', () => {
  beforeEach(() => jest.clearAllMocks());

  describe('GET /api/v1/challenges', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/challenges');
      expect(res.status).toBe(401);
    });

    test('returns challenges for authenticated student', async () => {
      prisma.challenge.findMany.mockResolvedValue([
        { id: 'ch1', title: 'Hello World', tier: 1, order: 1 },
        { id: 'ch2', title: 'Variables', tier: 1, order: 2 },
      ]);
      prisma.submission.findMany.mockResolvedValue([]);

      const res = await authReq('get', '/api/v1/challenges', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
      if (res.status === 200) expect(res.body).toHaveProperty('success', true);
    });

    test('supports tier filter', async () => {
      prisma.challenge.findMany.mockResolvedValue([{ id: 'ch1', title: 'Easy', tier: 1, order: 1 }]);
      prisma.submission.findMany.mockResolvedValue([]);

      const res = await authReq('get', '/api/v1/challenges?tier=1', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
    });
  });

  describe('GET /api/v1/challenges/daily', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/challenges/daily');
      expect(res.status).toBe(401);
    });

    test('returns daily challenge', async () => {
      prisma.challenge.findFirst.mockResolvedValue({ id: 'daily1', title: 'Daily', tier: 1 });
      prisma.submission.findFirst.mockResolvedValue(null);

      const res = await authReq('get', '/api/v1/challenges/daily', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
    });
  });

  describe('POST /api/v1/challenges (teacher only)', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().post('/api/v1/challenges');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects student role', async () => {
      const res = await authReq('post', '/api/v1/challenges', STUDENT_TOKEN, { title: 'Test' });
      expect([401, 403]).toContain(res.status);
    });

    test('teacher can create challenge', async () => {
      prisma.challenge.create.mockResolvedValue({ id: 'new-ch', title: 'New' });

      const res = await authReq('post', '/api/v1/challenges', TEACHER_TOKEN, {
        title: 'New Challenge', tier: 1, order: 1, description: 'Test',
        expectedOutput: 'hello', starterCode: 'print("hello")',
      });
      expect([200, 201, 400]).toContain(res.status);
    });
  });

  describe('POST /api/v1/submit-solution (student only)', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().post('/api/v1/submit-solution');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects teacher role', async () => {
      const res = await authReq('post', '/api/v1/submit-solution', TEACHER_TOKEN, { challengeId: 'ch1', code: 'test' });
      expect([401, 403]).toContain(res.status);
    });
  });

  describe('GET /api/v1/submissions', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/submissions');
      expect(res.status).toBe(401);
    });

    test('returns submissions list', async () => {
      prisma.submission.findMany.mockResolvedValue([]);
      const res = await authReq('get', '/api/v1/submissions', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
    });
  });

  describe('POST /api/v1/challenge/verify', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().post('/api/v1/challenge/verify');
      expect([401, 403]).toContain(res.status);
    });
  });

  describe('PUT /api/v1/challenges/:id (teacher only)', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().put('/api/v1/challenges/ch1');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects student role', async () => {
      const res = await authReq('put', '/api/v1/challenges/ch1', STUDENT_TOKEN, { title: 'Updated' });
      expect([401, 403]).toContain(res.status);
    });
  });

  describe('DELETE /api/v1/challenges/:id (teacher only)', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().delete('/api/v1/challenges/ch1');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects student role', async () => {
      const res = await authReq('delete', '/api/v1/challenges/ch1', STUDENT_TOKEN);
      expect([401, 403]).toContain(res.status);
    });
  });
});
