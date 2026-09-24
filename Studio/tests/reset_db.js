const { PrismaClient } = require('../server/node_modules/@prisma/client');
const prisma = new PrismaClient();

async function main() {
  console.log('=== RESETTING DATABASE ===');

  // Delete in order (respect foreign keys)
  await prisma.challengeVerification.deleteMany();
  await prisma.securityAlert.deleteMany();
  await prisma.studentAssessmentResult.deleteMany();
  await prisma.assessment.deleteMany();
  await prisma.studentSubmission.deleteMany();
  await prisma.submission.deleteMany();
  await prisma.userAchievement.deleteMany();
  await prisma.dailyXP.deleteMany();
  await prisma.studentProfile.deleteMany();
  await prisma.cloudWorkspace.deleteMany();
  await prisma.activityLog.deleteMany();
  await prisma.loginLog.deleteMany();
  await prisma.attendance.deleteMany();
  await prisma.project.deleteMany();
  await prisma.invitation.deleteMany();
  await prisma.challengeTier.deleteMany();
  await prisma.user.deleteMany();
  await prisma.classroom.deleteMany();
  await prisma.student.deleteMany();
  await prisma.lesson.deleteMany();
  await prisma.course.deleteMany();
  await prisma.school.deleteMany();

  console.log('All data deleted.');

  // Verify
  const counts = {
    users: await prisma.user.count(),
    schools: await prisma.school.count(),
    submissions: await prisma.submission.count(),
    challenges: await prisma.challenge.count(),
  };
  console.log('After reset:', counts);
  console.log('Challenges preserved:', counts.challenges);
}

main().catch(console.error).finally(() => prisma.$disconnect());
