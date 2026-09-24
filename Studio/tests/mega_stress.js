const http = require('http');
const fs = require('fs');

const BASE_HOST = 'localhost';
const BASE_PORT = 3000;
const DURATION = 30 * 60 * 1000;
const LOG_FILE = 'tests/mega_stress_log.json';

const stats = {
  totalRequests: 0,
  success: 0,
  clientErrors: 0,
  serverErrors: 0,
  rateLimited: 0,
  lockedOut: 0,
  hackAttempts: 0,
  hacksBlocked: 0,
  submissions: 0,
  submissionsPassed: 0,
  submissionsFailed: 0,
  assessmentsCreated: 0,
  challengeSubmits: 0,
  profileViews: 0,
  leaderboardViews: 0,
  roadmapViews: 0,
  achievementViews: 0,
  workspaceSaves: 0,
  classroomChecks: 0,
  assessmentViews: 0,
  dashboardViews: 0,
  errorCodes: {},
  startTime: Date.now(),
  log: [],
};

function apiReq(method, path, body, token) {
  return new Promise((resolve) => {
    const data = body ? JSON.stringify(body) : null;
    const headers = {
      'Content-Type': 'application/json',
      'User-Agent': 'DhadStress/4.0',
    };
    if (token) headers['Authorization'] = 'Bearer ' + token;

    const opts = {
      method, hostname: BASE_HOST, port: BASE_PORT,
      path, headers, timeout: 30000,
    };

    const r = http.request(opts, (res) => {
      let buf = '';
      res.on('data', (c) => buf += c);
      res.on('end', () => {
        stats.totalRequests++;
        const sc = res.statusCode;
        if (sc >= 200 && sc < 300) stats.success++;
        else if (sc === 429) { stats.rateLimited++; stats.hacksBlocked++; }
        else if (sc === 423) { stats.lockedOut++; stats.hacksBlocked++; }
        else if (sc >= 400 && sc < 500) stats.clientErrors++;
        else if (sc >= 500) stats.serverErrors++;
        stats.errorCodes[sc] = (stats.errorCodes[sc] || 0) + 1;
        try { resolve({ s: sc, d: JSON.parse(buf) }); }
        catch(e) { resolve({ s: sc, d: null }); }
      });
    });
    r.on('error', () => { stats.totalRequests++; stats.serverErrors++; resolve({ s: 0, d: null }); });
    r.on('timeout', () => { r.destroy(); stats.totalRequests++; stats.serverErrors++; resolve({ s: 0, d: null }); });
    if (data) r.write(data);
    r.end();
  });
}

async function login(natId) {
  const r = await apiReq('POST', '/api/v1/auth/login', { username: natId, password: 'Test1234!' });
  return r.d?.accessToken || null;
}

function pick(arr) { return arr[Math.floor(Math.random() * arr.length)]; }

// Student actions
async function studentLoop(token, challengeIds, assessmentIds) {
  while (Date.now() - stats.startTime < DURATION) {
    const roll = Math.random() * 100;
    try {
      if (roll < 35) {
        // Submit a challenge
        const chId = pick(challengeIds);
        const output = pick(['5','10','15','25','30','55','100','1000','42','720','9','3.14','0','YES','1','7','20','50','300']);
        const code = pick(['print(5+5)','print(10)','print(15)','if(true){print("YES")}','for(var i=0;i<10;i++){print(i)}']);
        const r = await apiReq('POST', '/api/v1/student/challenge/submit', {
          challengeId: chId, code, output,
        }, token);
        stats.challengeSubmits++;
        if (r.s === 200 && r.d?.passed) stats.submissionsPassed++;
        else stats.submissionsFailed++;
      } else if (roll < 50) {
        // View leaderboard
        await apiReq('GET', '/api/v1/student/leaderboard?limit=50', null, token);
        stats.leaderboardViews++;
      } else if (roll < 60) {
        // View profile
        await apiReq('GET', '/api/v1/student/profile', null, token);
        stats.profileViews++;
      } else if (roll < 68) {
        // View roadmap
        await apiReq('GET', '/api/v1/student/roadmap', null, token);
        stats.roadmapViews++;
      } else if (roll < 73) {
        // View achievements
        await apiReq('GET', '/api/v1/student/achievements', null, token);
        stats.achievementViews++;
      } else if (roll < 78) {
        // Save workspace
        await apiReq('POST', '/api/v1/workspace/save', {
          challengeId: pick(challengeIds), code: 'print("saved")', language: 'daad'
        }, token);
        stats.workspaceSaves++;
      } else if (roll < 82) {
        // Check classroom
        await apiReq('GET', '/api/v1/student/classroom/check', null, token);
        stats.classroomChecks++;
      } else if (roll < 87) {
        // View assessments
        await apiReq('GET', '/api/v1/student/assessments', null, token);
        stats.assessmentViews++;
      } else if (roll < 92) {
        // === HACK ATTEMPTS ===
        stats.hackAttempts++;
        const hackType = Math.random();
        if (hackType < 0.15) {
          // Try to submit with passed=true
          await apiReq('POST', '/api/v1/student/challenge/submit', {
            challengeId: pick(challengeIds), code: 'print("HACK")', output: 'HACK', passed: true
          }, token);
        } else if (hackType < 0.30) {
          // Try to access teacher dashboard
          await apiReq('GET', '/api/v1/analytics/dashboard', null, token);
        } else if (hackType < 0.45) {
          // SQL injection attempt
          await apiReq('POST', '/api/v1/auth/login', { username: "admin' OR '1'='1", password: "test" });
        } else if (hackType < 0.60) {
          // XSS attempt
          await apiReq('POST', '/api/v1/auth/login', { username: '<script>alert(1)</script>', password: 'x' });
        } else if (hackType < 0.75) {
          // Try to create assessment as student
          await apiReq('POST', '/api/v1/assessments/create', {
            classroomId: 'fake', title: 'HACK', allowedTime: 60,
            startTime: new Date().toISOString(), endTime: new Date(Date.now()+3600000).toISOString()
          }, token);
        } else {
          // Try to access admin endpoints
          await apiReq('GET', '/api/v1/analytics/security-alerts', null, token);
        }
      } else {
        // Idle wait
        await new Promise(r => setTimeout(r, 500 + Math.random() * 3000));
      }
    } catch(e) {}
    await new Promise(r => setTimeout(r, 100 + Math.random() * 1500));
  }
}

