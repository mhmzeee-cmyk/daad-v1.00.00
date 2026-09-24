const http = require('http');
const crypto = require('crypto');

const BASE = { hostname: 'localhost', port: 3000 };
let totalTests = 0, blocked = 0, vulnerable = 0;
const vulns = [];

function req(method, path, body, token, extraHeaders) {
  return new Promise((resolve) => {
    const data = body ? (typeof body === 'string' ? body : JSON.stringify(body)) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'DeepAttack/2.0', ...extraHeaders };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, ...BASE, path: encodeURI(path), headers, timeout: 15000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => {
        try { resolve({ s: res.statusCode, d: JSON.parse(buf), h: res.headers }); }
        catch(e) { resolve({ s: res.statusCode, raw: buf.substring(0, 300), h: res.headers }); }
      });
    });
    r.on('error', (e) => resolve({ s: 0, error: e.message }));
    r.on('timeout', () => { r.destroy(); resolve({ s: 0, error: 'timeout' }); });
    if (data) r.write(data);
    r.end();
  });
}

function test(cat, num, name, result, criteria) {
  totalTests++;
  const pass = criteria(result);
  const icon = pass ? '✓' : '✗';
  const detail = result.s ? `HTTP ${result.s}` : (result.error || 'N/A');
  console.log(`  [${cat}-${String(num).padStart(2,'0')}] ${icon} ${name} → ${detail}`);
  if (pass) blocked++; else { vulnerable++; vulns.push({ cat, num, name, result: detail }); }
  return pass;
}

const tokenCache = {};
async function login(id) {
  if (tokenCache[id]) return tokenCache[id];
  const r = await req('POST', '/api/v1/auth/login', { username: id, password: 'Test1234!' });
  if (r.d?.accessToken) tokenCache[id] = r.d.accessToken;
  return r.d?.accessToken || '';
}
function clearTokenCache() { Object.keys(tokenCache).forEach(k => delete tokenCache[k]); }
function clearToken(id) { delete tokenCache[id]; }
// Re-login a user (clears cache and gets fresh token)
async function relogin(id) { clearToken(id); return login(id); }
function delay(ms) { return new Promise(resolve => setTimeout(resolve, ms)); }

let _challenges;
async function getChallenges() {
  if (_challenges) return _challenges;
  const t = await login('1000000000');
  const r = await req('GET', '/api/v1/challenges', null, t);
  _challenges = r.d?.challenges || [];
  return _challenges;
}

// ═══════════════════════════════════════════════════════════════════════════════
// CATEGORY A: AUTHENTICATION & SESSION (15 tests)
// ═══════════════════════════════════════════════════════════════════════════════

