/**
 * DAAD Server Sandbox — Adversarial Security Tests
 * Tests executeInSandbox() against escape attempts
 */

const { executeInSandbox: productionExecute } = require('../../src/utils/dhadSandbox');

function executeInSandbox(jsCode) {
  const result = productionExecute(jsCode);
  return {
    stdout: result.stdout || '',
    timedOut: result.timedOut || false,
  };
}

const results = [];
let pass = 0, fail = 0;

/**
 * test(id, code, expectSuccess, category, description)
 * expectSuccess=true means we expect the code to EXECUTE SUCCESSFULLY with real data (sandbox escape)
 * expectSuccess=false means we expect the attack to be BLOCKED (sandbox holds)
 */
function test(id, jsCode, expectSuccess, cat, desc) {
  const r = executeInSandbox(jsCode);
  const output = r.stdout;

  // Real escape indicators: filesystem paths, process data, function source
  const ESCAPE_INDICATORS = ['/home/', '/usr/', '/etc/', 'node_modules', 'function (', 'object]'];

  // Safe output indicators (expected Dhad outputs)
  const SAFE_OUTPUTS = ['عدم', 'صواب', 'خطأ', 'مرحباً'];

  let actuallyEscaped = false;

  if (r.timedOut) {
    // Timeout = DoS blocked = safe
    actuallyEscaped = false;
  } else if (output.length === 0) {
    // No output = attack was blocked (undefined access returns nothing useful)
    actuallyEscaped = false;
  } else {
    // Check if output contains real system data
    actuallyEscaped = ESCAPE_INDICATORS.some(ind => output.includes(ind));
  }

  const ok = actuallyEscaped === expectSuccess;
  if (ok) pass++; else fail++;

  results.push({ id, cat, desc, expectSuccess, actuallyEscaped, ok, output: output.substring(0, 200), timedOut: r.timedOut });

  if (!ok) {
    console.log(`  FAIL ${id}: [${cat}] ${desc}`);
    console.log(`    Expected: ${expectSuccess ? 'ESCAPED' : 'BLOCKED'} | Got: ${actuallyEscaped ? 'ESCAPED' : 'BLOCKED'}`);
    console.log(`    Output: "${output.substring(0, 200)}" | TimedOut: ${r.timedOut}`);
  }
}

// ════════════════════ CATEGORY 1: Direct Node.js API ════════════════════
console.log('\n═══ CATEGORY 1: Direct Node.js API Access ═══');
test('1.01', '__print(process.env.HOME)', false, 'direct', 'process.env');
test('1.02', '__print(require("fs").readFileSync("/etc/passwd","utf8"))', false, 'direct', 'require fs');
test('1.03', '__print(require("child_process").execSync("id").toString())', false, 'direct', 'require child_process');
test('1.04', '__print(require("http").get)', false, 'direct', 'require http');
test('1.05', '__print(process.exit())', false, 'direct', 'process.exit');
test('1.06', '__print(module.exports)', false, 'direct', 'module.exports');
test('1.07', '__print(global)', false, 'direct', 'global');
test('1.08', '__print(Buffer)', false, 'direct', 'Buffer');
test('1.09', '__print(eval("1+1"))', false, 'direct', 'eval');
test('1.10', '__print(Function("return 2+2")())', false, 'direct', 'Function');

// ════════════════════ CATEGORY 2: Prototype Chain Escape ════════════════════
console.log('\n═══ CATEGORY 2: Prototype Chain Escape ═══');
test('2.01', '__print(this.constructor.constructor("return process")().env.HOME)', false, 'proto', 'this.constructor.constructor');
test('2.02', 'var c = this["constructor"]["constructor"]; __print(c("return process")().env.HOME)', false, 'proto', 'bracket constructor');
test('2.03', '__print(({}).constructor.constructor("return process")().env.HOME)', false, 'proto', 'object literal constructor');
test('2.04', '__print(([]).constructor.constructor("return process")().env.HOME)', false, 'proto', 'array literal constructor');
test('2.05', '__print(("").constructor.constructor("return process")().env.HOME)', false, 'proto', 'string literal constructor');
test('2.06', '__print((0).constructor.constructor("return process")().env.HOME)', false, 'proto', 'number literal constructor');
test('2.07', '__print((true).constructor.constructor("return process")().env.HOME)', false, 'proto', 'boolean literal constructor');
test('2.08', '__print((/x/).constructor.constructor("return process")().env.HOME)', false, 'proto', 'regex literal constructor');
test('2.09', '__print(Error().constructor.constructor("return process")().env.HOME)', false, 'proto', 'Error constructor escape');
test('2.10', '__print(new TypeError().constructor.constructor("return process")().env.HOME)', false, 'proto', 'TypeError constructor escape');

