// ========================================================
// Security Tests — التحقق من أمان نظام التقييم
// تأكد من أن: السيرفر مصدر الحقيقة الوحيد
// لا يمكن تزوير الدرجة أو XP من العميل
// ========================================================

const {
  normalize,
  detectHardcoding,
  evaluateOutput,
  evaluateCodeStructure,
  evaluateIntegrity,
  evaluate,
} = require('../../src/utils/serverEvaluator');

const { verifyCode } = require('../../src/utils/codeVerifier');

// ── ServerEvaluator: التقييم على السيرفر ──────────────────────────────────────
describe('ServerEvaluator: Security Tests', () => {

  describe('normalize: تطبيع المخرجات', () => {
    test('يطبع المسافات المتعددة', () => {
      expect(normalize('  hello   world  ')).toBe('hello world');
    });

    test('يزيل التشكيل العربي', () => {
      expect(normalize('مَرْحَبًا')).toBe('مرحبا');
    });

    test('يزيل أحرف الاتجاه', () => {
      expect(normalize('\u200F\u200Eمرحبا')).toBe('مرحبا');
    });

    test('يتعامل مع null/undefined', () => {
      expect(normalize(null)).toBe('');
      expect(normalize(undefined)).toBe('');
      expect(normalize('')).toBe('');
    });
  });

  describe('detectHardcoding: كشف الغش', () => {
    test('يكشف print("output") مباشرة', () => {
      const result = detectHardcoding('print("15")', '15');
      expect(result.suspicious).toBe(true);
      expect(result.reasons).toContain('hardcoded_direct');
    });

    test('يكشف print("output") بالعربي', () => {
      const result = detectHardcoding('طباعة("مرحبا بالعالم")', 'مرحبا بالعالم');
      expect(result.suspicious).toBe(true);
      expect(result.reasons).toContain('hardcoded_direct');
    });

    test('يكشف الإخراج الرقمي المباشر', () => {
      const result = detectHardcoding('print(25)', '25');
      expect(result.suspicious).toBe(true);
      expect(result.reasons).toContain('numeric_hardcode');
    });

    test('لا يكتشف كود حسابي حقيقي', () => {
      const code = 'صحيح x = 5\nصحيح y = 3\nطباعة(x * y)';
      const result = detectHardcoding(code, '15');
      expect(result.suspicious).toBe(false);
    });

    test('لا يكتشف كود مع شرط', () => {
      const code = 'إذا (x > 5) {\n  طباعة("كبير")\n}否则 {\n  طباعة("صغير")\n}';
      const result = detectHardcoding(code, 'كبير');
      expect(result.suspicious).toBe(false);
    });

    test('لا يكتشف كود مع حلقة', () => {
      const code = 'لكل (i من 1 إلى 10) {\n  طباعة(i)\n}';
      const result = detectHardcoding(code, '1 2 3 4 5 6 7 8 9 10');
      expect(result.suspicious).toBe(false);
    });

    test('returns not suspicious when no expected output', () => {
      const result = detectHardcoding('print("hello")', '');
      expect(result.suspicious).toBe(false);
    });
  });

  describe('evaluateOutput: تقييم الإخراج', () => {
    test('يقبل الإخراج المطابق', () => {
      const result = evaluateOutput('15', '15', false, '', '');
      expect(result.match).toBe(true);
      expect(result.score).toBe(60);
    });

    test('يرفض الإخراج غير المطابق', () => {
      const result = evaluateOutput('16', '15', false, '', '');
      expect(result.match).toBe(false);
      expect(result.score).toBe(0);
    });

    test('يرفض الإخراج عندما لا يوجد متوقع', () => {
      const result = evaluateOutput('أي شيء', '', false, '', '');
      expect(result.match).toBe(false);
      expect(result.score).toBe(0);
      expect(result.reason).toBeDefined();
    });

    test('يتعامل مع dynamic output', () => {
      const result = evaluateOutput('أحمد', '', true, '{{student.name}}', 'أحمد');
      expect(result.match).toBe(true);
    });
  });

  describe('evaluateCodeStructure: تقييم بنية الكود', () => {
    test('يكشف المتغيرات الصالحة', () => {
      const result = evaluateCodeStructure('صحيح x = 5\nطباعة(x)', '[]');
      expect(result.valid).toBe(true);
      expect(result.score).toBe(25);
    });

    test('يفشل بدون متغيرات عندما مطلوب', () => {
      const reqs = JSON.stringify([{ type: 'minVariables', count: 1 }]);
      const result = evaluateCodeStructure('print(5)', reqs);
      expect(result.valid).toBe(false);
      expect(result.errors.length).toBeGreaterThan(0);
    });

    test('يفشل بدون print عندما مطلوب', () => {
      const reqs = JSON.stringify([{ type: 'minPrints', count: 1 }]);
      const result = evaluateCodeStructure('صحيح x = 5', reqs);
      expect(result.valid).toBe(false);
    });

    test('يكشف mustNotContain', () => {
      const reqs = JSON.stringify([{ type: 'mustNotContain', values: ['console.log'] }]);
      const result = evaluateCodeStructure('console.log("test")', reqs);
      expect(result.valid).toBe(false);
    });
  });

  describe('evaluateIntegrity: تقييم السلامة', () => {
    test('يكشف hardcoding', () => {
      const result = evaluateIntegrity('print("15")', '15');
      expect(result.suspicious).toBe(true);
      expect(result.score).toBeLessThan(15);
    });

    test('يقبل كود حقيقي', () => {
      const code = 'صحيح x = 5\nصحيح y = 3\nطباعة(x + y)';
      const result = evaluateIntegrity(code, '8');
      expect(result.suspicious).toBe(false);
      expect(result.score).toBe(15);
    });

    test('tier 1-2: لا يكشف hardcoding للمستويات الأساسية', () => {
      const result = evaluateIntegrity('print("15")', '15', 1);
      expect(result.suspicious).toBe(false);
      expect(result.score).toBe(15);
    });

    test('tier 3+: يكشف hardcoding للمستويات المتقدمة', () => {
      const result = evaluateIntegrity('print("15")', '15', 3);
      expect(result.suspicious).toBe(true);
      expect(result.score).toBeLessThan(15);
    });
  });

  describe('evaluate: التقييم الشامل', () => {
    const baseChallenge = {
      expectedOutput: '15',
      dynamicOutput: '',
      requirements: '[]',
      tier: 3,
    };

    test('كود صحيح يحصل على نتيجة (server لا ينفذ الكود، يتحقق من البنية فقط)', () => {
      const code = 'صحيح x = 5\nصحيح y = 3\nطباعة(x * y)';
      const result = evaluate(code, baseChallenge, '');
      // Server evaluator doesn't execute code — evaluates structure + integrity only
      expect(result.score).toBeGreaterThan(0);
      expect(result.codeErrors.length).toBe(0);
      expect(result.syntaxScore).toBe(100); // 25 * 4 = 100
    });

    test('كود فارغ يحصل على خطأ', () => {
      const result = evaluate('', baseChallenge, '');
      expect(result.passed).toBe(false);
      expect(result.codeErrors.length).toBeGreaterThan(0);
    });

    test('كود بدون print يحصل على خطأ', () => {
      const result = evaluate('صحيح x = 5', baseChallenge, '');
      expect(result.passed).toBe(false);
      expect(result.codeErrors.some(e => e.includes('print'))).toBe(true);
    });

    test('hardcoding يخفض النتيجة', () => {
      const result = evaluate('print("15")', baseChallenge, '');
      expect(result.passed).toBe(false);
      expect(result.hardcodingFlags.length).toBeGreaterThan(0);
    });

    test('لا يثق بأي بيانات من العميل — لا يوجد output في المدخلات', () => {
      // evaluate() لا يأخذ output من العميل أبداً
      // المدخلات هي: code + challenge + studentName فقط
      const code = 'صحيح x = 5\nطباعة(x)';
      const result = evaluate(code, baseChallenge, 'أحمد');
      // لا يمكن تمرير output مزيف
      expect(result).toHaveProperty('passed');
      expect(result).toHaveProperty('score');
      expect(result).toHaveProperty('codeErrors');
    });
  });
});

