#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantList>
#include <QVariantMap>

class ChallengeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QVariantList challenges READ challenges NOTIFY challengesChanged)
    Q_PROPERTY(QVariantList submissions READ submissions NOTIFY submissionsChanged)
    Q_PROPERTY(QVariantMap dailyChallenge READ dailyChallenge NOTIFY dailyChallengeChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString lastResult READ lastResult NOTIFY lastResultChanged)

public:
    explicit ChallengeManager(QObject *parent = nullptr);

    QString serverUrl() const { return m_serverUrl; }
    void setServerUrl(const QString &url);
    QVariantList challenges() const { return m_challenges; }
    QVariantList submissions() const { return m_submissions; }
    QVariantMap dailyChallenge() const { return m_dailyChallenge; }
    bool loading() const { return m_loading; }
    QString lastError() const { return m_lastError; }
    QString lastResult() const { return m_lastResult; }

    Q_INVOKABLE void fetchChallenges(const QString &token);
    Q_INVOKABLE void fetchDailyChallenge(const QString &token);
    Q_INVOKABLE void submitSolution(const QString &token, const QString &challengeId,
                                    const QString &code, const QString &language);
    Q_INVOKABLE void createChallenge(const QString &token, const QString &title,
                                     const QString &description, const QString &expectedOutput,
                                     const QString &difficulty);
    Q_INVOKABLE void pushToCloud(const QString &token, const QVariantMap &challenge);
    Q_INVOKABLE void fetchSubmissions(const QString &token);
    Q_INVOKABLE void clearError();

signals:
    void serverUrlChanged();
    void challengesChanged();
    void submissionsChanged();
    void dailyChallengeChanged();
    void loadingChanged();
    void lastErrorChanged();
    void lastResultChanged();
    void solutionSubmitted(bool passed, const QString &message);
    void challengeCreated(bool success, const QString &message);

private:
    void setLoading(bool l);
    void setLastError(const QString &err);
    void setLastResult(const QString &res);
    QNetworkRequest createAuthRequest(const QString &endpoint, const QString &token) const;

    QNetworkAccessManager *m_nam;
    QString m_serverUrl;
    QVariantList m_challenges;
    QVariantList m_submissions;
    QVariantMap m_dailyChallenge;
    bool m_loading = false;
    QString m_lastError;
    QString m_lastResult;
};