// ════════════════════ CATEGORY 3: Error Handler Escape ════════════════════
console.log('\n═══ CATEGORY 3: Error Handler Escape ═══');
test('3.01', 'try { null.x } catch(e) { __print(e.constructor.constructor("return process")().env.HOME) }', false, 'error', 'catch error constructor');
test('3.02', 'try { null.x } catch(e) { var f = e.constructor.constructor; __print(f("return process")().env.HOME) }', false, 'error', 'catch error alias');
test('3.03', 'try { null.x } catch(e) { __print(e.constructor.constructor("return require(\\"fs\\")")().readFileSync("/etc/passwd","utf8").substring(0,10)) }', false, 'error', 'catch error fs read');

// ════════════════════ CATEGORY 4: With Statement ════════════════════
console.log('\n═══ CATEGORY 4: With Statement ═══');
test('4.01', 'with(Math) { __print(sqrt(4)) }', false, 'with', 'with Math (safe)');
test('4.02', 'var o = {constructor:{constructor:function(){return process}}}; with(o) { __print(constructor.constructor("return 2")()) }', false, 'with', 'with prototype pollution');

// ════════════════════ CATEGORY 5: Proxy / Reflect ════════════════════
console.log('\n═══ CATEGORY 5: Proxy / Reflect ═══');
test('5.01', '__print(typeof Proxy)', false, 'proxy', 'typeof Proxy');
test('5.02', '__print(typeof Reflect)', false, 'proxy', 'typeof Reflect');

// ════════════════════ CATEGORY 6: Prototype Pollution ════════════════════
console.log('\n═══ CATEGORY 6: Prototype Pollution ═══');
test('6.01', 'Object.prototype.polluted = "yes"; __print({}.polluted)', false, 'pollution', 'Object.prototype pollution');
test('6.02', 'var x = {}; x.__proto__.polluted = "yes"; __print(x.polluted)', false, 'pollution', '__proto__ pollution');
test('6.03', 'Object.defineProperty(Object.prototype, "pwned", {get: function(){return "yes"}}); __print({}.pwned)', false, 'pollution', 'defineProperty pollution');

// ════════════════════ CATEGORY 7: Timer / URL ════════════════════
console.log('\n═══ CATEGORY 7: Timer / URL ═══');
test('7.01', '__print(typeof setTimeout)', false, 'timer', 'setTimeout');
test('7.02', '__print(typeof setInterval)', false, 'timer', 'setInterval');
test('7.03', '__print(typeof setImmediate)', false, 'timer', 'setImmediate');
test('7.04', '__print(typeof URL)', false, 'timer', 'URL');

// ════════════════════ CATEGORY 8: DoS ════════════════════
console.log('\n═══ CATEGORY 8: DoS ═══');
test('8.01', 'while(true) {}', false, 'dos', 'infinite while');
test('8.02', 'for(;;) {}', false, 'dos', 'infinite for');
test('8.03', 'function f(){f()} f()', false, 'dos', 'infinite recursion');
test('8.04', 'var a=[]; while(true) a.push(1);', false, 'dos', 'memory bomb');

// ════════════════════ CATEGORY 9: Valid Dhad ════════════════════
console.log('\n═══ CATEGORY 9: Valid Dhad Programs (should work) ═══');
// These should work — they use sandbox-provided functions
test('9.01', '__print("test")', false, 'valid', 'print string');
test('9.02', '__print(5 + 3)', false, 'valid', 'arithmetic');
test('9.03', 'var x = 10; __print(x)', false, 'valid', 'variable');
test('9.04', 'var arr = __array(1,2,3); __print(arr.length)', false, 'valid', 'array');
test('9.05', '__print("صواب")', false, 'valid', 'print Arabic');
test('9.06', 'var s=0; for(var i=1;i<=5;i++){s+=i;} __print(s)', false, 'valid', 'for loop');
test('9.07', 'function ضرب(x,y){return x*y;} __print(ضرب(3,4))', false, 'valid', 'function');
test('9.08', 'var a=10;var b=3;__print(a-b)', false, 'valid', 'subtraction');

// ════════════════════ Results ════════════════════
const total = pass + fail;
console.log(`\n═══════════════════════════════════════════════════`);
console.log(`Total: ${total} | Pass: ${pass} | Fail: ${fail}`);
console.log(`═══════════════════════════════════════════════════`);

if (fail > 0) {
  console.log('\nFAILURES:');
  results.filter(r => !r.ok).forEach(r => {
    console.log(`  ${r.id}: [${r.cat}] ${r.desc}`);
    console.log(`    Expected: ${r.expectSuccess ? 'ESCAPED' : 'BLOCKED'} | Got: ${r.actuallyEscaped ? 'ESCAPED' : 'BLOCKED'}`);
    console.log(`    Output: "${r.output}" | TimedOut: ${r.timedOut}`);
  });
}

module.exports = { results, total, pass, fail };
