const fs = require('fs');
const files = ['challenges_tier1.js','challenges_tier2.js','challenges_tier3.js','challenges_tier4.js','challenges_tier5.js'];
for (const f of files) {
  const path = 'tests/' + f;
  let content = fs.readFileSync(path, 'utf8');
  const before = (content.match(/starterCode:/g) || []).length;
  content = content.replace(/starterCode:'[^']*'/g, "starterCode:''");
  fs.writeFileSync(path, content, 'utf8');
  console.log(f + ': ' + before + ' starterCode fields cleared');
}
