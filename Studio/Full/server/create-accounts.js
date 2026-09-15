const { PrismaClient } = require('@prisma/client');
const bcrypt = require('bcryptjs');
const crypto = require('crypto');

const prisma = new PrismaClient();

async function createTestAccounts() {
  // C3: Generate random password — never hardcoded
  const password = process.env.SEED_PASSWORD || crypto.randomBytes(16).toString('hex');
  const hash = await bcrypt.hash(password, 12);

  // Get or create school
  let school = await prisma.school.findFirst({ where: { code: 'DAA001' } });
  if (!school) {
    school = await prisma.school.create({
      data: {
        name: 'Daad Academy',
        code: 'DAA001',
        email: 'admin@daad.academy',
      },
    });
    console.log('Created school:', school.name);
  }

  // Get classrooms
  const classrooms = await prisma.classroom.findMany({ where: { schoolId: school.id } });
  const grade7 = classrooms.find(c => c.name.includes('7'));
  const grade9 = classrooms.find(c => c.name.includes('9'));

  const accounts = [
    // Teachers
    {
      email: 'ahmad@daad.academy',
      name: 'Ahmad Al-Rashid',
      role: 'TEACHER',
      nationalId: null,
      classroomId: null,
    },
    {
      email: 'teacher@daad.academy',
      name: 'Test Teacher',
      role: 'TEACHER',
      nationalId: null,
      classroomId: null,
    },
    // Students
    {
      email: 'sara@test.com',
      name: 'Sara Al-Otaibi',
      role: 'STUDENT',
      nationalId: '1099990001',
      classroomId: grade7 ? grade7.id : null,
    },
    {
      email: 'mohammed@test.com',
      name: 'Mohammed Al-Harbi',
      role: 'STUDENT',
      nationalId: '1099990002',
      classroomId: grade7 ? grade7.id : null,
    },
    {
      email: 'fatima@test.com',
      name: 'Fatima Al-Zahrani',
      role: 'STUDENT',
      nationalId: '1099990003',
      classroomId: grade9 ? grade9.id : null,
    },
    {
      email: 'ali@test.com',
      name: 'Ali Al-Mutairi',
      role: 'STUDENT',
      nationalId: '1099990004',
      classroomId: grade9 ? grade9.id : null,
    },
  ];

  for (const acc of accounts) {
    try {
      const existing = await prisma.user.findFirst({
        where: {
          schoolId: school.id,
          OR: [
            { email: acc.email },
            ...(acc.nationalId ? [{ nationalId: acc.nationalId }] : []),
          ],
        },
      });

      if (existing) {
        await prisma.user.update({
          where: { id: existing.id },
          data: { passwordHash: hash, isActive: true, isVerified: true, isApproved: true },
        });
        console.log(`Updated: ${acc.email} (${acc.role})`);
      } else {
        await prisma.user.create({
          data: {
            schoolId: school.id,
            classroomId: acc.classroomId,
            email: acc.email,
            name: acc.name,
            role: acc.role,
            nationalId: acc.nationalId,
            passwordHash: hash,
            isActive: true,
            isVerified: true,
            isApproved: true,
          },
        });
        console.log(`Created: ${acc.email} (${acc.role})`);
      }
    } catch (e) {
      console.error(`Error with ${acc.email}:`, e.message);
    }
  }

  console.log('\n=== Test Accounts ===');
  console.log('Password for all accounts:', password);
  console.log('\nTeachers:');
  console.log('  Email: ahmad@daad.academy  | Password:', password);
  console.log('  Email: teacher@daad.academy | Password:', password);
  console.log('\nStudents:');
  console.log('  Email: sara@test.com      | Password:', password);
  console.log('  Email: mohammed@test.com  | Password:', password);
  console.log('  Email: fatima@test.com    | Password:', password);
  console.log('  Email: ali@test.com       | Password:', password);

  await prisma.$disconnect();
}

createTestAccounts().catch(e => {
  console.error(e);
  prisma.$disconnect();
});