// ── Security: منع تزوير الدرجة من العميل ────────────────────────────────────────
describe('Security: Client Cannot Forge Scores', () => {

  test('student.js does not accept output from req.body', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    // The submission handler should only destructure challengeId and code
    expect(source).toContain('const { challengeId, code } = req.body;');
    // Should NOT have output, score, passed, etc. from req.body
    expect(source).not.toMatch(/req\.body\.output/);
    expect(source).not.toMatch(/req\.body\.score/);
    expect(source).not.toMatch(/req\.body\.passed/);
    expect(source).not.toMatch(/req\.body\.syntaxScore/);
    expect(source).not.toMatch(/req\.body\.performanceScore/);
    expect(source).not.toMatch(/req\.body\.xpAwarded/);
    expect(source).not.toMatch(/req\.body\.executionTime/);
  });

  test('student.js uses ServerEvaluator for all decisions', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    // Should import and use serverEvaluator
    expect(source).toContain('serverEvaluator');
    expect(source).toContain('evaluate(code, challenge, studentName, executionResult)');
    // Should use evalResult for everything
    expect(source).toContain('evalResult.passed');
    expect(source).toContain('evalResult.score');
    expect(source).toContain('evalResult.syntaxScore');
    expect(source).toContain('evalResult.performanceScore');
    expect(source).toContain('evalResult.codeErrors');
  });

  test('student.js uses $transaction for atomicity', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    expect(source).toContain('prisma.$transaction(async (tx)');
  });
});

