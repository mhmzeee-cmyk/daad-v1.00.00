#include "ChallengeManager.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QSslConfiguration>

ChallengeManager::ChallengeManager(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
    m_serverUrl = QString::fromUtf8("http://localhost:3000");
}

void ChallengeManager::setServerUrl(const QString &url)
{
    if (m_serverUrl == url) return;
    m_serverUrl = url;
    emit serverUrlChanged();
}

void ChallengeManager::setLoading(bool l)
{
    if (m_loading == l) return;
    m_loading = l;
    emit loadingChanged();
}

void ChallengeManager::setLastError(const QString &err)
{
    if (m_lastError == err) return;
    m_lastError = err;
    emit lastErrorChanged();
}

void ChallengeManager::setLastResult(const QString &res)
{
    if (m_lastResult == res) return;
    m_lastResult = res;
    emit lastResultChanged();
}

QNetworkRequest ChallengeManager::createAuthRequest(const QString &endpoint, const QString &token) const
{
    QNetworkRequest request(QUrl(m_serverUrl + endpoint));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    if (!token.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
    }

    return request;
}

// ── Fetch All Challenges ─────────────────────────────────────────────────────

void ChallengeManager::fetchChallenges(const QString &token)
{
    setLoading(true);
    setLastError(QString());

    QNetworkRequest request = createAuthRequest(
        QString::fromUtf8("/api/v1/challenges"), token);

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setLoading(false);

        if (reply->error() != QNetworkReply::NoError) {
            setLastError(reply->errorString());
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        m_challenges.clear();
        if (obj.contains(QString::fromUtf8("challenges"))) {
            QJsonArray arr = obj[QString::fromUtf8("challenges")].toArray();
            for (const auto &item : arr) {
                m_challenges.append(item.toObject().toVariantMap());
            }
        }

        emit challengesChanged();
    });
}

// ── Fetch Daily Challenge ────────────────────────────────────────────────────

void ChallengeManager::fetchDailyChallenge(const QString &token)
{
    setLoading(true);
    setLastError(QString());

    QNetworkRequest request = createAuthRequest(
        QString::fromUtf8("/api/v1/challenges/daily"), token);

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setLoading(false);

        if (reply->error() != QNetworkReply::NoError) {
            setLastError(reply->errorString());
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        if (obj.contains(QString::fromUtf8("challenge"))) {
            m_dailyChallenge = obj[QString::fromUtf8("challenge")].toObject().toVariantMap();
        } else {
            m_dailyChallenge = obj.toVariantMap();
        }

        emit dailyChallengeChanged();
    });
}

// ── Submit Solution ──────────────────────────────────────────────────────────

void ChallengeManager::submitSolution(const QString &token, const QString &challengeId,
                                      const QString &code, const QString &language)
{
    setLoading(true);
    setLastError(QString());
    setLastResult(QString());

    QJsonObject body;
    body[QString::fromUtf8("challengeId")] = challengeId;
    body[QString::fromUtf8("code")] = code;
    body[QString::fromUtf8("language")] = language.isEmpty()
        ? QString::fromUtf8("daad") : language;

    QNetworkRequest request = createAuthRequest(
        QString::fromUtf8("/api/v1/submit-solution"), token);

    QNetworkReply *reply = m_nam->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setLoading(false);

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        if (reply->error() != QNetworkReply::NoError) {
            QString msg = obj[QString::fromUtf8("message")].toString(
                reply->errorString());
            setLastError(msg);
            emit solutionSubmitted(false, msg);
            return;
        }

        bool passed = obj[QString::fromUtf8("passed")].toBool(false);
        QString message = obj[QString::fromUtf8("message")].toString(
            QString::fromUtf8("\u062A\u0645 \u0627\u0644\u0625\u0631\u0633\u0627\u0644"));

        setLastResult(message);
        emit solutionSubmitted(passed, message);
    });
}

void ChallengeManager::clearError()
{
    if (!m_lastError.isEmpty()) {
        m_lastError.clear();
        emit lastErrorChanged();
    }
}

// ── Create Challenge (Teacher) ───────────────────────────────────────────────

void ChallengeManager::createChallenge(const QString &token, const QString &title,
                                       const QString &description, const QString &expectedOutput,
                                       const QString &difficulty)
{
    setLoading(true);
    setLastError(QString());

    QJsonObject body;
    body[QString::fromUtf8("title")] = title;
    body[QString::fromUtf8("description")] = description;
    body[QString::fromUtf8("expectedOutput")] = expectedOutput;
    body[QString::fromUtf8("difficulty")] = difficulty.isEmpty()
        ? QString::fromUtf8("medium") : difficulty;

    QNetworkRequest request = createAuthRequest(
        QString::fromUtf8("/api/v1/challenges"), token);

    QNetworkReply *reply = m_nam->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setLoading(false);

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        if (reply->error() != QNetworkReply::NoError) {
            QString msg = obj[QString::fromUtf8("message")].toString(
                reply->errorString());
            setLastError(msg);
            emit challengeCreated(false, msg);
            return;
        }

        QString message = obj[QString::fromUtf8("message")].toString(
            QString::fromUtf8("\u062A\u0645 \u0625\u0646\u0634\u0627\u0621 \u0627\u0644\u062A\u062D\u062F\u064A"));

        setLastResult(message);
        emit challengeCreated(true, message);
    });
}

// ── Push to Cloud ────────────────────────────────────────────────────────────

void ChallengeManager::pushToCloud(const QString &token, const QVariantMap &challenge)
{
    setLoading(true);
    setLastError(QString());

    QJsonObject body = QJsonObject::fromVariantMap(challenge);

    QNetworkRequest request = createAuthRequest(
        QString::fromUtf8("/api/v1/challenges/push"), token);

    QNetworkReply *reply = m_nam->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setLoading(false);

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        if (reply->error() != QNetworkReply::NoError) {
            QString msg = obj[QString::fromUtf8("message")].toString(
                reply->errorString());
            setLastError(msg);
            emit challengeCreated(false, msg);
            return;
        }

        QString message = obj[QString::fromUtf8("message")].toString(
            QString::fromUtf8("\u062A\u0645 \u0627\u0644\u0631\u0641\u0639 \u0625\u0644\u0649 \u0627\u0644\u0633\u0631\u0639\u0629"));

        setLastResult(message);
        emit challengeCreated(true, message);
    });
}

// ── Fetch Submissions (Teacher sees students' work) ──────────────────────────

void ChallengeManager::fetchSubmissions(const QString &token)
{
    setLoading(true);
    setLastError(QString());

    QNetworkRequest request = createAuthRequest(
        QString::fromUtf8("/api/v1/submissions"), token);

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setLoading(false);

        if (reply->error() != QNetworkReply::NoError) {
            setLastError(reply->errorString());
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        m_submissions.clear();
        if (obj.contains(QString::fromUtf8("submissions"))) {
            QJsonArray arr = obj[QString::fromUtf8("submissions")].toArray();
            for (const auto &item : arr) {
                m_submissions.append(item.toObject().toVariantMap());
            }
        }

        emit submissionsChanged();
    });
}
