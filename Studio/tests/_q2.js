process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
require('../server/node_modules/dotenv').config({ path: '../server/.env' });
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
async function main() {
  const lessons = await p.lesson.findMany({ orderBy: { order: 'asc' } });
  console.log('Lessons in DB:', lessons.length);
  if (lessons.length === 0) {
    console.log('NO LESSONS! The roadmap endpoint needs lessons to function.');
  }
  lessons.forEach(l => console.log('  Order:', l.order, '| Title:', l.title, '| Published:', l.published));

  const challenges = await p.challenge.count();
  console.log('\nChallenges:', challenges);

  const profiles = await p.studentProfile.count();
  console.log('Student profiles:', profiles);
}
main().catch(console.error).finally(() => p.$disconnect());
