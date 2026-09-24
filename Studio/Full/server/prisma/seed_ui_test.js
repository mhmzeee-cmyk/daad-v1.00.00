// ====================================================
// UI Test Seed Script — Creates test accounts for interactive testing
// ====================================================
const { PrismaClient } = require("@prisma/client");
const bcrypt = require("bcryptjs");

const prisma = new PrismaClient();
const BCRYPT_ROUNDS = 12;

const PASSWORD = "Test1234!";

async function seed() {
  try {
    console.log("=== UI Test Seed ===\n");

    // 1. School
    let school = await prisma.school.findFirst();
    if (!school) {
      school = await prisma.school.create({
        data: {
          name: "Dhad Academy",
          code: "DHAD01",
          email: "admin@dhad-academy.com",
          phone: "+966500000001",
          address: "Riyadh, Saudi Arabia",
          planType: "SCHOOL",
          totalLicenses: 100,
          usedLicenses: 5,
        },
      });
      console.log("Created school:", school.name, school.id);
    } else {
      console.log("Using existing school:", school.name, school.id);
    }

    const passwordHash = await bcrypt.hash(PASSWORD, BCRYPT_ROUNDS);

    // 2. Teacher
    let teacher = await prisma.user.findFirst({ where: { email: "teacher@dhad.com" } });
    if (!teacher) {
      teacher = await prisma.user.create({
        data: {
          schoolId: school.id,
          role: "TEACHER",
          name: "Ahmad Al-Rashid",
          email: "teacher@dhad.com",
          nationalId: null,
          passwordHash,
          isActive: true,
          isVerified: true,
          isApproved: true,
          isOnboarded: true,
        },
      });
      console.log("Created teacher:", teacher.name, teacher.id);
    } else {
      // Ensure password is set
      if (!teacher.passwordHash) {
        await prisma.user.update({ where: { id: teacher.id }, data: { passwordHash } });
      }
      console.log("Using existing teacher:", teacher.name, teacher.id);
    }

    // 3. Classroom
    let classroom = await prisma.classroom.findFirst({ where: { teacherId: teacher.id } });
    if (!classroom) {
      classroom = await prisma.classroom.create({
        data: {
          schoolId: school.id,
          teacherId: teacher.id,
          name: "Grade 7 Programming",
          description: "Introduction to programming with Dhad",
          gradeLevel: "7th Grade",
          maxStudents: 30,
          isActive: true,
        },
      });
      console.log("Created classroom:", classroom.name, classroom.id);
    } else {
      console.log("Using existing classroom:", classroom.name, classroom.id);
    }

    // 4. Student
    let student = await prisma.user.findFirst({ where: { email: "student@dhad.com" } });
    if (!student) {
      student = await prisma.user.create({
        data: {
          schoolId: school.id,
          classroomId: classroom.id,
          role: "STUDENT",
          name: "Sara Al-Otaibi",
          email: "student@dhad.com",
          nationalId: "1099990001",
          passwordHash,
          isActive: true,
          isVerified: true,
          isApproved: true,
          isOnboarded: true,
        },
      });
      console.log("Created student:", student.name, student.id);
    } else {
      // Ensure password + classroomId
      const updates = {};
      if (!student.passwordHash) updates.passwordHash = passwordHash;
      if (!student.classroomId) updates.classroomId = classroom.id;
      if (!student.isActive) updates.isActive = true;
      if (Object.keys(updates).length > 0) {
        await prisma.user.update({ where: { id: student.id }, data: updates });
      }
      console.log("Using existing student:", student.name, student.id);
    }

    // 5. Student Profile
    const existingProfile = await prisma.studentProfile.findUnique({ where: { studentId: student.id } });
    if (!existingProfile) {
      await prisma.studentProfile.create({
        data: {
          studentId: student.id,
          totalXP: 350,
          currentLevel: 2,
          currentStreak: 3,
          longestStreak: 7,
          lastActivityDate: new Date().toISOString().split("T")[0],
        },
      });
      console.log("Created student profile with 350 XP, Level 2");
    }

    // 6. Admin (if missing)
    const existingAdmin = await prisma.user.findFirst({ where: { role: "ADMIN" } });
    if (!existingAdmin) {
      await prisma.user.create({
        data: {
          schoolId: school.id,
          role: "ADMIN",
          name: "System Admin",
          email: "admin@dhadstudio.com",
          passwordHash,
          isActive: true,
          isVerified: true,
          isApproved: true,
        },
      });
      console.log("Created admin: admin@dhadstudio.com");
    }

    console.log("\n=== SEED COMPLETE ===");
    console.log("School ID:  ", school.id);
    console.log("Teacher ID: ", teacher.id);
    console.log("Classroom:  ", classroom.id);
    console.log("Student ID: ", student.id);
  } catch (err) {
    console.error("Seed error:", err);
  } finally {
    await prisma.$disconnect();
  }
}

seed();
