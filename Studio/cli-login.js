#!/usr/bin/env node
// CLI Login — Same database, same server, same security as browser
const http = require('http');
const fs = require('fs');
const path = require('path');

const COOKIE_FILE = path.join(__dirname, '.cli-session.json');
const SERVER = 'http://localhost:3000';

function request(method, urlPath, body, cookies, csrfToken) {
  return new Promise((resolve, reject) => {
    const url = new URL(SERVER + urlPath);
    const headers = { 'Content-Type': 'application/json' };
    if (cookies) headers['Cookie'] = cookies;
    if (csrfToken) headers['X-CSRF-Token'] = csrfToken;

    const req = http.request({
      hostname: url.hostname,
      port: url.port,
      path: url.pathname,
      method,
      headers
    }, res => {
      let data = '';
      res.on('data', c => data += c);
      res.on('end', () => {
        const setCookies = res.headers['set-cookie'] || [];
        resolve({ status: res.statusCode, body: data, setCookies });
      });
    });
    req.on('error', reject);
    if (body) req.write(JSON.stringify(body));
    req.end();
  });
}

function parseCookies(setCookies) {
  return setCookies.map(c => c.split(';')[0]).join('; ');
}

function extractCsrf(cookies) {
  const match = cookies.match(/_csrf_dhad=([^;]+)/);
  return match ? match[1] : null;
}

async function saveSession(data) {
  fs.writeFileSync(COOKIE_FILE, JSON.stringify(data, null, 2));
}

async function loadSession() {
  if (fs.existsSync(COOKIE_FILE)) {
    return JSON.parse(fs.readFileSync(COOKIE_FILE, 'utf8'));
  }
  return null;
}

async function login(email, password) {
  // Step 1: GET to get CSRF cookie
  const init = await request('GET', '/health', null, null, null);
  const cookies = parseCookies(init.setCookies);
  const csrf = extractCsrf(cookies);

  // Step 2: POST login
  const loginRes = await request('POST', '/api/v1/auth/login', { username: email, password }, cookies, csrf);

  if (loginRes.status === 200) {
    const data = JSON.parse(loginRes.body);
    const allCookies = parseCookies(loginRes.setCookies);

    // Merge cookies
    const mergedCookies = cookies + '; ' + allCookies;

    await saveSession({
      email,
      userId: data.userId,
      role: data.role,
      username: data.username,
      school: data.profile?.school?.name,
      cookies: mergedCookies,
      csrf,
      loginTime: new Date().toISOString()
    });

    return { success: true, data, cookies: mergedCookies, csrf };
  } else {
    let errMsg;
    try { errMsg = JSON.parse(loginRes.body).message; } catch { errMsg = loginRes.body; }
    return { success: false, status: loginRes.status, error: errMsg };
  }
}

async function apiCall(method, path, body) {
  const session = await loadSession();
  if (!session) {
    console.log('❌ No session. Run: node cli-login.js <email> <password>');
    process.exit(1);
  }

  // Refresh CSRF
  const init = await request('GET', '/health', null, session.cookies, null);
  const freshCookies = parseCookies(init.setCookies);
  const csrf = extractCsrf(freshCookies) || session.csrf;

  const res = await request(method, path, body, session.cookies, csrf);

  if (res.status === 401 || res.status === 403) {
    console.log('❌ Session expired. Login again:');
    console.log('   node cli-login.js <email> <password>');
    process.exit(1);
  }

  return JSON.parse(res.body);
}

// ── Main ──────────────────────────────────────────────────────────────────────

async function main() {
  const args = process.argv.slice(2);

  if (args.length === 0) {
    // Show session info
    const session = await loadSession();
    if (session) {
      console.log('📋 Active Session:');
      console.log(`   User:  ${session.username}`);
      console.log(`   Role:  ${session.role}`);
      console.log(`   School: ${session.school}`);
      console.log(`   Login: ${session.loginTime}`);
    } else {
      console.log('Usage:');
      console.log('  node cli-login.js <email> <password>     — Login');
      console.log('  node cli-login.js                        — Show session');
      console.log('  node cli-login.js me                     — Profile');
      console.log('  node cli-login.js challenges              — List challenges');
      console.log('  node cli-login.js logout                  — Logout');
    }
    return;
  }

  if (args[0] === 'logout') {
    fs.unlinkSync(COOKIE_FILE);
    console.log('✅ Logged out');
    return;
  }

  if (args[0] === 'me') {
    const schools = await apiCall('GET', '/api/v1/schools');
    const session = await loadSession();
    console.log('👤 Session Info:');
    console.log(`   User:  ${session.username}`);
    console.log(`   Role:  ${session.role}`);
    console.log(`   School: ${session.school}`);
    console.log('🏫 Schools:', JSON.stringify(schools, null, 2));
    return;
  }

  if (args[0] === 'challenges') {
    const ch = await apiCall('GET', '/api/v1/challenges');
    console.log('🎯 Challenges:', JSON.stringify(ch, null, 2));
    return;
  }

  // Login
  const email = args[0];
  const password = args[1];

  if (!email || !password) {
    console.log('Usage: node cli-login.js <email> <password>');
    process.exit(1);
  }

  process.stdout.write('🔐 Logging in...');
  const result = await login(email, password);

  if (result.success) {
    console.log(` ✅`);
    console.log(`   Welcome: ${result.data.profile?.name || result.data.username}`);
    console.log(`   Role: ${result.data.role}`);
    console.log(`   School: ${result.data.profile?.school?.name || 'N/A'}`);
    console.log(`   Token expires in: ${result.data.expiresIn}s`);
  } else {
    console.log(` ❌ (${result.status})`);
    console.log(`   ${result.error}`);
    process.exit(1);
  }
}

main().catch(e => { console.error('Error:', e.message); process.exit(1); });
