// ====================================================
// Generate strong secrets for .env (H1+H7+M5)
// Usage:
//   node scripts/generate-secrets.js           # print only (safe)
//   node scripts/generate-secrets.js --write   # append missing keys to .env (never overwrites)
// ====================================================
const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

const SECRETS = {
  JWT_SECRET: 64,
  JWT_REFRESH_SECRET: 64,
  HMAC_SECRET: 64,
  CSRF_SECRET: 32,
  ENCRYPTION_KEY: 32,
};

const generated = {};
for (const [key, bytes] of Object.entries(SECRETS)) {
  generated[key] = crypto.randomBytes(bytes).toString('hex');
}

if (process.argv.includes('--write')) {
  const envPath = path.join(__dirname, '../.env');
  let content = fs.existsSync(envPath) ? fs.readFileSync(envPath, 'utf8') : '';
  if (content.length > 0 && !content.endsWith('\n')) content += '\n';
  let added = 0;
  for (const [key, value] of Object.entries(generated)) {
    const present = new RegExp(`^${key}=`, 'm').test(content);
    if (!present) {
      content += `${key}=${value}\n`;
      added++;
    }
  }
  fs.writeFileSync(envPath, content, 'utf8');
  console.log(`Wrote ${added} missing secret(s) to ${envPath} (existing values untouched).`);
} else {
  console.log('Add these to your .env (or re-run with --write):');
  for (const [key, value] of Object.entries(generated)) {
    console.log(`${key}=${value}`);
  }
}
