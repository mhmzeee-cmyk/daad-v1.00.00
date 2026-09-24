process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
require('../server/node_modules/dotenv').config({ path: '../server/.env' });
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const p = new PrismaClient();

async function main() {
  console.log('Creating 5 lessons for roadmap...');

  const lessonData = [
    { order: 1, title: 'اساسيات البرمجة', contentSlug: 'basics' },
    { order: 2, title: 'المنطق والتحكم', contentSlug: 'logic-control' },
    { order: 3, title: 'هيكل البيانات', contentSlug: 'data-structures' },
    { order: 4, title: 'البرمجة الكائنية', contentSlug: 'oop' },
    { order: 5, title: 'الخوارزميات وادارة الذاكرة', contentSlug: 'algorithms' },
  ];

  // Get first school
  const school = await p.school.findFirst({ where: { name: 'School_A' } });
  if (!school) { console.log('No school found!'); return; }

  for (const ld of lessonData) {
    await p.lesson.create({
      data: {
        schoolId: school.id,
        title: ld.title,
        contentSlug: ld.contentSlug,
        order: ld.order,
        published: true,
        visibility: 'PUBLIC',
        content: 'محتوى ' + ld.title,
      }
    });
    console.log('  Created lesson:', ld.order, ld.title);
  }

  // Verify
  const count = await p.lesson.count();
  console.log('\nTotal lessons:', count);

  // Test roadmap logic
  console.log('\n--- Roadmap Logic ---');
  for (let tier = 1; tier <= 5; tier++) {
    const chCount = await p.challenge.count({ where: { tier } });
    console.log('Tier', tier + ':', chCount, 'challenges');
  }
}

main().catch(console.error).finally(() => p.$disconnect());
