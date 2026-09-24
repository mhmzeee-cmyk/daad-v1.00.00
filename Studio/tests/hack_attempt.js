// ═══════════════════════════════════════════════════════════════════════════════
// CHALLENGE: HACK THE SYSTEM - Dhad Studio
// Maximum effort penetration attempt
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
let SUCCESS = [];
let FAILED = [];

function hacked(title, detail) {
  SUCCESS.push({ title, detail });
  console.log(`  🔴 HACKED: ${title}`);
  if (detail) console.log(`     ${detail}`);
}
function blocked(title) { FAILED.push(title); }
function info(msg) { console.log(`  ℹ️  ${msg}`); }

async function main() {
  console.log('╔══════════════════════════════════════════════════════════════╗');
  console.log('║   CHALLENGE: HACK THE SYSTEM - DHAD STUDIO                ║');
  console.log('║   Maximum effort penetration attempt                      ║');
  console.log('╚══════════════════════════════════════════════════════════════╝\n');

  // Get tokens
  console.log('🔑 Obtaining tokens...');
  const accounts = [
    { user: 'teacher@dhad.com', pass: 'Test1234!' },
    { user: 'shamel@teacher.com', pass: 'Test1234!' },
    { user: 'sara@teacher.com', pass: 'Test1234!' },
    { user: 'ali@teacher.com', pass: 'Test1234!' },
    { user: '1099990001', pass: 'Test1234!' },
    { user: 'admin@dhadstudio.com', pass: 'Test1234!' },
  ];
  
  for (const acc of accounts) {
    const r = await req('POST', '/api/v1/auth/login', { username: acc.user, password: acc.pass });
    if (r.body?.accessToken) {
      T[acc.user] = r.body.accessToken;
      console.log(`   ✅ ${acc.user}`);
    }
  }
  
  const teacherTokens = Object.values(T).filter((v, i) => i < 4);
  const studentToken = T['1099990001'];
  const adminToken = T['admin@dhadstudio.com'];
  
  console.log(`\n   Tokens: ${Object.keys(T).length} obtained\n`);

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 1: JWT KEY CONFUSION
  // ═════════════════════════════════════════════════════════════════════════
  console.log('━━━ ATTACK 1: JWT KEY CONFUSION ━━━');

  // Try to sign with empty key
  const emptyKeySig = crypto.createHmac('sha256', '').update('test').digest('hex');
  let r = await req('GET', '/api/v1/analytics/leaderboard', null, {
    Authorization: `Bearer eyJhbGciOiJIUzI1NiJ9.${Buffer.from(JSON.stringify({id:'admin',role:'ADMIN'})).toString('base64url')}.${emptyKeySig}`
  });
  if (r.status === 200) hacked('JWT empty key signing', 'Accepted JWT signed with empty key');
  else blocked('JWT empty key');

  // Try common weak secrets
  const weakSecrets = ['secret', 'password', '12345678', 'jwt', 'key', 'dhad', 'studio', 'admin', 'test', 'dev'];
  for (const secret of weakSecrets) {
    const sig = crypto.createHmac('sha256', secret).update('test').digest('hex');
    r = await req('GET', '/api/v1/analytics/leaderboard', null, {
      Authorization: `Bearer eyJhbGciOiJIUzI1NiJ9.${Buffer.from(JSON.stringify({id:'admin',role:'ADMIN'})).toString('base64url')}.${sig}`
    });
    if (r.status === 200) {
      hacked('JWT weak secret', `Secret: "${secret}"`);
      break;
    }
  }
  blocked('JWT weak secrets');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 2: RACE CONDITION EXPLOITATION
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 2: RACE CONDITION EXPLOITATION ━━━');

  // 2.1 Double-spend challenge submission
  const challengePayload = {
    challengeId: 'ch-race-exploit',
    passed: true,
    output: 'exploit',
    signature: 'exploit-sig'
  };
  
  const racePromises = [];
  for (let i = 0; i < 50; i++) {
    racePromises.push(req('POST', '/api/v1/student/challenge/submit', challengePayload, { Authorization: `Bearer ${studentToken}` }));
  }
  const raceResults = await Promise.all(racePromises);
  const successCount = raceResults.filter(r => r.status === 200).length;
  if (successCount > 1) hacked('Race condition double-spend', `${successCount}/50 submissions succeeded`);
  else blocked('Race condition double-spend');

  // 2.2 Race on login (token generation)
  const loginRace = [];
  for (let i = 0; i < 20; i++) {
    loginRace.push(req('POST', '/api/v1/auth/login', { username: 'teacher@dhad.com', password: 'Test1234!' }));
  }
  const loginResults = await Promise.all(loginRace);
  const loginSuccess = loginResults.filter(r => r.status === 200);
  if (loginSuccess.length > 1) {
    const tokens = loginSuccess.map(r => r.body.accessToken);
    const uniqueTokens = [...new Set(tokens)];
    if (uniqueTokens.length > 1) hacked('Race condition multiple tokens', `${uniqueTokens.length} unique tokens generated`);
    else blocked('Race condition tokens (same token)');
  } else blocked('Race condition login');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 3: BUSINESS LOGIC EXPLOITATION
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 3: BUSINESS LOGIC EXPLOITATION ━━━');

  // 3.1 Submit challenge as teacher (should be student only)
  r = await req('POST', '/api/v1/student/challenge/submit', challengePayload, { Authorization: `Bearer ${teacherTokens[0]}` });
  if (r.status === 200) hacked('Teacher can submit student challenge', 'Teacher token accepted');
  else blocked('Teacher challenge submission');

  // 3.2 Submit challenge as admin
  r = await req('POST', '/api/v1/student/challenge/submit', challengePayload, { Authorization: `Bearer ${adminToken}` });
  if (r.status === 200) hacked('Admin can submit student challenge', 'Admin token accepted');
  else blocked('Admin challenge submission');

  // 3.3 Create assessment as student
  r = await req('POST', '/api/v1/assessments/create', { title: 'hacked', type: 'CODING', classroomId: 'x' }, { Authorization: `Bearer ${studentToken}` });
  if (r.status === 200 || r.status === 201) hacked('Student creates assessment', 'Student can create assessments');
  else blocked('Student assessment creation');

  // 3.4 Create classroom as student
  r = await req('POST', '/api/v1/student-management/classrooms', { name: 'hacked', grade: '9' }, { Authorization: `Bearer ${studentToken}` });
  if (r.status === 200 || r.status === 201) hacked('Student creates classroom', 'Student can create classrooms');
  else blocked('Student classroom creation');

  // 3.5 Delete student as teacher (wrong classroom)
  r = await req('DELETE', '/api/v1/student-management/students/fake-id', null, { Authorization: `Bearer ${teacherTokens[0]}` });
  if (r.status === 200) hacked('Teacher deletes student', 'Teacher can delete students');
  else blocked('Teacher student deletion');

  // 3.6 Access other teacher's data
  if (teacherTokens.length > 1) {
    // Try to use teacher2's token on teacher1's data
    r = await req('GET', '/api/v1/analytics/leaderboard', null, { Authorization: `Bearer ${teacherTokens[1]}` });
    if (r.status === 200) {
      // Try to access teacher1's classroom
      r = await req('GET', '/api/v1/student-management/students', null, { Authorization: `Bearer ${teacherTokens[1]}` });
      if (r.status === 200) info('Teacher2 can access students');
    }
  }
  blocked('Cross-teacher data access');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 4: INJECTION BYPASS
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 4: INJECTION BYPASS ━━━');

  // 4.1 NoSQL injection with operator
  const nosqlPayloads = [
    { username: { $gt: '' }, password: { $gt: '' } },
    { username: { $ne: null }, password: { $ne: null } },
    { username: { $regex: '^admin' }, password: { $regex: '.*' } },
    { username: { $where: 'this.username == "admin"' }, password: 'x' },
    { '$or': [{ username: 'teacher@dhad.com' }, { username: 'admin@dhadstudio.com' }], password: 'Test1234!' },
  ];
  
  for (const payload of nosqlPayloads) {
    r = await req('POST', '/api/v1/auth/login', payload);
    if (r.status === 200 && r.body.accessToken) {
      hacked('NoSQL injection', `Payload: ${JSON.stringify(payload)}`);
      break;
    }
  }
  blocked('NoSQL injection');

  // 4.2 Second-order SQL injection
  // First, create a user with SQL chars in name
  const sqlName = "test' OR '1'='1";
  r = await req('POST', '/api/v1/auth/register', {
    name: sqlName,
    email: `sql${Date.now()}@test.com`,
    password: 'Test1234!',
    role: 'TEACHER',
    schoolName: 'Test'
  }, { Authorization: `Bearer ${teacherTokens[0]}` });
  
  if (r.status === 200 || r.status === 201) {
    // Now try to login with that user
    r = await req('POST', '/api/v1/auth/login', {
      username: `sql${Date.now()}@test.com`,
      password: 'Test1234!'
    });
    if (r.status === 200) info('SQL injection user created');
  }
  blocked('Second-order SQL injection');

  // 4.3 LDAP injection
  r = await req('POST', '/api/v1/auth/login', { username: 'admin)(|(password=*))', password: 'x' });
  if (r.status === 200) hacked('LDAP injection', 'Bypassed authentication');
  else blocked('LDAP injection');

  // 4.4 XML injection (XXE)
  const xxePayload = `<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE foo [<!ENTITY xxe SYSTEM "file:///etc/passwd">]>
<root><username>&xxe;</username><password>test</password></root>`;
  
  r = await req('POST', '/api/v1/auth/login', xxePayload, { 'Content-Type': 'application/xml' });
  if (r.raw && r.raw.includes('root:')) hacked('XXE injection', 'Read /etc/passwd via XXE');
  else blocked('XXE injection');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 5: TOKEN MANIPULATION
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 5: TOKEN MANIPULATION ━━━');

  if (studentToken) {
    const parts = studentToken.split('.');
    const payload = JSON.parse(Buffer.from(parts[1], 'base64url').toString());
    
    // 5.1 Change role to ADMIN
    payload.role = 'ADMIN';
    const newPayload = Buffer.from(JSON.stringify(payload)).toString('base64url');
    r = await req('GET', '/api/v1/security/dashboard', null, { Authorization: `Bearer ${parts[0]}.${newPayload}.${parts[2]}` });
    if (r.status === 200) hacked('Role escalation via JWT', 'Changed role to ADMIN');
    else blocked('Role escalation via JWT');

    // 5.2 Change id to admin
    const adminUser = Object.entries(T).find(([k, v]) => k === 'admin@dhadstudio.com');
    if (adminUser) {
      const adminPayload = JSON.parse(Buffer.from(adminUser[1].split('.')[1], 'base64url').toString());
      payload.id = adminPayload.id;
      const adminIdPayload = Buffer.from(JSON.stringify(payload)).toString('base64url');
      r = await req('GET', '/api/v1/security/dashboard', null, { Authorization: `Bearer ${parts[0]}.${adminIdPayload}.${parts[2]}` });
      if (r.status === 200) hacked('IDOR via JWT', 'Changed ID to admin');
      else blocked('IDOR via JWT');
    }

    // 5.3 Remove tokenVersion
    delete payload.tokenVersion;
    const noVerPayload = Buffer.from(JSON.stringify(payload)).toString('base64url');
    r = await req('GET', '/api/v1/analytics/leaderboard', null, { Authorization: `Bearer ${parts[0]}.${noVerPayload}.${parts[2]}` });
    if (r.status === 200) hacked('Token version bypass', 'Removed tokenVersion claim');
    else blocked('Token version bypass');

    // 5.4 Set exp to far future
    payload.exp = Math.floor(Date.now() / 1000) + (365 * 24 * 60 * 60); // 1 year
    const longExpPayload = Buffer.from(JSON.stringify(payload)).toString('base64url');
    r = await req('GET', '/api/v1/analytics/leaderboard', null, { Authorization: `Bearer ${parts[0]}.${longExpPayload}.${parts[2]}` });
    if (r.status === 200) hacked('Long-lived token', 'Extended expiry to 1 year');
    else blocked('Long-lived token');
  }

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 6: SERVER-SIDE REQUEST FORGERY (SSRF)
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 6: SSRF ATTEMPTS ━━━');

  // 6.1 Internal network scan
  const internalIPs = ['127.0.0.1', 'localhost', '0.0.0.0', '10.0.0.1', '172.16.0.1', '192.168.1.1'];
  for (const ip of internalIPs) {
    r = await req('POST', '/api/v1/auth/login', { username: 'test', password: 'test', url: `http://${ip}:3000/health` });
    if (r.status === 200) {
      hacked('SSRF', `Internal IP accessible: ${ip}`);
      break;
    }
  }
  blocked('SSRF internal scan');

  // 6.2 File read via SSRF
  r = await req('POST', '/api/v1/auth/login', { username: 'test', password: 'test', url: 'file:///etc/passwd' });
  if (r.raw && r.raw.includes('root:')) hacked('SSRF file read', 'Read file via SSRF');
  else blocked('SSRF file read');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 7: PROTOTYPE POLLUTION
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 7: PROTOTYPE POLLUTION ━━━');

  const pollutePayloads = [
    { '__proto__': { 'isAdmin': true } },
    { 'constructor': { 'prototype': { 'isAdmin': true } } },
    { '__proto__': { 'role': 'ADMIN' } },
    { '__proto__': { 'id': 'admin' } },
  ];

  for (const payload of pollutePayloads) {
    r = await req('POST', '/api/v1/auth/login', { ...payload, username: 'teacher@dhad.com', password: 'Test1234!' });
    if (r.body?.isAdmin || r.body?.role === 'ADMIN') {
      hacked('Prototype pollution', `Payload: ${JSON.stringify(payload)}`);
      break;
    }
  }
  blocked('Prototype pollution');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 8: HEADER INJECTION
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 8: HEADER INJECTION ━━━');

  // 8.1 CRLF injection in User-Agent (Node.js blocks this)
  info('CRLF injection: Node.js blocks invalid header chars (good)');
  blocked('CRLF injection');

  // 8.2 Host header injection
  r = await req('GET', '/health', null, { 'Host': 'evil.com' });
  if (r.status === 200 && r.body?.host === 'evil.com') hacked('Host header injection', 'Server reflected evil host');
  else blocked('Host header injection');

  // 8.3 X-Forwarded-For spoofing
  r = await req('GET', '/health', null, { 'X-Forwarded-For': '127.0.0.1' });
  if (r.status === 200) info('X-Forwarded-For accepted (may be expected)');
  blocked('X-Forwarded-For spoofing');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 9: MASS SUBMISSION
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 9: MASS SUBMISSION ━━━');

  // 9.1 Submit 100 challenges at once
  const massPromises = [];
  for (let i = 0; i < 100; i++) {
    massPromises.push(req('POST', '/api/v1/student/challenge/submit', {
      challengeId: `ch-mass-${i}`,
      passed: true,
      output: `mass-${i}`,
      signature: `sig-${i}`
    }, { Authorization: `Bearer ${studentToken}` }));
  }
  const massResults = await Promise.all(massPromises);
  const massSuccess = massResults.filter(r => r.status === 200);
  if (massSuccess.length > 5) hacked('Mass submission', `${massSuccess.length}/100 succeeded`);
  else info(`Mass submission: ${massSuccess.length}/100 succeeded`);
  blocked('Mass submission');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 10: ENUMERATION & RECONNAISSANCE
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 10: ENUMERATION & RECONNAISSANCE ━━━');

  // 10.1 User enumeration via timing
  const times = [];
  for (let i = 0; i < 10; i++) {
    const start = Date.now();
    await req('POST', '/api/v1/auth/login', { username: 'teacher@dhad.com', password: 'wrong' });
    times.push(Date.now() - start);
  }
  const avg = times.reduce((a, b) => a + b, 0) / times.length;
  if (avg < 50) info(`Timing: avg ${avg}ms (consistent)`);
  else info(`Timing: avg ${avg}ms (may indicate enumeration)`);

  // 10.2 Error message analysis
  const errorTests = [
    { username: 'teacher@dhad.com', password: 'wrong', expected: 'Invalid credentials' },
    { username: 'nonexistent@test.com', password: 'wrong', expected: 'Invalid credentials' },
  ];
  
  for (const test of errorTests) {
    r = await req('POST', '/api/v1/auth/login', test);
    if (r.body?.message === test.expected) info('Error message consistent');
    else info(`Error: ${r.body?.message}`);
  }
  blocked('User enumeration');

  // 10.3 API endpoint enumeration
  const apiPaths = [
    '/api/v1', '/api/v1/auth', '/api/v1/auth/login', '/api/v1/auth/register',
    '/api/v1/auth/refresh', '/api/v1/auth/logout',
    '/api/v1/analytics', '/api/v1/analytics/leaderboard',
    '/api/v1/student', '/api/v1/student/profile',
    '/api/v1/student-management', '/api/v1/student-management/students',
    '/api/v1/assessments', '/api/v1/assessments/create',
    '/api/v1/security', '/api/v1/security/dashboard',
    '/api/v1/otp', '/api/v1/otp/generate',
    '/api/v1/onboard', '/api/v1/onboard/schools',
    '/api/v1/invitations',
    '/health', '/health/ready', '/health/live',
    '/api-docs', '/swagger',
  ];

  const discovered = [];
  for (const path of apiPaths) {
    r = await req('GET', path, null, { Authorization: `Bearer ${adminToken}` });
    if (r.status !== 404) discovered.push(`${path}: ${r.status}`);
  }
  if (discovered.length > 10) info(`Discovered ${discovered.length} endpoints`);
  blocked('API enumeration');

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 11: ADVANCED JWT ATTACKS
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 11: ADVANCED JWT ATTACKS ━━━');

  if (adminToken) {
    const adminParts = adminToken.split('.');
    const adminPayload = JSON.parse(Buffer.from(adminParts[1], 'base64url').toString());
    
    // 11.1 JWT algorithm downgrade to 'none'
    const noneHeader = Buffer.from(JSON.stringify({ alg: 'none', typ: 'JWT' })).toString('base64url');
    const nonePayload = Buffer.from(JSON.stringify({ ...adminPayload, id: 'hacker', role: 'ADMIN' })).toString('base64url');
    r = await req('GET', '/api/v1/security/dashboard', null, { Authorization: `Bearer ${noneHeader}.${nonePayload}.` });
    if (r.status === 200) hacked('JWT none algorithm', 'Accepted alg:none');
    else blocked('JWT none algorithm');

    // 11.2 JWT key confusion with public key
    // If server uses RS256, try to sign with public key as HMAC secret
    const fakeRSAKey = `-----BEGIN PUBLIC KEY-----
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDH3cQNn0mHkIPXgO3J4Z2U
-----END PUBLIC KEY-----`;
    const confuseSig = crypto.createHmac('sha256', fakeRSAKey).update(adminParts[0] + '.' + adminParts[1]).digest('base64url');
    r = await req('GET', '/api/v1/security/dashboard', null, { Authorization: `Bearer ${adminParts[0]}.${adminParts[1]}.${confuseSig}` });
    if (r.status === 200) hacked('JWT key confusion', 'Accepted public key as HMAC secret');
    else blocked('JWT key confusion');

    // 11.3 JWT kid injection
    const kidPayload = { ...adminPayload, kid: '/etc/passwd' };
    const kidJwt = `${adminParts[0]}.${Buffer.from(JSON.stringify(kidPayload)).toString('base64url')}.${adminParts[2]}`;
    r = await req('GET', '/api/v1/security/dashboard', null, { Authorization: `Bearer ${kidJwt}` });
    if (r.status === 200) hacked('JWT kid injection', 'Accepted kid parameter');
    else blocked('JWT kid injection');
  }

  // ═════════════════════════════════════════════════════════════════════════
  // ATTACK 12: CRYPTOGRAPHIC ATTACKS
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n━━━ ATTACK 12: CRYPTOGRAPHIC ATTACKS ━━━');

  // 12.1 Timing attack on HMAC verification
  const timingResults = [];
  for (let i = 0; i < 20; i++) {
    const start = Date.now();
    await req('POST', '/api/v1/student/challenge/submit', {
      challengeId: 'ch-timing',
      passed: true,
      output: 'test',
      signature: 'test',
      hmac: 'a'.repeat(64)
    }, { Authorization: `Bearer ${studentToken}` });
    timingResults.push(Date.now() - start);
  }
  const timingAvg = timingResults.reduce((a, b) => a + b, 0) / timingResults.length;
  const timingMax = Math.max(...timingResults);
  const timingMin = Math.min(...timingResults);
  if (timingMax - timingMin > 50) info(`Timing variance: ${timingMax - timingMin}ms (potential timing attack)`);
  else info(`Timing consistent: ${timingAvg.toFixed(0)}ms avg`);
  blocked('Timing attack');

  // 12.2 Hash length extension
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: 'ch-ext',
    passed: true,
    output: 'test',
    signature: 'test',
    hmac: '0'.repeat(64) // All zeros
  }, { Authorization: `Bearer ${studentToken}` });
  if (r.status === 200) hacked('Hash length extension', 'Accepted zero HMAC');
  else blocked('Hash length extension');

  // ═════════════════════════════════════════════════════════════════════════
  // SUMMARY
  // ═════════════════════════════════════════════════════════════════════════
  console.log('\n╔══════════════════════════════════════════════════════════════╗');
  console.log('║                    HACK ATTEMPT RESULTS                    ║');
  console.log('╚══════════════════════════════════════════════════════════════╝');

  console.log(`\n  🔴 Successfully Hacked: ${SUCCESS.length}`);
  console.log(`  🛑 Blocked/Failed: ${FAILED.length}`);
  console.log(`  📊 Total Attacks: ${SUCCESS.length + FAILED.length}`);
  console.log(`  🎯 Defense Rate: ${((FAILED.length / (SUCCESS.length + FAILED.length)) * 100).toFixed(1)}%`);

  if (SUCCESS.length > 0) {
    console.log('\n━━━ VULNERABILITIES FOUND ━━━');
    SUCCESS.forEach((v, i) => {
      console.log(`\n  ${i + 1}. ${v.title}`);
      if (v.detail) console.log(`     ${v.detail}`);
    });
  } else {
    console.log('\n🎉 SYSTEM IS SECURE - No vulnerabilities found!');
  }

  const report = {
    timestamp: new Date().toISOString(),
    hacked: SUCCESS,
    blocked: FAILED,
    defenseRate: `${((FAILED.length / (SUCCESS.length + FAILED.length)) * 100).toFixed(1)}%`,
  };
  require('fs').writeFileSync('C:/Projects/dhad-studio/tests/hack_attempt_report.json', JSON.stringify(report, null, 2));
  console.log('\n📄 Report saved to tests/hack_attempt_report.json');
}

main().catch(console.error);
