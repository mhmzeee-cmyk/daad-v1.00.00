#include "SchoolOnboarder.hpp"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QSslConfiguration>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QMimeDatabase>
#include <QFileDialog>

// ── Constructor ──────────────────────────────────────────────────────────────

SchoolOnboarder::SchoolOnboarder(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
    m_serverUrl = QString::fromUtf8("http://localhost:3000");
}

// ── Property setters ─────────────────────────────────────────────────────────

void SchoolOnboarder::setServerUrl(const QString &url)
{
    QString trimmedUrl = url.endsWith('/') ? url.chopped(1) : url;
    if (m_serverUrl == trimmedUrl) return;
    QUrl parsed(trimmedUrl);
    if (parsed.isValid() && !parsed.host().isEmpty()) {
        m_serverUrl = trimmedUrl;
        emit serverUrlChanged();
    }
}

void SchoolOnboarder::setState(State s) { if (m_state != s) { m_state = s; emit stateChanged(); } }
void SchoolOnboarder::setStep(Step s) { if (m_step != s) { m_step = s; emit stepChanged(); } }
void SchoolOnboarder::setProgress(int p) { if (m_progress != p) { m_progress = p; emit progressChanged(); } }
void SchoolOnboarder::setStatusMessage(const QString &msg) { if (m_statusMessage != msg) { m_statusMessage = msg; emit statusMessageChanged(); } }
void SchoolOnboarder::setLastError(const QString &err) { if (m_lastError != err) { m_lastError = err; emit lastErrorChanged(); } }
void SchoolOnboarder::setLoginError(const QString &err) { if (m_loginError != err) { m_loginError = err; emit loginErrorChanged(); } }
void SchoolOnboarder::setActivationCode(const QString &code) { if (m_activationCode != code) { m_activationCode = code; emit activationCodeChanged(); } }

// ── Login ────────────────────────────────────────────────────────────────────

void SchoolOnboarder::login(const QString &email, const QString &password)
{
    if (email.isEmpty() || password.isEmpty()) {
        setLoginError(QString::fromUtf8("\u0627\u0644\u0628\u0631\u064A\u062F \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A \u0648\u0643\u0644\u0645\u0629 \u0627\u0644\u0633\u0631 \u0645\u0637\u0644\u0648\u0628\u0627\u0646"));
        return;
    }

    setState(Idle);
    setStatusMessage(QString::fromUtf8("\u062C\u0627\u0631\u064D \u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062F\u062E\u0648\u0644..."));
    setLoginError(QString());

    QJsonObject payload;
    payload[QString::fromUtf8("username")] = email;
    payload[QString::fromUtf8("password")] = password;

    QJsonDocument doc(payload);
    QByteArray body = doc.toJson(QJsonDocument::Compact);

    QUrl url(m_serverUrl + QString::fromUtf8("/api/v1/auth/login"));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setTransferTimeout(30000);

    QNetworkReply *reply = m_nam->post(request, body);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            QByteArray respBody = reply->readAll();
            QJsonDocument respDoc = QJsonDocument::fromJson(respBody);
            QString errMsg = QString::fromUtf8("\u0641\u0634\u0644 \u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062F\u062E\u0648\u0644");
            if (respDoc.isObject()) {
                QJsonObject obj = respDoc.object();
                if (obj.contains(QString::fromUtf8("message")))
                    errMsg = obj[QString::fromUtf8("message")].toString();
            }
            setLoginError(errMsg);
            setStatusMessage(QString());
            return;
        }

        QJsonDocument respDoc = QJsonDocument::fromJson(reply->readAll());
        if (!respDoc.isObject()) {
            setLoginError(QString::fromUtf8("\u0627\u0633\u062A\u062C\u0627\u0628\u0629 \u063A\u0631\u064A\u0628\u0629 \u0645\u0646 \u0627\u0644\u062E\u0627\u062F\u0645"));
            return;
        }

        QJsonObject obj = respDoc.object();
        QString role = obj[QString::fromUtf8("role")].toString();
        if (role != QString::fromUtf8("ADMIN")) {
            setLoginError(QString::fromUtf8("\u0647\u0630\u0627 \u0627\u0644\u0623\u062F\u0648\u0627\u062A \u0645\u062D\u0641\u0648\u0638\u0629 \u0644\u0644\u0645\u062F\u064A\u0631\u064A\u0646 \u0641\u0642\u0637"));
            return;
        }

        m_token = obj[QString::fromUtf8("accessToken")].toString();
        m_adminName = obj[QString::fromUtf8("name")].toString();
        if (m_adminName.isEmpty())
            m_adminName = obj[QString::fromUtf8("username")].toString();
        m_schoolName.clear();
        m_filePaths.clear();
        m_teacherCount = 0;
        m_studentCount = 0;
        emit loginStateChanged();
        emit statsChanged();
        emit schoolNameChanged();

        setStep(StepSchoolInfo);
        setStatusMessage(QString::fromUtf8("\u0623\u0647\u0644\u0627\u064B \u0628\u0643 %1").arg(m_adminName));
        setProgress(5);
    });
}

