process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
process.env.JWT_SECRET = 'dhad-studio-dev-secret-key-change-in-production';
process.env.HMAC_SECRET = 'dhad-studio-hmac-secret-2026-production';

const { PrismaClient } = require('../server/node_modules/@prisma/client');
const prisma = new PrismaClient();

async function main() {
  console.log('=== RESEED CHALLENGES ===');
  
  const count = await prisma.challenge.count();
  console.log('Current challenges:', count);
  
  const subCount = await prisma.submission.count();
  console.log('Submissions:', subCount);
  
  await prisma.studentSubmission.deleteMany();
  await prisma.submission.deleteMany();
  await prisma.challengeTier.deleteMany();
  await prisma.cloudWorkspace.deleteMany();
  await prisma.challengeVerification.deleteMany();
  await prisma.challenge.deleteMany();
  console.log('Deleted all submissions, tier assignments, workspaces, verifications, challenges');
  
  const challenges = require('./challenges_data.js');
  console.log('Loaded', challenges.length, 'challenges');
  
  const lessons = await prisma.lesson.count();
  const school = await prisma.school.findFirst();
  if (!school) { console.error('No school found!'); prisma.$disconnect(); return; }
  
  if (lessons === 0) {
    console.log('Recreating lessons...');
    const lessonData = [
      { title: 'أساسيات ض', contentSlug: 'basics', order: 1, published: true },
      { title: 'التحكم المنطقي', contentSlug: 'conditionals', order: 2, published: true },
      { title: 'الدوال', contentSlug: 'functions', order: 3, published: true },
      { title: 'الفئات', contentSlug: 'classes', order: 4, published: true },
      { title: 'المهارات المتقدمة', contentSlug: 'advanced', order: 5, published: true },
      { title: 'المصفوفات', contentSlug: 'arrays', order: 6, published: true },
    ];
    for (const l of lessonData) {
      await prisma.lesson.create({ data: { ...l, schoolId: school.id, content: '' } });
    }
    console.log('Lessons created');
  } else {
    console.log('Lessons exist:', lessons);
    // Add missing lessons (e.g. Arrays)
    const existing = await prisma.lesson.findMany({ select: { order: true } });
    const existingOrders = existing.map(l => l.order);
    const missingLessons = [
      { title: 'المصفوفات', contentSlug: 'arrays', order: 6, published: true },
    ];
    for (const l of missingLessons) {
      if (!existingOrders.includes(l.order)) {
        await prisma.lesson.create({ data: { ...l, schoolId: school.id, content: '' } });
        console.log('Added missing lesson:', l.title);
      }
    }
  }
  
  let created = 0;
  const admin = await prisma.user.findFirst({ where: { role: 'ADMIN' } });
  for (const c of challenges) {
    try {
      await prisma.challenge.create({ data: { ...c, authorId: admin.id, published: true } });
      created++;
    } catch(e) {
      console.error('Failed:', c.title, e.message);
    }
  }
  
  console.log('Created', created, 'challenges');
  
  // NOTE: starterCode is now preserved from tier data files
  // Lesson challenges have templates like طباعة("...") that students complete
  // Exercise challenges have empty starterCode for students to write from scratch
  
  for (let tier = 1; tier <= 7; tier++) {
    const tierCount = challenges.filter(c => c.tier === tier).length;
    console.log('Tier', tier + ':', tierCount, 'challenges');
  }
  
  prisma.$disconnect();
}

main();
