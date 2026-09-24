const crypto = require('crypto');

// Set JWT_SECRET before requiring csrf module so CSRF_SECRET is derived from it
process.env.JWT_SECRET = 'test-csrf-secret-key-2026';

const { csrfProtection, CSRF_TOKEN_NAME } = require('../../src/middlewares/csrf');

function generateTestCsrfToken() {
  // Use HMAC-derived CSRF_SECRET matching the middleware's derivation
  const csrfSecret = crypto.createHmac('sha256', process.env.JWT_SECRET || 'fallback-csrf').update('csrf-dhad-v1').digest('hex');
  const random = crypto.randomBytes(32).toString('hex');
  const signature = crypto.createHmac('sha256', csrfSecret).update(random).digest('hex');
  return random + '.' + signature;
}

function mockReq(method, cookies, headers) {
  return { method: method || 'GET', cookies: cookies || {}, headers: headers || {}, path: '/test' };
}

function mockRes() {
  const res = { statusCode: null, body: null, cookieName: null, cookieValue: null, cookieOpts: null };
  res.status = jest.fn().mockReturnValue(res);
  res.json = jest.fn().mockReturnValue(res);
  res.cookie = jest.fn().mockImplementation((name, value, opts) => {
    res.cookieName = name;
    res.cookieValue = value;
    res.cookieOpts = opts;
    return res;
  });
  return res;
}

describe('csrfProtection', () => {
  let next;

  beforeEach(() => { next = jest.fn(); });

  describe('GET requests (safe methods)', () => {
    test('sets CSRF cookie on GET to /api/v1/csrf-token', () => {
      const req = mockReq('GET');
      req.path = '/api/v1/csrf-token';
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.cookie).toHaveBeenCalled();
      expect(res.cookieName).toBe(CSRF_TOKEN_NAME);
    });

    test('issues cookie on regular GET when no valid token exists', () => {
      const req = mockReq('GET');
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(next).toHaveBeenCalled();
      expect(res.cookie).toHaveBeenCalled();
      expect(res.cookieName).toBe(CSRF_TOKEN_NAME);
    });

    test('does not re-issue cookie on regular GET with valid token', () => {
      // Build a valid signed token using HMAC-derived CSRF_SECRET (matching the middleware)
      const csrfSecret = crypto.createHmac('sha256', process.env.JWT_SECRET || 'fallback-csrf').update('csrf-dhad-v1').digest('hex');
      const random = crypto.randomBytes(32).toString('hex');
      const signature = crypto.createHmac('sha256', csrfSecret).update(random).digest('hex');
      const validToken = `${random}.${signature}`;

      const req = mockReq('GET', { [CSRF_TOKEN_NAME]: validToken });
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(next).toHaveBeenCalled();
      expect(res.cookie).not.toHaveBeenCalled();
    });

    test('always refreshes CSRF cookie on /api/v1/csrf-token endpoint', () => {
      const req = mockReq('GET', { [CSRF_TOKEN_NAME]: 'existing-token' });
      req.path = '/api/v1/csrf-token';
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.cookie).toHaveBeenCalled();
    });

    test('skips for HEAD requests', () => {
      const req = mockReq('HEAD');
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(next).toHaveBeenCalled();
    });

    test('skips for OPTIONS requests', () => {
      const req = mockReq('OPTIONS');
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(next).toHaveBeenCalled();
    });
  });

  describe('POST requests (state-changing)', () => {
    test('rejects POST without CSRF token', () => {
      const req = mockReq('POST');
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.status).toHaveBeenCalledWith(403);
      expect(res.json).toHaveBeenCalledWith(
        expect.objectContaining({ error: 'CSRF_TOKEN_MISSING' })
      );
      expect(next).not.toHaveBeenCalled();
    });

    test('rejects POST with mismatched tokens', () => {
      const req = mockReq('POST', { [CSRF_TOKEN_NAME]: 'token-a' }, { 'x-csrf-token': 'token-b' });
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.status).toHaveBeenCalledWith(403);
      expect(res.json).toHaveBeenCalledWith(
        expect.objectContaining({ error: 'CSRF_TOKEN_INVALID' })
      );
    });

    test('accepts POST with matching valid tokens', () => {
      const token = generateTestCsrfToken();
      const req = mockReq('POST', { [CSRF_TOKEN_NAME]: token }, { 'x-csrf-token': token });
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(next).toHaveBeenCalled();
    });

    test('rejects POST with only cookie token (no header)', () => {
      const req = mockReq('POST', { [CSRF_TOKEN_NAME]: 'some-token' });
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.status).toHaveBeenCalledWith(403);
    });

    test('rejects POST with only header token (no cookie)', () => {
      const req = mockReq('POST', {}, { 'x-csrf-token': 'some-token' });
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.status).toHaveBeenCalledWith(403);
    });

    test('rejects POST with invalid token format', () => {
      const req = mockReq('POST', { [CSRF_TOKEN_NAME]: 'no-dot' }, { 'x-csrf-token': 'no-dot' });
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.status).toHaveBeenCalledWith(403);
    });
  });

  describe('PUT/DELETE requests', () => {
    test('rejects PUT without CSRF token', () => {
      const req = mockReq('PUT');
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.status).toHaveBeenCalledWith(403);
    });

    test('rejects DELETE without CSRF token', () => {
      const req = mockReq('DELETE');
      const res = mockRes();
      csrfProtection(req, res, next);
      expect(res.status).toHaveBeenCalledWith(403);
    });
  });
});

describe('CSRF_TOKEN_NAME', () => {
  test('is defined and non-empty', () => {
    expect(CSRF_TOKEN_NAME).toBeDefined();
    expect(CSRF_TOKEN_NAME.length).toBeGreaterThan(0);
  });
});
