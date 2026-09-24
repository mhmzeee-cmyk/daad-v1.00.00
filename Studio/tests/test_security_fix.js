process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
require('../server/node_modules/dotenv').config({ path: '../server/.env' });
const http = require('http');

function apiReq(method, path, body, token) {
  return new Promise((resolve) => {
    const data = body ? JSON.stringify(body) : null;
    const headers = { 'Content-Type': 'application/json', 'User-Agent': 'SecurityTest/1.0' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const r = http.request({ method, hostname: 'localhost', port: 3000, path, headers, timeout: 10000 }, (res) => {
      let buf = '';
      res.on('data', c => buf += c);
      res.on('end', () => { try { resolve({ s: res.statusCode, d: JSON.parse(buf) }); } catch(e) { resolve({ s: res.statusCode, d: null }); } });
    });
    r.on('error', () => resolve({ s: 0, d: null }));
    if (data) r.write(data);
    r.end();
  });
}

async function main() {
  // Login as student Mohammed_1 (nationalId 1000000000)
  const loginRes = await apiReq('POST', '/api/v1/auth/login', { username: '1000000000', password: 'Test1234!' });
  const token = loginRes.d?.accessToken;
  if (!token) { console.log('Login failed!'); return; }
  console.log('Logged in as Mohammed_1');

  // Get a tier 2 challenge
  const { PrismaClient } = require('../server/node_modules/@prisma/client');
  const prisma = new PrismaClient();
  const tier2Challenge = await prisma.challenge.findFirst({ where: { tier: 2 }, select: { id: true, title: true } });
  console.log('\nTrying to submit tier 2 challenge:', tier2Challenge.title);
  console.log('Student has NOT completed tier 1 yet (needs 80%)\n');

  // Try to submit it
  const submitRes = await apiReq('POST', '/api/v1/student/challenge/submit', {
    challengeId: tier2Challenge.id,
    code: 'print(5+5)',
    output: '10',
  }, token);

  console.log('=== SUBMISSION RESULT ===');
  console.log('Status:', submitRes.s);
  console.log('Response:', JSON.stringify(submitRes.d, null, 2));

  if (submitRes.s === 403 && submitRes.d?.tierLocked) {
    console.log('\n✅ SECURITY FIX WORKS! Tier 2 is properly locked.');
  } else if (submitRes.s === 200) {
    console.log('\n❌ SECURITY STILL BROKEN! Tier 2 accepted the submission!');
  } else {
    console.log('\nOther response');
  }

  // Now test tier 1 (should work)
  const tier1Challenge = await prisma.challenge.findFirst({ where: { tier: 1, order: 4 }, select: { id: true, title: true } });
  console.log('\nTrying tier 1 challenge:', tier1Challenge.title);
  const t1Res = await apiReq('POST', '/api/v1/student/challenge/submit', {
    challengeId: tier1Challenge.id,
    code: 'print("test")',
    output: 'test',
  }, token);
  console.log('Status:', t1Res.s);
  console.log('Response:', JSON.stringify(t1Res.d, null, 2));

  if (t1Res.s === 200) {
    console.log('\n✅ Tier 1 works correctly (unlocked)');
  }
}

main().catch(console.error);
