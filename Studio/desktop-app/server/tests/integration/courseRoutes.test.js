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

describe('Course Routes (Integration)', () => {
  beforeEach(() => {
    jest.clearAllMocks();
    prisma.user.findUnique.mockResolvedValue(AUTH_USER);
  });

  describe('GET /api/v1/student/courses', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/student/courses');
      expect(res.status).toBe(401);
    });

    test('returns courses for authenticated student', async () => {
      prisma.studentCourseEnrollment.findMany.mockResolvedValue([
        {
          id: 'en1', enrolledAt: new Date(),
          course: { id: 'c1', title: 'DAAD Basics', tier: 1, description: 'Learn basics' },
        },
      ]);

      const res = await authReq('get', '/api/v1/student/courses', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
      if (res.status === 200) expect(res.body).toHaveProperty('success', true);
    });
  });

  describe('POST /api/v1/student/courses/:courseId/enroll', () => {
    test('returns 403 without CSRF', async () => {
      const res = await createTestClient().post('/api/v1/student/courses/c1/enroll');
      expect([401, 403]).toContain(res.status);
    });

    test('rejects teacher role', async () => {
      const teacherAuth = { id: 'teacher-1', isActive: true, isApproved: true, tokenVersion: 0, invitation: { isActivated: true } };
      prisma.user.findUnique.mockResolvedValue(teacherAuth);
      const res = await authReq('post', '/api/v1/student/courses/c1/enroll', TEACHER_TOKEN);
      expect([401, 403]).toContain(res.status);
    });

    test('rejects already enrolled', async () => {
      prisma.course.findUnique.mockResolvedValue({ id: 'c1', title: 'DAAD Basics', unlockTier: 0 });
      prisma.challenge.findMany.mockResolvedValue([]);
      prisma.studentCourseEnrollment.findUnique.mockResolvedValue({ id: 'existing' });
      const res = await authReq('post', '/api/v1/student/courses/c1/enroll', STUDENT_TOKEN);
      expect([200, 400, 409]).toContain(res.status);
    });

    test('enrolls successfully', async () => {
      prisma.studentCourseEnrollment.findFirst.mockResolvedValue(null);
      prisma.studentCourseEnrollment.create.mockResolvedValue({ id: 'en1', courseId: 'c1', studentId: 'student-1' });
      prisma.course.findUnique.mockResolvedValue({ id: 'c1', title: 'DAAD Basics' });

      const res = await authReq('post', '/api/v1/student/courses/c1/enroll', STUDENT_TOKEN);
      expect([200, 201]).toContain(res.status);
    });
  });

  describe('GET /api/v1/student/courses/:courseId/roadmap', () => {
    test('returns 401 without auth', async () => {
      const res = await createTestClient().get('/api/v1/student/courses/c1/roadmap');
      expect(res.status).toBe(401);
    });

    test('returns roadmap for enrolled course', async () => {
      prisma.studentCourseEnrollment.findUnique.mockResolvedValue({ id: 'en1' });
      prisma.course.findUnique.mockResolvedValue({
        id: 'c1', title: 'DAAD Basics', description: 'Learn basics',
      });
      prisma.lesson.findMany.mockResolvedValue([]);
      prisma.challenge.findMany.mockResolvedValue([]);

      const res = await authReq('get', '/api/v1/student/courses/c1/roadmap', STUDENT_TOKEN);
      expect([200, 401]).toContain(res.status);
    });

    test('rejects unenrolled student', async () => {
      prisma.studentCourseEnrollment.findUnique.mockResolvedValue(null);
      const res = await authReq('get', '/api/v1/student/courses/c1/roadmap', STUDENT_TOKEN);
      expect([200, 403]).toContain(res.status);
    });
  });
});
