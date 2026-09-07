const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
async function main() {
  const c = await p.challenge.count();
  console.log('Total challenges:', c);
  const r = await p.challenge.findMany({ take: 5, select: { id: true, title: true, tier: true, expectedOutput: true } });
  r.forEach(x => console.log(x.id.substring(0, 20), '...', x.title.substring(0, 30), 'tier:', x.tier, 'out:', x.expectedOutput.substring(0, 30)));
  await p.$disconnect();
}
main();