void SchoolOnboarder::logout()
{
    m_token.clear();
    m_adminName.clear();
    m_activationCode.clear();
    m_filePaths.clear();
    m_teacherCount = 0;
    m_studentCount = 0;
    m_schoolName.clear();
    m_schoolStage.clear();
    m_expectedStudents = 0;
    m_lastError.clear();
    m_loginError.clear();
    setState(Idle);
    setStep(StepLogin);
    setProgress(0);
    setStatusMessage(QString());
    setLastError(QString());
    emit loginStateChanged();
    emit statsChanged();
    emit schoolNameChanged();
    emit activationCodeChanged();
}

// ── School Info ──────────────────────────────────────────────────────────────

void SchoolOnboarder::setSchoolInfo(const QString &name, const QString &stage, int expectedStudents)
{
    if (name.trimmed().isEmpty()) {
        setLastError(QString::fromUtf8("\u0627\u0633\u0645 \u0627\u0644\u0645\u062F\u0631\u0633\u0629 \u0645\u0637\u0644\u0648\u0628"));
        setState(Error);
        return;
    }
    if (name.length() > 200) {
        setLastError(QString::fromUtf8("\u0627\u0633\u0645 \u0627\u0644\u0645\u062F\u0631\u0633\u0629 \u064A\u062C\u0628 \u0623\u0646 \u064A\u0643\u0648\u0646 200 \u062D\u0631\u0641"));
        setState(Error);
        return;
    }

    m_schoolName = name.trimmed();
    m_schoolStage = stage;
    m_expectedStudents = expectedStudents;
    emit schoolNameChanged();

    setStep(StepSelectFiles);
    setStatusMessage(QString::fromUtf8("\u0627\u062E\u062A\u0631 \u0645\u0644\u0641\u0627\u062A CSV \u0623\u0648 Excel"));
    setProgress(10);
}

// ── File Selection ───────────────────────────────────────────────────────────

void SchoolOnboarder::selectFiles()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        nullptr,
        QString::fromUtf8("\u0627\u062E\u062A\u0631 \u0645\u0644\u0641\u0627\u062A \u0627\u0644\u0645\u0639\u0644\u0648\u0645\u0627\u062A"),
        QString(),
        QString::fromUtf8("\u0645\u0644\u0641\u0627\u062F\u0627\u062A \u0627\u0644\u0628\u064A\u0627\u0646\u0627\u062A (*.csv *.xlsx *.xls);;\u0643\u0644 \u0627\u0644\u0645\u0644\u0641\u0627\u062A (*)"));

    if (!filePaths.isEmpty()) {
        selectFiles(filePaths);
    }
}

void SchoolOnboarder::selectFiles(const QStringList &filePaths)
{
    m_filePaths.clear();
    m_teacherCount = 0;
    m_studentCount = 0;
    m_lastError.clear();
    setProgress(10);

    if (filePaths.isEmpty()) {
        setState(Error);
        setLastError(QString::fromUtf8("\u0644\u0645 \u064A\u062A\u0645 \u0627\u062E\u062A\u064A\u0627\u0631 \u0645\u0644\u0641"));
        return;
    }

    m_filePaths = filePaths;

    setStatusMessage(QString::fromUtf8("%1 \u0645\u0644\u0641\u0627\u062A \u0645\u062D\u062F\u062F\u0629 \u2014 \u0627\u0631\u0633\u0644 \u0644\u0644\u0633\u064A\u0631\u0641\u0631 \u0644\u0644\u062A\u062D\u0642\u0642").arg(filePaths.size()));
    setProgress(50);
    emit statsChanged();

    setStep(StepReview);
}

