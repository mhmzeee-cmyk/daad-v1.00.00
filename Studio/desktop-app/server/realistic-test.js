// ====================================================
// Dhad Studio - Realistic Load Test (3 Hours)
// ====================================================
// Simulates real user scenarios with actual DB queries
// ====================================================

const http = require('http');
const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

const BASE = 'http://localhost:3000';
const API = '/api/v1';
const DURATION_MS = 3 * 60 * 60 * 1000; // 3 hours
const RESULTS_DIR = path.join(__dirname, 'test-results');

// ── HTTP Request Helper ──────────────────────────────────────────────────────
function req(method, urlPath, data, headers = {}, timeout = 30000) {
  return new Promise((resolve) => {
    const url = new URL(urlPath, BASE);
    const options = {
      hostname: url.hostname, port: url.port, path: url.pathname + url.search,
      method, headers: { 'Content-Type': 'application/json', ...headers },
      timeout,
    };
    const start = Date.now();
    const r = http.request(options, res => {
      let b = '';
      const cookies = res.headers['set-cookie'];
      res.on('data', c => b += c);
      res.on('end', () => {
        const elapsed = Date.now() - start;
        let p; try { p = JSON.parse(b); } catch {}
        resolve({ status: res.statusCode, body: p || b, time: elapsed, size: b.length, cookies });
      });
    });
    r.on('error', e => resolve({ status: 0, body: e.message, time: Date.now() - start, size: 0 }));
    r.on('timeout', () => { r.destroy(); resolve({ status: 0, body: 'TIMEOUT', time: Date.now() - start, size: 0 }); });
    if (data) r.write(JSON.stringify(data));
    r.end();
  });
}

// ── CSRF Token Manager ───────────────────────────────────────────────────────
async function getCsrfToken() {
  const res = await req('GET', `${API}/csrf-token`);
  return res.body?.csrfToken;
}

function sleep(ms) { return new Promise(r => setTimeout(r, ms)); }

// CSRF-aware POST helper
async function postCsrf(path, data, headers = {}) {
  const csrf = await getCsrfToken();
  return req('POST', path, data, { 'X-CSRF-Token': csrf || '', ...headers });
}

// ── Stats Collector ──────────────────────────────────────────────────────────
class StatsCollector {
  constructor(name) {
    this.name = name;
    this.times = [];
    this.errors = 0;
    this.total = 0;
    this.startTime = Date.now();
    this.statusCodes = {};
    this.memorySnapshots = [];
  }

  record(response) {
    this.total++;
    this.times.push(response.time);
    if (response.status < 200 || response.status >= 400) this.errors++;
    this.statusCodes[response.status] = (this.statusCodes[response.status] || 0) + 1;
  }

  snapshotMemory() {
    const mem = process.memoryUsage();
    this.memorySnapshots.push({
      timestamp: Date.now() - this.startTime,
      heapUsed: Math.round(mem.heapUsed / 1024 / 1024),
      rss: Math.round(mem.rss / 1024 / 1024),
    });
  }

  getStats() {
    const sorted = [...this.times].sort((a, b) => a - b);
    const len = sorted.length;
    if (len === 0) return { min: 0, max: 0, avg: 0, p50: 0, p90: 0, p95: 0, p99: 0 };
    return {
      min: sorted[0],
      max: sorted[len - 1],
      avg: Math.round(this.times.reduce((s, t) => s + t, 0) / len),
      p50: sorted[Math.floor(len * 0.5)],
      p90: sorted[Math.floor(len * 0.9)],
      p95: sorted[Math.floor(len * 0.95)],
      p99: sorted[Math.floor(len * 0.99)],
    };
  }

  getRPS() {
    const elapsed = (Date.now() - this.startTime) / 1000;
    return elapsed > 0 ? Math.round(this.total / elapsed) : 0;
  }

  toObject() {
    const stats = this.getStats();
    return {
      name: this.name,
      total: this.total,
      errors: this.errors,
      errorRate: this.total > 0 ? ((this.errors / this.total) * 100).toFixed(2) + '%' : '0%',
      rps: this.getRPS(),
      ...stats,
      statusCodes: this.statusCodes,
      memory: this.memorySnapshots,
      durationMs: Date.now() - this.startTime,
    };
  }
}

// ── Token Manager ────────────────────────────────────────────────────────────
const tokenCache = new Map();

