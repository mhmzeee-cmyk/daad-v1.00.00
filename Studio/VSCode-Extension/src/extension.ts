import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import * as os from 'os';
import { execFile } from 'child_process';
import { DHAD_KEYWORDS } from './keywords';

// اكتشاف مترجم ض الحقيقي: مجلد المشروع أولًا ثم PATH
function findCompiler(workspaceFolder: string | undefined): string {
    const candidates: string[] = [];
    if (workspaceFolder) {
        candidates.push(path.join(workspaceFolder, 'Compiler', 'build', 'bin', 'daad-compiler'));
    }
    //احتمال فتح مجلد Studio/VSCode-Extension نفسه كمساحة عمل
    if (workspaceFolder) {
        candidates.push(path.join(workspaceFolder, '..', '..', 'Compiler', 'build', 'bin', 'daad-compiler'));
    }
    for (const c of candidates) {
        try {
            if (fs.existsSync(c)) { return c; }
        } catch { /* تجاهل */ }
    }
    return 'daad-compiler'; // الاعتماد على PATH كحل أخير
}

// صعودًا من ملف حتى جذر المستودع (مجلد Compiler/) لضبط مسارات التضمين
function findCompilerRoot(startDir: string): string | null {
    let dir = startDir;
    for (let i = 0; i < 8; i++) {
        try {
            if (fs.statSync(path.join(dir, 'Compiler')).isDirectory()) { return dir; }
        } catch { /* تابع الصعود */ }
        const parent = path.dirname(dir);
        if (parent === dir) { break; }
        dir = parent;
    }
    return null;
}
function parseDiagnostics(output: string, docUri: vscode.Uri): vscode.Diagnostic[] {
    const diags: vscode.Diagnostic[] = [];
    const re = /(?:سطر|line)\s*[:#]?\s*(\d+)/g;
    let m: RegExpExecArray | null;
    const seen = new Set<number>();
    while ((m = re.exec(output)) !== null) {
        const line = Math.max(0, parseInt(m[1], 10) - 1);
        if (seen.has(line)) { continue; }
        seen.add(line);
        const range = new vscode.Range(line, 0, line, Number.MAX_SAFE_INTEGER);
        diags.push(new vscode.Diagnostic(range, output.split('\n').slice(0, 3).join('\n'), vscode.DiagnosticSeverity.Error));
        if (seen.size >= 20) { break; }
    }
    return diags;
}

export function activate(context: vscode.ExtensionContext) {
    const diagCollection = vscode.languages.createDiagnosticCollection('daad');
    context.subscriptions.push(diagCollection);
    const output = vscode.window.createOutputChannel('ض مترجم');
    context.subscriptions.push(output);

    const compileCmd = vscode.commands.registerCommand('daad.compileCurrentFile', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'daad') {
            vscode.window.showErrorMessage('افتح ملف ض (.ض) أولاً');
            return;
        }
        const doc = editor.document;
        const ws = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
        const compiler = findCompiler(ws);
        if (compiler === 'daad-compiler') {
            vscode.window.showWarningMessage('لم يُعثر على daad-compiler في المشروع — تأكد من بنائه: cmake -S Compiler -B Compiler/build && cmake --build Compiler/build');
        }
        // المخرجات دائمًا في مجلد مؤقت — لا تلوث مجلد المصدر أبدًا
        const tmpDir = fs.mkdtempSync(path.join(os.tmpdir(), 'daad-vscode-'));
        const outCpp = path.join(tmpDir, 'out.cpp');
        output.clear();
        output.appendLine(`[ض] ${compiler} "${doc.fileName}" -o "${outCpp}"`);
        execFile(compiler, [doc.fileName, '-o', outCpp], { timeout: 60000 }, (err: Error | null, stdout: string, stderr: string) => {
            const log = `${stdout}\n${stderr}`;
            output.append(log);
            output.show(true);
            diagCollection.set(doc.uri, parseDiagnostics(log, doc.uri));
            if (err) {
                vscode.window.showErrorMessage('فشل التحويل — راجع لوحة المشاكل والمخرجات');
                return;
            }
            vscode.window.showInformationMessage(`تم التحويل بنجاح: ${outCpp}`);
        });
    });

    context.subscriptions.push(compileCmd);

    // بناء وتشغيل F9: ترجمة حقيقية ثم g++ ثم تنفيذ — المخرجات في /tmp دائمًا
    const buildRunCmd = vscode.commands.registerCommand('daad.buildAndRun', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'daad') {
            vscode.window.showErrorMessage('افتح ملف ض (.ض) أولاً');
            return;
        }
        const doc = editor.document;
        if (doc.isUntitled) {
            vscode.window.showErrorMessage('احفظ ملف ض أولاً ثم شغّله');
            return;
        }
        await doc.save();
        const ws = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
        const compiler = findCompiler(ws);
        const root = findCompilerRoot(path.dirname(doc.fileName)) ?? ws;
        const inc = root ? `-I "${root}" -I "${path.join(root, 'Compiler', 'include')}"` : '';
        const term = vscode.window.createTerminal('ض: تشغيل');
        term.show();
        // ملاحظة: مسارات التضمين تُحسم لصالح جذر المستودع عند توفره
        term.sendText(`${compiler} "${doc.fileName}" -o /tmp/daad-run.cpp && g++ -std=c++20 /tmp/daad-run.cpp -o /tmp/daad-run ${inc} && echo "─── مخرجات البرنامج ───" && /tmp/daad-run`);
    });
    context.subscriptions.push(buildRunCmd);

    // إكمال تلقائي لكل الكلمات المفتاحية (116) مع التفاصيل
    const kindMap: Record<string, vscode.CompletionItemKind> = {
        Type: vscode.CompletionItemKind.TypeParameter,
        Control: vscode.CompletionItemKind.Keyword,
        Variable: vscode.CompletionItemKind.Variable,
        Class: vscode.CompletionItemKind.Class,
        Function: vscode.CompletionItemKind.Function,
        Module: vscode.CompletionItemKind.Module,
        Builtin: vscode.CompletionItemKind.Function,
        Operator: vscode.CompletionItemKind.Operator,
        Widget: vscode.CompletionItemKind.Class,
        Library: vscode.CompletionItemKind.Reference,
    };
    const completion = vscode.languages.registerCompletionItemProvider('daad', {
        provideCompletionItems() {
            return DHAD_KEYWORDS.map((k) => {
                const item = new vscode.CompletionItem(k.word, kindMap[k.kind] ?? vscode.CompletionItemKind.Keyword);
                item.detail = k.detail;
                item.documentation = new vscode.MarkdownString(k.doc);
                return item;
            });
        },
    });
    context.subscriptions.push(completion);

    // تفاصيل عند التحويم فوق أي كلمة مفتاحية
    const hover = vscode.languages.registerHoverProvider('daad', {
        provideHover(doc, pos) {
            const range = doc.getWordRangeAtPosition(pos);
            if (!range) { return undefined; }
            const word = doc.getText(range);
            const found = DHAD_KEYWORDS.find((k) => k.word === word);
            if (!found) { return undefined; }
            return new vscode.Hover(new vscode.MarkdownString(`**${found.word}** — ${found.detail}\n\n${found.doc}`));
        },
    });
    context.subscriptions.push(hover);
}

export function deactivate() {}
