#include "DaadHighlighter.h"
#include "ThemeManager.h"
#include "Daad/Lexer.hpp"
#include "Daad/Token.hpp"
#include <QTextDocument>

DaadHighlighter::DaadHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
    // استمع لتغيير الثيم
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &DaadHighlighter::onThemeChanged);
    m_isDark = (ThemeManager::instance().currentTheme() == ThemeManager::Theme::Dark);
    setupFormats();
}

void DaadHighlighter::onThemeChanged() {
    m_isDark = (ThemeManager::instance().currentTheme() == ThemeManager::Theme::Dark);
    setupFormats();
    rehighlight();
}

void DaadHighlighter::setupFormats() {
    m_formats.clear();
    auto fmt = [&](QColor fg, bool bold=false, bool italic=false){
        QTextCharFormat f; f.setForeground(fg); if(bold) f.setFontWeight(QFont::Bold); if(italic) f.setFontItalic(true); return f;
    };
    // ألوان ثيمية
    QColor cType = m_isDark ? QColor("#268bd2") : QColor("#268bd2"); // أنواع زرقاء ثابتة
    QColor cControl = m_isDark ? QColor("#dc322f") : QColor("#d33682"); // تحكم وردي/أحمر
    QColor cOOP = QColor("#b58900"); // أصناف أصفر
    QColor cFunc = m_isDark ? QColor("#859900") : QColor("#859900");
    QColor cLiteral = QColor("#2aa198");
    QColor cString = QColor("#2aa198");
    QColor cNumber = QColor("#859900");
    QColor cIdent = m_isDark ? QColor("#839496") : QColor("#657b83");
    QColor cOp = QColor("#93a1a1");
    QColor cComment = QColor("#93a1a1");

    m_commentFormat = fmt(cComment, false, true);
    m_stringFormat = fmt(cString);
    m_numberFormat = fmt(cNumber);

    using TT = daad::TokenType;
    // أنواع البيانات
    auto setType = [&](TT t, QColor c){ m_formats[int(t)] = fmt(c, true); };
    setType(TT::TOKEN_KW_INT, cType); setType(TT::TOKEN_KW_DOUBLE, cType);
    setType(TT::TOKEN_KW_BOOL, cType); setType(TT::TOKEN_KW_CHAR, cType);
    setType(TT::TOKEN_KW_STRING, cType); setType(TT::TOKEN_KW_CONST, cType);
    setType(TT::TOKEN_KW_VOID, cType); setType(TT::TOKEN_KW_AUTO, cType);
    // حرفيات
    setType(TT::TOKEN_KW_TRUE, cLiteral); setType(TT::TOKEN_KW_FALSE, cLiteral);
    setType(TT::TOKEN_KW_NULLPTR, cLiteral); setType(TT::TOKEN_KW_TYPEDEF, cType);
    // تحكم
    setType(TT::TOKEN_KW_IF, cControl); setType(TT::TOKEN_KW_ELSE, cControl);
    setType(TT::TOKEN_KW_WHILE, cControl); setType(TT::TOKEN_KW_DO, cControl);
    setType(TT::TOKEN_KW_FOR, cControl); setType(TT::TOKEN_KW_IN, cControl);
    setType(TT::TOKEN_KW_SWITCH, cControl); setType(TT::TOKEN_KW_CASE, cControl);
    setType(TT::TOKEN_KW_DEFAULT, cControl); setType(TT::TOKEN_KW_BREAK, cControl);
    setType(TT::TOKEN_KW_CONTINUE, cControl); setType(TT::TOKEN_KW_GOTO, cControl);
    setType(TT::TOKEN_KW_RETURN, cControl);
    // OOP
    setType(TT::TOKEN_KW_CLASS, cOOP); setType(TT::TOKEN_KW_STRUCT, cOOP);
    setType(TT::TOKEN_KW_ENUM, cOOP); setType(TT::TOKEN_KW_INTERFACE, cOOP);
    setType(TT::TOKEN_KW_NAMESPACE, cOOP);
    setType(TT::TOKEN_KW_PUBLIC, cOOP); setType(TT::TOKEN_KW_PRIVATE, cOOP);
    setType(TT::TOKEN_KW_PROTECTED, cOOP); setType(TT::TOKEN_KW_INHERIT, cOOP);
    setType(TT::TOKEN_KW_SELF, cOOP); setType(TT::TOKEN_KW_BASE, cOOP);
    setType(TT::TOKEN_KW_ABSTRACT, cOOP);
    // دوال
    setType(TT::TOKEN_KW_FUNCTION, cFunc); setType(TT::TOKEN_KW_NEW, cFunc);
    setType(TT::TOKEN_KW_DELETE, cFunc); setType(TT::TOKEN_KW_POINTER, cFunc);
    setType(TT::TOKEN_KW_REFERENCE, cFunc); setType(TT::TOKEN_KW_STATIC, cFunc);
    setType(TT::TOKEN_KW_INLINE, cFunc); setType(TT::TOKEN_KW_EXTERN, cFunc);
    setType(TT::TOKEN_KW_TEMPLATE, cFunc);
    // استثناء
    setType(TT::TOKEN_KW_TRY, cControl); setType(TT::TOKEN_KW_CATCH, cControl);
    setType(TT::TOKEN_KW_FINALLY, cControl); setType(TT::TOKEN_KW_THROW, cControl);
    setType(TT::TOKEN_KW_ASSERT, cControl); setType(TT::TOKEN_KW_EXCEPTION, cControl);
    setType(TT::TOKEN_KW_TYPEOF, cControl); setType(TT::TOKEN_KW_SIZEOF, cControl);
    setType(TT::TOKEN_KW_INCREMENT, cControl); setType(TT::TOKEN_KW_DECREMENT, cControl);
    // أنظمة
    setType(TT::TOKEN_KW_SYNC, cOOP); setType(TT::TOKEN_KW_AWAIT, cOOP);
    setType(TT::TOKEN_KW_THREAD, cOOP); setType(TT::TOKEN_KW_LOCK, cOOP);
    setType(TT::TOKEN_KW_SHARED, cOOP); setType(TT::TOKEN_KW_UNIQUE, cOOP);
    setType(TT::TOKEN_KW_IMPORT, cControl); setType(TT::TOKEN_KW_EXPORT, cControl);
    setType(TT::TOKEN_KW_MODULE, cOOP); setType(TT::TOKEN_KW_ALTERNATIVE, cControl);
    // GUI
    setType(TT::TOKEN_KW_BUTTON, cOOP); setType(TT::TOKEN_KW_TEXTFIELD, cOOP);
    setType(TT::TOKEN_KW_COMBOBOX, cOOP); setType(TT::TOKEN_KW_IMAGE, cOOP);
    setType(TT::TOKEN_KW_CHECKBOX, cOOP); setType(TT::TOKEN_KW_SLIDER, cOOP);
    setType(TT::TOKEN_KW_DROPDOWN, cOOP); setType(TT::TOKEN_KW_PANEL, cOOP);
    setType(TT::TOKEN_KW_LABEL, cOOP); setType(TT::TOKEN_KW_COLUMN, cOOP);
    setType(TT::TOKEN_KW_ROW, cOOP); setType(TT::TOKEN_KW_GRID, cOOP);
    setType(TT::TOKEN_KW_PROGRESSBAR, cOOP); setType(TT::TOKEN_KW_TABBAR, cOOP);
    // طباعة
    setType(TT::TOKEN_KW_PRINT, cFunc); setType(TT::TOKEN_KW_INPUT, cFunc);
    setType(TT::TOKEN_KW_AND_ARABIC, cControl); setType(TT::TOKEN_KW_OR_ARABIC, cControl);
    // صور 20
    setType(TT::TOKEN_KW_LOAD_IMAGE, cFunc); setType(TT::TOKEN_KW_DRAW_IMAGE, cFunc);
    setType(TT::TOKEN_KW_IMAGE_SIZE, cFunc); setType(TT::TOKEN_KW_SAVE_IMAGE, cFunc);
    setType(TT::TOKEN_KW_CROP_IMAGE, cFunc); setType(TT::TOKEN_KW_RESIZE, cFunc);
    setType(TT::TOKEN_KW_ROTATE_IMAGE, cFunc); setType(TT::TOKEN_KW_FLIP_IMAGE, cFunc);
    setType(TT::TOKEN_KW_OPACITY, cFunc); setType(TT::TOKEN_KW_FILTER, cFunc);
    setType(TT::TOKEN_KW_OVERLAY, cFunc); setType(TT::TOKEN_KW_BACKGROUND, cFunc);
    setType(TT::TOKEN_KW_PIXEL, cFunc); setType(TT::TOKEN_KW_DRAW, cFunc);
    setType(TT::TOKEN_KW_FILL, cFunc); setType(TT::TOKEN_KW_RECTANGLE, cFunc);
    setType(TT::TOKEN_KW_CIRCLE, cFunc); setType(TT::TOKEN_KW_LINE, cFunc);
    setType(TT::TOKEN_KW_TEXT_ON_CANVAS, cFunc); setType(TT::TOKEN_KW_CLEAR, cFunc);
    setType(TT::TOKEN_KW_X, cIdent); setType(TT::TOKEN_KW_Y, cIdent);

    // محددات وعمليات تبقى افتراضية
    Q_UNUSED(cIdent); Q_UNUSED(cOp);
}

