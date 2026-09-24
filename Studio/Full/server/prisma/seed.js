// ============================================================
// DAAD Studio — Main Seed Script
// Seeds 210 Arabic challenges across 7 tiers
// Uses the same data as reseed_all_challenges.js
// ============================================================

const { PrismaClient } = require('@prisma/client');
const path = require('path');

process.env.DATABASE_URL = process.env.DATABASE_URL || ('file:' + path.resolve(__dirname, './dev.db'));
const prisma = new PrismaClient();

async function main() {
  console.log('═══════════════════════════════════════════════════');
  console.log('  DAAD Studio — Seed 210 Challenges');
  console.log('═══════════════════════════════════════════════════\n');

  const existingCount = await prisma.challenge.count();
  if (existingCount >= 200) {
    console.log(`✅ Database already seeded (${existingCount} challenges found)`);
    console.log('   Run "node reseed_all_challenges.js" to force reseed');
    return;
  }

  // Load challenge data from tests/challenges_data.js
  const challengesPath = path.resolve(__dirname, '../../../tests/challenges_data.js');
  let challenges;
  try {
    challenges = require(challengesPath);
  } catch (e) {
    console.error('❌ Cannot load challenges_data.js');
    console.error('   Make sure tests/challenges_data.js exists');
    console.error('   Path:', challengesPath);
    process.exit(1);
  }

  console.log(`📦 Loaded ${challenges.length} challenges from data file`);

  // Find admin author
  const admin = await prisma.user.findFirst({ where: { role: 'ADMIN' } });
  if (!admin) {
    console.error('❌ No ADMIN user found! Create an admin first.');
    process.exit(1);
  }
  console.log(`📌 Author: ${admin.name} (${admin.id})`);

  // Insert challenges
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
          expectedOutput: c.expectedOutput || '',
          dynamicOutput: c.dynamicOutput || '',
          requirements: c.requirements || '[]',
          difficulty: c.difficulty || 'BEGINNER',
          challengeType: c.title.includes('المثال') ? 'LESSON' : 'EXERCISE',
          tier: c.tier,
          order: c.order,
          xpReward: c.xpReward || c.points || 10,
          points: c.points || 10,
          published: true,
          isSystem: false,
          authorId: admin.id,
          schoolId: null,
        }
      });
      created++;
      tierCounts[c.tier] = (tierCounts[c.tier] || 0) + 1;
    } catch (e) {
      failed++;
      if (failed <= 5) console.error(`  ❌ ${c.title}: ${e.message}`);
    }
  }

  // Summary
  const tierNames = {
    1: 'asics', 2: 'Logic', 3: 'Data Structures',
    4: 'OOP', 5: 'Algorithms', 6: 'Arrays', 7: 'Advanced OOP'
  };
  console.log('\n═══════════════════════════════════════════════════');
  for (let tier = 1; tier <= 7; tier++) {
    console.log(`  Tier ${tier} (${tierNames[tier]}): ${tierCounts[tier] || 0} challenges`);
  }
  console.log(`\n  ✅ Created: ${created}`);
  console.log(`  ❌ Failed: ${failed}`);

  const dbCount = await prisma.challenge.count();
  console.log(`  📊 Total in DB: ${dbCount}`);
  console.log('═══════════════════════════════════════════════════');
}

main()
  .catch(console.error)
  .finally(() => prisma.$disconnect());
