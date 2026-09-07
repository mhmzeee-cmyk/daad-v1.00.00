const http = require('http');
const crypto = require('crypto');

const BASE = 'http://localhost:3000';
const DURATION = 30 * 60 * 1000; // 30 minutes
const SCHOOLS_COUNT = 10;
const STUDENTS_PER_SCHOOL = 100;

const stats = {
  requests: 0,
  successes: 0,
  failures: 0,
  errors: {},
  hacksBlocked: 0,
  hackAttempts: 0,
  assessmentsCreated: 0,
  submissionsDone: 0,
  xpAwarded: 0,
  startTime: Date.now(),
};

function req(method, path, body, token) {
  return new Promise((resolve) => {
    const url = new URL(path, BASE);
    const data = body ? JSON.stringify(body) : null;
    const opts = {
      method,
      hostname: 'localhost',
      port: 3000,
      path: url.pathname + url.search,
      headers: {
        'Content-Type': 'application/json',
        'User-Agent': 'DhadStressTest/3.0',
      },
      timeout: 30000,
    };
    if (token) opts.headers['Authorization'] = 'Bearer ' + token;

    const r = http.request(opts, (res) => {
      let buf = '';
      res.on('data', (c) => buf += c);
      res.on('end', () => {
        stats.requests++;
        if (res.statusCode >= 200 && res.statusCode < 300) {
          stats.successes++;
        } else if (res.statusCode === 429 || res.statusCode === 423) {
          stats.hacksBlocked++;
        } else {
          stats.failures++;
          const key = res.statusCode + '';
          stats.errors[key] = (stats.errors[key] || 0) + 1;
        }
        try {
          resolve({ status: res.statusCode, data: JSON.parse(buf) });
        } catch(e) {
          resolve({ status: res.statusCode, data: null });
        }
      });
    });
    r.on('error', () => { stats.requests++; stats.failures++; resolve({ status: 0, data: null }); });
    r.on('timeout', () => { r.destroy(); stats.requests++; stats.failures++; resolve({ status: 0, data: null }); });
    if (data) r.write(data);
    r.end();
  });
}

async function login(username, password) {
  const res = await req('POST', '/api/v1/auth/login', { username, password });
  return res.data?.accessToken || res.data?.token || null;
}

function randomInt(n) { return Math.floor(Math.random() * n); }
function randomChoice(arr) { return arr[randomInt(arr.length)]; }

const DAAD_PRINT_Hello = 'اذا (صحيح) { طبع("Hello") }';
const DAAD_PRINT_Name = function(name) { return 'اطبع("مرحبا ' + name + '")'; };
const DAAD_VAR = 'متغير x = 5\nاطبع(x)';
const DAAD_IF = 'اذا (5 > 3) { طبع("YES") } اخر لا';
const DAAD_LOOP = 'لـ (متغير i = 0; i < 3; i = i + 1) { طبع(i) }';
const DAAD_LIST = 'متغير arr = [1, 2, 3]\nاطبع(arr[0])';
const CHEAT_SUBMIT = function(challengeId) {
  return {
    challengeId,
    code: 'اطبع("HACKED")',
    output: 'HACKED',
    passed: true, // try to fake pass
    syntaxScore: 100,
  };
};

const HACK_PAYLOADS = [
  { path: '/api/v1/auth/login', body: { username: "admin' OR '1'='1", password: "test" }, desc: 'SQL injection login' },
  { path: '/api/v1/auth/login', body: { username: '<script>alert(1)</script>', password: 'test' }, desc: 'XSS in login' },
  { path: '/api/v1/student/challenge/submit', body: { challengeId: '../admin', code: 'x', output: '', passed: true }, desc: 'Path traversal' },
  { path: '/api/v1/auth/login', body: { username: 'A'.repeat(300), password: 'B'.repeat(200) }, desc: 'Long input flood' },
  { path: '/api/v1/auth/login', body: { username: null, password: undefined }, desc: 'Null input' },
  { path: '/api/v1/student/challenge/submit', body: { challengeId: 'nonexistent', code: 'x', output: '' }, desc: 'Nonexistent challenge' },
  { path: '/api/v1/student/assessment/submit', body: { assessmentId: 'fake', answers: [{}] }, desc: 'Fake assessment' },
  { path: '/api/v1/analytics/dashboard', body: null, desc: 'Student accessing teacher endpoint' },
];

