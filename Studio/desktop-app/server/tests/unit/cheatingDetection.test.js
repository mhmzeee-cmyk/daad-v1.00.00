// ====================================================
// M7 — hardcoding detection: tier-2+ active, tier-1 exempt, obfuscation caught
// ====================================================
const { detectHardcoding } = require('../../src/utils/serverEvaluator');

describe('M7 hardcoding detection', () => {
  test('tier 1 stays exempt (starter ships the answer)', () => {
    const r = detectHardcoding('طباعة("مرحبا")', 'مرحبا', 1);
    expect(r.suspicious).toBe(false);
  });

  test('tier 2 direct print is flagged (no longer skipped)', () => {
    const r = detectHardcoding('طباعة("مرحبا")', 'مرحبا', 2);
    expect(r.suspicious).toBe(true);
    expect(r.reasons).toContain('hardcoded_direct');
  });

  test('tier 3 concat obfuscation is flagged', () => {
    const r = detectHardcoding('طباعة("مرح" + "با")', 'مرحبا', 3);
    expect(r.suspicious).toBe(true);
    expect(r.reasons).toContain('obfuscated_concat');
  });

  test('tier 3 fromCharCode obfuscation is flagged', () => {
    const r = detectHardcoding('طباعة(String.fromCharCode(1605,1585,1581,1576,1575))', 'مرحبا', 3);
    expect(r.suspicious).toBe(true);
    expect(r.reasons).toContain('obfuscated_encoding');
  });

  test('legit computation is not flagged', () => {
    const r = detectHardcoding('صحيح a = 20\nصحيح b = 22\nطباعة(a + b)', '42', 3);
    expect(r.suspicious).toBe(false);
  });
});
