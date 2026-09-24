// ====================================================
// C2 — Sandbox hardening regression tests (quick mitigation)
// Verifies: dangerous globals absent, prototype walks blocked,
// output cap enforced, infinite loops time out, legit code works.
// NOTE: vm is still not a full security boundary (see SECURITY_FIX_PLAN.md C2);
// these tests guard the quick mitigation, not the final isolated-vm fix.
// ====================================================
const { executeDhad, executeInSandbox, LIMITS } = require('../../src/utils/dhadSandbox');

describe('Dhad Sandbox — C2 hardening', () => {
  test('legit program still works', () => {
    const r = executeDhad('صحيح x = 5\nطباعة(x + 3)');
    expect(r.success).toBe(true);
    expect(r.output).toBe('8');
  });

  test('process/require/Function are unavailable', () => {
    expect(executeInSandbox('__print(typeof process)').stdout).toBe('undefined');
    expect(executeInSandbox('__print(typeof require)').stdout).toBe('undefined');
    expect(executeInSandbox('__print(Function)').stdout).toBe('عدم');
  });

  test('constructor chain cannot reach host process', () => {
    // Inner-realm constructors exist, but host globals stay unreachable:
    expect(executeInSandbox('__print(typeof ({}).constructor.constructor("return process")())').stdout)
      .toBe('undefined');
  });

  test('methods stay usable (prototype access required by V8 invariants)', () => {
    // `Array.prototype.slice` powers __array — blocking 'prototype' on
    // functions would break it, so we assert usability instead:
    expect(executeInSandbox('__print(typeof [].map)').stdout).toBe('function');
    const r = executeDhad('صحيح arr = [1, 2, 3]\nطباعة(arr[0])');
    expect(r.success).toBe(true);
    expect(r.output).toBe('1');
  });

  test('getPrototypeOf reflection is blocked', () => {
    const r = executeInSandbox('__print(Object.getPrototypeOf({}))');
    expect(r.exitStatus).toBe(1);
    expect(r.runtimeError).toBeDefined();
  });

  test('output flood is truncated at the cap', () => {
    const r = executeInSandbox("for (let i = 0; i < 500; i++) { __print('x'.repeat(1000)); }");
    expect(r.exitStatus).toBe(0);
    expect(r.stdout).toContain('اقتطاع');
    expect(Buffer.byteLength(r.stdout, 'utf8')).toBeLessThanOrEqual(LIMITS.MAX_OUTPUT_BYTES + 512);
  });

  test('infinite loop times out', () => {
    const r = executeInSandbox('while (true) {}');
    expect(r.timedOut).toBe(true);
    expect(r.exitStatus).toBe(1);
  }, 15000);
});