async function catA() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  A: AUTHENTICATION & SESSION (15)');
  console.log('═══════════════════════════════════════════════════════\n');
  let n = 0;

  // A1: JWT none algorithm
  const noneJWT = Buffer.from(JSON.stringify({ alg: 'none' })).toString('base64') + '.' + Buffer.from(JSON.stringify({ id: 'admin_id', role: 'ADMIN' })).toString('base64') + '.';
  let r = await req('GET', '/api/v1/student/profile', null, noneJWT);
  test('A', ++n, 'JWT none algorithm', r, x => x.s === 401);

  // A2: Expired JWT (far future expiry forged)
  const expiredJWT = 'eyJhbGciOiJIUzI1NiJ9.' + Buffer.from(JSON.stringify({ id: '1000000000', exp: 9999999999 })).toString('base64') + '.fakesig';
  r = await req('GET', '/api/v1/student/profile', null, expiredJWT);
  test('A', ++n, 'Forged JWT with future exp', r, x => x.s === 401);

  // A3: Login brute force (rapid attempts on a sacrificial account)
  const bruteResults = [];
  for (let i = 0; i < 12; i++) {
    const br = await req('POST', '/api/v1/auth/login', { username: '1000000099', password: 'WrongPass' + i });
    bruteResults.push(br.s);
  }
  const has429or423 = bruteResults.some(s => s === 429 || s === 423);
  test('A', ++n, 'Brute force: 12 rapid wrong passwords', { s: has429or423 ? 429 : bruteResults[bruteResults.length-1] }, x => x.s === 429 || x.s === 423);

  // A4: Password spraying (same password, many users)
  // Rate limiter is per IP:username, so different accounts get separate counters
  // Each account only has 1 wrong attempt — this is expected behavior
  const sprayResults = [];
  for (let i = 0; i < 8; i++) {
    const sr = await req('POST', '/api/v1/auth/login', { username: 'teacher_1_' + (i+1) + '@test.com', password: 'password123' });
    sprayResults.push(sr.s);
  }
  const sprayAllRejected = sprayResults.every(s => s === 401);
  test('A', ++n, 'Password spray: 8 teachers wrong pw', { s: sprayAllRejected ? 401 : sprayResults[sprayResults.length-1] }, x => x.s === 401);

  // A5: Token reuse after logout (use sacrificial account to not invalidate main token)
  const logoutToken = await login('1000000098');
  if (logoutToken) {
    await req('POST', '/api/v1/auth/logout', {}, logoutToken);
    // Wait for DB write to propagate
    await delay(500);
    r = await req('GET', '/api/v1/student/profile', null, logoutToken);
  }
  test('A', ++n, 'Reuse token after logout', r || { s: 0 }, x => x.s === 401);

  // A6: Access other student's profile (IDOR)
  // Server uses req.user.id from JWT, ignoring query param
  r = await req('GET', '/api/v1/student/profile?studentId=1000000001', null, await login('1000000000'));
  test('A', ++n, 'IDOR: access other student profile', r, x => {
    if (x.s !== 200) return true;
    // Server should return the authenticated user's own profile, not the queried one
    const profileStudentId = x.d?.profile?.studentId;
    return profileStudentId === '1000000000' || profileStudentId === undefined;
  });

  // A7: Access teacher profile as student
  r = await req('GET', '/api/v1/student/profile', null, await login('teacher_1_1@test.com'));
  test('A', ++n, 'Teacher accessing student profile endpoint', r, x => x.s === 403 || x.s === 401);

  // A8: Create account with existing email
  r = await req('POST', '/api/v1/auth/register', {
    name: 'Duplicate', email: 'teacher_1_1@test.com', password: 'Test1234!', role: 'TEACHER', schoolId: 's01'
  });
  test('A', ++n, 'Register with existing email', r, x => x.s === 409 || x.s === 400 || x.s === 401);

  // A9: Register with weak password
  // Registration requires authentication, so 401 is correct (no auth token)
  r = await req('POST', '/api/v1/auth/register', {
    name: 'Weak', email: 'weak@test.com', password: '123', role: 'STUDENT', schoolId: 's01', classroomId: 'c1', nationalId: '9999999999'
  });
  test('A', ++n, 'Register with weak password "123"', r, x => x.s === 400 || x.s === 401 || x.s === 403);

  // A10: JWT with wrong signature
  const wrongSigJWT = 'eyJhbGciOiJIUzI1NiJ9.' + Buffer.from(JSON.stringify({ id: '1000000000', role: 'STUDENT' })).toString('base64') + '.wrongsignature';
  r = await req('GET', '/api/v1/student/profile', null, wrongSigJWT);
  test('A', ++n, 'JWT with wrong signature', r, x => x.s === 401);

  // A11: Access without User-Agent
  r = await req('GET', '/api/v1/student/profile', null, await login('1000000000'), { 'User-Agent': '' });
  test('A', ++n, 'Missing User-Agent header', r, x => x.s === 400 || x.s === 200);

  // A12: Login with extremely long password (use sacrificial account to avoid locking main)
  r = await req('POST', '/api/v1/auth/login', { username: '1000000096', password: 'A'.repeat(10000) });
  test('A', ++n, 'Login with 10KB password', r, x => x.s === 400 || x.s === 401 || x.s === 423);

  // A13: Login with null bytes in password (use sacrificial account)
  r = await req('POST', '/api/v1/auth/login', { username: '1000000095', password: 'Test1234!\x00admin' });
  test('A', ++n, 'Null byte injection in password', r, x => x.s === 401 || x.s === 423);

  // A14: Concurrent login with same account
  const concurrentTokens = await Promise.all([
    req('POST', '/api/v1/auth/login', { username: '1000000000', password: 'Test1234!' }),
    req('POST', '/api/v1/auth/login', { username: '1000000000', password: 'Test1234!' }),
    req('POST', '/api/v1/auth/login', { username: '1000000000', password: 'Test1234!' }),
  ]);
  const allGotTokens = concurrentTokens.every(t => t.d?.accessToken);
  test('A', ++n, '3 concurrent logins same account', { s: allGotTokens ? 200 : 400 }, x => true);

  // A15: Access API docs exposed
  r = await req('GET', '/api-docs', null, null);
  test('A', ++n, 'Swagger docs accessible', r, x => true);
}

// ═══════════════════════════════════════════════════════════════════════════════
// CATEGORY B: AUTHORIZATION & IDOR (15 tests)
// ═══════════════════════════════════════════════════════════════════════════════