// ── Upload ───────────────────────────────────────────────────────────────────

void SchoolOnboarder::upload()
{
    if (m_filePaths.isEmpty()) {
        setState(Error);
        setLastError(QString::fromUtf8("\u0644\u0627 \u062A\u0648\u062C\u062F \u0645\u0644\u0641\u0627\u062A \u0644\u0644\u0631\u0641\u0639"));
        return;
    }
    if (m_token.isEmpty()) {
        setState(Error);
        setLastError(QString::fromUtf8("\u063A\u064A\u0631 \u0645\u0635\u0631\u062D \u0628\u0644\u062D\u0642 \u0627\u0644\u062F\u062E\u0648\u0644"));
        return;
    }

    setState(Uploading);
    setProgress(75);
    setStatusMessage(QString::fromUtf8("\u062C\u0627\u0631\u064D \u0631\u0641\u0639 \u0627\u0644\u0628\u064A\u0627\u0646\u0627\u062A..."));

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart namePart;
    namePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(QString::fromUtf8("form-data; name=\"schoolName\"")));
    namePart.setBody(m_schoolName.toUtf8());
    multiPart->append(namePart);

    QHttpPart stagePart;
    stagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString::fromUtf8("form-data; name=\"schoolStage\"")));
    stagePart.setBody(m_schoolStage.toUtf8());
    multiPart->append(stagePart);

    QHttpPart expectedPart;
    expectedPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant(QString::fromUtf8("form-data; name=\"expectedStudents\"")));
    expectedPart.setBody(QString::number(m_expectedStudents).toUtf8());
    multiPart->append(expectedPart);

    for (const QString &filePath : m_filePaths) {
        QFile *file = new QFile(filePath);
        if (!file->open(QIODevice::ReadOnly)) {
            delete file;
            delete multiPart;
            setState(Error);
            setLastError(QString::fromUtf8("\u0641\u0634\u0644 \u0641\u062A\u062D \u0627\u0644\u0645\u0644\u0641: ") + filePath);
            return;
        }

        QFileInfo fi(filePath);
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                          QVariant(QString::fromUtf8("form-data; name=\"files\"; filename=\"%1\"").arg(fi.fileName())));

        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(fi);
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mime.name()));
        filePart.setBodyDevice(file);
        file->setParent(multiPart);
        multiPart->append(filePart);
    }

    QUrl url(m_serverUrl + QString::fromUtf8("/api/v1/onboard-school/files"));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
    request.setRawHeader("Accept", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setTransferTimeout(30000);

    QNetworkReply *reply = m_nam->post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::uploadProgress, this, [this](qint64 sent, qint64 total) {
        if (total > 0) {
            int uploadPct = 75 + static_cast<int>((sent * 20) / total);
            setProgress(qMin(uploadPct, 95));
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QString errMsg;
            QByteArray respBody = reply->readAll();
            QJsonDocument respDoc = QJsonDocument::fromJson(respBody);
            if (respDoc.isObject()) {
                QJsonObject respObj = respDoc.object();
                if (respObj.contains(QString::fromUtf8("message")))
                    errMsg = respObj[QString::fromUtf8("message")].toString();
            }
            if (httpCode == 401 || httpCode == 403) {
                errMsg = QString::fromUtf8("\u0627\u0644\u062C\u0648\u0627\u0632 \u0635\u0644\u062D \u0623\u0648 \u0645\u0646\u0635\u062D \u0644\u0644\u0648\u0635\u0641 \u0628\u0647\u0630\u0627 \u0627\u0644\u062E\u0627\u062F\u0645");
                m_token.clear();
                emit loginStateChanged();
            }
            if (errMsg.isEmpty())
                errMsg = QString::fromUtf8("\u0641\u0634\u0644 \u0627\u0644\u0627\u062A\u0635\u0627\u0644 \u0628\u0627\u0644\u062E\u0627\u062F\u0645");
            setState(Error);
            setLastError(errMsg);
            emit uploadFailed(errMsg);
            return;
        }

        setProgress(100);

        QJsonDocument respDoc = QJsonDocument::fromJson(reply->readAll());
        QString successMsg = QString::fromUtf8("\u062A\u0645 \u0627\u0644\u0631\u0641\u0639 \u0628\u0646\u062C\u0622\u064B \u0645\u0639 \u0646\u062C\u0627\u062D!");

        if (respDoc.isObject()) {
            QJsonObject obj = respDoc.object();
            if (obj.contains(QString::fromUtf8("message")))
                successMsg = obj[QString::fromUtf8("message")].toString();
            if (obj.contains(QString::fromUtf8("stats"))) {
                QJsonObject stats = obj[QString::fromUtf8("stats")].toObject();
                m_teacherCount = stats[QString::fromUtf8("teachers")].toInt();
                m_studentCount = stats[QString::fromUtf8("students")].toInt();
                emit statsChanged();
            }
            if (obj.contains(QString::fromUtf8("warnings"))) {
                QJsonArray warns = obj[QString::fromUtf8("warnings")].toArray();
                if (!warns.isEmpty()) {
                    QStringList warnList;
                    for (const auto &w : warns) warnList.append(w.toString());
                    successMsg += QString::fromUtf8("\n\n\u062A\u0646\u0628\u064A\u0647\u0627\u062A:\n") + warnList.join(QString::fromUtf8("\n"));
                }
            }
            // Capture activation code (non-password mode)
            if (obj.contains(QString::fromUtf8("activationCode"))) {
                setActivationCode(obj[QString::fromUtf8("activationCode")].toString());
            }
        }

        setStatusMessage(successMsg);
        setState(Success);
        setStep(StepDone);
        emit uploadSuccess(successMsg);
    });
}

