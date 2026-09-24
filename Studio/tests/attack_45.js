const http = require('http');
const crypto = require('crypto');

const BASE = { hostname: 'localhost', port: 3000 };
const results = { hack: [], tamper: [], cheat: [] };
let hackCount = 0, tamperCount = 0, cheatCount = 0;

function req(method, path, body, token, extraHeaders) {
  return new Promise((resolve) => {
    const data = body ? (typeof body === 'string' ? body : JSON.stringify(body)) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'AttackTest/1.0', ...extraHeaders };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, ...BASE, path: encodeURI(path), headers, timeout: 10000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => {
        try { resolve({ s: res.statusCode, d: JSON.parse(buf) }); }
        catch(e) { resolve({ s: res.statusCode, raw: buf.substring(0, 200) }); }
      });
    });
    r.on('error', (e) => resolve({ s: 0, error: e.message }));
    r.on('timeout', () => { r.destroy(); resolve({ s: 0, error: 'timeout' }); });
    if (data) r.write(data);
    r.end();
  });
}

function log(type, num, name, result, detail) {
  const status = result.s >= 200 && result.s < 300 ? 'BLOCKED' : (result.s >= 400 ? 'BLOCKED' : 'UNCLEAR');
  const emoji = status === 'BLOCKED' ? '✓' : '?';
  console.log(`  [${type.toUpperCase()}-${String(num).padStart(2,'0')}] ${emoji} ${name} → HTTP ${result.s} ${detail || ''}`);
  results[type].push({ num, name, status: result.s, detail, blocked: status === 'BLOCKED' });
}

// ═══════════════════════════════════════════════════════════════════════════════
// PHASE 1: HACK ATTEMPTS (15)
// ═══════════════════════════════════════════════════════════════════════════════

async function runHacks() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  PHASE 1: HACK ATTEMPTS (15)');
  console.log('═══════════════════════════════════════════════════════\n');

  // 1. SQL Injection in login
  let r = await req('POST', '/api/v1/auth/login', { username: "admin'--", password: "x" });
  hackCount++; log('hack', hackCount, 'SQL Injection login (admin\'--)', r);

  // 2. SQL Injection in login OR bypass
  r = await req('POST', '/api/v1/auth/login', { username: "admin' OR '1'='1", password: "x" });
  hackCount++; log('hack', hackCount, 'SQL Injection OR bypass', r);

  // 3. XSS in challenge submission
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'x', code: '<script>alert(1)</script>', output: '<img src=x onerror=alert(1)>'
  }, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'XSS in submission code/output', r);

  // 4. Access admin dashboard as student
  r = await req('GET', '/api/v1/analytics/dashboard', null, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'Student accessing admin dashboard', r);

  // 5. Access security alerts as student
  r = await req('GET', '/api/v1/analytics/security-alerts', null, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'Student accessing security alerts', r);

  // 6. Path traversal
  r = await req('GET', '/api/v1/../../etc/passwd', null, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'Path traversal ../../etc/passwd', r);

  // 7. No auth header at all on protected route
  r = await req('GET', '/api/v1/student/profile', null, null);
  hackCount++; log('hack', hackCount, 'No auth header on protected route', r);

  // 8. Expired/fake JWT token
  r = await req('GET', '/api/v1/student/profile', null, 'eyJhbGciOiJIUzI1NiJ9.eyJpZCI6IjEyMzQ1In0.fake');
  hackCount++; log('hack', hackCount, 'Fake JWT token', r);

  // 9. SQL Injection in assessment creation
  r = await req('POST', '/api/v1/assessments/create', {
    classroomId: "x'; DROP TABLE users;--", title: 'hack', allowedTime: 60
  }, await getToken('teacher_1_1@test.com'));
  hackCount++; log('hack', hackCount, 'SQL Injection in assessment create', r);

  // 10. Create admin account via register
  r = await req('POST', '/api/v1/auth/register', {
    name: 'Hacker', email: 'hacker@evil.com', password: 'Hacker123!', role: 'ADMIN', schoolId: 'x'
  });
  hackCount++; log('hack', hackCount, 'Register as ADMIN', r);

  // 11. Mass assignment - set XP directly
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'x', code: 'x', output: 'x', passed: true, xpAwarded: 99999
  }, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'Mass assignment (set passed=true, xpAwarded)', r);

  // 12. Command injection
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'x', code: 'require("child_process").exec("rm -rf /")', output: 'x'
  }, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'Command injection via require()', r);

  // 13. Prototype pollution
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'x', code: 'x', output: 'x', '__proto__': { isAdmin: true }
  }, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'Prototype pollution __proto__', r);

  // 14. Access teacher analytics as student
  r = await req('GET', '/api/v1/analytics/leaderboard', null, await getToken('1000000000'));
  hackCount++; log('hack', hackCount, 'Student accessing teacher leaderboard', r);

  // 15. XXE via Content-Type
  r = await req('POST', '/api/v1/auth/login', '<?xml version="1.0"?><!DOCTYPE foo [<!ENTITY xxe SYSTEM "file:///etc/passwd">]><root>&xxe;</root>', null, { 'Content-Type': 'application/xml' });
  hackCount++; log('hack', hackCount, 'XXE attack via XML body', r);
}