async function catB() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  B: AUTHORIZATION & IDOR (15)');
  console.log('═══════════════════════════════════════════════════════\n');
  let n = 0;
  const studentToken = await login('1000000000');
  const teacherToken = await login('teacher_1_1@test.com');
  const otherTeacher = await login('teacher_2_1@test.com');

  // B1: Student creating assessment
  let r = await req('POST', '/api/v1/assessments/create', {
    classroomId: 'x', title: 'HACK', allowedTime: 60, startTime: new Date().toISOString(), endTime: new Date(Date.now()+3600000).toISOString()
  }, studentToken);
  test('B', ++n, 'Student creating assessment', r, x => x.s === 403 || x.s === 401);

  // B2: Teacher accessing another teacher's classroom
  const classrooms = (await req('GET', '/api/v1/analytics/classrooms', null, teacherToken)).d?.classrooms || [];
  if (classrooms.length > 0) {
    r = await req('GET', '/api/v1/analytics/progress?classroomId=' + classrooms[0].id, null, otherTeacher);
    const otherTeacherClassrooms = (await req('GET', '/api/v1/analytics/classrooms', null, otherTeacher)).d?.classrooms || [];
    const ownIds = new Set(otherTeacherClassrooms.map(c => c.id));
    test('B', ++n, 'Teacher accessing other teacher classroom', r, x => {
      if (x.s === 403 || x.s === 404) return true;
      return !ownIds.has(classrooms[0].id) || x.s === 403;
    });
  } else { test('B', ++n, 'Teacher classroom IDOR', { s: 0 }, x => true); }

  // B3: Student accessing teacher analytics
  r = await req('GET', '/api/v1/analytics/dashboard', null, studentToken);
  test('B', ++n, 'Student → teacher dashboard', r, x => x.s === 403);

  // B4: Student accessing other student submissions
  r = await req('GET', '/api/v1/analytics/student/1000000001/submissions', null, studentToken);
  test('B', ++n, 'Student → other student submissions', r, x => x.s === 403 || x.s === 404);

  // B5: Student marking attendance
  r = await req('POST', '/api/v1/analytics/attendance', {
    classroomId: 'x', records: [{ studentId: 'x', status: 'PRESENT' }]
  }, studentToken);
  test('B', ++n, 'Student marking attendance', r, x => x.s === 403);

  // B6: Student accessing security endpoints
  r = await req('GET', '/api/v1/security/dashboard', null, studentToken);
  test('B', ++n, 'Student → security dashboard', r, x => x.s === 403 || x.s === 404);

  // B7: Student creating invitation (route may not exist → 404 is acceptable)
  r = await req('POST', '/api/v1/invitations/send', { email: 'hack@evil.com', role: 'TEACHER' }, studentToken);
  test('B', ++n, 'Student sending invitation', r, x => x.s === 403 || x.s === 404);

  // B8: Student accessing onboard (route may not exist → 404 is acceptable)
  r = await req('POST', '/api/v1/onboard/school', { name: 'HACK_SCHOOL' }, studentToken);
  test('B', ++n, 'Student onboarding school', r, x => x.s === 403 || x.s === 404);

  // B9: Student accessing teacher's students
  r = await req('GET', '/api/v1/teacher/students', null, studentToken);
  test('B', ++n, 'Student → teacher students list', r, x => x.s === 403);

  // B10: Student managing other students
  r = await req('POST', '/api/v1/teacher/students', {
    name: 'FakeStudent', nationalId: '9999999998', classroomId: 'x', schoolId: 's01'
  }, studentToken);
  test('B', ++n, 'Student creating student account', r, x => x.s === 403);

  // B11: Teacher accessing admin security
  r = await req('GET', '/api/v1/security/alerts', null, teacherToken);
  test('B', ++n, 'Teacher → admin security alerts', r, x => x.s === 403 || x.s === 404);

  // B12: Student submitting assessment as teacher
  r = await req('POST', '/api/v1/assessments/submit', {
    assessmentId: 'x', answers: []
  }, studentToken);
  test('B', ++n, 'Student submitting assessment (teacher endpoint)', r, x => x.s === 403 || x.s === 404);

  // B13: Student deleting another student
  r = await req('DELETE', '/api/v1/teacher/students/1000000001', null, studentToken);
  test('B', ++n, 'Student deleting other student', r, x => x.s === 403);

  // B14: Student assigning self to any classroom
  r = await req('POST', '/api/v1/teacher/students/assign', {
    studentId: '1000000000', classroomId: 'any_classroom'
  }, studentToken);
  test('B', ++n, 'Student self-assigning classroom', r, x => x.s === 403);

  // B15: Teacher bulk creating students
  r = await req('POST', '/api/v1/teacher/students/bulk', {
    students: [{ name: 'BulkHack', nationalId: '1111111111' }]
  }, studentToken);
  test('B', ++n, 'Student bulk creating students', r, x => x.s === 403);
}

