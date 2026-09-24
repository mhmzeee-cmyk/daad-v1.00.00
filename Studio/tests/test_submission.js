process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
process.env.JWT_SECRET = 'dhad-studio-dev-secret-key-change-in-production';
process.env.HMAC_SECRET = 'dhad-studio-hmac-secret-2026-production';

const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();

(async () => {
  // Get first T1 challenge
  const challenge = await p.challenge.findFirst({ where: { tier: 1, order: 1 } });
  console.log('Challenge:', challenge.title);
  console.log('StarterCode:', challenge.starterCode);
  console.log('Expected:', challenge.expectedOutput);
  console.log('Requirements:', challenge.requirements);
  
  // Simulate code verification
  const { verifyCode } = require('../server/src/utils/codeVerifier');
  const studentCode = 'طباعة("مرحباً بالعالم")';
  const result = verifyCode(studentCode, challenge.requirements);
  console.log('\nVerification result:', JSON.stringify(result));
  
  // Test with code that doesn't use required keyword
  const badCode = 'console.log("مرحباً")';
  const result2 = verifyCode(badCode, challenge.requirements);
  console.log('Bad code verification:', JSON.stringify(result2));
  
  await p.$disconnect();
})().catch(e => { console.error(e); process.exit(1); });
