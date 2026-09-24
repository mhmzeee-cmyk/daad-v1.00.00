#include "CloudNetworkService.hpp"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QSettings>
#include <QDateTime>
#include <QSslConfiguration>
#include <QSslError>

CloudNetworkService::CloudNetworkService(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
    , m_refreshTimer(new QTimer(this))
{
    m_serverUrl = QString::fromUtf8("http://localhost:3000");

    connect(m_nam, &QNetworkAccessManager::sslErrors, this, &CloudNetworkService::onSslErrors);

    m_refreshTimer->setSingleShot(true);
    connect(m_refreshTimer, &QTimer::timeout, this, &CloudNetworkService::refreshToken);

    loadToken();
}

void CloudNetworkService::setServerUrl(const QString &url)
{
    if (m_serverUrl == url) return;
    m_serverUrl = url;
    emit serverUrlChanged();
}

void CloudNetworkService::connectToServer(const QString &url)
{
    if (!url.isEmpty()) {
        setServerUrl(url);
    }

    if (m_serverUrl.isEmpty()) {
        setLastError(QString::fromUtf8("\u0639\u0646\u0648\u0627\u0646 \u0627\u0644\u062E\u0627\u062F\u0645 \u063A\u064A\u0631 \u0645\u062D\u062F\u062F"));
        return;
    }

    setAuthState(STATE_CONNECTING);

    QNetworkRequest request(QUrl(m_serverUrl + QString::fromUtf8("/api/health")));
    request.setRawHeader("Content-Type", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() == QNetworkReply::NoError) {
            setAuthState(STATE_DISCONNECTED);
            emit serverConnected();
        } else {
            setAuthState(STATE_DISCONNECTED);
            setLastError(reply->errorString());
            emit serverDisconnected(reply->errorString());
        }
    });
}

void CloudNetworkService::login(const QString &username, const QString &password)
{
    if (m_serverUrl.isEmpty()) {
        setLastError(QString::fromUtf8("\u0639\u0646\u0648\u0627\u0646 \u0627\u0644\u062E\u0627\u062F\u0645 \u063A\u064A\u0631 \u0645\u062D\u062F\u062F"));
        emit loginFailed(m_lastError);
        return;
    }

    setAuthState(STATE_AUTHENTICATING);

    QJsonObject body;
    body[QString::fromUtf8("username")] = username;
    body[QString::fromUtf8("password")] = password;

    QNetworkRequest request(QUrl(m_serverUrl + QString::fromUtf8("/api/v1/auth/login")));
    request.setRawHeader("Content-Type", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    QNetworkReply *reply = m_nam->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onLoginReply(reply);
    });
}

void CloudNetworkService::onLoginReply(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QString errorMsg = QString::fromUtf8("\u0641\u0634\u0644 \u0627\u0644\u062A\u0633\u062C\u064A\u0644");

        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains(QString::fromUtf8("message"))) {
                errorMsg = obj[QString::fromUtf8("message")].toString();
            }
        }

        setAuthState(STATE_DISCONNECTED);
        setLastError(errorMsg);
        emit loginFailed(errorMsg);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();

    if (!obj.contains(QString::fromUtf8("accessToken"))) {
        setAuthState(STATE_DISCONNECTED);
        setLastError(QString::fromUtf8("\u0627\u0633\u062A\u062C\u0627\u0628 \u063A\u064A\u0631 \u0635\u062D\u064A\u062D \u0645\u0646 \u0627\u0644\u062E\u0627\u062F\u0645"));
        emit loginFailed(m_lastError);
        return;
    }

    m_token.accessToken = obj[QString::fromUtf8("accessToken")].toString();
    m_token.refreshToken = obj[QString::fromUtf8("refreshToken")].toString();
    m_token.expiresAt = QDateTime::currentSecsSinceEpoch() + obj[QString::fromUtf8("expiresIn")].toInt(3600);
    m_token.userId = obj[QString::fromUtf8("userId")].toString();
    m_token.username = obj[QString::fromUtf8("username")].toString();
    m_token.role = obj[QString::fromUtf8("role")].toString();
    m_token.tokenVersion = obj[QString::fromUtf8("tokenVersion")].toInt(1);

    if (obj.contains(QString::fromUtf8("profile"))) {
        m_userProfile = obj[QString::fromUtf8("profile")].toObject().toVariantMap();
    } else {
        m_userProfile[QString::fromUtf8("username")] = m_token.username;
        m_userProfile[QString::fromUtf8("fullName")] = m_token.username;
    }

    saveToken();
    setAuthState(STATE_CONNECTED);
    emit userInfoChanged();
    emit loginSuccess(m_token.role, m_userProfile);

    if (m_token.expiresAt > 0) {
        qint64 msUntilRefresh = (m_token.expiresAt - QDateTime::currentSecsSinceEpoch() - 300) * 1000;
        if (msUntilRefresh > 0) {
            m_refreshTimer->start(msUntilRefresh);
        }
    }
}

