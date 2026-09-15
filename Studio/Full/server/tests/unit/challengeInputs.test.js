// ====================================================
// Item-2 — per-challenge stdin queue for ادخل
// Queue order, exhaustion default, browser-parity coercion,
// sync/isolated parity. (E2E submit proven live; see plan log.)
// ====================================================
const {
  executeDhad,
  executeInSandbox,
  executeIsolated,
  executeDhadIsolated,
} = require('../../src/utils/dhadSandbox');

const READ_TWO = 'صحيح a = ادخل()\nصحيح b = ادخل()\nطباعة(a)\nطباعة(b)';

describe('Item-2 input queue (sync path)', () => {
  test('consumes inputs in order', () => {
    const r = executeDhad(READ_TWO, ['7', 'أحمد']);
    expect(r.success).toBe(true);
    expect(r.output).toBe('7\nأحمد');
  });

  test('exhausted queue keeps legacy 0 default', () => {
    const r = executeDhad('صحيح a = ادخل()\nطباعة(a)', []);
    expect(r.success).toBe(true);
    expect(r.output).toBe('0');
  });

  test('numeric strings coerce like browser prompt()', () => {
    const r = executeDhad('صحيح a = ادخل()\nطباعة(a + 1)', ['41']);
    expect(r.success).toBe(true);
    expect(r.output).toBe('42');
  });

  test('non-numeric strings stay strings', () => {
    const r = executeDhad('صحيح a = ادخل()\nطباعة(a)', ['أحمد']);
    expect(r.success).toBe(true);
    expect(r.output).toBe('أحمد');
  });

  test('invalid inputs value defaults to empty queue', () => {
    const r = executeDhad('صحيح a = ادخل()\nطباعة(a)', 'not-an-array');
    expect(r.success).toBe(true);
    expect(r.output).toBe('0');
  });

  test('executeInSandbox honors the queue directly', () => {
    const r = executeInSandbox('__print(__input()); __print(__input());', [1, 2]);
    expect(r.exitStatus).toBe(0);
    expect(r.stdout).toBe('1\n2');
  });
});

describe('Item-2 input queue (isolated worker path)', () => {
  test('parity with sync path', async () => {
    const a = executeDhad(READ_TWO, ['7', 'أحمد']);
    const b = await executeDhadIsolated(READ_TWO, ['7', 'أحمد']);
    expect(b.success).toBe(a.success);
    expect(b.output).toBe(a.output);
  });

  test('exhaustion + coercion parity', async () => {
    const b = await executeDhadIsolated('صحيح a = ادخل()\nصحيح c = ادخل()\nطباعة(a)\nطباعة(c)', ['41']);
    expect(b.success).toBe(true);
    expect(b.output).toBe('41\n0');
  });
});
