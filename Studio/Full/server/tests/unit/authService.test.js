const {
  validatePassword,
  hashPassword,
  comparePassword,
  generateTokens,
  verifyAccessToken,
  verifyRefreshToken,
  isTokenBlacklisted,
  blacklistToken,
  validateEmail,
  sanitizeInput,
} = require('../../src/services/authService');

process.env.JWT_SECRET = 'test-secret-key-for-unit-tests-2026';
process.env.JWT_REFRESH_SECRET = 'test-refresh-secret-key-for-unit-tests-2026';
process.env.JWT_ACCESS_EXPIRES = '1h';
process.env.JWT_REFRESH_EXPIRES = '7d';

describe('authService', () => {

  describe('validatePassword', () => {
    test('rejects password shorter than 8 chars', () => {
      const errors = validatePassword('Ab1');
      expect(errors).toContain('Password must be at least 8 characters');
    });

    test('rejects password without uppercase', () => {
      const errors = validatePassword('abcdefg1');
      expect(errors.some(e => e.includes('كبير'))).toBe(true);
    });

    test('rejects password without lowercase', () => {
      const errors = validatePassword('ABCDEFG1');
      expect(errors.some(e => e.includes('صغير'))).toBe(true);
    });

    test('rejects password without number', () => {
      const errors = validatePassword('ABCDEFGh');
      expect(errors.some(e => e.includes('رقم'))).toBe(true);
    });

    test('accepts valid password', () => {
      const errors = validatePassword('StrongPass1');
      expect(errors).toHaveLength(0);
    });

    test('returns multiple errors for very weak password', () => {
      const errors = validatePassword('1');
      expect(errors.length).toBeGreaterThanOrEqual(3);
    });
  });

  describe('hashPassword & comparePassword', () => {
    test('hashPassword returns a hash string', async () => {
      const hash = await hashPassword('TestPass1');
      expect(typeof hash).toBe('string');
      expect(hash).toMatch(/^\$2[aby]?\$/);
    });

    test('comparePassword returns true for matching password', async () => {
      const hash = await hashPassword('TestPass1');
      const result = await comparePassword('TestPass1', hash);
      expect(result).toBe(true);
    });

    test('comparePassword returns false for wrong password', async () => {
      const hash = await hashPassword('TestPass1');
      const result = await comparePassword('WrongPass1', hash);
      expect(result).toBe(false);
    });

    test('different hashes for same password (random salt)', async () => {
      const hash1 = await hashPassword('TestPass1');
      const hash2 = await hashPassword('TestPass1');
      expect(hash1).not.toBe(hash2);
    });
  });

  describe('generateTokens', () => {
    const mockUser = {
      id: 'user-123',
      schoolId: 'school-456',
      classroomId: 'class-789',
      role: 'STUDENT',
      name: 'أحمد',
      tokenVersion: 0,
    };

    test('returns accessToken and refreshToken', () => {
      const tokens = generateTokens(mockUser);
      expect(tokens).toHaveProperty('accessToken');
      expect(tokens).toHaveProperty('refreshToken');
      expect(typeof tokens.accessToken).toBe('string');
      expect(typeof tokens.refreshToken).toBe('string');
    });

    test('accessToken is decodable', () => {
      const tokens = generateTokens(mockUser);
      const decoded = verifyAccessToken(tokens.accessToken);
      expect(decoded.id).toBe('user-123');
      expect(decoded.role).toBe('STUDENT');
      expect(decoded.name).toBe('أحمد');
    });

    test('refreshToken is decodable', () => {
      const tokens = generateTokens(mockUser);
      const decoded = verifyRefreshToken(tokens.refreshToken);
      expect(decoded.id).toBe('user-123');
      expect(decoded.type).toBe('refresh');
    });

    test('handles user without classroomId', () => {
      const userNoClass = { ...mockUser, classroomId: undefined };
      const tokens = generateTokens(userNoClass);
      const decoded = verifyAccessToken(tokens.accessToken);
      expect(decoded.classroomId).toBeNull();
    });
  });

  describe('verifyAccessToken', () => {
    test('verifies valid token', () => {
      const tokens = generateTokens({ id: 'u1', role: 'TEACHER', name: 'تست' });
      const decoded = verifyAccessToken(tokens.accessToken);
      expect(decoded.id).toBe('u1');
    });

    test('throws on invalid token', () => {
      expect(() => verifyAccessToken('invalid.token.here')).toThrow();
    });

    test('throws on empty token', () => {
      expect(() => verifyAccessToken('')).toThrow();
    });
  });

  describe('verifyRefreshToken', () => {
    test('verifies valid refresh token', () => {
      const tokens = generateTokens({ id: 'u2', role: 'STUDENT', name: 'تست2' });
      const decoded = verifyRefreshToken(tokens.refreshToken);
      expect(decoded.id).toBe('u2');
      expect(decoded.type).toBe('refresh');
    });

    test('throws on invalid refresh token', () => {
      expect(() => verifyRefreshToken('garbage')).toThrow();
    });
  });

  describe('blacklistToken & isTokenBlacklisted', () => {
    test('blacklisted token is detected', async () => {
      const hash = 'test-token-hash-abc';
      await blacklistToken(hash);
      expect(await isTokenBlacklisted(hash)).toBe(true);
    });

    test('non-blacklisted token is not detected', async () => {
      expect(await isTokenBlacklisted('non-existent-hash')).toBe(false);
    });

    test('different hashes are independent', async () => {
      await blacklistToken('hash-a');
      expect(await isTokenBlacklisted('hash-a')).toBe(true);
      expect(await isTokenBlacklisted('hash-b')).toBe(false);
    });
  });

  describe('validateEmail', () => {
    test('accepts valid email', () => {
      expect(validateEmail('test@example.com')).toBe(true);
    });

    test('accepts email with subdomain', () => {
      expect(validateEmail('user@mail.example.com')).toBe(true);
    });

    test('rejects email without @', () => {
      expect(validateEmail('testexample.com')).toBe(false);
    });

    test('rejects email without domain', () => {
      expect(validateEmail('test@')).toBe(false);
    });

    test('rejects null/undefined', () => {
      expect(validateEmail(null)).toBe(false);
      expect(validateEmail(undefined)).toBe(false);
    });

    test('rejects empty string', () => {
      expect(validateEmail('')).toBe(false);
    });

    test('rejects non-string input', () => {
      expect(validateEmail(123)).toBe(false);
    });

    test('rejects email with newline', () => {
      expect(validateEmail('test@\nexample.com')).toBe(false);
    });

    test('rejects email longer than 254 chars', () => {
      const longEmail = 'a'.repeat(250) + '@test.com';
      expect(validateEmail(longEmail)).toBe(false);
    });
  });

  describe('sanitizeInput', () => {
    test('removes null bytes', () => {
      expect(sanitizeInput('hello\0world')).toBe('helloworld');
    });

    test('removes control characters', () => {
      expect(sanitizeInput('hello\x01\x02\x03world')).toBe('helloworld');
    });

    test('removes carriage returns', () => {
      expect(sanitizeInput('hello\r\nworld')).toBe('hello\nworld');
    });

    test('trims whitespace', () => {
      expect(sanitizeInput('  hello  ')).toBe('hello');
    });

    test('returns non-string input as-is', () => {
      expect(sanitizeInput(42)).toBe(42);
      expect(sanitizeInput(null)).toBeNull();
    });

    test('handles empty string', () => {
      expect(sanitizeInput('')).toBe('');
    });
  });
});
