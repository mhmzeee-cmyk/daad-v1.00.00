// ═══════════════════════════════════════════════════════════════════════════════
// 10,000 BOT STRESS TEST - Dhad Studio
// Simulates massive concurrent login attempts
// ═══════════════════════════════════════════════════════════════════════════════

const http = require('http');
const crypto = require('crypto');

const TARGET = { hostname: 'localhost', port: 3000 };
const TOTAL_BOTS = 10000;
const BATCH_SIZE = 500; // Concurrent requests per batch

// Bot credentials
const VALID_CREDENTIALS = [
  { username: 'teacher@dhad.com', password: 'Test1234!' },
  { username: 'shamel@teacher.com', password: 'Test1234!' },
  { username: '1099990001', password: 'Test1234!' },
  { username: 'admin@dhadstudio.com', password: 'Test1234!' },
];

const INVALID_CREDENTIALS = [
  { username: 'hacker@evil.com', password: 'wrongpassword' },
  { username: "' OR '1'='1", password: 'sqlinjection' },
  { username: '<script>alert(1)</script>', password: 'xss' },
  { username: 'A'.repeat(1000), password: 'long' },
];

function makeRequest(body) {
  return new Promise((resolve) => {
    const data = JSON.stringify(body);
    const startTime = Date.now();
    
    const req = http.request({
      hostname: TARGET.hostname,
      port: TARGET.port,
      path: '/api/v1/auth/login',
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Content-Length': Buffer.byteLength(data),
        'User-Agent': `Bot-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`,
        'X-Forwarded-For': `${Math.floor(Math.random() * 255)}.${Math.floor(Math.random() * 255)}.${Math.floor(Math.random() * 255)}.${Math.floor(Math.random() * 255)}`,
      },
      timeout: 10000,
    }, (res) => {
      let buf = '';
      res.on('data', d => buf += d);
      res.on('end', () => {
        const elapsed = Date.now() - startTime;
        let parsed;
        try { parsed = JSON.parse(buf); } catch { parsed = buf; }
        resolve({
          status: res.statusCode,
          elapsed,
          body: parsed,
          rateLimited: res.statusCode === 429,
          locked: res.statusCode === 423,
          success: res.statusCode === 200,
          error: res.statusCode >= 400,
        });
      });
    });
    
    req.on('error', (e) => {
      resolve({ status: 0, elapsed: Date.now() - startTime, body: { error: e.message }, rateLimited: false, locked: false, success: false, error: true });
    });
    
    req.on('timeout', () => {
      req.destroy();
      resolve({ status: 0, elapsed: Date.now() - startTime, body: { error: 'TIMEOUT' }, rateLimited: false, locked: false, success: false, error: true });
    });
    
    req.write(data);
    req.end();
  });
}

async function runBatch(botId, batchSize) {
  const promises = [];
  for (let i = 0; i < batchSize; i++) {
    const id = botId + i;
    // Mix of valid and invalid credentials
    const creds = id % 4 === 0
      ? VALID_CREDENTIALS[id % VALID_CREDENTIALS.length]
      : INVALID_CREDENTIALS[id % INVALID_CREDENTIALS.length];
    promises.push(makeRequest(creds));
  }
  return Promise.all(promises);
}

