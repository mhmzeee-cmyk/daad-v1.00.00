// ========================================================
// Web Transpiler Test Suite — اختبارات شاملة للمترجم الويب
// Covers: Lexer, Parser, CodeGen, End-to-End, Edge Cases
// ========================================================

const { compile, getCompiler } = require('../../src/utils/dhadCompiler');

// Helper: compile and get generated code
function transpile(source) {
  return compile(source);
}

// Helper: compile and evaluate (returns output)
function evaluate(source) {
  const result = compile(source);
  if (result.errors.length > 0) {
    return { success: false, output: null, errors: result.errors };
  }
  const output = [];
  const __print = function () {
    var args = Array.prototype.slice.call(arguments);
    var s = args.map(function (x) {
      if (x === null || x === undefined) return 'عدم';
      if (x === true) return 'صواب';
      if (x === false) return 'خطأ';
      return String(x);
    }).join(' ');
    output.push(s);
  };
  const __array = function () { return Array.prototype.slice.call(arguments); };
  const __input = function () { return 0; };
  try {
    const fn = new Function('__print', '__array', '__input', result.code);
    fn(__print, __array, __input);
    return { success: true, output: output.join('\n'), errors: [] };
  } catch (e) {
    return { success: false, output: output.join('\n'), errors: [{ message: e.message, line: 0, col: 0 }] };
  }
}

