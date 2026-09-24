const http = require('http');
const fs = require('fs');

const DURATION = 60 * 60 * 1000; // 60 minutes
const LOG_FILE = 'tests/hour_stress_log.json';

const stats = {
  totalRequests: 0, success: 0, clientErrors: 0, serverErrors: 0,
  rateLimited: 0, lockedOut: 0,
  hackAttempts: 0, hacksBlocked: 0,
  submissions: 0, passed: 0, failed: 0,
  tierBlocked: 0, seqBlocked: 0,
  assessmentsCreated: 0,
  teacherActions: 0, studentActions: 0,
  loginsOk: 0, loginsFailed: 0,
  errorCodes: {}, startTime: Date.now(),
};

function api(method, path, body, token) {
  return new Promise((resolve) => {
    const data = body ? JSON.stringify(body) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'DhadHourTest/5.0' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, hostname: 'localhost', port: 3000, path: encodeURI(path), headers, timeout: 30000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => {
        stats.totalRequests++;
        const sc = res.statusCode;
        if (sc >= 200 && sc < 300) stats.success++;
        else if (sc === 429) { stats.rateLimited++; stats.hacksBlocked++; }
        else if (sc === 423) { stats.lockedOut++; stats.hacksBlocked++; }
        else if (sc >= 400 && sc < 500) stats.clientErrors++;
        else if (sc >= 500) stats.serverErrors++;
        stats.errorCodes[sc] = (stats.errorCodes[sc] || 0) + 1;
        try { resolve({ s: sc, d: JSON.parse(buf) }); } catch(e) { resolve({ s: sc, d: null }); }
      });
    });
    r.on('error', () => { stats.totalRequests++; stats.serverErrors++; resolve({ s: 0, d: null }); });
    r.on('timeout', () => { r.destroy(); stats.totalRequests++; stats.serverErrors++; resolve({ s: 0, d: null }); });
    if (data) r.write(data);
    r.end();
  });
}

function sleep(ms) { return new Promise(r => setTimeout(r, ms)); }

async function login(id) {
  const r = await api('POST', '/api/v1/auth/login', { username: id, password: 'Test1234!' });
  if (r.s === 200 && r.d?.accessToken) { stats.loginsOk++; return r.d.accessToken; }
  stats.loginsFailed++;
  return null;
}

function pick(a) { return a[Math.floor(Math.random() * a.length)]; }

class ConcurrencyLimiter {
  constructor(max) { this.max = max; this.running = 0; this.queue = []; }
  async run(fn) {
    while (this.running >= this.max) await new Promise(r => this.queue.push(r));
    this.running++;
    try { return await fn(); } finally { this.running--; if (this.queue.length) this.queue.shift()(); }
  }
}
const limiter = new ConcurrencyLimiter(5);

async function studentWork(token, tierChallenges, allChallengeIds) {
  const speed = 0.3 + Math.random() * 2.5;

  for (let tier = 1; tier <= 5; tier++) {
    const challenges = tierChallenges[tier] || [];
    if (challenges.length === 0) continue;

    for (let i = 0; i < challenges.length; i++) {
      if (Date.now() - stats.startTime > DURATION) return;

      const ch = challenges[i];
      const outputs = ['5','10','15','25','30','55','100','1000','42','720','9','3.14','0','YES','1','7','20','50','300'];
      const codes = ['print(5+5)','print(10)','print(15)','if(true){print("YES")}','print(50)','print(100)'];

      const r = await limiter.run(() =>
        api('POST', '/api/v1/student/challenge/submit', {
          challengeId: ch.id, code: pick(codes), output: pick(outputs),
        }, token)
      );
      stats.submissions++;
      stats.studentActions++;
      if (r.s === 200 && r.d?.passed) stats.passed++;
      else if (r.s === 200) stats.failed++;
      else if (r.s === 403 && r.d?.tierLocked) stats.tierBlocked++;
      else if (r.s === 403 && r.d?.challengeLocked) stats.seqBlocked++;

      await sleep((500 + Math.random() * 1500) / speed);
    }
  }

  while (Date.now() - stats.startTime < DURATION) {
    try {
      const roll = Math.random();
      if (roll < 0.3) await api('GET', '/api/v1/student/leaderboard?limit=50', null, token);
      else if (roll < 0.5) await api('GET', '/api/v1/student/profile', null, token);
      else if (roll < 0.6) await api('GET', '/api/v1/student/roadmap', null, token);
      else if (roll < 0.7) await api('GET', '/api/v1/student/achievements', null, token);
      else if (roll < 0.75) {
        stats.hackAttempts++;
        const hackType = Math.random();
        if (hackType < 0.3) await api('POST', '/api/v1/student/challenge/submit', { challengeId: 'hack', code: 'x', output: 'y' }, token);
        else if (hackType < 0.5) await api('GET', '/api/v1/analytics/dashboard', null, token);
        else if (hackType < 0.7) await api('POST', '/api/v1/auth/login', { username: "admin' OR '1'='1", password: "x" });
        else if (hackType < 0.85) await api('POST', '/api/v1/assessments/create', { classroomId: 'fake', title: 'HACK', allowedTime: 60 }, token);
        else await api('POST', '/api/v1/invitations/send', { email: 'hack@evil.com', role: 'ADMIN' }, token);
      }
      else if (allChallengeIds.length > 0) {
        await api('POST', '/api/v1/workspace/save', { challengeId: pick(allChallengeIds), code: 'print("x")', language: 'daad' }, token);
      }
      else await api('GET', '/api/v1/student/profile', null, token);
    } catch (e) {}
    await sleep(800 / speed + Math.random() * 2000);
  }
}

