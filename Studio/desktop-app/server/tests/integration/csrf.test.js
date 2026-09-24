const { createTestClient } = require('../helpers/app');

describe('CSRF Protection (Integration)', () => {
  let client;

  beforeAll(() => {
    client = createTestClient();
  });

  describe('GET requests (safe methods)', () => {
    test('GET /health sets CSRF cookie', async () => {
      const res = await client.get('/health');
      expect([200, 503]).toContain(res.status);
      const cookies = res.headers['set-cookie'] || [];
      const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
      expect(csrfCookie).toBeDefined();
      expect(csrfCookie).toContain('SameSite=Lax');
    });

    test('GET does not require CSRF token', async () => {
      const res = await client.get('/health/live');
      expect(res.status).toBe(200);
    });
  });

  describe('POST requests (state-changing)', () => {
    test('POST without CSRF token returns 403', async () => {
      const res = await client
        .post('/api/v1/workspace/save')
        .send({ challengeId: 'ch1', code: 'print("hello")' });
      expect(res.status).toBe(403);
      expect(res.body.error).toContain('CSRF');
    });

    test('POST with mismatched CSRF tokens returns 403', async () => {
      const res = await client
        .post('/api/v1/workspace/save')
        .set('Cookie', '_csrf_dhad=token-a')
        .set('X-CSRF-Token', 'token-b')
        .send({ challengeId: 'ch1', code: 'print("hello")' });
      expect(res.status).toBe(403);
    });

    test('POST with matching CSRF tokens passes CSRF check', async () => {
      // First get a valid CSRF token via GET
      const getRes = await client.get('/health');
      const cookies = getRes.headers['set-cookie'] || [];
      const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
      expect(csrfCookie).toBeDefined();

      const csrfToken = csrfCookie.split(';')[0].split('=')[1];

      // Now POST with matching token - should get past CSRF (may fail for other reasons)
      const res = await client
        .post('/api/v1/auth/login')
        .set('Cookie', `_csrf_dhad=${csrfToken}`)
        .set('X-CSRF-Token', csrfToken)
        .send({ username: 'test', password: 'pass' });

      // Should NOT be 403 CSRF error
      expect(res.status).not.toBe(403);
    });
  });

  describe('PUT/DELETE requests', () => {
    test('PUT without CSRF token returns 403', async () => {
      const res = await client
        .put('/api/v1/auth/change-password')
        .send({ oldPassword: 'old', newPassword: 'new' });
      expect(res.status).toBe(403);
    });

    test('DELETE without CSRF token returns 403', async () => {
      const res = await client
        .delete('/api/v1/teacher/students/fake-id');
      expect(res.status).toBe(403);
    });
  });
});