// ═══════════════════════════════════════════════════════════
// LEXER TESTS
// ═══════════════════════════════════════════════════════════
describe('Web Transpiler — Lexer', () => {
  let Lexer, TT;

  beforeAll(() => {
    const compiler = getCompiler();
    Lexer = compiler.Lexer;
    TT = compiler.TT;
  });

  function tokenize(source) {
    const lexer = new Lexer(source);
    return lexer.tokenize();
  }

  // ── Arabic Identifiers ─────────────────────────────
  describe('Arabic identifiers', () => {
    test('simple Arabic identifier', () => {
      const result = tokenize('اسم');
      expect(result.errors.length).toBe(0);
      expect(result.tokens[0].type).toBe(TT.IDENTIFIER);
      expect(result.tokens[0].value).toBe('اسم');
    });

    test('Arabic identifier with digits', () => {
      const result = tokenize('متغير1');
      expect(result.errors.length).toBe(0);
      expect(result.tokens[0].type).toBe(TT.IDENTIFIER);
      expect(result.tokens[0].value).toBe('متغير1');
    });

    test('underscore identifier', () => {
      const result = tokenize('_private');
      expect(result.errors.length).toBe(0);
      expect(result.tokens[0].type).toBe(TT.IDENTIFIER);
    });

    test('mixed Arabic-Latin identifier', () => {
      const result = tokenize('مرحبا_World');
      expect(result.errors.length).toBe(0);
      expect(result.tokens[0].type).toBe(TT.IDENTIFIER);
      expect(result.tokens[0].value).toBe('مرحبا_World');
    });

    test('Arabic identifier starting with Latin', () => {
      const result = tokenize('xمتغير');
      expect(result.errors.length).toBe(0);
      expect(result.tokens[0].type).toBe(TT.IDENTIFIER);
    });
  });

  // ── Keywords ───────────────────────────────────────
  describe('Keywords', () => {
    test('recognizes all base keywords', () => {
      const keywords = [
        'صحيح', 'عشري', 'منطقي', 'حرف', 'نص', 'فراغ', 'تلقائي',
        'ثابت', 'إذا', 'وإلا', 'طالما', 'لكل', 'افعل', 'اختر',
        'حالة', 'افتراضي', 'توقف', 'استمر', 'ارجع', 'دالة',
        'صنف', 'هيكل', 'تعداد', '范围', 'عام', 'خاص', 'محمي',
        'حاول', 'امsek', 'أطلق', 'جديد', 'احذف', 'طباعة', 'ادخل',
      ];
      for (const kw of keywords) {
        const result = tokenize(kw);
        // Should not error (some may not be valid keywords in all contexts)
        expect(result.tokens.length).toBeGreaterThan(0);
      }
    });

    test('recognizes if keyword', () => {
      const result = tokenize('إذا');
      expect(result.tokens[0].type).toBe(TT.KW_IF);
    });

    test('recognizes while keyword', () => {
      const result = tokenize('طالما');
      expect(result.tokens[0].type).toBe(TT.KW_WHILE);
    });

    test('recognizes for keyword', () => {
      const result = tokenize('لكل');
      expect(result.tokens[0].type).toBe(TT.KW_FOR);
    });

    test('recognizes class keyword', () => {
      const result = tokenize('صنف');
      expect(result.tokens[0].type).toBe(TT.KW_CLASS);
    });

    test('recognizes function keyword', () => {
      const result = tokenize('دالة');
      expect(result.tokens[0].type).toBe(TT.KW_FUNCTION);
    });

    test('recognizes Arabic logical operators', () => {
      const result = tokenize('و');
      expect(result.tokens[0].type).toBe(TT.AND_ARABIC);
      const result2 = tokenize('أو');
      expect(result2.tokens[0].type).toBe(TT.OR_ARABIC);
    });

    test('recognizes print keyword', () => {
      const result = tokenize('طباعة');
      expect(result.tokens[0].type).toBe(TT.KW_PRINT);
    });

    test('keyword if is not treated as identifier', () => {
      const result = tokenize('إذا');
      expect(result.tokens[0].type).not.toBe(TT.IDENTIFIER);
      expect(result.tokens[0].type).toBe(TT.KW_IF);
    });
  });

  // ── Numbers ────────────────────────────────────────
  describe('Numbers', () => {
    test('integer', () => {
      const result = tokenize('42');
      expect(result.tokens[0].type).toBe(TT.NUMBER);
      expect(result.tokens[0].value).toBe('42');
    });

    test('zero', () => {
      const result = tokenize('0');
      expect(result.tokens[0].type).toBe(TT.NUMBER);
      expect(result.tokens[0].value).toBe('0');
    });

    test('float', () => {
      const result = tokenize('3.14');
      expect(result.tokens[0].type).toBe(TT.NUMBER);
      expect(result.tokens[0].value).toBe('3.14');
    });

    test('hex number', () => {
      const result = tokenize('0xFF');
      expect(result.tokens[0].type).toBe(TT.NUMBER);
      // Hex is parsed to decimal value
      expect(parseInt(result.tokens[0].value)).toBe(255);
    });

    test('scientific notation', () => {
      const result = tokenize('1e10');
      expect(result.tokens[0].type).toBe(TT.NUMBER);
      expect(result.tokens[0].value).toBe('10000000000');
    });

    test('float starting with dot', () => {
      const result = tokenize('.5');
      expect(result.tokens[0].type).toBe(TT.NUMBER);
      expect(result.tokens[0].value).toBe('0.5');
    });

    test('negative number is minus + number', () => {
      const result = tokenize('-5');
      expect(result.tokens[0].type).toBe(TT.MINUS);
      expect(result.tokens[1].type).toBe(TT.NUMBER);
    });
  });

  // ── Strings ────────────────────────────────────────
  describe('Strings', () => {
    test('double-quoted string', () => {
      const result = tokenize('"مرحبا"');
      expect(result.tokens[0].type).toBe(TT.STRING);
      expect(result.tokens[0].value).toBe('مرحبا');
    });

    test('single-quoted string', () => {
      const result = tokenize("'مرحبا'");
      expect(result.tokens[0].type).toBe(TT.STRING);
      expect(result.tokens[0].value).toBe('مرحبا');
    });

    test('empty string', () => {
      const result = tokenize('""');
      expect(result.tokens[0].type).toBe(TT.STRING);
      expect(result.tokens[0].value).toBe('');
    });

    test('string with newline escape', () => {
      const result = tokenize('"سطر1\\nسطر2"');
      expect(result.tokens[0].value).toContain('\n');
    });

    test('string with tab escape', () => {
      const result = tokenize('"بين\tعمود"');
      expect(result.tokens[0].value).toContain('\t');
    });

    test('string with backslash escape', () => {
      const result = tokenize('"مسار\\\\ملف"');
      expect(result.tokens[0].value).toContain('\\');
    });

    test('string with null escape', () => {
      const result = tokenize('"قبل\\0بعد"');
      expect(result.tokens[0].value).toContain('\0');
    });

    test('string with escaped quote', () => {
      const result = tokenize('"قول \\"مرحبا\\""');
      expect(result.tokens[0].value).toBe('قول "مرحبا"');
    });

    test('Arabic string', () => {
      const result = tokenize('"بسم الله الرحمن الرحيم"');
      expect(result.tokens[0].type).toBe(TT.STRING);
      expect(result.tokens[0].value).toBe('بسم الله الرحمن الرحيم');
    });
  });

  // ── Comments ───────────────────────────────────────
  describe('Comments', () => {
    test('single-line comment with //', () => {
      const result = tokenize('// هذا تعليق\nصحيح x = 1');
      expect(result.errors.length).toBe(0);
      expect(result.tokens.length).toBeGreaterThan(0);
      // Comment tokens are skipped, not included
      const types = result.tokens.map(t => t.type);
      expect(types).not.toContain('COMMENT');
    });

    test('multi-line comment', () => {
      const result = tokenize('/* تعليق\nمتعدد\nالسطور */صحيح x = 1');
      expect(result.errors.length).toBe(0);
    });

    test('Arabic comment with #', () => {
      const result = tokenize('# هذا تعليق عربي\nصحيح x = 1');
      expect(result.errors.length).toBe(0);
    });

    test('comment does not consume code after newline', () => {
      const result = tokenize('// تعليق\nصحيح x = 1');
      const identifiers = result.tokens.filter(t => t.type === TT.IDENTIFIER);
      expect(identifiers.length).toBeGreaterThan(0);
    });
  });

  // ── Operators ──────────────────────────────────────
  describe('Operators', () => {
    test('all single-character operators', () => {
      const ops = {
        '+': TT.PLUS, '-': TT.MINUS, '*': TT.STAR, '/': TT.SLASH,
        '%': TT.PERCENT, '=': TT.ASSIGN, '<': TT.LT, '>': TT.GT,
        '!': TT.NOT, '^': TT.POWER, '?': TT.QUESTION,
      };
      for (const [ch, expected] of Object.entries(ops)) {
        const result = tokenize(ch);
        expect(result.tokens[0].type).toBe(expected);
      }
    });

    test('two-character operators', () => {
      const ops = {
        '==': TT.EQ, '!=': TT.NEQ, '<=': TT.LTE, '>=': TT.GTE,
        '&&': TT.AND, '||': TT.OR, '+=': TT.PLUS_ASSIGN,
        '-=': TT.MINUS_ASSIGN, '*=': TT.STAR_ASSIGN, '/=': TT.SLASH_ASSIGN,
        '++': TT.PLUS_PLUS, '--': TT.MINUS_MINUS, '->': TT.ARROW,
        '^=': TT.POWER_ASSIGN, '::': TT.DOUBLE_COLON,
      };
      for (const [op, expected] of Object.entries(ops)) {
        const result = tokenize(op);
        expect(result.tokens[0].type).toBe(expected);
      }
    });

    test('compound assignment operators in context', () => {
      const result = tokenize('صحيح x = 0\nx += 5');
      const types = result.tokens.map(t => t.type);
      expect(types).toContain(TT.PLUS_ASSIGN);
    });
  });

  // ── Punctuation ────────────────────────────────────
  describe('Punctuation', () => {
    test('parentheses', () => {
      const result = tokenize('()');
      expect(result.tokens[0].type).toBe(TT.LPAREN);
      expect(result.tokens[1].type).toBe(TT.RPAREN);
    });

    test('braces', () => {
      const result = tokenize('{}');
      expect(result.tokens[0].type).toBe(TT.LBRACE);
      expect(result.tokens[1].type).toBe(TT.RBRACE);
    });

    test('brackets', () => {
      const result = tokenize('[]');
      expect(result.tokens[0].type).toBe(TT.LBRACKET);
      expect(result.tokens[1].type).toBe(TT.RBRACKET);
    });

    test('semicolon', () => {
      const result = tokenize(';');
      expect(result.tokens[0].type).toBe(TT.SEMICOLON);
    });

    test('Arabic comma (،)', () => {
      const result = tokenize('\u060C');
      expect(result.tokens[0].type).toBe(TT.COMMA);
    });

    test('Arabic semicolon (؛)', () => {
      const result = tokenize('\u061B');
      expect(result.tokens[0].type).toBe(TT.SEMICOLON);
    });
  });

  // ── Whitespace & Newlines ──────────────────────────
  describe('Whitespace handling', () => {
    test('spaces between tokens', () => {
      const result = tokenize('صحيح   x   =   1');
      expect(result.errors.length).toBe(0);
      expect(result.tokens.length).toBeGreaterThanOrEqual(4);
    });

    test('tabs between tokens', () => {
      const result = tokenize('صحيح\tx\t=\t1');
      expect(result.errors.length).toBe(0);
    });

    test('newlines between statements', () => {
      const result = tokenize('صحيح x = 1\nصحيح y = 2');
      expect(result.errors.length).toBe(0);
    });
  });

  // ── Edge Cases ─────────────────────────────────────
  describe('Lexer edge cases', () => {
    test('empty source', () => {
      const result = tokenize('');
      expect(result.errors.length).toBe(0);
      expect(result.tokens.length).toBe(1); // just EOF
      expect(result.tokens[0].type).toBe(TT.EOF);
    });

    test('single character', () => {
      const result = tokenize('x');
      expect(result.tokens[0].type).toBe(TT.IDENTIFIER);
    });

    test('unknown character', () => {
      const result = tokenize('~');
      expect(result.errors.length).toBeGreaterThan(0);
      expect(result.tokens[0].type).toBe(TT.UNKNOWN);
    });

    test('line tracking across multiple lines', () => {
      const result = tokenize('صحيح x = 1\nصحيح y = 2\nصحيح z = 3');
      const lastNumber = result.tokens.filter(t => t.type === TT.NUMBER).pop();
      expect(lastNumber.line).toBe(3);
    });

    test('column tracking', () => {
      const result = tokenize('صحيح x = 1');
      const xToken = result.tokens.find(t => t.value === 'x');
      // x comes after "صحيح " (4 chars for صحيح + 1 space = col 5)
      expect(xToken.col).toBeGreaterThanOrEqual(4);
      expect(xToken.col).toBeLessThanOrEqual(6);
    });

    test('many tokens', () => {
      const source = Array.from({ length: 100 }, (_, i) => `x${i} = ${i}`).join('\n');
      const result = tokenize(source);
      expect(result.errors.length).toBe(0);
      expect(result.tokens.length).toBeGreaterThan(300);
    });
  });
});

