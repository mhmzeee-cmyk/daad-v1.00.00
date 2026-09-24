// ====================================================
// Fix Challenge Tier Assignments
// Reassigns challenges to correct tiers based on content
// Run: node prisma/fix_tiers.js
// ====================================================

const { PrismaClient } = require('@prisma/client');
const prisma = new PrismaClient();

const TIER_RULES = [
  { tier: 1, keywords: ['print', 'variable', 'string', 'number', 'hello', 'output', 'basic', 'print', 'اطبع', 'متغير', 'نص', 'رقم', 'اساسي', ' BASIC', 'declare', 'assign'], title: 'أساسيات البرمجة' },
  { tier: 2, keywords: ['if', 'else', 'condition', 'switch', 'case', 'logic', ' شرط', ' تحكم', 'if', 'else', 'switch', 'conditional', 'boolean', 'منطقي'], title: 'المنطق والتحكم' },
  { tier: 3, keywords: ['loop', 'for', 'while', 'repeat', 'iterate', 'iteration', 'كرر', 'تكرار', 'loop', 'for', 'while', 'iteration', 'repeat', 'forEach'], title: 'الحلقات والتكرار' },
  { tier: 4, keywords: ['function', 'method', 'def', 'return', 'parameter', 'argument', 'دالة', 'method', 'function', 'procedure', 'param', 'call', 'invoke'], title: 'الدوال والمетодات' },
  { tier: 5, keywords: ['array', 'list', 'push', 'pop', 'shift', 'unshift', 'splice', 'مصفوفة', 'قائمة', 'array', 'list', 'index', 'element', 'item'], title: 'المصفوفات والقوائم' },
  { tier: 6, keywords: ['object', 'class', 'new', 'this', 'property', 'method', 'constructor', 'كائن', 'فئة', 'class', 'object', 'instance', 'constructor', 'new', 'this'], title: 'البرمجة الكائنية' },
  { tier: 7, keywords: ['try', 'catch', 'throw', 'error', 'exception', 'async', 'await', 'promise', 'حاول', 'امسك', ' خطأ', 'exception', 'error', 'try', 'catch', 'async', 'await', 'promise'], title: 'معالجة الأخطاء والمتزامن' },
];

function detectTier(challenge) {
  const text = ((challenge.title || '') + ' ' + (challenge.description || '') + ' ' + (challenge.starterCode || '')).toLowerCase();

  for (const rule of TIER_RULES) {
    for (const kw of rule.keywords) {
      if (text.includes(kw.toLowerCase())) {
        return rule.tier;
      }
    }
  }

  // Fallback: assign by difficulty
  const diffMap = { BEGINNER: 1, INTERMEDIATE: 3, ADVANCED: 5, EXPERT: 7 };
  return diffMap[challenge.difficulty] || 1;
}

async function fixTiers() {
  try {
    console.log('🔧 Fixing challenge tier assignments...\n');

    const challenges = await prisma.challenge.findMany({
      where: { courseId: null },
      select: { id: true, title: true, description: true, starterCode: true, difficulty: true, tier: true }
    });

    console.log(`Found ${challenges.length} challenges\n`);

    let updated = 0;
    for (const c of challenges) {
      const newTier = detectTier(c);
      if (newTier !== c.tier) {
        console.log(`  ${c.title}`);
        console.log(`    Old tier: ${c.tier} → New tier: ${newTier}`);
        await prisma.challenge.update({
          where: { id: c.id },
          data: { tier: newTier }
        });
        updated++;
      }
    }

    console.log(`\n✅ Updated ${updated} challenges`);

    // Show final distribution
    const tiers = await prisma.challenge.groupBy({
      by: ['tier'],
      where: { courseId: null },
      _count: true,
      orderBy: { tier: 'asc' }
    });

    console.log('\n📊 Final tier distribution:');
    for (const t of tiers) {
      const rule = TIER_RULES.find(r => r.tier === t.tier);
      console.log(`  Tier ${t.tier} (${rule ? rule.title : '?'}): ${t._count} challenges`);
    }

  } catch (error) {
    console.error('❌ Error:', error);
  } finally {
    await prisma.$disconnect();
  }
}

fixTiers();