// Teacher actions
async function teacherLoop(token, classroomIds) {
  while (Date.now() - stats.startTime < DURATION) {
    const roll = Math.random() * 100;
    try {
      if (roll < 20) {
        // Create assessment
        const now = new Date();
        const future = new Date(now.getTime() + 3600000);
        await apiReq('POST', '/api/v1/assessments/create', {
          classroomId: pick(classroomIds),
          title: 'Exam_' + Math.floor(Math.random()*10000),
          description: 'Auto-generated assessment',
          assessmentType: pick(['CODING','MULTIPLE_CHOICE']),
          allowedTime: 60, startTime: now.toISOString(), endTime: future.toISOString(),
          totalPoints: 100, passingScore: 60, isPublished: true,
        }, token);
        stats.assessmentsCreated++;
      } else if (roll < 35) {
        await apiReq('GET', '/api/v1/analytics/dashboard', null, token);
        stats.dashboardViews++;
      } else if (roll < 50) {
        await apiReq('GET', '/api/v1/analytics/leaderboard', null, token);
        stats.leaderboardViews++;
      } else if (roll < 60) {
        await apiReq('GET', '/api/v1/analytics/progress', null, token);
      } else if (roll < 70) {
        await apiReq('GET', '/api/v1/analytics/classrooms', null, token);
      } else if (roll < 75) {
        await apiReq('GET', '/api/v1/analytics/assessments', null, token);
        stats.assessmentViews++;
      } else if (roll < 80) {
        await apiReq('GET', '/api/v1/analytics/security-alerts', null, token);
      } else if (roll < 85) {
        // Teacher hack attempt
        stats.hackAttempts++;
        await apiReq('GET', "/api/v1/analytics/leaderboard?classroom=' OR 1=1--", null, token);
      } else {
        await new Promise(r => setTimeout(r, 2000 + Math.random() * 5000));
      }
    } catch(e) {}
    await new Promise(r => setTimeout(r, 500 + Math.random() * 2000));
  }
}

