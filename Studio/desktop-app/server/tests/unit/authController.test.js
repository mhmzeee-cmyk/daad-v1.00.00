jest.mock('../../src/middlewares/strictSecurity', () => ({
  isAccountLocked: jest.fn().mockReturnValue(false),
  recordFailedAttempt: jest.fn(),
  clearFailedAttempts: jest.fn(),
  logAudit: jest.fn(),
  strictRateLimit: jest.fn((req, res, next) => next()),
  validatePasswordStrength: jest.fn().mockReturnValue([]),
}));

jest.mock('../../src/utils/logger', () => ({
  logger: { info: jest.fn(), warn: jest.fn(), error: jest.fn(), debug: jest.fn() },
}));

jest.mock('../../src/services/authService', () => ({
  validatePassword: jest.fn().mockReturnValue([]),
  hashPassword: jest.fn().mockResolvedValue('$2b$12$hashedpassword'),
  comparePassword: jest.fn().mockResolvedValue(true),
  generateTokens: jest.fn().mockReturnValue({ accessToken: 'access-token', refreshToken: 'refresh-token' }),
  verifyAccessToken: jest.fn().mockReturnValue({ id: 'u1', role: 'STUDENT' }),
  verifyRefreshToken: jest.fn().mockReturnValue({ id: 'u1', type: 'refresh' }),
  isTokenBlacklisted: jest.fn().mockReturnValue(false),
  blacklistToken: jest.fn(),
  validateEmail: jest.fn().mockReturnValue(true),
  sanitizeInput: jest.fn((s) => s),
  hashToken: jest.fn().mockReturnValue('hashed-token'),
}));

const { login, register, logout, refreshToken, changePassword } = require('../../src/controllers/authController');

function mockReq(body, cookies) {
  return {
    body: body || {},
    params: {},
    ip: '127.0.0.1',
    cookies: cookies || {},
    headers: {},
    path: '/auth/login',
    method: 'POST',
    app: { get: jest.fn().mockReturnValue(null) },
    user: null,
  };
}

function mockRes() {
  const res = { statusCode: null, body: null, clearedCookies: [], cookieSet: [] };
  res.status = jest.fn().mockReturnValue(res);
  res.json = jest.fn().mockReturnValue(res);
  res.cookie = jest.fn().mockImplementation((name, val, opts) => {
    res.cookieSet.push({ name, val, opts });
    return res;
  });
  res.clearCookie = jest.fn().mockImplementation((name, opts) => {
    res.clearedCookies.push(name);
    return res;
  });
  return res;
}

describe('authController', () => {
  let next;

  beforeEach(() => {
    next = jest.fn();
    jest.clearAllMocks();
  });

  describe('login', () => {
    test('rejects non-string username', async () => {
      const req = mockReq({ username: 123, password: 'pass' });
      const res = mockRes();
      await login(req, res, next);
      expect(res.status).toHaveBeenCalledWith(400);
      expect(res.json).toHaveBeenCalledWith(
        expect.objectContaining({ error: 'خطأ في الطلب' })
      );
    });

    test('rejects empty username', async () => {
      const req = mockReq({ username: '', password: 'pass' });
      const res = mockRes();
      await login(req, res, next);
      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects empty password', async () => {
      const req = mockReq({ username: 'admin', password: '' });
      const res = mockRes();
      await login(req, res, next);
      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects username longer than 254 chars', async () => {
      const req = mockReq({ username: 'a'.repeat(255), password: 'pass' });
      const res = mockRes();
      await login(req, res, next);
      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects password longer than 128 chars', async () => {
      const req = mockReq({ username: 'admin', password: 'a'.repeat(129) });
      const res = mockRes();
      await login(req, res, next);
      expect(res.status).toHaveBeenCalledWith(400);
    });
  });

  describe('register', () => {
    test('rejects non-string email', async () => {
      const req = mockReq({ email: 123, password: 'StrongPass1', name: 'أحمد' });
      const res = mockRes();
      await register(req, res, next);
      expect(res.status).toHaveBeenCalledWith(400);
    });

    test('rejects missing name', async () => {
      const req = mockReq({ email: 'test@test.com', password: 'StrongPass1' });
      const res = mockRes();
      await register(req, res, next);
      expect(res.status).toHaveBeenCalledWith(400);
    });
  });

  describe('logout', () => {
    test('clears tokens and returns success', async () => {
      const req = mockReq({}, { access_token: 'at', refresh_token: 'rt' });
      req.user = { id: 'u1' };
      const res = mockRes();
      await logout(req, res, next);
      expect(res.json).toHaveBeenCalled();
    });
  });

  describe('refreshToken', () => {
    test('rejects missing refresh token', async () => {
      const req = mockReq({}, {});
      const res = mockRes();
      await refreshToken(req, res, next);
      expect(res.status).toHaveBeenCalled();
    });
  });
});
