// ═══════════════════════════════════════════════════════════════════════════════
// ANTI-CHEAT VULNERABILITY TEST - Dhad Studio
// Tests all possible cheating vectors
// ═══════════════════════════════════════════════════════════════════════════════

const http = require('http');
const crypto = require('crypto');

const HOST = 'localhost';
const PORT = 3000;

function req(method, path, body, headers = {}) {
  return new Promise((resolve) => {
    const data = body !== null && body !== undefined
      ? (typeof body === 'string' ? body : JSON.stringify(body))
      : null;
    const opts = {
      hostname: HOST, port: PORT, path, method,
      headers: {
        'Content-Type': 'application/json',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36',
        ...(data ? { 'Content-Length': Buffer.byteLength(data) } : {}),
        ...headers
      },
      timeout: 10000
    };
    const r = http.request(opts, res => {
      let buf = '';
      res.on('data', d => buf += d);
      res.on('end', () => {
        try { resolve({ status: res.statusCode, body: JSON.parse(buf), raw: buf, headers: res.headers }); }
        catch { resolve({ status: res.statusCode, body: buf, raw: buf, headers: res.headers }); }
      });
    });
    r.on('error', e => resolve({ status: 0, body: { error: e.message }, raw: '', headers: {} }));
    r.on('timeout', () => { r.destroy(); resolve({ status: 0, body: { error: 'TIMEOUT' }, raw: '', headers: {} }); });
    if (data) r.write(data);
    r.end();
  });
}

let T = {};
let F = [];
let PASSED = 0, FAILED = 0;

function vuln(sev, cat, title, detail) {
  F.push({ severity: sev, category: cat, title, detail: detail || '' });
  FAILED++;
  const icon = { CRITICAL: '🔴', HIGH: '🟠', MEDIUM: '🟡', LOW: '🔵' }[sev] || '⚪';
  console.log(`  ${icon} [${sev}] ${title}`);
  if (detail) console.log(`     ${detail}`);
}
function pass(msg) { PASSED++; console.log(`  ✅ ${msg}`); }
function info(msg) { console.log(`  ℹ️  ${msg}`); }

