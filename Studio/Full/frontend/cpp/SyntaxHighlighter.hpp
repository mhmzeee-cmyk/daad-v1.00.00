/**
 * @file SyntaxHighlighter.hpp
 * @brief مبرّز التلوين اللوني للغة 'ض' - 110+ كلمة مفتاحية بثيم Dark IDE
 * @author فريق ض استديو
 * @version 3.0.0
 *
 * QSyntaxHighlighter مكتوب بـ C++ لأقصى أداء أثناء الكتابة السريعة.
 * regex آمن مع الأحرف العربية (حدود عبر Unicode ranges).
 * ترتيب أولويات يمنع التلوين المتقاطع.
 */

#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QVector>

class DaadHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
    Q_PROPERTY(QTextDocument* document READ document WRITE setDocument NOTIFY documentChanged)

public:
    explicit DaadHighlighter(QTextDocument* parent = nullptr);

    Q_INVOKABLE void setDocumentFromQml(QObject* quickDoc);
    QTextDocument* document() const;

signals:
    void documentChanged();

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
        int priority = 0;
    };

    QVector<HighlightRule> m_rules;

    QTextCharFormat m_keywordDataFormat;
    QTextCharFormat m_keywordControlFormat;
    QTextCharFormat m_keywordOopFormat;
    QTextCharFormat m_keywordFunctionFormat;
    QTextCharFormat m_keywordErrorFormat;
    QTextCharFormat m_keywordAdvancedFormat;
    QTextCharFormat m_keywordUiFormat;
    QTextCharFormat m_keywordArrayFormat;
    QTextCharFormat m_keywordValueFormat;
    QTextCharFormat m_operatorFormat;
    QTextCharFormat m_bracketFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_numberFormat;

    void buildRules();

    static QString arabicBoundary(const QString& word) {
        return QStringLiteral("(?<![_\\x{0600}-\\x{06FF}\\w])") +
               word +
               QStringLiteral("(?![\\x{0600}-\\x{06FF}\\w])");
    }
};
