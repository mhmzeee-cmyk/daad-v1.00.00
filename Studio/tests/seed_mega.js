process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
process.env.JWT_SECRET = 'dhad-studio-dev-secret-key-change-in-production';
process.env.HMAC_SECRET = 'dhad-studio-hmac-secret-2026-production';

const { PrismaClient } = require('../server/node_modules/@prisma/client');
const bcrypt = require('../server/node_modules/bcryptjs');
const prisma = new PrismaClient();
const HASH = '$2a$12$7a1psQSDRx798tL2W.WtZeNsQY7B7Sf3Xk3iFG0ukSWnrDfKbYsY2';

const SCHOOLS = [
  'School_A','School_B','School_C','School_D','School_E',
  'School_F','School_G','School_H','School_I','School_J'
];

const NAMES = [
  'Mohammed','Ahmed','Ali','Hassan','Hussein','Khalid','Omar','Youssef','Ibrahim','Ismail',
  'Uthman','Zakaria','Yahya','Musa','Sulaiman','Adam','Shams','Noor','Qamar','Saud',
  'Rashid','Fahd','Sultan','Abdullah','Abdulrahman','Tariq','Majed','Yasser','Bilal','Hatem',
  'Jamal','Kareem','Adel','Nasser','Walid','Hisham','Rami','Munir','Jassem','Hani',
  'Amir','Ayman','Tamer','Samir','Imad','Fouad','Ghazi','Hazem','Raeed','Shady',
  'Fatima','Khadija','Aisha','Maryam','Sara','Layla','Noora','Hind','Rim','Dina',
  'Mona','Rana','Dana','Lian','Haya','Lina','Mira','Samar','Huda','Bothina',
  'Reem','Jana','Dana','Aya','Yasmin','Malak','Hoor','Shahd','Lamia','Nada',
  'Amani','Rasha','Sumayya','Huda','Najla','Abla','Khawla','Salma','Manal','Arwa',
  'Rana','Lina','Hiba','Dalia','Sahar','Nadia','Amal','Nabih','Bassam','Kazim',
];

