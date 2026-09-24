process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, '../server/prisma/dev.db');
const { PrismaClient } = require('../server/node_modules/@prisma/client');
const bcrypt = require('../server/node_modules/bcryptjs');
const prisma = new PrismaClient();

const cryptoChallenges = require('./courses_crypto');
const cyberChallenges = require('./courses_cyber');
const webChallenges = require('./courses_web');

async function main() {
  console.log('=== SEEDING ADDITIONAL COURSES ===\n');

  // Find or create school
  let school = await prisma.school.findFirst({ where: { name: 'العبقري' } });
  if (!school) {
    school = await prisma.school.create({ data: { name: 'العبقري', code: 'ABQARI' } });
  }

  // Find admin user as author
  const admin = await prisma.user.findFirst({ where: { role: 'ADMIN' } });
  if (!admin) { console.error('No admin user found!'); return; }

  // ── 1. التشفير ──
  let cryptoCourse = await prisma.course.findFirst({ where: { title: 'التشفير' } });
  if (!cryptoCourse) {
    cryptoCourse = await prisma.course.create({
      data: {
        title: 'التشفير',
        description: 'دورة تعلمك أساسيات التشفير وحماية البيانات في لغة ض',
        icon: '🔐',
        category: 'التشفير',
        order: 1,
        unlockTier: 3,
        schoolId: school.id
      }
    });
    console.log('Created course: التشفير');
  }

  // Create lessons for crypto (10 lessons)
  const cryptoLessons = [
    { title: ' مفهوم التشفير', slug: 'crypto_what_is_encryption' },
    { title: ' فك التشفير', slug: 'crypto_decryption' },
    { title: 'المفتاح', slug: 'crypto_keys' },
    { title: 'التشفير المتماثل', slug: 'crypto_symmetric' },
    { title: 'التشفير غير المتماثل', slug: 'crypto_asymmetric' },
    { title: 'تشفير الهاش', slug: 'crypto_hash' },
    { title: 'شفرة القيصر', slug: 'crypto_caesar' },
    { title: 'تشفير XOR', slug: 'crypto_xor' },
    { title: 'HMAC - التحقق بالتوقيع', slug: 'crypto_hmac' },
    { title: 'ملخص دورة التشفير', slug: 'crypto_summary' }
  ];
  for (let i = 0; i < cryptoLessons.length; i++) {
    const lesson = cryptoLessons[i];
    const existing = await prisma.lesson.findFirst({ where: { courseId: cryptoCourse.id, order: i + 1 } });
    if (!existing) {
      await prisma.lesson.create({
        data: {
          title: lesson.title,
          contentSlug: lesson.slug,
          order: i + 1,
          schoolId: school.id,
          courseId: cryptoCourse.id
        }
      });
    }
  }

  // Create crypto challenges
  let cryptoCreated = 0;
  for (const ch of cryptoChallenges) {
    const existing = await prisma.challenge.findFirst({ where: { title: ch.title, courseId: cryptoCourse.id } });
    if (!existing) {
      let reqs = ch.requirements;
      if (typeof reqs === 'string' && reqs.startsWith('[')) {
        reqs = JSON.parse(reqs);
        reqs = JSON.stringify(reqs);
      } else if (Array.isArray(reqs)) {
        const normalized = reqs.map(r => {
          if (typeof r === 'string' && r.includes(':')) {
            const [type, keyword] = r.split(':');
            return { type: type, keyword: keyword };
          }
          return r;
        });
        reqs = JSON.stringify(normalized);
      }
      await prisma.challenge.create({
        data: {
          title: ch.title,
          description: ch.description,
          starterCode: ch.starterCode || '',
          expectedOutput: ch.expectedOutput,
          dynamicOutput: ch.dynamicOutput || '',
          requirements: reqs,
          difficulty: ch.difficulty,
          tier: ch.tier,
          order: ch.order,
          points: ch.points,
          xpReward: ch.xpReward,
          authorId: admin.id,
          courseId: cryptoCourse.id,
          published: true,
          isSystem: true
        }
      });
      cryptoCreated++;
    }
  }
  console.log('Crypto challenges: ' + cryptoCreated + ' created');

  // ── 2. الأمن السيبراني ──
  let cyberCourse = await prisma.course.findFirst({ where: { title: 'الأمن السيبراني' } });
  if (!cyberCourse) {
    cyberCourse = await prisma.course.create({
      data: {
        title: 'الأمن السيبراني',
        description: 'دورة شاملة في الأمن السيبراني: اختبار الاختراق وحماية الشبكات والتحليل الجنائي',
        icon: '🛡️',
        category: 'الأمن السيبراني',
        order: 2,
        unlockTier: 5,
        schoolId: school.id
      }
    });
    console.log('Created course: الأمن السيبراني');
  }

  // Create lessons for cyber (30 lessons)
  const cyberLessons = [
    'مقدمة في الاختراق', 'جمع المعلومات', 'مسح الشبكات', 'استغلال الثغرات',
    'الحصول على الوصول', 'حفظ الوصول', 'تجنب الكشف', 'تقرير الاختراق',
    'أدوات اختبار الاختراق', 'تمرين عملي',
    'مقدمة أمن الشبكات', 'جدران الحماية', 'أنظمة كشف الاختراق', 'VPN',
    'تشفير الشبكات', 'أمان WiFi', 'مراقبة الشبكة', 'استجابة الحوادث',
    'سياسات الأمان', 'حماية التطبيقات',
    'مقدمة التحليل الجنائي', 'تحليل الأقراص', 'تحليل الذاكرة', 'تحليل الشبكات',
    'البريد الإلكتروني', 'الأدلة الرقمية', 'سلسلة الحفظ', 'التوقيعات الرقمية',
    'التقارير الجنائية', 'دراسة حالة'
  ];
  for (let i = 0; i < cyberLessons.length; i++) {
    const existing = await prisma.lesson.findFirst({ where: { courseId: cyberCourse.id, order: i + 1 } });
    if (!existing) {
      await prisma.lesson.create({
        data: {
          title: cyberLessons[i],
          contentSlug: 'cyber_lesson_' + (i + 1),
          order: i + 1,
          schoolId: school.id,
          courseId: cyberCourse.id
        }
      });
    }
  }

  // Create cyber challenges
  let cyberCreated = 0;
  for (const ch of cyberChallenges) {
    const existing = await prisma.challenge.findFirst({ where: { title: ch.title, courseId: cyberCourse.id } });
    if (!existing) {
      // Normalize requirements to JSON string
      let reqs = ch.requirements;
      if (Array.isArray(reqs)) {
        const normalized = reqs.map(r => {
          if (typeof r === 'string' && r.includes(':')) {
            const [type, keyword] = r.split(':');
            return { type: type, keyword: keyword };
          }
          return r;
        });
        reqs = JSON.stringify(normalized);
      }
      await prisma.challenge.create({
        data: {
          title: ch.title,
          description: ch.description,
          starterCode: ch.starterCode || '',
          expectedOutput: ch.expectedOutput,
          dynamicOutput: ch.dynamicOutput || '',
          requirements: reqs,
          difficulty: ch.difficulty,
          tier: ch.tier,
          order: ch.order,
          points: ch.points,
          xpReward: ch.xpReward,
          authorId: admin.id,
          courseId: cyberCourse.id,
          published: true,
          isSystem: true
        }
      });
      cyberCreated++;
    }
  }
  console.log('Cyber challenges: ' + cyberCreated + ' created');

  // ── 3. برمجة صفحات الويب ──
  let webCourse = await prisma.course.findFirst({ where: { title: 'برمجة صفحات الويب' } });
  if (!webCourse) {
    webCourse = await prisma.course.create({
      data: {
        title: 'برمجة صفحات الويب',
        description: 'دورة تعلمك HTML وCSS من الصفر لبناء صفحات ويب جميلة',
        icon: '🌐',
        category: 'برمجة صفحات الويب',
        order: 3,
        unlockTier: 4,
        schoolId: school.id
      }
    });
    console.log('Created course: برمجة صفحات الويب');
  }

  // Create lessons for web (10 lessons)
  const webLessons = [
    'أساسيات العناصر', 'العناصر النصية', 'الروابط والصور', 'القوائم والجداول',
    'النماذج', 'الجداول المتقدمة', 'CSS fundamentals', 'الألوان والخطوط',
    'التخطيط المرن', 'مشروع عملي'
  ];
  for (let i = 0; i < webLessons.length; i++) {
    const existing = await prisma.lesson.findFirst({ where: { courseId: webCourse.id, order: i + 1 } });
    if (!existing) {
      await prisma.lesson.create({
        data: {
          title: webLessons[i],
          contentSlug: 'web_lesson_' + (i + 1),
          order: i + 1,
          schoolId: school.id,
          courseId: webCourse.id
        }
      });
    }
  }

  // Create web challenges
  let webCreated = 0;
  for (const ch of webChallenges) {
    const existing = await prisma.challenge.findFirst({ where: { title: ch.title, courseId: webCourse.id } });
    if (!existing) {
      let reqs = ch.requirements;
      if (typeof reqs === 'string' && reqs.startsWith('[')) {
        reqs = JSON.parse(reqs);
        reqs = JSON.stringify(reqs);
      } else if (Array.isArray(reqs)) {
        const normalized = reqs.map(r => {
          if (typeof r === 'string' && r.includes(':')) {
            const [type, keyword] = r.split(':');
            return { type: type, keyword: keyword };
          }
          return r;
        });
        reqs = JSON.stringify(normalized);
      }
      await prisma.challenge.create({
        data: {
          title: ch.title,
          description: ch.description,
          starterCode: ch.starterCode || '',
          expectedOutput: ch.expectedOutput,
          dynamicOutput: ch.dynamicOutput || '',
          requirements: reqs,
          difficulty: ch.difficulty,
          tier: ch.tier,
          order: ch.order,
          points: ch.points,
          xpReward: ch.xpReward,
          authorId: admin.id,
          courseId: webCourse.id,
          published: true,
          isSystem: true
        }
      });
      webCreated++;
    }
  }
  console.log('Web challenges: ' + webCreated + ' created');

  // ── UPDATE EXISTING LESSON TITLES ──
  console.log('\n--- Updating existing lesson titles ---');

  // Update crypto lessons
  if (cryptoCourse) {
    for (let i = 0; i < cryptoLessons.length; i++) {
      await prisma.lesson.updateMany({
        where: { courseId: cryptoCourse.id, order: i + 1 },
        data: { title: cryptoLessons[i].title, contentSlug: cryptoLessons[i].slug }
      });
    }
    console.log('Crypto lessons updated');
  }

  // Update cyber lessons
  if (cyberCourse) {
    for (let i = 0; i < cyberLessons.length; i++) {
      await prisma.lesson.updateMany({
        where: { courseId: cyberCourse.id, order: i + 1 },
        data: { title: cyberLessons[i] }
      });
    }
    console.log('Cyber lessons updated');
  }

  // Update web lessons
  if (webCourse) {
    for (let i = 0; i < webLessons.length; i++) {
      await prisma.lesson.updateMany({
        where: { courseId: webCourse.id, order: i + 1 },
        data: { title: webLessons[i] }
      });
    }
    console.log('Web lessons updated');
  }

  // Update crypto challenges - fix HMAC keyword space and improve descriptions
  if (cryptoCourse) {
    // Fix HMAC challenge requirements (remove leading space)
    const hmacChallenges = await prisma.challenge.findMany({
      where: { courseId: cryptoCourse.id, title: { contains: 'HMAC' } }
    });
    for (const ch of hmacChallenges) {
      if (ch.requirements && ch.requirements.includes('" HMAC"')) {
        const fixedReqs = ch.requirements.replace('" HMAC"', '"HMAC"');
        await prisma.challenge.update({
          where: { id: ch.id },
          data: { requirements: fixedReqs }
        });
      }
    }
    console.log('HMAC keyword space fixed');
  }

  // Summary
  console.log('\n=== SUMMARY ===');
  console.log('Crypto: ' + cryptoCreated + ' challenges, 10 lessons');
  console.log('Cyber: ' + cyberCreated + ' challenges, 30 lessons');
  console.log('Web: ' + webCreated + ' challenges, 10 lessons');
  console.log('Total: ' + (cryptoCreated + cyberCreated + webCreated) + ' challenges');
}

main().catch(console.error).finally(() => prisma.$disconnect());
