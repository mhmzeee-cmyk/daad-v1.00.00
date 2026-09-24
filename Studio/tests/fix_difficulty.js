// Fix difficulty labels: each tier gets one consistent difficulty
// Tier 1: BEGINNER, Tier 2: INTERMEDIATE, Tier 3: ADVANCED, Tier 4: INTERMEDIATE, Tier 5: EXPERT, Tier 6: ADVANCED

const fs = require('fs');
const path = require('path');

const TIER_DIFFICULTY = {
  1: 'BEGINNER',
  2: 'INTERMEDIATE',
  3: 'ADVANCED',
  4: 'INTERMEDIATE',
  5: 'EXPERT',
  6: 'ADVANCED'
};

for (let t = 1; t <= 6; t++) {
  const filePath = path.join(__dirname, 'challenges_tier' + t + '.js');
  let content = fs.readFileSync(filePath, 'utf8');

  // Replace all difficulty values with the tier's designated difficulty
  const newDiff = TIER_DIFFICULTY[t];
  content = content.replace(/difficulty:'[A-Z]+'/g, "difficulty:'" + newDiff + "'");

  fs.writeFileSync(filePath, content, 'utf8');
  console.log('Tier ' + t + ': all -> ' + newDiff);
}

console.log('\nDone. Reloading to verify...');
for (let t = 1; t <= 6; t++) {
  const challenges = require('./challenges_tier' + t);
  const diffs = {};
  challenges.forEach(c => { diffs[c.difficulty] = (diffs[c.difficulty] || 0) + 1; });
  console.log('Tier ' + t + ':', JSON.stringify(diffs));
}
