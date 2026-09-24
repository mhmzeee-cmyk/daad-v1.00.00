// ====================================================
// Dhad Studio - Seed Data for Load Testing
// ====================================================
// Generates: 10 schools, 500 students, 50 teachers, 5 admins,
//            50 classrooms, 5 courses, 100 challenges, 20 assessments
// ====================================================

const { PrismaClient } = require('@prisma/client');
const bcrypt = require('bcrypt');
const crypto = require('crypto');

const prisma = new PrismaClient();
const PASSWORD_HASH = null; // computed lazily

async function hashPassword() {
  return bcrypt.hash('Test1234!', 10);
}

function cuid() {
  return crypto.randomBytes(12).toString('base64url').replace(/[^a-z0-9]/gi, '').substring(0, 25);
}

const SCHOOL_NAMES = [
  'مدرسة النور', 'مدرسة المعرفة', 'مدرسة التميز', 'مدرسة الإبداع', 'مدرسة المستقبل',
  'مدرسة الرحمة', 'مدرسة السلام', 'مدرسة الأمل', 'مدرسة الفجر', 'مدرسة البيان'
];

const TEACHER_NAMES = [
  'أحمدمحمد', 'فاطمةعلي', 'خالدعمر', 'نورةسالم', 'عبداللهحسن',
  'سارةإسماعيل', 'عمربكر', 'ليلىأحمد', 'ياسرعبدالرحمن', 'هدىعثمان',
  'محمدإمام', 'رناقصي', 'حسنistributor', 'مريمكريم', 'طارقشريف',
  'ناديناصر', 'وليدفؤاد', 'آينور', 'ساميرضwan', 'منىعبدالعزيز',
  'عثمانعلي', 'زينبحسن', 'بلالقاسم', 'رحيمنور', 'أروى Saleh',
  'كريمabdullah', 'داناحسين', 'ماجد سعيد', 'رنا الصالح', 'أحمد عمر',
  'فاطمة الزهراء', 'علي حسن', 'مريم يوسف', 'عمر الخطيب', 'سارة أحمد',
  'خالد الراشد', 'نورة القحطاني', 'يوسف المنصور', 'هدى الشمري', 'عبدالرحمن السعيد',
  'أسامة المرتضى', 'رنا الحربي', 'سلطان القحطاني', 'منال العتيبي', 'فهد الشمري',
  'عائشة البكري', 'طارق العمري', 'ليلى الدوسري', 'ياسر القحطاني', 'أحمد الراشد'
];

const STUDENT_FIRST = ['محمد', 'أحمد', 'خالد', 'عمر', 'يوسف', 'إبراهيم', 'عثمان', 'بلال', 'حسن', 'طارق', 'سارة', 'فاطمة', 'نورة', 'ريم', 'هدى', 'ليلى', 'مريم', 'دانا', 'آينور', 'رنا', 'أحمد', 'علي', 'حسن', 'عمر', 'خالد', 'محمد', 'أحمد', 'يوسف', 'إبراهيم', 'بلال', 'سارة', 'فاطمة', 'نورة', 'ريم', 'هدى', 'ليلى', 'مريم', 'دانا', 'آينور', 'رنا', 'أحمد', 'علي', 'حسن', 'عمر', 'خالد', 'محمد', 'أحمد', 'يوسف', 'إبراهيم', 'بلال'];
const STUDENT_LAST = ['بن علي', 'بن محمد', 'بن أحمد', 'بن خالد', 'بن عمر', 'بنت أحمد', 'بنت محمد', 'بنت علي', 'بنت خالد', 'بن يوسف', 'بن إبراهيم', 'بن عثمان', 'بنت فاطمة', 'بنت نورة', 'بن حسن', 'بن طارق', 'بنت سارة', 'بنت ريم', 'بن ريان', 'بنت لينا', 'بن ياسر', 'بن سعيد', 'بنت هند', 'بن فيصل', 'بنت عبير', 'بن ماجد', 'بنت لمى', 'بن عبدالعزيز', 'بنت جواهر', 'بن نواف'];

const COURSE_DATA = [
  { title: 'أساسيات الضاد', category: 'MAIN', lessons: 10 },
  { title: 'الحلقات والشروط', category: 'MAIN', lessons: 10 },
  { title: 'الدوال والمُعاملات', category: 'MAIN', lessons: 10 },
  { title: 'الفئات والكائنات', category: 'MAIN', lessons: 10 },
  { title: 'مشاريع تطبيقية', category: 'PROJECTS', lessons: 10 },
];

const DIFFICULTIES = ['BEGINNER', 'BEGINNER', 'BEGINNER', 'INTERMEDIATE', 'INTERMEDIATE', 'ADVANCED'];
const CHALLENGE_TYPES = ['LESSON', 'EXERCISE'];

function rand(arr) { return arr[Math.floor(Math.random() * arr.length)]; }
function randInt(min, max) { return Math.floor(Math.random() * (max - min + 1)) + min; }

