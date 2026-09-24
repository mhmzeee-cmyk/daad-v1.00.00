const { csrfRequest, prisma, generateTestJWT, app } = require('../helpers/app');
const authService = require('../../src/services/authService');

describe('Auth Endpoints (Integration)', () => {
  beforeEach(() => {
    jest.clearAllMocks();
    prisma.user.findUnique.mockResolvedValue({
      id: 'user-1', isActive: true, isApproved: true, tokenVersion: 0,
      invitation: { isActivated: true },
    });
  });

  // ── POST /api/v1/auth/login ──────────────────────────────────────────────
  describe('POST /api/v1/auth/login', () => {
    test('rejects non-string username', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/login', { username: 123, password: 'pass' });
      expect(res.status).toBe(400);
      expect(res.body.error).toBeDefined();
    });

    test('rejects empty username', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/login', { username: '', password: 'pass' });
      expect(res.status).toBe(400);
    });

    test('rejects empty password', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/login', { username: 'admin', password: '' });
      expect(res.status).toBe(400);
    });

    test('rejects missing body', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/login', {});
      expect(res.status).toBe(400);
    });

    test('rejects username > 254 chars', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/login', { username: 'a'.repeat(255), password: 'pass' });
      expect(res.status).toBe(400);
    });

    test('returns 401 for non-existent user', async () => {
      prisma.user.findFirst.mockResolvedValue(null);
      const res = await csrfRequest('post', '/api/v1/auth/login', { username: 'nonexistent@test.com', password: 'pass' });
      expect(res.status).toBe(401);
      expect(res.body.error).toBeDefined();
    });

    test('returns 401 for wrong password', async () => {
      prisma.user.findFirst.mockResolvedValue({
        id: 'user-1', email: 'test@test.com', name: 'Test User',
        passwordHash: '$2b$12$hashedpassword', role: 'STUDENT', tokenVersion: 0,
        school: { id: 'school-1', name: 'Test School' },
      });
      authService.comparePassword = jest.fn().mockResolvedValue(false);

      const res = await csrfRequest('post', '/api/v1/auth/login', { username: 'test@test.com', password: 'wrongpass' });
      expect(res.status).toBe(401);
    });

    test('returns 200 with cookies on successful login', async () => {
      prisma.user.findFirst.mockResolvedValue({
        id: 'user-1', email: 'test@test.com', name: 'Test User',
        passwordHash: '$2b$12$hashedpassword', role: 'STUDENT', tokenVersion: 0,
        school: { id: 'school-1', name: 'Test School' },
      });
      authService.comparePassword = jest.fn().mockResolvedValue(true);
      authService.generateTokens = jest.fn().mockReturnValue({
        accessToken: 'test-access-token', refreshToken: 'test-refresh-token',
      });

      const res = await csrfRequest('post', '/api/v1/auth/login', { username: 'test@test.com', password: 'correctpass' });
      expect(res.status).toBe(200);
      expect(res.body).toHaveProperty('userId', 'user-1');
      expect(res.body).toHaveProperty('role', 'STUDENT');
      expect(res.body).toHaveProperty('username', 'Test User');

      // Tokens should be in cookies AND accessToken in body (for Bridge compatibility)
      expect(res.body).toHaveProperty('accessToken');
      expect(res.body).not.toHaveProperty('refreshToken');

      const cookies = res.headers['set-cookie'] || [];
      const accessCookie = cookies.find(c => c.includes('access_token'));
      const refreshCookie = cookies.find(c => c.includes('refresh_token'));
      expect(accessCookie).toBeDefined();
      expect(refreshCookie).toBeDefined();
    });

    test('login with nationalId (student)', async () => {
      prisma.user.findFirst
        .mockResolvedValueOnce(null)
        .mockResolvedValueOnce({
          id: 'student-1', email: null, nationalId: '1234567890',
          name: 'Student User', passwordHash: '$2b$12$hashedpassword',
          role: 'STUDENT', tokenVersion: 0,
          school: { id: 'school-1', name: 'Test School' },
        });
      authService.comparePassword = jest.fn().mockResolvedValue(true);
      authService.generateTokens = jest.fn().mockReturnValue({
        accessToken: 'test-access', refreshToken: 'test-refresh',
      });

      const res = await csrfRequest('post', '/api/v1/auth/login', { username: '1234567890', password: 'pass123' });
      expect(res.status).toBe(200);
      expect(res.body.userId).toBe('student-1');
    });
  });

  // ── POST /api/v1/auth/refresh ───────────────────────────────────────────
  describe('POST /api/v1/auth/refresh', () => {
    test('rejects missing refresh token', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/refresh', {});
      expect(res.status).toBe(400);
      expect(res.body.error).toBeDefined();
    });

    test('rejects invalid refresh token', async () => {
      const request = require('supertest');
      const agent = request.agent(app);
      const getRes = await agent.get('/health');
      const cookies = getRes.headers['set-cookie'] || [];
      const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
      const csrfToken = csrfCookie ? csrfCookie.split(';')[0].split('=')[1] : '';
      const allCookies = [...cookies.map(c => c.split(';')[0]), 'refresh_token=invalid-token-value'].join('; ');

      const res = await agent
        .post('/api/v1/auth/refresh')
        .set('Cookie', allCookies)
        .set('X-CSRF-Token', csrfToken)
        .send({});

      expect([400, 401]).toContain(res.status);
    });
  });

  // ── POST /api/v1/auth/logout (protected) ────────────────────────────────
  describe('POST /api/v1/auth/logout', () => {
    test('rejects logout without auth token', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/logout', {});
      expect(res.status).toBe(401);
    });

    test('performs logout with valid token', async () => {
      const request = require('supertest');
      const agent = request.agent(app);
      const getRes = await agent.get('/health');
      const cookies = getRes.headers['set-cookie'] || [];
      const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
      const csrfToken = csrfCookie ? csrfCookie.split(';')[0].split('=')[1] : '';
      const allCookies = cookies.map(c => c.split(';')[0]).join('; ');
      const token = generateTestJWT({ id: 'user-1', role: 'STUDENT', schoolId: 'school-1' });

      const res = await agent
        .post('/api/v1/auth/logout')
        .set('Cookie', allCookies)
        .set('X-CSRF-Token', csrfToken)
        .set('Authorization', `Bearer ${token}`)
        .send({});

      expect(res.status).toBe(200);
      expect(res.body.success).toBe(true);

      const resCookies = res.headers['set-cookie'] || [];
      const accessCookie = resCookies.find(c => c.includes('access_token'));
      const refreshCookie = resCookies.find(c => c.includes('refresh_token'));
      expect(accessCookie).toBeDefined();
      expect(accessCookie).toMatch(/Max-Age=0|Expires=Thu, 01 Jan 1970/);
      expect(refreshCookie).toBeDefined();
    });
  });

  // ── POST /api/v1/auth/register (protected: teacher/admin only) ──────────
  describe('POST /api/v1/auth/register', () => {
    test('rejects register without auth', async () => {
      const res = await csrfRequest('post', '/api/v1/auth/register', {
        email: 'new@test.com', password: 'Strong1Pass', name: 'New User',
      });
      expect(res.status).toBe(401);
    });

    test('rejects register with student role', async () => {
      const request = require('supertest');
      const agent = request.agent(app);
      const getRes = await agent.get('/health');
      const cookies = getRes.headers['set-cookie'] || [];
      const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
      const csrfToken = csrfCookie ? csrfCookie.split(';')[0].split('=')[1] : '';
      const allCookies = cookies.map(c => c.split(';')[0]).join('; ');
      const token = generateTestJWT({ id: 'user-1', role: 'STUDENT' });

      const res = await agent
        .post('/api/v1/auth/register')
        .set('Cookie', allCookies)
        .set('X-CSRF-Token', csrfToken)
        .set('Authorization', `Bearer ${token}`)
        .send({ email: 'new@test.com', password: 'Strong1Pass', name: 'New User' });

      expect([401, 403]).toContain(res.status);
    });
  });
});


