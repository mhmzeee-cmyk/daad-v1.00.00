/**
 * @file GeminiBackend.hpp
 * @brief خلفية Gemini AI - BYOK + Chat
 * @author فريق ض استديو
 * @version 2.0.0
 */

#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

class GeminiBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasKey READ hasKey NOTIFY hasKeyChanged)
    Q_PROPERTY(bool testing READ isTesting NOTIFY testingChanged)
    Q_PROPERTY(bool chatBusy READ isChatBusy NOTIFY chatBusyChanged)
    Q_PROPERTY(bool errorBusy READ isErrorBusy NOTIFY errorBusyChanged)
    Q_PROPERTY(QString maskedKey READ maskedKey NOTIFY hasKeyChanged)
    Q_PROPERTY(QVariantList chatHistory READ chatHistory NOTIFY chatHistoryChanged)

public:
    explicit GeminiBackend(QObject* parent = nullptr);
    ~GeminiBackend() override;

    [[nodiscard]] bool hasKey() const;
    [[nodiscard]] bool isTesting() const;
    [[nodiscard]] bool isChatBusy() const;
    [[nodiscard]] bool isErrorBusy() const;
    [[nodiscard]] QString maskedKey() const;
    [[nodiscard]] QVariantList chatHistory() const;

    Q_INVOKABLE void saveKey(const QString& key);
    Q_INVOKABLE void removeKey();
    Q_INVOKABLE QString getKey() const;
    Q_INVOKABLE void testConnection();
    Q_INVOKABLE void sendMessage(const QString& text, const QString& systemPrompt);
    Q_INVOKABLE void clearChat();
    Q_INVOKABLE void explainError(const QString& errorMessage, const QString& sourceCode, int errorLine);

signals:
    void hasKeyChanged();
    void testingChanged();
    void chatBusyChanged();
    void errorBusyChanged();
    void keySaved(bool success, const QString& message);
    void testResult(bool success, const QString& message);
    void chatResponse(bool success, const QString& userMsg, const QString& aiMsg);
    void chatHistoryChanged();
    void errorExplained(bool success, const QString& explanation, const QString& fixedCode);

private:
    static QString extractCodeBlock(const QString& response);
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_chatReply = nullptr;
    QNetworkReply* m_testReply = nullptr;
    QNetworkReply* m_errorReply = nullptr;
    bool m_testing = false;
    bool m_chatBusy = false;
    bool m_errorBusy = false;
    QVariantList m_chatHistory;
};
