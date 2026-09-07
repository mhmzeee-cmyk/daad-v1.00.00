#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

class CloudNetworkService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(int authState READ authState NOTIFY authStateChanged)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authStateChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString username READ username NOTIFY userInfoChanged)
    Q_PROPERTY(QString userRole READ userRole NOTIFY userInfoChanged)
    Q_PROPERTY(QVariantMap userProfile READ userProfile NOTIFY userInfoChanged)

public:
    enum AuthState {
        STATE_DISCONNECTED = 0,
        STATE_CONNECTING,
        STATE_AUTHENTICATING,
        STATE_CONNECTED,
        STATE_OFFLINE_MODE
    };
    Q_ENUM(AuthState)

    struct AuthToken {
        QString accessToken;
        QString refreshToken;
        qint64 expiresAt = 0;
        QString userId;
        QString username;
        QString role;
        int tokenVersion = 0;

        bool isValid() const {
            return !accessToken.isEmpty() && QDateTime::currentSecsSinceEpoch() < expiresAt;
        }
    };

    explicit CloudNetworkService(QObject *parent = nullptr);

    QString serverUrl() const { return m_serverUrl; }
    void setServerUrl(const QString &url);

    int authState() const { return static_cast<int>(m_authState); }
    bool isAuthenticated() const { return m_authState == STATE_CONNECTED; }
    QString lastError() const { return m_lastError; }
    QString username() const { return m_token.username; }
    QString userRole() const { return m_token.role; }
    QVariantMap userProfile() const { return m_userProfile; }

    Q_INVOKABLE void login(const QString &username, const QString &password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void refreshToken();
    Q_INVOKABLE void fetchCoursesAndChallenges();
    Q_INVOKABLE void syncStudentProgress(const QString &courseId, const QString &challengeId, int score);
    Q_INVOKABLE void connectToServer(const QString &url = QString());
    Q_INVOKABLE void setOfflineMode(bool offline) { m_offlineMode = offline; emit offlineModeChanged(); }
    bool isOfflineMode() const { return m_offlineMode; }

signals:
    void serverUrlChanged();
    void authStateChanged();
    void lastErrorChanged();
    void userInfoChanged();

    void loginSuccess(const QString &role, const QVariantMap &profile);
    void loginFailed(const QString &message);
    void logoutSuccess();
    void coursesLoaded(const QVariantList &courses);
    void challengesLoaded(const QVariantList &challenges);
    void progressSynced(bool success, const QString &message);
    void serverConnected();
    void serverDisconnected(const QString &reason);
    void networkError(const QString &errorString);
    void offlineModeChanged();

private slots:
    void onLoginReply(QNetworkReply *reply);
    void onRefreshReply(QNetworkReply *reply);
    void onCoursesReply(QNetworkReply *reply);
    void onProgressReply(QNetworkReply *reply);
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    void setAuthState(AuthState state);
    void setLastError(const QString &error);
    QNetworkRequest createRequest(const QString &endpoint) const;
    void saveToken();
    void loadToken();
    void handleReply(QNetworkReply *reply, std::function<void(const QJsonObject &)> callback);

    QNetworkAccessManager *m_nam;
    QString m_serverUrl;
    AuthState m_authState = STATE_DISCONNECTED;
    AuthToken m_token;
    QVariantMap m_userProfile;
    QString m_lastError;
    QTimer *m_refreshTimer;
    bool m_offlineMode = false;
};
