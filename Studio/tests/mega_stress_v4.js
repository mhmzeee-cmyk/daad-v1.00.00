// ═══════════════════════════════════════════════════════════════════════════════
// DHAD STUDIO - MEGA STRESS TEST v4
// 100,000 Students + 1,000 Teachers + 1,000 Assessments
// 3 Hours Continuous | Comprehensive Report
// ═══════════════════════════════════════════════════════════════════════════════

const http = require('http');
const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');
const os = require('os');
const fs = require('fs');

const BASE = { hostname: 'localhost', port: 3000 };
const DURATION_MS = 3 * 60 * 60 * 1000; // 3 hours
const WORKER_COUNT = Math.min(os.cpus().length * 2, 16);
const TOTAL_STUDENTS = 500;   // Reduced for SQLite (bcrypt is CPU-bound ~750ms each)
const TOTAL_TEACHERS = 100;   // We have 100 teachers

// ═══════════════════════════════════════════════════════════════════════════════
// │   METRICS COLLECTOR                                                         │
// ═══════════════════════════════════════════════════════════════════════════════

const metrics = {
  startTime: Date.now(),
  totalRequests: 0,
  successfulRequests: 0,
  failedRequests: 0,
  byStatus: {},
  byEndpoint: {},
  responseTimes: [],
  errors: [],
  logins: { success: 0, failed: 0 },
  challenges: { submitted: 0, passed: 0, blocked: 0 },
  assessments: { created: 0, submitted: 0 },
  attacks: { attempted: 0, blocked: 0 },
  teacherActions: 0,
  studentActions: 0,
  maxResponseTime: 0,
  maxResponseTimeRequest: null,
  p50: 0, p90: 0, p95: 0, p99: 0,
};

function recordRequest(endpoint, status, responseTime, detail) {
  metrics.totalRequests++;
  if (status >= 200 && status < 400) metrics.successfulRequests++;
  else metrics.failedRequests++;

  metrics.byStatus[status] = (metrics.byStatus[status] || 0) + 1;

  const key = endpoint.split('?')[0];
  if (!metrics.byEndpoint[key]) metrics.byEndpoint[key] = { count: 0, errors: 0, totalTime: 0, maxTime: 0 };
  metrics.byEndpoint[key].count++;
  metrics.byEndpoint[key].totalTime += responseTime;
  if (responseTime > metrics.byEndpoint[key].maxTime) metrics.byEndpoint[key].maxTime = responseTime;

  metrics.responseTimes.push(responseTime);
  if (responseTime > metrics.maxResponseTime) {
    metrics.maxResponseTime = responseTime;
    metrics.maxResponseTimeRequest = detail || endpoint;
  }
}

function calculatePercentiles() {
  if (metrics.responseTimes.length === 0) return;
  const sorted = [...metrics.responseTimes].sort((a, b) => a - b);
  const len = sorted.length;
  metrics.p50 = sorted[Math.floor(len * 0.5)];
  metrics.p90 = sorted[Math.floor(len * 0.9)];
  metrics.p95 = sorted[Math.floor(len * 0.95)];
  metrics.p99 = sorted[Math.floor(len * 0.99)];
}

// ═══════════════════════════════════════════════════════════════════════════════
// │   HTTP HELPER                                                               │
// ═══════════════════════════════════════════════════════════════════════════════

function req(method, path, body, token, extraHeaders) {
  return new Promise((resolve) => {
    const start = Date.now();
    const data = body ? (typeof body === 'string' ? body : JSON.stringify(body)) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'MegaStress/4.0', ...extraHeaders };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, ...BASE, path: encodeURI(path), headers, timeout: 30000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => {
        const elapsed = Date.now() - start;
        try {
          const d = JSON.parse(buf);
          resolve({ s: res.statusCode, d, h: res.headers, t: elapsed });
        } catch (e) {
          resolve({ s: res.statusCode, raw: buf.substring(0, 200), h: res.headers, t: elapsed });
        }
      });
    });
    r.on('error', (e) => resolve({ s: 0, error: e.message, t: Date.now() - start }));
    r.on('timeout', () => { r.destroy(); resolve({ s: 0, error: 'timeout', t: Date.now() - start }); });
    if (data) r.write(data);
    r.end();
  });
}

function delay(ms) { return new Promise(r => setTimeout(r, ms)); }

