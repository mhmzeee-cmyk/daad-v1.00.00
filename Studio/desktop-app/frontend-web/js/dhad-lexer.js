// ═══════════════════════════════════════════════════════════════════════════════
// Dhad (ض) Language — Lexer Tokenizer (v2)
// UTF-16 native tokenizer — works with JavaScript string encoding
// ═══════════════════════════════════════════════════════════════════════════════

var DhadLexer = (function () {
  'use strict';

  // ── Token Types ─────────────────────────────────────────────────────────────
  var TT = {
    EOF: 'EOF',
    UNKNOWN: 'UNKNOWN',
    IDENTIFIER: 'IDENTIFIER',
    NUMBER: 'NUMBER',
    STRING: 'STRING',

    // Keywords (103 total: 83 base + 20 image)
    KW_TRUE: 'KW_TRUE',         // صواب
    KW_FALSE: 'KW_FALSE',       // خطأ
    KW_NULL: 'KW_NULL',         // عدم
    KW_INT: 'KW_INT',           // صحيح
    KW_DOUBLE: 'KW_DOUBLE',     // عشري
    KW_BOOL: 'KW_BOOL',         // منطقي
    KW_CHAR: 'KW_CHAR',         // حرف
    KW_STRING: 'KW_STRING',     // نص
    KW_VOID: 'KW_VOID',         // فراغ
    KW_AUTO: 'KW_AUTO',         // تلقائي
    KW_CONST: 'KW_CONST',       // ثابت
    KW_TYPEDEF: 'KW_TYPEDEF',   // عرّف
    KW_IF: 'KW_IF',             // إذا
    KW_ELSE: 'KW_ELSE',         // وإلا
    KW_WHILE: 'KW_WHILE',       // طالما
    KW_FOR: 'KW_FOR',           // لكل
    KW_DO: 'KW_DO',             // افعل
    KW_SWITCH: 'KW_SWITCH',     // اختر
    KW_CASE: 'KW_CASE',         // حالة
    KW_DEFAULT: 'KW_DEFAULT',   // افتراضي
    KW_BREAK: 'KW_BREAK',       // توقف
    KW_CONTINUE: 'KW_CONTINUE', // استمر
    KW_GOTO: 'KW_GOTO',         // انتقل
    KW_RETURN: 'KW_RETURN',     // ارجع
    KW_THROW: 'KW_THROW',       // ارمِ
    KW_TRY: 'KW_TRY',           // حاول
    KW_CATCH: 'KW_CATCH',       // امسك
    KW_FINALLY: 'KW_FINALLY',   // أخيراً
    KW_CLASS: 'KW_CLASS',       // صنف
    KW_STRUCT: 'KW_STRUCT',     // هيكل
    KW_ENUM: 'KW_ENUM',         // تعداد
    KW_INTERFACE: 'KW_INTERFACE', // واجهة
    KW_NAMESPACE: 'KW_NAMESPACE', // نطاق
    KW_PUBLIC: 'KW_PUBLIC',     // عام
    KW_PRIVATE: 'KW_PRIVATE',   // خاص
    KW_PROTECTED: 'KW_PROTECTED', // محمي
    KW_INHERIT: 'KW_INHERIT',   // يرث
    KW_SELF: 'KW_SELF',         // ذاتي / هذا
    KW_BASE: 'KW_BASE',         // الأصل
    KW_ABSTRACT: 'KW_ABSTRACT', // مجرد
    KW_FUNCTION: 'KW_FUNCTION', // دالة
    KW_NEW: 'KW_NEW',           // جديد
    KW_DELETE: 'KW_DELETE',     // احذف
    KW_POINTER: 'KW_POINTER',   // مؤشر
    KW_REFERENCE: 'KW_REFERENCE', // مرجع
    KW_STATIC: 'KW_STATIC',     // ساكن
    KW_INLINE: 'KW_INLINE',     // مضمن
    KW_EXTERN: 'KW_EXTERN',     // خارجي
    KW_TEMPLATE: 'KW_TEMPLATE', // قالب
    KW_ASSERT: 'KW_ASSERT',     // تأكد
    KW_SIZEOF: 'KW_SIZEOF',     // حجم_الـ
    KW_TYPEOF: 'KW_TYPEOF',     // نوع_الـ
    KW_SYNC: 'KW_SYNC',         // تزامن
    KW_AWAIT: 'KW_AWAIT',       // انتظر
    KW_THREAD: 'KW_THREAD',     // خيط
    KW_LOCK: 'KW_LOCK',         // احجز
    KW_SHARED: 'KW_SHARED',     // مشترك
    KW_UNIQUE: 'KW_UNIQUE',     // فريد
    KW_IMPORT: 'KW_IMPORT',     // استورد
    KW_EXPORT: 'KW_EXPORT',     // صدّر
    KW_MODULE: 'KW_MODULE',     // وحدة
    KW_IN: 'KW_IN',             // في
    KW_ALTERNATIVE: 'KW_ALTERNATIVE', // بديل
    KW_EXCEPTION: 'KW_EXCEPTION', // استثناء
    KW_PRINT: 'KW_PRINT',       // طباعة
    KW_INPUT: 'KW_INPUT',       // ادخل
    KW_NOT: 'KW_NOT',           // ليس
    KW_INCREMENT: 'KW_INCREMENT', // زد
    KW_DECREMENT: 'KW_DECREMENT', // انقص

    // Coordinate Axes (2) — المحاور
    KW_X: 'KW_X',               // س (المحور السيني)
    KW_Y: 'KW_Y',               // ص (المحور الصادي)

    // Image Processing Keywords (20) — معالجة الصور
    KW_LOAD_IMAGE: 'KW_LOAD_IMAGE',   // حمّل_صورة
    KW_DRAW_IMAGE: 'KW_DRAW_IMAGE',   // ارسم_صورة
    KW_IMAGE_SIZE: 'KW_IMAGE_SIZE',   // حجم_صورة
    KW_SAVE_IMAGE: 'KW_SAVE_IMAGE',   // احفظ_صورة
    KW_CROP_IMAGE: 'KW_CROP_IMAGE',   // قص_صورة
    KW_RESIZE: 'KW_RESIZE',           // غيّر_حجم
    KW_ROTATE_IMAGE: 'KW_ROTATE_IMAGE', // لف_صورة
    KW_FLIP_IMAGE: 'KW_FLIP_IMAGE',   // قلب_صورة
    KW_OPACITY: 'KW_OPACITY',         // شفافية
    KW_FILTER: 'KW_FILTER',           // فلتر
    KW_OVERLAY: 'KW_OVERLAY',         // تراكب
    KW_BACKGROUND: 'KW_BACKGROUND',   // خلفية
    KW_PIXEL: 'KW_PIXEL',             // بكسل
    KW_DRAW: 'KW_DRAW',               // ارسم
    KW_FILL: 'KW_FILL',               // ملء
    KW_RECTANGLE: 'KW_RECTANGLE',     // مستطيل
    KW_CIRCLE: 'KW_CIRCLE',           // دائرة
    KW_LINE: 'KW_LINE',               // خط
    KW_TEXT_ON_CANVAS: 'KW_TEXT_ON_CANVAS', // نص_على_لوحة
    KW_CLEAR: 'KW_CLEAR',             // مسح

    // GUI Keywords (14)
    KW_BUTTON: 'KW_BUTTON',     // زر_أمر
    KW_TEXTFIELD: 'KW_TEXTFIELD', // حقل_نص
    KW_COMBOBOX: 'KW_COMBOBOX', // قائمة_خيارات
    KW_IMAGE: 'KW_IMAGE',       // صورة
    KW_CHECKBOX: 'KW_CHECKBOX', // مربع_اختيار
    KW_SLIDER: 'KW_SLIDER',     // شريط_تمرير
    KW_DROPDOWN: 'KW_DROPDOWN', // قائمة_منسدلة
    KW_PANEL: 'KW_PANEL',       // لوحة
    KW_LABEL: 'KW_LABEL',       // تسمية
    KW_COLUMN: 'KW_COLUMN',     // عمود
    KW_ROW: 'KW_ROW',           // صف
    KW_GRID: 'KW_GRID',         // شبكة
    KW_PROGRESSBAR: 'KW_PROGRESSBAR', // شريط_تلوين
    KW_TABBAR: 'KW_TABBAR',     // علامة_تبويب

    // Operators
    PLUS: 'PLUS',               // +
    MINUS: 'MINUS',             // -
    STAR: 'STAR',               // *
    SLASH: 'SLASH',             // /
    PERCENT: 'PERCENT',         // %
    ASSIGN: 'ASSIGN',           // =
    EQ: 'EQ',                   // ==
    NEQ: 'NEQ',                 // !=
    LT: 'LT',                   // <
    GT: 'GT',                   // >
    LTE: 'LTE',                 // <=
    GTE: 'GTE',                 // >=
    AND: 'AND',                 // &&
    OR: 'OR',                   // ||
    NOT: 'NOT',                 // !
    PLUS_ASSIGN: 'PLUS_ASSIGN', // +=
    MINUS_ASSIGN: 'MINUS_ASSIGN', // -=
    STAR_ASSIGN: 'STAR_ASSIGN', // *=
    SLASH_ASSIGN: 'SLASH_ASSIGN', // /=
    PLUS_PLUS: 'PLUS_PLUS',     // ++
    MINUS_MINUS: 'MINUS_MINUS', // --
    ARROW: 'ARROW',             // ->
    POWER: 'POWER',             // ^
    POWER_ASSIGN: 'POWER_ASSIGN', // ^=

    // Arabic Logical Operators
    AND_ARABIC: 'AND_ARABIC',   // و
    OR_ARABIC: 'OR_ARABIC',     // أو

    // Punctuation
    LPAREN: 'LPAREN',           // (
    RPAREN: 'RPAREN',           // )
    LBRACE: 'LBRACE',           // {
    RBRACE: 'RBRACE',           // }
    LBRACKET: 'LBRACKET',       // [
    RBRACKET: 'RBRACKET',       // ]
    SEMICOLON: 'SEMICOLON',     // ;
    COLON: 'COLON',             // :
    DOT: 'DOT',                 // .
    COMMA: 'COMMA',             // ,
    QUESTION: 'QUESTION',       // ?
    AMPERSAND: 'AMPERSAND',     // &
    DOUBLE_COLON: 'DOUBLE_COLON', // ::
    AT: 'AT',                   // @
    HASH: 'HASH',               // #
  };

  // ── Arabic Keyword Map (103 keywords: 83 base + 20 image) ──────────────────
  var KEYWORDS = {
    // Data Types (12)
    'صحيح':     TT.KW_INT,
    'عشري':     TT.KW_DOUBLE,
    'منطقي':     TT.KW_BOOL,
    'حرف':       TT.KW_CHAR,
    'نص':       TT.KW_STRING,
    'فراغ':     TT.KW_VOID,
    'تلقائي':   TT.KW_AUTO,
    'ثابت':     TT.KW_CONST,
    'عرّف':     TT.KW_TYPEDEF,
    'صواب':     TT.KW_TRUE,
    'خطأ':      TT.KW_FALSE,
    'عدم':      TT.KW_NULL,

    // Control Flow (12)
    'إذا':       TT.KW_IF,
    'اذا':       TT.KW_IF,
    'وإلا':     TT.KW_ELSE,
    'طالما':     TT.KW_WHILE,
    'بينما':    TT.KW_WHILE,    // بديل — يتوافق مع Desktop
    'لكل':      TT.KW_FOR,
    'كرر':      TT.KW_FOR,      // بديل — يتوافق مع Desktop
    'افعل':      TT.KW_DO,
    'اختر':      TT.KW_SWITCH,
    'حالة':      TT.KW_CASE,
    'افتراضي':  TT.KW_DEFAULT,
    'توقف':      TT.KW_BREAK,
    'اكسر':      TT.KW_BREAK,
    'استمر':     TT.KW_CONTINUE,
    'تابع':      TT.KW_CONTINUE,
    'انتقل':     TT.KW_GOTO,
    'بديل':      TT.KW_ALTERNATIVE,
    'زد':        TT.KW_INCREMENT,   // بديل — يتوافق مع Desktop
    'انقص':      TT.KW_DECREMENT,   // بديل — يتوافق مع Desktop

    // OOP & Scoping (12)
    'صنف':       TT.KW_CLASS,
    'فئة':      TT.KW_CLASS,    // بديل — يتوافق مع Desktop
    'هيكل':     TT.KW_STRUCT,
    'تعداد':     TT.KW_ENUM,
    'واجهة':     TT.KW_INTERFACE,
    'نطاق':     TT.KW_NAMESPACE,
    'عام':      TT.KW_PUBLIC,
    'خاص':      TT.KW_PRIVATE,
    'محمي':     TT.KW_PROTECTED,
    'يرث':      TT.KW_INHERIT,
    'هذا':      TT.KW_SELF,
    'ذاتي':     TT.KW_SELF,     // بديل — يتوافق مع Desktop
    'الأصل':     TT.KW_BASE,
    'مجرّد':     TT.KW_ABSTRACT,

    // Functions & Memory (10)
    'دالة':      TT.KW_FUNCTION,
    'ارجع':     TT.KW_RETURN,
    'جديد':     TT.KW_NEW,
    'احذف':     TT.KW_DELETE,
    'مؤشر':     TT.KW_POINTER,
    'مرجع':     TT.KW_REFERENCE,
    'ساكن':     TT.KW_STATIC,
    'مضمن':     TT.KW_INLINE,
    'خارجي':     TT.KW_EXTERN,
    'قالب':     TT.KW_TEMPLATE,

    // Error Handling (8)
    'حاول':      TT.KW_TRY,
    'امسك':     TT.KW_CATCH,
    'أمسك':     TT.KW_CATCH,
    'أخيراً':   TT.KW_FINALLY,
    'ارمِ':     TT.KW_THROW,
    'تأكد':     TT.KW_ASSERT,
    'استثناء':  TT.KW_EXCEPTION,
    'نوع_الـ':  TT.KW_TYPEOF,
    'حجم_الـ':  TT.KW_SIZEOF,

    // Concurrency & Modules (10)
    'تزامن':    TT.KW_SYNC,
    'انتظر':    TT.KW_AWAIT,
    'بانتظار':  TT.KW_AWAIT,
    'خيط':      TT.KW_THREAD,
    'احجز':     TT.KW_LOCK,
    'مشترك':    TT.KW_SHARED,
    'فريد':     TT.KW_UNIQUE,
    'استورد':   TT.KW_IMPORT,
    'صدّر':     TT.KW_EXPORT,
    'وحدة':     TT.KW_MODULE,
    'في':       TT.KW_IN,

    // Special (4)
    'طباعة':    TT.KW_PRINT,
    'اطبع':     TT.KW_PRINT,
    'ادخل':     TT.KW_INPUT,
    'ليس':      TT.KW_NOT,
    'و':        TT.AND_ARABIC,
    'أو':       TT.OR_ARABIC,

    // Coordinate Axes (2)
    'س':        TT.KW_X,
    'ص':        TT.KW_Y,

    // GUI (14)
    'زر_أمر':      TT.KW_BUTTON,
    'حقل_نص':      TT.KW_TEXTFIELD,
    'قائمة_خيارات': TT.KW_COMBOBOX,
    'صورة':        TT.KW_IMAGE,
    'مربع_اختيار': TT.KW_CHECKBOX,
    'شريط_تمرير': TT.KW_SLIDER,
    'قائمة_منسدلة': TT.KW_DROPDOWN,
    'لوحة':        TT.KW_PANEL,
    'تسمية':       TT.KW_LABEL,
    'عمود':        TT.KW_COLUMN,
    'صف':          TT.KW_ROW,
    'شبكة':        TT.KW_GRID,
    'شريط_تلوين': TT.KW_PROGRESSBAR,
    'علامة_تبويب': TT.KW_TABBAR,

    // Image Processing (20) — صيغ مفاتيح معالجة الصور
    'حمّل_صورة':    TT.KW_LOAD_IMAGE,     // load_image
    'ارسم_صورة':    TT.KW_DRAW_IMAGE,     // draw_image
    'حجم_صورة':     TT.KW_IMAGE_SIZE,     // image_size
    'احفظ_صورة':    TT.KW_SAVE_IMAGE,     // save_image
    'قص_صورة':      TT.KW_CROP_IMAGE,     // crop_image
    'غيّر_حجم':     TT.KW_RESIZE,         // resize
    'لف_صورة':      TT.KW_ROTATE_IMAGE,   // rotate_image
    'قلب_صورة':     TT.KW_FLIP_IMAGE,     // flip_image
    'شفافية':       TT.KW_OPACITY,        // opacity
    'فلتر':         TT.KW_FILTER,         // filter
    'تراكب':        TT.KW_OVERLAY,        // overlay
    'خلفية':        TT.KW_BACKGROUND,     // background
    'بكسل':         TT.KW_PIXEL,          // pixel
    'ارسم':         TT.KW_DRAW,           // draw
    'ملء':          TT.KW_FILL,           // fill
    'مستطيل':       TT.KW_RECTANGLE,      // rectangle
    'دائرة':        TT.KW_CIRCLE,         // circle
    'خط':           TT.KW_LINE,           // line
    'نص_على_لوحة':  TT.KW_TEXT_ON_CANVAS, // text_on_canvas
    'مسح':          TT.KW_CLEAR,          // clear
  };

  // ── Lexer ───────────────────────────────────────────────────────────────────
  function Lexer(source) {
    this.source = source;
    this.pos = 0;
    this.line = 1;
    this.col = 1;
    this.tokens = [];
    this.errors = [];
  }

  // ── Character Helpers (UTF-16 native) ───────────────────────────────────────
  // JS strings are UTF-16: charCodeAt(pos) gives the codepoint for BMP chars
  // (which includes all Arabic: U+0600–U+06FF)
  function peek(L, offset) {
    var p = L.pos + (offset || 0);
    if (p >= L.source.length) return -1;
    return L.source.charCodeAt(p);
  }

  // Handle surrogate pairs for characters outside BMP (emoji, etc.)
  function peekCodepoint(L, offset) {
    var p = L.pos + (offset || 0);
    if (p >= L.source.length) return -1;
    var hi = L.source.charCodeAt(p);
    if (hi >= 0xD800 && hi <= 0xDBFF && p + 1 < L.source.length) {
      var lo = L.source.charCodeAt(p + 1);
      if (lo >= 0xDC00 && lo <= 0xDFFF) {
        return ((hi - 0xD800) << 10) + (lo - 0xDC00) + 0x10000;
      }
    }
    return hi;
  }

  function advance(L, n) {
    n = n || 1;
    for (var i = 0; i < n; i++) {
      if (L.pos < L.source.length) {
        var c = L.source.charCodeAt(L.pos);
        if (c === 10) { // newline
          L.line++;
          L.col = 1;
        } else {
          L.col++;
        }
        // Skip surrogate pair second half
        if (c >= 0xD800 && c <= 0xDBFF && L.pos + 1 < L.source.length) {
          var lo = L.source.charCodeAt(L.pos + 1);
          if (lo >= 0xDC00 && lo <= 0xDFFF) {
            L.pos++; // skip the low surrogate too
            L.col++;
          }
        }
        L.pos++;
      }
    }
  }

  function currentChar(L) {
    if (L.pos >= L.source.length) return '';
    return L.source[L.pos];
  }

  function isArabic(cp) {
    return (cp >= 0x0600 && cp <= 0x06FF) ||
           (cp >= 0x0750 && cp <= 0x077F) ||
           (cp >= 0xFB50 && cp <= 0xFDFF) ||
           (cp >= 0xFE70 && cp <= 0xFEFF);
  }

  // علامات الترقيم العربية يجب ألا تُعتبر حروف معرّفات:
  // ، U+060C فاصلة | ؛ U+061B فاصلة منقوطة | ؟ U+061F استفهام
  function isArabicPunctuation(cp) {
    return cp === 0x060C || cp === 0x061B || cp === 0x061F;
  }

  function isLetter(cp) {
    return ((cp >= 65 && cp <= 90) ||
           (cp >= 97 && cp <= 122) ||
           isArabic(cp) ||
           cp === 95) && !isArabicPunctuation(cp); // underscore
  }

  function isDigit(cp) {
    return cp >= 48 && cp <= 57;
  }

  function isHexDigit(cp) {
    return (cp >= 48 && cp <= 57) ||
           (cp >= 65 && cp <= 70) ||
           (cp >= 97 && cp <= 102);
  }

  // ── Token Creation ──────────────────────────────────────────────────────────
  function makeToken(type, value, line, col) {
    return { type: type, value: value, line: line, col: col };
  }

  // ── Read identifier/keyword ─────────────────────────────────────────────────
  // Identifiers: Arabic letters, Latin letters, digits, underscore
  function readIdentifier(L) {
    var start = L.pos;
    var startCol = L.col;
    var startLine = L.line;

    // Read first character (must be letter or _)
    var first = peek(L);
    if (isLetter(first)) {
      advance(L);
    } else {
      return null;
    }

    // Read rest (letter or digit)
    while (L.pos < L.source.length) {
      var c = peek(L);
      if (isLetter(c) || isDigit(c)) {
        advance(L);
      } else {
        break;
      }
    }

    var word = L.source.substring(start, L.pos);

    // Check if keyword
    if (KEYWORDS.hasOwnProperty(word)) {
      return makeToken(KEYWORDS[word], word, startLine, startCol);
    }
    return makeToken(TT.IDENTIFIER, word, startLine, startCol);
  }

  // ── Read number ─────────────────────────────────────────────────────────────
  function readNumber(L) {
    var start = L.pos;
    var startCol = L.col;
    var startLine = L.line;
    var numStr = '';

    // Hex: 0x...
    if (peek(L) === 48 && peek(L, 1) === 120) { // 0x
      numStr = '0x';
      advance(L, 2);
      while (isHexDigit(peek(L))) {
        numStr += String.fromCharCode(peek(L));
        advance(L);
      }
    } else {
      // Decimal
      while (isDigit(peek(L))) {
        numStr += String.fromCharCode(peek(L));
        advance(L);
      }
      // Float
      if (peek(L) === 46 && isDigit(peek(L, 1))) { // .
        numStr += '.';
        advance(L);
        while (isDigit(peek(L))) {
          numStr += String.fromCharCode(peek(L));
          advance(L);
        }
      }
      // Scientific: e.g. 1e10, 1.5e-3
      var cc = peek(L);
      if (cc === 101 || cc === 69) { // e or E
        numStr += String.fromCharCode(cc);
        advance(L);
        cc = peek(L);
        if (cc === 43 || cc === 45) { // + or -
          numStr += String.fromCharCode(cc);
          advance(L);
        }
        while (isDigit(peek(L))) {
          numStr += String.fromCharCode(peek(L));
          advance(L);
        }
      }
    }

    var val = numStr.indexOf('.') >= 0 || numStr.indexOf('e') >= 0 || numStr.indexOf('E') >= 0
      ? parseFloat(numStr)
      : (numStr.indexOf('0x') === 0 ? parseInt(numStr, 16) : parseInt(numStr, 10));

    return makeToken(TT.NUMBER, String(val), startLine, startCol);
  }

  // ── Read string ─────────────────────────────────────────────────────────────
  function readString(L, quote) {
    var startLine = L.line;
    var startCol = L.col;
    var str = '';
    advance(L); // skip opening quote

    while (L.pos < L.source.length) {
      var c = peek(L);
      if (c === quote) { // closing quote
        advance(L);
        return makeToken(TT.STRING, str, startLine, startCol);
      }
      if (c === 92) { // backslash
        advance(L);
        var esc = peek(L);
        if (esc === 110) str += '\n';       // \n
        else if (esc === 116) str += '\t';   // \t
        else if (esc === 114) str += '\r';   // \r
        else if (esc === 92) str += '\\';    // \\
        else if (esc === quote) str += String.fromCharCode(quote);
        else if (esc === 48) str += '\0';    // \0
        else str += String.fromCharCode(esc);
        advance(L);
      } else if (c === 10) { // newline inside string
        return makeToken(TT.STRING, str, startLine, startCol);
      } else {
        str += String.fromCharCode(c);
        advance(L);
      }
    }

    return makeToken(TT.STRING, str, startLine, startCol);
  }

  // ── Main Tokenize Loop ──────────────────────────────────────────────────────
  Lexer.prototype.tokenize = function () {
    var L = this;
    var src = L.source;

    while (L.pos < src.length) {
      var cp = peek(L);
      var startLine = L.line;
      var startCol = L.col;

      // ── Skip whitespace ───────────────────────────────────────────────────
      if (cp === 32 || cp === 9 || cp === 13) {
        advance(L);
        continue;
      }

      // ── Skip newline ─────────────────────────────────────────────────────
      if (cp === 10) {
        advance(L);
        continue;
      }

      // ── Single-line comment: // ──────────────────────────────────────────
      if (cp === 47 && peek(L, 1) === 47) {
        while (L.pos < src.length && peek(L) !== 10) advance(L);
        continue;
      }

      // ── Multi-line comment: /* ... */ ────────────────────────────────────
      if (cp === 47 && peek(L, 1) === 42) {
        advance(L, 2);
        while (L.pos < src.length) {
          if (peek(L) === 42 && peek(L, 1) === 47) { advance(L, 2); break; }
          advance(L);
        }
        continue;
      }

      // ── Arabic comment: # ────────────────────────────────────────────────
      if (cp === 35) {
        while (L.pos < src.length && peek(L) !== 10) advance(L);
        continue;
      }

      // ── Strings ──────────────────────────────────────────────────────────
      if (cp === 34 || cp === 39) {
        L.tokens.push(readString(L, cp));
        continue;
      }

      // ── Numbers ──────────────────────────────────────────────────────────
      if (isDigit(cp) || (cp === 46 && isDigit(peek(L, 1)))) {
        L.tokens.push(readNumber(L));
        continue;
      }

      // ── Identifier / Keyword ─────────────────────────────────────────────
      if (isLetter(cp)) {
        var tok = readIdentifier(L);
        if (tok) L.tokens.push(tok);
        continue;
      }

      // ── Two-character operators ──────────────────────────────────────────
      var c2 = peek(L, 1);
      if (cp === 61 && c2 === 61) { // ==
        L.tokens.push(makeToken(TT.EQ, '==', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 33 && c2 === 61) { // !=
        L.tokens.push(makeToken(TT.NEQ, '!=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 60 && c2 === 61) { // <=
        L.tokens.push(makeToken(TT.LTE, '<=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 62 && c2 === 61) { // >=
        L.tokens.push(makeToken(TT.GTE, '>=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 38 && c2 === 38) { // &&
        L.tokens.push(makeToken(TT.AND, '&&', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 124 && c2 === 124) { // ||
        L.tokens.push(makeToken(TT.OR, '||', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 43 && c2 === 61) { // +=
        L.tokens.push(makeToken(TT.PLUS_ASSIGN, '+=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 45 && c2 === 61) { // -=
        L.tokens.push(makeToken(TT.MINUS_ASSIGN, '-=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 42 && c2 === 61) { // *=
        L.tokens.push(makeToken(TT.STAR_ASSIGN, '*=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 47 && c2 === 61) { // /=
        L.tokens.push(makeToken(TT.SLASH_ASSIGN, '/=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 43 && c2 === 43) { // ++
        L.tokens.push(makeToken(TT.PLUS_PLUS, '++', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 45 && c2 === 45) { // --
        L.tokens.push(makeToken(TT.MINUS_MINUS, '--', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 45 && c2 === 62) { // ->
        L.tokens.push(makeToken(TT.ARROW, '->', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 94 && c2 === 61) { // ^=
        L.tokens.push(makeToken(TT.POWER_ASSIGN, '^=', startLine, startCol)); advance(L, 2); continue;
      }
      if (cp === 58 && c2 === 58) { // ::
        L.tokens.push(makeToken(TT.DOUBLE_COLON, '::', startLine, startCol)); advance(L, 2); continue;
      }

      // ── Single-character operators/punctuation ───────────────────────────
      var singleMap = {
        43: TT.PLUS, 45: TT.MINUS, 42: TT.STAR, 47: TT.SLASH,
        37: TT.PERCENT, 61: TT.ASSIGN, 60: TT.LT, 62: TT.GT,
        33: TT.NOT, 40: TT.LPAREN, 41: TT.RPAREN,
        123: TT.LBRACE, 125: TT.RBRACE, 91: TT.LBRACKET, 93: TT.RBRACKET,
        59: TT.SEMICOLON, 58: TT.COLON, 46: TT.DOT, 44: TT.COMMA,
        64: TT.AT, 35: TT.HASH, 94: TT.POWER, 63: TT.QUESTION, 38: TT.AMPERSAND
      };

      if (singleMap[cp]) {
        L.tokens.push(makeToken(singleMap[cp], String.fromCharCode(cp), startLine, startCol));
        advance(L);
        continue;
      }

      // الفاصلة العربية (، U+060C) تعامل مثل الفاصلة العادية
      if (cp === 0x060C) {
        L.tokens.push(makeToken(TT.COMMA, '\u060C', startLine, startCol));
        advance(L);
        continue;
      }

      // الفاصلة المنقوطة العربية (؛ U+061B) تعامل مثل الفاصلة المنقوطة العادية
      if (cp === 0x061B) {
        L.tokens.push(makeToken(TT.SEMICOLON, '\u061B', startLine, startCol));
        advance(L);
        continue;
      }

      // ── Unknown ──────────────────────────────────────────────────────────
      L.tokens.push(makeToken(TT.UNKNOWN, String.fromCharCode(cp), startLine, startCol));
      L.errors.push({
        message: 'حرف غير معروف: ' + String.fromCharCode(cp),
        line: startLine,
        col: startCol
      });
      advance(L);
    }

    L.tokens.push(makeToken(TT.EOF, '', L.line, L.col));
    return { tokens: L.tokens, errors: L.errors };
  };

  // ── Exports ─────────────────────────────────────────────────────────────────
  return {
    TT: TT,
    KEYWORDS: KEYWORDS,
    Lexer: Lexer,
    isKeyword: function (type) { return type.indexOf('KW_') === 0; }
  };
})();

if (typeof module !== 'undefined' && module.exports) {
  module.exports = DhadLexer;
}
