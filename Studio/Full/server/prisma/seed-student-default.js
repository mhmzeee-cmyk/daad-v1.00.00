// ====================================================
// GAP-F1 (student-only system): Default seed — school + general classroom + starter challenges
// Idempotent: only creates what is missing. Prints the public inviteCode at the end.
// Usage: npm run db:seed:student   (or: node prisma/seed-student-default.js)
// ====================================================
const { PrismaClient } = require('@prisma/client');
const bcrypt = require('bcryptjs');
const crypto = require('crypto');

const prisma = new PrismaClient();

const INVITE_CODE = 'DHO001';

const STARTER_CHALLENGES = [
  {
    title: 'أول برنامج: طباعة',
    description: 'اكتب برنامجاً يطبع كلمة مرحبا على الشاشة.',
    starterCode: '// اكتب كود الطباعة هنا\n',
    expectedOutput: 'مرحبا',
    order: 1,
  },
  {
    title: 'جمع عددين',
    description: 'اجمع العددين 2 و 3 واطبع الناتج.',
    starterCode: 'صحيح a = 2\nصحيح b = 3\n// اطبع مجموع a و b هنا\n',
    expectedOutput: '5',
    order: 2,
  },
  {
    title: 'شرط بسيط',
    description: 'إذا كانت قيمة x أكبر من 5 اطبع "كبير" وإلا اطبع "صغير".',
    starterCode: 'صحيح x = 10\n// اكتب الشرط هنا\n',
    expectedOutput: 'كبير',
    order: 3,
  },
];

async function main() {
  // 1. School
  let school = await prisma.school.findFirst();
  if (!school) {
    school = await prisma.school.create({
      data: { name: 'مدرسة ض العامة', code: 'DHA001', planType: 'SCHOOL', totalLicenses: 10000 },
    });
    console.log('Created school:', school.name, school.id);
  } else {
    console.log('Using school:', school.name, school.id);
  }

  // 2. System owner (STUDENT role — owns the general classroom row)
  let owner = await prisma.user.findFirst({ where: { email: 'system-owner@local' } });
  if (!owner) {
    const randomPassword = crypto.randomBytes(16).toString('hex');
    owner = await prisma.user.create({
      data: {
        schoolId: school.id,
        role: 'STUDENT',
        name: 'مالك النظام',
        email: 'system-owner@local',
        passwordHash: await bcrypt.hash(randomPassword, 12),
        isActive: true,
        isVerified: true,
        isApproved: true,
        isOnboarded: true,
      },
    });
    console.log('Created system owner:', owner.id);
  } else {
    console.log('Using owner:', owner.id);
  }

  // 3. General classroom (public invite code)
  let classroom = await prisma.classroom.findFirst({ where: { inviteCode: INVITE_CODE } });
  if (!classroom) {
    classroom = await prisma.classroom.create({
      data: {
        schoolId: school.id,
        teacherId: owner.id,
        name: 'الفصل العام',
        description: 'فصل عام مفتوح لجميع الطلاب',
        maxStudents: 10000,
        isActive: true,
        inviteCode: INVITE_CODE,
      },
    });
    console.log('Created classroom:', classroom.name, classroom.id);
  } else {
    console.log('Using classroom:', classroom.name, classroom.id);
  }

  // 4. Starter challenges (only if none published)
  const published = await prisma.challenge.count({ where: { published: true } });
  if (published === 0) {
    for (const c of STARTER_CHALLENGES) {
      await prisma.challenge.create({
        data: {
          title: c.title,
          description: c.description,
          starterCode: c.starterCode,
          expectedOutput: c.expectedOutput,
          dynamicOutput: '',
          requirements: '[]',
          difficulty: 'BEGINNER',
          challengeType: 'EXERCISE',
          tier: 1,
          order: c.order,
          points: 100,
          xpReward: 10,
          published: true,
          isSystem: false,
          authorId: owner.id,
          schoolId: null,
        },
      });
    }
    console.log(`Created ${STARTER_CHALLENGES.length} starter challenges`);
  } else {
    console.log(`Challenges already present: ${published} published`);
  }

  console.log('\n========================================');
  console.log(`Invite code for students: ${INVITE_CODE}`);
  console.log('========================================');
}

main()
  .catch((e) => { console.error('SEED-ERROR:', e.message); process.exit(1); })
  .finally(() => prisma.$disconnect());
