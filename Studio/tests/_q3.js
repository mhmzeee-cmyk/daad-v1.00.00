process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
require('../server/node_modules/dotenv').config({ path: '../server/.env' });
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
async function main() {
  const lessons = await p.lesson.findMany({ orderBy: { order: 'asc' } });
  console.log('Lessons:', lessons.length);
  for (const l of lessons) {
    const chCount = await p.challenge.count({ where: { tier: l.order } });
    console.log('  Tier ' + l.order + ': ' + l.title + ' (' + chCount + ' challenges)');
  }
  
  // Check a student's view
  const student = await p.user.findFirst({ where: { name: 'Mohammed_1' }, select: { id: true, name: true } });
  if (student) {
    const subs = await p.submission.count({ where: { studentId: student.id } });
    const passed = await p.submission.count({ where: { studentId: student.id, passed: true } });
    console.log('\nStudent Mohammed_1:', subs, 'submissions,', passed, 'passed');
    console.log('Tier 1 status: should be UNLOCKED (first tier is always open)');
  }
}
main().catch(console.error).finally(() => p.$disconnect());