async function main() {
  console.log('🌱 Starting seed...');
  const startTime = Date.now();

  const password = await hashPassword();

  // Clean existing data (order matters for foreign keys)
  console.log('  🗑️  Cleaning existing data...');
  await prisma.challengeVerification.deleteMany();
  await prisma.studentAssessmentResult.deleteMany();
  await prisma.assessment.deleteMany();
  await prisma.submission.deleteMany();
  await prisma.studentSubmission.deleteMany();
  await prisma.cloudWorkspace.deleteMany();
  await prisma.challengeTier.deleteMany();
  await prisma.challenge.deleteMany();
  await prisma.project.deleteMany();
  await prisma.studentCourseEnrollment.deleteMany();
  await prisma.dailyXP.deleteMany();
  await prisma.userAchievement.deleteMany();
  await prisma.achievement.deleteMany();
  await prisma.studentProfile.deleteMany();
  await prisma.classroom.deleteMany();
  await prisma.course.deleteMany();
  await prisma.lesson.deleteMany();
  await prisma.activityLog.deleteMany();
  await prisma.loginLog.deleteMany();
  await prisma.attendance.deleteMany();
  await prisma.invitation.deleteMany();
  await prisma.securityAlert.deleteMany();
  await prisma.user.deleteMany();
  await prisma.student.deleteMany();
  await prisma.school.deleteMany();

  // 1. Schools (10)
  console.log('  🏫 Creating 10 schools...');
  const schools = [];
  for (let i = 0; i < 10; i++) {
    const school = await prisma.school.create({
      data: {
        id: `school_${i + 1}`,
        name: SCHOOL_NAMES[i],
        code: `SCH${String(i + 1).padStart(3, '0')}`,
        email: `admin@school${i + 1}.edu`,
        totalLicenses: 100,
        usedLicenses: 55,
      }
    });
    schools.push(school);
  }

  // 2. Teachers (50 — 5 per school)
  console.log('  👨‍🏫 Creating 50 teachers...');
  const teachers = [];
  for (let i = 0; i < 50; i++) {
    const schoolIdx = Math.floor(i / 5);
    const teacher = await prisma.user.create({
      data: {
        id: `teacher_${i + 1}`,
        schoolId: schools[schoolIdx].id,
        role: 'TEACHER',
        name: TEACHER_NAMES[i],
        email: `teacher_${i + 1}@test.com`,
        nationalId: `T${String(i + 1).padStart(4, '0')}`,
        passwordHash: password,
        isActive: true,
        isVerified: true,
        isApproved: true,
        isOnboarded: true,
      }
    });
    teachers.push(teacher);
  }

  // 3. Admins (5)
  console.log('  👨‍💼 Creating 5 admins...');
  for (let i = 0; i < 5; i++) {
    await prisma.user.create({
      data: {
        id: `admin_${i + 1}`,
        schoolId: schools[i].id,
        role: 'ADMIN',
        name: `-admin ${i + 1}`,
        email: `admin_${i + 1}@test.com`,
        nationalId: `A${String(i + 1).padStart(4, '0')}`,
        passwordHash: password,
        isActive: true,
        isVerified: true,
        isApproved: true,
        isOnboarded: true,
      }
    });
  }

  // 4. Classrooms (50 — 5 per school, each teacher has 1)
  console.log('  🏠 Creating 50 classrooms...');
  const classrooms = [];
  for (let i = 0; i < 50; i++) {
    const schoolIdx = Math.floor(i / 5);
    const classroom = await prisma.classroom.create({
      data: {
        id: `classroom_${i + 1}`,
        schoolId: schools[schoolIdx].id,
        teacherId: teachers[i].id,
        name: `الفصل ${String.fromCharCode(65 + (i % 5))} - ${schools[schoolIdx].name}`,
        gradeLevel: `${randInt(7, 12)}`,
        subject: 'برمجة الضاد',
        maxStudents: 40,
      }
    });
    classrooms.push(classroom);
  }

  // 5. Students (500 — 10 per classroom)
  console.log('  🎓 Creating 500 students...');
  const students = [];
  for (let i = 0; i < 500; i++) {
    const classIdx = Math.floor(i / 10);
    const first = rand(STUDENT_FIRST);
    const last = rand(STUDENT_LAST);
    const student = await prisma.user.create({
      data: {
        id: `student_${i + 1}`,
        schoolId: classrooms[classIdx].schoolId,
        classroomId: classrooms[classIdx].id,
        role: 'STUDENT',
        name: `${first} ${last}`,
        email: `student_${i + 1}@test.com`,
        nationalId: `S${String(i + 1).padStart(4, '0')}`,
        passwordHash: password,
        isActive: true,
        isVerified: true,
        isApproved: true,
        isOnboarded: true,
      }
    });
    students.push(student);

    // Create student record
    await prisma.student.create({
      data: {
        id: `student_rec_${i + 1}`,
        schoolId: classrooms[classIdx].schoolId,
        gradeLevel: classrooms[classIdx].gradeLevel,
        status: 'ACTIVE',
      }
    });
  }

  // 6. Courses (5)
  console.log('  📚 Creating 5 courses...');
  const courses = [];
  for (let i = 0; i < 5; i++) {
    const course = await prisma.course.create({
      data: {
        id: `course_${i + 1}`,
        schoolId: schools[0].id,
        title: COURSE_DATA[i].title,
        description: `دورة ${COURSE_DATA[i].title} — تعلم البرمجة بالضاد`,
        category: COURSE_DATA[i].category,
        order: i + 1,
        published: true,
      }
    });
    courses.push(course);
  }

  // 7. Challenges (100 — 20 per course)
  console.log('  🧩 Creating 100 challenges...');
  const challenges = [];
  for (let i = 0; i < 100; i++) {
    const courseIdx = Math.floor(i / 20);
    const diff = rand(DIFFICULTIES);
    const challenge = await prisma.challenge.create({
      data: {
        id: `challenge_${i + 1}`,
        authorId: teachers[courseIdx * 5].id,
        title: `تحدي ${i + 1} — ${COURSE_DATA[courseIdx].title}`,
        description: `حل هذا التحدي usando مهاراتك في ${COURSE_DATA[courseIdx].title}`,
        starterCode: '// اكتب كودك هنا',
        expectedOutput: `output_${i + 1}`,
        difficulty: diff,
        challengeType: rand(CHALLENGE_TYPES),
        published: true,
        tier: Math.ceil((i % 20) / 4),
        order: (i % 20) + 1,
        points: diff === 'BEGINNER' ? 100 : diff === 'INTERMEDIATE' ? 200 : 300,
        xpReward: diff === 'BEGINNER' ? 50 : diff === 'INTERMEDIATE' ? 100 : 150,
        isSystem: false,
        schoolId: schools[0].id,
        courseId: courses[courseIdx].id,
      }
    });
    challenges.push(challenge);
  }

  // 8. Enroll all students in all courses
  console.log('  📋 Enrolling students in courses...');
  const enrollmentValues = [];
  for (let s = 0; s < 500; s++) {
    for (let c = 0; c < 5; c++) {
      const eid = `enroll_${s}_${c}`;
      enrollmentValues.push(`('${eid}','${students[s].id}','${courses[c].id}',datetime('now'),NULL)`);
    }
  }
  // Batch insert in chunks of 500
  for (let i = 0; i < enrollmentValues.length; i += 500) {
    const chunk = enrollmentValues.slice(i, i + 500);
    await prisma.$executeRawUnsafe(
      `INSERT OR IGNORE INTO StudentCourseEnrollment (id, studentId, courseId, enrolledAt, completedAt) VALUES ${chunk.join(',')}`
    );
  }

  // 9. Assessments (20 — 4 per classroom that has students)
  console.log('  📝 Creating 20 assessments...');
  const now = new Date();
  const assessments = [];
  for (let i = 0; i < 20; i++) {
    const classIdx = i % 50;
    const assessment = await prisma.assessment.create({
      data: {
        id: `assessment_${i + 1}`,
        classroomId: classrooms[classIdx].id,
        title: `اختبار ${i + 1} — ${COURSE_DATA[i % 5].title}`,
        description: `اختبار شامل في ${COURSE_DATA[i % 5].title}`,
        assessmentType: 'CODING',
        allowedTime: 180, // 3 hours
        startTime: new Date(now.getTime() - 3600000), // started 1 hour ago
        endTime: new Date(now.getTime() + 7200000),   // ends in 2 hours
        totalPoints: 100,
        passingScore: 60,
        isPublished: true,
        lockdownMode: true,
        challengeIds: JSON.stringify([
          challenges[i * 5].id,
          challenges[i * 5 + 1].id,
          challenges[i * 5 + 2].id,
        ]),
        questions: JSON.stringify([
          { id: 'q1', text: 'اكتب برنامج يطبع "مرحبا"', points: 33 },
          { id: 'q2', text: 'اكتب دالة تجمع عددين', points: 33 },
          { id: 'q3', text: 'اكتب حلقة تطبع الأرقام من 1 إلى 10', points: 34 },
        ]),
        starterCode: '// اكتب كودك هنا',
        instructions: 'حل الأسئلة التالية using الضاد',
      }
    });
    assessments.push(assessment);
  }

  const elapsed = ((Date.now() - startTime) / 1000).toFixed(1);
  console.log(`\n✅ Seed complete in ${elapsed}s`);
  console.log(`   📊 Summary:`);
  console.log(`      Schools:     10`);
  console.log(`      Teachers:    50`);
  console.log(`      Admins:      5`);
  console.log(`      Students:    500`);
  console.log(`      Classrooms:  50`);
  console.log(`      Courses:     5`);
  console.log(`      Challenges:  100`);
  console.log(`      Assessments: 20`);
  console.log(`      Enrollments: ${500 * 5}`);
}

main()
  .catch(e => { console.error('❌ Seed failed:', e); process.exit(1); })
  .finally(() => prisma.$disconnect());