// ── Activation Flow ──────────────────────────────────────────────────────────

void SchoolOnboarder::startActivation()
{
    m_lastError.clear();
    m_activationCode.clear();
    emit activationCodeChanged();
    setStep(StepActivateRequest);
    setStatusMessage(QString::fromUtf8("\u0623\u062F\u062E\u0644 \u0628\u0631\u064A\u062F\u0643 \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A \u0644\u062A\u0641\u0639\u064A\u0644 \u062D\u0633\u0627\u0628\u0643"));
    setProgress(0);
}

void SchoolOnboarder::requestActivationOtp(const QString &email)
{
    if (email.trimmed().isEmpty()) {
        setLastError(QString::fromUtf8("\u0627\u0644\u0628\u0631\u064A\u062F \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A \u0645\u0637\u0644\u0648\u0628"));
        setState(Error);
        return;
    }

    setState(Idle);
    setStatusMessage(QString::fromUtf8("\u062C\u0627\u0631\u064D \u0625\u0631\u0633\u0627\u0644 \u0631\u0645\u0632 \u0627\u0644\u062A\u0641\u0639\u064A\u0644..."));
    setLastError(QString());
    setActivationCode(QString());

    QJsonObject payload;
    payload[QString::fromUtf8("identifier")] = email.trimmed().toLower();

    QJsonDocument doc(payload);
    QByteArray body = doc.toJson(QJsonDocument::Compact);

    QUrl url(m_serverUrl + QString::fromUtf8("/api/v1/auth/request-otp"));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setTransferTimeout(30000);

    QNetworkReply *reply = m_nam->post(request, body);
    connect(reply, &QNetworkReply::finished, this, [this, reply, email]() {
        reply->deleteLater();

        QByteArray respBody = reply->readAll();
        QJsonDocument respDoc = QJsonDocument::fromJson(respBody);

        if (reply->error() != QNetworkReply::NoError) {
            QString errMsg = QString::fromUtf8("\u0641\u0634\u0644 \u0625\u0631\u0633\u0627\u0644 \u0631\u0645\u0632 \u0627\u0644\u062A\u0641\u0639\u064A\u0644");
            if (respDoc.isObject()) {
                QJsonObject obj = respDoc.object();
                if (obj.contains(QString::fromUtf8("message")))
                    errMsg = obj[QString::fromUtf8("message")].toString();
            }
            setLastError(errMsg);
            setStatusMessage(QString());
            return;
        }

        QString msg = QString::fromUtf8("\u062A\u0645 \u0625\u0631\u0633\u0627\u0644 \u0631\u0645\u0632 \u0627\u0644\u062A\u0641\u0639\u064A\u0644");
        if (respDoc.isObject()) {
            QJsonObject obj = respDoc.object();
            if (obj.contains(QString::fromUtf8("message")))
                msg = obj[QString::fromUtf8("message")].toString();
            // Capture OTP from server response (demo mode)
            if (obj.contains(QString::fromUtf8("otp"))) {
                QString otp = obj[QString::fromUtf8("otp")].toString();
                if (!otp.isEmpty()) {
                    setActivationCode(otp);
                }
            }
        }

        setStep(StepActivateVerify);
        setStatusMessage(msg);
        setProgress(50);
        emit activationOtpSent(msg);
    });
}