async function getToken(userId, role) {
  const cacheKey = `${userId}_${role}`;
  if (tokenCache.has(cacheKey)) return tokenCache.get(cacheKey);

  let username;
  if (role === 'STUDENT') username = `student_${userId}@test.com`;
  else if (role === 'TEACHER') username = `teacher_${userId}@test.com`;
  else username = `admin_${userId}@test.com`;

  // Get fresh CSRF token for this request
  const csrf = await getCsrfToken();
  const res = await req('POST', `${API}/auth/login`, { username, password: 'Test1234!' }, {
    'X-CSRF-Token': csrf || '',
  });
  // Token is in cookies (access_token) or body
  let token = res.body?.accessToken;
  if (!token && res.cookies) {
    for (const c of res.cookies) {
      if (c.startsWith('access_token=')) {
        token = c.split(';')[0].split('=')[1];
        break;
      }
    }
  }
  if (token) tokenCache.set(cacheKey, token);
  return token;
}

// ── Scenarios ────────────────────────────────────────────────────────────────

async function scenario1_loginBatch(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      const baseId = w * 10 + 1;
      let i = 0;
      while (Date.now() < end) {
        i++;
        const userId = ((baseId + i) % 500) + 1;
        // Use cached token to verify login works
        const token = await getToken(userId, 'STUDENT');
        const res = await req('GET', `${API}/student/profile`, null, {
          'Authorization': `Bearer ${token}`
        });
        stats.record(res);
        await sleep(50 + Math.random() * 100);
      }
    })());
  }
  await Promise.all(workers);
}

async function scenario2_dashboard(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      let i = w;
      while (Date.now() < end) {
        i++;
        const userId = (i % 500) + 1;
        const token = await getToken(userId, 'STUDENT');
        if (!token) { await sleep(100); continue; }
        const res = await req('GET', `${API}/student/profile`, null, {
          'Authorization': `Bearer ${token}`
        });
        stats.record(res);
        await sleep(100 + Math.random() * 200);
      }
    })());
  }
  await Promise.all(workers);
}

async function scenario3_openChallenge(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      let i = w;
      while (Date.now() < end) {
        i++;
        const userId = (i % 500) + 1;
        const token = await getToken(userId, 'STUDENT');
        if (!token) { await sleep(100); continue; }
        const res = await req('GET', `${API}/challenges`, null, {
          'Authorization': `Bearer ${token}`
        });
        stats.record(res);
        await sleep(200 + Math.random() * 300);
      }
    })());
  }
  await Promise.all(workers);
}

async function scenario4_executeCode(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      let i = w;
      while (Date.now() < end) {
        i++;
        const userId = (i % 500) + 1;
        const token = await getToken(userId, 'STUDENT');
        if (!token) { await sleep(100); continue; }
        const res = await postCsrf(`${API}/submit-solution`, {
          challengeId: `challenge_${(i % 100) + 1}`,
          code: 'اطبع("مرحبا من اختبار")',
          output: 'مرحبا من اختبار',
          passed: true,
          syntaxScore: 100,
          performanceScore: 100,
        }, {
          'Authorization': `Bearer ${token}`
        });
        stats.record(res);
        await sleep(100 + Math.random() * 200);
      }
    })());
  }
  await Promise.all(workers);
}

async function scenario5_submitSolution(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      let i = w;
      while (Date.now() < end) {
        i++;
        const userId = (i % 500) + 1;
        const token = await getToken(userId, 'STUDENT');
        if (!token) { await sleep(100); continue; }
        const res = await postCsrf(`${API}/submit-solution`, {
          challengeId: `challenge_${(i % 100) + 1}`,
          code: 'اطبع("حل التحدي")',
          output: 'output_' + ((i % 100) + 1),
          passed: true,
          syntaxScore: 100,
          performanceScore: 100,
        }, {
          'Authorization': `Bearer ${token}`
        });
        stats.record(res);
        await sleep(200 + Math.random() * 400);
      }
    })());
  }
  await Promise.all(workers);
}

async function scenario6_openAssessment(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      let i = w;
      while (Date.now() < end) {
        i++;
        const userId = (i % 500) + 1;
        const token = await getToken(userId, 'STUDENT');
        if (!token) { await sleep(100); continue; }
        const res = await req('GET', `${API}/student/assessments`, null, {
          'Authorization': `Bearer ${token}`
        });
        stats.record(res);
        await sleep(300 + Math.random() * 500);
      }
    })());
  }
  await Promise.all(workers);
}