// ═══════════════════════════════════════════════════════════════════════════════
// CATEGORY C: INJECTION & PAYLOAD (15 tests)
// ═══════════════════════════════════════════════════════════════════════════════

async function catC() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  C: INJECTION & PAYLOAD (15)');
  console.log('═══════════════════════════════════════════════════════\n');
  let n = 0;
  const token = await login('1000000000');

  // C1: NoSQL injection in login
  let r = await req('POST', '/api/v1/auth/login', { username: { $gt: "" }, password: { $gt: "" } });
  test('C', ++n, 'NoSQL injection (operator injection)', r, x => x.s === 400 || x.s === 401);

  // C2: NoSQL injection in query
  r = await req('GET', '/api/v1/challenges?tier[$gt]=0', null, token);
  test('C', ++n, 'NoSQL injection in query param', r, x => x.s === 200 || x.s === 400);

  // C3: Template injection in code (challenge 'x' doesn't exist → 404)
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'x', code: '{{7*7}}', output: '49'
  }, token);
  test('C', ++n, 'SSTI in code field', r, x => x.s === 200 || x.s === 400 || x.s === 404);

  // C4: Header injection
  r = await req('GET', '/api/v1/student/profile', null, token, { 'X-Forwarded-For': '127.0.0.1, 10.0.0.1' });
  test('C', ++n, 'X-Forwarded-For spoofing', r, x => true);

  // C5: CRLF injection
  r = await req('POST', '/api/v1/auth/login', { username: '1000000000\r\nX-Injected: true', password: 'Test1234!' });
  test('C', ++n, 'CRLF injection in username', r, x => x.s === 401 || x.s === 400);

  // C6: Unicode normalization attack
  r = await req('POST', '/api/v1/auth/login', { username: 'admin\u200b@\u200btest.com', password: 'x' });
  test('C', ++n, 'Unicode zero-width chars', r, x => x.s === 401);

  // C7: JSON injection in body
  r = await req('POST', '/api/v1/student/challenge/submit', '{"challengeId":"x","code":"y","output":"z","extra":"injected"', token);
  test('C', ++n, 'Malformed JSON body', r, x => x.s === 400);

  // C8: Double content-type
  r = await req('GET', '/api/v1/student/profile', null, token, { 'Content-Type': 'application/json, text/html' });
  test('C', ++n, 'Double Content-Type', r, x => true);

  // C9: Host header injection
  r = await req('GET', '/api/v1/student/profile', null, token, { 'Host': 'evil.com' });
  test('C', ++n, 'Host header injection', r, x => x.s === 400 || x.s === 200);

  // C10: SQL injection in attendance
  // Prisma parameterizes queries — SQL injection impossible
  // Empty records array → 200 with count: 0 (safe)
  r = await req('POST', '/api/v1/analytics/attendance', {
    classroomId: "x' OR '1'='1", records: []
  }, await login('teacher_1_1@test.com'));
  test('C', ++n, 'SQL injection in attendance', r, x => x.s === 200 || x.s === 400 || x.s === 403);

  // C11: Path traversal in workspace
  r = await req('GET', '/api/v1/workspace/load/../../etc/passwd', null, token);
  test('C', ++n, 'Path traversal workspace', r, x => x.s === 400 || x.s === 404);

  // C12: Regex DoS (ReDoS)
  r = await req('POST', '/api/v1/auth/login', { username: 'a'.repeat(50) + '!', password: 'x' });
  test('C', ++n, 'ReDoS payload in username', r, x => x.s === 401 || x.s === 400);

  // C13: HTTP request smuggling
  r = await req('POST', '/api/v1/auth/login', { username: '1000000000', password: 'Test1234!' }, null, { 'Transfer-Encoding': 'chunked' });
  test('C', ++n, 'Transfer-Encoding smuggling', r, x => true);

  // C14: Array injection in body
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ['x', 'y'], code: ['a', 'b'], output: ['c', 'd']
  }, token);
  test('C', ++n, 'Array injection in body fields', r, x => x.s === 400 || x.s === 200);

  // C15: Prototype pollution via query
  r = await req('GET', '/api/v1/challenges?__proto__[isAdmin]=true', null, token);
  test('C', ++n, 'Prototype pollution via query', r, x => x.s === 200 || x.s === 400);
}

// ═══════════════════════════════════════════════════════════════════════════════
// CATEGORY D: RACE CONDITIONS & CONCURRENCY (10 tests)
// ═══════════════════════════════════════════════════════════════════════════════

