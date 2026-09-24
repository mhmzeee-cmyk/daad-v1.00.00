#include "EditorTab.h"
#include "DaadHighlighter.h"
#include "DaadCompleter.h"
#include "Daad/Lexer.hpp"
#include "Daad/Parser.hpp"
#include "Daad/Diagnostics.hpp"
#include <QFile>
#include <QTextStream>
#include <QTextOption>
#include <QFileInfo>
#include <QMessageBox>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTextCursor>

EditorTab::EditorTab(QWidget* parent) : QPlainTextEdit(parent) {
    setupEditor();
    m_isUntitled = true;
    m_highlighter = new DaadHighlighter(document());
    m_completer = new DaadCompleter(this);
    m_completer->setWidget(this);
    m_diagTimer.setSingleShot(true);
    m_diagTimer.setInterval(500);
    connect(&m_diagTimer, &QTimer::timeout, this, &EditorTab::onDiagnosticsTimeout);
}

EditorTab::EditorTab(const QString& filePath, QWidget* parent) : QPlainTextEdit(parent) {
    setupEditor();
    m_highlighter = new DaadHighlighter(document());
    m_completer = new DaadCompleter(this);
    m_completer->setWidget(this);
    m_diagTimer.setSingleShot(true);
    m_diagTimer.setInterval(500);
    connect(&m_diagTimer, &QTimer::timeout, this, &EditorTab::onDiagnosticsTimeout);
    loadFromFile(filePath);
}

EditorTab::~EditorTab() {}

void EditorTab::setupEditor() {
    setLayoutDirection(Qt::RightToLeft);
    QTextOption opt = document()->defaultTextOption();
    opt.setTextDirection(Qt::RightToLeft);
    opt.setAlignment(Qt::AlignRight);
    document()->setDefaultTextOption(opt);
    QFont f("Noto Naskh Arabic", 12);
    if (f.family() != "Noto Naskh Arabic") f = QFont("Tahoma", 10);
    setFont(f);
    setLineWrapMode(QPlainTextEdit::WidgetWidth);
    setPlaceholderText("اكتب كود ض هنا من اليمين إلى اليسار... (Ctrl+Space للإكمال)");
    connect(document(), &QTextDocument::modificationChanged, this, &EditorTab::modificationChanged);
    connect(this, &QPlainTextEdit::textChanged, this, &EditorTab::onTextChanged);
}

bool EditorTab::loadFromFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts.setAutoDetectUnicode(true);
    setPlainText(ts.readAll());
    m_filePath = path;
    m_isUntitled = false;
    document()->setModified(false);
    updateCompleterModel();
    m_diagTimer.start();
    return true;
}

bool EditorTab::saveToFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << toPlainText();
    f.close();
    m_filePath = path;
    m_isUntitled = false;
    document()->setModified(false);
    emit fileSaved(path);
    return true;
}

bool EditorTab::save() {
    if (m_isUntitled || m_filePath.isEmpty()) return false;
    return saveToFile(m_filePath);
}

QString EditorTab::displayName() const {
    if (m_isUntitled || m_filePath.isEmpty()) return "غير محفوظ.ض";
    return QFileInfo(m_filePath).fileName();
}

void EditorTab::onTextChanged() {
    updateCompleterModel();
    m_diagTimer.start();
}

// ── إكمال تلقائي VS Code-like ────────────────────────────────────────────

QString EditorTab::wordUnderCursor() const {
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QStringList EditorTab::collectDynamicWords() const {
    QStringList words;
    QSet<QString> seen;
    std::string utf8 = toPlainText().toStdString();
    if (utf8.empty()) return words;
    daad::Lexer lexer(utf8);
    daad::Token tok;
    while ((tok = lexer.getNextToken()).type != daad::TokenType::TOKEN_EOF) {
        if (tok.type == daad::TokenType::TOKEN_IDENTIFIER) {
            QString w = QString::fromStdString(tok.text);
            if (!seen.contains(w) && w.length() >= 2) { seen.insert(w); words << w; }
        }
    }
    return words;
}

void EditorTab::updateCompleterModel() {
    if (!m_completer) return;
    m_completer->setDynamicWords(collectDynamicWords());
}

void EditorTab::keyPressEvent(QKeyEvent* e) {
    if (m_completer && m_completer->popup()->isVisible()) {
        switch (e->key()) {
            case Qt::Key_Enter: case Qt::Key_Return: case Qt::Key_Escape: case Qt::Key_Tab: case Qt::Key_Backtab:
                e->ignore(); return;
            default: break;
        }
    }
    bool isShortcut = (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space;
    QPlainTextEdit::keyPressEvent(e);

    if (!m_completer) return;
    bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (ctrlOrShift && e->text().isEmpty()) return;

    QString prefix = wordUnderCursor();
    // تلقائي بعد حرفين عربيين أو إنجليزيين
    bool autoTrigger = !isShortcut && prefix.length() >= 2;
    if (!isShortcut && e->text().isEmpty()) autoTrigger = false;

    if (isShortcut || autoTrigger) {
        m_completer->updateForWord(prefix);
        // إظهار Popup عند موقع المؤشر
        QRect cr = cursorRect();
        cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
        m_completer->complete(cr);
    } else {
        m_completer->popup()->hide();
    }
}

// ── تشخيص فوري ──────────────────────────────────────────────────────────

void EditorTab::onDiagnosticsTimeout() {
    QString text = toPlainText();
    std::string utf8 = text.toStdString();
    daad::DiagnosticsEngine diag;
    daad::Lexer lexer(utf8);
    daad::Parser parser(lexer, diag);
    // حاول تحليل كل الجمل لجمع التشخيصات
    while (true) {
        auto stmt = parser.parseStatement();
        if (!stmt) { if (parser.isAtEnd()) break; continue; }
    }
    QString report;
    if (!diag.hasErrors() && diag.getDiagnostics().empty()) {
        report = "لا يوجد أخطاء — جاهز للبناء";
    } else {
        for (auto& d : diag.getDiagnostics()) {
            QString sev = (d.severity == daad::Severity::Error) ? "خطأ" : (d.severity == daad::Severity::Warning ? "تحذير" : "ملاحظة");
            report += QString("[%1] سطر %2:%3 — %4\n").arg(sev).arg(d.line).arg(d.column).arg(QString::fromStdString(d.message));
        }
    }
    emit diagnosticsReady(report);
}