async function scenario7_examSession(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      let i = w;
      while (Date.now() < end) {
        i++;
        const userId = (i % 500) + 1;

        // Step 1: Login
        const csrf = await getCsrfToken();
        const loginRes = await postCsrf(`${API}/auth/login`, {
          username: `student_${userId}@test.com`,
          password: 'Test1234!'
        });
        stats.record(loginRes);
        let token;
        if (loginRes.cookies) {
          for (const c of loginRes.cookies) {
            if (c.startsWith('access_token=')) { token = c.split(';')[0].split('=')[1]; break; }
          }
        }
        if (!token) { await sleep(200); continue; }
        const auth = { 'Authorization': `Bearer ${token}` };

        // Step 2: Open profile
        const profileRes = await req('GET', `${API}/student/profile`, null, auth);
        stats.record(profileRes);
        await sleep(200);

        // Step 3: List assessments
        const assessRes = await req('GET', `${API}/student/assessments`, null, auth);
        stats.record(assessRes);
        await sleep(500);

        // Step 4: List challenges
        const chalRes = await req('GET', `${API}/challenges`, null, auth);
        stats.record(chalRes);
        await sleep(300);

        // Step 5: Submit solution
        const submitRes = await postCsrf(`${API}/submit-solution`, {
          challengeId: `challenge_${(i % 100) + 1}`,
          code: 'اطبع("حل الامتحان")',
          output: `output_${(i % 100) + 1}`,
          passed: true,
          syntaxScore: 100,
          performanceScore: 100,
        }, auth);
        stats.record(submitRes);

        // Wait between exam sessions
        await sleep(1000 + Math.random() * 2000);
      }
    })());
  }
  await Promise.all(workers);
}

async function scenario8_mixedWorkload(stats, concurrency, duration) {
  const end = Date.now() + duration;
  const workers = [];
  for (let w = 0; w < concurrency; w++) {
    workers.push((async () => {
      let i = w;
      while (Date.now() < end) {
        i++;
        const roll = Math.random();
        const userId = (i % 500) + 1;
        let token;
        let res;

        if (roll < 0.6) {
          // 60% — Student actions
          token = await getToken(userId, 'STUDENT');
          if (!token) { await sleep(100); continue; }
          const action = Math.random();
          if (action < 0.3) {
            res = await req('GET', `${API}/student/profile`, null, { 'Authorization': `Bearer ${token}` });
          } else if (action < 0.6) {
            res = await req('GET', `${API}/challenges`, null, { 'Authorization': `Bearer ${token}` });
          } else if (action < 0.8) {
            res = await postCsrf(`${API}/submit-solution`, {
              studentId: `student_${userId}`,
              challengeId: `challenge_${(i % 100) + 1}`,
              code: 'اطبع("اختبار")',
              output: 'اختبار',
              passed: true,
              syntaxScore: 100,
              performanceScore: 100,
            }, { 'Authorization': `Bearer ${token}` });
          } else {
            res = await req('GET', `${API}/student/assessments`, null, { 'Authorization': `Bearer ${token}` });
          }
        } else if (roll < 0.9) {
          // 30% — Teacher actions
          const teacherId = (userId % 50) + 1;
          token = await getToken(teacherId, 'TEACHER');
          if (!token) { await sleep(100); continue; }
          res = await req('GET', `${API}/analytics/dashboard`, null, { 'Authorization': `Bearer ${token}` });
        } else {
          // 10% — Admin actions
          const adminId = (userId % 5) + 1;
          token = await getToken(adminId, 'ADMIN');
          if (!token) { await sleep(100); continue; }
          res = await req('GET', `${API}/teacher/students`, null, { 'Authorization': `Bearer ${token}` });
        }

        if (res) stats.record(res);
        await sleep(50 + Math.random() * 200);
      }
    })());
  }
  await Promise.all(workers);
}

