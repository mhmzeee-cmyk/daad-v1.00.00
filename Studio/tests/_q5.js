const http = require('http');

function api(method, path, body, token) {
  return new Promise((resolve) => {
    const data = body ? JSON.stringify(body) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'Test/1.0' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, hostname: 'localhost', port: 3000, path, headers, timeout: 10000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => { try { resolve({ s: res.statusCode, d: JSON.parse(buf) }); } catch(e) { resolve({ s: res.statusCode, d: null }); } });
    });
    r.on('error', () => resolve({ s: 0, d: null }));
    r.on('timeout', () => { r.destroy(); resolve({ s: 0, d: null }); });
    if (data) r.write(data);
    r.end();
  });
}

async function main() {
  // Login as Mohammed_1 (has submissions)
  const loginRes = await api('POST', '/api/v1/auth/login', { username: '1000000000', password: 'Test1234!' });
  if (!loginRes.d?.accessToken) { console.log('Login failed'); return; }
  const token = loginRes.d.accessToken;
  console.log('Logged in as Mohammed_1');

  // Get profile
  const profileRes = await api('GET', '/api/v1/student/profile', null, token);
  const p = profileRes.d?.profile || profileRes.d;
  console.log('\n=== Profile ===');
  console.log('Name:', p.studentName);
  console.log('Total XP:', p.totalXP);
  console.log('Level:', p.currentLevel);
  console.log('Streak:', p.currentStreak);
  console.log('Total Challenges:', p.totalChallenges);
  console.log('Rank:', p.rank, '/', p.totalStudents);
  console.log('Today XP:', p.todayXP);
  console.log('Achievements:', p.totalAchievements);
  console.log('\n=== Recent Activities ===');
  (p.recentActivities || []).slice(0, 5).forEach(function(a) {
    console.log('  ' + a.action + ' - ' + a.createdAt);
  });
}

main();