void DaadHighlighter::highlightBlock(const QString& text) {
    // 1) تعليقات // و /* */
    // معالجة // سطر واحد
    int commentStart = text.indexOf("//");
    if (commentStart >= 0) {
        setFormat(commentStart, text.length() - commentStart, m_commentFormat);
        // لا نمرر الباقي للـ Lexer بعد التعليق
        // لكن نكمل تلوين ما قبل التعليق فقط عبر Lexer
        QString codePart = text.left(commentStart);
        if (codePart.trimmed().isEmpty()) return;
        // استخدم codePart للتحليل
        std::string utf8 = codePart.toStdString();
        daad::Lexer lexer(utf8);
        daad::Token tok;
        while ((tok = lexer.getNextToken()).type != daad::TokenType::TOKEN_EOF) {
            if (tok.type == daad::TokenType::TOKEN_UNKNOWN) continue;
            QString qtext = QString::fromStdString(tok.text);
            int start = int(tok.column) - 1;
            int len = qtext.length();
            if (start < 0 || start >= codePart.length()) continue;
            // لا تلون ما بعد التعليق
            if (start + len > commentStart) len = commentStart - start;
            if (len <= 0) continue;
            if (tok.type == daad::TokenType::TOKEN_NUMBER) setFormat(start, len, m_numberFormat);
            else if (tok.type == daad::TokenType::TOKEN_STRING) setFormat(start, len, m_stringFormat);
            else if (m_formats.contains(int(tok.type))) setFormat(start, len, m_formats[int(tok.type)]);
        }
        return;
    }

    // 2) استخدام Lexer الأصلي على السطر
    std::string utf8 = text.toStdString();
    if (utf8.empty()) return;
    daad::Lexer lexer(utf8);
    daad::Token tok;
    while ((tok = lexer.getNextToken()).type != daad::TokenType::TOKEN_EOF) {
        if (tok.type == daad::TokenType::TOKEN_UNKNOWN) continue;
        QString qtext = QString::fromStdString(tok.text);
        int start = int(tok.column) - 1;
        int len = qtext.length();
        // معالجة الفواصل العربية: Lexer يعيد ";" لكن النص الأصلي قد يحتوي "؛"
        // نحتاج لضبط الطول للعربية — qtext يعطي الطول الصحيح
        if (start < 0 || start >= text.length()) continue;
        if (tok.type == daad::TokenType::TOKEN_NUMBER) {
            setFormat(start, len, m_numberFormat);
        } else if (tok.type == daad::TokenType::TOKEN_STRING) {
            setFormat(start, len, m_stringFormat);
        } else if (m_formats.contains(int(tok.type))) {
            setFormat(start, len, m_formats[int(tok.type)]);
        } else if (tok.type == daad::TokenType::TOKEN_IDENTIFIER) {
            // معرف عادي — يبقى افتراضي
        } else if (tok.type == daad::TokenType::TOKEN_SEMICOLON || tok.type == daad::TokenType::TOKEN_COMMA) {
            // فواصل — رمادي فاتح
            QTextCharFormat f; f.setForeground(QColor("#93a1a1"));
            setFormat(start, len, f);
        }
    }

    // 3) تعليقات /* */ متعددة الأسطر — حالة مبسطة
    if (text.contains("/*")) {
        int start = text.indexOf("/*");
        setFormat(start, text.length() - start, m_commentFormat);
    }
    if (previousBlockState() == 1) {
        setFormat(0, text.length(), m_commentFormat);
        if (text.contains("*/")) setCurrentBlockState(0); else setCurrentBlockState(1);
    }
    if (text.contains("/*") && !text.contains("*/")) setCurrentBlockState(1);
}
