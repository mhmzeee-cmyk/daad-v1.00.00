// ====================================================
// Student-only system + Refresh rotation (R1 + C1)
// ====================================================
const { csrfRequest, prisma } = require('../helpers/app');
const authService = require('../../src/services/authService');

describe('Student-only system (R1 + C1)', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });

  // ── R1: login rejects non-STUDENT roles with a generic 401 ──
  test('R1: login rejects TEACHER role with generic 401', async () => {
    prisma.user.findFirst.mockResolvedValue({
      id: 'teacher-1', email: 't@test.com', name: 'T',
      passwordHash: '$2b$12$hashedpassword', role: 'TEACHER', tokenVersion: 0,
      school: { id: 'school-1', name: 'Test School' },
    });
    authService.comparePassword = jest.fn().mockResolvedValue(true);

    const res = await csrfRequest('post', '/api/v1/auth/login', { username: 't@test.com', password: 'correctpass' });
    expect(res.status).toBe(401);
    expect(res.body.message).toBe('بيانات الدخول غير صحيحة');
  });

  test('R1: login rejects ADMIN role with generic 401', async () => {
    prisma.user.findFirst.mockResolvedValue({
      id: 'admin-1', email: 'a@test.com', name: 'A',
      passwordHash: '$2b$12$hashedpassword', role: 'ADMIN', tokenVersion: 0,
      school: { id: 'school-1', name: 'Test School' },
    });
    authService.comparePassword = jest.fn().mockResolvedValue(true);

    const res = await csrfRequest('post', '/api/v1/auth/login', { username: 'a@test.com', password: 'correctpass' });
    expect(res.status).toBe(401);
    expect(res.body.message).toBe('بيانات الدخول غير صحيحة');
  });

  // ── R1: register forces STUDENT even when TEACHER is sent ──
  test('R1: register forces STUDENT role', async () => {
    prisma.school.findFirst.mockResolvedValue({ id: 'school-1', name: 'Test School' });
    prisma.user.findFirst.mockResolvedValue(null);
    prisma.user.create.mockImplementation(({ data }) => Promise.resolve({
      id: 'new-user-id', ...data, school: { id: 'school-1', name: 'Test School' },
    }));

    const res = await csrfRequest('post', '/api/v1/auth/register', {
      name: 'New Student', email: 'new@test.com', role: 'TEACHER', password: 'Str0ng!Seed9x',
    });
    expect(res.status).toBe(201);
    expect(prisma.user.create).toHaveBeenCalled();
    const created = prisma.user.create.mock.calls[0][0].data;
    expect(created.role).toBe('STUDENT');
    expect(res.body.user.role).toBe('STUDENT');
  });

  // ── C1: refresh rotation works and old token is rejected ──
  test('C1: refresh rotates pair, sets cookies, rejects reuse', async () => {
    const user = {
      id: 'user-1', name: 'Test User', email: 'test@test.com', role: 'STUDENT',
      schoolId: 'school-1', classroomId: null, isActive: true, tokenVersion: 0,
    };
    prisma.user.findUnique.mockResolvedValue({ ...user });
    prisma.user.update.mockImplementation(({ where, data }) => Promise.resolve({ ...where, ...data }));

    const { refreshToken } = authService.generateTokens(user);

    // First use → 200 + fresh cookies
    const res1 = await csrfRequest('post', '/api/v1/auth/refresh', { refreshToken });
    expect(res1.status).toBe(200);
    expect(res1.body).toEqual(expect.objectContaining({ success: true, expiresIn: 3600 }));
    const cookies = res1.headers['set-cookie'] || [];
    expect(cookies.find((c) => c.includes('access_token'))).toBeDefined();
    expect(cookies.find((c) => c.includes('refresh_token'))).toBeDefined();

    // Reuse of the rotated-out token → 401 (blacklisted)
    const res2 = await csrfRequest('post', '/api/v1/auth/refresh', { refreshToken });
    expect(res2.status).toBe(401);
  });

  test('C1: refresh rejects missing and tampered tokens', async () => {
    const resMissing = await csrfRequest('post', '/api/v1/auth/refresh', {});
    expect(resMissing.status).toBe(400);

    const resBad = await csrfRequest('post', '/api/v1/auth/refresh', { refreshToken: 'not-a-token' });
    expect(resBad.status).toBe(401);
  });
});
