import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export function activate(context: vscode.ExtensionContext) {
    // أمر التحويل
    const compileCmd = vscode.commands.registerCommand('daad.compileCurrentFile', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'daad') {
            vscode.window.showErrorMessage('افتح ملف ض (.ض) أولاً');
            return;
        }

        const code = editor.document.getText();
        // استدعاء المحرك المحلي (يتطلب تثبيت daad-compiler في PATH)
        const terminal = vscode.window.createTerminal('ض مترجم');
        terminal.show();
        const tempFile = path.join(path.dirname(editor.document.uri.fsPath), 'temp_daad.cpp');
        // هنا نكتب الكود لملف مؤقت ونستدعي CLI
        terminal.sendText(`daad-compiler "${editor.document.uri.fsPath}" -o "${tempFile}"`);
    });

    context.subscriptions.push(compileCmd);
}

export function deactivate() {}