async function main() {
  console.log('=== MEGA SEED START ===');

  // Create a dummy user for challenge author
  // Create a dummy school first
  const dummySchool = await prisma.school.create({
    data: {
      name: 'System_School', code: 'SYS',
      email: 'system@dhad.com', totalLicenses: 9999,
    }
  });
  const dummy = await prisma.user.create({
    data: {
      schoolId: dummySchool.id, role: 'ADMIN', name: 'System', email: 'system@dhad.com',
      passwordHash: HASH, isActive: true, isVerified: true, isApproved: true
    }
  });

  // Create 150 challenges across 5 tiers
  console.log('Creating 150 challenges...');
  const expectedOutputs = ['5','10','15','25','30','55','100','1000','42','720','9','3.14','0','YES','1','7','20','50','300','100'];
  const codes = ['var x = 5\nprint(x)','print(5+5)','print(15)','if (5>3) { print("YES") }','for (var i=0;i<10;i++) { print(i) }'];
  let chCount = 0;
  const challengeIds = [];

  for (let tier = 1; tier <= 5; tier++) {
    const counts = [24, 28, 32, 30, 36];
    const diffs = ['BEGINNER','INTERMEDIATE','INTERMEDIATE','ADVANCED','EXPERT'];
    for (let i = 0; i < counts[tier-1]; i++) {
      const ch = await prisma.challenge.create({
        data: {
          authorId: dummy.id,
          title: 'Challenge_T' + tier + '_' + (i+1),
          description: 'Challenge tier ' + tier + ' number ' + (i+1),
          starterCode: '',
          expectedOutput: expectedOutputs[i % expectedOutputs.length],
          dynamicOutput: '',
          requirements: '[]',
          difficulty: diffs[tier-1],
          challengeType: 'BUG_FIX',
          published: true,
          tier: tier,
          order: i + 1,
          points: 10 * tier,
          xpReward: 5 * tier,
          isSystem: true,
        }
      });
      challengeIds.push(ch.id);
      chCount++;
    }
  }
  console.log('Challenges created:', chCount);

  // Create 10 schools
  console.log('\nCreating 10 schools...');
  const allTeachers = [];
  const allClassrooms = [];

  for (let s = 0; s < 10; s++) {
    const school = await prisma.school.create({
      data: {
        name: SCHOOLS[s],
        code: 'S' + String(s+1).padStart(2,'0'),
        email: 'admin@s' + (s+1) + '.com',
        totalLicenses: 2000,
      }
    });

    // 10 teachers per school = 100 total
    for (let t = 0; t < 10; t++) {
      const teacher = await prisma.user.create({
        data: {
          schoolId: school.id, role: 'TEACHER',
          name: 'Teacher_' + (s+1) + '_' + (t+1),
          email: 'teacher_' + (s+1) + '_' + (t+1) + '@test.com',
          passwordHash: HASH, isActive: true, isVerified: true, isApproved: true,
        }
      });
      allTeachers.push(teacher);

      const classroom = await prisma.classroom.create({
        data: {
          schoolId: school.id, teacherId: teacher.id,
          name: 'Class_' + (s+1) + '_' + (t+1),
          subject: ['programming','computer','tech'][t % 3],
          gradeLevel: String(7 + (t % 6)),
          maxStudents: 150,
        }
      });
      allClassrooms.push(classroom);

      // 100 students per classroom = 1000 per school = 10000 total
      for (let st = 0; st < 100; st++) {
        const globalIdx = s * 1000 + t * 100 + st;
        const nameIdx = globalIdx % NAMES.length;
        const natId = String(1000000000 + globalIdx);

        await prisma.user.create({
          data: {
            schoolId: school.id, classroomId: classroom.id,
            role: 'STUDENT',
            name: NAMES[nameIdx] + '_' + (globalIdx + 1),
            nationalId: natId,
            email: 's' + globalIdx + '@test.com',
            passwordHash: HASH,
            isActive: true, isVerified: true, isApproved: true,
            studentProfile: { create: { totalXP: 0, currentLevel: 1 } }
          }
        }).catch(() => {});
      }
    }
    const sc = await prisma.user.count({ where: { schoolId: school.id, role: 'STUDENT' } });
    console.log('  School ' + (s+1) + ': 10 teachers, ' + sc + ' students');
  }

  // Create 100 assessments
  console.log('\nCreating 100 assessments...');
  for (let i = 0; i < 100; i++) {
    const cls = allClassrooms[i % allClassrooms.length];
    const now = new Date();
    const future = new Date(now.getTime() + 3600000);
    await prisma.assessment.create({
      data: {
        classroomId: cls.id,
        title: 'Exam_' + (i+1),
        description: 'Assessment number ' + (i+1),
        assessmentType: i % 2 === 0 ? 'CODING' : 'MULTIPLE_CHOICE',
        allowedTime: 60,
        startTime: now,
        endTime: future,
        totalPoints: 100,
        passingScore: 60,
        isPublished: true,
        lockdownMode: true,
        challengeIds: JSON.stringify(challengeIds.slice(0, 3)),
      }
    });
  }

  // Summary
  console.log('\n=== FINAL SUMMARY ===');
  console.log('Schools:', await prisma.school.count());
  console.log('Users:', await prisma.user.count());
  console.log('Students:', await prisma.user.count({ where: { role: 'STUDENT' } }));
  console.log('Teachers:', await prisma.user.count({ where: { role: 'TEACHER' } }));
  console.log('Classrooms:', await prisma.classroom.count());
  console.log('Challenges:', await prisma.challenge.count());
  console.log('Assessments:', await prisma.assessment.count());
  console.log('\nLogin: nationalId (1000000000-100099999) / Test1234!');
}

main().catch(console.error).finally(() => prisma.$disconnect());
