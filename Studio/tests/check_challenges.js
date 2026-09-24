process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
(async () => {
  const ch = await p.challenge.findMany({
    select: { id: true, title: true, description: true, starterCode: true, expectedOutput: true, tier: true, order: true, xpReward: true, difficulty: true },
    orderBy: [{ tier: 'asc' }, { order: 'asc' }]
  });
  
  // Check challenges visible in screenshot (old placeholders?)
  console.log('=== ALL CHALLENGES ===');
  for (const c of ch) {
    console.log(`T${c.tier} #${c.order} | ${c.title} | XP:${c.xpReward} | ${c.difficulty}`);
  }
  console.log('Total:', ch.length);
  
  // Check submissions
  const subs = await p.submission.findMany({ orderBy: { createdAt: 'desc' }, take: 10, select: { id: true, challengeId: true, passed: true, output: true, createdAt: true } });
  console.log('\n=== RECENT SUBMISSIONS ===');
  for (const s of subs) {
    console.log(JSON.stringify(s));
  }
  
  await p.$disconnect();
})().catch(e => { console.error(e); process.exit(1); });
