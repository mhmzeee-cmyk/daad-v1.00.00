process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();

(async () => {
  const challenges = await p.challenge.findMany({ select: { id: true, title: true, starterCode: true } });
  console.log('Total challenges:', challenges.length);
  
  // Clear all starterCode to empty template
  const result = await p.challenge.updateMany({
    data: { starterCode: '// اكتب كودك هنا' }
  });
  console.log('Cleared starterCode for', result.count, 'challenges');
  
  await p.$disconnect();
})().catch(e => { console.error(e); process.exit(1); });
