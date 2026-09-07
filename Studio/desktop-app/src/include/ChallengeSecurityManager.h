// ==============================================================================
// CHALLENGE SECURITY MANAGER
// C++ Qt Implementation for Cryptographic Proof of Completion
// Client-side challenge verification with HMAC-SHA256
// ==============================================================================

#ifndef CHALLENGE_SECURITY_MANAGER_H
#define CHALLENGE_SECURITY_MANAGER_H

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QProcessEnvironment>

class ChallengeSecurityManager : public QObject {
    Q_OBJECT

public:
    explicit ChallengeSecurityManager(QObject *parent = nullptr);
    ~ChallengeSecurityManager() {}

    // Generate cryptographic proof for completed challenge
    QString generateProof(int taskId, const QString &studentId, 
                          const QString &serverUrl = "http://localhost:3000/api/v1/challenge/verify");

    // Set the shared secret key for HMAC operations
    void setSharedSecret(const QByteArray &secret) { m_sharedSecret = secret; }

    // Get current Unix timestamp
    static qint64 getCurrentTimestamp() {
        return QDateTime::currentSecsSinceEpoch();
    }

signals:
    void proofGenerated(const QString &confirmationToken);
    void proofGeneratedError(const QString &error);

private:
    QByteArray m_sharedSecret;
    QNetworkAccessManager *m_networkManager;

    // Internal HMAC-SHA256 implementation for embedded environments
    QByteArray computeHmacSHA256(const QByteArray &key, const QByteArray &data) {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(key);
        hash.addData(data);
        return hash.result();
    }

    // Prepare the message format matching backend expectations
    QByteArray prepareMessage(int taskId, const QString &studentId, qint64 timestamp) {
        return QString("%1:%2:%3").arg(taskId).arg(studentId).arg(timestamp).toUtf8();
    }
};

// Implementation

ChallengeSecurityManager::ChallengeSecurityManager(QObject *parent) 
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this)) {
    QString envSecret = QProcessEnvironment::systemEnvironment().value("DHAD_HMAC_SECRET");
    if (!envSecret.isEmpty()) {
        m_sharedSecret = envSecret.toUtf8();
    } else {
        m_sharedSecret = QByteArray("DhadStudioDevFallbackKey2026", 28);
        qWarning() << "DHAD_HMAC_SECRET not set - using development fallback";
    }
}

QString ChallengeSecurityManager::generateProof(int taskId, const QString &studentId, 
                                               const QString &serverUrl) {
    qint64 timestamp = getCurrentTimestamp();
    QByteArray message = prepareMessage(taskId, studentId, timestamp);
    QByteArray signature = computeHmacSHA256(m_sharedSecret, message);

    // Convert to hex string for transmission
    QString hexSignature = signature.toHex();

    // Create JSON payload
    QJsonObject payload;
    payload["studentId"] = studentId;
    payload["taskId"] = taskId;
    payload["timestamp"] = QString::number(timestamp);
    payload["signature"] = hexSignature;

    // Send to backend asynchronously
    QJsonDocument jsonDoc(payload);
    QNetworkRequest request(QUrl(serverUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, 
                     QByteArray::number(jsonDoc.toJson().size()));

    QNetworkReply *reply = m_networkManager->post(request, jsonDoc.toJson());

    // Connect reply handlers
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument responseDoc = QJsonDocument(responseData);
            if (responseDoc.isObject()) {
                QJsonObject response = responseDoc.object();
                if (response.contains("success") && response["success"].toBool()) {
                    QString confirmationToken = response["confirmationToken"].toString();
                    emit proofGenerated(confirmationToken);
                } else {
                    emit proofGeneratedError("Server returned failure: " + 
                                            response["message"].toString());
                }
            }
        } else {
            emit proofGeneratedError("Network error: " + reply->errorString());
        }
        reply->deleteLater();
    });

    return hexSignature;
}

#endif // CHALLENGE_SECURITY_MANAGER_H