async function main() {
  console.log('╔══════════════════════════════════════════════════════════════╗');
  console.log('║   ANTI-CHEAT VULNERABILITY TEST - DHAD STUDIO             ║');
  console.log('║   Testing all cheating vectors                            ║');
  console.log('╚══════════════════════════════════════════════════════════════╝\n');

  // ─── SETUP ───────────────────────────────────────────────────────────────
  console.log('🔑 Obtaining tokens...');
  const teacherAccounts = ['teacher@dhad.com', 'shamel@teacher.com', 'sara@teacher.com', 'ali@teacher.com'];
  for (const acc of teacherAccounts) {
    const t = await req('POST', '/api/v1/auth/login', { username: acc, password: 'Test1234!' });
    if (t.body?.accessToken) { T.teacher = t.body.accessToken; console.log(`   Teacher: ${acc}`); break; }
  }
  const s = await req('POST', '/api/v1/auth/login', { username: '1099990001', password: 'Test1234!' });
  T.student = s.body?.accessToken;
  const a = await req('POST', '/api/v1/auth/login', { username: 'admin@dhadstudio.com', password: 'Test1234!' });
  T.admin = a.body?.accessToken;
  console.log(`   Teacher: ${T.teacher ? '✅' : '❌'}  Student: ${T.student ? '✅' : '❌'}  Admin: ${T.admin ? '✅' : '❌'}\n`);

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 1: CHALLENGE RESULT FORGERY (20 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('━━━ 1. CHALLENGE RESULT FORGERY ━━━');

  // 1.1 Submit with passed=true (server should verify)
  let r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'fake-challenge-id',
    passed: true,
    output: 'hacked',
    signature: 'fake-signature'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status === 404) pass('1.1 Fake challenge ID rejected (404)');
  else if (r.status === 403) pass('1.1 Fake challenge rejected (403)');
  else vuln('CRITICAL', 'FORGERY', '1.1 Fake challenge accepted', `Status: ${r.status}`);

  // 1.2 Submit with manipulated output
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'expected-output',
    signature: 'forged'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.2 Manipulated output rejected');
  else vuln('CRITICAL', 'FORGERY', '1.2 Manipulated output accepted', `Status: ${r.status}`);

  // 1.3 Submit without signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.3 Missing signature rejected');
  else vuln('HIGH', 'FORGERY', '1.3 Missing signature accepted', `Status: ${r.status}`);

  // 1.4 Submit with empty signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: ''
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.4 Empty signature rejected');
  else vuln('HIGH', 'FORGERY', '1.4 Empty signature accepted', `Status: ${r.status}`);

  // 1.5 Submit with null signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: null
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.5 Null signature rejected');
  else vuln('HIGH', 'FORGERY', '1.5 Null signature accepted', `Status: ${r.status}`);

  // 1.6 Submit with undefined signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: undefined
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.6 Undefined signature rejected');
  else vuln('HIGH', 'FORGERY', '1.6 Undefined signature accepted', `Status: ${r.status}`);

  // 1.7 Submit with numeric signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 12345
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.7 Numeric signature rejected');
  else vuln('MEDIUM', 'FORGERY', '1.7 Numeric signature accepted', `Status: ${r.status}`);

  // 1.8 Submit with array signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: ['a', 'b', 'c']
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.8 Array signature rejected');
  else vuln('MEDIUM', 'FORGERY', '1.8 Array signature accepted', `Status: ${r.status}`);

  // 1.9 Submit with object signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: { key: 'value' }
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.9 Object signature rejected');
  else vuln('MEDIUM', 'FORGERY', '1.9 Object signature accepted', `Status: ${r.status}`);

  // 1.10 Submit with very long signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'A'.repeat(10000)
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.10 Long signature rejected');
  else vuln('MEDIUM', 'FORGERY', '1.10 Long signature accepted', `Status: ${r.status}`);

  // 1.11 Submit with SQL injection in signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: "' OR '1'='1"
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.11 SQLi in signature rejected');
  else vuln('CRITICAL', 'FORGERY', '1.11 SQLi in signature accepted', `Status: ${r.status}`);

  // 1.12 Submit with XSS in output
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: '<script>alert(1)</script>',
    signature: 'test'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.12 XSS in output rejected');
  else vuln('MEDIUM', 'FORGERY', '1.12 XSS in output accepted', `Status: ${r.status}`);

  // 1.13 Submit with null byte in output
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test\x00hacked',
    signature: 'test'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.13 Null byte in output rejected');
  else vuln('HIGH', 'FORGERY', '1.13 Null byte in output accepted', `Status: ${r.status}`);

  // 1.14 Submit without authentication
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test'
  });
  if (r.status === 401) pass('1.14 No auth rejected (401)');
  else vuln('CRITICAL', 'FORGERY', '1.14 No auth accepted', `Status: ${r.status}`);

  // 1.15 Submit with expired token
  const expiredPayload = Buffer.from(JSON.stringify({ id: 'hacker', role: 'STUDENT', iat: 1000000000, exp: 1000000001 })).toString('base64url');
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test'
  }, { Authorization: `Bearer fake.${expiredPayload}.sig` });
  if (r.status === 401) pass('1.15 Expired token rejected');
  else vuln('HIGH', 'FORGERY', '1.15 Expired token accepted', `Status: ${r.status}`);

  // 1.16 Submit with admin token (privilege escalation)
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test'
  }, { Authorization: `Bearer ${T.admin}` });
  if (r.status === 403 || r.status === 401) pass('1.16 Admin token rejected');
  else vuln('HIGH', 'FORGERY', '1.16 Admin token accepted for student endpoint', `Status: ${r.status}`);

  // 1.17 Submit with teacher token (privilege escalation)
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test'
  }, { Authorization: `Bearer ${T.teacher}` });
  if (r.status === 403 || r.status === 401) pass('1.17 Teacher token rejected');
  else vuln('HIGH', 'FORGERY', '1.17 Teacher token accepted for student endpoint', `Status: ${r.status}`);

  // 1.18 Double submission (idempotency)
  const doubleSub = {
    challengeId: 'ch-test-duplicate',
    passed: true,
    output: 'test',
    signature: 'test'
  };
  const r1 = await req('POST', '/api/v1/student/challenge/submit', doubleSub, { Authorization: `Bearer ${T.student}` });
  const r2 = await req('POST', '/api/v1/student/challenge/submit', doubleSub, { Authorization: `Bearer ${T.student}` });
  if (r1.status === r2.status || r2.status === 409) pass('1.18 Double submission handled');
  else info(`1.18 Double submission: ${r1.status} -> ${r2.status}`);

  // 1.19 Submit with future timestamp
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    timestamp: Date.now() + 86400000 // Tomorrow
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.19 Future timestamp rejected');
  else info(`1.19 Future timestamp: ${r.status}`);

  // 1.20 Submit with past timestamp (replay)
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    timestamp: Date.now() - 86400000 // Yesterday
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('1.20 Old timestamp rejected');
  else vuln('MEDIUM', 'FORGERY', '1.20 Old timestamp accepted', `Status: ${r.status}`);

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 2: HMAC SIGNATURE FORGERY (15 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ 2. HMAC SIGNATURE FORGERY ━━━');

  // 2.1 Empty HMAC signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: '',
    hmac: ''
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.1 Empty HMAC rejected');
  else vuln('HIGH', 'HMAC', '2.1 Empty HMAC accepted', `Status: ${r.status}`);

  // 2.2 Null HMAC signature
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: null
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.2 Null HMAC rejected');
  else vuln('HIGH', 'HMAC', '2.2 Null HMAC accepted', `Status: ${r.status}`);

  // 2.3 Forged HMAC with common secrets
  const commonSecrets = ['secret', 'password', '12345678', 'dhad-studio', 'jwt-secret', 'hmac-secret'];
  for (const secret of commonSecrets) {
    const forgedHmac = crypto.createHmac('sha256', secret).update('test-data').digest('hex');
    r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: 'ch-real',
      passed: true,
      output: 'test',
      signature: 'test',
      hmac: forgedHmac
    }, { Authorization: `Bearer ${T.student}` });
    if (r.status !== 200) {
      pass(`2.3 Forged HMAC (${secret}) rejected`);
      break;
    }
  }

  // 2.4 HMAC with wrong algorithm
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: crypto.createHash('md5').update('test-data').digest('hex')
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.4 MD5 HMAC rejected');
  else vuln('MEDIUM', 'HMAC', '2.4 MD5 HMAC accepted', `Status: ${r.status}`);

  // 2.5 HMAC with truncated hash
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: 'abc123'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.5 Truncated HMAC rejected');
  else vuln('MEDIUM', 'HMAC', '2.5 Truncated HMAC accepted', `Status: ${r.status}`);

  // 2.6 HMAC replay attack (old timestamp)
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: 'replay-attack',
    timestamp: Date.now() - 3600000 // 1 hour ago
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.6 HMAC replay rejected');
  else vuln('HIGH', 'HMAC', '2.6 HMAC replay accepted', `Status: ${r.status}`);

  // 2.7 HMAC with future timestamp
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: 'future-hmac',
    timestamp: Date.now() + 3600000 // 1 hour ahead
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.7 Future HMAC timestamp rejected');
  else vuln('MEDIUM', 'HMAC', '2.7 Future HMAC timestamp accepted', `Status: ${r.status}`);

  // 2.8 HMAC with no timestamp
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: 'no-timestamp-hmac'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.8 HMAC without timestamp rejected');
  else vuln('MEDIUM', 'HMAC', '2.8 HMAC without timestamp accepted', `Status: ${r.status}`);

  // 2.9 HMAC with special characters
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: '<script>alert(1)</script>'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.9 XSS in HMAC rejected');
  else vuln('MEDIUM', 'HMAC', '2.9 XSS in HMAC accepted', `Status: ${r.status}`);

  // 2.10 HMAC with SQL injection
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: "' OR '1'='1"
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.10 SQLi in HMAC rejected');
  else vuln('CRITICAL', 'HMAC', '2.10 SQLi in HMAC accepted', `Status: ${r.status}`);

  // 2.11 HMAC with very long value
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: 'A'.repeat(10000)
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.11 Long HMAC rejected');
  else vuln('MEDIUM', 'HMAC', '2.11 Long HMAC accepted', `Status: ${r.status}`);

  // 2.12 HMAC with null byte
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: 'abc\x00def'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('2.12 Null byte in HMAC rejected');
  else vuln('HIGH', 'HMAC', '2.12 Null byte in HMAC accepted', `Status: ${r.status}`);

  // 2.13 HMAC timing attack test
  const timingTests = [];
  for (let i = 0; i < 10; i++) {
    const start = Date.now();
    await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: 'ch-real',
      passed: true,
      output: 'test',
      signature: 'test',
      hmac: 'a'.repeat(64)
    }, { Authorization: `Bearer ${T.student}` });
    timingTests.push(Date.now() - start);
  }
  const avg = timingTests.reduce((a, b) => a + b, 0) / timingTests.length;
  const maxDiff = Math.max(...timingTests) - Math.min(...timingTests);
  if (maxDiff < 100) pass(`2.13 HMAC timing consistent (diff: ${maxDiff}ms)`);
  else info(`2.13 HMAC timing varies: ${maxDiff}ms`);

  // 2.14 HMAC with empty body
  r = await req('POST', '/api/v1/student/challenge/submit', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 400) pass('2.14 Empty body rejected');
  else vuln('MEDIUM', 'HMAC', '2.14 Empty body accepted', `Status: ${r.status}`);

  // 2.15 HMAC with malformed JSON
  r = await req('POST', '/api/v1/student/challenge/submit', '{invalid json', { Authorization: `Bearer ${T.student}` });
  if (r.status === 400) pass('2.15 Malformed JSON rejected');
  else vuln('MEDIUM', 'HMAC', '2.15 Malformed JSON accepted', `Status: ${r.status}`);

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 3: ANSWER COPYING & SHARING (10 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ 3. ANSWER COPYING & SHARING ━━━');

  // 3.1 Student accessing other student's submission
  r = await req('GET', '/api/v1/student/submissions', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 200) {
    const submissions = Array.isArray(r.body) ? r.body : r.body?.submissions || [];
    if (submissions.length > 0) {
      const otherId = submissions[0]?.studentId;
      if (otherId && otherId !== JSON.parse(Buffer.from(T.student.split('.')[1], 'base64url').toString()).id) {
        r = await req('GET', `/api/v1/student/${otherId}/submissions`, null, { Authorization: `Bearer ${T.student}` });
        if (r.status === 403) pass('3.1 Cannot access other student submissions');
        else vuln('CRITICAL', 'COPY', '3.1 Can access other student submissions', `Status: ${r.status}`);
      } else pass('3.1 Only own submissions visible');
    } else pass('3.1 No submissions to test');
  } else info(`3.1 Submissions endpoint: ${r.status}`);

  // 3.2 Student accessing challenge answers
  r = await req('GET', '/api/v1/challenges/answers', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.2 Challenge answers not accessible');
  else vuln('HIGH', 'COPY', '3.2 Challenge answers accessible', `Status: ${r.status}`);

  // 3.3 Student accessing expected output
  r = await req('GET', '/api/v1/challenges/expected-output', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.3 Expected output not accessible');
  else vuln('HIGH', 'COPY', '3.3 Expected output accessible', `Status: ${r.status}`);

  // 3.4 Student accessing solution code
  r = await req('GET', '/api/v1/challenges/solution', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.4 Solution code not accessible');
  else vuln('HIGH', 'COPY', '3.4 Solution code accessible', `Status: ${r.status}`);

  // 3.5 Student accessing hints
  r = await req('GET', '/api/v1/challenges/hints', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.5 Hints not accessible before attempt');
  else info(`3.5 Hints endpoint: ${r.status}`);

  // 3.6 Student accessing assessment answers
  r = await req('GET', '/api/v1/assessments/answers', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.6 Assessment answers not accessible');
  else vuln('HIGH', 'COPY', '3.6 Assessment answers accessible', `Status: ${r.status}`);

  // 3.7 Student accessing other student's assessment result
  r = await req('GET', '/api/v1/assessments/results', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 200) {
    const results = r.body?.results || r.body || [];
    if (Array.isArray(results) && results.length > 0) {
      const otherResult = results.find(r => r.studentId !== JSON.parse(Buffer.from(T.student.split('.')[1], 'base64url').toString()).id);
      if (otherResult) vuln('CRITICAL', 'COPY', '3.7 Other student results visible', 'Other student data found');
      else pass('3.7 Only own results visible');
    } else pass('3.7 No results to test');
  } else pass(`3.7 Results endpoint: ${r.status}`);

  // 3.8 Student accessing teacher's assessment key
  r = await req('GET', '/api/v1/assessments/key', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.8 Assessment key not accessible');
  else vuln('CRITICAL', 'COPY', '3.8 Assessment key accessible', `Status: ${r.status}`);

  // 3.9 Student accessing grading rubric
  r = await req('GET', '/api/v1/assessments/rubric', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.9 Grading rubric not accessible');
  else vuln('HIGH', 'COPY', '3.9 Grading rubric accessible', `Status: ${r.status}`);

  // 3.10 Student accessing challenge test cases
  r = await req('GET', '/api/v1/challenges/test-cases', null, { Authorization: `Bearer ${T.student}` });
  if (r.status === 403 || r.status === 404) pass('3.10 Test cases not accessible');
  else vuln('HIGH', 'COPY', '3.10 Test cases accessible', `Status: ${r.status}`);

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 4: TIME MANIPULATION (10 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ 4. TIME MANIPULATION ━━━');

  // 4.1 Submit with timestamp in the past
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: new Date(Date.now() - 86400000).toISOString() // Yesterday
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.1 Past timestamp rejected');
  else vuln('MEDIUM', 'TIME', '4.1 Past timestamp accepted', `Status: ${r.status}`);

  // 4.2 Submit with timestamp in the future
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: new Date(Date.now() + 86400000).toISOString() // Tomorrow
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.2 Future timestamp rejected');
  else vuln('MEDIUM', 'TIME', '4.2 Future timestamp accepted', `Status: ${r.status}`);

  // 4.3 Submit with very old timestamp
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: '2020-01-01T00:00:00.000Z'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.3 Very old timestamp rejected');
  else vuln('MEDIUM', 'TIME', '4.3 Very old timestamp accepted', `Status: ${r.status}`);

  // 4.4 Submit with invalid timestamp format
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: 'not-a-date'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.4 Invalid timestamp rejected');
  else vuln('MEDIUM', 'TIME', '4.4 Invalid timestamp accepted', `Status: ${r.status}`);

  // 4.5 Submit with null timestamp
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: null
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.5 Null timestamp rejected');
  else vuln('MEDIUM', 'TIME', '4.5 Null timestamp accepted', `Status: ${r.status}`);

  // 4.6 Submit with timestamp as number
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: Date.now()
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.6 Numeric timestamp rejected');
  else info(`4.6 Numeric timestamp: ${r.status}`);

  // 4.7 Submit with timezone manipulation
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: '2024-01-01T00:00:00.000+14:00' // Extreme timezone
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.7 Timezone manipulation rejected');
  else info(`4.7 Timezone manipulation: ${r.status}`);

  // 4.8 Submit with leap second
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: '2024-06-30T23:59:60.000Z'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.8 Leap second rejected');
  else info(`4.8 Leap second: ${r.status}`);

  // 4.9 Submit with negative timestamp
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: -1000000000
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.9 Negative timestamp rejected');
  else vuln('MEDIUM', 'TIME', '4.9 Negative timestamp accepted', `Status: ${r.status}`);

  // 4.10 Submit with extremely large timestamp
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    submittedAt: 99999999999999
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('4.10 Extremely large timestamp rejected');
  else vuln('MEDIUM', 'TIME', '4.10 Extremely large timestamp accepted', `Status: ${r.status}`);

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 5: XP & SCORE MANIPULATION (15 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ 5. XP & SCORE MANIPULATION ━━━');

  // 5.1 Direct XP modification endpoint
  r = await req('POST', '/api/v1/student/xp', { xp: 999999 }, { Authorization: `Bearer ${T.student}` });
  if (r.status === 404) pass('5.1 XP endpoint not found (good)');
  else if (r.status === 403) pass('5.1 XP modification forbidden');
  else vuln('CRITICAL', 'XP', '5.1 XP endpoint accessible', `Status: ${r.status}`);

  // 5.2 Direct level modification
  r = await req('POST', '/api/v1/student/level', { level: 99 }, { Authorization: `Bearer ${T.student}` });
  if (r.status === 404) pass('5.2 Level endpoint not found (good)');
  else if (r.status === 403) pass('5.2 Level modification forbidden');
  else vuln('CRITICAL', 'XP', '5.2 Level endpoint accessible', `Status: ${r.status}`);

  // 5.3 Direct streak modification
  r = await req('POST', '/api/v1/student/streak', { streak: 999 }, { Authorization: `Bearer ${T.student}` });
  if (r.status === 404) pass('5.3 Streak endpoint not found (good)');
  else if (r.status === 403) pass('5.3 Streak modification forbidden');
  else vuln('CRITICAL', 'XP', '5.3 Streak endpoint accessible', `Status: ${r.status}`);

  // 5.4 Direct achievements modification
  r = await req('POST', '/api/v1/student/achievements', { achievementId: 'admin-badge' }, { Authorization: `Bearer ${T.student}` });
  if (r.status === 404) pass('5.4 Achievement endpoint not found (good)');
  else if (r.status === 403) pass('5.4 Achievement modification forbidden');
  else vuln('HIGH', 'XP', '5.4 Achievement endpoint accessible', `Status: ${r.status}`);

  // 5.5 Challenge submission with manipulated XP reward
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: 999999
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.5 XP reward in body rejected');
  else vuln('HIGH', 'XP', '5.5 XP reward in body accepted', `Status: ${r.status}`);

  // 5.6 Challenge submission with negative XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: -1000
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.6 Negative XP rejected');
  else vuln('HIGH', 'XP', '5.6 Negative XP accepted', `Status: ${r.status}`);

  // 5.7 Challenge submission with zero XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: 0
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.7 Zero XP rejected');
  else info(`5.7 Zero XP: ${r.status}`);

  // 5.8 Challenge submission with float XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: 99.99
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.8 Float XP rejected');
  else info(`5.8 Float XP: ${r.status}`);

  // 5.9 Challenge submission with string XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: '999999'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.9 String XP rejected');
  else info(`5.9 String XP: ${r.status}`);

  // 5.10 Challenge submission with array XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: [1, 2, 3]
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.10 Array XP rejected');
  else info(`5.10 Array XP: ${r.status}`);

  // 5.11 Challenge submission with object XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: { amount: 999999 }
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.11 Object XP rejected');
  else info(`5.11 Object XP: ${r.status}`);

  // 5.12 Challenge submission with Infinity XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: Infinity
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.12 Infinity XP rejected');
  else vuln('HIGH', 'XP', '5.12 Infinity XP accepted', `Status: ${r.status}`);

  // 5.13 Challenge submission with NaN XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: NaN
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.13 NaN XP rejected');
  else vuln('HIGH', 'XP', '5.13 NaN XP accepted', `Status: ${r.status}`);

  // 5.14 Challenge submission with overflow XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: Number.MAX_SAFE_INTEGER
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.14 Overflow XP rejected');
  else vuln('HIGH', 'XP', '5.14 Overflow XP accepted', `Status: ${r.status}`);

  // 5.15 Challenge submission with underflow XP
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-real',
    passed: true,
    output: 'test',
    signature: 'test',
    xpReward: Number.MIN_SAFE_INTEGER
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('5.15 Underflow XP rejected');
  else vuln('HIGH', 'XP', '5.15 Underflow XP accepted', `Status: ${r.status}`);

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 6: REPLAY ATTACKS (10 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ 6. REPLAY ATTACKS ━━━');

  // 6.1 Replay same submission multiple times
  const replayPayload = {
    challengeId: 'ch-replay-test',
    passed: true,
    output: 'replay-test',
    signature: 'replay-sig'
  };
  const replayResults = [];
  for (let i = 0; i < 5; i++) {
    replayResults.push(await req('POST', '/api/v1/student/challenge/submit', replayPayload, { Authorization: `Bearer ${T.student}` }));
  }
  const replayStatuses = replayResults.map(r => r.status);
  const uniqueStatuses = [...new Set(replayStatuses)];
  if (uniqueStatuses.length === 1) pass('6.1 Replay returns consistent status');
  else info(`6.1 Replay statuses: ${replayStatuses.join(', ')}`);

  // 6.2 Replay with different token (same user)
  const s2 = await req('POST', '/api/v1/auth/login', { username: '1099990001', password: 'Test1234!' });
  if (s2.body?.accessToken) {
    r = await req('POST', '/api/v1/student/challenge/submit', replayPayload, { Authorization: `Bearer ${s2.body.accessToken}` });
    if (r.status !== 200) pass('6.2 Replay with new token rejected');
    else info(`6.2 Replay with new token: ${r.status}`);
  }

  // 6.3 Replay old HMAC
  r = await req('POST', '/api/v1/student/challenge/submit', {
    ...replayPayload,
    hmac: 'old-hmac-value',
    timestamp: Date.now() - 3600000
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('6.3 Old HMAC replay rejected');
  else vuln('HIGH', 'REPLAY', '6.3 Old HMAC replay accepted', `Status: ${r.status}`);

  // 6.4 Replay with modified data
  r = await req('POST', '/api/v1/student/challenge/submit', {
    ...replayPayload,
    output: 'modified-output'
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('6.4 Modified replay rejected');
  else info(`6.4 Modified replay: ${r.status}`);

  // 6.5 Replay assessment submission
  r = await req('POST', '/api/v1/assessments/submit', {
    assessmentId: 'fake-assessment',
    answers: [{ questionId: 'q1', answer: 'A' }]
  }, { Authorization: `Bearer ${T.student}` });
  if (r.status !== 200) pass('6.5 Fake assessment submission rejected');
  else vuln('HIGH', 'REPLAY', '6.5 Fake assessment accepted', `Status: ${r.status}`);

  // 6.6-6.10 Additional replay tests
  for (let i = 6; i <= 10; i++) {
    r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: `ch-replay-${i}`,
      passed: true,
      output: `replay-${i}`,
      signature: `sig-${i}`,
      timestamp: Date.now() - (i * 86400000)
    }, { Authorization: `Bearer ${T.student}` });
    if (r.status !== 200) pass(`6.${i} Old submission rejected`);
    else info(`6.${i} Old submission: ${r.status}`);
  }

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 7: RACE CONDITIONS (10 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ 7. RACE CONDITIONS ━━━');

  // 7.1-7.5 Concurrent challenge submissions
  for (let i = 1; i <= 5; i++) {
    const racePromises = [];
    for (let j = 0; j < 10; j++) {
      racePromises.push(req('POST', '/api/v1/student/challenge/submit', {
        challengeId: `ch-race-${i}`,
        passed: true,
        output: `race-${j}`,
        signature: `sig-${j}`
      }, { Authorization: `Bearer ${T.student}` }));
    }
    const raceResults = await Promise.all(racePromises);
    const successCount = raceResults.filter(r => r.status === 200).length;
    if (successCount <= 1) pass(`7.${i} Race condition: ${successCount}/10 succeeded`);
    else vuln('HIGH', 'RACE', `7.${i} Race condition: ${successCount}/10 succeeded`, 'Multiple wins');
  }

  // 7.6-7.10 Concurrent login attempts
  for (let i = 6; i <= 10; i++) {
    const loginPromises = [];
    for (let j = 0; j < 10; j++) {
      loginPromises.push(req('POST', '/api/v1/auth/login', { username: 'teacher@dhad.com', password: 'Test1234!' }));
    }
    const loginResults = await Promise.all(loginPromises);
    const successCount = loginResults.filter(r => r.status === 200).length;
    if (successCount <= 1) pass(`7.${i} Login race: ${successCount}/10 succeeded`);
    else info(`7.${i} Login race: ${successCount}/10 succeeded`);
  }

  // ═════════════════════════════════════════════════════════════════════════
  // CATEGORY 8: PROCTORING BYPASS (10 tests)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ 8. PROCTORING BYPASS ━━━');

  // 8.1-8.5 Tab switch detection bypass
  for (let i = 1; i <= 5; i++) {
    r = await req('POST', '/api/v1/student/activity', {
      type: 'tab_switch',
      assessmentId: `assess-${i}`,
      timestamp: Date.now()
    }, { Authorization: `Bearer ${T.student}` });
    if (r.status === 404) pass(`8.${i} Tab switch endpoint: 404`);
    else info(`8.${i} Tab switch: ${r.status}`);
  }

  // 8.6-8.10 Copy-paste detection bypass
  for (let i = 6; i <= 10; i++) {
    r = await req('POST', '/api/v1/student/activity', {
      type: 'copy_paste',
      assessmentId: `assess-${i}`,
      timestamp: Date.now()
    }, { Authorization: `Bearer ${T.student}` });
    if (r.status === 404) pass(`8.${i} Copy-paste endpoint: 404`);
    else info(`8.${i} Copy-paste: ${r.status}`);
  }

  // ═════════════════════════════════════════════════════════════════════════
  // SUMMARY
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n╔══════════════════════════════════════════════════════════════╗');
  console.log('║                  ANTI-CHEAT TEST RESULTS                   ║');
  console.log('╚══════════════════════════════════════════════════════════════╝');

  const bySeverity = {};
  F.forEach(f => { bySeverity[f.severity] = (bySeverity[f.severity] || 0) + 1; });

  console.log(`  🔴 CRITICAL: ${bySeverity.CRITICAL || 0}`);
  console.log(`  🟠 HIGH:     ${bySeverity.HIGH || 0}`);
  console.log(`  🟡 MEDIUM:   ${bySeverity.MEDIUM || 0}`);
  console.log(`  🔵 LOW:      ${bySeverity.LOW || 0}`);
  console.log(`  ─────────────────────`);
  console.log(`  ✅ Passed:   ${PASSED}`);
  console.log(`  ❌ Failed:   ${FAILED}`);
  console.log(`  📊 Total:    ${PASSED + FAILED}`);
  console.log(`  🎯 Score:    ${((PASSED / (PASSED + FAILED)) * 100).toFixed(1)}%`);

  if (F.length > 0) {
    console.log('\n━━━ DETAILED FINDINGS ━━━');
    F.forEach((f, i) => {
      console.log(`\n  ${i + 1}. [${f.severity}] ${f.category}: ${f.title}`);
      if (f.detail) console.log(`     ${f.detail}`);
    });
  }

  const report = {
    timestamp: new Date().toISOString(),
    findings: F,
    summary: { ...bySeverity, passed: PASSED, failed: FAILED, total: PASSED + FAILED },
    score: `${((PASSED / (PASSED + FAILED)) * 100).toFixed(1)}%`
  };
  require('fs').writeFileSync('C:/Projects/dhad-studio/tests/anti_cheat_report.json', JSON.stringify(report, null, 2));
  console.log('\n📄 Report saved to tests/anti_cheat_report.json');
}

main().catch(console.error);