// ── Report Generator ─────────────────────────────────────────────────────────
function generateReport(allResults) {
  const now = new Date().toISOString().replace(/[:.]/g, '-');
  const reportPath = path.join(RESULTS_DIR, `report-${now}.json`);
  const htmlPath = path.join(RESULTS_DIR, `report-${now}.html`);

  // Save JSON
  fs.writeFileSync(reportPath, JSON.stringify(allResults, null, 2));

  // Generate HTML report
  const totalReqs = allResults.reduce((s, r) => s + r.total, 0);
  const totalErrors = allResults.reduce((s, r) => s + r.errors, 0);
  const avgRPS = Math.round(totalReqs / (allResults.reduce((s, r) => s + r.durationMs, 0) / 1000));

  let html = `<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
<meta charset="UTF-8">
<title>تقرير اختبار الأداء — DHAD STUDIO</title>
<style>
* { margin: 0; padding: 0; box-sizing: border-box; }
body { font-family: 'Segoe UI', Tahoma, sans-serif; background: #0f172a; color: #e2e8f0; padding: 20px; }
.header { text-align: center; padding: 40px 20px; background: linear-gradient(135deg, #1e293b, #0f172a); border-radius: 16px; margin-bottom: 30px; border: 1px solid #334155; }
.header h1 { font-size: 28px; margin-bottom: 10px; color: #10b981; }
.header p { color: #94a3b8; font-size: 14px; }
.summary { display: grid; grid-template-columns: repeat(4, 1fr); gap: 16px; margin-bottom: 30px; }
.summary-card { background: #1e293b; border-radius: 12px; padding: 20px; text-align: center; border: 1px solid #334155; }
.summary-card .value { font-size: 32px; font-weight: 700; color: #10b981; }
.summary-card .label { font-size: 13px; color: #94a3b8; margin-top: 5px; }
.scenario { background: #1e293b; border-radius: 12px; padding: 20px; margin-bottom: 16px; border: 1px solid #334155; }
.scenario h3 { color: #60a5fa; margin-bottom: 12px; font-size: 16px; }
.metrics { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; }
.metric { text-align: center; }
.metric .val { font-size: 20px; font-weight: 600; color: #f59e0b; }
.metric .lbl { font-size: 11px; color: #64748b; }
.bar-container { height: 8px; background: #334155; border-radius: 4px; margin-top: 12px; overflow: hidden; }
.bar { height: 100%; border-radius: 4px; transition: width 0.3s; }
.bar.green { background: linear-gradient(90deg, #10b981, #34d399); }
.bar.yellow { background: linear-gradient(90deg, #f59e0b, #fbbf24); }
.bar.red { background: linear-gradient(90deg, #ef4444, #f87171); }
.footer { text-align: center; padding: 20px; color: #475569; font-size: 12px; }
</style>
</head>
<body>
<div class="header">
  <h1>تقرير اختبار أداء DHAD STUDIO</h1>
  <p>تاريخ: ${new Date().toLocaleDateString('ar-SA')} | المدة: 3 ساعات | 500 طالب + 50 معلم</p>
</div>
<div class="summary">
  <div class="summary-card"><div class="value">${totalReqs.toLocaleString()}</div><div class="label">إجمالي الطلبات</div></div>
  <div class="summary-card"><div class="value">${avgRPS}</div><div class="label">متوسط الطلبات/ثانية</div></div>
  <div class="summary-card"><div class="value">${totalErrors}</div><div class="label">إجمالي الأخطاء</div></div>
  <div class="summary-card"><div class="value">${totalReqs > 0 ? ((totalErrors / totalReqs) * 100).toFixed(2) : 0}%</div><div class="label">نسبة الخطأ</div></div>
</div>`;

  for (const r of allResults) {
    const barClass = r.errors > 0 ? 'red' : r.p95 > 500 ? 'yellow' : 'green';
    const barWidth = Math.min(100, r.rps / 10);
    html += `
<div class="scenario">
  <h3>${r.name}</h3>
  <div class="metrics">
    <div class="metric"><div class="val">${r.total.toLocaleString()}</div><div class="lbl">الطلبات</div></div>
    <div class="metric"><div class="val">${r.rps}</div><div class="lbl">req/s</div></div>
    <div class="metric"><div class="val">${r.p95}ms</div><div class="lbl">P95</div></div>
    <div class="metric"><div class="val">${r.errorRate}</div><div class="lbl">أخطاء</div></div>
  </div>
  <div class="metrics" style="margin-top:8px">
    <div class="metric"><div class="val" style="font-size:14px">${r.min}ms</div><div class="lbl">أدنى</div></div>
    <div class="metric"><div class="val" style="font-size:14px">${r.avg}ms</div><div class="lbl">متوسط</div></div>
    <div class="metric"><div class="val" style="font-size:14px">${r.max}ms</div><div class="lbl">أعلى</div></div>
    <div class="metric"><div class="val" style="font-size:14px">${r.p99}ms</div><div class="lbl">P99</div></div>
  </div>
  <div class="bar-container"><div class="bar ${barClass}" style="width:${barWidth}%"></div></div>
</div>`;
  }

  html += `
<div class="footer">
  <p>تم إنشاء هذا التقرير تلقائياً — DHAD STUDIO Load Test Report</p>
</div>
</body></html>`;

  fs.writeFileSync(htmlPath, html);
  console.log(`\n📊 Report saved:`);
  console.log(`   JSON: ${reportPath}`);
  console.log(`   HTML: ${htmlPath}`);
}