async function teacherWork(token, classroomIds, targetAssessments) {
  let created = 0;
  while (Date.now() - stats.startTime < DURATION) {
    try {
      const roll = Math.random();
      if (roll < 0.4 && created < targetAssessments && classroomIds.length > 0) {
        const now = new Date();
        const future = new Date(now.getTime() + 3600000);
        const r = await api('POST', '/api/v1/assessments/create', {
          classroomId: pick(classroomIds),
          title: 'AutoExam_' + Date.now() + '_' + Math.floor(Math.random()*9999),
          description: 'Assessment auto-generated during stress test',
          assessmentType: pick(['CODING','MULTIPLE_CHOICE']),
          allowedTime: pick([30, 45, 60, 90]),
          startTime: now.toISOString(), endTime: future.toISOString(),
          totalPoints: 100, passingScore: pick([50, 60, 70]),
          isPublished: true,
        }, token);
        if (r.s === 200) { stats.assessmentsCreated++; created++; }
        stats.teacherActions++;
      } else if (roll < 0.55) {
        await api('GET', '/api/v1/analytics/dashboard', null, token);
        stats.teacherActions++;
      } else if (roll < 0.65) {
        await api('GET', '/api/v1/analytics/leaderboard', null, token);
        stats.teacherActions++;
      } else if (roll < 0.72) {
        await api('GET', '/api/v1/analytics/progress', null, token);
        stats.teacherActions++;
      } else if (roll < 0.78) {
        await api('GET', '/api/v1/analytics/classrooms', null, token);
        stats.teacherActions++;
      } else if (roll < 0.82) {
        await api('GET', '/api/v1/analytics/assessments', null, token);
        stats.teacherActions++;
      } else if (roll < 0.85) {
        await api('GET', '/api/v1/analytics/security-alerts', null, token);
        stats.teacherActions++;
      } else if (roll < 0.88) {
        stats.hackAttempts++;
        await api('GET', "/api/v1/analytics/leaderboard?classroom=hack" + encodeURIComponent("' OR 1=1--"), null, token);
      }
    } catch (e) {}
    await sleep(1000 + Math.random() * 4000);
  }
}

