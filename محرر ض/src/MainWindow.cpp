#include "MainWindow.h"
#include "ThemeManager.h"
#include "FileTreeWidget.h"
#include "EditorTab.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QDockWidget>
#include <QLabel>
#include <QStatusBar>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextOption>
#include <QIcon>
#include <QFileInfo>
#include <QDir>
#include <QCloseEvent>

// MainWindow — واجهة عربية 100% RTL — بدون إيموجي — مرحلة 2: FileTree + Tabs حقيقية

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("محرر ض — Daad Editor 1.0");
    setLayoutDirection(Qt::RightToLeft);
    resize(1280, 800);

    createMenus();
    createToolBar();
    createStatusBar();
    createCentral();
    createDocks();

    ThemeManager::instance().applyToApp(qApp);
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, [this](ThemeManager::Theme){
        ThemeManager::instance().applyToApp(qApp);
        updateThemeAction();
    });
    updateThemeAction();

    // مشروع افتراضي = مجلد Examples
    m_projectRoot = "/home/mhmzeee/Downloads/daad-v1.00.00-main/Examples";
    if (m_fileTree) m_fileTree->setProjectRoot(m_projectRoot);
}

void MainWindow::createMenus() {
    QMenu* fileMenu = menuBar()->addMenu("ملف");
    QAction* newAct = fileMenu->addAction("جديد");
    newAct->setShortcut(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &MainWindow::onNewFile);

    QAction* openAct = fileMenu->addAction("فتح ملف");
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::onOpenFile);

    QAction* openFolderAct = fileMenu->addAction("فتح مجلد");
    openFolderAct->setShortcut(QKeySequence("Ctrl+Shift+O"));
    connect(openFolderAct, &QAction::triggered, this, &MainWindow::onOpenFolder);

    QAction* saveAct = fileMenu->addAction("حفظ");
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSaveFile);

    QAction* saveAsAct = fileMenu->addAction("حفظ باسم");
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::onSaveAs);

    fileMenu->addSeparator();
    QAction* exitAct = fileMenu->addAction("خروج");
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QMenu* editMenu = menuBar()->addMenu("تحرير");
    editMenu->addAction("تراجع")->setShortcut(QKeySequence::Undo);
    editMenu->addAction("إعادة")->setShortcut(QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("قص")->setShortcut(QKeySequence::Cut);
    editMenu->addAction("نسخ")->setShortcut(QKeySequence::Copy);
    editMenu->addAction("لصق")->setShortcut(QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction("بحث")->setShortcut(QKeySequence::Find);
    editMenu->addAction("استبدال");

    QMenu* viewMenu = menuBar()->addMenu("عرض");
    m_toggleThemeAct = viewMenu->addAction("تبديل الثيم (فاتح/غامق)");
    m_toggleThemeAct->setShortcut(QKeySequence("Ctrl+T"));
    connect(m_toggleThemeAct, &QAction::triggered, this, &MainWindow::onToggleTheme);
    viewMenu->addAction("إظهار شجرة الملفات");
    viewMenu->addAction("إظهار المخرجات");
    viewMenu->addAction("إظهار الصفحة الرسومية");

    QMenu* projectMenu = menuBar()->addMenu("مشروع");
    projectMenu->addAction("جديد ملف ض", this, &MainWindow::onNewFile);
    projectMenu->addAction("جديد مجلد");

    QMenu* buildMenu = menuBar()->addMenu("بناء");
    QAction* buildAct = buildMenu->addAction("بناء المشروع");
    buildAct->setShortcut(QKeySequence("Ctrl+B"));
    connect(buildAct, &QAction::triggered, this, &MainWindow::onConvertToCpp);
    QAction* convertAct = buildMenu->addAction("تحويل إلى C++");
    convertAct->setShortcut(QKeySequence("Ctrl+D"));
    connect(convertAct, &QAction::triggered, this, &MainWindow::onConvertToCpp);

    QMenu* runMenu = menuBar()->addMenu("تشغيل");
    QAction* runAct = runMenu->addAction("تشغيل");
    runAct->setShortcut(QKeySequence("F5"));
    connect(runAct, &QAction::triggered, this, &MainWindow::onRun);
    runMenu->addAction("إيقاف");
    runMenu->addAction("تنظيف");

    QMenu* toolsMenu = menuBar()->addMenu("أدوات");
    toolsMenu->addAction("إعدادات");

    QMenu* helpMenu = menuBar()->addMenu("مساعدة");
    QAction* aboutAct = helpMenu->addAction("حول محرر ض");
    connect(aboutAct, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction("توثيق اللغة");
}

void MainWindow::createToolBar() {
    QToolBar* tb = addToolBar("شريط الأدوات");
    tb->setLayoutDirection(Qt::RightToLeft);
    tb->setMovable(false);
    tb->addAction(QIcon(":/icons/new.png"), "جديد", this, &MainWindow::onNewFile);
    tb->addAction(QIcon(":/icons/open.png"), "فتح", this, &MainWindow::onOpenFile);
    tb->addAction(QIcon(":/icons/save.png"), "حفظ", this, &MainWindow::onSaveFile);
    tb->addSeparator();
    tb->addAction(QIcon(":/icons/build.png"), "بناء", this, &MainWindow::onConvertToCpp);
    tb->addAction(QIcon(":/icons/run.png"), "تشغيل", this, &MainWindow::onRun);
    tb->addAction(QIcon(":/icons/convert.png"), "تحويل", this, &MainWindow::onConvertToCpp);
    tb->addAction(QIcon(":/icons/theme.png"), "ثيم", this, &MainWindow::onToggleTheme);
}

void MainWindow::createCentral() {
    m_tabs = new QTabWidget(this);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(true);
    m_tabs->setLayoutDirection(Qt::RightToLeft);
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabClose);
    connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
    setCentralWidget(m_tabs);
    // تبويب ترحيبي
    onNewFile();
}

void MainWindow::createDocks() {
    // شجرة الملفات — يمين RTL — QFileSystemModel حقيقي
    m_fileTree = new FileTreeWidget(this);
    m_fileTree->setLayoutDirection(Qt::RightToLeft);
    connect(m_fileTree, &FileTreeWidget::fileDoubleClicked, this, &MainWindow::onFileTreeDoubleClicked);

    QDockWidget* fileDock = new QDockWidget("مستكشف المشروع", this);
    fileDock->setLayoutDirection(Qt::RightToLeft);
    fileDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    fileDock->setWidget(m_fileTree);
    addDockWidget(Qt::RightDockWidgetArea, fileDock);

    // المخرجات الثلاثة — تبويبات سفلية
    QTabWidget* outputTabs = new QTabWidget;
    outputTabs->setLayoutDirection(Qt::RightToLeft);

    m_outputDhad = new QTextEdit;
    m_outputDhad->setReadOnly(true);
    m_outputDhad->setLayoutDirection(Qt::RightToLeft);
    m_outputDhad->setPlaceholderText("ناتج ض — سيظهر هنا بعد التشغيل");
    outputTabs->addTab(m_outputDhad, "ناتج ض");

    m_outputCpp = new QTextEdit;
    m_outputCpp->setReadOnly(true);
    m_outputCpp->setLayoutDirection(Qt::LeftToRight);
    m_outputCpp->setPlaceholderText("كود C++ المحول — سيظهر هنا");
    outputTabs->addTab(m_outputCpp, "كود C++ المحول");

    m_outputErrors = new QTextEdit;
    m_outputErrors->setReadOnly(true);
    m_outputErrors->setLayoutDirection(Qt::RightToLeft);
    m_outputErrors->setPlaceholderText("الأخطاء + زمن التنفيذ — سيظهر هنا");
    outputTabs->addTab(m_outputErrors, "الأخطاء + زمن التنفيذ");

    QDockWidget* outDock = new QDockWidget("المخرجات", this);
    outDock->setLayoutDirection(Qt::RightToLeft);
    outDock->setWidget(outputTabs);
    addDockWidget(Qt::BottomDockWidgetArea, outDock);

    QDockWidget* gfxDock = new QDockWidget("المخرجات الرسومية", this);
    gfxDock->setLayoutDirection(Qt::RightToLeft);
    QTextEdit* gfxPlaceholder = new QTextEdit;
    gfxPlaceholder->setReadOnly(true);
    gfxPlaceholder->setLayoutDirection(Qt::RightToLeft);
    gfxPlaceholder->setPlainText("الصفحة الرسومية — QGraphicsView\nلعرض نواتج الصور (حمّل_صورة, ارسم_صورة, مستطيل...)\n(المرحلة 5)");
    gfxDock->setWidget(gfxPlaceholder);
    addDockWidget(Qt::BottomDockWidgetArea, gfxDock);
    tabifyDockWidget(outDock, gfxDock);
    outDock->raise();
}

void MainWindow::createStatusBar() {
    m_statusLabel = new QLabel("سطر: 1  عمود: 1", this);
    m_timeLabel = new QLabel("زمن التنفيذ: -", this);
    m_encLabel = new QLabel("UTF-8 | ض", this);
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addWidget(m_encLabel);
    statusBar()->addPermanentWidget(m_timeLabel);
    statusBar()->showMessage("جاهز — محرر ض RTL — المرحلة 2: FileTree + Tabs", 3000);
}

void MainWindow::updateThemeAction() {
    QString name = ThemeManager::instance().currentThemeName();
    m_toggleThemeAct->setText(QString("الثيم الحالي: %1 — تبديل").arg(name));
}

// ── Tabs ───────────────────────────────────────────────────────────────────

EditorTab* MainWindow::currentEditor() const {
    return qobject_cast<EditorTab*>(m_tabs->currentWidget());
}

EditorTab* MainWindow::openFileInTab(const QString& path) {
    // إن كان مفتوحاً بالفعل، انتقل إليه
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (auto* e = qobject_cast<EditorTab*>(m_tabs->widget(i))) {
            if (e->filePath() == path) { m_tabs->setCurrentIndex(i); return e; }
        }
    }
    EditorTab* editor = new EditorTab(path, this);
    connect(editor, &EditorTab::modificationChanged, this, [this, editor](bool mod){
        updateTabTitle(editor);
        Q_UNUSED(mod);
    });
    connect(editor->document(), &QTextDocument::contentsChanged, this, &MainWindow::updateCursorPosition);
    // تتبع المؤشر
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
    connect(editor, &EditorTab::diagnosticsReady, this, &MainWindow::onDiagnosticsReady);
    QFileInfo info(path);
    m_tabs->addTab(editor, info.fileName());
    m_tabs->setCurrentWidget(editor);
    statusBar()->showMessage(QString("فتح: %1").arg(path), 2000);
    return editor;
}