// ═══════════════════════════════════════════════════════════════════════════════
// │   Dhad CODE SNIPPETS (Real solutions)                                       │
// ═══════════════════════════════════════════════════════════════════════════════

const CODE_SNIPPETS = [
  { code: 'print(5 + 3)', output: '8' },
  { code: 'print(10 * 2)', output: '20' },
  { code: 'print(100 / 4)', output: '25' },
  { code: 'print(7 - 2)', output: '5' },
  { code: 'print(2 ** 8)', output: '256' },
  { code: 'print(15 % 4)', output: '3' },
  { code: 'print(50 + 50)', output: '100' },
  { code: 'print(999 - 1)', output: '998' },
  { code: 'print(3 * 7)', output: '21' },
  { code: 'print(144 / 12)', output: '12' },
  { code: 'print(25 * 4)', output: '100' },
  { code: 'print(1000 / 10)', output: '100' },
  { code: 'print(11 + 22)', output: '33' },
  { code: 'print(56 - 28)', output: '28' },
  { code: 'print(9 * 9)', output: '81' },
  { code: 'print(64 / 8)', output: '8' },
  { code: 'print(17 + 13)', output: '30' },
  { code: 'print(200 - 75)', output: '125' },
  { code: 'print(6 * 8)', output: '48' },
  { code: 'print(45 / 5)', output: '9' },
];

const CHEAT_CODES = [
  'print("hacked")',
  '// empty\nprint(0)',
  'print(1)',
  'print("")',
  'while(true){}',
  'print(eval("1+1"))',
];

const ATTACK_PAYLOADS = [
  { body: { challengeId: 'x', code: 'print(1)', output: '1' } },
  { body: { challengeId: "'; DROP TABLE--", code: 'print(1)', output: '1' } },
  { body: { challengeId: '__proto__', code: 'print(1)', output: '1' } },
  { body: { challengeId: ['a','b'], code: ['c','d'], output: ['e','f'] } },
  { body: { code: '{{7*7}}', output: '49' } },
  { body: { challengeId: null, code: undefined, output: NaN } },
];

const ATTACK_JWTS = [
  'eyJhbGciOiJub25lIn0.eyJpZCI6ImFkbWluIiwicm9sZSI6IkFETUlOIn0.',
  'fake.token.here',
  'eyJhbGciOiJIUzI1NiJ9.eyJpZCI6IjEwMDAwMDAwMDAiIn0.wrong',
];

// ═══════════════════════════════════════════════════════════════════════════════
// │   MAIN TEST ORCHESTRATOR                                                    │
// ═══════════════════════════════════════════════════════════════════════════════

async function runPhase(name, fn, durationMs) {
  console.log(`\n  ▶ ${name} (${Math.round(durationMs/1000)}s)`);
  const end = Date.now() + durationMs;
  let iterations = 0;
  while (Date.now() < end) {
    await fn(iterations++);
    if (iterations % 100 === 0) {
      const elapsed = Math.round((Date.now() - (end - durationMs)) / 1000);
      const remaining = Math.round((end - Date.now()) / 1000);
      process.stdout.write(`\r    [${elapsed}s/${Math.round(durationMs/1000)}s] requests: ${metrics.totalRequests} ok: ${metrics.successfulRequests} err: ${metrics.failedRequests}`);
    }
  }
  console.log(`\n    ✓ Done (${iterations} iterations)`);
}

// ── Login Batch ──────────────────────────────────────────────────────────────
async function loginBatch(userIds, role) {
  const results = [];
  const batchSize = 10;
  for (let i = 0; i < userIds.length; i += batchSize) {
    const batch = userIds.slice(i, i + batchSize);
    const batchResults = await Promise.all(batch.map(id => {
      const username = role === 'TEACHER' ? `teacher_${id}@test.com` : String(id);
      return req('POST', '/api/v1/auth/login', { username, password: 'Test1234!' });
    }));
    batchResults.forEach((r, idx) => {
      const endpoint = '/api/v1/auth/login';
      recordRequest(endpoint, r.s, r.t, `login_${role}_${batch[idx]}`);
      if (r.d?.accessToken) {
        metrics.logins.success++;
        results.push({ id: batch[idx], token: r.d.accessToken, role });
      } else {
        metrics.logins.failed++;
      }
    });
  }
  return results;
}

// ═══════════════════════════════════════════════════════════════════════════════
// │   WORKER THREADS                                                            │
// ═══════════════════════════════════════════════════════════════════════════════

