process.env.JWT_SECRET = 'test-jwt-secret-integration-2026';
process.env.HMAC_SECRET = 'test-hmac-secret-integration-2026';
process.env.NODE_ENV = 'test';
process.env.PORT = '0';
process.env.ALLOWED_ORIGINS = 'http://localhost:3000';

// Mock Prisma before requiring app
jest.mock('../../src/utils/prisma', () => {
  const mockPrisma = {
    user: {
      findUnique: jest.fn().mockResolvedValue({
        id: 'test-user-id',
        isActive: true,
        isApproved: true,
        tokenVersion: 0,
        invitation: { isActivated: true },
      }),
      findFirst: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-id', ...data })),
      update: jest.fn().mockImplementation(({ where, data }) => Promise.resolve({ ...where, ...data })),
      delete: jest.fn().mockResolvedValue({}),
      count: jest.fn().mockResolvedValue(0),
    },
    classroom: {
      findUnique: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-classroom-id', ...data })),
    },
    school: {
      findUnique: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-school-id', ...data })),
    },
    challenge: {
      findUnique: jest.fn().mockResolvedValue(null),
      findFirst: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-challenge-id', ...data })),
    },
    submission: {
      findMany: jest.fn().mockResolvedValue([]),
      findFirst: jest.fn().mockResolvedValue(null),
      count: jest.fn().mockResolvedValue(0),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-submission-id', ...data })),
    },
    achievement: {
      findMany: jest.fn().mockResolvedValue([]),
    },
    userAchievement: {
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-ua-id', ...data })),
    },
    studentProfile: {
      findUnique: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-sp-id', ...data })),
      update: jest.fn().mockImplementation(({ where, data }) => Promise.resolve({ ...where, ...data })),
    },
    studentCourseEnrollment: {
      findFirst: jest.fn().mockResolvedValue(null),
      findUnique: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-sce-id', ...data })),
    },
    course: {
      findUnique: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
    },
    lesson: {
      findMany: jest.fn().mockResolvedValue([]),
    },
    cloudWorkspace: {
      findUnique: jest.fn().mockResolvedValue(null),
      findMany: jest.fn().mockResolvedValue([]),
      upsert: jest.fn().mockImplementation(({ create }) => Promise.resolve({ id: 'mock-ws-id', ...create })),
      deleteMany: jest.fn().mockResolvedValue({ count: 0 }),
    },
    securityAlert: {
      findMany: jest.fn().mockResolvedValue([]),
      findUnique: jest.fn().mockResolvedValue(null),
      groupBy: jest.fn().mockResolvedValue([]),
      count: jest.fn().mockResolvedValue(0),
      update: jest.fn().mockImplementation(({ where, data }) => Promise.resolve({ ...where, ...data })),
    },
    dailyXp: {
      findFirst: jest.fn().mockResolvedValue(null),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-dxp-id', ...data })),
    },
    session: {
      findMany: jest.fn().mockResolvedValue([]),
      deleteMany: jest.fn().mockResolvedValue({ count: 0 }),
    },
    activityLog: {
      findMany: jest.fn().mockResolvedValue([]),
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-activity-id', ...data })),
    },
    loginLog: {
      create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-loginlog-id', ...data })),
    },
    $queryRaw: jest.fn().mockResolvedValue([{ '?column?': 1 }]),
    $connect: jest.fn().mockResolvedValue(undefined),
    $disconnect: jest.fn().mockResolvedValue(undefined),
    $transaction: jest.fn().mockImplementation(async (fn) => fn({
      submission: { create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-sub-id', ...data })) },
      activityLog: { create: jest.fn().mockImplementation(({ data }) => Promise.resolve({ id: 'mock-al-id', ...data })) },
    })),
  };
  return mockPrisma;
});

// Mock Redis
jest.mock('../../src/utils/redis', () => ({
  connectRedis: jest.fn().mockResolvedValue(undefined),
  disconnectRedis: jest.fn().mockResolvedValue(undefined),
  redisClient: { get: jest.fn(), set: jest.fn(), del: jest.fn() },
}));

// Mock logger to suppress output during tests
jest.mock('../../src/utils/logger', () => ({
  logger: {
    info: jest.fn(),
    warn: jest.fn(),
    error: jest.fn(),
    debug: jest.fn(),
  },
  requestLogger: (req, res, next) => next(),
}));

const app = require('../../src/index');
const prisma = require('../../src/utils/prisma');

/**
 * Creates a supertest instance for the Express app.
 * Handles CSRF by extracting tokens from cookies.
 */
function createTestClient() {
  const request = require('supertest');
  return request(app);
}

/**
 * Make a CSRF-protected request. First does a GET to obtain the CSRF token,
 * then makes the actual request with both cookie and header.
 */
async function csrfRequest(method, url, body) {
  const request = require('supertest');
  const agent = request.agent(app);

  // Get CSRF token via GET
  const getRes = await agent.get('/health');
  const cookies = getRes.headers['set-cookie'] || [];
  const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
  const csrfToken = csrfCookie ? csrfCookie.split(';')[0].split('=')[1] : '';

  // Collect all cookies from the GET response
  const allCookies = cookies.map(c => c.split(';')[0]).join('; ');

  let req = agent[method](url)
    .set('Cookie', allCookies)
    .set('X-CSRF-Token', csrfToken);

  if (body && (method === 'post' || method === 'put' || method === 'patch')) {
    req = req.send(body);
  }

  return req;
}

/**
 * Generate a valid-looking JWT for auth tests (won't pass real verification,
 * but lets us test middleware flow).
 */
function generateTestJWT(payload) {
  const jwt = require('jsonwebtoken');
  return jwt.sign(
    { id: payload.id || 'test-user-id', role: payload.role || 'STUDENT', schoolId: payload.schoolId || 'school-1', ...payload },
    process.env.JWT_SECRET,
    { expiresIn: '1h' }
  );
}

/**
 * Get a CSRF token from a GET request, then return it for use in POST requests.
 */
async function getCsrfToken(client) {
  const res = await client.get('/health');
  const cookies = res.headers['set-cookie'] || [];
  const csrfCookie = cookies.find(c => c.includes('_csrf_dhad'));
  if (csrfCookie) {
    return csrfCookie.split(';')[0].split('=')[1];
  }
  return null;
}

module.exports = { createTestClient, generateTestJWT, getCsrfToken, csrfRequest, prisma, app };
