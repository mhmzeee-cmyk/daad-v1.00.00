process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
async function main() {
  const challenges = await p.challenge.findMany({ select: { id: true, title: true, tier: true, requirements: true, expectedOutput: true }, take: 5 });
  challenges.forEach(c => {
    console.log(c.title + ' (T' + c.tier + ')');
    console.log('  requirements:', c.requirements || 'NULL');
    console.log('  expectedOutput:', c.expectedOutput);
    console.log('');
  });
  
  const withReq = await p.challenge.count({ where: { requirements: { not: null } } });
  const total = await p.challenge.count();
  console.log('Challenges with requirements:', withReq, '/', total);
}
main().catch(console.error).finally(() => p.$disconnect());
