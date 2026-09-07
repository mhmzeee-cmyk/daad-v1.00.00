process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
async function main() {
  const inactive = await p.user.count({ where: { role: 'STUDENT', isActive: false } });
  const active = await p.user.count({ where: { role: 'STUDENT', isActive: true } });
  console.log('Active students:', active);
  console.log('Inactive students:', inactive);
  
  const classrooms = await p.classroom.findMany({ select: { id: true, name: true, teacherId: true, _count: { select: { students: true } } }, take: 5 });
  console.log('\nSample classrooms:');
  classrooms.forEach(c => console.log('  ' + c.name + ' - teacher: ' + c.teacherId.substring(0, 8) + '... - students: ' + c._count.students));
}
main().catch(console.error).finally(() => p.$disconnect());
