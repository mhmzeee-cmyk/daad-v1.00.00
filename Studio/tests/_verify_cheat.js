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
  const loginRes = await req('POST', '/api/v1/auth/login', { username: '1000000000', password: 'Test1234!' });
  const token = loginRes.d?.accessToken;
  if (!token) { console.log('Login failed'); return; }

  const chRes = await req('GET', '/api/v1/challenges', null, token);
  const challenges = chRes.d?.challenges || [];
  const ch1 = challenges.find(c => c.tier === 1);
  if (!ch1) { console.log('No T1 challenge'); return; }

  console.log('Testing challenge:', ch1.id, ch1.title);
  console.log('Expected output:', ch1.expectedOutput);
  console.log('');

  // Test 1: Cheat - output matches but code is garbage
  let r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: 'GARBAGE_CODE_NO_PRINT', output: ch1.expectedOutput
  }, token);
  console.log('Cheat 1 - Wrong code, right output:');
  console.log('  passed:', r.d?.passed, '| message:', r.d?.message);

  // Test 2: Cheat - client says passed=true
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: 'print(5+5)', output: '10', passed: true
  }, token);
  console.log('\nCheat 2 - Client passed=true:');
  console.log('  passed:', r.d?.passed, '| message:', r.d?.message);

  // Test 3: Cheat - empty code
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: '', output: ch1.expectedOutput
  }, token);
  console.log('\nCheat 3 - Empty code, right output:');
  console.log('  passed:', r.d?.passed, '| message:', r.d?.message);

  // Test 4: Cheat - extra XP fields
  r = await req('POST', '/api/v1/student/challenge/submit', {
    challengeId: ch1.id, code: 'print(5+5)', output: '10', xpAwarded: 99999, level: 99
  }, token);
  console.log('\nCheat 4 - Extra xpAwarded/level fields:');
  console.log('  passed:', r.d?.passed, '| xpAwarded:', r.d?.xpAwarded);

  // Test 5: Verify XP didn't change
  const profRes = await req('GET', '/api/v1/student/profile', null, token);
  console.log('\nFinal XP:', profRes.d?.profile?.totalXP);
  console.log('Final Level:', profRes.d?.profile?.currentLevel);
}

main();