async function main() {
  console.log('═══════════════════════════════════════════════════════');
  console.log('  Dhad Studio - 1 HOUR STRESS TEST v3');
  console.log('  Sequential logins + WAL mode + concurrency limiter');
  console.log('  Teachers: create 100+ assessments');
  console.log('  Students: solve challenges sequentially');
  console.log('═══════════════════════════════════════════════════════\n');

  console.log('[1] Login bootstrap + fetch data...');
  const allTeacherEmails = [];
  for (let s = 0; s < 10; s++) {
    for (let t = 0; t < 10; t++) {
      allTeacherEmails.push('teacher_' + (s+1) + '_' + (t+1) + '@test.com');
    }
  }
  const allStudentIds = [];
  for (let i = 0; i < 2000; i++) allStudentIds.push(String(1000000000 + i));

  let bootstrapToken = await login(allTeacherEmails[0]);
  if (!bootstrapToken) bootstrapToken = await login(allStudentIds[0]);
  if (!bootstrapToken) { console.log('FATAL: Server not running!'); return; }
  await sleep(2000);

  const chRes = await api('GET', '/api/v1/challenges', null, bootstrapToken);
  const allChallenges = chRes.d?.challenges || chRes.d?.data || [];
  const tierChallenges = {};
  for (let t = 1; t <= 5; t++) tierChallenges[t] = allChallenges.filter(c => c.tier === t);
  for (let t = 1; t <= 5; t++) console.log('  Tier ' + t + ': ' + tierChallenges[t].length + ' challenges');
  const allChallengeIds = allChallenges.map(c => c.id);

  const classRes = await api('GET', '/api/v1/analytics/classrooms', null, bootstrapToken);
  const classroomIds = (classRes.d?.classrooms || []).map(c => c.id);
  console.log('  Classrooms: ' + classroomIds.length);

  console.log('\n[2] Sequential student logins (max 200, 1 per 2.5s)...');
  const students = [];
  const MAX_STUDENTS = 200;
  for (let i = 0; i < MAX_STUDENTS; i++) {
    const tok = await login(allStudentIds[i]);
    if (tok) students.push(tok);
    if ((i+1) % 20 === 0) console.log('  Logged in: ' + students.length + '/' + (i+1));
    await sleep(2500);
  }
  console.log('  Students ready: ' + students.length);

  console.log('\n[3] Waiting 65s for rate limit window to clear...');
  await sleep(65000);
  console.log('  Logging in teachers...');

  const teachers = [];
  for (let i = 0; i < allTeacherEmails.length; i++) {
    const tok = await login(allTeacherEmails[i]);
    if (tok) teachers.push(tok);
    if ((i+1) % 10 === 0) console.log('  Teachers logged in: ' + teachers.length + '/' + (i+1));
    await sleep(2500);
  }
  console.log('  Teachers ready: ' + teachers.length);

  console.log('\n[4] Starting 1-hour session...');
  console.log('  Students: solving tier 1 -> 2 -> 3 -> 4 -> 5 sequentially');
  console.log('  Teachers: creating 100 assessments + viewing reports');
  console.log('');

  const allWork = [];
  for (const tok of students) allWork.push(studentWork(tok, tierChallenges, allChallengeIds));
  for (const tok of teachers) allWork.push(teacherWork(tok, classroomIds, Math.ceil(100 / Math.max(teachers.length, 1))));

  const reporter = setInterval(() => {
    const el = Math.floor((Date.now() - stats.startTime) / 1000);
    const m = Math.floor(el/60), s = el%60;
    const rps = stats.totalRequests / (el || 1);
    console.log(
      '  [' + String(m).padStart(2,'0') + ':' + String(s).padStart(2,'0') + '] ' +
      'Req:' + stats.totalRequests +
      ' | OK:' + stats.success +
      ' | Sub:' + stats.passed + '/' + stats.submissions + ' passed' +
      ' | Assess:' + stats.assessmentsCreated +
      ' | TierBlock:' + stats.tierBlocked +
      ' | SeqBlock:' + stats.seqBlocked +
      ' | RPS:' + rps.toFixed(1) +
      ' | 5xx:' + stats.serverErrors
    );
  }, 30000);

  await Promise.all(allWork);
  clearInterval(reporter);

  const elapsed = (Date.now() - stats.startTime) / 1000;
  const rps = stats.totalRequests / elapsed;

  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  1-HOUR STRESS TEST v3 - FINAL REPORT');
  console.log('═══════════════════════════════════════════════════════');
  console.log('  Duration:           ' + Math.floor(elapsed) + 's (' + (elapsed/60).toFixed(1) + ' min)');
  console.log('  Total Requests:     ' + stats.totalRequests);
  console.log('  Success (2xx):      ' + stats.success);
  console.log('  Client Errors:      ' + stats.clientErrors);
  console.log('  Server Errors:      ' + stats.serverErrors);
  console.log('  Rate Limited:       ' + stats.rateLimited);
  console.log('  Locked Out:         ' + stats.lockedOut);
  console.log('  Requests/sec:       ' + rps.toFixed(2));
  console.log('');
  console.log('  --- Logins ---');
  console.log('  Successful:         ' + stats.loginsOk);
  console.log('  Failed:             ' + stats.loginsFailed);
  console.log('');
  console.log('  --- Challenges ---');
  console.log('  Submissions:        ' + stats.submissions);
  console.log('    Passed:           ' + stats.passed);
  console.log('    Failed:           ' + stats.failed);
  console.log('  Pass rate:          ' + (stats.submissions > 0 ? (stats.passed/stats.submissions*100).toFixed(1) : 0) + '%');
  console.log('  Tier blocked:       ' + stats.tierBlocked);
  console.log('  Sequential blocked: ' + stats.seqBlocked);
  console.log('');
  console.log('  --- Assessments ---');
  console.log('  Created by teachers:' + stats.assessmentsCreated);
  console.log('');
  console.log('  --- Security ---');
  console.log('  Hack attempts:      ' + stats.hackAttempts);
  console.log('  Hacks blocked:      ' + stats.hacksBlocked);
  console.log('  Student actions:    ' + stats.studentActions);
  console.log('  Teacher actions:    ' + stats.teacherActions);
  console.log('');
  console.log('  --- Error Breakdown ---');
  for (const [code, count] of Object.entries(stats.errorCodes).sort((a,b) => b[1]-a[1])) {
    console.log('    HTTP ' + code + ': ' + count);
  }
  console.log('═══════════════════════════════════════════════════════');

  fs.writeFileSync(LOG_FILE, JSON.stringify(stats, null, 2));
  console.log('Log saved to ' + LOG_FILE);
}

main().catch(console.error);
