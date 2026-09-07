const { PrismaClient } = require('@prisma/client');
const p = new PrismaClient();
(async () => {
  const u = await p.user.count();
  const s = await p.submission.count();
  const c = await p.challenge.count();
  const l = await p.lesson.count();
  const as = await p.assessment.count();
  const cl = await p.classroom.count();
  console.log(JSON.stringify({ users: u, submissions: s, challenges: c, lessons: l, assessments: as, classrooms: cl }));
  await p.$disconnect();
})().catch(e => { console.error(e.message); process.exit(1); });