if (isMainThread) {
  // ═══════════════════════════════════════════════════════════════════════════
  // │   MAIN THREAD                                                             │
  // ═══════════════════════════════════════════════════════════════════════════

  const REPORT_FILE = `tests/mega_report_${Date.now()}.json`;

  async function main() {
    console.log('═══════════════════════════════════════════════════════════════');
    console.log('  DHAD STUDIO - MEGA STRESS TEST v4');
    console.log(`  ${TOTAL_STUDENTS.toLocaleString()} Students + ${TOTAL_TEACHERS} Teachers + 1,000 Assessments`);
    console.log('  Duration: 3 Hours Continuous');
    console.log('═══════════════════════════════════════════════════════════════');
    console.log(`  Workers: ${WORKER_COUNT}`);
    console.log(`  Started: ${new Date().toISOString()}`);
    console.log('═══════════════════════════════════════════════════════════════');

    // ── Health Check ─────────────────────────────────────────────────────
    console.log('\n  [PHASE 0] Health check...');
    const health = await req('GET', '/api/v1/student/profile', null, 'invalid');
    if (health.s === 0) {
      console.log('  ❌ Server not running! Start it first.');
      process.exit(1);
    }
    console.log('  ✓ Server is running (status: ' + health.s + ')');

    // ═════════════════════════════════════════════════════════════════════
    // PHASE 1: MASS LOGIN (10 min)
    // ═════════════════════════════════════════════════════════════════════
    console.log('\n═══════════════════════════════════════════════════════════════');
    console.log('  PHASE 1: MASS LOGIN');
    console.log('═══════════════════════════════════════════════════════════════');

    // Login students with concurrency limiter (max 3 concurrent) to avoid ECONNRESET
    console.log(`\n  [1.1] Logging in ${TOTAL_STUDENTS.toLocaleString()} students (max 5 concurrent)...`);
    const studentTokens = [];
    const STUDENT_BATCH = 5;
    const totalStudentBatches = Math.ceil(TOTAL_STUDENTS / STUDENT_BATCH);
    const studentStart = Date.now();

    for (let batch = 0; batch < totalStudentBatches; batch++) {
      const startId = batch * STUDENT_BATCH + 1000000000;
      const ids = [];
      for (let i = 0; i < STUDENT_BATCH; i++) ids.push(startId + i);

      const batchResults = await Promise.all(ids.map(id =>
        req('POST', '/api/v1/auth/login', { username: String(id), password: 'Test1234!' })
      ));

      batchResults.forEach((r, idx) => {
        recordRequest('/api/v1/auth/login', r.s, r.t, 'student_login');
        if (r.d?.accessToken) {
          metrics.logins.success++;
          studentTokens.push({ id: ids[idx], token: r.d.accessToken });
        } else {
          metrics.logins.failed++;
        }
      });

      await delay(50);

      if (batch % 200 === 0) {
        const pct = Math.round((batch / totalStudentBatches) * 100);
        const elapsed = Math.round((Date.now() - studentStart) / 1000);
        const eta = Math.round((elapsed / Math.max(batch, 1)) * (totalStudentBatches - batch));
        process.stdout.write(`\r    [${pct}%] ${batch * STUDENT_BATCH}/${TOTAL_STUDENTS.toLocaleString()} logged in | ${elapsed}s | tokens: ${studentTokens.length} | ETA: ${eta}s`);
      }
    }
    console.log(`\n  ✓ Students logged in: ${studentTokens.length}/${TOTAL_STUDENTS} (${Math.round((Date.now() - studentStart) / 1000)}s)`);

    // Login teachers
    console.log(`\n  [1.2] Logging in ${TOTAL_TEACHERS} teachers...`);
    const teacherTokens = [];
    const TEACHER_BATCH = 10;
    const totalTeacherBatches = Math.ceil(TOTAL_TEACHERS / TEACHER_BATCH);

    for (let batch = 0; batch < totalTeacherBatches; batch++) {
      const startIdx = batch * TEACHER_BATCH;
      const promises = [];
      for (let i = 0; i < TEACHER_BATCH && (startIdx + i) < TOTAL_TEACHERS; i++) {
        const tNum = startIdx + i + 1;
        const schoolNum = Math.ceil(tNum / 10);
        const idx = ((tNum - 1) % 10) + 1;
        const username = `teacher_${schoolNum}_${idx}@test.com`;
        promises.push(req('POST', '/api/v1/auth/login', { username, password: 'Test1234!' }));
      }
      const results = await Promise.all(promises);
      results.forEach((r, idx) => {
        recordRequest('/api/v1/auth/login', r.s, r.t, 'teacher_login');
        if (r.d?.accessToken) {
          metrics.logins.success++;
          teacherTokens.push({ id: startIdx + idx, token: r.d.accessToken, schoolId: r.d.profile?.school?.id });
        } else {
          metrics.logins.failed++;
        }
      });
    }
    console.log(`  ✓ Teachers logged in: ${teacherTokens.length}`);

    // ═════════════════════════════════════════════════════════════════════
    // PHASE 2: CHALLENGE SOLVING + ASSESSMENTS (2h 40min)
    // ═════════════════════════════════════════════════════════════════════
    console.log('\n═══════════════════════════════════════════════════════════════');
    console.log('  PHASE 2: CHALLENGE SOLVING + ASSESSMENTS + ATTACKS');
    console.log('═══════════════════════════════════════════════════════════════');

    // Get available challenges
    const chalResp = await req('GET', '/api/v1/challenges', null, studentTokens[0]?.token);
    const challenges = chalResp.d?.challenges || [];
    console.log(`  Available challenges: ${challenges.length}`);

    if (challenges.length === 0) {
      console.log('  ❌ No challenges found!');
      process.exit(1);
    }

    // Get classrooms for teachers
    const classResp = await req('GET', '/api/v1/analytics/classrooms', null, teacherTokens[0]?.token);
    const classrooms = classResp.d?.classrooms || [];
    console.log(`  Available classrooms: ${classrooms.length}`);

    // ═════════════════════════════════════════════════════════════════════
    // WORKER DISTRIBUTION
    // ═════════════════════════════════════════════════════════════════════
    // Divide users among workers
    const studentsPerWorker = Math.ceil(studentTokens.length / WORKER_COUNT);
    const teachersPerWorker = Math.ceil(teacherTokens.length / WORKER_COUNT);

    const workers = [];
    const workerResults = [];

    for (let w = 0; w < WORKER_COUNT; w++) {
      const studentSlice = studentTokens.slice(w * studentsPerWorker, (w + 1) * studentsPerWorker);
      const teacherSlice = teacherTokens.slice(w * teachersPerWorker, (w + 1) * teachersPerWorker);

      const workerPromise = new Promise((resolve, reject) => {
        const worker = new Worker(__filename, {
          workerData: {
            workerId: w,
            students: studentSlice,
            teachers: teacherSlice,
            challenges: challenges.map(c => ({ id: c.id, tier: c.tier, expectedOutput: c.expectedOutput })),
            classrooms: classrooms.map(c => c.id),
            durationMs: DURATION_MS - (Date.now() - metrics.startTime) - 30000, // leave 30s for report
          }
        });
        worker.on('message', (msg) => {
          if (msg.type === 'metrics') {
            // Merge worker metrics
            msg.data.requests.forEach(r => metrics.totalRequests++);
            metrics.logins.success += msg.data.logins.success;
            metrics.logins.failed += msg.data.logins.failed;
            Object.entries(msg.data.byStatus || {}).forEach(([k, v]) => {
              metrics.byStatus[k] = (metrics.byStatus[k] || 0) + v;
              const code = parseInt(k);
              if (code >= 200 && code < 400) metrics.successfulRequests += v;
              else metrics.failedRequests += v;
            });
            metrics.logins.failed += msg.data.logins.failed;
            metrics.challenges.submitted += msg.data.challenges.submitted;
            metrics.challenges.passed += msg.data.challenges.passed;
            metrics.challenges.blocked += msg.data.challenges.blocked;
            metrics.attacks.attempted += msg.data.attacks.attempted;
            metrics.attacks.blocked += msg.data.attacks.blocked;
            metrics.assessments.created += msg.data.assessments.created;
            metrics.assessments.submitted += msg.data.assessments.submitted;
            metrics.teacherActions += msg.data.teacherActions;
            metrics.studentActions += msg.data.studentActions;
            metrics.errors.push(...msg.data.errors);
            Object.entries(msg.data.byEndpoint || {}).forEach(([k, v]) => {
              if (!metrics.byEndpoint[k]) metrics.byEndpoint[k] = { count: 0, errors: 0, totalTime: 0, maxTime: 0 };
              metrics.byEndpoint[k].count += v.count;
              metrics.byEndpoint[k].errors += v.errors;
              metrics.byEndpoint[k].totalTime += v.totalTime;
              if (v.maxTime > metrics.byEndpoint[k].maxTime) metrics.byEndpoint[k].maxTime = v.maxTime;
            });
            if (msg.data.maxResponseTime > metrics.maxResponseTime) {
              metrics.maxResponseTime = msg.data.maxResponseTime;
              metrics.maxResponseTimeRequest = msg.data.maxResponseTimeRequest;
            }
            metrics.responseTimes.push(...(msg.data.sampleTimes || []));
          }
        });
        worker.on('error', reject);
        worker.on('exit', (code) => {
          if (code !== 0) console.log(`  ⚠️  Worker ${w} exited with code ${code}`);
          resolve();
        });
        workers.push(worker);
      });
      workerResults.push(workerPromise);
    }

    console.log(`  Launched ${WORKER_COUNT} workers`);
    console.log(`  Students per worker: ${studentsPerWorker}`);
    console.log(`  Teachers per worker: ${teachersPerWorker}`);
    console.log(`\n  ⏳ Running for 3 hours... (started ${new Date().toISOString()})`);

    // Progress reporter
    const progressInterval = setInterval(() => {
      const elapsed = Math.round((Date.now() - metrics.startTime) / 60000);
      const remaining = Math.round((DURATION_MS - (Date.now() - metrics.startTime)) / 60000);
      calculatePercentiles();
      console.log(`\n  [${elapsed}m/${180}] Total: ${metrics.totalRequests} | OK: ${metrics.successfulRequests} | Err: ${metrics.failedRequests} | P95: ${metrics.p95}ms | Max: ${metrics.maxResponseTime}ms`);
    }, 60000); // every minute

    // Wait for all workers
    await Promise.all(workerResults);
    clearInterval(progressInterval);

    // ═════════════════════════════════════════════════════════════════════
    // PHASE 3: GENERATE REPORT
    // ═════════════════════════════════════════════════════════════════════
    console.log('\n═══════════════════════════════════════════════════════════════');
    console.log('  PHASE 3: GENERATING REPORT');
    console.log('═══════════════════════════════════════════════════════════════');

    calculatePercentiles();
    const totalDuration = Math.round((Date.now() - metrics.startTime) / 1000);

    const report = {
      title: 'Dhad Studio Mega Stress Test Report',
      version: 'v4',
      generatedAt: new Date().toISOString(),
      duration: `${Math.floor(totalDuration / 3600)}h ${Math.floor((totalDuration % 3600) / 60)}m ${totalDuration % 60}s`,
      durationSeconds: totalDuration,
      config: {
        targetStudents: TOTAL_STUDENTS,
        targetTeachers: TOTAL_TEACHERS,
        targetAssessments: 1000,
        workerThreads: WORKER_COUNT,
      },
      summary: {
        totalRequests: metrics.totalRequests,
        successfulRequests: metrics.successfulRequests,
        failedRequests: metrics.failedRequests,
        requestsPerSecond: Math.round(metrics.totalRequests / totalDuration),
        successRate: metrics.totalRequests > 0 ? Math.round((metrics.successfulRequests / metrics.totalRequests) * 100) + '%' : 'N/A',
      },
      loginStats: {
        successful: metrics.logins.success,
        failed: metrics.logins.failed,
      },
      challengeStats: {
        submitted: metrics.challenges.submitted,
        passed: metrics.challenges.passed,
        blocked: metrics.challenges.blocked,
        passRate: metrics.challenges.submitted > 0 ? Math.round((metrics.challenges.passed / metrics.challenges.submitted) * 100) + '%' : 'N/A',
      },
      assessmentStats: {
        created: metrics.assessments.created,
        submitted: metrics.assessments.submitted,
      },
      attackStats: {
        attempted: metrics.attacks.attempted,
        blocked: metrics.attacks.blocked,
        blockRate: metrics.attacks.attempted > 0 ? Math.round((metrics.attacks.blocked / metrics.attacks.attempted) * 100) + '%' : 'N/A',
      },
      responseTime: {
        max: metrics.maxResponseTime,
        maxRequest: metrics.maxResponseTimeRequest,
        p50: metrics.p50,
        p90: metrics.p90,
        p95: metrics.p95,
        p99: metrics.p99,
        average: metrics.responseTimes.length > 0 ? Math.round(metrics.responseTimes.reduce((a, b) => a + b, 0) / metrics.responseTimes.length) : 0,
        samples: metrics.responseTimes.length,
      },
      statusBreakdown: metrics.byStatus,
      endpointStats: Object.entries(metrics.byEndpoint).map(([path, data]) => ({
        path,
        requests: data.count,
        errors: data.errors,
        avgResponseTime: data.count > 0 ? Math.round(data.totalTime / data.count) : 0,
        maxResponseTime: data.maxTime,
      })).sort((a, b) => b.requests - a.requests),
      topErrors: metrics.errors.sort((a, b) => (b.count || 1) - (a.count || 1)).slice(0, 50),
    };

    fs.writeFileSync(REPORT_FILE, JSON.stringify(report, null, 2));
    console.log(`  ✓ Report saved to ${REPORT_FILE}`);

    // Print summary
    console.log('\n═══════════════════════════════════════════════════════════════');
    console.log('  FINAL REPORT');
    console.log('═══════════════════════════════════════════════════════════════');
    console.log(`  Duration:          ${report.duration}`);
    console.log(`  Total Requests:    ${report.summary.totalRequests.toLocaleString()}`);
    console.log(`  Successful:        ${report.summary.successfulRequests.toLocaleString()}`);
    console.log(`  Failed:            ${report.summary.failedRequests.toLocaleString()}`);
    console.log(`  Requests/sec:      ${report.summary.requestsPerSecond}`);
    console.log(`  Success Rate:      ${report.summary.successRate}`);
    console.log(`  Logins OK/Failed:  ${metrics.logins.success}/${metrics.logins.failed}`);
    console.log(`  Challenges:        ${metrics.challenges.submitted} submitted, ${metrics.challenges.passed} passed, ${metrics.challenges.blocked} blocked`);
    console.log(`  Assessments:       ${metrics.assessments.created} created, ${metrics.assessments.submitted} submitted`);
    console.log(`  Attacks:           ${metrics.attacks.attempted} attempted, ${metrics.attacks.blocked} blocked`);
    console.log(`  Teacher Actions:   ${metrics.teacherActions}`);
    console.log(`  Student Actions:   ${metrics.studentActions}`);
    console.log(`  Response Times:`);
    console.log(`    P50:  ${metrics.p50}ms`);
    console.log(`    P90:  ${metrics.p90}ms`);
    console.log(`    P95:  ${metrics.p95}ms`);
    console.log(`    P99:  ${metrics.p99}ms`);
    console.log(`    Max:  ${metrics.maxResponseTime}ms (${metrics.maxResponseTimeRequest || 'N/A'})`);
    console.log(`    Avg:  ${report.responseTime.average}ms`);
    console.log('═══════════════════════════════════════════════════════════════');
  }

  main().catch(err => { console.error('Fatal:', err); process.exit(1); });

} else {
  // ═══════════════════════════════════════════════════════════════════════════
  // │   WORKER THREAD                                                          │
  // ═══════════════════════════════════════════════════════════════════════════

  const { workerId, students, teachers, challenges, classrooms, durationMs } = workerData;
  const wMetrics = {
    requests: [],
    logins: { success: 0, failed: 0 },
    challenges: { submitted: 0, passed: 0, blocked: 0 },
    assessments: { created: 0, submitted: 0 },
    attacks: { attempted: 0, blocked: 0 },
    teacherActions: 0,
    studentActions: 0,
    errors: [],
    byStatus: {},
    byEndpoint: {},
    maxResponseTime: 0,
    maxResponseTimeRequest: null,
    sampleTimes: [],
  };

  function wReq(method, path, body, token) {
    return new Promise((resolve) => {
      const start = Date.now();
      const data = body ? (typeof body === 'string' ? body : JSON.stringify(body)) : null;
      const headers = { 'Content-Type': 'application/json', 'User-Agent': `Worker-${workerId}` };
      if (token) headers['Authorization'] = 'Bearer ' + token;
      const r = http.request({ method, ...BASE, path: encodeURI(path), headers, timeout: 30000 }, (res) => {
        let buf = '';
        res.on('data', c => buf += c);
        res.on('end', () => {
          const elapsed = Date.now() - start;
          try { resolve({ s: res.statusCode, d: JSON.parse(buf), t: elapsed }); }
          catch (e) { resolve({ s: res.statusCode, raw: buf.substring(0, 100), t: elapsed }); }
        });
      });
      r.on('error', (e) => resolve({ s: 0, error: e.message, t: Date.now() - start }));
      r.on('timeout', () => { r.destroy(); resolve({ s: 0, error: 'timeout', t: Date.now() - start }); });
      if (data) r.write(data);
      r.end();
    });
  }

  function record(method, path, status, time, detail) {
    wMetrics.requests.push({ path, status, time });
    wMetrics.byStatus[status] = (wMetrics.byStatus[status] || 0) + 1;
    const key = path.split('?')[0];
    if (!wMetrics.byEndpoint[key]) wMetrics.byEndpoint[key] = { count: 0, errors: 0, totalTime: 0, maxTime: 0 };
    wMetrics.byEndpoint[key].count++;
    wMetrics.byEndpoint[key].totalTime += time;
    if (time > wMetrics.byEndpoint[key].maxTime) wMetrics.byEndpoint[key].maxTime = time;
    if (time > wMetrics.maxResponseTime) {
      wMetrics.maxResponseTime = time;
      wMetrics.maxResponseTimeRequest = detail || path;
    }
    if (wMetrics.sampleTimes.length < 5000) wMetrics.sampleTimes.push(time);
  }

  function pickRandom(arr) { return arr[Math.floor(Math.random() * arr.length)]; }

  async function workerLoop() {
    const end = Date.now() + durationMs;
    let tick = 0;

    // ═══════════════════════════════════════════════════════════════════════
    // │   STUDENT BEHAVIOR PROFILES                                          │
    // ═══════════════════════════════════════════════════════════════════════
    // Each student has a "behavior" that determines their activity pattern:
    //   0: Normal solver (60%) - solves challenges sequentially
    //   1: Fast solver (20%) - rapid-fire submissions
    //   2: Cheater (10%) - tries to submit wrong/garbage code
    //   3: Attacker (5%) - sends malicious payloads
    //   4: Inactive (5%) - just logs in, does nothing

    while (Date.now() < end) {
      tick++;
      const phase = tick % 100;

      // ── STUDENT ACTIONS ──────────────────────────────────────────────────
      // Process 10 students per tick
      const studentBatch = students.slice((tick * 10) % Math.max(students.length, 1), ((tick * 10) + 10) % Math.max(students.length, 1));
      if (studentBatch.length === 0) continue;

      await Promise.all(studentBatch.map(async (student) => {
        const behavior = (student.id * 7 + tick) % 20; // Deterministic per student

        if (behavior < 12) {
          // ── Normal Solver ──
          const chIdx = Math.floor(Math.random() * Math.min(challenges.length, 150));
          const ch = challenges[chIdx];
          if (!ch) return;

          const snippet = pickRandom(CODE_SNIPPETS);
          const r = await wReq('POST', '/api/v1/student/challenge/submit', {
            challengeId: ch.id,
            code: snippet.code,
            output: snippet.output,
          }, student.token);
          record('POST', '/api/v1/student/challenge/submit', r.s, r.t, `student_${student.id}_solve`);
          wMetrics.studentActions++;

          if (r.s === 200 && r.d?.passed) wMetrics.challenges.passed++;
          else if (r.s === 200) wMetrics.challenges.submitted++;
          else if (r.s === 403) wMetrics.challenges.blocked++;
          else wMetrics.challenges.submitted++;

        } else if (behavior < 16) {
          // ── Fast Solver ──
          for (let i = 0; i < 3; i++) {
            const chIdx = Math.floor(Math.random() * Math.min(challenges.length, 50));
            const ch = challenges[chIdx];
            if (!ch) continue;
            const snippet = pickRandom(CODE_SNIPPETS);
            const r = await wReq('POST', '/api/v1/student/challenge/submit', {
              challengeId: ch.id, code: snippet.code, output: snippet.output,
            }, student.token);
            record('POST', '/api/v1/student/challenge/submit', r.s, r.t, `student_${student.id}_fast`);
            wMetrics.studentActions++;
            if (r.s === 200 && r.d?.passed) wMetrics.challenges.passed++;
            else wMetrics.challenges.submitted++;
          }

        } else if (behavior < 18) {
          // ── Cheater ──
          const ch = pickRandom(challenges.slice(0, 30));
          if (!ch) return;
          const cheatCode = pickRandom(CHEAT_CODES);
          const r = await wReq('POST', '/api/v1/student/challenge/submit', {
            challengeId: ch.id, code: cheatCode, output: '1',
          }, student.token);
          record('POST', '/api/v1/student/challenge/submit', r.s, r.t, `student_${student.id}_cheat`);
          wMetrics.studentActions++;
          wMetrics.challenges.submitted++;
          if (r.s === 200 || r.s === 403) wMetrics.challenges.blocked++;

        } else if (behavior < 19) {
          // ── Attacker ──
          const attack = pickRandom(ATTACK_PAYLOADS);
          const jwt = pickRandom(ATTACK_JWTS);
          const endpoints = [
            ['POST', '/api/v1/student/challenge/submit', attack.body],
            ['GET', '/api/v1/student/profile', null],
            ['POST', '/api/v1/auth/login', { username: 'admin', password: 'hack' }],
            ['GET', '/api/v1/challenges', null],
            ['GET', '/api/v1/analytics/dashboard', null],
          ];
          const [method, path, body] = pickRandom(endpoints);
          const r = await wReq(method, path, body, jwt);
          record(method, path, r.s, r.t, `attack_${workerId}_${tick}`);
          wMetrics.attacks.attempted++;
          if (r.s >= 400) wMetrics.attacks.blocked++;

        } else {
          // ── Inactive ──
          const r = await wReq('GET', '/api/v1/student/profile', null, student.token);
          record('GET', '/api/v1/student/profile', r.s, r.t, `student_${student.id}_idle`);
          wMetrics.studentActions++;
        }
      }));

      // ── TEACHER ACTIONS (every 10 ticks) ───────────────────────────────
      if (tick % 10 === 0 && teachers.length > 0) {
        const teacherBatch = teachers.slice(0, Math.min(teachers.length, 5));
        await Promise.all(teacherBatch.map(async (teacher) => {
          const action = tick % 30;
          if (action < 10) {
            // View dashboard
            const r = await wReq('GET', '/api/v1/analytics/dashboard', null, teacher.token);
            record('GET', '/api/v1/analytics/dashboard', r.s, r.t, `teacher_${teacher.id}_dash`);
            wMetrics.teacherActions++;
          } else if (action < 20) {
            // View leaderboard
            const r = await wReq('GET', '/api/v1/analytics/leaderboard?limit=50', null, teacher.token);
            record('GET', '/api/v1/analytics/leaderboard', r.s, r.t, `teacher_${teacher.id}_lb`);
            wMetrics.teacherActions++;
          } else {
            // View classrooms
            const r = await wReq('GET', '/api/v1/analytics/classrooms', null, teacher.token);
            record('GET', '/api/v1/analytics/classrooms', r.s, r.t, `teacher_${teacher.id}_cls`);
            wMetrics.teacherActions++;
          }
        }));
      }

      // ── ASSESSMENT CREATION (teacher, every 200 ticks) ─────────────────
      if (tick % 200 === 0 && teachers.length > 0 && classrooms.length > 0) {
        const teacher = pickRandom(teachers);
        const classId = pickRandom(classrooms);
        const now = new Date();
        const r = await wReq('POST', '/api/v1/assessments/create', {
          classroomId: classId,
          title: `Assessment_W${workerId}_T${tick}`,
          allowedTime: 60,
          startTime: now.toISOString(),
          endTime: new Date(now.getTime() + 3600000).toISOString(),
          challengeIds: challenges.slice(0, 5).map(c => c.id),
        }, teacher.token);
        record('POST', '/api/v1/assessments/create', r.s, r.t, `assessment_create`);
        if (r.s === 200 || r.s === 201) wMetrics.assessments.created++;
      }

      // ── PERIODIC REPORT BACK TO MAIN ──────────────────────────────────
      if (tick % 500 === 0) {
        parentPort.postMessage({ type: 'metrics', data: { ...wMetrics, sampleTimes: [] } });
        // Keep only last 1000 request samples
        if (wMetrics.requests.length > 1000) wMetrics.requests = wMetrics.requests.slice(-1000);
        if (wMetrics.sampleTimes.length > 5000) wMetrics.sampleTimes = [];
      }
    }

    // Final report
    parentPort.postMessage({ type: 'metrics', data: wMetrics });
  }

  workerLoop().catch(err => {
    console.error(`Worker ${workerId} error:`, err.message);
    parentPort.postMessage({ type: 'metrics', data: wMetrics });
  });
}
