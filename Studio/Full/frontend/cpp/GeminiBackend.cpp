/**
 * @file GeminiBackend.cpp
 * @brief تنفيذ خلفية Gemini AI - BYOK + Chat
 */

#include "GeminiBackend.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

static const QString SETTINGS_APP_NAME = "DaadStudio";
static const QString SETTINGS_ORG_NAME = "DaadStudio";
static const QString KEY_GEMINI_API = "gemini_api_key";
static const QString GEMINI_API_BASE = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=";

GeminiBackend::GeminiBackend(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

GeminiBackend::~GeminiBackend() {
    if (m_chatReply) { m_chatReply->abort(); m_chatReply->deleteLater(); }
    if (m_testReply) { m_testReply->abort(); m_testReply->deleteLater(); }
    if (m_errorReply) { m_errorReply->abort(); m_errorReply->deleteLater(); }
}

bool GeminiBackend::hasKey() const {
    QSettings settings(SETTINGS_ORG_NAME, SETTINGS_APP_NAME);
    return !settings.value(KEY_GEMINI_API).toString().trimmed().isEmpty();
}

bool GeminiBackend::isTesting() const { return m_testing; }
bool GeminiBackend::isChatBusy() const { return m_chatBusy; }
bool GeminiBackend::isErrorBusy() const { return m_errorBusy; }

QString GeminiBackend::maskedKey() const {
    QSettings settings(SETTINGS_ORG_NAME, SETTINGS_APP_NAME);
    QString key = settings.value(KEY_GEMINI_API).toString().trimmed();
    if (key.isEmpty()) return QString();
    if (key.length() <= 8) return QString(key.length(), '*');
    return key.left(4) + QString(key.length() - 8, '*') + key.right(4);
}

QVariantList GeminiBackend::chatHistory() const { return m_chatHistory; }

QString GeminiBackend::getKey() const {
    QSettings settings(SETTINGS_ORG_NAME, SETTINGS_APP_NAME);
    return settings.value(KEY_GEMINI_API).toString().trimmed();
}

void GeminiBackend::saveKey(const QString& key) {
    QString trimmed = key.trimmed();
    QSettings settings(SETTINGS_ORG_NAME, SETTINGS_APP_NAME);
    if (trimmed.isEmpty()) {
        settings.remove(KEY_GEMINI_API);
    } else {
        settings.setValue(KEY_GEMINI_API, trimmed);
    }
    settings.sync();
    emit hasKeyChanged();
    emit keySaved(!trimmed.isEmpty(), trimmed.isEmpty() ? "تم إزالة المفتاح" : "تم حفظ المفتاح بنجاح");
}

void GeminiBackend::removeKey() {
    QSettings settings(SETTINGS_ORG_NAME, SETTINGS_APP_NAME);
    settings.remove(KEY_GEMINI_API);
    settings.sync();
    emit hasKeyChanged();
    emit keySaved(true, "تم إزالة المفتاح");
}

void GeminiBackend::testConnection() {
    if (m_testing) return;
    QString key = getKey();
    if (key.isEmpty()) { emit testResult(false, "لم يتم إدخال مفتاح بعد"); return; }

    m_testing = true;
    emit testingChanged();

    QJsonObject textPart;
    textPart["text"] = QJsonValue("Say hello in one word.");
    QJsonObject bodyContent;
    bodyContent["parts"] = QJsonArray{textPart};
    QJsonObject requestBody;
    requestBody["contents"] = QJsonArray{bodyContent};

    QNetworkRequest request(QUrl(GEMINI_API_BASE + key));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_testReply = m_networkManager->post(request, QJsonDocument(requestBody).toJson());
    connect(m_testReply, &QNetworkReply::finished, this, [this]() {
        if (!m_testReply) return;
        m_testReply->deleteLater();
        m_testing = false;
        emit testingChanged();

        if (m_testReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(m_testReply->readAll());
            if (doc.isObject() && doc.object().contains("candidates")) {
                emit testResult(true, "تم الاتصال بنجاح - المفتاح صالح");
            } else {
                emit testResult(false, "الاستجابة غير متوقعة");
            }
        } else {
            int code = m_testReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (code == 400 || code == 403) emit testResult(false, "المفتاح غير صالح");
            else if (code == 429) emit testResult(false, "تم تجاوز حد الطلبات");
            else emit testResult(false, "فشل: " + m_testReply->errorString());
        }
        m_testReply = nullptr;
    });
}

void GeminiBackend::sendMessage(const QString& text, const QString& systemPrompt) {
    if (m_chatBusy) return;
    QString key = getKey();
    if (key.isEmpty()) { emit chatResponse(false, text, "الرجاء إدخال مفتاح API أولاً"); return; }

    m_chatBusy = true;
    emit chatBusyChanged();

    QVariantMap userMsg;
    userMsg["role"] = "user";
    userMsg["text"] = text;
    userMsg["time"] = QDateTime::currentDateTime().toString("HH:mm");
    m_chatHistory.append(userMsg);
    emit chatHistoryChanged();

    QJsonObject requestBody;

    if (!systemPrompt.isEmpty()) {
        QJsonObject sysPart;
        sysPart["text"] = systemPrompt;
        QJsonObject sysInstruction;
        sysInstruction["parts"] = QJsonArray{sysPart};
        requestBody["systemInstruction"] = sysInstruction;
    }

    QJsonArray contents;
    for (const auto& msg : m_chatHistory) {
        QVariantMap m = msg.toMap();
        QJsonObject part;
        part["text"] = m["text"].toString();
        QJsonObject content;
        content["role"] = m["role"].toString() == "user" ? "user" : "model";
        content["parts"] = QJsonArray{part};
        contents.append(content);
    }
    requestBody["contents"] = contents;

    QNetworkRequest request(QUrl(GEMINI_API_BASE + key));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_chatReply = m_networkManager->post(request, QJsonDocument(requestBody).toJson());
    connect(m_chatReply, &QNetworkReply::finished, this, [this, text]() {
        if (!m_chatReply) return;
        m_chatReply->deleteLater();

        if (m_chatReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(m_chatReply->readAll());
            QJsonObject root = doc.object();

            QString aiText;
            if (root.contains("candidates")) {
                QJsonArray candidates = root["candidates"].toArray();
                if (!candidates.isEmpty()) {
                    QJsonObject candidate = candidates[0].toObject();
                    if (candidate.contains("content")) {
                        QJsonObject content = candidate["content"].toObject();
                        QJsonArray parts = content["parts"].toArray();
                        if (!parts.isEmpty()) {
                            aiText = parts[0].toObject()["text"].toString();
                        }
                    }
                }
            }

            if (aiText.isEmpty()) {
                aiText = "لم أتمكن من معالجة طلبك";
            }

            QVariantMap aiMsg;
            aiMsg["role"] = "assistant";
            aiMsg["text"] = aiText;
            aiMsg["time"] = QDateTime::currentDateTime().toString("HH:mm");
            m_chatHistory.append(aiMsg);
            emit chatHistoryChanged();
            emit chatResponse(true, text, aiText);
        } else {
            QString errMsg;
            int code = m_chatReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QByteArray body = m_chatReply->readAll();
            QString bodyStr = QString::fromUtf8(body);

            QString apiMsg;
            if (bodyStr.contains("error")) {
                QJsonDocument errDoc = QJsonDocument::fromJson(body);
                if (errDoc.isObject()) {
                    QJsonObject errObj = errDoc.object().value("error").toObject();
                    apiMsg = errObj.value("message").toString();
                }
            }

            if (code == 400 || code == 403) errMsg = "المفتاح غير صالح (" + QString::number(code) + ")";
            else if (code == 429) errMsg = "تم تجاوز حد الطلبات - يرجى التحقق من الفاتورة";
            else errMsg = "خطأ " + QString::number(code) + ": " + m_chatReply->errorString();

            if (!apiMsg.isEmpty()) {
                if (apiMsg.contains("quota")) errMsg = "تم استنفاذ الحد المجاني\nيرجى تفعيل الفاتورة في Google Cloud Console";
                else if (apiMsg.contains("API key not valid")) errMsg = "مفتاح API غير صالح - يرجى التحقق من المفتاح";
                else errMsg += "\n" + apiMsg;
            }

            QVariantMap aiMsg;
            aiMsg["role"] = "assistant";
            aiMsg["text"] = errMsg;
            aiMsg["time"] = QDateTime::currentDateTime().toString("HH:mm");
            aiMsg["isError"] = true;
            m_chatHistory.append(aiMsg);
            emit chatHistoryChanged();
            emit chatResponse(false, text, errMsg);
        }

        m_chatReply = nullptr;
        m_chatBusy = false;
        emit chatBusyChanged();
    });
}

QString GeminiBackend::extractCodeBlock(const QString& response) {
    int start = response.indexOf("```");
    if (start == -1) return QString();
    int codeStart = response.indexOf("\n", start);
    if (codeStart == -1) return QString();
    codeStart++;
    int end = response.indexOf("```", codeStart);
    if (end == -1) end = response.length();
    return response.mid(codeStart, end - codeStart).trimmed();
}

void GeminiBackend::explainError(const QString& errorMessage, const QString& sourceCode, int errorLine) {
    if (m_errorBusy) return;
    QString key = getKey();
    if (key.isEmpty()) { emit errorExplained(false, "الرجاء إدخال مفتاح API أولاً", QString()); return; }

    m_errorBusy = true;
    emit errorBusyChanged();

    QString systemPrompt =
        "أنت مهندس برمجيات ومصحح أخطاء خبير في لغة البرمجة 'ض' (Daad). "
        "لغة 'ض' هي لغة عربية تترجم إلى C++20. "
        "الكلمات المفتاحية في 'ض': صحيح, عشري, منطقي, حرف, نص, ثابت, فراغ, تلقائي, "
        "صواب, خطأ, عدم, عرّف, إذا, وإلا, طالما, افعل, لكل, في, اختر, حالة, افتراضي, "
        "توقف, استمر, انتقل, صنف, هيكل, تعداد, واجهة, نطاق, عام, خاص, محمي, يرث, "
        "ذاتي, الأصل, مجرّد, دالة, ارجع, جديد, احذف, مؤشر, مرجع, ساكن, مضمن, خارجي, "
        "قالب, حاول, امسك, أخيراً, ارمِ, تأكد, استثناء, نوع_الـ, حجم_الـ, "
        "تزامن, انتظر, خيط, احجز, مشترك, فريد, استورد, صدّر, وحدة, بديل.\n"
        " print تكتب بالعربية: طباعة()\n\n"
        "قم بتحليل الخطأ المرفق بناءً على كود المستخدم واشرح المشكلة بالعربية بأسلوب تعليمي بسيط. "
        "قدم الكود البديل الصحيح تماماً بدون مقدمات طويلة.\n"
        "التنسيق المطلوب:\n"
        "1. ابدأ بـ **سبب الخطأ:** ثم شرح مبسط\n"
        "2. ثم **الكود المصحح:** ثم كود بلغة ض الصحيح بين علامتين مزدوجتين مزدوجتين\n"
        "3. لا تكتب أي نص بعد الكود المصحح";

    QString userPayload =
        "## خطأ التجميع:\n" + errorMessage + "\n\n" +
        "## كود المستخدم (سطر الخطأ: " + QString::number(errorLine) + "):\n" + sourceCode;

    QJsonObject sysPart;
    sysPart["text"] = systemPrompt;
    QJsonObject sysInstruction;
    sysInstruction["parts"] = QJsonArray{sysPart};

    QJsonObject userPart;
    userPart["text"] = userPayload;
    QJsonObject userContent;
    userContent["role"] = "user";
    userContent["parts"] = QJsonArray{userPart};

    QJsonObject requestBody;
    requestBody["systemInstruction"] = sysInstruction;
    requestBody["contents"] = QJsonArray{userContent};

    QNetworkRequest request(QUrl(GEMINI_API_BASE + key));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_errorReply = m_networkManager->post(request, QJsonDocument(requestBody).toJson());
    connect(m_errorReply, &QNetworkReply::finished, this, [this]() {
        if (!m_errorReply) return;
        m_errorReply->deleteLater();

        if (m_errorReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(m_errorReply->readAll());
            QJsonObject root = doc.object();

            QString aiText;
            if (root.contains("candidates")) {
                QJsonArray candidates = root["candidates"].toArray();
                if (!candidates.isEmpty()) {
                    QJsonObject candidate = candidates[0].toObject();
                    if (candidate.contains("content")) {
                        QJsonObject content = candidate["content"].toObject();
                        QJsonArray parts = content["parts"].toArray();
                        if (!parts.isEmpty()) {
                            aiText = parts[0].toObject()["text"].toString();
                        }
                    }
                }
            }

            if (aiText.isEmpty()) {
                emit errorExplained(false, "لم أتمكن من تحليل الخطأ", QString());
            } else {
                QString fixedCode = extractCodeBlock(aiText);
                emit errorExplained(true, aiText, fixedCode);
            }
        } else {
            QString errMsg;
            int code = m_errorReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QByteArray body = m_errorReply->readAll();
            QString bodyStr = QString::fromUtf8(body);

            QString apiMsg;
            if (bodyStr.contains("error")) {
                QJsonDocument errDoc = QJsonDocument::fromJson(body);
                if (errDoc.isObject()) {
                    QJsonObject errObj = errDoc.object().value("error").toObject();
                    apiMsg = errObj.value("message").toString();
                }
            }

            if (code == 400 || code == 403) errMsg = "المفتاح غير صالح (" + QString::number(code) + ")";
            else if (code == 429) errMsg = "تم تجاوز حد الطلبات - يرجى التحقق من الفاتورة";
            else errMsg = "خطأ " + QString::number(code) + ": " + m_errorReply->errorString();

            if (!apiMsg.isEmpty()) {
                if (apiMsg.contains("quota")) errMsg = "تم استنفاذ الحد المجاني\nيرجى تفعيل الفاتورة في Google Cloud Console";
                else if (apiMsg.contains("API key not valid")) errMsg = "مفتاح API غير صالح - يرجى التحقق من المفتاح";
                else errMsg += "\n" + apiMsg;
            }

            emit errorExplained(false, errMsg, QString());
        }

        m_errorReply = nullptr;
        m_errorBusy = false;
        emit errorBusyChanged();
    });
}

void GeminiBackend::clearChat() {
    m_chatHistory.clear();
    emit chatHistoryChanged();
}