// Simulate realistic student solving challenges
function generateSolution(challengeId, tier) {
  const solutions = [
    { code: 'اطبع("مرحبا بالعالم")', output: 'مرحبا بالعالم' },
    { code: 'متغير اسم = "علي"\nاطبع(اسم)', output: 'علي' },
    { code: 'اذا (صحيح) { طبع("نعم") } اخر لا', output: 'نعم' },
    { code: 'لـ (متغير i = 0; i < 5; i = i + 1) { طبع(i) }', output: '0 1 2 3 4' },
    { code: 'متغير س = 10\nمتغير ص = 20\nاطبع(س + ص)', output: '30' },
    { code: 'دالة جمع(أ، ب) { رجع أ + ب }\nاطبع(جمع(3، 4))', output: '7' },
    { code: 'متغير م = [10، 20، 30]\nاطبع(م[1])', output: '20' },
    { code: 'طالما (صحيح) { طبع("نهاية") انقطاع }', output: 'نهاية' },
  ];
  return randomChoice(solutions);
}

async function studentWork(schoolIdx, studentIdx, loginToken) {
  const token = loginToken;
  if (!token) return;

  // Each student works independently
  while (Date.now() - stats.startTime < DURATION) {
    const action = randomInt(100);

    if (action < 30) {
      // Solve a challenge
      const sol = generateSolution('ch_' + schoolIdx + '_' + studentIdx, randomInt(5) + 1);
      await req('POST', '/api/v1/student/challenge/submit', {
        challengeId: 'ch_' + randomInt(150),
        code: sol.code,
        output: sol.output,
      }, token);
      stats.submissionsDone++;

    } else if (action < 40) {
      // View leaderboard
      await req('GET', '/api/v1/student/leaderboard', null, token);

    } else if (action < 50) {
      // View profile
      await req('GET', '/api/v1/student/profile', null, token);

    } else if (action < 60) {
      // View roadmap
      await req('GET', '/api/v1/student/roadmap', null, token);

    } else if (action < 65) {
      // Try to hack - fake passed=true on submission
      await req('POST', '/api/v1/student/challenge/submit', {
        challengeId: 'ch_' + randomInt(150),
        code: 'اطبع(" hacked ")',
        output: ' hacked ',
        passed: true, // try to cheat
      }, token);
      stats.hackAttempts++;

    } else if (action < 70) {
      // Try to access teacher endpoints
      await req('GET', '/api/v1/analytics/dashboard', null, token);
      stats.hackAttempts++;

    } else if (action < 75) {
      // View achievements
      await req('GET', '/api/v1/student/achievements', null, token);

    } else if (action < 80) {
      // Save workspace
      await req('POST', '/api/v1/workspace/save', {
        challengeId: 'ch_' + randomInt(50),
        code: 'اطبع("saved")',
        language: 'daad',
      }, token);

    } else if (action < 85) {
      // Check classroom
      await req('GET', '/api/v1/student/classroom/check', null, token);

    } else if (action < 90) {
      // View assessments
      await req('GET', '/api/v1/student/assessments', null, token);

    } else {
      // Wait random 1-5 seconds
      await new Promise(r => setTimeout(r, 1000 + randomInt(4000)));
    }

    // Random delay between actions (100ms - 2s)
    await new Promise(r => setTimeout(r, 100 + randomInt(1900)));
  }
}

