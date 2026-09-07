/**
 * @file SyntaxHighlighter.cpp
 * @brief تنفيذ مبرّز التلوين اللوني للغة 'ض' - 110+ كلمة مفتاحية
 *
 * Dark IDE Theme (Matte Eye-Friendly):
 *   - Keywords:    Gold/Orange  #FF9F43
 *   - Types:       Cyan         #48DBFB
 *   - OOP:         Lavender     #C792EA
 *   - Functions:   Sky Blue     #64B5F6
 *   - Error:       Coral        #E57373
 *   - Advanced:    Mint Green   #81C784
 *   - UI:          Hot Red      #EF5350
 *   - Arrays:      Teal         #26C6DA
 *   - Values:      Pink         #F48FB1
 *   - Operators:   Orange       #FFB74D
 *   - Brackets:    Gray         #B0BEC5
 *   - Strings:     Peach        #E8A87C
 *   - Comments:    Olive Gray   #78909C (italic)
 *   - Numbers:     Lavender     #B39DDB
 *
 * Arabic regex: boundary via (?<![\x{0600}-\x{06FF}\w]) ... (?![\x{0600}-\x{06FF}\w])
 * Priority ordering prevents cross-color contamination.
 */

#include "SyntaxHighlighter.hpp"
#include <QQuickTextDocument>

DaadHighlighter::DaadHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    // ── 1. أنواع البيانات والتعريفات الأساسية: ذهبي دافئ #FF9F43 ──
    m_keywordDataFormat.setForeground(QColor("#FF9F43"));
    m_keywordDataFormat.setFontWeight(QFont::DemiBold);

    // ── 2. جمل التحكم والتدفق: فيروزي م refreshed #14B8A6 ──
    m_keywordControlFormat.setForeground(QColor("#14B8A6"));
    m_keywordControlFormat.setFontWeight(QFont::DemiBold);

    // ── 3. الكائنات OOP والنطاقات: لافندر ناعم #C792EA ──
    m_keywordOopFormat.setForeground(QColor("#C792EA"));
    m_keywordOopFormat.setFontWeight(QFont::DemiBold);

    // ── 4. الدوال وإدارة الذاكرة: أزرق سماوي #64B5F6 ──
    m_keywordFunctionFormat.setForeground(QColor("#64B5F6"));
    m_keywordFunctionFormat.setFontWeight(QFont::DemiBold);

    // ── 5. معالجة الأخطاء: مرجاني #E57373 ──
    m_keywordErrorFormat.setForeground(QColor("#E57373"));
    m_keywordErrorFormat.setFontWeight(QFont::DemiBold);

    // ── 6. الأنظمة المتقدمة: أخضر نعناعي #81C784 ──
    m_keywordAdvancedFormat.setForeground(QColor("#81C784"));
    m_keywordAdvancedFormat.setFontWeight(QFont::DemiBold);

    // ── 7. برمجة الواجهات الرسومية GUI: أحمر حيوي #EF5350 ──
    m_keywordUiFormat.setForeground(QColor("#EF5350"));
    m_keywordUiFormat.setFontWeight(QFont::DemiBold);

    // ── 8. المصفوفات والهياكل: سماوي فاتح #26C6DA ──
    m_keywordArrayFormat.setForeground(QColor("#26C6DA"));
    m_keywordArrayFormat.setFontWeight(QFont::DemiBold);

    // ── 9. القيم المنطقية والثوابت: وردي ناعم #F48FB1 ──
    m_keywordValueFormat.setForeground(QColor("#F48FB1"));
    m_keywordValueFormat.setFontWeight(QFont::DemiBold);

    // ── 10. المعاملات: برتقالي حيوي #FFB74D ──
    m_operatorFormat.setForeground(QColor("#FFB74D"));
    m_operatorFormat.setFontWeight(QFont::DemiBold);

    // ── 11. الأقواس والأعمدة: رمادي فاتح #B0BEC5 ──
    m_bracketFormat.setForeground(QColor("#B0BEC5"));

    // ── النصوص المقتبسة: برتقالي بني دافئ #E8A87C ──
    m_stringFormat.setForeground(QColor("#E8A87C"));

    // ── التعليقات: رمادي زيتي مائل مع Italic #78909C ──
    m_commentFormat.setForeground(QColor("#78909C"));
    m_commentFormat.setFontItalic(true);

    // ── الأرقام: بنفسجي فاتح #B39DDB ──
    m_numberFormat.setForeground(QColor("#B39DDB"));

    buildRules();
}

