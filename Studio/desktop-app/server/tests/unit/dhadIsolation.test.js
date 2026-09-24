// ====================================================
// C2-full — isolated worker execution tests
// Proves: parity with sync path, real timeout kill, real memory
// containment (API process survives), escape containment, output cap.
// ====================================================
const {
  executeDhad,
  executeIsolated,
  executeDhadIsolated,
} = require('../../src/utils/dhadSandbox');

describe('C2-full isolated execution', () => {
  test('parity: isolated matches sync on legit programs', async () => {
    const cases = [
      'طباعة("مرحبا")',
      'صحيح x = 2\nطباعة(x + 3)',
      'صنف شكل {\n    دالة شكل(صحيح ن) {\n        هذا.ن = ن\n    }\n    دالة قيمة() {\n        ارجع هذا.ن\n    }\n}\nصحيح ش = جديد شكل(7)\nطباعة(ش.قيمة())',
    ];
    for (const code of cases) {
      const a = executeDhad(code);
      const b = await executeDhadIsolated(code);
      expect(b.success).toBe(a.success);
      expect(b.output).toBe(a.output);
      expect(b.exitStatus).toBe(a.exitStatus);
    }
  });

  test('escape attempt stays contained', async () => {
    const r = await executeIsolated('__print(typeof process)');
    expect(r.exitStatus).toBe(0);
  });

  test('escape attempt cannot read process', async () => {
    const r = await executeIsolated("__print(typeof ({}).constructor.constructor('return process')())");
    expect(r.stdout).toBe('undefined');
  });

  test('output flood is truncated via isolated path', async () => {
    const r = await executeIsolated("for (let i = 0; i < 500; i++) { __print('x'.repeat(1000)); }");
    expect(r.exitStatus).toBe(0);
    expect(r.stdout).toContain('اقتطاع');
  });

  test('infinite loop is killed by timeout (worker terminate)', async () => {
    const r = await executeIsolated('while(true){}');
    expect(r.timedOut).toBe(true);
    expect(r.exitStatus).toBe(1);
  }, 20000);

  test('memory bomb is contained and process survives', async () => {
    const r = await executeIsolated("let a=[]; while(true){a.push('x'.repeat(100000));}");
    expect(r.heapExceeded === true || r.timedOut === true).toBe(true);
    // API process must still work afterwards (no crash, no hang)
    const after = await executeDhadIsolated('صحيح x = 2\nطباعة(x + 3)');
    expect(after.success).toBe(true);
    expect(after.output).toBe('5');
  }, 20000);

  // ── Item-5: مجرّd enforcement — isolated + sync parity ──
  test('item-5: مجرّd direct new fails in both sync and isolated', async () => {
    const sync = executeDhad('مجرّد صنف حيوان {}\nجديد حيوان()');
    const isolated = await executeDhadIsolated('مجرّد صنف حيوان {}\nجديد حيوان()');
    expect(sync.success).toBe(false);
    expect(isolated.success).toBe(false);
    expect(sync.errors[0].message).toContain('مجرّد');
    expect(isolated.errors[0].message).toContain('مجرّد');
  });

  test('item-5: child of مجرّd works in both sync and isolated', async () => {
    const sync = executeDhad('مجرّد صنف حيوان {}\nصنف قط يرث حيوان {}\nجديد قط()');
    const isolated = await executeDhadIsolated('مجرّد صنف حيوان {}\nصنف قط يرث حيوان {}\nجديد قط()');
    expect(sync.success).toBe(true);
    expect(isolated.success).toBe(true);
  });

  test('item-5: regular class new works in both sync and isolated', async () => {
    const code = 'صنف شكل {\n    دالة جديد(صحيح ن) {\n        هذا.ن = ن\n    }\n    دالة قيمة() {\n        ارجع هذا.ن\n    }\n}\nصحيح ش = جديد شكل(7)\nطباعة(ش.قيمة())';
    const sync = executeDhad(code);
    const isolated = await executeDhadIsolated(code);
    expect(sync.success).toBe(true);
    expect(isolated.success).toBe(true);
    expect(sync.output).toBe('7');
    expect(isolated.output).toBe('7');
  });
});