void SchoolOnboarder::verifyActivationOtp(const QString &email, const QString &otp, const QString &password)
{
    if (email.trimmed().isEmpty() || otp.trimmed().isEmpty() || password.isEmpty()) {
        setLastError(QString::fromUtf8("\u062C\u0645\u064A\u0639 \u0627\u0644\u062D\u0642\u0648\u0644 \u0645\u0637\u0644\u0648\u0628"));
        setState(Error);
        return;
    }

    setStatusMessage(QString::fromUtf8("\u062C\u0627\u0631\u064D \u062A\u0641\u0639\u064A\u0644 \u0627\u0644\u062D\u0633\u0627\u0628..."));

    QJsonObject payload;
    payload[QString::fromUtf8("identifier")] = email.trimmed().toLower();
    payload[QString::fromUtf8("otp")] = otp.trimmed();
    payload[QString::fromUtf8("password")] = password;

    QJsonDocument doc(payload);
    QByteArray body = doc.toJson(QJsonDocument::Compact);

    QUrl url(m_serverUrl + QString::fromUtf8("/api/v1/auth/verify-otp"));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setTransferTimeout(30000);

    QNetworkReply *reply = m_nam->post(request, body);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        QByteArray respBody = reply->readAll();
        QJsonDocument respDoc = QJsonDocument::fromJson(respBody);

        if (reply->error() != QNetworkReply::NoError) {
            QString errMsg = QString::fromUtf8("\u0641\u0634\u0644 \u062A\u0641\u0639\u064A\u0644 \u0627\u0644\u062D\u0633\u0627\u0628");
            if (respDoc.isObject()) {
                QJsonObject obj = respDoc.object();
                if (obj.contains(QString::fromUtf8("message")))
                    errMsg = obj[QString::fromUtf8("message")].toString();
            }
            setLastError(errMsg);
            setStatusMessage(QString());
            return;
        }

        QString msg = QString::fromUtf8("\u062A\u0645 \u062A\u0641\u0639\u064A\u0644 \u0627\u0644\u062D\u0633\u0627\u0628 \u0628\u0646\u062C\u0627\u062D!");
        if (respDoc.isObject()) {
            QJsonObject obj = respDoc.object();
            if (obj.contains(QString::fromUtf8("message")))
                msg = obj[QString::fromUtf8("message")].toString();
        }

        setProgress(100);
        setStep(StepActivateDone);
        setStatusMessage(msg);
        setState(Success);
        emit activationSuccess(msg);
    });
}

void SchoolOnboarder::backToLogin()
{
    logout();
}

void SchoolOnboarder::backToActivateRequest()
{
    m_lastError.clear();
    setLastError(QString());
    setStep(StepActivateRequest);
    setProgress(0);
}

// ── Navigation ───────────────────────────────────────────────────────────────

void SchoolOnboarder::goBack()
{
    if (m_step == StepSelectFiles) {
        setStep(StepSchoolInfo);
    } else if (m_step == StepReview) {
        setStep(StepSelectFiles);
        m_filePaths.clear();
        m_teacherCount = 0;
        m_studentCount = 0;
        emit statsChanged();
    } else if (m_step == StepActivateVerify) {
        setStep(StepActivateRequest);
    }
}

void SchoolOnboarder::reset()
{
    m_filePaths.clear();
    m_teacherCount = 0;
    m_studentCount = 0;
    m_schoolName.clear();
    m_schoolStage.clear();
    m_expectedStudents = 0;
    m_lastError.clear();

    setState(Idle);
    if (m_token.isEmpty()) {
        setStep(StepLogin);
    } else {
        setStep(StepSchoolInfo);
    }
    setProgress(0);
    setStatusMessage(QString());
    setLastError(QString());
    emit schoolNameChanged();
    emit statsChanged();
}
