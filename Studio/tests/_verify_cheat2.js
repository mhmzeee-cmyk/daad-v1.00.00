const http = require('http');
function req(method, path, body, token) {
  return new Promise((resolve) => {
    const data = body ? JSON.stringify(body) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'Verify/1.0' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, hostname: 'localhost', port: 3000, path: encodeURI(path), headers, timeout: 10000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => { try { resolve({ s: res.statusCode, d: JSON.parse(buf) }); } catch(e) { resolve({ s: res.statusCode }); } });
    });
    r.on('error', () => resolve({ s: 0 }));
    r.on('timeout', () => { r.destroy(); resolve({ s: 0 }); });
    if (data) r.write(data);
    r.end();
  });
}

async function main() {
  // Use a fresh student
  const loginRes = await req('POST', '/api/v1/auth/login', { username: '1000000088', password: 'Test1234!' });
  const token = loginRes.d?.accessToken;
  if (!token) { console.log('Login failed'); return; }
  console.log('Logged in as student 1000000050');

  const chRes = await req('GET', '/api/v1/challenges', null, token);
  const challenges = chRes.d?.challenges || [];
  const ch1 = challenges.find(c => c.tier === 1);
  if (!ch1) { console.log('No T1 challenge'); return; }

  console.log('Testing:', ch1.title);
  console.log('Expected output:', JSON.stringify(ch1.expectedOutput));
  console.log('');

  // Get initial XP
  let prof = await req('GET', '/api/v1/student/profile', null, token);
  console.log('Initial XP:', prof.d?.profile?.totalXP);

  // CHEAT 1: Code is garbage, output matches
  let r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: 'XYZ123_GARBAGE', output: ch1.expectedOutput
  }, token);
  console.log('\n[CHEAT 1] Wrong code, right output:');
  console.log('  passed:', r.d?.passed, '| xpAwarded:', r.d?.xpAwarded, '| msg:', r.d?.message);
  if (r.d?.codeErrors) console.log('  codeErrors:', r.d.codeErrors);

  // CHEAT 2: Client says passed=true
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: 'print(999)', output: '999', passed: true
  }, token);
  console.log('\n[CHEAT 2] Client passed=true, wrong output:');
  console.log('  passed:', r.d?.passed, '| xpAwarded:', r.d?.xpAwarded, '| msg:', r.d?.message);

  // CHEAT 3: Extra fields
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: 'print(5+5)', output: '10', xpAwarded: 99999, level: 99
  }, token);
  console.log('\n[CHEAT 3] Extra xpAwarded=99999:');
  console.log('  passed:', r.d?.passed, '| xpAwarded:', r.d?.xpAwarded);

  // CHEAT 4: Empty code
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: '', output: ch1.expectedOutput
  }, token);
  console.log('\n[CHEAT 4] Empty code:');
  console.log('  passed:', r.d?.passed, '| xpAwarded:', r.d?.xpAwarded, '| msg:', r.d?.message);

  // Verify XP unchanged
  prof = await req('GET', '/api/v1/student/profile', null, token);
  console.log('\nFinal XP:', prof.d?.profile?.totalXP);
  console.log('XP changed:', prof.d?.profile?.totalXP !== 0 ? 'YES (BAD!)' : 'NO (GOOD!)');
}

main();