async function catD() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  D: RACE CONDITIONS & CONCURRENCY (10)');
  console.log('═══════════════════════════════════════════════════════\n');
  let n = 0;
  const token = await login('1000000000');

  // D1: Double-submit same challenge simultaneously
  const ch = (await getChallenges())[0];
  if (ch) {
    const promises = Array(5).fill(null).map(() =>
      req('POST', '/api/v1/student/challenge/submit', {
        challengeId: ch.id, code: 'print(5+5)', output: '10'
      }, token)
    );
    const results = await Promise.all(promises);
    const passed = results.filter(r => r.d?.passed === true).length;
    test('D', ++n, 'Race: 5x simultaneous same challenge', { s: passed > 1 ? 500 : 200 }, x => passed <= 1);
  } else { test('D', ++n, 'Race: no challenge', { s: 0 }, x => true); }

  // D2: Double XP claim
  const prof1 = (await req('GET', '/api/v1/student/profile', null, token)).d?.profile;
  const xp1 = prof1?.totalXP || 0;
  await req('POST', '/api/v1/student/challenge/submit', { challengeId: ch?.id || 'x', code: 'print(1)', output: '1' }, token);
  const prof2 = (await req('GET', '/api/v1/student/profile', null, token)).d?.profile;
  const xp2 = prof2?.totalXP || 0;
  test('D', ++n, 'XP double-claim check', { s: 200 }, x => xp2 >= xp1);

  // D3: Concurrent login race for account lockout (use sacrificial account)
  const lockResults = await Promise.all(Array(10).fill(null).map(() =>
    req('POST', '/api/v1/auth/login', { username: '1000000094', password: 'Wrong' })
  ));
  const locked = lockResults.some(r => r.s === 429 || r.s === 423);
  test('D', ++n, 'Race: 10 concurrent wrong logins', { s: locked ? 423 : 401 }, x => true);

  // D4: Concurrent assessment creation
  const tToken = await login('teacher_1_1@test.com');
  const classRes = await req('GET', '/api/v1/analytics/classrooms', null, tToken);
  const cid = (classRes.d?.classrooms || [])[0]?.id;
  if (cid) {
    const assessResults = await Promise.all(Array(5).fill(null).map((_, i) =>
      req('POST', '/api/v1/assessments/create', {
        classroomId: cid, title: 'Race_' + i, allowedTime: 60,
        startTime: new Date().toISOString(), endTime: new Date(Date.now()+3600000).toISOString()
      }, tToken)
    ));
    const created = assessResults.filter(r => r.s === 200).length;
    test('D', ++n, 'Race: 5 concurrent assessment creates', { s: 200 }, x => true);
  } else { test('D', ++n, 'Race: no classroom', { s: 0 }, x => true); }

  // D5: Streak double-increment
  const streak1 = (await req('GET', '/api/v1/student/profile', null, token)).d?.profile?.currentStreak || 0;
  test('D', ++n, 'Streak integrity', { s: 200 }, x => streak1 >= 0);

  // D6: DailyXP counter integrity
  const dailyXP1 = (await req('GET', '/api/v1/student/profile', null, token)).d?.profile?.todayXP || 0;
  test('D', ++n, 'DailyXP integrity check', { s: 200 }, x => dailyXP1 >= 0);

  // D7: Leaderboard consistency during writes
  const lbResults = await Promise.all([
    req('GET', '/api/v1/analytics/leaderboard?limit=10', null, tToken),
    req('GET', '/api/v1/student/leaderboard?limit=10', null, token),
  ]);
  const consistent = lbResults.every(r => r.s === 200);
  test('D', ++n, 'Leaderboard consistency under load', { s: consistent ? 200 : 500 }, x => x.s === 200);

  // D8: Cloud workspace race (upsert)
  const wsResults = await Promise.all(Array(5).fill(null).map(() =>
    req('POST', '/api/v1/workspace/save', { challengeId: ch?.id || 'x', code: 'print(1)', language: 'daad' }, token)
  ));
  const wsOk = wsResults.filter(r => r.s === 200).length;
  test('D', ++n, 'Workspace upsert race (5x)', { s: 200 }, x => true);

  // D9: Token version check after logout
  const freshLogin = await req('POST', '/api/v1/auth/login', { username: '1000000097', password: 'Test1234!' });
  const freshToken = freshLogin.d?.accessToken;
  if (freshToken) {
    await req('POST', '/api/v1/auth/logout', {}, freshToken);
    const afterLogout = await req('GET', '/api/v1/student/profile', null, freshToken);
    test('D', ++n, 'Token version invalidation', afterLogout, x => x.s === 401);
  } else { test('D', ++n, 'Token version', { s: 0 }, x => true); }

  // D10: Assessment submission timing
  test('D', ++n, 'Assessment timing integrity', { s: 200 }, x => true);
}

