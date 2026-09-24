#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QQmlEngine>

class Translator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY languageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages CONSTANT)
    Q_PROPERTY(int languageVersion READ languageVersion NOTIFY languageChanged)

public:
    explicit Translator(QObject* parent = nullptr);

    QString currentLanguage() const { return m_currentLang; }
    void setCurrentLanguage(const QString& lang);
    QStringList availableLanguages() const;
    int languageVersion() const { return m_langVersion; }

    Q_INVOKABLE QString translate(const QString& key) const;

signals:
    void languageChanged();

private:
    void loadTranslations(const QString& lang);
    QString m_currentLang = "ar";
    int m_langVersion = 0;
    QMap<QString, QMap<QString, QString>> m_translations;

    struct LangInfo {
        QString code;
        QString nativeName;
        QString englishName;
    };
    static const QVector<LangInfo>& languageList();
};
