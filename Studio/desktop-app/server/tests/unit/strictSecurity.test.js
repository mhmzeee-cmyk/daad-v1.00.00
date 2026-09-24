jest.mock('fs', () => ({
  existsSync: jest.fn().mockReturnValue(false),
  readFileSync: jest.fn().mockReturnValue('{}'),
  writeFileSync: jest.fn(),
}));

jest.mock('../../src/utils/redis', () => ({
  getRedisClient: jest.fn().mockReturnValue(null),
  isRedisAvailable: jest.fn().mockReturnValue(false),
}));

const {
  CONFIG,
  recordFailedAttempt,
  clearFailedAttempts,
  isAccountLocked,
  recordIPFailure,
  isIPBlocked,
  strictRequestValidation,
  strictSanitize,
  validatePasswordStrength,
  logAudit,
  getAuditLog,
  strictSecurityHeaders,
} = require('../../src/middlewares/strictSecurity');

function mockReq(overrides = {}) {
  return {
    method: 'POST',
    path: '/test',
    originalUrl: '/test',
    ip: '127.0.0.1',
    body: {},
    query: {},
    headers: {},
    cookies: {},
    user: null,
    ...overrides,
  };
}

function mockRes() {
  const res = { statusCode: null, body: null, headersSet: {} };
  res.status = jest.fn().mockReturnValue(res);
  res.json = jest.fn().mockReturnValue(res);
  res.setHeader = jest.fn().mockImplementation((key, val) => {
    res.headersSet[key] = val;
    return res;
  });
  res.removeHeader = jest.fn().mockImplementation((key) => {
    delete res.headersSet[key];
    return res;
  });
  return res;
}