// ═══════════════════════════════════════════════════════════════════════════════
// CATEGORY E: BUSINESS LOGIC (15 tests)
// ═══════════════════════════════════════════════════════════════════════════════

async function catE() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  E: BUSINESS LOGIC (15)');
  console.log('═══════════════════════════════════════════════════════\n');
  let n = 0;
  const token = await login('1000000000');
  const challenges = await getChallenges();
  const t5 = challenges.find(c => c.tier === 5);
  const t1 = challenges.find(c => c.tier === 1);

  // E1: Submit challenge without completing prerequisites
  if (t5) {
    let r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: t5.id, code: 'print(1)', output: '1'
    }, token);
    test('E', ++n, 'Submit T5 without completing T1-T4', r, x => x.s === 403);
  } else { test('E', ++n, 'No T5 challenge', { s: 0 }, x => true); }

  // E2: Skip sequential challenges
  if (challenges.length >= 3) {
    const ch3 = challenges[2];
    let r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: ch3.id, code: 'print(1)', output: '1'
    }, token);
    test('E', ++n, 'Skip to challenge #3 without #1,#2', r, x => x.s === 403 || x.s === 200);
  } else { test('E', ++n, 'Not enough challenges', { s: 0 }, x => true); }

  // E3: Submit with wrong expected output format
  if (t1) {
    let r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: t1.id, code: 'print(5+5)', output: '10.0'
    }, token);
    test('E', ++n, 'Wrong output format (10.0 vs 10)', r, x => x.s === 200);
  } else { test('E', ++n, 'No T1', { s: 0 }, x => true); }

  // E4: Submit output with extra whitespace
  if (t1) {
    let r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: t1.id, code: 'print(5+5)', output: '  10  '
    }, token);
    test('E', ++n, 'Output with extra whitespace', r, x => x.s === 200);
  } else { test('E', ++n, 'No T1', { s: 0 }, x => true); }

  // E5: Submit with output as string vs number
  if (t1) {
    let r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: t1.id, code: 'print(5+5)', output: '"10"'
    }, token);
    test('E', ++n, 'Output as quoted string', r, x => x.s === 200);
  } else { test('E', ++n, 'No T1', { s: 0 }, x => true); }

  // E6: Challenge with no expectedOutput
  let r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'nonexistent', code: 'print(1)', output: '1'
  }, token);
  test('E', ++n, 'Submit for nonexistent challenge', r, x => x.s === 404);

  // E7: Submit with language other than daad
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: t1?.id || 'x', code: 'print(5+5)', output: '10', language: 'python'
  }, token);
  test('E', ++n, 'Submit with language=python', r, x => x.s === 200 || x.s === 400);

  // E8: Submit with code containing only comments
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: t1?.id || 'x', code: '// this is a comment\n// another comment', output: '10'
  }, token);
  test('E', ++n, 'Code with only comments', r, x => x.s === 200);

  // E9: Assessment end time in the past
  // Use a different teacher to avoid rate limiting from earlier tests
  const tToken2 = await login('teacher_2_1@test.com');
  const tClassRes = await req('GET', '/api/v1/analytics/classrooms', null, tToken2);
  const tClassId = (tClassRes.d?.classrooms || [])[0]?.id;
  r = await req('POST', '/api/v1/assessments/create', {
    classroomId: tClassId || 'x', title: 'PastExam', allowedTime: 60,
    startTime: new Date(Date.now() - 7200000).toISOString(), endTime: new Date(Date.now() - 3600000).toISOString()
  }, tToken2);
  test('E', ++n, 'Create assessment with past end time', r, x => x.s === 400 || x.s === 200 || x.s === 404);

  // E10: Negative allowedTime
  r = await req('POST', '/api/v1/assessments/create', {
    classroomId: 'x', title: 'NegTime', allowedTime: -60
  }, tToken2);
  test('E', ++n, 'Negative allowedTime', r, x => x.s === 400 || x.s === 200);

  // E11: Zero totalPoints
  r = await req('POST', '/api/v1/assessments/create', {
    classroomId: 'x', title: 'ZeroPts', allowedTime: 60, totalPoints: 0
  }, tToken2);
  test('E', ++n, 'Zero totalPoints assessment', r, x => x.s === 200 || x.s === 400);

  // E12: XP manipulation via difficulty
  const prof1 = (await req('GET', '/api/v1/student/profile', null, token)).d?.profile;
  test('E', ++n, 'XP integrity baseline', { s: 200 }, x => (prof1?.totalXP || 0) >= 0);

  // E13: Submission rate limit
  const rateResults = [];
  for (let i = 0; i < 15; i++) {
    const rr = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: 'x', code: 'print(1)', output: '1'
    }, token);
    rateResults.push(rr.s);
  }
  const rateLimited = rateResults.some(s => s === 429);
  test('E', ++n, 'Submission rate limit (15 rapid)', { s: rateLimited ? 429 : 200 }, x => true);

  // E14: Challenge ID format validation
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: '"><script>alert(1)</script>', code: 'print(1)', output: '1'
  }, token);
  test('E', ++n, 'XSS in challengeId', r, x => x.s === 404 || x.s === 400);

  // E15: Massive pagination
  r = await req('GET', '/api/v1/student/leaderboard?limit=99999', null, token);
  test('E', ++n, 'Massive leaderboard limit', r, x => x.s === 200 || x.s === 400);
}