// ── Security: XP محمي ──────────────────────────────────────────────────────────
describe('Security: XP Protection', () => {

  test('awardXP has no speed bonus', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/controllers/studentProfileController.js'),
      'utf8'
    );
    const awardXpStart = source.indexOf('async function awardXP');
    const awardXpBody = source.substring(awardXpStart, awardXpStart + 1200);
    expect(awardXpBody).not.toContain('speedBonus');
    expect(awardXpBody).not.toContain('performanceBonus');
  });

  test('awardXP uses baseXP only (difficulty-based)', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/controllers/studentProfileController.js'),
      'utf8'
    );
    const awardXpStart = source.indexOf('async function awardXP');
    const awardXpBody = source.substring(awardXpStart, awardXpStart + 1200);
    expect(awardXpBody).toContain('const totalXP = baseXP;');
    expect(awardXpBody).not.toContain('speedBonus');
    expect(awardXpBody).not.toContain('performanceBonus');
  });

  test('awardXP accepts transaction parameter', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/controllers/studentProfileController.js'),
      'utf8'
    );
    expect(source).toMatch(/async function awardXP\(submissionData, studentId, tx\)/);
  });

  test('checkAndAwardAchievements uses upsert (idempotent)', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/controllers/studentProfileController.js'),
      'utf8'
    );
    expect(source).toContain('userAchievement.upsert');
    expect(source).toContain('userId_achievementId');
  });
});

// ── Security: Streak صحيح ──────────────────────────────────────────────────────
describe('Security: Streak Logic', () => {

  test('calculateDailyStreak does not increment on same day', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/controllers/studentProfileController.js'),
      'utf8'
    );
    // Should have early return for today
    expect(source).toContain('Already logged today');
    expect(source).toContain('return profile.currentStreak');
  });

  test('streak is day-based, not submission-based', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/controllers/studentProfileController.js'),
      'utf8'
    );
    // Should check yesterday, not submission count
    expect(source).toContain('yesterdayStr');
    expect(source).toContain('lastActivity === yesterdayStr');
  });
});

// ── Security: Tier/Sequential Lock ─────────────────────────────────────────────
describe('Security: Tier and Sequential Lock', () => {

  test('tier lock checks previous tier completion from DB', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    expect(source).toContain('prevCompletionRate < 80');
    expect(source).toContain('CHALLENGE_SUBMISSION_BLOCKED_TIER_LOCKED');
  });

  test('sequential lock checks previous challenge from DB', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    expect(source).toContain('CHALLENGE_SUBMISSION_BLOCKED_SEQUENTIAL');
    expect(source).toContain('prevChallengePassed');
  });

  test('tier lock reads challenge tier from DB, not client', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    // challenge.tier comes from prisma.challenge.findUnique, not req.body
    expect(source).toContain('challenge.tier');
    expect(source).not.toMatch(/req\.body\.tier/);
  });
});

// ── Security: Idempotent XP ───────────────────────────────────────────────────
describe('Security: Idempotent XP', () => {

  test('existingPass check prevents double XP', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    expect(source).toContain('existingPass');
    expect(source).toContain('alreadyPassed: true');
  });

  test('UserAchievement has unique constraint', () => {
    const fs = require('fs');
    const path = require('path');
    const schema = fs.readFileSync(
      path.join(__dirname, '../../prisma/schema.prisma'),
      'utf8'
    );
    expect(schema).toContain('@@unique([userId, achievementId])');
  });
});

// ── Security: ActivityLog لا يمكن تزويره ──────────────────────────────────────
describe('Security: ActivityLog Integrity', () => {

  test('ActivityLog is created by server only, not client', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    // ActivityLog create is inside the transaction, after evaluation
    expect(source).toContain("action: evalResult.passed ? 'CHALLENGE_PASS' : 'CHALLENGE_SUBMIT'");
    // No client-supplied action field
    expect(source).not.toMatch(/req\.body\.action/);
  });
});

// ── Security: Submission Integrity ─────────────────────────────────────────────
describe('Security: Submission Integrity', () => {

  test('submission stores server-computed values only', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    // passed comes from evalResult
    expect(source).toContain('passed: evalResult.passed');
    // output is empty (server does not trust client output)
    // Output now stores server-computed execution result (may be empty or truncated)
    expect(source).toContain('output:');
    // scores come from evalResult
    expect(source).toContain('syntaxScore: evalResult.syntaxScore');
    expect(source).toContain('performanceScore: evalResult.performanceScore');
  });

  test('submission includes evaluatorVersion in activity log', () => {
    const fs = require('fs');
    const path = require('path');
    const source = fs.readFileSync(
      path.join(__dirname, '../../src/routes/student.js'),
      'utf8'
    );
    expect(source).toContain('evaluatorVersion');
  });
});