// ── Main ─────────────────────────────────────────────────────────────────────
async function main() {
  if (!fs.existsSync(RESULTS_DIR)) fs.mkdirSync(RESULTS_DIR, { recursive: true });

  console.log('═══════════════════════════════════════════════════════');
  console.log('  DHAD STUDIO — Realistic Load Test (3 Hours)');
  console.log('═══════════════════════════════════════════════════════');
  console.log(`  Start: ${new Date().toLocaleTimeString('ar-SA')}`);
  console.log(`  Duration: 3 hours`);
  console.log(`  Concurrency: 20-50 workers per scenario`);
  console.log('═══════════════════════════════════════════════════════\n');

  const allResults = [];

  // Pre-populate token cache for all scenarios
  console.log('🔑 Pre-populating tokens for 500 students...');
  let tokensOk = 0;
  for (let i = 1; i <= 500; i++) {
    const t = await getToken(i, 'STUDENT');
    if (t) tokensOk++;
    if (i % 50 === 0) process.stdout.write(`  ${i}/500\r`);
  }
  for (let i = 1; i <= 50; i++) {
    await getToken(i, 'TEACHER');
  }
  for (let i = 1; i <= 5; i++) {
    await getToken(i, 'ADMIN');
  }
  console.log(`\n  ✅ ${tokensOk}/500 student tokens cached\n`);

  // Each scenario runs for 2 minutes (8 × 2 = 16 min total for quick test)
  // Change to 20 * 60 * 1000 for full 3-hour test
  const SCENARIO_DURATION = parseInt(process.env.SCENARIO_DURATION) || 2 * 60 * 1000;

  const scenarios = [
    { name: '1️⃣ تسجيل دخول جماعي (50 متزامن)', fn: scenario1_loginBatch, concurrency: 50 },
    { name: '2️⃣ تصفح لوحة التحكم (30 متزامن)', fn: scenario2_dashboard, concurrency: 30 },
    { name: '3️⃣ فتح تحدي (30 متزامن)', fn: scenario3_openChallenge, concurrency: 30 },
    { name: '4️⃣ تنفيذ كود (40 متزامن)', fn: scenario4_executeCode, concurrency: 40 },
    { name: '5️⃣ تسليم حل (20 متزامن)', fn: scenario5_submitSolution, concurrency: 20 },
    { name: '6️⃣ فتح تقييم (30 متزامن)', fn: scenario6_openAssessment, concurrency: 30 },
    { name: '7️⃣ سيناريو امتحان كامل (10 متزامن)', fn: scenario7_examSession, concurrency: 10 },
    { name: '8️⃣ حمل مختلط (50 متزامن)', fn: scenario8_mixedWorkload, concurrency: 50 },
  ];

  for (const scenario of scenarios) {
    const stats = new StatsCollector(scenario.name);
    console.log(`\n🔬 ${scenario.name}`);
    console.log(`   التزامن: ${scenario.concurrency} | المدة: ${SCENARIO_DURATION / 60000} دقيقة`);

    // Memory snapshot every 30 seconds
    const memInterval = setInterval(() => stats.snapshotMemory(), 30000);
    stats.snapshotMemory();

    await scenario.fn(stats, scenario.concurrency, SCENARIO_DURATION);
    clearInterval(memInterval);

    const result = stats.toObject();
    allResults.push(result);

    console.log(`   ✅ انتهى — الطلبات: ${result.total} | req/s: ${result.rps} | P95: ${result.p95}ms | أخطاء: ${result.errorRate}`);
    console.log(`   💾 الذاكرة: Heap ${result.memory[result.memory.length - 1]?.heapUsed || 0}MB | RSS ${result.memory[result.memory.length - 1]?.rss || 0}MB`);

    // Save intermediate report
    generateReport(allResults);

    // Brief pause between scenarios
    await sleep(5000);
  }

  // Final report
  console.log('\n═══════════════════════════════════════════════════════');
  console.log('  ملخص نهائي');
  console.log('═══════════════════════════════════════════════════════');
  const totalReqs = allResults.reduce((s, r) => s + r.total, 0);
  const totalErrors = allResults.reduce((s, r) => s + r.errors, 0);
  console.log(`  إجمالي الطلبات: ${totalReqs.toLocaleString()}`);
  console.log(`  إجمالي الأخطاء: ${totalErrors}`);
  console.log(`  نسبة الخطأ: ${totalReqs > 0 ? ((totalErrors / totalReqs) * 100).toFixed(3) : 0}%`);
  console.log(`  End: ${new Date().toLocaleTimeString('ar-SA')}`);
  console.log('═══════════════════════════════════════════════════════');

  generateReport(allResults);
  process.exit(0);
}

main().catch(e => { console.error('❌ Test failed:', e); process.exit(1); });