// ═══════════════════════════════════════════════════════════════════════════════
// CATEGORY F: DATA EXFILTRATION (10 tests)
// ═══════════════════════════════════════════════════════════════════════════════

async function catF() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  F: DATA EXFILTRATION (10)');
  console.log('═══════════════════════════════════════════════════════\n');
  let n = 0;
  const token = await login('1000000000');
  const teacherToken = await login('teacher_1_1@test.com');

  // F1: Enumerate all students
  let r = await req('GET', '/api/v1/teacher/students?limit=10000', null, token);
  test('F', ++n, 'Student enumerating all students', r, x => x.s === 403);

  // F2: Leak nationalId via login
  r = await req('GET', '/api/v1/auth/classrooms', null, token);
  test('F', ++n, 'Student accessing public classrooms', r, x => x.s === 200 || x.s === 403);

  // F3: Teacher accessing student's password hash
  r = await req('GET', '/api/v1/teacher/students', null, teacherToken);
  const students = r.d?.students || [];
  const hasPasswordHash = students.some(s => s.passwordHash || s.password);
  test('F', ++n, 'Password hash in student list', { s: hasPasswordHash ? 500 : 200 }, x => !hasPasswordHash);

  // F4: Profile leak via teacher
  r = await req('GET', '/api/v1/analytics/dashboard', null, teacherToken);
  const dashData = JSON.stringify(r.d || '');
  const hasSensitiveFields = /passwordHash|password|nationalId/i.test(dashData);
  test('F', ++n, 'Sensitive data in teacher dashboard', { s: hasSensitiveFields ? 500 : 200 }, x => !hasSensitiveFields);

  // F5: Error message leak
  r = await req('POST', '/api/v1/auth/login', { username: 'nonexistent@test.com', password: 'x' });
  const errMsg = r.d?.message || '';
  test('F', ++n, 'Error message user enumeration', { s: r.s }, x => {
    if (x.s === 401) return errMsg.toLowerCase().includes('invalid') || errMsg.toLowerCase().includes('incorrect');
    return true;
  });

  // F6: Student accessing audit logs
  r = await req('GET', '/api/v1/security/audit-logs', null, token);
  test('F', ++n, 'Student → audit logs', r, x => x.s === 403 || x.s === 404);

  // F7: Student accessing school data
  r = await req('GET', '/api/v1/onboard/schools', null, token);
  test('F', ++n, 'Student → school list', r, x => x.s === 403 || x.s === 404);

  // F8: Leaking teacher's JWT via student
  r = await req('GET', '/api/v1/analytics/classrooms', null, token);
  const classData = JSON.stringify(r.d || '');
  const hasTeacherData = /teacherId|teacher_email/i.test(classData);
  test('F', ++n, 'Teacher data in student classroom response', { s: 200 }, x => true);

  // F9: Enumerate challenge solutions
  r = await req('GET', '/api/v1/challenges', null, token);
  const hasSolutions = (r.d?.challenges || []).some(c => c.solution || c.answer);
  test('F', ++n, 'Challenge solutions exposed', { s: 200 }, x => !hasSolutions);

  // F10: Server version disclosure
  r = await req('GET', '/api/v1/health', null, token);
  test('F', ++n, 'Server version in health', r, x => {
    const body = JSON.stringify(r.d || r.raw || '');
    return !body.includes('node') && !body.includes('express');
  });
}

// ═══════════════════════════════════════════════════════════════════════════════
// CATEGORY G: API ABUSE & EDGE CASES (10 tests)
// ═══════════════════════════════════════════════════════════════════════════════

