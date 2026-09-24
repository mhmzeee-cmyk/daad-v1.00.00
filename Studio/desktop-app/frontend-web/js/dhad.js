// ═══════════════════════════════════════════════════════════════════════════════
// Dhad (ض) Language — Main Interpreter (v2)
// Pipeline: Source → Lexer → Parser → AST → CodeGen → JavaScript
// ═══════════════════════════════════════════════════════════════════════════════

var Dhad = (function () {
  'use strict';

  var Lexer = DhadLexer.Lexer;
  var Parser = DhadParser.Parser;
  var CodeGen = DhadCodeGen.CodeGen;

  // ── Compile: Source → JavaScript code string ────────────────────────────────
  function compile(source) {
    var errors = [];

    // Phase 1: Tokenize
    var lexer = new Lexer(source);
    var lexResult = lexer.tokenize();
    errors = errors.concat(lexResult.errors);

    if (errors.length > 0) {
      return { code: null, errors: errors };
    }

    // Phase 2: Parse
    var parser = new Parser(lexResult.tokens, errors);
    var ast = parser.parseProgram();
    errors = errors.concat(parser.errors);

    if (errors.length > 0) {
      return { code: null, errors: errors };
    }

    // Phase 3: Code Generation
    var codegen = new CodeGen();
    var code = codegen.generate(ast);

    return { code: code, errors: errors, ast: ast };
  }

  // ── Security: Validate generated code before execution ──────────────────────
  // Blocks patterns that could escape the sandbox via prototype chain or globals.
  // This is a defense-in-depth layer; the codegen should never produce these,
  // but we validate to prevent exploits from modified or injected code.
  var DANGEROUS_PATTERNS = [
    /\bprocess\b/,
    /\brequire\s*\(/,
    /\beval\s*\(/,
    /\bFunction\s*\(/,
    /\bfetch\s*\(/,
    /\bXMLHttpRequest\b/,
    /\bWebSocket\b/,
    /\bimport\s*\(/,
    /\bglobalThis\b/,
    /\b__proto__\b/,
    /\bprototype\s*\.\s*constructor\b/,
    /\bObject\s*\.\s*getPrototypeOf\b/,
    /\bObject\s*\.\s*setPrototypeOf\b/,
    /\barguments\s*\.\s*callee\b/,
    /\bwindow\s*\.\s*(parent|top|self)\b/,
    /\bself\s*\.\s*(parent|top)\b/,
    /\bframes\b/,
    /\bopener\b/,
    /\blocation\s*[.=]/,
    /\bdocument\s*\.\s*(cookie|write|writeln)\b/,
    /\blocalStorage\b/,
    /\bsessionStorage\b/,
    /\bIndexedDB\b/,
    /\bindexedDB\b/,
    /\bnavigator\b/,
    /\bwindow\b/,
  ];

  function validateGeneratedCode(code) {
    if (!code || typeof code !== 'string') {
      return { safe: false, reason: 'الكود فارغ' };
    }
    // SECURITY: Normalize code before checking patterns
    // Strip invisible Unicode characters and control chars that break word-boundary regex
    var normalized = code
      .replace(/[\u200B\u200C\u200D\u200E\u200F\u202A-\u202E\u2060-\u206F\uFEFF\u00AD]/g, '')
      .replace(/[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]/g, '')
      .replace(/\u00A0/g, ' ');
    var collapsed = normalized.replace(/[\n\r\t]/g, '');
    for (var i = 0; i < DANGEROUS_PATTERNS.length; i++) {
      if (DANGEROUS_PATTERNS[i].test(normalized) || DANGEROUS_PATTERNS[i].test(collapsed)) {
        return { safe: false, reason: 'نمط غير آمن: ' + DANGEROUS_PATTERNS[i].source };
      }
    }
    return { safe: true };
  }

  // ── Run: Source → Execute in sandbox → Return output ────────────────────────
  function run(source) {
    var compileResult = compile(source);
    if (compileResult.errors.length > 0) {
      return {
        success: false,
        output: null,
        errors: compileResult.errors
      };
    }

    // Security: validate generated code before execution
    var validation = validateGeneratedCode(compileResult.code);
    if (!validation.safe) {
      return {
        success: false,
        output: null,
        errors: [{
          message: 'الكود المُولّد يحتوي على نمط غير آمن: ' + validation.reason,
          line: 0,
          col: 0
        }]
      };
    }

    var output = [];
    var guiWidgets = [];
    var guiEvents = {};

    // Build sandbox
    var sandbox = buildSandbox(output, guiWidgets, guiEvents);

    try {
      var fn = new Function('__print', '__guiPrint', '__array', '__widgets', '__events', '__input', compileResult.code);
      fn(sandbox.print, sandbox.guiPrint, sandbox.array, sandbox.widgets, sandbox.events, sandbox.input);

      return {
        success: true,
        output: output,
        guiWidgets: guiWidgets,
        guiEvents: guiEvents,
        errors: []
      };
    } catch (e) {
      return {
        success: false,
        output: output,
        errors: [{
          message: e.message || 'خطأ غير معروف',
          line: 0,
          col: 0
        }]
      };
    }
  }

  // ── Build Sandbox ───────────────────────────────────────────────────────────
  function buildSandbox(output, guiWidgets, guiEvents) {
    
    return {
      print: function () {
        var args = Array.prototype.slice.call(arguments);
        var s = args.map(function (x) {
          if (x === null) return 'عدم';
          if (x === true) return 'صواب';
          if (x === false) return 'خطأ';
          if (typeof x === 'undefined') return 'عدم';
          return String(x);
        }).join(' ');
        output.push(s);
        if (typeof console !== 'undefined') console.log(s);
      },

      guiPrint: function (text) {
        output.push(text);
        if (typeof document !== 'undefined') {
          var el = document.getElementById('output');
          if (el) el.textContent += text + '\n';
        }
      },

      // ادخل: قراءة قيمة من المستخدم — تحاول التحويل لرقم وإلا ترجع نصًا
      input: function () {
        var raw = null;
        if (typeof prompt === 'function') {
          raw = prompt('ادخل قيمة:');
        }
        if (raw === null) return 0;
        var num = Number(raw);
        return isNaN(num) ? raw : num;
      },

      array: function () {
        return Array.prototype.slice.call(arguments);
      },

      widgets: guiWidgets,
      events: guiEvents
    };
  }

  // ── Translate (for backward compat with old API) ────────────────────────────
  function translate(source) {
    var result = compile(source);
    return {
      code: result.code,
      errors: result.errors
    };
  }

  // ── Exports ─────────────────────────────────────────────────────────────────
  return {
    compile: compile,
    run: run,
    translate: translate
  };
})();

// ── Backward-compatible DhadInterpreter (used by web editor pages) ────────────
var DhadInterpreter = (function () {
  function transpile(code) {
    var result = Dhad.compile(code);
    if (result.errors && result.errors.length > 0) {
      var errMsg = result.errors.map(function(e) {
        return 'خطأ (سطر ' + e.line + '): ' + e.message;
      }).join('\n');
      throw new Error(errMsg);
    }
    return result.code;
  }

  function execute(code) {
    var result = Dhad.run(code);
    if (result.errors && result.errors.length > 0) {
      var errMsg = result.errors.map(function(e) {
        return 'خطأ (سطر ' + e.line + '): ' + e.message;
      }).join('\n');
      throw new Error(errMsg);
    }
    return {
      output: result.output ? result.output.join('\n') : '',
      success: result.success,
      errors: result.errors || [],
      guiWidgets: result.guiWidgets || [],
      guiEvents: result.guiEvents || {}
    };
  }

  function highlight(code) {
    if (typeof DhadHighlight !== 'undefined') return DhadHighlight(code);
    // SECURITY: Escape HTML to prevent XSS when highlight library unavailable
    if (typeof escapeHtml === 'function') return escapeHtml(code);
    var s = String(code);
    var map = { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#039;', '`': '&#96;' };
    return s.replace(/[&<>"'`]/g, function(ch) { return map[ch]; });
  }

  return {
    transpile: transpile,
    execute: execute,
    highlight: highlight
  };
})();

// Export for Node.js
if (typeof module !== 'undefined' && module.exports) {
  module.exports = Dhad;
  module.exports.DhadInterpreter = DhadInterpreter;
  // Make DhadInterpreter globally available (matches browser behavior)
  if (typeof global !== 'undefined') global.DhadInterpreter = DhadInterpreter;
}
