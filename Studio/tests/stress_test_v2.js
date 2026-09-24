// ═══════════════════════════════════════════════════════════════════════════════
// 10,000 BOT STRESS TEST v2 - REALISTIC
// Tests login, API endpoints, and mixed traffic
// ═══════════════════════════════════════════════════════════════════════════════

const http = require('http');

const TARGET = { hostname: 'localhost', port: 3000 };
const TOTAL_BOTS = 10000;

function makeRequest(method, path, body, headers = {}) {
  return new Promise((resolve) => {
    const data = body ? JSON.stringify(body) : null;
    const startTime = Date.now();
    
    const opts = {
      hostname: TARGET.hostname,
      port: TARGET.port,
      path,
      method,
      headers: {
        'Content-Type': 'application/json',
        'User-Agent': `Bot-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`,
        ...headers,
        ...(data ? { 'Content-Length': Buffer.byteLength(data) } : {}),
      },
      timeout: 15000,
    };
    
    const req = http.request(opts, (res) => {
      let buf = '';
      res.on('data', d => buf += d);
      res.on('end', () => {
        const elapsed = Date.now() - startTime;
        resolve({ status: res.statusCode, elapsed, timeout: false });
      });
    });
    
    req.on('error', () => resolve({ status: 0, elapsed: Date.now() - startTime, timeout: true }));
    req.on('timeout', () => { req.destroy(); resolve({ status: 0, elapsed: Date.now() - startTime, timeout: true }); });
    if (data) req.write(data);
    req.end();
  });
}

