const { createTestClient } = require('../helpers/app');

describe('Security Middleware (Integration)', () => {
  let client;

  beforeAll(() => {
    client = createTestClient();
  });

  describe('Security Headers', () => {
    test('sets X-Frame-Options', async () => {
      const res = await client.get('/health');
      expect(res.headers['x-frame-options']).toBeDefined();
    });

    test('sets X-Content-Type-Options', async () => {
      const res = await client.get('/health');
      expect(res.headers['x-content-type-options']).toBe('nosniff');
    });

    test('sets X-XSS-Protection', async () => {
      const res = await client.get('/health');
      expect(res.headers['x-xss-protection']).toBeDefined();
    });

    test('sets Strict-Transport-Security', async () => {
      const res = await client.get('/health');
      expect(res.headers['strict-transport-security']).toBeDefined();
    });

    test('does not expose X-Powered-By', async () => {
      const res = await client.get('/health');
      expect(res.headers['x-powered-by']).toBeUndefined();
    });
  });

  describe('Request Validation', () => {
    test('rejects URL longer than 2048 chars', async () => {
      const longUrl = '/health?' + 'a'.repeat(2100);
      const res = await client.get(longUrl);
      expect(res.status).toBe(414);
    });

    test('rejects path traversal attempts', async () => {
      const res = await client.get('/health/..%2f..%2f..%2fetc/passwd');
      expect(res.status).toBe(400);
    });

    test('allows normal requests', async () => {
      const res = await client.get('/health/live');
      expect(res.status).toBe(200);
    });
  });

  describe('Content-Type Handling', () => {
    test('accepts application/json', async () => {
      const request = require('supertest');
      const agent = request.agent(require('../../src/index'));
      const getRes = await agent.get('/health');
      const cookies = getRes.headers['set-cookie'] || [];
      const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
      const csrfToken = csrfCookie ? csrfCookie.split(';')[0].split('=')[1] : '';
      const allCookies = cookies.map(c => c.split(';')[0]).join('; ');

      const res = await agent
        .post('/api/v1/auth/login')
        .set('Cookie', allCookies)
        .set('X-CSRF-Token', csrfToken)
        .set('Content-Type', 'application/json')
        .send(JSON.stringify({ username: 'test', password: 'pass' }));
      // Should not fail on content-type (may fail for other reasons)
      expect(res.status).not.toBe(415);
    });
  });

  describe('Protected API Routes', () => {
    test('returns 401 for unauthenticated /api/v1/ routes', async () => {
      const res = await client.get('/api/v1/student/profile');
      expect(res.status).toBe(401);
      expect(res.body).toHaveProperty('error');
    });

    test('returns 401 for unauthenticated POST to /api/v1/ routes', async () => {
      const request = require('supertest');
      const agent = request.agent(require('../../src/index'));
      const getRes = await agent.get('/health');
      const cookies = getRes.headers['set-cookie'] || [];
      const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
      const csrfToken = csrfCookie ? csrfCookie.split(';')[0].split('=')[1] : '';
      const allCookies = cookies.map(c => c.split(';')[0]).join('; ');

      const res = await agent
        .post('/api/v1/auth/logout')
        .set('Cookie', allCookies)
        .set('X-CSRF-Token', csrfToken)
        .send({});
      expect(res.status).toBe(401);
    });
  });
});
