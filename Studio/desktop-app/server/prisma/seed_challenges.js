const { PrismaClient } = require('@prisma/client');
const prisma = new PrismaClient();

async function seed() {
  const teacher = await prisma.user.findFirst({ where: { email: 'ahmed@alnoor.edu' } });
  const student = await prisma.user.findFirst({ where: { nationalId: '1098765401' } });

  const c1 = await prisma.challenge.create({ data: { authorId: teacher.id, title: '\u0637\u0628\u0627\u0639\u0629 \u0627\u0644\u062A\u0631\u062D\u064A\u0628', description: '\u0627\u0643\u062A\u0628 \u0628\u0631\u0646\u0627\u0645\u062C \u064A\u0637\u0628\u0639 \u0627\u0644\u062A\u0631\u062D\u064A\u0628 \u0627\u0644\u0635\u062D\u064A\u062D', starterCode: 'print("Hello World")', expectedOutput: '\u0623\u0647\u0644\u0627\u064B \u0639\u0644\u064A\u0643\u0645', difficulty: 'BEGINNER', published: true }});
  const c2 = await prisma.challenge.create({ data: { authorId: teacher.id, title: '\u062D\u0644\u0642\u0629 \u062A\u0643\u0631\u0627\u0631', description: '\u0627\u0633\u062A\u062E\u062F\u0645 \u062D\u0644\u0642\u0629 \u062A\u0643\u0631\u0627\u0631 \u0644\u062D\u0633\u0627\u0628 \u0645\u062C\u0645\u0648\u0639\u0629 \u0627\u0644\u0623\u0631\u0642\u0627\u0645 \u0645\u0646 1 \u0625\u0644\u0649 10', starterCode: 'for i from 1 to 10:\n    print(i)', expectedOutput: '55', difficulty: 'INTERMEDIATE', published: true }});
  const c3 = await prisma.challenge.create({ data: { authorId: teacher.id, title: '\u062D\u0633\u0627\u0628 \u0645\u0633\u0627\u062D\u0629 \u0645\u0633\u062A\u0637\u064A\u0644', description: '\u0627\u0643\u062A\u0628 \u0628\u0631\u0646\u0627\u0645\u062C \u064A\u062D\u0633\u0628 \u0645\u0633\u0627\u062D\u0629 \u0645\u0633\u062A\u0637\u064A\u0644', starterCode: 'def area(width, height):\n    return width * height', expectedOutput: 'area(5,3)=15', difficulty: 'INTERMEDIATE', published: true }});

  await prisma.submission.create({ data: { challengeId: c1.id, studentId: student.id, code: 'print("Hello World")', language: 'daad', passed: true, output: '\u0623\u0647\u0644\u0627\u064B \u0639\u0644\u064A\u0643\u0645' }});
  await prisma.submission.create({ data: { challengeId: c2.id, studentId: student.id, code: 'for i from 1 to 10:\n    print(i)', language: 'daad', passed: true, output: '55' }});
  await prisma.submission.create({ data: { challengeId: c3.id, studentId: student.id, code: 'def area(w,h):\n    return w*h', language: 'daad', passed: true, output: '15' }});

  console.log('[Seed] 3 challenges + 3 submissions created on Supabase');
  await prisma.$disconnect();
}
seed().catch(e => { console.error(e); process.exit(1); });