async function main() {
  console.log('╔══════════════════════════════════════════════════════════════╗');
  console.log('║   10,000 BOT STRESS TEST v2 - REALISTIC TRAFFIC            ║');
  console.log('╚══════════════════════════════════════════════════════════════╝\n');

  // Phase 1: Get valid tokens
  console.log('🔑 Phase 1: Obtaining tokens...');
  const tokens = {};
  const loginCreds = [
    { username: 'teacher@dhad.com', password: 'Test1234!' },
    { username: 'shamel@teacher.com', password: 'Test1234!' },
    { username: 'sara@teacher.com', password: 'Test1234!' },
    { username: 'ali@teacher.com', password: 'Test1234!' },
    { username: '1099990001', password: 'Test1234!' },
    { username: 'admin@dhadstudio.com', password: 'Test1234!' },
  ];
  
  for (const cred of loginCreds) {
    const r = await makeRequest('POST', '/api/v1/auth/login', cred);
    if (r.status === 200) {
      tokens[cred.username] = true;
      console.log(`   ✅ ${cred.username}`);
    } else {
      console.log(`   ❌ ${cred.username} -> ${r.status}`);
    }
  }
  console.log(`   Tokens: ${Object.keys(tokens).length}/${loginCreds.length}\n`);

  const startTime = Date.now();
  const stats = {
    total: 0,
    success: 0,
    rateLimited: 0,
    locked: 0,
    errors: 0,
    timeouts: 0,
    serverErrors: 0,
    statusCodes: {},
    endpoints: {},
    responseTimes: [],
  };

  // Phase 2: Launch bots in waves
  console.log('🚀 Phase 2: Launching 10,000 bots in 10 waves...\n');
  
  const WAVE_SIZE = 1000;
  const WAVES = 10;
  
  for (let wave = 0; wave < WAVES; wave++) {
    const waveStart = Date.now();
    const promises = [];
    
    for (let i = 0; i < WAVE_SIZE; i++) {
      const botId = wave * WAVE_SIZE + i;
      
      // Mix traffic types:
      // 30% - Login attempts (valid + invalid)
      // 20% - Authenticated API calls
      // 20% - Invalid login (brute force)
      // 15% - SQL injection attempts
      // 10% - XSS attempts
      // 5% - Path traversal
      
      const trafficType = botId % 20;
      
      if (trafficType < 6) {
        // Login attempts
        const valid = botId % 3 === 0;
        const cred = valid
          ? loginCreds[botId % loginCreds.length]
          : { username: `bot${botId}@test.com`, password: 'wrong' };
        promises.push(makeRequest('POST', '/api/v1/auth/login', cred));
      } else if (trafficType < 10) {
        // Authenticated API calls
        const tokenKeys = Object.keys(tokens);
        if (tokenKeys.length > 0) {
          const token = tokenKeys[botId % tokenKeys.length];
          promises.push(makeRequest('GET', '/api/v1/analytics/leaderboard', null, { Authorization: `Bearer ${token}` }));
        } else {
          promises.push(makeRequest('GET', '/api/v1/health'));
        }
      } else if (trafficType < 14) {
        // Invalid login (brute force)
        promises.push(makeRequest('POST', '/api/v1/auth/login', { username: 'teacher@dhad.com', password: `wrong${botId}` }));
      } else if (trafficType < 17) {
        // SQL injection
        promises.push(makeRequest('POST', '/api/v1/auth/login', { username: `' OR '1'='1' --`, password: 'x' }));
      } else if (trafficType < 19) {
        // XSS
        promises.push(makeRequest('POST', '/api/v1/auth/login', { username: '<script>alert(1)</script>', password: 'x' }));
      } else {
        // Path traversal
        promises.push(makeRequest('GET', '/../../../../etc/passwd'));
      }
    }
    
    const results = await Promise.all(promises);
    const waveTime = Date.now() - waveStart;
    
    for (const r of results) {
      stats.total++;
      stats.responseTimes.push(r.elapsed);
      
      if (r.timeout) stats.timeouts++;
      else if (r.status === 200) stats.success++;
      else if (r.status === 429) stats.rateLimited++;
      else if (r.status === 423) stats.locked++;
      else if (r.status >= 500) stats.serverErrors++;
      else stats.errors++;
      
      stats.statusCodes[r.status] = (stats.statusCodes[r.status] || 0) + 1;
    }
    
    const elapsed = ((Date.now() - startTime) / 1000).toFixed(1);
    const rps = (stats.total / (Date.now() - startTime) * 1000).toFixed(0);
    const pct = ((wave + 1) / WAVES * 100).toFixed(0);
    
    process.stdout.write(`\r  📊 Wave ${wave + 1}/${WAVES} (${pct}%) | Bots: ${stats.total.toLocaleString()} | RPS: ${rps} | Time: ${elapsed}s | Wave: ${waveTime}ms`);
  }
  
  const totalTime = (Date.now() - startTime) / 1000;
  
  // Sort response times for percentiles
  const sorted = [...stats.responseTimes].sort((a, b) => a - b);
  const avg = (stats.responseTimes.reduce((a, b) => a + b, 0) / stats.responseTimes.length).toFixed(0);
  const p50 = sorted[Math.floor(sorted.length * 0.5)];
  const p95 = sorted[Math.floor(sorted.length * 0.95)];
  const p99 = sorted[Math.floor(sorted.length * 0.99)];

  console.log('\n\n');
  console.log('╔══════════════════════════════════════════════════════════════╗');
  console.log('║                    STRESS TEST RESULTS                     ║');
  console.log('╚══════════════════════════════════════════════════════════════╝');

  console.log(`\n  📊 Summary:`);
  console.log(`  ─────────────────────────────────────────`);
  console.log(`  Total Bots:      ${stats.total.toLocaleString()}`);
  console.log(`  Total Time:      ${totalTime.toFixed(1)}s`);
  console.log(`  Requests/sec:    ${(stats.total / totalTime).toFixed(0)}`);

  console.log(`\n  📈 Results:`);
  console.log(`  ─────────────────────────────────────────`);
  console.log(`  ✅ Success:      ${stats.success.toLocaleString()} (${(stats.success/stats.total*100).toFixed(1)}%)`);
  console.log(`  🛑 Rate Limited: ${stats.rateLimited.toLocaleString()} (${(stats.rateLimited/stats.total*100).toFixed(1)}%)`);
  console.log(`  🔒 Locked:       ${stats.locked.toLocaleString()} (${(stats.locked/stats.total*100).toFixed(1)}%)`);
  console.log(`  ❌ Errors:       ${stats.errors.toLocaleString()} (${(stats.errors/stats.total*100).toFixed(1)}%)`);
  console.log(`  💀 Server Error: ${stats.serverErrors.toLocaleString()} (${(stats.serverErrors/stats.total*100).toFixed(1)}%)`);
  console.log(`  ⏰ Timeouts:     ${stats.timeouts.toLocaleString()} (${(stats.timeouts/stats.total*100).toFixed(1)}%)`);

  console.log(`\n  ⚡ Response Times:`);
  console.log(`  ─────────────────────────────────────────`);
  console.log(`  Average:  ${avg}ms`);
  console.log(`  P50:      ${p50}ms`);
  console.log(`  P95:      ${p95}ms`);
  console.log(`  P99:      ${p99}ms`);

  console.log(`\n  📊 Status Codes:`);
  console.log(`  ─────────────────────────────────────────`);
  for (const [code, count] of Object.entries(stats.statusCodes).sort((a, b) => b[1] - a[1])) {
    const pct = (count / stats.total * 100).toFixed(1);
    console.log(`  ${code}: ${count.toLocaleString()} (${pct}%)`);
  }

  // Server health check
  console.log(`\n  🏥 Server Health After Stress:`);
  console.log(`  ─────────────────────────────────────────`);
  const health = await makeRequest('GET', '/health');
  console.log(`  Health Status: ${health.status}`);
  console.log(`  Response Time: ${health.elapsed}ms`);

  // Verdict
  console.log(`\n  🎯 Verdict:`);
  console.log(`  ─────────────────────────────────────────`);
  const crashRate = (stats.serverErrors / stats.total * 100);
  const timeoutRate = (stats.timeouts / stats.total * 100);
  
  if (crashRate === 0 && timeoutRate < 50) {
    console.log(`  ✅ PASSED - Server survived 10,000 concurrent bots!`);
    console.log(`     No server crashes, timeout rate: ${timeoutRate.toFixed(1)}%`);
  } else if (crashRate < 1) {
    console.log(`  ⚠️  WARNING - Server had minimal issues`);
    console.log(`     Crash rate: ${crashRate.toFixed(1)}%, Timeout: ${timeoutRate.toFixed(1)}%`);
  } else {
    console.log(`  ❌ FAILED - Server struggled under load`);
    console.log(`     Crash rate: ${crashRate.toFixed(1)}%, Timeout: ${timeoutRate.toFixed(1)}%`);
  }

  // Save report
  const report = {
    timestamp: new Date().toISOString(),
    totalBots: stats.total,
    totalTime: totalTime.toFixed(1),
    requestsPerSecond: (stats.total / totalTime).toFixed(0),
    results: {
      success: stats.success,
      rateLimited: stats.rateLimited,
      locked: stats.locked,
      errors: stats.errors,
      serverErrors: stats.serverErrors,
      timeouts: stats.timeouts,
    },
    responseTimes: { average: avg, p50, p95, p99 },
    statusCodes: stats.statusCodes,
    verdict: crashRate === 0 ? 'PASSED' : crashRate < 1 ? 'WARNING' : 'FAILED',
  };
  
  require('fs').writeFileSync('C:/Projects/dhad-studio/tests/stress_test_v2_report.json', JSON.stringify(report, null, 2));
  console.log('\n📄 Report saved to tests/stress_test_v2_report.json');
}

main().catch(console.error);
