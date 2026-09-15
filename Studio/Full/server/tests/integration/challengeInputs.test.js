// ====================================================
// Item-2 integration — inputs visible in list + honored on submit.
// Mocks: DB rows only. Execution is REAL (isolated worker).
// ====================================================
const { createTestClient, generateTestJWT, prisma } = require('../helpers/app');

const request = require('supertest');
const app = require('../../src/index');

const STUDENT_TOKEN = generateTestJWT({ id: 'student-1', role: 'STUDENT', schoolId: 'school-1' });

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

const DEMO = {
  id: 'demo-input-1', tier: 1, order: 31, title: 'إدخال: المثال',
  expectedOutput: 'العمر: 7', dynamicOutput: '', requirements: '[]',
  difficulty: 'BEGINNER', inputs: '["7"]',
};

describe('Item-2 challenge inputs (integration)', () => {
  beforeEach(() => jest.clearAllMocks());

  test('list exposes inputs field', async () => {
    prisma.challenge.findMany.mockResolvedValue([{ ...DEMO }]);
    prisma.submission.findMany.mockResolvedValue([]);

    const res = await authReq('get', '/api/v1/challenges', STUDENT_TOKEN);
    expect(res.status).toBe(200);
    expect(res.body.challenges[0].inputs).toBe('["7"]');
  });

  test('submit consumes challenge inputs (real worker execution)', async () => {
    prisma.challenge.findUnique.mockResolvedValue({ ...DEMO });
    prisma.challenge.findMany.mockResolvedValue([{ id: 'demo-input-1', order: 31 }]);
    prisma.submission.findFirst.mockResolvedValue(null);
    prisma.user.findUnique.mockResolvedValue({
      id: 'student-1', name: 'باحث', isActive: true, isApproved: true,
      tokenVersion: 0, schoolId: 'school-1', invitation: { isActivated: true },
    });

    const res = await authReq('post', '/api/v1/student/challenge/submit', STUDENT_TOKEN, {
      challengeId: 'demo-input-1',
      code: 'صحيح العمر = ادخل()\nطباعة("العمر: " + العمر)',
    });
    expect(res.status).toBe(200);
    expect(res.body.passed).toBe(true);
    expect(res.body.score).toBe(100);
  }, 20000);

  test('submit with exhausted inputs falls back to 0', async () => {
    prisma.challenge.findUnique.mockResolvedValue({ ...DEMO, inputs: '[]' });
    prisma.challenge.findMany.mockResolvedValue([{ id: 'demo-input-1', order: 31 }]);
    prisma.submission.findFirst.mockResolvedValue(null);
    prisma.user.findUnique.mockResolvedValue({
      id: 'student-1', name: 'باحث', isActive: true, isApproved: true,
      tokenVersion: 0, schoolId: 'school-1', invitation: { isActivated: true },
    });

    const res = await authReq('post', '/api/v1/student/challenge/submit', STUDENT_TOKEN, {
      challengeId: 'demo-input-1',
      code: 'صحيح العمر = ادخل()\nطباعة("العمر: " + العمر)',
    });
    expect(res.status).toBe(200);
    expect(res.body.passed).toBe(false);
  }, 20000);
});