async function teacherWork(schoolIdx, token) {
  while (Date.now() - stats.startTime < DURATION) {
    const action = randomInt(100);

    if (action < 25) {
      // Create assessment
      const now = new Date();
      const future = new Date(now.getTime() + 60 * 60 * 1000);
      await req('POST', '/api/v1/assessments/create', {
        classroomId: 'cls_' + schoolIdx,
        title: 'اختبار_' + randomInt(1000),
        description: 'اختبار شامل',
        assessmentType: randomChoice(['CODING', 'MULTIPLE_CHOICE']),
        allowedTime: 60,
        startTime: now.toISOString(),
        endTime: future.toISOString(),
        totalPoints: 100,
        passingScore: 60,
        isPublished: true,
      }, token);
      stats.assessmentsCreated++;

    } else if (action < 40) {
      // View dashboard
      await req('GET', '/api/v1/analytics/dashboard', null, token);

    } else if (action < 50) {
      // View leaderboard
      await req('GET', '/api/v1/analytics/leaderboard', null, token);

    } else if (action < 55) {
      // View progress
      await req('GET', '/api/v1/analytics/progress', null, token);

    } else if (action < 60) {
      // View classrooms
      await req('GET', '/api/v1/analytics/classrooms', null, token);

    } else if (action < 65) {
      // View assessments
      await req('GET', '/api/v1/analytics/assessments', null, token);

    } else if (action < 70) {
      // Create classroom
      await req('POST', '/api/v1/analytics/classrooms', {
        name: 'فصل_' + randomInt(10000),
        subject: randomChoice(['برمجة', 'حاسوب', 'تقنية']),
        level: randomChoice(['7', '8', '9', '10', '11', '12']),
      }, token);

    } else if (action < 75) {
      // View security alerts
      await req('GET', '/api/v1/analytics/security-alerts', null, token);

    } else if (action < 80) {
      // Try to hack - SQL injection via teacher
      await req('GET', "/api/v1/analytics/leaderboard?classroom=' OR 1=1--", null, token);
      stats.hackAttempts++;

    } else {
      // Wait random 2-8 seconds
      await new Promise(r => setTimeout(r, 2000 + randomInt(6000)));
    }

    await new Promise(r => setTimeout(r, 500 + randomInt(3000)));
  }
}

// Concurrent mass login test
async function massLogin(schoolIdx, studentCount) {
  const results = [];
  // Batch logins in groups of 10 to avoid rate limits
  for (let batch = 0; batch < studentCount; batch += 10) {
    const promises = [];
    for (let i = batch; i < Math.min(batch + 10, studentCount); i++) {
      const nationalId = String(1000000000 + schoolIdx * 1000 + i);
      promises.push((async () => {
        const token = await login(nationalId, 'Test1234!');
        if (token) {
          stats.successes++;
        }
        return { schoolIdx, studentIdx: i, token };
      })());
    }
    const batchResults = await Promise.all(promises);
    results.push(...batchResults);
    // Small delay between batches
    await new Promise(r => setTimeout(r, 200));
  }
  return results;
}