void MainWindow::updateTabTitle(EditorTab* editor) {
    int idx = m_tabs->indexOf(editor);
    if (idx == -1) return;
    QString name = editor->displayName();
    if (editor->document()->isModified()) name += " *";
    m_tabs->setTabText(idx, name);
}

void MainWindow::onNewFile() {
    EditorTab* editor = new EditorTab(this);
    editor->setPlainText("صحيح س = 5 ؛\nطباعة(س) ؛");
    connect(editor, &EditorTab::modificationChanged, this, [this, editor](bool){ updateTabTitle(editor); });
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
    connect(editor, &EditorTab::diagnosticsReady, this, &MainWindow::onDiagnosticsReady);
    m_tabs->addTab(editor, editor->displayName());
    m_tabs->setCurrentWidget(editor);
}

void MainWindow::onOpenFile() {
    QString path = QFileDialog::getOpenFileName(this, "فتح ملف ض", m_projectRoot, "ملفات ض (*.ض *.daad *.ضasm);;كل الملفات (*)");
    if (!path.isEmpty()) openFileInTab(path);
}

void MainWindow::onOpenFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "فتح مجلد مشروع ض", m_projectRoot);
    if (!dir.isEmpty()) {
        m_projectRoot = dir;
        m_fileTree->setProjectRoot(dir);
        statusBar()->showMessage(QString("المشروع: %1").arg(dir), 3000);
    }
}

