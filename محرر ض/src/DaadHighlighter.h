#pragma once

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QHash>

// DaadHighlighter — تلوين 107 نوع عبر Lexer الأصلي — يدعم ثيم فاتح/غامق
class DaadHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit DaadHighlighter(QTextDocument* parent = nullptr);

public slots:
    void onThemeChanged();

protected:
    void highlightBlock(const QString& text) override;

private:
    void setupFormats();
    QHash<int, QTextCharFormat> m_formats; // key = int(TokenType)
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_numberFormat;
    bool m_isDark = false;
};
