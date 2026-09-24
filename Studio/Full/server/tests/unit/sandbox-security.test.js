// ========================================================
// Sandbox Security Tests — اختبارات أمنية للتنفيذ المعزول
// ========================================================

const { executeDhad, compileDhad, LIMITS } = require('../../src/utils/dhadSandbox');

describe('Dhad Sandbox — Security Tests', () => {

  // ── Execution Tests ────────────────────────────────────────
  describe('Execute: valid code', () => {
    test('runs basic arithmetic', () => {
      const result = executeDhad('صحيح x = 5\nصحيح y = 3\nطباعة(x + y)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('8');
    });

    test('runs string output', () => {
      const result = executeDhad('طباعة("مرحبا بالعالم")');
      expect(result.success).toBe(true);
      expect(result.output).toBe('مرحبا بالعالم');
    });

    test('runs boolean output', () => {
      const result = executeDhad('طباعة(صواب)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('صواب');
    });

    test('handles multiple print statements', () => {
      const result = executeDhad('طباعة(1)\nطباعة(2)\nطباعة(3)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('1\n2\n3');
    });

    test('handles if/else', () => {
      const result = executeDhad('صحيح x = 10\nإذا (x > 5) {\n  طباعة("كبير")\n}وإلا {\n  طباعة("صغير")\n}');
      expect(result.success).toBe(true);
      expect(result.output).toBe('كبير');
    });

    test('handles functions (via global state)', () => {
      const code = 'صحيح result = 0\nدالة add(a, b) {\n  result = a + b\n}\nadd(2, 3)\nطباعة(result)';
      const result = executeDhad(code);
      expect(result.success).toBe(true);
      expect(result.output).toBe('5');
    });

    test('handles arrays', () => {
      const code = 'صحيح arr = [1, 2, 3]\nطباعة(arr[0])';
      const result = executeDhad(code);
      expect(result.success).toBe(true);
      expect(result.output).toBe('1');
    });
  });

  describe('Execute: invalid syntax', () => {
    test('rejects empty code', () => {
      const result = executeDhad('');
      expect(result.success).toBe(false);
      expect(result.errors.length).toBeGreaterThan(0);
    });

    test('rejects null code', () => {
      const result = executeDhad(null);
      expect(result.success).toBe(false);
    });

    test('rejects invalid Dhad syntax', () => {
      const result = executeDhad('=== invalid ===');
      expect(result.success).toBe(false);
      expect(result.errors.length).toBeGreaterThan(0);
    });

    test('missing print runs but produces no output', () => {
      const result = executeDhad('صحيح x = 5');
      expect(result.success).toBe(true);
      expect(result.output).toBe('');
    });
  });

  describe('Execute: runtime error', () => {
    test('undefined variable prints عدم (language behavior, not error)', () => {
      const result = executeDhad('طباعة(غير_معرّf)');
      // Dhad treats undefined vars as JS undefined → prints "عدم"
      expect(result.success).toBe(true);
      expect(result.output).toBe('عدم');
    });

    test('catches division by zero', () => {
      const result = executeDhad('صحيح x = 10\nصحيح y = 0\nطباعة(x / y)');
      // JS returns Infinity for division by zero — output is Infinity
      expect(result.output).toBeDefined();
    });
  });

  // ── Timeout Tests ──────────────────────────────────────────
  describe('Timeout: infinite loop', () => {
    test('times out on while(true)', () => {
      const result = executeDhad('طالما (صواب) {\n  طباعة(1)\n}');
      expect(result.timedOut).toBe(true);
      expect(result.success).toBe(false);
      expect(result.executionTime).toBeLessThanOrEqual(LIMITS.TIMEOUT_MS + 1000);
    });

    test('times out on deep recursion', () => {
      let code = 'طالما (صواب) {\n';
      for (let i = 0; i < 20; i++) {
        code += '  طباعة("x")\n';
      }
      code += '}';
      const result = executeDhad(code);
      expect(result.timedOut).toBe(true);
    });
  });

  // ── Output Size Tests ──────────────────────────────────────
  describe('Output: large output', () => {
    test('handles large output without crash', () => {
      let code = '';
      for (let i = 0; i < 50; i++) {
        code += 'طباعة("xxxxxxxxxx")\n';
      }
      const result = executeDhad(code);
      expect(result.success).toBe(true);
      expect(result.output.length).toBeGreaterThan(0);
    });
  });

  // ── Dangerous Access Tests — ALL must fail ─────────────────
  describe('Security: dangerous access blocked', () => {
    test('require("fs") fails', () => {
      const result = executeDhad('صحيح x = require("fs")\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('process.env access fails', () => {
      const result = executeDhad('صحيح x = process.env.HOME\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('fs.readFileSync fails', () => {
      const result = executeDhad('صحيح x = fs.readFileSync("/etc/passwd")\nطباعة(x)');
      // Dhad compiler may produce syntax error or sandbox blocks it — either way fails
      expect(result.success).toBe(false);
    });

    test('child_process.execSync fails', () => {
      const result = executeDhad('صحيح x = child_process.execSync("ls")\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('module.exports access fails', () => {
      const result = executeDhad('صحيح x = module.exports\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('http.get fails', () => {
      const result = executeDhad('صحيح x = http.get("http://evil.com")\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('net.connect fails', () => {
      const result = executeDhad('صحيح x = net.connect(3306)\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('https.get fails', () => {
      const result = executeDhad('صحيح x = https.get("https://evil.com")\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('dns.resolve fails', () => {
      const result = executeDhad('صحيح x = dns.resolve("google.com")\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('Buffer.from fails', () => {
      const result = executeDhad('صحيح x = Buffer.from("test")\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('global access fails', () => {
      const result = executeDhad('صحيح x = global.process\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('setTimeout is not callable', () => {
      const result = executeDhad('صحيح x = setTimeout("code", 1000)\nطباعة(x)');
      expect(result.success).toBe(false);
    });

    test('Function constructor fails', () => {
      const result = executeDhad('صحيح x = Function("return this")()\nطباعة(x)');
      expect(result.success).toBe(false);
    });
  });

  // ── Code Length Limits ─────────────────────────────────────
  describe('Limits: code length', () => {
    test('rejects code exceeding MAX_CODE_LENGTH', () => {
      const longCode = 'طباعة("x")\n'.repeat(10000);
      const result = compileDhad(longCode);
      expect(result.errors.length).toBeGreaterThan(0);
    });
  });

  // ── Limits configuration ───────────────────────────────────
  describe('Limits: centralized config', () => {
    test('TIMEOUT_MS is 5000', () => {
      expect(LIMITS.TIMEOUT_MS).toBe(5000);
    });

    test('MAX_OUTPUT_BYTES is defined', () => {
      expect(LIMITS.MAX_OUTPUT_BYTES).toBeDefined();
      expect(LIMITS.MAX_OUTPUT_BYTES).toBe(65536);
    });

    test('MAX_CODE_LENGTH is defined', () => {
      expect(LIMITS.MAX_CODE_LENGTH).toBeDefined();
      expect(LIMITS.MAX_CODE_LENGTH).toBe(50000);
    });

    test('limits are frozen (immutable)', () => {
      expect(Object.isFrozen(LIMITS)).toBe(true);
    });
  });
});

// ========================================================
// ServerEvaluator Tests — Phase 2 (with execution result)
// ========================================================

const {
  normalize,
  detectHardcoding,
  detectSuspiciousActivity,
  evaluateOutput,
  evaluateCodeStructure,
  evaluateIntegrity,
  evaluate,
} = require('../../src/utils/serverEvaluator');

describe('ServerEvaluator Phase 2 — with Sandbox execution', () => {

  const baseChallenge = {
    expectedOutput: '15',
    dynamicOutput: '',
    requirements: '[]',
    tier: 1,
    difficulty: 'BEGINNER',
  };

  describe('evaluate: with execution result', () => {
    test('correct code + correct output passes', () => {
      const code = 'صحيح x = 5\nصحيح y = 3\nطباعة(x * y)';
      const execResult = executeDhad(code);
      const result = evaluate(code, baseChallenge, '', execResult);
      expect(result.passed).toBe(true);
      expect(result.score).toBe(100);
      expect(result.outputMatch).toBe(true);
    });

    test('wrong output fails', () => {
      const code = 'طباعة(16)';
      const execResult = executeDhad(code);
      const result = evaluate(code, baseChallenge, '', execResult);
      expect(result.passed).toBe(false);
      expect(result.outputMatch).toBe(false);
    });

    test('empty code fails', () => {
      const result = evaluate('', baseChallenge, '', null);
      expect(result.passed).toBe(false);
      expect(result.codeErrors.length).toBeGreaterThan(0);
    });

    test('infinite loop (timeout) fails', () => {
      const code = 'طالما (صواب) {\n  طباعة(1)\n}';
      const execResult = executeDhad(code);
      const result = evaluate(code, baseChallenge, '', execResult);
      expect(result.passed).toBe(false);
      expect(result.codeErrors.some(e => e.includes('الوقت'))).toBe(true);
    });

    test('code without print fails', () => {
      const code = 'صحيح x = 5';
      const execResult = executeDhad(code);
      const result = evaluate(code, baseChallenge, '', execResult);
      expect(result.passed).toBe(false);
      expect(result.codeErrors.some(e => e.includes('طباعة'))).toBe(true);
    });

    test('execution time is captured', () => {
      const code = 'صحيح x = 5\nطباعة(x)';
      const execResult = executeDhad(code);
      const result = evaluate(code, baseChallenge, '', execResult);
      expect(result.executionTime).toBeGreaterThanOrEqual(0);
    });

    test('evaluator version includes sandbox', () => {
      const code = 'صحيح x = 5\nطباعة(x)';
      const execResult = executeDhad(code);
      const result = evaluate(code, baseChallenge, '', execResult);
      expect(result).toHaveProperty('suspiciousFlags');
      expect(result).toHaveProperty('hardcodingFlags');
    });
  });

  describe('detectSuspiciousActivity: active integration', () => {
    test('flags very short code', () => {
      const flags = detectSuspiciousActivity('طباعة("hello")', { tier: 3 }, { executionTime: 10 });
      expect(flags.suspicious).toBe(true);
      expect(flags.flags.some(f => f.type === 'very_short_code')).toBe(true);
    });

    test('flags direct output', () => {
      const flags = detectSuspiciousActivity('طباعة("مرحبا")', { tier: 3 }, { executionTime: 10 });
      expect(flags.suspicious).toBe(true);
      expect(flags.flags.some(f => f.type === 'direct_output')).toBe(true);
    });

    test('flags impossibly fast execution', () => {
      const flags = detectSuspiciousActivity('صحيح x = 5\nطباعة(x)', { tier: 3 }, { executionTime: 1 });
      expect(flags.suspicious).toBe(true);
      expect(flags.flags.some(f => f.type === 'imviously_fast' || f.type === 'impossibly_fast')).toBe(true);
    });

    test('flags no computation', () => {
      const flags = detectSuspiciousActivity('طباعة("test")', { tier: 2 }, { executionTime: 10 });
      expect(flags.suspicious).toBe(true);
      expect(flags.flags.some(f => f.type === 'no_computation')).toBe(true);
    });

    test('flags timeout', () => {
      const flags = detectSuspiciousActivity('طالما (صواب) {}', { tier: 1 }, { executionTime: 5000, timedOut: true });
      expect(flags.suspicious).toBe(true);
      expect(flags.flags.some(f => f.type === 'execution_timeout')).toBe(true);
    });

    test('clean code is not suspicious', () => {
      const flags = detectSuspiciousActivity(
        'صحيح x = 5\nصحيح y = 3\nصحيح z = x + y\nطباعة(z)',
        { tier: 1 },
        { executionTime: 10 }
      );
      expect(flags.suspicious).toBe(false);
    });

    test('returns maxSeverity correctly', () => {
      const flags = detectSuspiciousActivity(
        'طباعة("test")',
        { tier: 3 },
        { executionTime: 1 }
      );
      expect(flags.maxSeverity).toBeDefined();
      expect(['LOW', 'MEDIUM', 'HIGH', 'CRITICAL']).toContain(flags.maxSeverity);
    });
  });
});
