jest.mock('../../src/middlewares/strictSecurity', () => ({
  strictSecurity: [],
  strictSanitize: (v) => v,
  validatePasswordStrength: jest.fn().mockReturnValue([]),
  recordFailedAttempt: jest.fn(),
  clearFailedAttempts: jest.fn(),
  isAccountLocked: jest.fn().mockReturnValue(false),
}));

jest.mock('bcryptjs', () => ({
  compare: jest.fn().mockResolvedValue(true),
  hash: jest.fn().mockResolvedValue('$2b$10$hashed'),
}));

jest.mock('../../src/utils/logger', () => ({
  logger: { info: jest.fn(), warn: jest.fn(), error: jest.fn(), debug: jest.fn() },
}));

const prismaMock = {
  user: { findFirst: jest.fn(), findUnique: jest.fn(), create: jest.fn(), update: jest.fn(), findMany: jest.fn() },
  studentProfile: { create: jest.fn() },
  classroom: { findMany: jest.fn(), findFirst: jest.fn(), findUnique: jest.fn() },
  school: { findUnique: jest.fn() },
  $transaction: jest.fn(),
  attendanceRecord: { findFirst: jest.fn(), create: jest.fn() },
  loginLog: { create: jest.fn() },
  activityLog: { create: jest.fn() },
  session: { create: jest.fn() },
  studentCourseEnrollment: { findMany: jest.fn() },
  submission: { findMany: jest.fn(), count: jest.fn() },
};

const {
  createStudent,
  bulkCreateStudents,
  getPublicClassrooms,
  getClassroomStudents,
  deleteStudent,
  getTeacherStudents,
} = require('../../src/controllers/studentManagementController');

function mockReq(body, params, user) {
  return {
    body: body || {},
    params: params || {},
    query: body || {},
    ip: '127.0.0.1',
    cookies: {},
    headers: { 'user-agent': 'test' },
    path: '/students',
    method: 'POST',
    app: { get: jest.fn().mockReturnValue(prismaMock) },
    user: user || null,
  };
}

function mockRes() {
  const res = { statusCode: null, body: null };
  res.status = jest.fn().mockReturnValue(res);
  res.json = jest.fn().mockReturnValue(res);
  res.cookie = jest.fn().mockReturnValue(res);
  return res;
}

