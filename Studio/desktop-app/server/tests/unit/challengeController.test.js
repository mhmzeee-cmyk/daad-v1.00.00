jest.mock('../../src/middlewares/strictSecurity', () => ({
  isAccountLocked: jest.fn().mockReturnValue(false),
  strictSecurity: [],
  strictSanitize: (v) => v,
}));

jest.mock('../../src/utils/logger', () => ({
  logger: { info: jest.fn(), warn: jest.fn(), error: jest.fn(), debug: jest.fn() },
  securityLogger: { info: jest.fn(), warn: jest.fn(), error: jest.fn() },
}));

jest.mock('../../src/utils/prisma', () => ({
  challenge: {
    findMany: jest.fn(),
    findFirst: jest.fn(),
    findUnique: jest.fn(),
    create: jest.fn(),
    update: jest.fn(),
    updateMany: jest.fn(),
    delete: jest.fn(),
    count: jest.fn(),
  },
  submission: {
    findMany: jest.fn(),
    findFirst: jest.fn(),
    create: jest.fn(),
    count: jest.fn(),
  },
  challengeVerification: { create: jest.fn() },
}));

const { isAccountLocked } = require('../../src/middlewares/strictSecurity');
const prisma = require('../../src/utils/prisma');
const {
  listChallenges,
  dailyChallenge,
  createChallenge,
  updateChallenge,
  deleteChallenge,
  publishChallenge,
  submitSolution,
  listSubmissions,
} = require('../../src/controllers/challengeController');

function mockReq(body, params, user, query) {
  return {
    body: body || {},
    params: params || {},
    query: query || body || {},
    ip: '127.0.0.1',
    cookies: {},
    headers: {},
    path: '/challenges',
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

describe('challengeController', () => {
  let next;

  beforeEach(() => {
    next = jest.fn();
    jest.clearAllMocks();
  });

  describe('listChallenges', () => {
    test('returns challenges for admin', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findMany.mockResolvedValue([{ id: 'c1', title: 'Test' }]);

      await listChallenges(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ challenges: expect.any(Array) }));
    });

    test('filters by tier', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1' }, { tier: '2' });
      const res = mockRes();
      prisma.challenge.findMany.mockResolvedValue([]);

      await listChallenges(req, res, next);

      const callArgs = prisma.challenge.findMany.mock.calls[0][0];
      expect(callArgs.where).toHaveProperty('tier', 2);
    });

    test('handles db error', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findMany.mockRejectedValue(new Error('DB'));

      await listChallenges(req, res, next);

      expect(next).toHaveBeenCalledWith(expect.any(Error));
    });
  });

  describe('dailyChallenge', () => {
    test('returns today daily', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      const today = new Date().toISOString().split('T')[0];
      prisma.challenge.findFirst.mockResolvedValue({ id: 'c1', dailyDate: today });

      await dailyChallenge(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ challenge: expect.any(Object) }));
    });

    test('returns 404 when none', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findFirst.mockResolvedValue(null);

      await dailyChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });

  describe('createChallenge', () => {
    test('creates with valid data', async () => {
      const req = mockReq({ title: 'New', description: 'Desc', difficulty: 'BEGINNER', tier: 1 }, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.create.mockResolvedValue({ id: 'c1', title: 'New' });

      await createChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(201);
    });

    test('rejects missing title', async () => {
      const req = mockReq({ description: 'No title' }, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();

      await createChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects title > 200', async () => {
      const req = mockReq({ title: 'x'.repeat(201) }, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();

      await createChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(400);
    });
  });

  describe('updateChallenge', () => {
    test('updates with valid ownership', async () => {
      const req = mockReq({ title: 'Upd' }, { id: 'c1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', schoolId: 's1' });
      prisma.challenge.update.mockResolvedValue({ id: 'c1' });

      await updateChallenge(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ challenge: expect.any(Object) }));
    });

    test('rejects schoolId mismatch', async () => {
      const req = mockReq({ title: 'Upd' }, { id: 'c1' }, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', schoolId: 's2' });

      await updateChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(403);
    });

    test('404 for non-existent', async () => {
      const req = mockReq({ title: 'Upd' }, { id: 'c999' }, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue(null);

      await updateChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });
  });

  describe('deleteChallenge', () => {
    test('deletes as admin', async () => {
      const req = mockReq({}, { id: 'c1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', schoolId: 's1', isSystem: false });
      prisma.challenge.delete.mockResolvedValue({});

      await deleteChallenge(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ success: true }));
    });

    test('prevents non-admin deleting system', async () => {
      const req = mockReq({}, { id: 'c1' }, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', schoolId: null });

      await deleteChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(403);
    });
  });

  describe('publishChallenge', () => {
    test('toggles publish', async () => {
      const req = mockReq({}, { id: 'c1' }, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', schoolId: null, published: false });
      prisma.challenge.update.mockResolvedValue({ id: 'c1', published: true });

      await publishChallenge(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ success: true }));
    });

    test('rejects other school', async () => {
      const req = mockReq({}, { id: 'c1' }, { id: 'u1', role: 'TEACHER', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', schoolId: 's2' });

      await publishChallenge(req, res, next);

      expect(res.status).toHaveBeenCalledWith(403);
    });
  });

  describe('submitSolution', () => {
    test('submits successfully', async () => {
      const req = mockReq({ challengeId: 'c1', code: 'print("hi")', output: 'hi' }, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', expectedOutput: 'hi', published: true, tier: 1, points: 100, schoolId: null });
      prisma.submission.findFirst.mockResolvedValue(null);
      prisma.submission.create.mockResolvedValue({ id: 'sub1', passed: true, createdAt: new Date() });
      prisma.challenge.update.mockResolvedValue({});
      isAccountLocked.mockReturnValue(false);

      await submitSolution(req, res, next);

      expect(res.status).toHaveBeenCalledWith(201);
    });

    test('404 for non-existent challenge', async () => {
      const req = mockReq({ challengeId: 'c999', code: 'x' }, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue(null);

      await submitSolution(req, res, next);

      expect(res.status).toHaveBeenCalledWith(404);
    });

    test('rejects unpublished', async () => {
      const req = mockReq({ challengeId: 'c1', code: 'x', output: 'wrong' }, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.challenge.findUnique.mockResolvedValue({ id: 'c1', expectedOutput: 'correct', published: true, schoolId: null });
      prisma.submission.findFirst.mockResolvedValue(null);
      prisma.submission.create.mockResolvedValue({ id: 'sub1', passed: false, createdAt: new Date() });
      isAccountLocked.mockReturnValue(false);

      await submitSolution(req, res, next);

      expect(res.status).toHaveBeenCalledWith(201);
    });
  });

  describe('listSubmissions', () => {
    test('returns submissions for student', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'STUDENT', schoolId: 's1' });
      const res = mockRes();
      prisma.submission.findMany.mockResolvedValue([{ id: 's1', studentId: 'u1' }]);

      await listSubmissions(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ submissions: expect.any(Array) }));
    });

    test('returns all for admin', async () => {
      const req = mockReq({}, {}, { id: 'u1', role: 'ADMIN', schoolId: 's1' });
      const res = mockRes();
      prisma.submission.findMany.mockResolvedValue([{ id: 's1' }, { id: 's2' }]);

      await listSubmissions(req, res, next);

      expect(res.json).toHaveBeenCalledWith(expect.objectContaining({ submissions: expect.any(Array) }));
    });
  });
});