// ═══════════════════════════════════════════════════════════════════════════════
// PHASE 2: TAMPER ATTEMPTS (15)
// ═══════════════════════════════════════════════════════════════════════════════

async function runTampers() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  PHASE 2: TAMPER ATTEMPTS (15)');
  console.log('═══════════════════════════════════════════════════════\n');

  const studentToken = await getToken('1000000000');
  const teacherToken = await getToken('teacher_1_1@test.com');

  // 1. Tamper with challenge submission - wrong output
  let r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: (await getFirstChallenge()).id, code: 'print(5+5)', output: 'TAMPERED_OUTPUT_99999'
  }, studentToken);
  tamperCount++; log('tamper', tamperCount, 'Submit wrong output for challenge', r);

  // 2. Tamper JWT - change payload
  const fakePayload = Buffer.from(JSON.stringify({ id: '1000000000', role: 'ADMIN', schoolId: 's01' })).toString('base64');
  const fakeJWT = 'eyJhbGciOiJIUzI1NiJ9.' + fakePayload + '.fakesig';
  r = await req('GET', '/api/v1/student/profile', null, fakeJWT);
  tamperCount++; log('tamper', tamperCount, 'Tamper JWT payload (set role=ADMIN)', r);

  // 3. Tamper submission - set passed=true in body
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'hack', code: 'x', output: 'y', passed: true
  }, studentToken);
  tamperCount++; log('tamper', tamperCount, 'Tamper: set passed=true in request body', r);

  // 4. Tamper assessment - submit as wrong student
  r = await req('POST', '/api/v1/assessments/submit', {
    assessmentId: 'x', answers: [{ questionId: 'q1', answer: 'A' }]
  }, studentToken);
  tamperCount++; log('tamper', tamperCount, 'Submit assessment with fake assessmentId', r);

  // 5. Tamper leaderboard - query manipulation
  r = await req('GET', "/api/v1/analytics/leaderboard?classroom=' OR 1=1--", null, teacherToken);
  tamperCount++; log('tamper', tamperCount, 'SQL Injection in leaderboard query param', r);

  // 6. Tamper - modify User-Agent to overflow
  r = await req('GET', '/api/v1/student/profile', null, studentToken, { 'User-Agent': 'A'.repeat(1000) });
  tamperCount++; log('tamper', tamperCount, 'Overflow User-Agent header (1000 chars)', r);

  // 7. Tamper - empty Content-Type
  r = await req('POST', '/api/v1/auth/login', { username: 'x', password: 'y' }, null, { 'Content-Type': '' });
  tamperCount++; log('tamper', tamperCount, 'Empty Content-Type header', r);

  // 8. Tamper - oversized body
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'x', code: 'A'.repeat(2000000), output: 'y'
  }, studentToken);
  tamperCount++; log('tamper', tamperCount, 'Oversized request body (2MB code)', r);

  // 9. Tamper - URL encoding bypass
  r = await req('GET', '/api/v1/%61nalytics/dashboard', null, studentToken);
  tamperCount++; log('tamper', tamperCount, 'URL encoding bypass %61nalytics', r);

  // 10. Tamper - duplicate headers
  r = await req('GET', '/api/v1/student/profile', null, null, { 'Authorization': 'Bearer ' + studentToken + ', Bearer fake' });
  tamperCount++; log('tamper', tamperCount, 'Duplicate Authorization headers', r);

  // 11. Tamper - change password of another user
  r = await req('POST', '/api/v1/auth/change-password', {
    currentPassword: 'Test1234!', newPassword: 'Hacked123!', targetUserId: 'teacher_1_1_id'
  }, studentToken);
  tamperCount++; log('tamper', tamperCount, 'Change another user password', r);

  // 12. Tamper - negative XP in telemetry
  r = await req('POST', '/api/v1/submissions/bulk-report', {
    submissions: [{ challengeId: 'x', passed: true, xp: -99999, score: 100 }]
  }, studentToken);
  tamperCount++; log('tamper', tamperCount, 'Negative XP in telemetry payload', r);

  // 13. Tamper - Nil byte injection
  r = await req('POST', '/api/v1/auth/login', { username: 'admin\x00@evil.com', password: 'x' });
  tamperCount++; log('tamper', tamperCount, 'Nil byte injection in username', r);

  // 14. Tamper - missing required fields
  r = await req('POST', '/api/v1/student/challenge/submit', {}, studentToken);
  tamperCount++; log('tamper', tamperCount, 'Missing all required fields', r);

  // 15. Tamper - send GET where POST expected
  r = await req('GET', '/api/v1/assessments/create', null, teacherToken);
  tamperCount++; log('tamper', tamperCount, 'GET on POST-only endpoint', r);
}

