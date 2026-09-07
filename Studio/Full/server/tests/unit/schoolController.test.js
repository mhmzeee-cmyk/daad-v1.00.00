jest.mock('../../src/middlewares/strictSecurity', () => ({
  strictSecurity: [],
  strictSanitize: (v) => v,
}));

jest.mock('../../src/utils/logger', () => ({
  logger: { info: jest.fn(), warn: jest.fn(), error: jest.fn(), debug: jest.fn() },
}));

const prismaMock = {
  school: { findMany: jest.fn(), findUnique: jest.fn(), update: jest.fn(), delete: jest.fn(), count: jest.fn() },
  user: { findUnique: jest.fn(), delete: jest.fn() },
};

const {
  getSchools,
  getSchoolById,
  updateSchool,
  deleteSchool,
  deleteUser,
} = require('../../src/controllers/schoolController');

function mockReq(body, params, user) {
  return {
    body: body || {},
    params: params || {},
    query: {},
    ip: '127.0.0.1',
    cookies: {},
    headers: {},
    path: '/schools',
    method: 'GET',
    app: { get: jest.fn().mockReturnValue(prismaMock) },
    user: user || null,
  };
}

function mockRes() {
  const res = { statusCode: null, body: null };
  res.status = jest.fn().mockReturnValue(res);
  res.json = jest.fn().mockReturnValue(res);
  return res;
}

describe('schoolController', () => {
  let next;

  beforeEach(() => {
    next = jest.fn();
    jest.clearAllMocks();
  });

  describe('getSchools', () => {
    test('returns all schools for admin', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findMany.mockResolvedValue([{ id: 's1', name: 'School', _count: { users: 10, courses: 5 } }]);

      await getSchools(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ schools: expect.any(Array) }));
    });

    test('returns only own school for non-admin', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'TEACHER', schoolId: 's1', name: 'Teacher' });
      const res = mockRes();
      prismaMock.school.findMany.mockResolvedValue([{ id: 's1', name: 'My School', _count: { users: 5, courses: 2 } }]);

      await getSchools(req, res, next);

      const callArgs = prismaMock.school.findMany.mock.calls[0][0];
      expect(callArgs.where).toHaveProperty('id', 's1');
    });

    test('handles db error', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findMany.mockRejectedValue(new Error('DB'));

      await getSchools(req, res, next);

      expect(next).toHaveBeenCalledWith(expect.any(Error));
    });
  });

  describe('getSchoolById', () => {
    test('returns school details for admin', async () => {
      const req = mockReq({}, { id: 's1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findUnique.mockResolvedValue({
        id: 's1', name: 'School',
        users: [{ role: 'TEACHER', name: 'T1' }, { role: 'STUDENT', name: 'S1' }],
        _count: { users: 10 },
      });

      await getSchoolById(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });

    test('returns counts for non-admin', async () => {
      const req = mockReq({}, { id: 's1' }, { id: 'u1', role: 'TEACHER', schoolId: 's1', name: 'Teacher' });
      const res = mockRes();
      prismaMock.school.findUnique.mockResolvedValue({ id: 's1', _count: { users: 10 } });

      await getSchoolById(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });

    test('404 for non-existent', async () => {
      const req = mockReq({}, { id: 's999' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findUnique.mockResolvedValue(null);

      await getSchoolById(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });

  describe('updateSchool', () => {
    test('updates school name', async () => {
      const req = mockReq({ name: 'New Name' }, { id: 's1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findUnique.mockResolvedValue({ id: 's1', name: 'Old' });
      prismaMock.school.update.mockResolvedValue({ id: 's1', name: 'New Name', provisionedAt: new Date() });

      await updateSchool(req, res, next);

      expect(res.json).toHaveBeenCalled();
      expect(prismaMock.school.update).toHaveBeenCalled();
    });

    test('rejects empty name', async () => {
      const req = mockReq({ name: '' }, { id: 's1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();

      await updateSchool(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects name > 200', async () => {
      const req = mockReq({ name: 'x'.repeat(201) }, { id: 's1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();

      await updateSchool(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('404 for non-existent school', async () => {
      const req = mockReq({ name: 'New' }, { id: 's999' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findUnique.mockResolvedValue(null);

      await updateSchool(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });

  describe('deleteSchool', () => {
    test('deletes school', async () => {
      const req = mockReq({}, { id: 's1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findUnique.mockResolvedValue({ id: 's1', name: 'School', _count: { users: 10, courses: 5 } });
      prismaMock.school.count.mockResolvedValue(5);
      prismaMock.school.delete.mockResolvedValue({});

      await deleteSchool(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });

    test('404 for non-existent', async () => {
      const req = mockReq({}, { id: 's999' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.school.findUnique.mockResolvedValue(null);

      await deleteSchool(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });

  describe('deleteUser', () => {
    test('deletes user', async () => {
      const req = mockReq({}, { id: 's1', userId: 'u2' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.user.findUnique.mockResolvedValue({ id: 'u2', schoolId: 's1', role: 'STUDENT', name: 'Student', school: { name: 'School' } });
      prismaMock.user.delete.mockResolvedValue({});

      await deleteUser(req, res, next);

      expect(res.json).toHaveBeenCalled();
    });

    test('prevents self-deletion', async () => {
      const req = mockReq({}, { id: 's1', userId: 'u1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.user.findUnique.mockResolvedValue({ id: 'u1', schoolId: 's1', name: 'Admin', school: { name: 'School' } });

      await deleteUser(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('404 for non-existent user', async () => {
      const req = mockReq({}, { id: 's1', userId: 'u999' }, { id: 'u1', role: 'ADMIN', schoolId: 's1', name: 'Admin' });
      const res = mockRes();
      prismaMock.user.findUnique.mockResolvedValue(null);

      await deleteUser(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });
});