describe('studentManagementController', () => {
  let next;

  beforeEach(() => {
    next = jest.fn();
    jest.clearAllMocks();
  });

  describe('createStudent', () => {
    test('creates student with valid data', async () => {
      const req = mockReq({ name: 'Ahmed', classroomId: 'cl1', nationalId: '123', password: 'Test1234!' }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.user.findFirst.mockResolvedValue(null);
      prismaMock.classroom.findFirst.mockResolvedValue({ id: 'cl1', teacherId: 'u1' });
      prismaMock.classroom.findUnique.mockResolvedValue({ id: 'cl1', schoolId: 's1' });
      prismaMock.school.findUnique.mockResolvedValue({ id: 's1', code: 'SCH' });
      prismaMock.user.create.mockResolvedValue({ id: 'u2', name: 'Ahmed', role: 'STUDENT', classroomId: 'cl1', classroom: { id: 'cl1', name: 'Class 1' } });
      prismaMock.studentProfile.create.mockResolvedValue({});

      await createStudent(req, res, next);

      expect(res.status).toHaveBeenCalledWith(201);
    });

    test('rejects missing name', async () => {
      const req = mockReq({ classroomId: 'cl1', password: 'Test1234!' }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();

      await createStudent(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects name > 100', async () => {
      const req = mockReq({ name: 'x'.repeat(101), classroomId: 'cl1', password: 'Test1234!' }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();

      await createStudent(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects missing password', async () => {
      const req = mockReq({ name: 'Ahmed', classroomId: 'cl1' }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.user.findFirst.mockResolvedValue(null);
      prismaMock.classroom.findFirst.mockResolvedValue({ id: 'cl1', teacherId: 'u1' });
      prismaMock.classroom.findUnique.mockResolvedValue({ id: 'cl1', schoolId: 's1' });
      prismaMock.school.findUnique.mockResolvedValue({ id: 's1', code: 'SCH' });

      await createStudent(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects duplicate nationalId', async () => {
      const req = mockReq({ name: 'Ahmed', classroomId: 'cl1', nationalId: '123', password: 'Test1234!' }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.classroom.findFirst.mockResolvedValue({ id: 'cl1', teacherId: 'u1' });
      prismaMock.classroom.findUnique.mockResolvedValue({ id: 'cl1', schoolId: 's1' });
      prismaMock.user.findFirst.mockResolvedValue({ id: 'existing' });

      await createStudent(req, res, next);

      expect(res.status).toHaveBeenCalledWith(409);
    });
  });

  describe('bulkCreateStudents', () => {
    test('creates multiple students', async () => {
      const req = mockReq({
        students: [{ name: 'Ahmed', nationalId: '111' }, { name: 'Sara', nationalId: '222' }],
        classroomId: 'cl1',
        password: 'Test1234!',
      }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.$transaction.mockResolvedValue([{ id: 'u2' }, { id: 'u3' }]);

      await bulkCreateStudents(req, res, next);

      expect(res.status).toHaveBeenCalledWith(201);
    });

    test('rejects empty array', async () => {
      const req = mockReq({ students: [], classroomId: 'cl1', password: 'Test1234!' }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();

      await bulkCreateStudents(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects > 50 students', async () => {
      const req = mockReq({
        students: Array(51).fill({ name: 'Test', nationalId: '123' }),
        classroomId: 'cl1',
        password: 'Test1234!',
      }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();

      await bulkCreateStudents(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });
  });

  describe('getPublicClassrooms', () => {
    test('returns classrooms', async () => {
      const req = mockReq();
      const res = mockRes();
      prismaMock.classroom.findMany.mockResolvedValue([
        { id: 'cl1', name: 'Class 1', gradeLevel: '10', school: { id: 's1', name: 'School' }, _count: { students: 10 } },
      ]);

      await getPublicClassrooms(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ classrooms: expect.any(Array) }));
    });
  });

  describe('getClassroomStudents', () => {
    test('returns students', async () => {
      const req = mockReq({}, { classroom_id: 'cl1' });
      const res = mockRes();
      prismaMock.user.findMany.mockResolvedValue([{ id: 'u1', name: 'Ahmed' }]);

      await getClassroomStudents(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });
  });

  describe('deleteStudent', () => {
    test('soft-deletes student', async () => {
      const req = mockReq({}, { id: 'u2' }, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.user.findUnique.mockResolvedValue({ id: 'u2', schoolId: 's1', role: 'STUDENT', classroomId: 'cl1' });
      prismaMock.classroom.findFirst.mockResolvedValue({ id: 'cl1', teacherId: 'u1' });
      prismaMock.user.update.mockResolvedValue({});

      await deleteStudent(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });

    test('rejects other school', async () => {
      const req = mockReq({}, { id: 'u2' }, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.user.findUnique.mockResolvedValue({ id: 'u2', schoolId: 's2', role: 'STUDENT' });

      await deleteStudent(req, res, next);

      expect(res.status).toHaveBeenCalledWith(403);
    });

    test('404 for non-existent', async () => {
      const req = mockReq({}, { id: 'u999' }, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.user.findUnique.mockResolvedValue(null);

      await deleteStudent(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });

  describe('getTeacherStudents', () => {
    test('returns students in teacher classrooms', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.classroom.findMany.mockResolvedValue([{ id: 'cl1', name: 'Class 1' }]);
      prismaMock.user.findMany.mockResolvedValue([{ id: 'u2', name: 'Ahmed', studentProfile: { xp: 100 } }]);

      await getTeacherStudents(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });

    test('filters by classroomId', async () => {
      const req = mockReq({ classroomId: 'cl1' }, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prismaMock.classroom.findFirst.mockResolvedValue({ id: 'cl1', teacherId: 'u1' });
      prismaMock.user.findMany.mockResolvedValue([{ id: 'u2', name: 'Ahmed', studentProfile: { xp: 100 } }]);

      await getTeacherStudents(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });
  });
});
