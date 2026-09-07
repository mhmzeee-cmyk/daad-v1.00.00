const OpenAI = require('openai');
const fs = require('fs');
const path = require('path');

const CONFIG = {
  maxFileChars: parseInt(process.env.MAX_CONTEXT || '12000', 10),
  maxDirFileChars: parseInt(process.env.MAX_DIR_CONTEXT || '4000', 10),
  temperature: parseFloat(process.env.TEMP || '0.3'),
};
const MODEL = process.env.MODEL || 'gpt-4o-mini';
const TOKEN = process.env.GITHUB_TOKEN;
if (!TOKEN) {
  console.error('GITHUB_TOKEN not set');
  process.exit(1);
}

const client = new OpenAI({
  baseURL: 'https://models.inference.ai.azure.com',
  apiKey: TOKEN,
});

const args = process.argv.slice(2);
const fileFlag = '--file';
const dirFlag = '--dir';
const files = [];
let dir = null;
let promptParts = [];

for (let i = 0; i < args.length; i++) {
  if (args[i] === fileFlag && i + 1 < args.length) {
    files.push(args[++i]);
  } else if (args[i] === dirFlag && i + 1 < args.length) {
    dir = args[++i];
  } else {
    promptParts.push(args[i]);
  }
}

const finalPrompt = promptParts.join(' ').trim() || fs.readFileSync(0, 'utf-8').trim();
if (!finalPrompt) {
  console.error('Usage: node github-agent.js [--file <path> ...] [--dir <dir>] <prompt>');
  process.exit(1);
}

async function main() {
  let context = '';

  if (dir) {
    const entries = fs.readdirSync(dir, { withFileTypes: true });
    const targets = entries.filter(e => {
      if (e.isDirectory()) return false;
      const ext = path.extname(e.name).toLowerCase();
      return ['.js', '.json', '.env', '.yml', '.yaml', '.md', '.txt', '.html', '.css'].includes(ext);
    });
    for (const e of targets) {
      const fullPath = path.join(dir, e.name);
      try {
        const content = fs.readFileSync(fullPath, 'utf-8').slice(0, CONFIG.maxDirFileChars);
        context += `\n=== ${e.name} ===\n${content}\n`;
      } catch { }
    }
  }

  for (const f of files) {
    try {
      const content = fs.readFileSync(f, 'utf-8').slice(0, CONFIG.maxFileChars);
      context += `\n=== ${path.basename(f)} ===\n${content}\n`;
    } catch (e) {
      context += `\n=== ${f} ===\n[ERROR: ${e.message}]\n`;
    }
  }

  // Truncate to ~32k chars to fit context window
  if (context.length > 32000) {
    context = context.slice(0, 32000) + '\n... [truncated] ...\n';
  }
  context += `\n=== PROJECT ROOT ===\nC:\\Projects\\dhad-studio\n`;

  const fullPrompt = context ? `${context}\n\n### INSTRUCTION ###\nأنت خبير تطوير مشاريع ض (Daad Studio). حلل ما سبق وأعطني:\n1. أهم 3 مشاكل حرجة تحتاج حل فوري\n2. كود محدد لحل كل مشكلة (كامل، جاهز للتطبيق)\n3. ملفات التعديل بالضبط\n4. أوامر لتشغيل/اختبار كل تغيير\n\n${finalPrompt}` : finalPrompt;

  const res = await client.chat.completions.create({
    model: MODEL,
    messages: [{ role: 'user', content: fullPrompt }],
    temperature: CONFIG.temperature,
  });

  console.log(res.choices[0].message.content);
}

main().catch(e => { console.error(e.message); process.exit(1); });
