// ض (Dhad) — أوامر الترجمة والتشغيل داخل VS Code (تعمل بالمترجم الحقيقي، بلا اعتماديات)
// VSCODE_SPEC المرجعي: docs/vscode/VSCODE_SPEC.md — الصيغ المستخدمة هنا هي الصيغ المتقاطعة المضمونة.
const vscode = require('vscode');
const path = require('path');
const fs = require('fs');
const os = require('os');

function q(s) {
  return '"' + String(s).replace(/"/g, '\\"') + '"';
}

// يجد جذر المستودع (مجلد Compiler/) صعودًا من ملف، ثم ثنائي المترجم المبني، وإلا PATH.
function findCompiler(startDir) {
  let dir = startDir;
  for (let i = 0; i < 8; i++) {
    const c = path.join(dir, 'Compiler');
    try {
      if (fs.statSync(c).isDirectory()) {
        const b1 = path.join(c, 'build-make', 'bin', 'daad-compiler');
        const b2 = path.join(c, 'build', 'bin', 'daad-compiler');
        if (fs.existsSync(b1)) return { root: c, bin: b1 };
        if (fs.existsSync(b2)) return { root: c, bin: b2 };
        return { root: c, bin: 'daad-compiler' };
      }
    } catch (e) { /* تابع الصعود */ }
    const parent = path.dirname(dir);
    if (parent === dir) break;
    dir = parent;
  }
  return { root: null, bin: 'daad-compiler' };
}

function currentDhadFile() {
  const editor = vscode.window.activeTextEditor;
  if (!editor || editor.document.languageId !== 'dhad') {
    vscode.window.showErrorMessage('افتح ملف ض (.ض) أولاً');
    return null;
  }
  if (editor.document.isUntitled) {
    vscode.window.showErrorMessage('احفظ ملف ض أولاً ثم شغّله');
    return null;
  }
  return editor.document;
}

async function compileOnly() {
  const doc = currentDhadFile();
  if (!doc) return;
  await doc.save();
  const file = doc.fileName;
  const dir = path.dirname(file);
  const base = path.basename(file, path.extname(file));
  const { bin } = findCompiler(dir);
  const outCpp = path.join(dir, base + '.cpp');
  const term = vscode.window.createTerminal('ض: ترجمة');
  term.show();
  term.sendText(`${q(bin)} ${q(file)} -o ${q(outCpp)}`);
  vscode.window.showInformationMessage('تم إرسال أمر الترجمة إلى الطرفية: ' + base + '.cpp');
}

async function compileAndRun() {
  const doc = currentDhadFile();
  if (!doc) return;
  await doc.save();
  const file = doc.fileName;
  const dir = path.dirname(file);
  const base = path.basename(file, path.extname(file)).replace(/[^\p{L}\p{N}_-]+/gu, '_');
  const { root, bin } = findCompiler(dir);
  const tmpDir = path.join(os.tmpdir(), 'dhad-run-' + process.pid);
  try { fs.mkdirSync(tmpDir, { recursive: true }); } catch (e) { /* موجود */ }
  const outCpp = path.join(tmpDir, base + '.cpp');
  const outExe = path.join(tmpDir, base + '.out');
  const inc = root ? ` -I ${q(root)} -I ${q(path.join(root, 'include'))}` : '';
  const term = vscode.window.createTerminal('ض: تشغيل');
  term.show();
  term.sendText(
    `${q(bin)} ${q(file)} -o ${q(outCpp)}` +
    ` && g++ -std=c++20 ${q(outCpp)} -o ${q(outExe)}${inc}` +
    ` && echo "─── مخرجات البرنامج ───" && ${q(outExe)}`
  );
}

function activate(context) {
  context.subscriptions.push(
    vscode.commands.registerCommand('dhad.compileAndRun', compileAndRun),
    vscode.commands.registerCommand('dhad.compile', compileOnly)
  );
}

function deactivate() {}

module.exports = { activate, deactivate };
