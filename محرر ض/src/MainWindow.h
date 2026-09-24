#pragma once

#include <QMainWindow>

class QLabel;
class QTextEdit;
class QTabWidget;
class FileTreeWidget;
class EditorTab;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onNewFile();
    void onOpenFile();
    void onOpenFolder();
    void onSaveFile();
    void onSaveAs();
    void onToggleTheme();
    void onConvertToCpp();
    void onRun();
    void onAbout();
    void onTabClose(int index);
    void onFileTreeDoubleClicked(const QString& path);
    void onCurrentTabChanged(int index);
    void updateCursorPosition();
    void onDiagnosticsReady(const QString& report);

private:
    void createMenus();
    void createToolBar();
    void createCentral();
    void createDocks();
    void createStatusBar();
    void updateThemeAction();
    EditorTab* currentEditor() const;
    EditorTab* openFileInTab(const QString& path);
    void updateTabTitle(EditorTab* editor);

    QTabWidget* m_tabs = nullptr;
    FileTreeWidget* m_fileTree = nullptr;
    QTextEdit* m_outputDhad = nullptr;
    QTextEdit* m_outputCpp = nullptr;
    QTextEdit* m_outputErrors = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_timeLabel = nullptr;
    QLabel* m_encLabel = nullptr;
    QAction* m_toggleThemeAct = nullptr;
    QString m_projectRoot;
};
