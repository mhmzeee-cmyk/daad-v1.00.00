const { errorHandler } = require('../../src/middlewares/errorHandler');
const { logger } = require('../../src/utils/logger');

function mockReq() {
  return { method: 'GET', path: '/test', originalUrl: '/test', ip: '127.0.0.1', user: { id: 'test_user' } };
}

function mockRes() {
  const res = { statusCode: null, body: null };
  res.status = jest.fn().mockReturnValue(res);
  res.json = jest.fn().mockReturnValue(res);
  return res;
}

describe('errorHandler', () => {
  let req, res, next;

  beforeEach(() => {
    req = mockReq();
    res = mockRes();
    next = jest.fn();
    jest.spyOn(logger, 'error').mockImplementation(() => {});
  });

  afterEach(() => { logger.error.mockRestore(); });

  test('handles Prisma P2002 unique constraint error', () => {
    const err = { code: 'P2002', meta: { target: ['email', 'schoolId'] } };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(409);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ error: 'تعارض' })
    );
  });

  test('handles Prisma P2002 without meta target', () => {
    const err = { code: 'P2002' };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(409);
  });

  test('handles Prisma P2025 record not found', () => {
    const err = { code: 'P2025' };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(404);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ error: 'غير موجود' })
    );
  });

  test('handles JsonWebTokenError', () => {
    const err = { name: 'JsonWebTokenError', message: 'invalid token' };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(401);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ error: 'غير مصرح' })
    );
  });

  test('handles TokenExpiredError', () => {
    const err = { name: 'TokenExpiredError', message: 'expired' };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(401);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ message: 'انتهت صلاحية الرمز' })
    );
  });

  test('handles TypeError (NoSQL injection)', () => {
    const err = new TypeError('Cannot read property of undefined');
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(400);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ error: 'خطأ في الطلب' })
    );
  });

  test('handles SyntaxError (malformed JSON)', () => {
    const err = new SyntaxError('Unexpected token');
    err.status = 400;
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(400);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ message: 'صيغة JSON غير صحيحة في جسم الطلب' })
    );
  });

  test('handles generic error with statusCode', () => {
    const err = { statusCode: 422, message: 'Validation failed', name: 'ValidationError' };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(422);
    // SECURITY: Error message is sanitized — never exposes internal details
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ message: 'حدث خطأ في معالجة طلبك' })
    );
  });

  test('handles 500 internal server error', () => {
    const err = new Error('Something broke');
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(500);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ error: 'خطأ داخلي في الخادم' })
    );
  });

  test('handles SyntaxError without status 400 as generic error', () => {
    const err = new SyntaxError('Bad syntax');
    err.status = 500;
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(500);
  });

  test('handles Prisma P1002 connection error as 503', () => {
    const err = { code: 'P1002', message: 'SQLite database is locked' };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(503);
    expect(res.json).toHaveBeenCalledWith(
      expect.objectContaining({ error: 'Service Unavailable' })
    );
  });

  test('handles Prisma P1017 pool exhausted as 503', () => {
    const err = { code: 'P1017', message: 'Pool timeout' };
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(503);
  });

  test('handles SQLITE_BUSY error as 503', () => {
    const err = new Error('SQLITE_BUSY: database is locked');
    errorHandler(err, req, res, next);
    expect(res.status).toHaveBeenCalledWith(503);
  });

  test('logs error with structured context', () => {
    const err = new Error('Test error');
    err.code = 'TEST';
    errorHandler(err, req, res, next);
    expect(logger.error).toHaveBeenCalledWith('Unhandled error', expect.objectContaining({
      message: 'Test error',
      code: 'TEST',
      route: '/test',
      method: 'GET',
      userId: 'test_user',
    }));
  });
});
