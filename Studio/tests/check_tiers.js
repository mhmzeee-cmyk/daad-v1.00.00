const { PrismaClient } = require("../server/node_modules/@prisma/client");
const prisma = new PrismaClient();
async function main() {
  const challenges = await prisma.challenge.findMany({
    where: { courseId: null, published: true },
    select: { id: true, title: true, tier: true, difficulty: true, courseId: true }
  });
  console.log("Total main published:", challenges.length);
  var tiers = {};
  challenges.forEach(function(c) { tiers[c.tier] = (tiers[c.tier] || 0) + 1; });
  console.log("By tier:", JSON.stringify(tiers));
  var noTier = challenges.filter(function(c) { return c.tier === null || c.tier === undefined; });
  console.log("Without tier:", noTier.length);
  await prisma.$disconnect();
}
main().catch(console.error);
