process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const prisma = new PrismaClient();

const TIER_DIFF = { 1:'BEGINNER', 2:'INTERMEDIATE', 3:'ADVANCED', 4:'ADVANCED', 5:'EXPERT', 6:'EXPERT', 7:'EXPERT' };

async function main() {
  for (let tier = 1; tier <= 7; tier++) {
    const diff = TIER_DIFF[tier];
    const result = await prisma.challenge.updateMany({
      where: { tier },
      data: { difficulty: diff }
    });
    console.log('Tier ' + tier + ' (' + diff + '): ' + result.count + ' updated');
  }
}

main().catch(console.error).finally(() => prisma.$disconnect());
