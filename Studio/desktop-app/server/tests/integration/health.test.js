const { createTestClient } = require('../helpers/app');

describe('Health Endpoints (Integration)', () => {
  let client;

  beforeAll(() => {
    client = createTestClient();
  });

  describe('GET /health', () => {
    test('returns 200 or 503 with health status', async () => {
      const res = await client.get('/health');
      expect([200, 503]).toContain(res.status);
      expect(res.body).toHaveProperty('status');
      expect(res.body).toHaveProperty('timestamp');
    });

    test('returns basic health info', async () => {
      const res = await client.get('/health');
      expect(res.body).toHaveProperty('status');
      expect(res.body).toHaveProperty('uptime');
    });
  });

  describe('GET /health/ready', () => {
    test('returns a readiness status', async () => {
      const res = await client.get('/health/ready');
      expect([200, 503]).toContain(res.status);
      expect(res.body).toHaveProperty('status');
    });
  });

  describe('GET /health/live', () => {
    test('returns 200 alive', async () => {
      const res = await client.get('/health/live');
      expect(res.status).toBe(200);
      expect(res.body).toEqual({ status: 'alive' });
    });
  });
});
