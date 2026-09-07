/**
 * DAAD Web Sandbox — Adversarial Security Tests
 * Tests validateGeneratedCode() against bypass attempts
 */

const DANGEROUS_PATTERNS = [
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
  /\bparent\b/,
  /\btop\b/,
  /\bself\b/,
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
  // Additional defense-in-depth patterns
  /\bconstructor\s*\[/,      // bracket notation constructor access
  /\bconstructor\s*\.\s*constructor\b/, // constructor.constructor escape
  /\["constructor"\]/,       // bracket notation ["constructor"]
  /\['constructor'\]/,       // bracket notation ['constructor']
];

function validateGeneratedCode(code) {
  if (!code || typeof code !== 'string') return { safe: false, reason: 'empty' };
  // Normalize: strip invisible Unicode chars + control chars that break \b word boundaries
  var normalized = code
    .replace(/[\u200B\u200C\u200D\u200E\u200F\u202A-\u202E\u2060-\u206F\uFEFF\u00AD\uFFF0-\uFFFD]/g, '')
    .replace(/[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]/g, '')
    .replace(/\u00A0/g, ' ');
  var collapsed = normalized.replace(/[\n\r\t]/g, '');
  for (var i = 0; i < DANGEROUS_PATTERNS.length; i++) {
    if (DANGEROUS_PATTERNS[i].test(normalized) || DANGEROUS_PATTERNS[i].test(collapsed)) {
      return { safe: false, reason: DANGEROUS_PATTERNS[i].source };
    }
  }
  return { safe: true };
}

const results = [];
let pass = 0, fail = 0;

// test(id, code, expectBlocked, category, description)
function test(id, code, expectBlocked, cat, desc) {
  const r = validateGeneratedCode(code);
  const blocked = !r.safe;
  const ok = blocked === expectBlocked;
  if (ok) pass++; else fail++;
  results.push({ id, cat, desc, expectBlocked, blocked, ok, reason: r.reason });
  if (!ok) console.log(`  FAIL ${id}: [${cat}] ${desc} — expected ${expectBlocked ? 'BLOCKED' : 'PASS'}, got ${blocked ? 'BLOCKED' : 'PASS'}`);
}

// ════════════════════ CATEGORY 1: Direct Keywords ════════════════════
console.log('\n═══ CATEGORY 1: Direct Keyword Injection ═══');
test('1.01', 'var x = process.env', true, 'direct', 'process.env');
test('1.02', 'require("fs")', true, 'direct', 'require()');
test('1.03', 'eval("alert(1)")', true, 'direct', 'eval()');
test('1.04', 'Function("return this")()', true, 'direct', 'Function()');
test('1.05', 'fetch("http://evil.com")', true, 'direct', 'fetch()');
test('1.06', 'new XMLHttpRequest()', true, 'direct', 'XMLHttpRequest');
test('1.07', 'new WebSocket("ws://evil")', true, 'direct', 'WebSocket');
test('1.08', 'import("fs")', true, 'direct', 'dynamic import');
test('1.09', 'globalThis.process', true, 'direct', 'globalThis');
test('1.10', 'x.__proto__', true, 'direct', '__proto__');
test('1.11', 'x.prototype.constructor', true, 'direct', 'prototype.constructor');
test('1.12', 'Object.getPrototypeOf(x)', true, 'direct', 'getPrototypeOf');
test('1.13', 'Object.setPrototypeOf(x, {})', true, 'direct', 'setPrototypeOf');
test('1.14', 'arguments.callee', true, 'direct', 'arguments.callee');
test('1.15', 'var x = window', true, 'direct', 'window');
test('1.16', 'var x = self', true, 'direct', 'self');
test('1.17', 'var x = top', true, 'direct', 'top');
test('1.18', 'var x = parent', true, 'direct', 'parent');
test('1.19', 'var x = frames', true, 'direct', 'frames');
test('1.20', 'var x = opener', true, 'direct', 'opener');
test('1.21', 'location.href = "http://evil"', true, 'direct', 'location.href=');
test('1.22', 'location = "http://evil"', true, 'direct', 'location=');
test('1.23', 'document.cookie', true, 'direct', 'document.cookie');
test('1.24', 'document.write("<script>")', true, 'direct', 'document.write');
test('1.25', 'localStorage.getItem("x")', true, 'direct', 'localStorage');
test('1.26', 'sessionStorage.getItem("x")', true, 'direct', 'sessionStorage');
test('1.27', 'indexedDB.open("db")', true, 'direct', 'indexedDB');
test('1.28', 'navigator.userAgent', true, 'direct', 'navigator');

// Safe patterns that MUST pass
test('1.29', 'console.log("x")', false, 'direct', 'console.log (safe)');
test('1.30', 'Math.random()', false, 'direct', 'Math.random (safe)');
test('1.31', 'var x = 5', false, 'direct', 'simple variable (safe)');
test('1.32', 'var arr = [1,2,3]', false, 'direct', 'array literal (safe)');
test('1.33', 'if (x > 5) { print(x); }', false, 'direct', 'if statement (safe)');
test('1.34', 'for (var i=0; i<10; i++) { print(i); }', false, 'direct', 'for loop (safe)');
test('1.35', 'function foo(x) { return x * 2; }', false, 'direct', 'function decl (safe)');

// ════════════════════ CATEGORY 2: Unicode ════════════════════
console.log('\n═══ CATEGORY 2: Unicode Bypass ═══');
test('2.01', 'pr\u202Aocess.env', true, 'unicode', 'RTL override in process');
test('2.02', 'pro\u200Bcess', true, 'unicode', 'ZWSP in process');
test('2.03', 'pr\u200Cocess', true, 'unicode', 'ZWNJ in process');
test('2.04', 'pr\u200Docess', true, 'unicode', 'ZWJ in process');
test('2.05', 'proce\u00ADss', true, 'unicode', 'soft hyphen in process');
test('2.06', 'pr\uFFFDocess', true, 'unicode', 'PUA char in process');
test('2.07', '\uFEFFrequire("fs")', true, 'unicode', 'BOM before require');
test('2.08', 'requi\u200Bre("fs")', true, 'unicode', 'ZWSP in require');
test('2.09', 'requ\u00ADire("fs")', true, 'unicode', 'soft hyphen in require');
test('2.10', 'e\u200Bval("x")', true, 'unicode', 'ZWSP in eval');
test('2.11', 'win\u200Bdow', true, 'unicode', 'ZWSP in window');
test('2.12', 'nav\u200Bigator', true, 'unicode', 'ZWSP in navigator');
test('2.13', 'loca\u200Btion', false, 'unicode', 'ZWSP in location (no . or = after)');

// ════════════════════ CATEGORY 3: Concatenation ════════════════════
console.log('\n═══ CATEGORY 3: String Concatenation ═══');
test('3.01', 'var a="pro"; var b="cess"; var c=a+b; eval(c)', true, 'concat', 'concat eval via string');
test('3.02', 'var a="win"; var b="dow"; window', true, 'concat', 'concat window');
test('3.03', 'var s = "proc" + "ess"; s.env', false, 'concat', 'concat process via + (theoretical)');
test('3.04', '["pro","cess"].join("")', false, 'concat', 'join process (theoretical)');
test('3.05', 'atob("cmVxdWlyZQ==")', false, 'concat', 'atob decode (no pattern)');

// ════════════════════ CATEGORY 4: Computed Properties ════════════════════
console.log('\n═══ CATEGORY 4: Computed Properties ═══');
test('4.01', 'this["con"]["structor"]["constructor"]("return process")()', true, 'computed', 'bracket constructor escape');
test('4.02', 'this["\x70ro\x63ess"]', true, 'computed', 'hex escape process');
test('4.03', 'this["\x77indow"]', true, 'computed', 'hex escape window');
test('4.04', 'var x = this["\x65val"]', false, 'computed', 'hex escape eval (theoretical)');
test('4.05', 'var x = this["\x66unction"]', false, 'computed', 'hex escape Function (theoretical)');

// ════════════════════ CATEGORY 5: Aliases ════════════════════
console.log('\n═══ CATEGORY 5: Aliases & Indirect References ═══');
test('5.01', 'var g = this.constructor.constructor("return this")(); g.process.env', true, 'alias', 'this.constructor.constructor');
test('5.02', 'var f = this["constructor"]["constructor"]; f("return process")()', true, 'alias', 'bracket constructor');
test('5.03', 'var x = {}["__proto__"]', true, 'alias', '__proto__ via object');
test('5.04', 'var x = {}["constructor"]["prototype"]', true, 'alias', 'constructor.prototype (new pattern)');
test('5.05', 'var x = [].__proto__', true, 'alias', '__proto__ via array');
test('5.06', 'var x = "".__proto__', true, 'alias', '__proto__ via string');
test('5.07', 'var x = (0)["constructor"]', true, 'alias', 'constructor via number');
test('5.08', 'var x = (true)["constructor"]', true, 'alias', 'constructor via boolean');

// ════════════════════ CATEGORY 6: Whitespace ════════════════════
console.log('\n═══ CATEGORY 6: Whitespace Tricks ═══');
test('6.01', 'pro\ncess', true, 'ws', 'newline in process');
test('6.02', 'pro\tcess', true, 'ws', 'tab in process');
test('6.03', 'pro ccess', false, 'ws', 'space in process (no match)');
test('6.04', 'requi re("fs")', false, 'ws', 'space in require (theoretical)');
test('6.05', 'ev al("x")', false, 'ws', 'space in eval (theoretical)');
test('6.06', 'win dow', false, 'ws', 'space in window (theoretical)');
test('6.07', 'loca\ntion', false, 'ws', 'newline in location (no . or = after)');
test('6.08', 'navi\ngator', true, 'ws', 'newline in navigator');

// ════════════════════ CATEGORY 7: Case ════════════════════
console.log('\n═══ CATEGORY 7: Case Variations ═══');
test('7.01', 'Process.env', false, 'case', 'capitalized Process (no match)');
test('7.02', 'WINDOW', false, 'case', 'uppercase WINDOW (no match)');
test('7.03', 'NAVIGATOR', false, 'case', 'uppercase NAVIGATOR (no match)');
test('7.04', 'EVAL("x")', false, 'case', 'uppercase EVAL (no match)');
test('7.05', 'Function("x")', true, 'case', 'capitalized Function (match)');
test('7.06', 'FUNCTION("x")', false, 'case', 'uppercase FUNCTION (no match)');
test('7.07', 'Eval("x")', false, 'case', 'capitalized Eval (case-sensitive, theoretical)');

// ════════════════════ CATEGORY 8: Encoded ════════════════════
console.log('\n═══ CATEGORY 8: Encoded Strings ═══');
test('8.01', 'String.fromCharCode(112,114,111,99,101,115,115)', false, 'encoded', 'fromCharCode process');
test('8.02', 'atob("cHJvY2Vzcy5lbnY=")', false, 'encoded', 'atob encoded process');
test('8.03', 'unescape("%70%72%6F%63%65%73%73")', false, 'encoded', 'unescape process');
test('8.04', 'decodeURIComponent("%70%72%6F%63%65%73%73")', false, 'encoded', 'decodeURIComponent process');
test('8.05', '\\x70\\x72\\x6F\\x63\\x65\\x73\\x73', false, 'encoded', 'hex escape process in string (theoretical)');
test('8.06', '\\u0070\\u0072\\u006F\\u0063\\u0065\\u0073\\u0073', false, 'encoded', 'unicode escape process (theoretical)');

// ════════════════════ CATEGORY 9: Comments ════════════════════
console.log('\n═══ CATEGORY 9: Comment-Based Hiding ═══');
test('9.01', '// process\nvar x = 5', true, 'comment', 'process in comment');
test('9.02', '/* eval */ var x = 5', false, 'comment', 'eval in comment (theoretical)');
test('9.03', 'var x = 5; // require("fs")', true, 'comment', 'require in comment');
test('9.04', 'var x = 5; /* window */', true, 'comment', 'window in comment');
test('9.05', 'var x = 5; // this is safe', false, 'comment', 'safe comment');
test('9.06', '/* \n process \n */ var x = 5', true, 'comment', 'process in multiline');

// ════════════════════ CATEGORY 10: Dhad Safe Patterns ════════════════════
console.log('\n═══ CATEGORY 10: Dhad Language Patterns (Safe) ═══');
test('10.01', '__print("مرحباً")', false, 'dhad', '__print');
test('10.02', 'var x = __input()', false, 'dhad', '__input');
test('10.03', 'var arr = __array(1,2,3)', false, 'dhad', '__array');
test('10.04', '__guiPrint("نص")', false, 'dhad', '__guiPrint');
test('10.05', '__dhad.rectangle(0,0,100,100)', false, 'dhad', '__dhad.rectangle');
test('10.06', '__dhad.setColor("red")', false, 'dhad', '__dhad.setColor');
test('10.07', 'var x = 5 + 3 * 2', false, 'dhad', 'arithmetic');
test('10.08', 'if (x > 5) { __print("big"); }', false, 'dhad', 'if statement');
test('10.09', 'for (var i=0; i<10; i++) { __print(i); }', false, 'dhad', 'for loop');
test('10.10', 'function foo(x) { return x * 2; }', false, 'dhad', 'function decl');
test('10.11', 'var obj = { name: "test", value: 42 }', false, 'dhad', 'object literal');
test('10.12', 'var arr = [1, 2, 3, 4, 5]', false, 'dhad', 'array literal');
test('10.13', 'try { x = 1; } catch(e) { x = 0; }', false, 'dhad', 'try-catch');
test('10.14', 'var x = true ? 1 : 0', false, 'dhad', 'ternary');
test('10.15', 'x = x + 1', false, 'dhad', 'compound assign');
test('10.16', 'var x = "hello " + "world"', false, 'dhad', 'string concat');
test('10.17', 'x = typeof(y)', false, 'dhad', 'typeof');
test('10.18', 'throw new Error("test")', false, 'dhad', 'throw');
test('10.19', 'x = x instanceof Array', false, 'dhad', 'instanceof');
test('10.20', 'var __files = { handles: [] }', false, 'dhad', '__files runtime');

// ════════════════════ Results ════════════════════
const total = pass + fail;
console.log(`\n═══════════════════════════════════════════════════`);
console.log(`Total: ${total} | Pass: ${pass} | Fail: ${fail}`);
console.log(`═══════════════════════════════════════════════════`);

if (fail > 0) {
  console.log('\nFAILURES:');
  results.filter(r => !r.ok).forEach(r => {
    console.log(`  ${r.id}: [${r.cat}] ${r.desc}`);
    console.log(`    Expected: ${r.expectBlocked ? 'BLOCKED' : 'PASS'} | Got: ${r.blocked ? 'BLOCKED' : 'PASS'}`);
  });
}

module.exports = { results, total, pass, fail };