void CloudNetworkService::logout()
{
    m_token = AuthToken();
    m_userProfile.clear();
    m_refreshTimer->stop();

    QSettings settings;
    settings.remove(QString::fromUtf8("cloud/token"));
    settings.remove(QString::fromUtf8("cloud/refreshToken"));

    setAuthState(STATE_DISCONNECTED);
    emit userInfoChanged();
    emit logoutSuccess();
}

void CloudNetworkService::refreshToken()
{
    if (m_token.refreshToken.isEmpty()) return;
    if (m_serverUrl.isEmpty()) return;

    QJsonObject body;
    body[QString::fromUtf8("refreshToken")] = m_token.refreshToken;

    QNetworkRequest request(QUrl(m_serverUrl + QString::fromUtf8("/api/auth/refresh")));
    request.setRawHeader("Content-Type", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    QNetworkReply *reply = m_nam->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onRefreshReply(reply);
    });
}

void CloudNetworkService::onRefreshReply(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        logout();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();

    m_token.accessToken = obj[QString::fromUtf8("accessToken")].toString();
    m_token.expiresAt = QDateTime::currentSecsSinceEpoch() + obj[QString::fromUtf8("expiresIn")].toInt(3600);

    if (obj.contains(QString::fromUtf8("refreshToken"))) {
        m_token.refreshToken = obj[QString::fromUtf8("refreshToken")].toString();
    }

    saveToken();

    qint64 msUntilRefresh = (m_token.expiresAt - QDateTime::currentSecsSinceEpoch() - 300) * 1000;
    if (msUntilRefresh > 0) {
        m_refreshTimer->start(msUntilRefresh);
    }
}

void CloudNetworkService::fetchCoursesAndChallenges()
{
    if (!m_token.isValid()) {
        setLastError(QString::fromUtf8("\u0627\u0644\u062C\u0644\u0633\u0629 \u063A\u064A\u0631 \u0645\u0635\u0631\u062D\u0629"));
        return;
    }

    QNetworkRequest request = createRequest(QString::fromUtf8("/api/courses"));
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onCoursesReply(reply);
    });
}

void CloudNetworkService::onCoursesReply(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        setLastError(reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();

    QVariantList courses;
    if (obj.contains(QString::fromUtf8("courses"))) {
        QJsonArray arr = obj[QString::fromUtf8("courses")].toArray();
        for (const auto &item : arr) {
            courses.append(item.toObject().toVariantMap());
        }
    }

    QVariantList challenges;
    if (obj.contains(QString::fromUtf8("challenges"))) {
        QJsonArray arr = obj[QString::fromUtf8("challenges")].toArray();
        for (const auto &item : arr) {
            challenges.append(item.toObject().toVariantMap());
        }
    }

    emit coursesLoaded(courses);
    emit challengesLoaded(challenges);
}

void CloudNetworkService::syncStudentProgress(const QString &courseId, const QString &challengeId, int score)
{
    if (!m_token.isValid()) {
        emit progressSynced(false, QString::fromUtf8("\u0627\u0644\u062C\u0644\u0633\u0629 \u063A\u064A\u0631 \u0645\u0635\u0631\u062D\u0629"));
        return;
    }

    QJsonObject body;
    body[QString::fromUtf8("courseId")] = courseId;
    body[QString::fromUtf8("challengeId")] = challengeId;
    body[QString::fromUtf8("score")] = score;

    QNetworkRequest request = createRequest(QString::fromUtf8("/api/progress/sync"));
    QNetworkReply *reply = m_nam->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onProgressReply(reply);
    });
}

