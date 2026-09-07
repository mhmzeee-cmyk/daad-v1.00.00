#pragma once

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>

class SchoolOnboarder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int step READ step NOTIFY stepChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString schoolName READ schoolName NOTIFY schoolNameChanged)
    Q_PROPERTY(int totalStudents READ totalStudents NOTIFY statsChanged)
    Q_PROPERTY(int totalTeachers READ totalTeachers NOTIFY statsChanged)
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY loginStateChanged)
    Q_PROPERTY(QString loginError READ loginError NOTIFY loginErrorChanged)
    Q_PROPERTY(QString adminName READ adminName NOTIFY loginStateChanged)
    Q_PROPERTY(QString activationCode READ activationCode NOTIFY activationCodeChanged)

public:
    enum State { Idle = 0, ReadingFiles, Uploading, Success, Error };
    Q_ENUM(State)

    enum Step {
        StepLogin = 0,
        StepSchoolInfo,
        StepSelectFiles,
        StepReview,
        StepDone,
        StepActivateRequest,
        StepActivateVerify,
        StepActivateDone
    };
    Q_ENUM(Step)

    explicit SchoolOnboarder(QObject *parent = nullptr);

    State state() const { return m_state; }
    int step() const { return m_step; }
    int progress() const { return m_progress; }
    QString statusMessage() const { return m_statusMessage; }
    QString schoolName() const { return m_schoolName; }
    int totalStudents() const { return m_studentCount; }
    int totalTeachers() const { return m_teacherCount; }
    QString serverUrl() const { return m_serverUrl; }
    void setServerUrl(const QString &url);
    QString lastError() const { return m_lastError; }
    bool isAdmin() const { return !m_token.isEmpty(); }
    QString loginError() const { return m_loginError; }
    QString adminName() const { return m_adminName; }
    QString activationCode() const { return m_activationCode; }

    Q_INVOKABLE void login(const QString &email, const QString &password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void setSchoolInfo(const QString &name, const QString &stage, int expectedStudents);
    Q_INVOKABLE void selectFiles();
    Q_INVOKABLE void selectFiles(const QStringList &filePaths);
    Q_INVOKABLE void upload();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void goBack();

    // Activation flow
    Q_INVOKABLE void startActivation();
    Q_INVOKABLE void requestActivationOtp(const QString &email);
    Q_INVOKABLE void verifyActivationOtp(const QString &email, const QString &otp, const QString &password);
    Q_INVOKABLE void backToLogin();
    Q_INVOKABLE void backToActivateRequest();

signals:
    void stateChanged();
    void stepChanged();
    void progressChanged();
    void statusMessageChanged();
    void schoolNameChanged();
    void statsChanged();
    void serverUrlChanged();
    void lastErrorChanged();
    void loginStateChanged();
    void loginErrorChanged();
    void uploadSuccess(const QString &message);
    void uploadFailed(const QString &error);
    void activationOtpSent(const QString &message);
    void activationSuccess(const QString &message);
    void activationCodeChanged();

private:
    void setState(State s);
    void setStep(Step s);
    void setProgress(int p);
    void setStatusMessage(const QString &msg);
    void setLastError(const QString &err);
    void setLoginError(const QString &err);
    void setActivationCode(const QString &code);

    QNetworkAccessManager *m_nam;
    State m_state = Idle;
    Step m_step = StepLogin;
    int m_progress = 0;
    QString m_statusMessage;
    QString m_schoolName;
    QString m_schoolStage;
    int m_expectedStudents = 0;
    QString m_serverUrl;
    QString m_lastError;
    QString m_loginError;
    QString m_token;
    QString m_adminName;
    QString m_activationCode;

    QStringList m_filePaths;
    int m_teacherCount = 0;
    int m_studentCount = 0;
};
