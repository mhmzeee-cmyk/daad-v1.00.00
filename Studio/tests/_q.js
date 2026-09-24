process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
async function main() {
  const u = await p.user.findFirst({
    where: { name: 'Tariq_6926' },
    include: {
      classroom: { select: { name: true, subject: true, gradeLevel: true } },
      school: { select: { name: true, code: true } },
      studentProfile: true
    }
  });
  if (!u) { console.log('Not found'); return; }
  console.log('Name:', u.name);
  console.log('NationalId:', u.nationalId);
  console.log('Email:', u.email);
  console.log('School:', u.school.name, '(' + u.school.code + ')');
  console.log('Classroom:', u.classroom.name);
  console.log('Subject:', u.classroom.subject);
  console.log('Grade:', u.classroom.gradeLevel);
  console.log('XP:', u.studentProfile.totalXP);
  console.log('Level:', u.studentProfile.currentLevel);
  console.log('Streak:', u.studentProfile.currentStreak);
}
main().catch(console.error).finally(() => p.$disconnect());
