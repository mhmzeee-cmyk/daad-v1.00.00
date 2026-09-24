const { PrismaClient } = require('../server/node_modules/@prisma/client');
process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const prisma = new PrismaClient();

async function main() {
  const submissions = await prisma.submission.count();
  const passed = await prisma.submission.count({ where: { passed: true } });
  const failed = await prisma.submission.count({ where: { passed: false } });
  const assessments = await prisma.assessment.count();
  const assessmentResults = await prisma.studentAssessmentResult.count();
  const users = await prisma.user.count();
  const students = await prisma.user.count({ where: { role: 'STUDENT' } });
  const teachers = await prisma.user.count({ where: { role: 'TEACHER' } });
  const schools = await prisma.school.count();
  const classrooms = await prisma.classroom.count();
  const loginLogs = await prisma.loginLog.count();
  const failedLogins = await prisma.loginLog.count({ where: { success: false } });
  const successLogins = await prisma.loginLog.count({ where: { success: true } });
  const xpProfiles = await prisma.studentProfile.count();
  const totalXP = await prisma.studentProfile.aggregate({ _sum: { totalXP: true } });

  console.log('=== Database Stats ===');
  console.log('Schools:', schools);
  console.log('Users:', users, '(Students:', students, ', Teachers:', teachers, ')');
  console.log('Classrooms:', classrooms);
  console.log('');
  console.log('--- Login Logs ---');
  console.log('Total logins:', loginLogs, '(Success:', successLogins, ', Failed:', failedLogins, ')');
  console.log('');
  console.log('--- Submissions ---');
  console.log('Total:', submissions, '(Passed:', passed, ', Failed:', failed, ')');
  console.log('Pass rate:', submissions > 0 ? Math.round(passed / submissions * 100) : 0, '%');
  console.log('');
  console.log('--- Assessments ---');
  console.log('Assessments created:', assessments);
  console.log('Assessment results:', assessmentResults);
  console.log('');
  console.log('--- XP ---');
  console.log('Profiles with XP:', xpProfiles);
  console.log('Total XP awarded:', totalXP._sum.totalXP || 0);

  // Show some recent submissions
  const recent = await prisma.submission.findMany({
    take: 10,
    orderBy: { createdAt: 'desc' },
    include: { challenge: { select: { title: true } }, student: { select: { name: true } } }
  });
  console.log('\n=== Recent 10 Submissions ===');
  recent.forEach(s => {
    console.log(' ', s.student.name, '-', s.challenge ? s.challenge.title.substring(0, 30) : 'N/A', '-', s.passed ? 'PASSED' : 'FAILED');
  });

  // Show assessments
  const assessList = await prisma.assessment.findMany({
    take: 10,
    include: { classroom: { select: { name: true } } }
  });
  console.log('\n=== Assessments Created ===');
  if (assessList.length === 0) {
    console.log('  None');
  } else {
    assessList.forEach(a => {
      console.log(' ', a.title.substring(0, 30), '-', a.classroom ? a.classroom.name : 'N/A', '-', a.assessmentType);
    });
  }

  // Students with most XP
  const topXP = await prisma.studentProfile.findMany({
    take: 10,
    orderBy: { totalXP: 'desc' },
    include: { student: { select: { name: true } } }
  });
  console.log('\n=== Top 10 Students by XP ===');
  topXP.forEach(p => {
    console.log(' ', p.student.name, '-', p.totalXP, 'XP, Level', p.currentLevel);
  });
}

main().catch(console.error).finally(() => prisma.$disconnect());
