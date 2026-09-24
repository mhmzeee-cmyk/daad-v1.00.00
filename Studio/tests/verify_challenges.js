process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
(async () => {
  const c = await p.challenge.groupBy({ by: ['tier'], _count: true, orderBy: { tier: 'asc' } });
  for (const r of c) console.log('Tier', r.tier + ':', r._count, 'challenges');
  console.log('Total:', await p.challenge.count());
  
  const sample1 = await p.challenge.findFirst({ where: { tier: 1 }, select: { title: true, description: true, starterCode: true, expectedOutput: true, requirements: true, difficulty: true, xpReward: true } });
  console.log('\nTier 1 sample:', JSON.stringify(sample1, null, 2));
  
  const sample5 = await p.challenge.findFirst({ where: { tier: 5 }, select: { title: true, description: true, starterCode: true, expectedOutput: true, requirements: true, difficulty: true, xpReward: true } });
  console.log('\nTier 5 sample:', JSON.stringify(sample5, null, 2));
  
  await p.$disconnect();
})();
