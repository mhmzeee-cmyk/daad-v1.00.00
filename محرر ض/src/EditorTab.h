#pragma once

#include <QPlainTextEdit>
#include <QFileInfo>
#include <QTimer>

class DaadHighlighter;
class DaadCompleter;

// EditorTab — تبويب محرر واحد RTL — تحميل/حفظ UTF-8 + تلوين 107 نوع + إكمال VS Code + تشخيص
class EditorTab : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit EditorTab(QWidget* parent = nullptr);
    explicit EditorTab(const QString& filePath, QWidget* parent = nullptr);
    ~EditorTab();

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);
    bool save(); // يحفظ في filePath الحالي

    QString filePath() const { return m_filePath; }
    QString displayName() const;
    bool isModifiedExternally() const { return false; }

signals:
    void fileSaved(const QString& path);
    void modificationChanged(bool modified);
    void diagnosticsReady(const QString& report); // للتبويب "الأخطاء"

protected:
    void keyPressEvent(QKeyEvent* e) override;

private slots:
    void onTextChanged();
    void onDiagnosticsTimeout();
    void updateCompleterModel();

private:
    void setupEditor();
    QString wordUnderCursor() const;
    QStringList collectDynamicWords() const;

    QString m_filePath;
    bool m_isUntitled = true;
    DaadHighlighter* m_highlighter = nullptr;
    DaadCompleter* m_completer = nullptr;
    QTimer m_diagTimer;
};
