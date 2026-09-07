const { PrismaClient } = require("../server/node_modules/@prisma/client");
const prisma = new PrismaClient();
async function main() {
  const students = await prisma.user.findMany({
    where: { role: "STUDENT" },
    select: { id: true, name: true, nationalId: true, classroomId: true }
  });
  students.forEach(function(s) {
    console.log(s.id.substring(0,10)+"...", s.name.substring(0,20), "nationalId:", s.nationalId, "classroom:", s.classroomId?.substring(0,10));
  });
  const classrooms = await prisma.classroom.findMany({ select: { id: true, name: true } });
  classrooms.forEach(function(c) { console.log("Classroom:", c.id.substring(0,10)+"...", c.name); });
  await prisma.$disconnect();
}
main().catch(console.error);
