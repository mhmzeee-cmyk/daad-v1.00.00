jest.mock('../../src/middlewares/strictSecurity', () => ({
  strictSecurity: [],
  strictSanitize: (v) => v,
}));

jest.mock('../../src/utils/logger', () => ({
  logger: { info: jest.fn(), warn: jest.fn(), error: jest.fn(), debug: jest.fn() },
}));

jest.mock('../../src/utils/prisma', () => ({
  course: { findMany: jest.fn(), findUnique: jest.fn() },
  studentCourseEnrollment: { findFirst: jest.fn(), findUnique: jest.fn(), findMany: jest.fn(), create: jest.fn() },
  lesson: { findMany: jest.fn() },
  challenge: { findMany: jest.fn() },
  submission: { findMany: jest.fn() },
}));

const prisma = require('../../src/utils/prisma');
const {
  getCourses,
  enrollCourse,
  getCourseRoadmap,
} = require('../../src/controllers/courseController');

function mockReq(body, params, user) {
  return {
    body: body || {},
    params: params || {},
    query: body || {},
    ip: '127.0.0.1',
    cookies: {},
    headers: {},
    path: '/courses',
    method: 'GET',
    app: { get: jest.fn().mockReturnValue(null) },
    user: user || null,
  };
}

function mockRes() {
  const res = { statusCode: null, body: null };
  res.status = jest.fn().mockReturnValue(res);
  res.json = jest.fn().mockReturnValue(res);
  return res;
}

describe('courseController', () => {
  let next;

  beforeEach(() => {
    next = jest.fn();
    jest.clearAllMocks();
  });

  describe('getCourses', () => {
    test('returns published non-MAIN courses', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findMany.mockResolvedValue([{ id: 'co1', title: 'Web', category: 'WEB', _count: { lessons: 3 } }]);
      prisma.studentCourseEnrollment.findMany.mockResolvedValue([]);
      prisma.challenge.findMany.mockResolvedValue([{ tier: 1, submissions: [] }]);

      await getCourses(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ courses: expect.any(Array) }));
    });

    test('includes enrollment status', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findMany.mockResolvedValue([{ id: 'co1', title: 'Web', category: 'WEB', _count: { lessons: 3 } }]);
      prisma.studentCourseEnrollment.findMany.mockResolvedValue([{ courseId: 'co1' }]);
      prisma.challenge.findMany.mockResolvedValue([{ tier: 1, submissions: [] }]);

      await getCourses(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ courses: expect.any(Array) }));
    });

    test('marks LOCKED when tier not met', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findMany.mockResolvedValue([{ id: 'co1', title: 'Adv', category: 'ADV', unlockTier: 5, _count: { lessons: 3 } }]);
      prisma.studentCourseEnrollment.findMany.mockResolvedValue([]);
      prisma.challenge.findMany.mockResolvedValue([]);

      await getCourses(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ courses: expect.any(Array) }));
    });

    test('handles db error', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findMany.mockRejectedValue(new Error('DB'));

      await getCourses(req, res, next);

      expect(next).toHaveBeenCalledWith(expect.any(Error));
    });
  });

  describe('enrollCourse', () => {
    test('enrolls student', async () => {
      const req = mockReq({}, { courseId: 'co1' }, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findUnique.mockResolvedValue({ id: 'co1', unlockTier: 0, published: true });
      prisma.challenge.findMany.mockResolvedValue([{ tier: 1, submissions: [] }]);
      prisma.studentCourseEnrollment.findUnique.mockResolvedValue(null);
      prisma.studentCourseEnrollment.create.mockResolvedValue({ id: 'e1', status: 'ENROLLED' });

      await enrollCourse(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ enrollment: expect.any(Object) }));
    });

    test('idempotent — returns existing', async () => {
      const req = mockReq({}, { courseId: 'co1' }, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findUnique.mockResolvedValue({ id: 'co1', unlockTier: 0, published: true });
      prisma.challenge.findMany.mockResolvedValue([{ tier: 1, submissions: [] }]);
      prisma.studentCourseEnrollment.findUnique.mockResolvedValue({ id: 'e1', status: 'ENROLLED' });

      await enrollCourse(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ message: expect.stringContaining('مسجّل') }));
      expect(prisma.studentCourseEnrollment.create).not.toHaveBeenCalled();
    });

    test('404 for non-existent course', async () => {
      const req = mockReq({}, { courseId: 'c999' }, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findUnique.mockResolvedValue(null);

      await enrollCourse(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });

    test('403 when tier locked', async () => {
      const req = mockReq({}, { courseId: 'co1' }, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findUnique.mockResolvedValue({ id: 'co1', unlockTier: 5, published: true });
      prisma.challenge.findMany.mockResolvedValue([]);

      await enrollCourse(req, res, next);

      expect(res.status).toHaveBeenCalledWith(403);
    });
  });

  describe('getCourseRoadmap', () => {
    test('returns roadmap for enrolled', async () => {
      const req = mockReq({}, { courseId: 'co1' }, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findUnique.mockResolvedValue({ id: 'co1', title: 'Web', description: '', icon: '', category: 'WEB' });
      prisma.studentCourseEnrollment.findUnique.mockResolvedValue({ courseId: 'co1', status: 'ENROLLED' });
      prisma.lesson.findMany.mockResolvedValue([{ id: 'l1', title: 'Intro', order: 1 }]);
      prisma.challenge.findMany.mockResolvedValue([]);

      await getCourseRoadmap(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ success: true, roadmap: expect.any(Object) }));
    });

    test('403 for non-enrolled', async () => {
      const req = mockReq({}, { courseId: 'co1' }, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findUnique.mockResolvedValue({ id: 'co1' });
      prisma.studentCourseEnrollment.findUnique.mockResolvedValue(null);

      await getCourseRoadmap(req, res, next);

      expect(res.status).toHaveBeenCalledWith(403);
    });

    test('404 for non-existent course', async () => {
      const req = mockReq({}, { courseId: 'c999' }, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.course.findUnique.mockResolvedValue(null);

      await getCourseRoadmap(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });
});