void CloudNetworkService::onProgressReply(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit progressSynced(false, reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = doc.object();
    QString message = obj[QString::fromUtf8("message")].toString(QString::fromUtf8("\u062A\u0645 \u0627\u0644\u0645\u0632\u0627\u0645\u0646\u0629 \u0628\u0646\u062C\u0627\u062D"));
    emit progressSynced(true, message);
}

QNetworkRequest CloudNetworkService::createRequest(const QString &endpoint) const
{
    QNetworkRequest request(QUrl(m_serverUrl + endpoint));
    request.setRawHeader("Content-Type", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    if (!m_token.accessToken.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_token.accessToken).toUtf8());
    }

    return request;
}

void CloudNetworkService::setAuthState(AuthState state)
{
    if (m_authState == state) return;
    m_authState = state;
    emit authStateChanged();
}

void CloudNetworkService::setLastError(const QString &error)
{
    if (m_lastError == error) return;
    m_lastError = error;
    emit lastErrorChanged();
    emit networkError(error);
}

void CloudNetworkService::saveToken()
{
    QSettings settings;
    settings.setValue(QString::fromUtf8("cloud/token"), m_token.accessToken);
    settings.setValue(QString::fromUtf8("cloud/refreshToken"), m_token.refreshToken);
    settings.setValue(QString::fromUtf8("cloud/expiresAt"), m_token.expiresAt);
    settings.setValue(QString::fromUtf8("cloud/userId"), m_token.userId);
    settings.setValue(QString::fromUtf8("cloud/username"), m_token.username);
    settings.setValue(QString::fromUtf8("cloud/role"), m_token.role);
}

void CloudNetworkService::loadToken()
{
    QSettings settings;
    m_token.accessToken = settings.value(QString::fromUtf8("cloud/token")).toString();
    m_token.refreshToken = settings.value(QString::fromUtf8("cloud/refreshToken")).toString();
    m_token.expiresAt = settings.value(QString::fromUtf8("cloud/expiresAt")).toLongLong();
    m_token.userId = settings.value(QString::fromUtf8("cloud/userId")).toString();
    m_token.username = settings.value(QString::fromUtf8("cloud/username")).toString();
    m_token.role = settings.value(QString::fromUtf8("cloud/role")).toString();

    if (m_token.isValid()) {
        setAuthState(STATE_CONNECTED);
        m_userProfile[QString::fromUtf8("username")] = m_token.username;
        m_userProfile[QString::fromUtf8("fullName")] = m_token.username;
        emit userInfoChanged();

        qint64 msUntilRefresh = (m_token.expiresAt - QDateTime::currentSecsSinceEpoch() - 300) * 1000;
        if (msUntilRefresh > 0) {
            m_refreshTimer->start(msUntilRefresh);
        } else {
            refreshToken();
        }
    } else if (!m_token.refreshToken.isEmpty()) {
        refreshToken();
    }
}

void CloudNetworkService::handleReply(QNetworkReply *reply, std::function<void(const QJsonObject &)> callback)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        setLastError(reply->errorString());
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isObject()) {
        callback(doc.object());
    }
}

void CloudNetworkService::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(reply);
    for (const auto &error : errors) {
        fprintf(stderr, "[CloudService] SSL Error: %s\n", error.errorString().toUtf8().constData());
    }
}