void DaadHighlighter::setDocumentFromQml(QObject* quickDoc) {
    if (!quickDoc) return;
    auto* qstd = qobject_cast<QQuickTextDocument*>(quickDoc);
    if (qstd && qstd->textDocument() != document()) {
        QSyntaxHighlighter::setDocument(qstd->textDocument());
        // Force LTR base direction for code editor to prevent bidi reordering
        QTextOption opt = qstd->textDocument()->defaultTextOption();
        opt.setTextDirection(Qt::LeftToRight);
        qstd->textDocument()->setDefaultTextOption(opt);
        emit documentChanged();
    }
}

QTextDocument* DaadHighlighter::document() const {
    return QSyntaxHighlighter::document();
}

void DaadHighlighter::buildRules() {
    // ═══════════════════════════════════════════════════════════
    // القاعدة الأعلى أولوية: التعليقات أولاً لمنع التلوين المتقاطع
    // ═══════════════════════════════════════════════════════════

    // التعليقات: تبدأ بـ # أو //
    m_rules.append({ QRegularExpression(QStringLiteral("^(\\s*)(#[^\n]*)")), m_commentFormat, 100 });
    m_rules.append({ QRegularExpression(QStringLiteral("(//[^\n]*)")), m_commentFormat, 100 });

    // النصوص المقتبسة
    m_rules.append({ QRegularExpression(QStringLiteral("\"[^\"]*\"")), m_stringFormat, 90 });

    // الأرقام (بما في ذلك الأعداد العشرية)
    m_rules.append({ QRegularExpression(QStringLiteral("\\b\\d+(\\.\\d+)?\\b")), m_numberFormat, 50 });

    // ═══════════════════════════════════════════════════════════
    // المعاملات والرموز الخاصة
    // ═══════════════════════════════════════════════════════════
    const QStringList operators = {
        QStringLiteral("\\+="), QStringLiteral("-="), QStringLiteral("\\*="),
        QStringLiteral("/="),  QStringLiteral("%"),   QStringLiteral("->"),
        QStringLiteral("=="),  QStringLiteral("!="),  QStringLiteral("<="),
        QStringLiteral(">="),  QStringLiteral("\\+"), QStringLiteral("\\*")
    };
    for (const QString& op : operators) {
        m_rules.append({ QRegularExpression(op), m_operatorFormat, 70 });
    }

    // الأقواس والأعمدة
    m_rules.append({ QRegularExpression(QStringLiteral("[\\(\\)\\{\\}\\[\\]]")), m_bracketFormat, 60 });

    // ═══════════════════════════════════════════════════════════
    // 1. أنواع البيانات والتعريفات الأساسية (12)
    // ═══════════════════════════════════════════════════════════
    const QStringList dataTypes = {
        QStringLiteral("صحيح"), QStringLiteral("عشري"), QStringLiteral("منطقي"),
        QStringLiteral("حرف"),  QStringLiteral("نص"),   QStringLiteral("ثابت"),
        QStringLiteral("فراغ"), QStringLiteral("تلقائي"), QStringLiteral("صواب"),
        QStringLiteral("خطأ"),  QStringLiteral("عدم"),  QStringLiteral("عرّف")
    };
    for (const QString& w : dataTypes) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordDataFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 2. جمل التحكم والتدفق والشرط (12)
    // ═══════════════════════════════════════════════════════════
    const QStringList controlFlow = {
        QStringLiteral("إذا"),    QStringLiteral("وإلا"),   QStringLiteral("طالما"),
        QStringLiteral("افعل"),   QStringLiteral("لكل"),    QStringLiteral("في"),
        QStringLiteral("اختر"),   QStringLiteral("حالة"),   QStringLiteral("افتراضي"),
        QStringLiteral("توقف"),   QStringLiteral("استمر"),  QStringLiteral("انتقل")
    };
    for (const QString& w : controlFlow) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordControlFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 3. الكائنات (OOP) والنطاقات وصلاحيات الوصول (12)
    // ═══════════════════════════════════════════════════════════
    const QStringList oopKeywords = {
        QStringLiteral("صنف"),    QStringLiteral("هيكل"),   QStringLiteral("تعداد"),
        QStringLiteral("واجهة"),  QStringLiteral("نطاق"),   QStringLiteral("عام"),
        QStringLiteral("خاص"),    QStringLiteral("محمي"),   QStringLiteral("يرث"),
        QStringLiteral("ذاتي"),   QStringLiteral("الأصل"),  QStringLiteral("مجرّد")
    };
    for (const QString& w : oopKeywords) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordOopFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 4. الدوال، المترجم، وإدارة الذاكرة (10)
    // ═══════════════════════════════════════════════════════════
    const QStringList functionKeywords = {
        QStringLiteral("دالة"),   QStringLiteral("ارجع"),   QStringLiteral("جديد"),
        QStringLiteral("احذف"),   QStringLiteral("مؤشر"),  QStringLiteral("مرجع"),
        QStringLiteral("ساكن"),   QStringLiteral("مضمن"),   QStringLiteral("خارجي"),
        QStringLiteral("قالب")
    };
    for (const QString& w : functionKeywords) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordFunctionFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 5. معالجة الأخطاء والتحقق من الأنظمة (8)
    // ═══════════════════════════════════════════════════════════
    const QStringList errorKeywords = {
        QStringLiteral("حاول"),    QStringLiteral("امسك"),    QStringLiteral("أخيراً"),
        QStringLiteral("ارمِ"),    QStringLiteral("تأكد"),   QStringLiteral("استثناء"),
        QStringLiteral("نوع_الـ"), QStringLiteral("حجم_الـ")
    };
    for (const QString& w : errorKeywords) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordErrorFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 6. الأنظمة المتقدمة، التزامن، والوحدات (10)
    // ═══════════════════════════════════════════════════════════
    const QStringList advancedKeywords = {
        QStringLiteral("تزامن"),   QStringLiteral("انتظر"),  QStringLiteral("خيط"),
        QStringLiteral("احجز"),    QStringLiteral("مشترك"), QStringLiteral("فريد"),
        QStringLiteral("استورد"),  QStringLiteral("صدّر"),  QStringLiteral("وحدة"),
        QStringLiteral("بديل")
    };
    for (const QString& w : advancedKeywords) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordAdvancedFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 7. برمجة الواجهات الرسومية GUI (14) - أحمر مميز
    // ═══════════════════════════════════════════════════════════
    const QStringList uiKeywords = {
        QStringLiteral("زر_أمر"),        QStringLiteral("حقل_نص"),
        QStringLiteral("قائمة_خيارات"), QStringLiteral("صورة"),
        QStringLiteral("مربع_اختيار"),  QStringLiteral("شريط_تمرير"),
        QStringLiteral("قائمة_منسدلة"), QStringLiteral("لوحة"),
        QStringLiteral("تسمية"),         QStringLiteral("عمود"),
        QStringLiteral("صف"),            QStringLiteral("شبكة"),
        QStringLiteral("شريط_تلوين"),   QStringLiteral("علامة_تبويب")
    };
    for (const QString& w : uiKeywords) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordUiFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 8. القيم المنطقية والثوابت (4)
    // ═══════════════════════════════════════════════════════════
    const QStringList logicValues = {
        QStringLiteral("صواب"), QStringLiteral("خطأ"), QStringLiteral("عدم"), QStringLiteral("ثابت")
    };
    for (const QString& w : logicValues) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordValueFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 9. المصفوفات والهياكل المتقدمة (8)
    // ═══════════════════════════════════════════════════════════
    const QStringList arrayKeywords = {
        QStringLiteral("مصفوفة"), QStringLiteral("قائمة"), QStringLiteral("قاموس"),
        QStringLiteral("مكدس"),   QStringLiteral("طابور"), QStringLiteral("شجرة"),
        QStringLiteral("رسم_بياني"), QStringLiteral("جدول")
    };
    for (const QString& w : arrayKeywords) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordArrayFormat, 80 });
    }

    // ═══════════════════════════════════════════════════════════
    // 10. دوال مدمجة (طباعة + إدخال/إخراج + ملفات + نظام)
    // ═══════════════════════════════════════════════════════════
    const QStringList builtins = {
        // إدخال/إخراج
        QStringLiteral("طباعة"), QStringLiteral("اقرأ_stdin"), QStringLiteral("اكتب_stdout"),
        QStringLiteral("اكتب_stderr"), QStringLiteral("افتح"), QStringLiteral("غلق"),
        QStringLiteral("اقرأ_سطر"), QStringLiteral("اكتب_سطر"), QStringLiteral("امسح_الشاشة"),
        // ملفات
        QStringLiteral("حذف"), QStringLiteral("نسخ"), QStringLiteral("إعادة_تسمية"),
        QStringLiteral("حجم_الملف"), QStringLiteral("ملف_موجود"),
        // رياضيات
        QStringLiteral("جذر"), QStringLiteral("قوة"), QStringLiteral("نسبة"),
        // تحويل
        QStringLiteral("تحويل_صحيح"), QStringLiteral("uyển_عشري"), QStringLiteral("تحويل_نص"),
        // نصوص
        QStringLiteral("طول"), QStringLiteral("تقليم"), QStringLiteral("بحث"),
        QStringLiteral("استبدال"), QStringLiteral("يحتوي"), QStringLiteral("يبدأ"),
        // مقارنة
        QStringLiteral("أكبر"), QStringLiteral("أصغر"), QStringLiteral("يساوي")
    };
    for (const QString& w : builtins) {
        m_rules.append({ QRegularExpression(arabicBoundary(w)), m_keywordFunctionFormat, 80 });
    }
}

void DaadHighlighter::highlightBlock(const QString& text) {
    for (const auto& rule : m_rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
