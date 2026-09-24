process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
process.env.JWT_SECRET = 'dhad-studio-dev-secret-key-change-in-production';
process.env.HMAC_SECRET = 'dhad-studio-hmac-secret-2026-production';

const { PrismaClient } = require('../server/node_modules/@prisma/client');
const prisma = new PrismaClient();

async function main() {
  console.log('=== DATABASE FINAL STATE ===\n');

  const schools = await prisma.school.count();
  const users = await prisma.user.count();
  const students = await prisma.user.count({ where: { role: 'STUDENT' } });
  const teachers = await prisma.user.count({ where: { role: 'TEACHER' } });
  const classrooms = await prisma.classroom.count();
  const challenges = await prisma.challenge.count();
  const assessments = await prisma.assessment.count();
  const submissions = await prisma.submission.count();
  const passed = await prisma.submission.count({ where: { passed: true } });
  const failed = await prisma.submission.count({ where: { passed: false } });
  const loginLogs = await prisma.loginLog.count();
  const loginsOK = await prisma.loginLog.count({ where: { success: true } });
  const loginsFail = await prisma.loginLog.count({ where: { success: false } });
  const profiles = await prisma.studentProfile.count();
  const xp = await prisma.studentProfile.aggregate({ _sum: { totalXP: true } });
  const assessmentResults = await prisma.studentAssessmentResult.count();
  const workspaces = await prisma.cloudWorkspace.count();
  const securityAlerts = await prisma.securityAlert.count();
  const activityLogs = await prisma.activityLog.count();

  console.log('Schools:           ', schools);
  console.log('Users:             ', users);
  console.log('  Students:        ', students);
  console.log('  Teachers:        ', teachers);
  console.log('Classrooms:        ', classrooms);
  console.log('Challenges:        ', challenges);
  console.log('Assessments:       ', assessments);
  console.log('');
  console.log('--- Activity ---');
  console.log('Submissions:       ', submissions, '(Passed:', passed, ', Failed:', failed, ')');
  console.log('Pass rate:         ', submissions > 0 ? Math.round(passed / submissions * 100) : 0, '%');
  console.log('Login Logs:        ', loginLogs, '(OK:', loginsOK, ', Fail:', loginsFail, ')');
  console.log('Student Profiles:  ', profiles);
  console.log('Total XP Awarded:  ', xp._sum.totalXP || 0);
  console.log('Assessment Results:', assessmentResults);
  console.log('Cloud Workspaces:  ', workspaces);
  console.log('Security Alerts:   ', securityAlerts);
  console.log('Activity Logs:     ', activityLogs);

  // Top XP students
  const topXP = await prisma.studentProfile.findMany({
    take: 10, orderBy: { totalXP: 'desc' },
    include: { student: { select: { name: true, schoolId: true } } }
  });
  console.log('\n--- Top 10 Students by XP ---');
  for (const p of topXP) {
    console.log('  ' + p.student.name + ' - ' + p.totalXP + ' XP, Level ' + p.currentLevel);
  }

  // Recent submissions
  const recent = await prisma.submission.findMany({
    take: 10, orderBy: { createdAt: 'desc' },
    include: { challenge: { select: { title: true } }, student: { select: { name: true } } }
  });
  console.log('\n--- Recent 10 Submissions ---');
  for (const s of recent) {
    console.log('  ' + s.student.name + ' -> ' + (s.challenge ? s.challenge.title : 'N/A') + ' [' + (s.passed ? 'PASS' : 'FAIL') + ']');
  }

  // Assessments
  const assessList = await prisma.assessment.findMany({
    take: 10, include: { classroom: { select: { name: true } } }
  });
  console.log('\n--- Assessments (first 10) ---');
  for (const a of assessList) {
    console.log('  ' + a.title + ' - ' + (a.classroom ? a.classroom.name : 'N/A') + ' [' + a.assessmentType + ']');
  }

  // Errors
  const errorSubs = await prisma.submission.findMany({ where: { passed: false }, take: 5, include: { challenge: { select: { title: true } }, student: { select: { name: true } } } });
  console.log('\n--- Failed Submissions (sample) ---');
  for (const s of errorSubs) {
    console.log('  ' + s.student.name + ' -> ' + (s.challenge ? s.challenge.title : 'N/A') + ' output: "' + s.output.substring(0, 30) + '"');
  }
}

main().catch(console.error).finally(() => prisma.$disconnect());