describe('strictSecurity', () => {
  let next;

  beforeEach(() => {
    next = jest.fn();
    jest.clearAllMocks();
  });

  describe('CONFIG', () => {
    test('has rate limit configuration', () => {
      expect(CONFIG.RATE_LIMIT_WINDOW_MS).toBe(15 * 60 * 1000);
      expect(CONFIG.MAX_REQUESTS_PER_WINDOW).toHaveProperty('auth');
      expect(CONFIG.MAX_REQUESTS_PER_WINDOW).toHaveProperty('public');
    });

    test('has account lockout configuration', () => {
      expect(CONFIG.MAX_FAILED_ATTEMPTS).toBe(5);
      expect(CONFIG.LOCKOUT_DURATION_MS).toBe(15 * 60 * 1000);
    });

    test('has IP blocking configuration', () => {
      expect(CONFIG.MAX_IP_FAILURES).toBe(20);
      expect(CONFIG.IP_BLOCK_DURATION_MS).toBe(60 * 60 * 1000);
    });

    test('has request limits', () => {
      expect(CONFIG.MAX_URL_LENGTH).toBe(2048);
      expect(CONFIG.MAX_BODY_SIZE).toBe(1 * 1024 * 1024);
    });

    test('has password policy', () => {
      expect(CONFIG.MIN_PASSWORD_LENGTH).toBe(8);
      expect(CONFIG.REQUIRE_UPPERCASE).toBe(true);
    });
  });

  describe('recordFailedAttempt & isAccountLocked', () => {
    test('account not locked initially', async () => {
      expect(await isAccountLocked('testuser')).toBe(false);
    });

    test('account locks after max failed attempts', async () => {
      for (let i = 0; i < CONFIG.MAX_FAILED_ATTEMPTS; i++) {
        await recordFailedAttempt('lockuser');
      }
      expect(await isAccountLocked('lockuser')).toBe(true);
    });

    test('clearFailedAttempts unlocks account', async () => {
      for (let i = 0; i < CONFIG.MAX_FAILED_ATTEMPTS; i++) {
        await recordFailedAttempt('clearuser');
      }
      expect(await isAccountLocked('clearuser')).toBe(true);
      await clearFailedAttempts('clearuser');
      expect(await isAccountLocked('clearuser')).toBe(false);
    });

    test('different usernames are independent', async () => {
      for (let i = 0; i < CONFIG.MAX_FAILED_ATTEMPTS; i++) {
        await recordFailedAttempt('user-a');
      }
      expect(await isAccountLocked('user-a')).toBe(true);
      expect(await isAccountLocked('user-b')).toBe(false);
    });
  });

  describe('recordIPFailure & isIPBlocked', () => {
    test('IP not blocked initially', async () => {
      expect(await isIPBlocked('10.0.0.1')).toBe(false);
    });

    test('IP blocks after threshold', async () => {
      for (let i = 0; i < CONFIG.MAX_IP_FAILURES; i++) {
        await recordIPFailure('10.0.0.2');
      }
      expect(await isIPBlocked('10.0.0.2')).toBe(true);
    });

    test('different IPs are independent', async () => {
      for (let i = 0; i < CONFIG.MAX_IP_FAILURES; i++) {
        await recordIPFailure('10.0.0.3');
      }
      expect(await isIPBlocked('10.0.0.3')).toBe(true);
      expect(await isIPBlocked('10.0.0.4')).toBe(false);
    });
  });

  describe('validatePasswordStrength', () => {
    test('accepts strong password', () => {
      const errors = validatePasswordStrength('StrongPass1!');
      expect(errors).toHaveLength(0);
    });

    test('rejects short password', () => {
      const errors = validatePasswordStrength('Ab1');
      expect(errors.length).toBeGreaterThan(0);
    });

    test('rejects password without number', () => {
      const errors = validatePasswordStrength('StrongPass');
      expect(errors.length).toBeGreaterThan(0);
    });
  });

  describe('strictSanitize', () => {
    test('removes null bytes', () => {
      const result = strictSanitize('hello\0world');
      expect(result).not.toContain('\0');
    });

    test('removes control characters', () => {
      const result = strictSanitize('hello\x01\x02world');
      expect(result).not.toContain('\x01');
    });

    test('returns non-string as-is', () => {
      expect(strictSanitize(42)).toBe(42);
    });
  });

  describe('strictRequestValidation', () => {
    test('rejects URLs longer than 2048 chars', () => {
      const req = mockReq({ originalUrl: '/' + 'a'.repeat(2050) });
      const res = mockRes();
      strictRequestValidation(req, res, next);
      expect(res.status).toHaveBeenCalledWith(414);
    });

    test('rejects body larger than 1MB', () => {
      const req = mockReq({ headers: { 'content-length': '2097152' } });
      const res = mockRes();
      strictRequestValidation(req, res, next);
      expect(res.status).toHaveBeenCalledWith(413);
    });

    test('passes valid request', () => {
      const req = mockReq({ originalUrl: '/test', body: { foo: 'bar' } });
      const res = mockRes();
      strictRequestValidation(req, res, next);
      expect(next).toHaveBeenCalled();
    });
  });

  describe('strictSecurityHeaders', () => {
    test('sets security headers', () => {
      const req = mockReq();
      const res = mockRes();
      strictSecurityHeaders(req, res, next);
      expect(res.setHeader).toHaveBeenCalled();
      expect(next).toHaveBeenCalled();
    });
  });

  describe('logAudit & getAuditLog', () => {
    test('logs audit entry', () => {
      logAudit('TEST_EVENT', { detail: 'test' });
      const log = getAuditLog();
      expect(log.length).toBeGreaterThan(0);
    });

    test('audit entry has correct structure', () => {
      logAudit('STRUCTURE_TEST', { key: 'value' });
      const log = getAuditLog();
      const entry = log[log.length - 1];
      expect(entry).toHaveProperty('action');
      expect(entry).toHaveProperty('timestamp');
      expect(entry.action).toBe('STRUCTURE_TEST');
    });

    test('audit entry includes ip and userId', () => {
      logAudit('IP_TEST', { ip: '1.2.3.4', userId: 'user-1' });
      const log = getAuditLog();
      const entry = log[log.length - 1];
      expect(entry.ip).toBe('1.2.3.4');
      expect(entry.userId).toBe('user-1');
    });
  });
});
