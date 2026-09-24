process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const bcrypt = require('../server/node_modules/bcryptjs');
const prisma = new PrismaClient();

async function main() {
  const PASSWORD = 'Test1234!';

  let school = await prisma.school.findFirst({ where: { name: 'العبقري' } });
  if (!school) {
    school = await prisma.school.create({ data: { name: 'العبقري', code: 'ABQARI' } });
    console.log('Created school');
  } else {
    console.log('School exists');
  }

  const teacherEmail = 'teacher_abqari@test.com';
  let teacher = await prisma.user.findFirst({ where: { email: teacherEmail, schoolId: school.id } });
  if (!teacher) {
    const hash = await bcrypt.hash(PASSWORD, 12);
    teacher = await prisma.user.create({
      data: { name: 'مدير العبقري', email: teacherEmail, passwordHash: hash, role: 'ADMIN', schoolId: school.id, nationalId: 'T_ABQARI', isActive: true, isApproved: true, isVerified: true }
    });
    console.log('Created teacher');
  } else {
    console.log('Teacher exists');
  }

  let classroom = await prisma.classroom.findFirst({ where: { name: 'الصف الأول', teacherId: teacher.id } });
  if (!classroom) {
    classroom = await prisma.classroom.create({ data: { name: 'الصف الأول', teacherId: teacher.id, schoolId: school.id } });
    console.log('Created classroom');
  } else {
    console.log('Classroom exists');
  }

  const nationalId = 'S_ABQARI_001';
  let student = await prisma.user.findFirst({ where: { nationalId, schoolId: school.id } });
  if (!student) {
    const hash = await bcrypt.hash(PASSWORD, 12);
    student = await prisma.user.create({
      data: { name: 'طالب اختباري', nationalId, passwordHash: hash, role: 'STUDENT', schoolId: school.id, classroomId: classroom.id, isActive: true, isVerified: true, isApproved: true }
    });
    console.log('Created student');
  } else {
    console.log('Student exists');
  }

  // Ensure StudentProfile
  let profile = await prisma.studentProfile.findFirst({ where: { studentId: student.id } });
  if (!profile) {
    await prisma.studentProfile.create({ data: { studentId: student.id, totalXP: 0, currentLevel: 1 } });
    console.log('Created profile');
  }

  // Mark ALL challenges as passed
  const challenges = await prisma.challenge.findMany();
  console.log(`Total challenges: ${challenges.length}`);

  let passed = 0;
  let alreadyPassed = 0;
  for (const ch of challenges) {
    const existing = await prisma.submission.findFirst({
      where: { studentId: student.id, challengeId: ch.id }
    });
    if (!existing) {
      await prisma.submission.create({
        data: {
          studentId: student.id,
          challengeId: ch.id,
          code: '// solved',
          output: ch.expectedOutput || '',
          passed: true,
          syntaxScore: 100,
          performanceScore: 100
        }
      });
      passed++;
    } else {
      alreadyPassed++;
    }
  }

  // Update XP
  const totalXP = challenges.reduce((sum, c) => sum + (c.xpReward || c.points || 10), 0);
  await prisma.studentProfile.update({
    where: { studentId: student.id },
    data: { totalXP, currentLevel: 6 }
  });

  console.log(`\n=== RESULT ===`);
  console.log(`Student: طالب اختباري`);
  console.log(`National ID: ${nationalId}`);
  console.log(`Password: ${PASSWORD}`);
  console.log(`School: العبقري`);
  console.log(`Classroom: الصف الأول`);
  console.log(`Challenges passed: ${passed} new, ${alreadyPassed} already`);
  console.log(`Total XP: ${totalXP}`);
  console.log(`Level: 6`);
}

main().catch(console.error).finally(() => prisma.$disconnect());
