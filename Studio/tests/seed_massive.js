const { PrismaClient } = require('../server/node_modules/@prisma/client');
const bcrypt = require('../server/node_modules/bcryptjs');

const prisma = new PrismaClient();
const BCRYPT_ROUNDS = 12;
const PASSWORD = 'Test1234!';
const SCHOOLS_COUNT = 10;
const STUDENTS_PER_SCHOOL = 100;

const SCHOOL_NAMES = [
  'أكاديمية البرمجة', 'مدرسة المستقبل', 'أكاديمية التقنية', 'مدرسة النور',
  'أكاديمية المعرفة', 'مدرسة الإبداع', 'أكاديمية العلم', 'مدرسة التكنولوجيا',
  'أكاديمية الابتكار', 'مدرسة التعليم'
];

const STUDENT_NAMES_AR = [
  'محمد', 'أحمد', 'علي', 'حسن', 'حسين', 'خالد', 'عمر', 'يوسف', 'إبراهيم', 'إسماعيل',
  'عثمان', 'زكريا', 'يحيى', 'موسى', 'هارون', 'سليمان', 'داود', 'إسحاق', 'يعقوب', 'نوح',
  'آدم', 'شيث', 'أدم', 'ابليس', 'ibilis', 'شمس', 'نور', 'قمر', 'كوكب', 'شمس',
  'فاطمة', 'خديجة', 'عائشة', 'مريم', 'سارة', 'ليلى', 'نورة', 'هند', 'ريم', 'دينا',
  'سعود', 'راشد', 'فهد', 'سلطان', 'عبدالله', 'عبدالرحمن', 'طارق', 'ماجد', 'ياسر', 'басил',
  'طارق', 'منير', 'جمال', 'كريم', 'عادل', 'ناصر', 'طارق', 'وليد', 'هشام', 'رامي',
  'بلال', 'حاتم', 'جاسم', 'ماجد', 'وليد', 'هاني', 'خالد', 'أمير', 'فهد', 'سلطان',
  'منى', 'رنا', 'دانا', 'ليان', 'جوانا', 'هيا', 'لينا', 'ميرا', 'سمر', 'هدى',
  'أحمد', 'محمد', 'علي', 'خالد', 'عمر', 'يوسف', 'إبراهيم', 'حسن', 'حسين', 'عثمان',
  'سارة', 'نورة', 'ريم', 'دينا', 'هند', 'ليلى', 'مريم', 'خديجة', 'فاطمة', 'عائشة',
];

async function main() {
  console.log('Seeding 10 schools × 100 students...\n');

  const passwordHash = await bcrypt.hash(PASSWORD, BCRYPT_ROUNDS);

  for (let s = 0; s < SCHOOLS_COUNT; s++) {
    console.log(`School ${s + 1}: ${SCHOOL_NAMES[s]}`);

    // Create school
    const school = await prisma.school.create({
      data: {
        name: SCHOOL_NAMES[s],
        code: `SCH${String(s + 1).padStart(2, '0')}`,
        email: `admin@school${s + 1}.com`,
        totalLicenses: 200,
      }
    });

    // Create admin
    const admin = await prisma.user.create({
      data: {
        schoolId: school.id,
        role: 'ADMIN',
        name: `مدير ${SCHOOL_NAMES[s]}`,
        email: `admin@school${s + 1}.com`,
        passwordHash,
        isActive: true,
        isVerified: true,
        isApproved: true,
      }
    });

    // Create teacher
    const teacher = await prisma.user.create({
      data: {
        schoolId: school.id,
        role: 'TEACHER',
        name: `معلم ${SCHOOL_NAMES[s]}`,
        email: `teacher@school${s + 1}.com`,
        passwordHash,
        isActive: true,
        isVerified: true,
        isApproved: true,
      }
    });

    // Create classroom
    const classroom = await prisma.classroom.create({
      data: {
        schoolId: school.id,
        teacherId: teacher.id,
        name: `قسم البرمجة ${s + 1}`,
        subject: 'برمجة',
        gradeLevel: '7',
        maxStudents: 150,
      }
    });

    // Create 100 students
    let created = 0;
    for (let i = 0; i < STUDENTS_PER_SCHOOL; i++) {
      const nationalId = String(1000000000 + s * 1000 + i);
      const nameIdx = (s * 100 + i) % STUDENT_NAMES_AR.length;
      const studentName = STUDENT_NAMES_AR[nameIdx] + '_' + (s + 1) + '_' + (i + 1);

      try {
        await prisma.user.create({
          data: {
            schoolId: school.id,
            classroomId: classroom.id,
            role: 'STUDENT',
            name: studentName,
            nationalId,
            email: `student${i}@school${s + 1}.com`,
            passwordHash,
            isActive: true,
            isVerified: true,
            isApproved: true,
            studentProfile: {
              create: {
                totalXP: 0,
                currentLevel: 1,
              }
            }
          }
        });
        created++;
      } catch (e) {
        // Skip duplicate
      }
    }
    console.log(`  Created: admin, teacher, classroom, ${created} students`);
  }

  // Get total counts
  const userCount = await prisma.user.count();
  const schoolCount = await prisma.school.count();
  console.log(`\nTotal: ${schoolCount} schools, ${userCount} users`);
  console.log(`\nLogin credentials:`);
  console.log(`  Students: nationalId (1000000000-1000099999) / ${PASSWORD}`);
  console.log(`  Teachers: teacher@school{1-10}.com / ${PASSWORD}`);
  console.log(`  Admins:   admin@school{1-10}.com / ${PASSWORD}`);
}

main()
  .catch(console.error)
  .finally(() => prisma.$disconnect());
