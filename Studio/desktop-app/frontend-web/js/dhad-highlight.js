// ═══════════════════════════════════════════════════════════════════════════════
// Dhad (ض) Language — Syntax Highlighting
// Python-like color scheme for dark themes
// ═══════════════════════════════════════════════════════════════════════════════

var DhadHighlight = (function () {
  'use strict';

  // ── Arabic keywords list (grouped by category) ────────────────────────────
  var TYPE_KEYWORDS = [
    'صحيح', 'عشري', 'منطقي', 'حرف', 'نص', 'فراغ', 'تلقائي', 'ثابت', 'عرّف',
    'صنف', 'هيكل', 'تعداد', 'واجهة', 'نطاق', 'قالب', 'دالة', 'قائمة'
  ];

  var CONTROL_KEYWORDS = [
    'إذا', 'وإلا', 'طالما', 'لكل', 'افعل', 'اختر', 'حالة', 'افتراضي',
    'اكسر', 'توقف', 'استمر', 'تابع', 'انتقل', 'ارجع',
    'حاول', 'امسك', 'أخيراً', 'ارمِ', 'تأكد',
    'استورد', 'صدّر', 'جديد', 'احذف', 'مؤشر', 'مرجع',
    'ساكن', 'مضمن', 'خارجي', 'مجرد', 'يرث',
    'تزامن', 'انتظر', 'بانتظار', 'خيط', 'احجز', 'مشترك', 'فريد',
    'حجم_الـ', 'نوع_الـ', 'في', 'بديل', 'استثناء'
  ];

  var ACCESS_KEYWORDS = ['عام', 'خاص', 'محمي'];
  var SELF_KEYWORDS = ['هذا', 'ذاتي', 'الأصل'];
  var BOOLEAN_KEYWORDS = ['صواب', 'خطأ', 'عدم'];
  var ARABIC_LOGIC = ['و', 'أو', 'ليس'];
  var PRINT_KEYWORD = ['طباعة'];

  var GUI_KEYWORDS = [
    'زر_أمر', 'حقل_نص', 'قائمة_خيارات', 'صورة', 'مربع_اختيار',
    'شريط_تمرير', 'قائمة_منسدلة', 'لوحة', 'تسمية', 'عمود', 'صف',
    'شبكة', 'شريط_تلوين', 'علامة_تبويب'
  ];

  var IMAGE_KEYWORDS = [
    'حمّل_صورة', 'ارسم_صورة', 'حجم_صورة', 'احفظ_صورة', 'قص_صورة',
    'غيّر_حجم', 'لف_صورة', 'قلب_صورة', 'شفافية', 'فلتر',
    'تراكب', 'خلفية', 'بكسل', 'ارسم', 'ملء',
    'مستطيل', 'دائرة', 'خط', 'نص_على_لوحة', 'مسح'
  ];

  // Build keyword sets for fast lookup
  var ALL_KEYWORDS = {};
  TYPE_KEYWORDS.forEach(function(k) { ALL_KEYWORDS[k] = 'type'; });
  CONTROL_KEYWORDS.forEach(function(k) { ALL_KEYWORDS[k] = 'control'; });
  ACCESS_KEYWORDS.forEach(function(k) { ALL_KEYWORDS[k] = 'access'; });
  SELF_KEYWORDS.forEach(function(k) { ALL_KEYWORDS[k] = 'self'; });
  BOOLEAN_KEYWORDS.forEach(function(k) { ALL_KEYWORDS[k] = 'bool'; });
  ARABIC_LOGIC.forEach(function(k) { ALL_KEYWORDS[k] = 'logic'; });
  PRINT_KEYWORD.forEach(function(k) { ALL_KEYWORDS[k] = 'print'; });
  GUI_KEYWORDS.forEach(function(k) { ALL_KEYWORDS[k] = 'gui'; });
  IMAGE_KEYWORDS.forEach(function(k) { ALL_KEYWORDS[k] = 'image'; });

  // ── Escape HTML ───────────────────────────────────────────────────────────
  function esc(text) {
    return text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
  }

  // ── Wrap text in a span with class ────────────────────────────────────────
  function wrap(text, cls) {
    return '<span class="' + cls + '">' + esc(text) + '</span>';
  }

  // ── Highlight a single line of Dhad code ──────────────────────────────────
  function highlightLine(line) {
    var result = '';
    var i = 0;
    var len = line.length;

    while (i < len) {
      var ch = line[i];

      // ── Arabic comment: // (but check it's not inside a string) ───
      // Skip — comments start with // in Dhad

      // ── String literal: "..." or '...' ────────────────────────────
      if (ch === '"' || ch === "'") {
        var quote = ch;
        var str = ch;
        i++;
        while (i < len && line[i] !== quote) {
          if (line[i] === '\\' && i + 1 < len) {
            str += line[i] + line[i + 1];
            i += 2;
          } else {
            str += line[i];
            i++;
          }
        }
        if (i < len) {
          str += line[i];
          i++;
        }
        result += wrap(str, 'hl-str');
        continue;
      }

      // ── Number literal ────────────────────────────────────────────
      if (ch >= '0' && ch <= '9') {
        var num = '';
        while (i < len && ((line[i] >= '0' && line[i] <= '9') || line[i] === '.')) {
          num += line[i];
          i++;
        }
        result += wrap(num, 'hl-num');
        continue;
      }

      // ── Arabic word (potential keyword or identifier) ──────────────
      if (ch >= '\u0600' && ch <= '\u06FF' || ch >= '\u0750' && ch <= '\u077F' ||
          ch >= '\uFB50' && ch <= '\uFDFF' || ch >= '\uFE70' && ch <= '\uFEFF') {
        var word = '';
        while (i < len) {
          var c = line[i];
          if ((c >= '\u0600' && c <= '\u06FF') || (c >= '\u0750' && c <= '\u077F') ||
              (c >= '\uFB50' && c <= '\uFDFF') || (c >= '\uFE70' && c <= '\uFEFF') ||
              (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c === '_') {
            word += c;
            i++;
          } else {
            break;
          }
        }

        // Check if it's a keyword
        var kwType = ALL_KEYWORDS[word];
        if (kwType) {
          switch (kwType) {
            case 'type':    result += wrap(word, 'hl-type'); break;
            case 'control': result += wrap(word, 'hl-kw'); break;
            case 'access':  result += wrap(word, 'hl-kw'); break;
            case 'self':    result += wrap(word, 'hl-self'); break;
            case 'bool':    result += wrap(word, 'hl-bool'); break;
            case 'logic':   result += wrap(word, 'hl-kw'); break;
            case 'print':   result += wrap(word, 'hl-kw'); break;
            case 'gui':     result += wrap(word, 'hl-gui'); break;
            case 'image':   result += wrap(word, 'hl-image'); break;
            default:        result += wrap(word, 'hl-id');
          }
        } else {
          // Check if followed by ( → function call
          var lookAhead = i;
          while (lookAhead < len && line[lookAhead] === ' ') lookAhead++;
          if (lookAhead < len && line[lookAhead] === '(') {
            result += wrap(word, 'hl-func');
          } else {
            result += wrap(word, 'hl-id');
          }
        }
        continue;
      }

      // ── Latin identifier ──────────────────────────────────────────
      if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch === '_') {
        var ident = '';
        while (i < len && ((line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z') || (line[i] >= '0' && line[i] <= '9') || line[i] === '_')) {
          ident += line[i];
          i++;
        }
        // Check if followed by ( → function call
        var lookAhead2 = i;
        while (lookAhead2 < len && line[lookAhead2] === ' ') lookAhead2++;
        if (lookAhead2 < len && line[lookAhead2] === '(') {
          result += wrap(ident, 'hl-func');
        } else {
          result += wrap(ident, 'hl-id');
        }
        continue;
      }

      // ── Multi-char operators ──────────────────────────────────────
      if (i + 1 < len) {
        var two = line[i] + line[i + 1];
        if (two === '==' || two === '!=' || two === '<=' || two === '>=' ||
            two === '&&' || two === '||' || two === '+=' || two === '-=' ||
            two === '*=' || two === '/=' || two === '++' || two === '--' ||
            two === '->' || two === '^=' || two === '::') {
          result += wrap(two, 'hl-op');
          i += 2;
          continue;
        }
      }

      // ── Single-char operators ─────────────────────────────────────
      if (ch === '+' || ch === '-' || ch === '*' || ch === '/' || ch === '%' ||
          ch === '=' || ch === '<' || ch === '>' || ch === '!' || ch === '^' || ch === '&') {
        result += wrap(ch, 'hl-op');
        i++;
        continue;
      }

      // ── Punctuation (different colors per type) ───────────────────
      if (ch === '(' || ch === ')') {
        result += wrap(ch, 'hl-punc-paren');
        i++;
        continue;
      }
      if (ch === '{' || ch === '}') {
        result += wrap(ch, 'hl-punc-brace');
        i++;
        continue;
      }
      if (ch === '[' || ch === ']') {
        result += wrap(ch, 'hl-punc-bracket');
        i++;
        continue;
      }
      if (ch === ';' || ch === ',' || ch === ':' || ch === '.') {
        result += wrap(ch, 'hl-punc-semi');
        i++;
        continue;
      }
      if (ch === '?' || ch === '@' || ch === '#') {
        result += wrap(ch, 'hl-punc');
        i++;
        continue;
      }

      // ── Whitespace / other ────────────────────────────────────────
      result += esc(ch);
      i++;
    }

    return result;
  }

  // ── Main highlight function ────────────────────────────────────────────────
  function highlight(code) {
    if (!code || code.length === 0) return '';

    var lines = code.split('\n');
    var html = '';

    for (var i = 0; i < lines.length; i++) {
      var line = lines[i];

      // Check for single-line comment: //
      var commentIdx = line.indexOf('//');
      if (commentIdx >= 0) {
        // Check if // is inside a string
        var inString = false;
        var stringChar = '';
        for (var j = 0; j < commentIdx; j++) {
          if ((line[j] === '"' || line[j] === "'") && (j === 0 || line[j - 1] !== '\\')) {
            if (!inString) {
              inString = true;
              stringChar = line[j];
            } else if (line[j] === stringChar) {
              inString = false;
            }
          }
        }
        if (!inString) {
          var codePart = line.substring(0, commentIdx);
          var commentPart = line.substring(commentIdx);
          html += highlightLine(codePart) + wrap(commentPart, 'hl-comment');
        } else {
          html += highlightLine(line);
        }
      } else {
        html += highlightLine(line);
      }

      if (i < lines.length - 1) {
        html += '\n';
      }
    }

    return html;
  }

  return highlight;
})();

if (typeof module !== 'undefined' && module.exports) {
  module.exports = DhadHighlight;
}