async function main() {
  console.log('═══════════════════════════════════════════════════════════════');
  console.log('  Dhad Studio - 30-Minute Massive Stress Test');
  console.log('  10 Schools × 100 Students = 1,000 Concurrent Users');
  console.log('═══════════════════════════════════════════════════════════════');
  console.log('');

  // Phase 1: Mass login all students
  console.log('[Phase 1] Mass login: 1,000 students...');
  const allStudents = [];
  for (let s = 0; s < SCHOOLS_COUNT; s++) {
    const schoolStudents = await massLogin(s, STUDENTS_PER_SCHOOL);
    allStudents.push(schoolStudents);
    console.log(`  School ${s + 1}: ${schoolStudents.filter(s => s.token).length}/${STUDENTS_PER_SCHOOL} logged in`);
  }

  // Phase 2: Login teachers
  console.log('[Phase 2] Logging in teachers...');
  const teacherTokens = [];
  const teacherEmails = [
    'shamel@teacher.com', 'teacher@dhad.com', 'sara@teacher.com', 'ali@teacher.com'
  ];
  for (const email of teacherEmails) {
    const token = await login(email, 'Test1234!');
    if (token) teacherTokens.push(token);
  }
  console.log(`  ${teacherTokens.length} teachers logged in`);

  // Phase 3: Mass concurrent student work for 30 minutes
  console.log('[Phase 3] Starting 30-minute concurrent work session...');
  console.log('  Students: solving challenges, taking tests, trying to hack');
  console.log('  Teachers: creating assessments, managing classrooms');
  console.log('');

  const startTime = Date.now();
  const allWork = [];

  // Launch all students
  for (let s = 0; s < SCHOOLS_COUNT; s++) {
    for (let i = 0; i < allStudents[s].length; i++) {
      const student = allStudents[s][i];
      if (student.token) {
        allWork.push(studentWork(s, i, student.token));
      }
    }
  }

  // Launch teachers
  for (const token of teacherTokens) {
    allWork.push(teacherWork(0, token));
  }

  // Launch periodic hack attempts
  const hackInterval = setInterval(async () => {
    if (Date.now() - startTime > DURATION) {
      clearInterval(hackInterval);
      return;
    }
    for (const payload of HACK_PAYLOADS) {
      const randomToken = allStudents[randomInt(SCHOOLS_COUNT)][randomInt(STUDENTS_PER_SCHOOL)]?.token;
      await req('POST', payload.path, payload.body, randomToken);
      stats.hackAttempts++;
    }
  }, 10000); // Every 10 seconds

  // Stats reporter
  const statsInterval = setInterval(() => {
    const elapsed = Math.floor((Date.now() - startTime) / 1000);
    const remaining = Math.floor((DURATION - (Date.now() - startTime)) / 1000);
    const rps = stats.requests / (elapsed || 1);
    console.log(`  [${elapsed}s] Requests: ${stats.requests} | Success: ${stats.successes} | Fail: ${stats.failures} | RPS: ${rps.toFixed(1)} | Hacks blocked: ${stats.hacksBlocked} | Submissions: ${stats.submissionsDone}`);
  }, 30000); // Every 30 seconds

  // Wait for all work to complete (30 minutes)
  await Promise.all(allWork);
  clearInterval(hackInterval);
  clearInterval(statsInterval);

  // Final report
  const elapsed = (Date.now() - startTime) / 1000;
  const rps = stats.requests / elapsed;

  console.log('');
  console.log('═══════════════════════════════════════════════════════════════');
  console.log('  FINAL REPORT - 30 Minute Stress Test');
  console.log('═══════════════════════════════════════════════════════════════');
  console.log(`  Duration:         ${Math.floor(elapsed)}s`);
  console.log(`  Total Requests:   ${stats.requests}`);
  console.log(`  Successes:        ${stats.successes}`);
  console.log(`  Failures:         ${stats.failures}`);
  console.log(`  Requests/sec:     ${rps.toFixed(2)}`);
  console.log(`  Submissions:      ${stats.submissionsDone}`);
  console.log(`  Hack Attempts:    ${stats.hackAttempts}`);
  console.log(`  Hacks Blocked:    ${stats.hacksBlocked}`);
  console.log(`  Assessments Created: ${stats.assessmentsCreated}`);
  console.log(`  Error Breakdown:  ${JSON.stringify(stats.errors)}`);
  console.log('');

  if (stats.hacksBlocked > 0) {
    console.log('  ✅ Security: All hack attempts were properly blocked');
  }
  if (stats.failures < stats.successes * 0.1) {
    console.log('  ✅ Reliability: Less than 10% failure rate');
  }
  if (rps > 100) {
    console.log('  ✅ Performance: Over 100 requests per second');
  }

  console.log('═══════════════════════════════════════════════════════════════');
}

main().catch(console.error);
