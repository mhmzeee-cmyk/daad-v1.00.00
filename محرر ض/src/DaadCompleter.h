#pragma once

#include <QCompleter>
#include <QStandardItemModel>
#include <QStringList>

// DaadCompleter — إكمال VS Code-like — 3 مصادر + فازّي + أيقونات PNG
class DaadCompleter : public QCompleter {
    Q_OBJECT
public:
    explicit DaadCompleter(QObject* parent = nullptr);
    void updateForWord(const QString& prefix);
    void rebuildModel(); // يعيد بناء النموذج من الكلمات والمكتبات والسياق الديناميكي

    // لتحديث السياق الديناميكي (متغيرات/دوال الملف الحالي)
    void setDynamicWords(const QStringList& words);

private:
    QStandardItemModel* m_model = nullptr;
    QStringList m_keywords;
    QStringList m_stdlib;
    QStringList m_dynamic;
    void loadKeywords();
    void loadStdlib();
};