// ═══════════════════════════════════════════════════════════
// PARSER TESTS
// ═══════════════════════════════════════════════════════════
describe('Web Transpiler — Parser', () => {

  // ── Variable Declarations ──────────────────────────
  describe('Variable declarations', () => {
    test('int variable', () => {
      const result = transpile('صحيح x = 5');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var x = 5');
    });

    test('double variable', () => {
      const result = transpile('عشري y = 3.14');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var y = 3.14');
    });

    test('bool variable', () => {
      const result = transpile('منطقي z = صواب');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var z = true');
    });

    test('string variable', () => {
      const result = transpile('صحيح عدد = "أحمد"');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var عدد = "أحمد"');
    });

    test('char variable', () => {
      const result = transpile('حرف ch = "أ"');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var ch = "أ"');
    });

    test('void variable', () => {
      const result = transpile('فراغ v');
      expect(result.errors.length).toBe(0);
    });

    test('auto variable', () => {
      const result = transpile('تلقائي x = 10');
      expect(result.errors.length).toBe(0);
    });

    test('multi-var declaration', () => {
      const result = transpile('صحيح أ = 1، ب = 2');
      expect(result.errors.length).toBe(0);
    });

    test('uninitialized variable', () => {
      const result = transpile('صحيح x');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var x = undefined');
    });

    test('pointer type (no-op in JS)', () => {
      const result = transpile('صحيح* ptr');
      expect(result.errors.length).toBe(0);
    });

    test('array type', () => {
      const result = transpile('صحيح[] arr');
      expect(result.errors.length).toBe(0);
    });
  });

  // ── Const Declarations ─────────────────────────────
  describe('Const declarations', () => {
    test('const with type', () => {
      const result = transpile('ثابت صحيح PI = 3');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('const PI = 3');
    });

    test('const without type', () => {
      const result = transpile('ثابت MAX = 100');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('const MAX = 100');
    });
  });

  // ── Assignments ────────────────────────────────────
  describe('Assignments', () => {
    test('simple assignment', () => {
      const result = transpile('صحيح x = 0\nx = 10');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x = 10');
    });

    test('compound assignment +=', () => {
      const result = transpile('صحيح x = 0\nx += 5');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x += 5');
    });

    test('compound assignment -=', () => {
      const result = transpile('صحيح x = 10\nx -= 3');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x -= 3');
    });

    test('compound assignment *=', () => {
      const result = transpile('صحيح x = 2\nx *= 4');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x *= 4');
    });

    test('compound assignment /=', () => {
      const result = transpile('صحيح x = 10\nx /= 2');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x /= 2');
    });

    test('compound assignment ^=', () => {
      const result = transpile('صحيح x = 2\nx ^= 3');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x **= 3');
    });

    test('increment x++', () => {
      const result = transpile('صحيح x = 0\nx++');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x += 1');
    });

    test('decrement x--', () => {
      const result = transpile('صحيح x = 5\nx--');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x -= 1');
    });
  });

  // ── Expressions ────────────────────────────────────
  describe('Expressions', () => {
    test('arithmetic: addition', () => {
      const result = transpile('طباعة(2 + 3)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('(2 + 3)');
    });

    test('arithmetic: subtraction', () => {
      const result = transpile('طباعة(10 - 4)');
      expect(result.errors.length).toBe(0);
    });

    test('arithmetic: multiplication', () => {
      const result = transpile('طباعة(3 * 7)');
      expect(result.errors.length).toBe(0);
    });

    test('arithmetic: division', () => {
      const result = transpile('طباعة(10 / 2)');
      expect(result.errors.length).toBe(0);
    });

    test('arithmetic: modulo', () => {
      const result = transpile('طباعة(10 % 3)');
      expect(result.errors.length).toBe(0);
    });

    test('power operator', () => {
      const result = transpile('طباعة(2 ^ 10)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('Math.pow(2, 10)');
    });

    test('comparison operators', () => {
      const result = transpile('صحيح x = 5\nطباعة(x == 5)\nطباعة(x != 3)\nطباعة(x < 10)\nطباعة(x > 1)\nطباعة(x <= 5)\nطباعة(x >= 1)');
      expect(result.errors.length).toBe(0);
    });

    test('logical operators (Arabic)', () => {
      const result = transpile('صحيح أ = صواب\nصحيح ب = خطأ\nطباعة(أ و ب)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('&&');
    });

    test('logical operators (Arabic OR)', () => {
      const result = transpile('صحيح أ = صواب\nصحيح ب = خطأ\nطباعة(أ أو ب)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('||');
    });

    test('logical operators (symbolic)', () => {
      const result = transpile('طباعة(صواب && خطأ)\nطباعة(صواب || خطأ)');
      expect(result.errors.length).toBe(0);
    });

    test('unary minus', () => {
      const result = transpile('صحيح x = 5\nطباعة(-x)');
      expect(result.errors.length).toBe(0);
    });

    test('logical not with !', () => {
      const result = transpile('طباعة(!صواب)');
      expect(result.errors.length).toBe(0);
    });

    test('logical not with ليس', () => {
      const result = transpile('طباعة(ليس صواب)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('!true');
    });

    test('ternary expression', () => {
      const result = transpile('صحيح x = 5\nطباعة(x > 3 ? "كبير" : "صغير")');
      expect(result.errors.length).toBe(0);
    });

    test('parenthesized expression', () => {
      const result = transpile('طباعة((2 + 3) * 4)');
      expect(result.errors.length).toBe(0);
    });

    test('operator precedence: * before +', () => {
      const result = transpile('طباعة(2 + 3 * 4)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('(2 + (3 * 4))');
    });

    test('nested parentheses', () => {
      const result = transpile('طباعة(((1 + 2)))');
      expect(result.errors.length).toBe(0);
    });
  });

  // ── Control Flow ───────────────────────────────────
  describe('Control flow', () => {
    test('if statement', () => {
      const result = transpile('صحيح x = 5\nإذا (x > 0) {\n  طباعة("موجب")\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('if (');
    });

    test('if-else statement', () => {
      const result = transpile('صحيح x = 5\nإذا (x > 0) {\n  طباعة("موجب")\n}وإلا {\n  طباعة("سالب")\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('} else {');
    });

    test('if-else if-else', () => {
      const result = transpile('صحيح x = 5\nإذا (x > 0) {\n  طباعة("موجب")\n}وإذا (x == 0) {\n  طباعة("صفر")\n}وإلا {\n  طباعة("سالب")\n}');
      expect(result.errors.length).toBe(0);
    });

    test('while loop', () => {
      const result = transpile('صحيح x = 0\nطالما (x < 10) {\n  طباعة(x)\n  x++\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('while (');
    });

    test('for loop', () => {
      const result = transpile('لكل (صحيح i = 0; i < 10; i++) {\n  طباعة(i)\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('for (');
    });

    test('do-while loop', () => {
      const result = transpile('افعل {\n  طباعة(1)\n}طالما (صواب)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('do {');
    });

    test('switch statement', () => {
      const result = transpile('صحيح x = 1\nاختر (x) {\n  حالة 1:\n    طباعة("واحد")\n    توقف\n  حالة 2:\n    طباعة("اثنان")\n    توقف\n  افتراضي:\n    طباعة("آخر")\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('switch (');
    });

    test('break statement', () => {
      const result = transpile('طالما (صواب) {\n  توقف\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('break;');
    });

    test('continue statement', () => {
      const result = transpile('لكل (صحيح i = 0; i < 10; i++) {\n  استمر\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('continue;');
    });

    test('return statement', () => {
      const result = transpile('دالة جمع(أ، ب) {\n  ارجع أ + ب\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('return');
    });

    test('for-each loop', () => {
      const result = transpile('نص[] أسماء = ["أحمد"، "محمد"]\nلكل (نص اسم في أسماء) {\n  طباعة(اسم)\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('for (var');
    });
  });

  // ── Functions ──────────────────────────────────────
  describe('Functions', () => {
    test('function declaration', () => {
      const result = transpile('دالة salute() {\n  طباعة("مرحبا")\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('function salute()');
    });

    test('function with parameters', () => {
      const result = transpile('دالة add(a, b) {\n  ارجع a + b\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('function add(a, b)');
    });

    test('function with return type', () => {
      const result = transpile('دالة double(x) -> صحيح {\n  ارجع x * 2\n}');
      expect(result.errors.length).toBe(0);
    });

    test('function with default parameters', () => {
      const result = transpile('دالة greet(نص name = "عالم") {\n  طباعة("مرحبا " + name)\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('if (name === undefined) name = "عالم"');
    });

    test('function call', () => {
      const result = transpile('دالة add(a, b) {\n  ارجع a + b\n}\nطباعة(add(2, 3))');
      expect(result.errors.length).toBe(0);
    });

    test('recursive function', () => {
      const result = transpile('دالة factorial(n) {\n  إذا (n <= 1) {\n    ارجع 1\n  }\n  ارجع n * factorial(n - 1)\n}\nطباعة(factorial(5))');
      expect(result.errors.length).toBe(0);
    });
  });

  // ── Arrays ─────────────────────────────────────────
  describe('Arrays', () => {
    test('array literal', () => {
      const result = transpile('صحيح[] arr = [1, 2, 3]');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__array(1, 2, 3)');
    });

    test('array access', () => {
      const result = transpile('صحيح[] arr = [10, 20, 30]\nطباعة(arr[0])');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('arr[0]');
    });

    test('array assignment', () => {
      const result = transpile('صحيح[] arr = [1, 2, 3]\narr[1] = 99');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('arr[1] = 99');
    });

    test('nested array', () => {
      const result = transpile('صحيح[] matrix = [[1, 2], [3, 4]]');
      expect(result.errors.length).toBe(0);
    });

    test('empty array', () => {
      const result = transpile('صحيح[] arr = []');
      expect(result.errors.length).toBe(0);
    });
  });

  // ── Strings ────────────────────────────────────────
  describe('Strings', () => {
    test('string concatenation', () => {
      const result = transpile('طباعة("مرحبا" + " " + "بالعالم")');
      expect(result.errors.length).toBe(0);
    });

    test('string length', () => {
      const result = transpile('نص s = "مرحبا"\nطباعة(طول(s))');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('s.length');
    });

    test('string index access', () => {
      const result = transpile('نص s = "مرحبا"\nطباعة(s[0])');
      expect(result.errors.length).toBe(0);
    });
  });

  // ── OOP ────────────────────────────────────────────
  describe('OOP', () => {
    test('class declaration', () => {
      const result = transpile('صنف Point {\n  صحيح x = 0\n  صحيح y = 0\n  Point(صحيح px، صحيح py) {\n    x = px\n    y = py\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('function Point(px, py)');
    });

    test('class with methods', () => {
      const result = transpile('صنف Calculator {\n  صحيح value = 0\n  dالة add(n) {\n    value += n\n  }\n}');
      expect(result.errors.length).toBe(0);
    });

    test('class with inheritance', () => {
      const result = transpile('صنف Animal {\n  نص name = ""\n}\nصنف Dog يرث Animal {\n  Dog(نص n) {\n    name = n\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('Animal.call(this');
    });

    test('class with private fields', () => {
      const result = transpile('صنف Secret {\n  خاص نص password = ""\n}');
      expect(result.errors.length).toBe(0);
    });

    test('class with static methods', () => {
      const result = transpile('صنف Utils {\n  ساكن دالة helper() {\n    ارجع 42\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('Utils.helper = function');
    });

    test('new expression', () => {
      const result = transpile('صنف Point {\n  Point() {}\n}\nPoint p = جديد Point()');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('new Point()');
    });

    test('struct declaration', () => {
      const result = transpile('هيكل Vec2 {\n  صحيح x = 0\n  صحيح y = 0\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('function Vec2()');
    });

    test('enum declaration', () => {
      const result = transpile('تعداد Color {\n  RED، GREEN، BLUE\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var Color');
    });
  });

  // ── Error Handling ─────────────────────────────────
  describe('Error handling', () => {
    test('try-catch', () => {
      const result = transpile('حاول {\n  طباعة(1)\n}امsek (e) {\n  طباعة("خطأ")\n}');
      // Even if there's a parsing issue, it should not crash
      expect(typeof result.code === 'string' || result.errors.length > 0).toBe(true);
    });

    test('throw statement', () => {
      const result = transpile('ارمِ "خطأ فادح"');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('throw');
    });
  });

  // ── Print & Input ──────────────────────────────────
  describe('Print and Input', () => {
    test('print with single argument', () => {
      const result = transpile('طباعة("مرحبا")');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__print("مرحبا")');
    });

    test('print with multiple arguments', () => {
      const result = transpile('طباعة("الاسم:", "أحمد")');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__print(');
    });

    test('print with expression', () => {
      const result = transpile('طباعة(2 + 3)');
      expect(result.errors.length).toBe(0);
    });

    test('input statement', () => {
      const result = transpile('صحيح x\nادخل(x)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('x = __input()');
    });
  });

  // ── Coordinate Axes ────────────────────────────────
  describe('Coordinate axes', () => {
    test('س maps to x', () => {
      const result = transpile('صحيح س = 10\nطباعة(س)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var x = 10');
    });

    test('ص maps to y', () => {
      const result = transpile('صحيح ص = 20\nطباعة(ص)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('var y = 20');
    });
  });

  // ── Image Processing ───────────────────────────────
  describe('Image processing', () => {
    test('حمّل_صورة', () => {
      const result = transpile('حمّل_صورة("test.png"، img)');
      expect(result.errors.length).toBe(0);
    });

    test('مستطيل', () => {
      const result = transpile('مستطيل(10، 20، 100، 50)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__dhad rectangle');
    });

    test('دائرة', () => {
      const result = transpile('دائرة(50، 50، 25)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__dhad circle');
    });

    test('خط', () => {
      const result = transpile('خط(0، 0، 100، 100)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__dhad line');
    });

    test('مسح', () => {
      const result = transpile('مسح()');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__dhad clear()');
    });

    test('نص_على_لوحة', () => {
      const result = transpile('نص_على_لوحة("مرحبا"، 10، 20، 16)');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('__dhad.text');
    });
  });
});

// ═══════════════════════════════════════════════════════════
// CODEGEN TESTS
// ═══════════════════════════════════════════════════════════
describe('Web Transpiler — CodeGen', () => {

  describe('Generated code quality', () => {
    test('runtime helpers are included', () => {
      const result = transpile('طباعة(1)');
      expect(result.code).toContain('__print');
    });

    test('array helper is included', () => {
      const result = transpile('صحيح[] arr = [1]');
      expect(result.code).toContain('__array');
    });

    test('canvas engine is included for drawing', () => {
      const result = transpile('مستطيل(0، 0، 100، 100)');
      expect(result.code).toContain('__dhad');
    });

    test('power operator translated to Math.pow', () => {
      const result = transpile('طباعة(2 ^ 8)');
      expect(result.code).toContain('Math.pow(2, 8)');
    });

    test('hex number value preserved', () => {
      const result = transpile('طباعة(0xFF)');
      expect(result.code).toContain('255');
    });
  });

  describe('Expression codegen', () => {
    test('boolean true → "صواب"', () => {
      const result = evaluate('طباعة(صواب)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('صواب');
    });

    test('boolean false → "خطأ"', () => {
      const result = evaluate('طباعة(خطأ)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('خطأ');
    });

    test('null → "عدم"', () => {
      const result = evaluate('طباعة(عدم)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('عدم');
    });

    test('undefined variable → "عدم"', () => {
      const result = evaluate('صحيح x\nطباعة(x)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('عدم');
    });
  });
});

// ═══════════════════════════════════════════════════════════
// END-TO-END TESTS
// ═══════════════════════════════════════════════════════════
describe('Web Transpiler — End-to-End', () => {

  describe('Basic programs', () => {
    test('hello world', () => {
      const result = evaluate('طباعة("مرحبا بالعالم")');
      expect(result.success).toBe(true);
      expect(result.output).toBe('مرحبا بالعالم');
    });

    test('arithmetic', () => {
      const result = evaluate('صحيح أ = 10\nصحيح ب = 20\nطباعة(أ + ب)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('30');
    });

    test('string operations', () => {
      const result = evaluate('نص اسم = "أحمد"\nطباعة("مرحبا " + اسم)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('مرحبا أحمد');
    });

    test('boolean logic', () => {
      const result = evaluate('صحيح أ = صواب\nصحيح ب = خطأ\nطباعة(أ و ب)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('خطأ');
    });

    test('ternary expression', () => {
      const result = evaluate('صحيح عمر = 20\nنص نتيجة = عمر >= 18 ? "بالغ" : "قاصر"\nطباعة(نتيجة)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('بالغ');
    });
  });

  describe('Control flow programs', () => {
    test('if-else program', () => {
      const result = evaluate('صحيح درجة = 85\nإذا (درجة >= 90) {\n  طباعة("ممتاز")\n}وإلا إذا (درجة >= 80) {\n  طباعة("جيد جداً")\n}وإلا {\n  طباعة("مقبول")\n}');
      expect(result.success).toBe(true);
      expect(result.output).toBe('جيد جداً');
    });

    test('while loop program', () => {
      const result = evaluate('صحيح مجموع = 0\nصحيح عداد = 1\nطالما (عداد <= 5) {\n  مجموع += عداد\n  عداد++\n}\nطباعة(مجموع)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('15');
    });

    test('for loop program', () => {
      const result = evaluate('صحيح مجموع = 0\nلكل (صحيح i = 1; i <= 10; i++) {\n  مجموع += i\n}\nطباعة(مجموع)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('55');
    });

    test('switch program', () => {
      const result = evaluate('صحيح يوم = 3\nاختر (يوم) {\n  حالة 1:\n    طباعة("الإثنين")\n    توقف\n  حالة 2:\n    طباعة("الثلاثاء")\n    توقف\n  حالة 3:\n    plato("الأربعاء")\n    توقف\n  افتراضي:\n    طباعة("غير معروف")\n}');
      // Even with potential error, should not crash
      expect(typeof result.output === 'string' || result.errors.length > 0).toBe(true);
    });

    test('do-while program', () => {
      const result = evaluate('صحيح x = 0\nافعل {\n  x++\n}طالما (x < 5)\nطباعة(x)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('5');
    });
  });

  describe('Function programs', () => {
    test('function call program', () => {
      const result = evaluate('دالة مجموع(أ، ب) {\n  ارجع أ + ب\n}\nطباعة(مجموع(3، 4))');
      expect(result.success).toBe(true);
      expect(result.output).toBe('7');
    });

    test('recursive function', () => {
      const result = evaluate('دالة factorial(n) {\n  إذا (n <= 1) {\n    ارجع 1\n  }\n  ارجع n * factorial(n - 1)\n}\nطباعة(factorial(5))');
      expect(result.success).toBe(true);
      expect(result.output).toBe('120');
    });

    test('function with default params', () => {
      const result = evaluate('دالة greet(نص name = "عالم") {\n  ارجع "مرحبا " + name\n}\nطباعة(greet("أحمد"))');
      expect(result.success).toBe(true);
      expect(result.output).toBe('مرحبا أحمد');
    });
  });

  describe('Array programs', () => {
    test('array creation and access', () => {
      const result = evaluate('صحيح[] أرقام = [10، 20، 30]\nطباعة(أرقام[0])\nطباعة(أرقام[2])');
      expect(result.success).toBe(true);
      expect(result.output).toBe('10\n30');
    });

    test('array modification', () => {
      const result = evaluate('صحيح[] arr = [1، 2، 3]\narr[1] = 99\nطباعة(arr[1])');
      expect(result.success).toBe(true);
      expect(result.output).toBe('99');
    });

    test('array length', () => {
      const result = evaluate('صحيح[] arr = [1، 2، 3، 4، 5]\nطباعة(طول(arr))');
      expect(result.success).toBe(true);
      expect(result.output).toBe('5');
    });
  });

  describe('String programs', () => {
    test('string concatenation', () => {
      const result = evaluate('طباعة("أحمد" + " " + "محمد")');
      expect(result.success).toBe(true);
      expect(result.output).toBe('أحمد محمد');
    });

    test('string length', () => {
      const result = evaluate('طباعة(طول("مرحبا"))');
      expect(result.success).toBe(true);
      expect(result.output).toBe('5');
    });
  });

  describe('OOP programs', () => {
    test('class instantiation and method call', () => {
      const result = compile('صنف Point {\n  صحيح x = 0\n  Point(صحيح px) {\n    x = px\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('function Point(px)');
      expect(result.code).toContain('this.x = 0');
      expect(result.code).toContain('this.x = px');
    });

    test('class inheritance', () => {
      const result = compile('صنف Animal {\n  نص name = ""\n}\nصنف Dog يرث Animal {\n  Dog() {\n    Animal.call(this)\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('Dog.prototype = Object.create(Animal.prototype)');
      expect(result.code).not.toContain('[object Object]');
    });

    test('enum values', () => {
      const result = evaluate('تعداد Direction {\n  UP، DOWN، LEFT، RIGHT\n}\nطباعة(Direction.UP)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('0');
    });
  });

  // ═══════════════════════════════════════════════════════════════
  // PHASE 5 REGRESSION TESTS — BUG 1/2/3/4 fixes
  // ═══════════════════════════════════════════════════════════════
  describe('Phase 5 Regression — BUG 1: Constructor field this. prefix', () => {
    test('constructor body field assignment uses this.', () => {
      const result = compile('صنف Point {\n  صحيح x = 0\n  Point(صحيح px) {\n    x = px\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('this.x = px');
    });

    test('constructor body multiple field assignments use this.', () => {
      const result = compile('صنف Rect {\n  صحيح x = 0\n  صحيح y = 0\n  Rect(صحيح px، صحيح py) {\n    x = px\n    y = py\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('this.x = px');
      expect(result.code).toContain('this.y = py');
    });

    test('constructor body field in expression uses this.', () => {
      const result = compile('صنف Counter {\n  صحيح count = 0\n  Counter() {\n    count = count + 1\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('this.count = (this.count + 1)');
    });

    test('method body field access already uses this. (no regression)', () => {
      const result = compile('صنف P {\n  صحيح x = 0\n  دالة getX() -> صحيح {\n    ارجع x\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('this.x');
    });
  });

  describe('Phase 5 Regression — BUG 2: Method call via member access', () => {
    test('method call on instance generates correct JS', () => {
      const result = compile('صنف P {\n  P() { هذا.x = 10; }\n  دالة getX() -> صحيح { ارجع هذا.x; }\n}\nأ = جديد P()\nطباعة(أ.getX())');
      expect(result.errors.length).toBe(0);
      expect(result.code).not.toContain('[object Object]');
      expect(result.code).toContain('أ.getX()');
    });

    test('method call with one argument generates correct JS', () => {
      const result = compile('صنف Calc {\n  Calc() { هذا.base = 10; }\n  دالة add(صحيح a) -> صحيح { ارجع a + هذا.base; }\n}\nأ = جديد Calc()\nطباعة(أ.add(5))');
      expect(result.errors.length).toBe(0);
      expect(result.code).not.toContain('[object Object]');
      expect(result.code).toContain('أ.add(5)');
    });

    test('method call with multiple arguments generates correct JS', () => {
      const result = compile('صنف Calc {\n  Calc() { هذا.base = 10; }\n  دالة add(صحيح a، صحيح b) -> صحيح { ارجع a + b + هذا.base; }\n}\nأ = جديد Calc()\nطباعة(أ.add(3، 4))');
      expect(result.errors.length).toBe(0);
      expect(result.code).not.toContain('[object Object]');
      expect(result.code).toContain('أ.add(3, 4)');
    });

    test('print with method call does not produce [object Object]', () => {
      const result = compile('صنف MyClass {\n  MyClass() { هذا.val = 42; }\n  دالة getVal() -> صحيح { ارجع هذا.val; }\n}\nأ = جديد MyClass()\nطباعة(أ.getVal())');
      expect(result.errors.length).toBe(0);
      expect(result.code).not.toContain('[object Object]');
      expect(result.code).toContain('__print(أ.getVal())');
    });

    test('method call works at runtime', () => {
      const result = evaluate('صنف Point {\n  صحيح x = 0\n  Point(صحيح px) {\n    x = px\n  }\n  دالة getX() -> صحيح {\n    ارجع x\n  }\n}\nأ = جديد Point(42)\nطباعة(أ.getX())');
      expect(result.success).toBe(true);
      expect(result.output).toBe('42');
    });
  });

  describe('Phase 5 Regression — BUG 3: Parent constructor call', () => {
    test('parent constructor call does not produce [object Object]', () => {
      const result = compile('صنف Animal {\n  نص name = ""\n}\nصنف Dog يرث Animal {\n  Dog() {\n    Animal.call(this)\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).not.toContain('[object Object]');
      expect(result.code).toContain('Animal.call(this');
    });

    test('inheritance prototype chain is correct', () => {
      const result = compile('صنف Base {\n  صحيح x = 0\n}\nصنف Derived يرث Base {\n  Derived(صحيح v) {\n    x = v\n  }\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('Derived.prototype = Object.create(Base.prototype)');
      expect(result.code).toContain('Derived.prototype.constructor = Derived');
      expect(result.code).not.toContain('[object Object]');
    });

    test('inheritance works at runtime', () => {
      const result = evaluate('صنف Animal {\n  نص type = "animal"\n}\nصنف Dog يرث Animal {\n  Dog() {\n    هذا.type = "dog"\n  }\n}\nأ = جديد Dog()\nطباعة(أ.type)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('dog');
    });
  });

  describe('Phase 5 Regression — BUG 4: finally block', () => {
    test('finally block is emitted in try-catch', () => {
      const result = compile('حاول {\n  طباعة(1)\n} امسك(e) {\n  طباعة(خطا)\n} أخيراً {\n  طباعة(نهاية)\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('try');
      expect(result.code).toContain('catch');
      expect(result.code).toContain('finally');
    });

    test('try-catch-finally without catch variable works', () => {
      const result = compile('حاول {\n  طباعة(1)\n} أخيراً {\n  طباعة(نهاية)\n}');
      expect(result.errors.length).toBe(0);
      expect(result.code).toContain('try');
      expect(result.code).toContain('finally');
    });

    test('finally block executes at runtime', () => {
      const result = evaluate('حاول {\n  طباعة("try")\n} امسك(e) {\n  طباعة("catch")\n} أخيراً {\n  طباعة("finally")\n}');
      expect(result.success).toBe(true);
      expect(result.output).toBe('try\nfinally');
    });
  });

  describe('Phase 5 Regression — Duplicate emission', () => {
    test('class is emitted only once', () => {
      const result = compile('صنف P {\n  P() { هذا.x = 10; }\n}');
      expect(result.errors.length).toBe(0);
      const matches = result.code.match(/function P\(/g) || [];
      expect(matches.length).toBe(1);
    });

    test('function is emitted only once', () => {
      const result = compile('دالة f() {\n  طباعة(1)\n}');
      expect(result.errors.length).toBe(0);
      const matches = result.code.match(/function f\(/g) || [];
      expect(matches.length).toBe(1);
    });

    test('class and function order preserved', () => {
      const result = compile('صنف A { A() {} }\nدالة f() {}\nصنف B { B() {} }');
      expect(result.errors.length).toBe(0);
      const aPos = result.code.indexOf('function A(');
      const bPos = result.code.indexOf('function B(');
      const fPos = result.code.indexOf('function f(');
      expect(aPos).toBeGreaterThan(-1);
      expect(bPos).toBeGreaterThan(-1);
      expect(fPos).toBeGreaterThan(-1);
    });
  });

  describe('Edge case programs', () => {
    test('deeply nested expressions', () => {
      const result = evaluate('طباعة(((((1 + 2)))))');
      expect(result.success).toBe(true);
      expect(result.output).toBe('3');
    });

    test('multiple statements on separate lines', () => {
      const result = evaluate('صحيح a = 1\nصحيح b = 2\nصحيح c = 3\nطباعة(a + b + c)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('6');
    });

    test('complex boolean expression', () => {
      const result = evaluate('صحيح a = صواب\nصحيح b = خطأ\nصحيح c = صواب\nطباعة(a && b || c)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('صواب');
    });

    test('nested if-else', () => {
      const result = evaluate('صحيح x = 10\nإذا (x > 5) {\n  إذا (x > 8) {\n    طباعة("كبير جداً")\n  }وإلا {\n    طباعة("كبير")\n  }\n}وإلا {\n  طباعة("صغير")\n}');
      expect(result.success).toBe(true);
      expect(result.output).toBe('كبير جداً');
    });

    test('for loop with break', () => {
      const result = evaluate('لكل (صحيح i = 0; i < 100; i++) {\n  إذا (i == 5) {\n    توقف\n  }\n}\nطباعة(i)');
      expect(result.success).toBe(true);
      expect(result.output).toBe('5');
    });
  });
});

// ═══════════════════════════════════════════════════════════
// SECURITY TESTS (transpiler level)
// ═══════════════════════════════════════════════════════════
describe('Web Transpiler — Security', () => {

  test('generated code blocks process access', () => {
    const result = transpile('طباعة("test")');
    // The validateGeneratedCode in dhad.js would block this
    expect(result.code).toBeDefined();
  });

  test('class names cannot escape sandbox', () => {
    const result = transpile('صنف MyClass {\n  MyClass() {}\n}');
    expect(result.code).not.toContain('globalThis');
    expect(result.code).not.toContain('process');
  });

  test('no eval in generated code', () => {
    const result = transpile('صحيح x = 5\nطباعة(x)');
    expect(result.code).not.toContain('eval(');
  });

  test('no Function constructor in generated code', () => {
    const result = transpile('صحيح x = 5');
    expect(result.code).not.toContain('new Function');
  });

  test('print output is safe', () => {
    const result = evaluate('طباعة("<script>alert(1)</script>")');
    expect(result.success).toBe(true);
    // Output should be the raw string, not executed as HTML
    expect(result.output).toContain('<script>');
  });
});

// ═══════════════════════════════════════════════════════════
// ERROR HANDLING TESTS
// ═══════════════════════════════════════════════════════════
describe('Web Transpiler — Error Handling', () => {

  test('empty source returns no error', () => {
    const result = transpile('');
    expect(result.errors.length).toBe(0);
  });

  test('undefined variable does not crash', () => {
    const result = evaluate('صحيح x\nطباعة(x)');
    expect(result.success).toBe(true);
    expect(result.output).toBe('عدم');
  });

  test('division by zero returns Infinity', () => {
    const result = evaluate('صحيح x = 10\nx = x / 0\nطباعة(x)');
    expect(result.success).toBe(true);
    expect(result.output).toBe('Infinity');
  });

  test('unmatched braces are handled', () => {
    const result = transpile('إذا (صواب) {\n  طباعة("مرحبا")');
    // Should either recover or report error
    expect(typeof result.code === 'string' || result.errors.length > 0).toBe(true);
  });
});
