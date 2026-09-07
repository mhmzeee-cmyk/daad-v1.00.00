const http = require('http');
function req(m, p, d, h = {}) {
  return new Promise(r => {
    const u = new URL(p, 'http://localhost:3000');
    const o = { hostname: u.hostname, port: u.port, path: u.pathname, method: m, headers: { 'Content-Type': 'application/json', ...h } };
    const q = http.request(o, res => {
      let b = '';
      res.on('data', c => b += c);
      res.on('end', () => r({ status: res.statusCode, body: b, cookies: res.headers['set-cookie'] }));
    });
    q.on('error', e => r({ status: 0, body: e.message, cookies: [] }));
    if (d) q.write(JSON.stringify(d));
    q.end();
  });
}
async function test() {
  const cs = await req('GET', '/api/v1/csrf-token');
  const t = JSON.parse(cs.body).csrfToken;
  const l = await req('POST', '/api/v1/auth/login', { username: 'student_1@test.com', password: 'Test1234!' }, { 'X-CSRF-Token': t });
  let at;
  for (const c of l.cookies || []) {
    if (c.startsWith('access_token=')) { at = c.split(';')[0].split('=')[1]; break; }
  }
  const a = { 'Authorization': 'Bearer ' + at };
  const urls = [
    ['/GET', '/api/v1/student/profile'],
    ['/GET', '/api/v1/challenges'],
    ['/GET', '/api/v1/student/assessments'],
    ['/GET', '/api/v1/student/leaderboard'],
  ];
  for (const [m, u] of urls) {
    const r = await req('GET', u, null, a);
    console.log(u, r.status, r.body.substring(0, 80));
  }
  // Test POST routes
  const vr = await req('POST', '/api/v1/challenge/verify', {
    studentId: 'student_1', challengeId: 'challenge_1',
    code: 'test', output: 'test', passed: true, syntaxScore: 100, performanceScore: 100
  }, { 'Authorization': 'Bearer ' + at, 'X-CSRF-Token': t });
  console.log('/challenge/verify', vr.status, vr.body.substring(0, 80));
  
  const sr = await req('POST', '/api/v1/submit-solution', {
    challengeId: 'challenge_1', code: 'test', output: 'test', passed: true, syntaxScore: 100, performanceScore: 100
  }, { 'Authorization': 'Bearer ' + at, 'X-CSRF-Token': t });
  console.log('/submit-solution', sr.status, sr.body.substring(0, 80));
}
test();