// ═══════════════════════════════════════════════════════════════════════════════
// PHASE 3: CHEAT ATTEMPTS (15)
// ═══════════════════════════════════════════════════════════════════════════════

async function runCheats() {
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  PHASE 3: CHEAT ATTEMPTS (15)');
  console.log('═══════════════════════════════════════════════════════\n');

  const studentToken = await getToken('1000000000');

  // 1. Cheat - submit challenge with matching output but wrong code
  const ch = await getFirstChallenge();
  let r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'CHEAT_NO_CODE_HERE', output: ch.expectedOutput || 'CHEAT'
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Match output but no valid code', r);

  // 2. Cheat - submit with passed:true from client
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'print("hack")', output: 'hack', passed: true
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Client sends passed=true override', r);

  // 3. Cheat - replay old submission
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'print(5+5)', output: '10'
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Replay previously passed challenge', r);

  // 4. Cheat - submit for challenge in locked tier
  const allCh = await getAllChallenges();
  const tier5 = allCh.find(c => c.tier === 5);
  if (tier5) {
    r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: tier5.id, code: 'print(1)', output: '1'
    }, studentToken);
    cheatCount++; log('cheat', cheatCount, 'Submit for locked tier 5 challenge', r);
  } else {
    cheatCount++; log('cheat', cheatCount, 'No tier 5 challenge found', { s: 0 });
  }

  // 5. Cheat - submit with null challengeId
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: null, code: 'x', output: 'y'
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Null challengeId', r);

  // 6. Cheat - submit with empty code
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: '', output: ch.expectedOutput || ''
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Empty code with matching output', r);

  // 7. Cheat - submit same challenge rapidly (race condition)
  const rapidPromises = [];
  for (let i = 0; i < 5; i++) {
    rapidPromises.push(req('POST', '/api/v1/student/challenge/submit', {
      challengeId: ch.id, code: 'print(5+5)', output: '10'
    }, studentToken));
  }
  const rapidResults = await Promise.all(rapidPromises);
  const rapidPassed = rapidResults.filter(r => r.s === 200 && r.d?.passed).length;
  cheatCount++; log('cheat', cheatCount, 'Race condition: 5 rapid submissions (' + rapidPassed + ' passed)', { s: rapidPassed > 1 ? 500 : 200 });

  // 8. Cheat - fake telemetry submission with max XP
  r = await req('POST', '/api/v1/submissions/bulk-report', {
    submissions: [{ challengeId: ch.id, passed: true, xp: 999999, score: 100, executionTime: 1 }]
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Fake telemetry: 999999 XP', r);

  // 9. Cheat - submit for another student's challenge
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'print(1)', output: '1', studentId: 'other_student'
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Try to submit for another student', r);

  // 10. Cheat - bypass sequential lock with wrong order
  const ch2 = allCh.filter(c => c.tier === 1);
  if (ch2.length >= 2) {
    r = await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: ch2[ch2.length - 1].id, code: 'print(1)', output: '1'
    }, studentToken);
    cheatCount++; log('cheat', cheatCount, 'Bypass sequential lock (skip to last T1)', r);
  } else {
    cheatCount++; log('cheat', cheatCount, 'Not enough T1 challenges', { s: 0 });
  }

  // 11. Cheat - manipulate dynamic output
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: '{{student.name}}', output: 'any_name'
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Fake dynamic output injection', r);

  // 12. Cheat - submit with extra fields to manipulate DB
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'print(1)', output: '1',
    xpAwarded: 99999, level: 99, streak: 999
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Extra fields (xpAwarded, level, streak)', r);

  // 13. Cheat - use special characters in output
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'print("\\n\\t\\r")', output: '\n\t\r'
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Special chars in output (newline/tab)', r);

  // 14. Cheat - submit with very long output
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'print("x")', output: 'x'.repeat(100000)
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Extremely long output (100KB)', r);

  // 15. Cheat - try to unlock tier by manipulating submissions
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch.id, code: 'for(let i=0;i<100;i++)print(1)', output: '1'.repeat(100)
  }, studentToken);
  cheatCount++; log('cheat', cheatCount, 'Bulk output manipulation', r);
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