async function main() {
  console.log('');
  console.log('===========================================================');
  console.log('  Dhad Studio - MEGA STRESS TEST');
  console.log('  10,000 Students + 100 Teachers = 30 Minutes');
  console.log('===========================================================');
  console.log('');

  // Phase 1: Fetch challenge and assessment IDs
  console.log('[1] Fetching real challenge IDs...');
  const challengeIds = [];
  const assessmentIds = [];
  const classroomIds = [];

  // Login as first teacher to get data
  const firstTeacherToken = await login('1000000000');
  if (!firstTeacherToken) {
    console.log('FATAL: Cannot login. Is server running?');
    return;
  }

  const chRes = await apiReq('GET', '/api/v1/challenges', null, firstTeacherToken);
  if (chRes.d?.challenges) {
    chRes.d.challenges.forEach(c => challengeIds.push(c.id));
  }
  console.log('  Got', challengeIds.length, 'challenges');

  // Get classrooms from DB
  const classRes = await apiReq('GET', '/api/v1/analytics/classrooms', null, firstTeacherToken);
  if (classRes.d?.classrooms) {
    classRes.d.classrooms.forEach(c => classroomIds.push(c.id));
  }
  console.log('  Got', classroomIds.length, 'classrooms');

  // Phase 2: Login batch of students (stagger to avoid rate limits)
  console.log('\n[2] Mass login students (10,000 students)...');
  const students = [];
  const BATCH = 20;
  const totalStudents = 10000;

  for (let i = 0; i < totalStudents; i += BATCH) {
    const batch = [];
    for (let j = i; j < Math.min(i + BATCH, totalStudents); j++) {
      const natId = String(1000000000 + j);
      batch.push((async () => {
        const t = await login(natId);
        if (t) students.push(t);
      })());
    }
    await Promise.all(batch);
    if (i % 1000 === 0 && i > 0) {
      console.log('  Logged in:', students.length, '/', i + BATCH);
    }
    await new Promise(r => setTimeout(r, 100));
  }
  console.log('  Students logged in:', students.length);

  // Phase 3: Login teachers
  console.log('\n[3] Logging in 100 teachers...');
  const teachers = [];
  for (let s = 0; s < 10; s++) {
    for (let t = 0; t < 10; t++) {
      const email = 'teacher_' + (s+1) + '_' + (t+1) + '@test.com';
      const token = await login(email);
      if (token) teachers.push(token);
    }
  }
  console.log('  Teachers logged in:', teachers.length);

  // Phase 4: 30-minute concurrent session
  console.log('\n[4] Starting 30-minute session...');
  console.log('  Students: solving challenges, viewing data, trying to hack');
  console.log('  Teachers: creating assessments, viewing reports');
  console.log('');

  const allWork = [];

  // Launch all students (max 500 concurrent to avoid overwhelming)
  const STUDENT_CONCURRENCY = 500;
  for (let i = 0; i < Math.min(students.length, STUDENT_CONCURRENCY); i++) {
    allWork.push(studentLoop(students[i], challengeIds, assessmentIds));
  }

  // Launch all teachers
  for (const t of teachers) {
    allWork.push(teacherLoop(t, classroomIds));
  }

  // Stats reporter
  const reporter = setInterval(() => {
    const elapsed = Math.floor((Date.now() - stats.startTime) / 1000);
    const min = Math.floor(elapsed / 60);
    const sec = elapsed % 60;
    const rps = stats.totalRequests / (elapsed || 1);
    console.log(
      '  [' + String(min).padStart(2,'0') + ':' + String(sec).padStart(2,'0') + '] ' +
      'Req:' + stats.totalRequests +
      ' | OK:' + stats.success +
      ' | Sub:' + stats.submissionsPassed + '/' + (stats.submissionsPassed + stats.submissionsFailed) +
      ' | Hack:' + stats.hacksBlocked + '/' + stats.hackAttempts +
      ' | RPS:' + rps.toFixed(1) +
      ' | 5xx:' + stats.serverErrors
    );
  }, 30000);

  await Promise.all(allWork);
  clearInterval(reporter);

  // Final report
  const elapsed = (Date.now() - stats.startTime) / 1000;
  const rps = stats.totalRequests / elapsed;

  console.log('');
  console.log('===========================================================');
  console.log('  FINAL REPORT');
  console.log('===========================================================');
  console.log('  Duration:           ' + Math.floor(elapsed) + 's (' + (elapsed/60).toFixed(1) + ' min)');
  console.log('  Total Requests:     ' + stats.totalRequests);
  console.log('  Success (2xx):      ' + stats.success);
  console.log('  Client Errors:      ' + stats.clientErrors);
  console.log('  Server Errors:      ' + stats.serverErrors);
  console.log('  Rate Limited (429): ' + stats.rateLimited);
  console.log('  Locked Out (423):   ' + stats.lockedOut);
  console.log('  Requests/sec:       ' + rps.toFixed(2));
  console.log('');
  console.log('  --- Student Activity ---');
  console.log('  Challenge Submits:  ' + stats.challengeSubmits);
  console.log('    Passed:           ' + stats.submissionsPassed);
  console.log('    Failed:           ' + stats.submissionsFailed);
  console.log('  Profile Views:      ' + stats.profileViews);
  console.log('  Leaderboard Views:  ' + stats.leaderboardViews);
  console.log('  Roadmap Views:      ' + stats.roadmapViews);
  console.log('  Achievement Views:  ' + stats.achievementViews);
  console.log('  Workspace Saves:    ' + stats.workspaceSaves);
  console.log('  Classroom Checks:   ' + stats.classroomChecks);
  console.log('  Assessment Views:   ' + stats.assessmentViews);
  console.log('');
  console.log('  --- Teacher Activity ---');
  console.log('  Assessments Created:' + stats.assessmentsCreated);
  console.log('  Dashboard Views:    ' + stats.dashboardViews);
  console.log('');
  console.log('  --- Security ---');
  console.log('  Hack Attempts:      ' + stats.hackAttempts);
  console.log('  Hacks Blocked:      ' + stats.hacksBlocked);
  console.log('');
  console.log('  --- Error Breakdown ---');
  for (const [code, count] of Object.entries(stats.errorCodes).sort((a,b) => b[1]-a[1])) {
    console.log('    HTTP ' + code + ': ' + count);
  }
  console.log('===========================================================');

  // Save log
  fs.writeFileSync(LOG_FILE, JSON.stringify(stats, null, 2));
  console.log('Log saved to ' + LOG_FILE);
}

main().catch(console.error);
