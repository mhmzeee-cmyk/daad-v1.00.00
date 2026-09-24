process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();
async function main() {
  const teachers = await p.user.findMany({ where: { role: 'TEACHER' }, take: 5, select: { name: true, email: true } });
  console.log('Teacher emails:');
  teachers.forEach(t => console.log('  ' + t.email));
}
main().catch(console.error).finally(() => p.$disconnect());