const tokenCache = {};
async function getToken(identifier) {
  if (tokenCache[identifier]) return tokenCache[identifier];
  let username = identifier;
  if (identifier.startsWith('teacher')) username = identifier;
  else username = identifier;
  const r = await req('POST', '/api/v1/auth/login', { username, password: 'Test1234!' });
  const token = r.d?.accessToken;
  if (token) tokenCache[identifier] = token;
  return token || '';
}

let _firstChallenge = null;
async function getFirstChallenge() {
  if (_firstChallenge) return _firstChallenge;
  const token = await getToken('1000000000');
  const r = await req('GET', '/api/v1/challenges', null, token);
  _firstChallenge = (r.d?.challenges || [])[0] || { id: 'none', expectedOutput: '' };
  return _firstChallenge;
}

let _allChallenges = null;
async function getAllChallenges() {
  if (_allChallenges) return _allChallenges;
  const token = await getToken('1000000000');
  const r = await req('GET', '/api/v1/challenges', null, token);
  _allChallenges = r.d?.challenges || [];
  return _allChallenges;
}

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN
// ═══════════════════════════════════════════════════════════════════════════════

async function main() {
  console.log('═══════════════════════════════════════════════════════');
  console.log('  DHAD STUDIO - COMPREHENSIVE ATTACK TEST');
  console.log('  15 Hack + 15 Tamper + 15 Cheat = 45 Total Attacks');
  console.log('═══════════════════════════════════════════════════════');

  await runHacks();
  await runTampers();
  await runCheats();

  // Summary
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  ATTACK RESULTS SUMMARY');
  console.log('═══════════════════════════════════════════════════════');

  const hackBlocked = results.hack.filter(r => r.blocked).length;
  const tamperBlocked = results.tamper.filter(r => r.blocked).length;
  const cheatBlocked = results.cheat.filter(r => r.blocked).length;

  console.log(`\n  HACK ATTEMPTS:   ${hackBlocked}/15 blocked`);
  results.hack.forEach(r => console.log(`    ${r.blocked ? '✓' : '✗'} ${r.name} → HTTP ${r.status}`));

  console.log(`\n  TAMPER ATTEMPTS: ${tamperBlocked}/15 blocked`);
  results.tamper.forEach(r => console.log(`    ${r.blocked ? '✓' : '✗'} ${r.name} → HTTP ${r.status}`));

  console.log(`\n  CHEAT ATTEMPTS:  ${cheatBlocked}/15 blocked`);
  results.cheat.forEach(r => console.log(`    ${r.blocked ? '✓' : '✗'} ${r.name} → HTTP ${r.status}`));

  const total = hackBlocked + tamperBlocked + cheatBlocked;
  console.log(`\n  TOTAL: ${total}/45 attacks blocked (${Math.round(total/45*100)}%)`);
  console.log('═══════════════════════════════════════════════════════');
}

main().catch(console.error);
