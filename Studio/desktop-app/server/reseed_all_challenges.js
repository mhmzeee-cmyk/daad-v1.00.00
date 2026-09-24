process.env.DATABASE_URL = 'file:' + require('path').resolve(__dirname, './prisma/dev.db');
process.env.JWT_SECRET = 'dhad-studio-dev-secret-key-change-in-production';
process.env.HMAC_SECRET = 'dhad-studio-hmac-secret-2026-production';

const { PrismaClient } = require('./node_modules/@prisma/client');
const prisma = new PrismaClient();

async function main() {
  console.log('═══════════════════════════════════════════════════');
  console.log('  إعادة تعبئة شاملة — 210 تحدي ببيانات حقيقية');
  console.log('═══════════════════════════════════════════════════\n');

  // ── الخطوة 1: حذف البيانات القديمة ──
  console.log('🗑️  حذف البيانات القديمة...');
  const ds1 = await prisma.studentSubmission.deleteMany();
  console.log(`  ✅ حذف ${ds1.count} studentSubmission`);
  const ds2 = await prisma.submission.deleteMany();
  console.log(`  ✅ حذف ${ds2.count} submission`);
  const ds3 = await prisma.challengeVerification.deleteMany();
  console.log(`  ✅ حذف ${ds3.count} challengeVerification`);
  const ds4 = await prisma.cloudWorkspace.deleteMany();
  console.log(`  ✅ حذف ${ds4.count} cloudWorkspace`);
  const ds5 = await prisma.challengeTier.deleteMany();
  console.log(`  ✅ حذف ${ds5.count} challengeTier`);
  const ds6 = await prisma.challenge.deleteMany();
  console.log(`  ✅ حذف ${ds6.count} challenge`);

  // ── الخطوة 2: تحميل البيانات ──
  console.log('\n📦 تحميل بيانات التحديات...');
  const challenges = require('../tests/challenges_data.js');
  console.log(`  ✅ تم تحميل ${challenges.length} تحدي`);

  // ── الخطوة 3: معرف المؤلف ──
  const admin = await prisma.user.findFirst({ where: { role: 'ADMIN' } });
  if (!admin) { console.error('❌ لا يوجد admin!'); return; }
  console.log(`  📌 المؤلف: ${admin.id}`);

  // ── الخطوة 4: إدخال التحديات ──
  console.log('\n🎯 إدخال التحديات...');
  let created = 0;
  let failed = 0;
  const tierCounts = {};

  for (const c of challenges) {
    try {
      await prisma.challenge.create({
        data: {
          title: c.title,
          description: c.description,
          starterCode: c.starterCode || '',
          expectedOutput: c.expectedOutput,
          dynamicOutput: c.dynamicOutput || '',
          requirements: c.requirements || '[]',
          difficulty: c.difficulty,
          tier: c.tier,
          order: c.order,
          xpReward: c.xpReward,
          points: c.points,
          published: true,
          isSystem: false,
          authorId: admin.id,
          schoolId: null, // System-wide challenges — available to all schools
        }
      });
      created++;
      tierCounts[c.tier] = (tierCounts[c.tier] || 0) + 1;
    } catch (e) {
      failed++;
      console.error(`  ❌ فشل: ${c.title} — ${e.message}`);
    }
  }

  // ── الخطوة 5: الملخص ──
  console.log('\n═══════════════════════════════════════════════════');
  console.log('  📊 ملخص التحديات');
  console.log('═══════════════════════════════════════════════════');
  const tierNames = {
    1: 'أساسيات', 2: 'منطق وتحكم', 3: 'هياكل بيانات',
    4: 'كائنية', 5: 'خوارزميات', 6: 'مصفوفات', 7: 'كائنية متقدمة'
  };
  for (let tier = 1; tier <= 7; tier++) {
    const count = tierCounts[tier] || 0;
    const diff = tier <= 1 ? 'BEGINNER' : tier <= 3 ? 'INTERMEDIATE' : tier <= 5 ? 'ADVANCED' : 'EXPERT';
    console.log(`  Tier ${tier} (${tierNames[tier]}): ${count} تحدي | ${diff}`);
  }
  console.log(`\n  ✅ تم إنشاء: ${created}`);
  console.log(`  ❌ فشل: ${failed}`);
  console.log(`  📈 الإجمالي: ${challenges.length}`);

  // ── الخطوة 6: التحقق ──
  console.log('\n🔍 التحقق...');
  const dbCount = await prisma.challenge.count();
  console.log(`  عدد التحديات في DB: ${dbCount}`);

  const emptyOutput = await prisma.challenge.count({ where: { expectedOutput: '' } });
  console.log(`  تحديات بـ expectedOutput فارغ: ${emptyOutput}`);

  const nullSchool = await prisma.challenge.count({ where: { schoolId: null } });
  console.log(`  تحديات بدون schoolId: ${nullSchool}`);

  if (dbCount === 210 && emptyOutput === 0 && nullSchool === 0) {
    console.log('\n✅✅✅ اكتملت إعادة التعبئة بنجاح! ✅✅✅');
  } else {
    console.log('\n⚠️  تحذير: هناك مشاكل في البيانات');
  }
}

main()
  .catch(console.error)
  .finally(() => prisma.$disconnect());