void MainWindow::onSaveFile() {
    EditorTab* ed = currentEditor();
    if (!ed) return;
    if (ed->filePath().isEmpty()) { onSaveAs(); return; }
    if (ed->save()) {
        updateTabTitle(ed);
        statusBar()->showMessage(QString("حفظ: %1").arg(ed->filePath()), 2000);
    }
}

void MainWindow::onSaveAs() {
    EditorTab* ed = currentEditor();
    if (!ed) return;
    QString path = QFileDialog::getSaveFileName(this, "حفظ باسم", ed->filePath().isEmpty() ? m_projectRoot + "/غير محفوظ.ض" : ed->filePath(), "ملفات ض (*.ض)");
    if (!path.isEmpty() && ed->saveToFile(path)) {
        updateTabTitle(ed);
        m_tabs->setTabText(m_tabs->currentIndex(), QFileInfo(path).fileName());
        statusBar()->showMessage(QString("حفظ: %1").arg(path), 2000);
    }
}

void MainWindow::onTabClose(int index) {
    QWidget* w = m_tabs->widget(index);
    if (auto* ed = qobject_cast<EditorTab*>(w)) {
        if (ed->document()->isModified()) {
            auto ret = QMessageBox::warning(this, "حفظ التغييرات",
                QString("هل تريد حفظ التغييرات في %1 ؟").arg(ed->displayName()),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            if (ret == QMessageBox::Save) { if (!ed->save()) { if (ed->filePath().isEmpty()) onSaveAs(); return; } }
            else if (ret == QMessageBox::Cancel) return;
        }
    }
    m_tabs->removeTab(index);
    w->deleteLater();
    if (m_tabs->count() == 0) onNewFile();
}

void MainWindow::onFileTreeDoubleClicked(const QString& path) {
    openFileInTab(path);
}

void MainWindow::onCurrentTabChanged(int index) {
    Q_UNUSED(index);
    updateCursorPosition();
}

void MainWindow::updateCursorPosition() {
    if (!m_statusLabel) return;
    EditorTab* ed = currentEditor();
    if (!ed) { m_statusLabel->setText("سطر: - عمود: -"); return; }
    QTextCursor cur = ed->textCursor();
    m_statusLabel->setText(QString("سطر: %1  عمود: %2").arg(cur.blockNumber()+1).arg(cur.columnNumber()+1));
}

void MainWindow::onDiagnosticsReady(const QString& report) {
    if (m_outputErrors) m_outputErrors->setPlainText(report);
}

// ── Build/Run (مؤقت للمرحلة 4 — سيُربط DaadCore في 5) ───────────────────────

void MainWindow::onToggleTheme() {
    ThemeManager::instance().toggle();
    ThemeManager::instance().applyToApp(qApp);
}

void MainWindow::onConvertToCpp() {
    EditorTab* ed = currentEditor();
    QString code = ed ? ed->toPlainText() : "";
    Q_UNUSED(code);
    if (!m_outputCpp || !m_outputDhad || !m_outputErrors || !m_timeLabel) return;
    m_outputCpp->setPlainText("// C++ محول (المرحلة 5 ستربط DaadCore)\n#include \"Daad/Runtime/DaadRuntime.hpp\"\nint main(){\n  int س = 5;\n  daad::runtime::daad_print(س);\n  return 0;\n}");
    m_outputDhad->setPlainText("ناتج ض: 5");
    m_outputErrors->setPlainText("الأخطاء: لا يوجد\nزمن التنفيذ: 12ms (تجريبي)");
    m_timeLabel->setText("زمن التنفيذ: 12ms");
    statusBar()->showMessage("تم التحويل إلى C++", 2000);
}

void MainWindow::onRun() { onConvertToCpp(); statusBar()->showMessage("تم التشغيل — F5", 2000); }

void MainWindow::onAbout() {
    QMessageBox::about(this, "حول محرر ض",
        "محرر ض — محرر لغة ض العربي\n"
        "المرحلة 4: إكمال VS Code + تشخيص فوري\n"
        "الإكمال: Ctrl+Space أو حرفين عربيين — فازّي + أيقونات PNG\n"
        "التشخيص: سطر:عمود — رسالة عربية فورية كل 500ms\n"
        "البناء: C++20 + Qt5 5.15 — بدون إيموجي");
}