async function main() {
  console.log('╔══════════════════════════════════════════════════════════════╗');
  console.log('║   10,000 BOT STRESS TEST - DHAD STUDIO                    ║');
  console.log('║   Testing server under massive concurrent load             ║');
  console.log('╚══════════════════════════════════════════════════════════════╝\n');
  
  console.log(`🎯 Target: http://${TARGET.hostname}:${TARGET.port}`);
  console.log(`🤖 Total Bots: ${TOTAL_BOTS.toLocaleString()}`);
  console.log(`⚡ Batch Size: ${BATCH_SIZE}`);
  console.log(`📦 Total Batches: ${Math.ceil(TOTAL_BOTS / BATCH_SIZE)}\n`);
  
  const startTime = Date.now();
  const allResults = [];
  const stats = {
    total: 0,
    success: 0,
    rateLimited: 0,
    locked: 0,
    errors: 0,
    timeouts: 0,
    serverErrors: 0,
    statusCodes: {},
    responseTimes: [],
  };
  
  console.log('🚀 Launching bots...\n');
  
  // Launch in batches
  const totalBatches = Math.ceil(TOTAL_BOTS / BATCH_SIZE);
  
  for (let batch = 0; batch < totalBatches; batch++) {
    const botId = batch * BATCH_SIZE;
    const batchSize = Math.min(BATCH_SIZE, TOTAL_BOTS - botId);
    
    const batchStart = Date.now();
    const results = await runBatch(botId, batchSize);
    const batchTime = Date.now() - batchStart;
    
    // Process results
    for (const r of results) {
      stats.total++;
      stats.responseTimes.push(r.elapsed);
      
      if (r.success) stats.success++;
      else if (r.rateLimited) stats.rateLimited++;
      else if (r.locked) stats.locked++;
      else if (r.status === 0) stats.timeouts++;
      else if (r.status >= 500) stats.serverErrors++;
      else stats.errors++;
      
      stats.statusCodes[r.status] = (stats.statusCodes[r.status] || 0) + 1;
    }
    
    // Progress
    const progress = ((batch + 1) / totalBatches * 100).toFixed(1);
    const elapsed = ((Date.now() - startTime) / 1000).toFixed(1);
    const rps = (stats.total / (Date.now() - startTime) * 1000).toFixed(0);
    
    process.stdout.write(`\r  📊 Progress: ${progress}% | Bots: ${stats.total.toLocaleString()}/${TOTAL_BOTS.toLocaleString()} | RPS: ${rps} | Time: ${elapsed}s`);
  }
  
  const totalTime = (Date.now() - startTime) / 1000;
  
  // Calculate stats
  const avgResponseTime = stats.responseTimes.length > 0
    ? (stats.responseTimes.reduce((a, b) => a + b, 0) / stats.responseTimes.length).toFixed(0)
    : 0;
  const maxResponseTime = Math.max(...stats.responseTimes);
  const minResponseTime = Math.min(...stats.responseTimes);
  const p95 = stats.responseTimes.sort((a, b) => a - b)[Math.floor(stats.responseTimes.length * 0.95)];
  const p99 = stats.responseTimes.sort((a, b) => a - b)[Math.floor(stats.responseTimes.length * 0.99)];
  
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
  console.log(`  Average:  ${avgResponseTime}ms`);
  console.log(`  Min:      ${minResponseTime}ms`);
  console.log(`  Max:      ${maxResponseTime}ms`);
  console.log(`  P95:      ${p95}ms`);
  console.log(`  P99:      ${p99}ms`);
  
  console.log(`\n  📊 Status Codes:`);
  console.log(`  ─────────────────────────────────────────`);
  for (const [code, count] of Object.entries(stats.statusCodes).sort((a, b) => b[1] - a[1])) {
    const pct = (count / stats.total * 100).toFixed(1);
    console.log(`  ${code}: ${count.toLocaleString()} (${pct}%)`);
  }
  
  // Server health check after stress test
  console.log(`\n  🏥 Server Health After Stress Test:`);
  console.log(`  ─────────────────────────────────────────`);
  const healthCheck = await makeRequest({ username: 'healthcheck', password: 'test' });
  console.log(`  Status: ${healthCheck.status}`);
  console.log(`  Response: ${JSON.stringify(healthCheck.body).substring(0, 100)}`);
  
  // Verdict
  console.log(`\n  🎯 Verdict:`);
  console.log(`  ─────────────────────────────────────────`);
  if (stats.serverErrors === 0 && stats.timeouts < 10) {
    console.log(`  ✅ PASSED - Server handled ${stats.total.toLocaleString()} concurrent requests`);
    console.log(`     without crashes or significant errors.`);
  } else if (stats.serverErrors < 100) {
    console.log(`  ⚠️  WARNING - Server had ${stats.serverErrors} errors under load.`);
  } else {
    console.log(`  ❌ FAILED - Server crashed or had significant errors.`);
    console.log(`     ${stats.serverErrors} server errors out of ${stats.total.toLocaleString()} requests.`);
  }
  
  // Save report
  const report = {
    timestamp: new Date().toISOString(),
    totalBots: TOTAL_BOTS,
    batchSize: BATCH_SIZE,
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
    responseTimes: {
      average: avgResponseTime,
      min: minResponseTime,
      max: maxResponseTime,
      p95,
      p99,
    },
    statusCodes: stats.statusCodes,
  };
  
  const fs = require('fs');
  fs.writeFileSync('C:/Projects/dhad-studio/tests/stress_test_report.json', JSON.stringify(report, null, 2));
  console.log('\n📄 Report saved to tests/stress_test_report.json');
}

main().catch(console.error);