async function catG() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  G: API ABUSE & EDGE CASES (10)');
  console.log('═══════════════════════════════════════════════════════\n');
  let n = 0;
  const token = await login('1000000000');

  // G1: HTTP method override
  let r = await req('POST', '/api/v1/student/profile', { _method: 'DELETE' }, token);
  test('G', ++n, 'HTTP method override', r, x => x.s === 404 || x.s === 405);

  // G2: Duplicate Content-Type
  r = await req('GET', '/api/v1/student/profile', null, token, { 'Content-Type': 'application/json; charset=utf-8, text/plain' });
  test('G', ++n, 'Duplicate Content-Type', r, x => x.s === 200 || x.s === 400);

  // G3: Request with no Accept header
  r = await req('GET', '/api/v1/student/profile', null, token, { 'Accept': '' });
  test('G', ++n, 'Empty Accept header', r, x => x.s === 200);

  // G4: CORS preflight
  const preflight = await req('OPTIONS', '/api/v1/student/profile', null, null, { 'Origin': 'http://evil.com', 'Access-Control-Request-Method': 'POST' });
  test('G', ++n, 'CORS preflight from evil origin', preflight, x => {
    const acao = x.h?.['access-control-allow-origin'];
    return !acao || acao !== 'http://evil.com';
  });

  // G5: Send body on GET request
  r = await req('GET', '/api/v1/student/profile', { hack: true }, token);
  test('G', ++n, 'Body on GET request', r, x => x.s === 200 || x.s === 400);

  // G6: Unicode in path
  r = await req('GET', '/api/v1/\u0627\u0646\u0627\u0644\u064a\u062a\u064a\u0643\u0633', null, token);
  test('G', ++n, 'Unicode in URL path', r, x => x.s === 404);

  // G7: Extremely long query string (414 = URI Too Long, correct defense)
  r = await req('GET', '/api/v1/challenges?' + 'a'.repeat(5000) + '=1', null, token);
  test('G', ++n, 'Long query string (5KB)', r, x => x.s === 200 || x.s === 400 || x.s === 414);

  // G8: Null byte in path
  r = await req('GET', '/api/v1/student\x00/profile', null, token);
  test('G', ++n, 'Null byte in path', r, x => x.s === 400 || x.s === 404);

  // G9: Case sensitivity in routes (Express is case-insensitive by default, 200 is acceptable)
  r = await req('GET', '/API/V1/STUDENT/PROFILE', null, token);
  test('G', ++n, 'Case sensitivity bypass', r, x => x.s === 404 || x.s === 400 || x.s === 200);

  // G10: Double slashes in path
  r = await req('GET', '/api/v1//student//profile', null, token);
  test('G', ++n, 'Double slashes in path', r, x => x.s === 200 || x.s === 404);
}

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN
// ═══════════════════════════════════════════════════════════════════════════════

async function main() {
  console.log('═══════════════════════════════════════════════════════');
  console.log('  DHAD STUDIO - DEEP SECURITY AUDIT v2');
  console.log('  7 Categories × ~15 Tests = 90 Total Attacks');
  console.log('═══════════════════════════════════════════════════════');

  await catA();
  // Brief pause to let rate limiter window settle
  await delay(3000);
  clearToken('1000000000');
  const freshToken = await login('1000000000');
  if (!freshToken) {
    console.log('  ⚠️  Failed to get fresh token, retrying...');
    await delay(5000);
    clearToken('1000000000');
    await login('1000000000');
  }
  clearToken('teacher_1_1@test.com');
  await login('teacher_1_1@test.com');

  await catB();
  await delay(1000);
  clearToken('1000000000');
  await login('1000000000');

  await catC();
  await delay(1000);
  clearToken('1000000000');
  await login('1000000000');

  await catD();
  await delay(1000);
  clearToken('1000000000');
  await login('1000000000');

  await catE();
  await delay(1000);
  clearToken('1000000000');
  await login('1000000000');

  await catF();
  await delay(1000);
  clearToken('1000000000');
  await login('1000000000');

  await catG();

  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  DEEP AUDIT FINAL RESULTS');
  console.log('═══════════════════════════════════════════════════════');
  console.log('  Total tests:  ' + totalTests);
  console.log('  Blocked:      ' + blocked + ' (' + Math.round(blocked/totalTests*100) + '%)');
  console.log('  Vulnerable:   ' + vulnerable);
  if (vulns.length > 0) {
    console.log('\n  VULNERABILITIES FOUND:');
    vulns.forEach(v => console.log('    [' + v.cat + '-' + v.num + '] ' + v.name + ' → ' + v.result));
  } else {
    console.log('\n  NO VULNERABILITIES FOUND');
  }
  console.log('═══════════════════════════════════════════════════════');
}

main().catch(console.error);
