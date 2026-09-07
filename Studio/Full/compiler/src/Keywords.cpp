#include "Daad/Keywords.hpp"
#include <unordered_map>

namespace daad {

void KeywordRegistry::initializeStandardKeywords() {
    // 1. أنواع البيانات والتعريفات الأساسية (12)
    m_keywordToType["صحيح"]     = KeywordType::KwInt;
    m_keywordToType["عشري"]     = KeywordType::KwDouble;
    m_keywordToType["منطقي"]    = KeywordType::KwBool;
    m_keywordToType["حرف"]      = KeywordType::KwChar;
    m_keywordToType["نص"]       = KeywordType::KwString;
    m_keywordToType["ثابت"]     = KeywordType::KwConst;
    m_keywordToType["فراغ"]     = KeywordType::KwVoid;
    m_keywordToType["تلقائي"]   = KeywordType::KwAuto;
    m_keywordToType["صواب"]     = KeywordType::KwTrue;
    m_keywordToType["خطأ"]      = KeywordType::KwFalse;
    m_keywordToType["عدم"]      = KeywordType::KwNullptr;
    m_keywordToType["عرّف"]     = KeywordType::KwTypedef;

    // 2. جمل التحكم والتدفق والشرط (14)
    m_keywordToType["إذا"]      = KeywordType::KwIf;
    m_keywordToType["اذا"]      = KeywordType::KwIf;      // بديل بدون همزة
    m_keywordToType["وإلا"]     = KeywordType::KwElse;
    m_keywordToType["طالما"]    = KeywordType::KwWhile;
    m_keywordToType["بينما"]    = KeywordType::KwWhile;
    m_keywordToType["افعل"]     = KeywordType::KwDo;
    m_keywordToType["لكل"]      = KeywordType::KwFor;
    m_keywordToType["كرر"]      = KeywordType::KwFor;
    m_keywordToType["في"]       = KeywordType::KwIn;
    m_keywordToType["اختر"]     = KeywordType::KwSwitch;
    m_keywordToType["حالة"]     = KeywordType::KwCase;
    m_keywordToType["افتراضي"]  = KeywordType::KwDefault;
    m_keywordToType["توقف"]     = KeywordType::KwBreak;
    m_keywordToType["اكسر"]     = KeywordType::KwBreak;   // بديل
    m_keywordToType["استمر"]    = KeywordType::KwContinue;
    m_keywordToType["تابع"]     = KeywordType::KwContinue; // بديل
    m_keywordToType["انتقل"]    = KeywordType::KwGoto;
    m_keywordToType["س"]        = KeywordType::KwX;       // محور الإحداثيات
    m_keywordToType["ص"]        = KeywordType::KwY;       // محور الإحداثيات

    // 3. الكائنات (OOP) والنطاقات وصلاحيات الوصول (12)
    m_keywordToType["صنف"]      = KeywordType::KwClass;
    m_keywordToType["فئة"]      = KeywordType::KwClass;
    m_keywordToType["هيكل"]     = KeywordType::KwStruct;
    m_keywordToType["تعداد"]    = KeywordType::KwEnum;
    m_keywordToType["واجهة"]    = KeywordType::KwInterface;
    m_keywordToType["نطاق"]     = KeywordType::KwNamespace;
    m_keywordToType["عام"]      = KeywordType::KwPublic;
    m_keywordToType["خاص"]      = KeywordType::KwPrivate;
    m_keywordToType["محمي"]     = KeywordType::KwProtected;
    m_keywordToType["يرث"]      = KeywordType::KwInherit;
    m_keywordToType["ذاتي"]     = KeywordType::KwSelf;
    m_keywordToType["هذا"]      = KeywordType::KwSelf;    // بديل — يتوافق مع Web
    m_keywordToType["الأصل"]    = KeywordType::KwBase;
    m_keywordToType["مجرّد"]    = KeywordType::KwAbstract;

    // 4. الدوال، المترجم، وإدارة الذاكرة (10)
    m_keywordToType["دالة"]     = KeywordType::KwFunction;
    m_keywordToType["ارجع"]     = KeywordType::KwReturn;
    m_keywordToType["جديد"]     = KeywordType::KwNew;
    m_keywordToType["احذف"]     = KeywordType::KwDelete;
    m_keywordToType["مؤشر"]     = KeywordType::KwPointer;
    m_keywordToType["مرجع"]     = KeywordType::KwReference;
    m_keywordToType["ساكن"]     = KeywordType::KwStatic;
    m_keywordToType["مضمن"]     = KeywordType::KwInline;
    m_keywordToType["خارجي"]    = KeywordType::KwExtern;
    m_keywordToType["قالب"]     = KeywordType::KwTemplate;

    // 5. معالجة الأخطاء والتحقق من الأنظمة (8)
    m_keywordToType["حاول"]     = KeywordType::KwTry;
    m_keywordToType["امسك"]     = KeywordType::KwCatch;
    m_keywordToType["أخيراً"]   = KeywordType::KwFinally;
    m_keywordToType["ارمِ"]     = KeywordType::KwThrow;
    m_keywordToType["تأكد"]     = KeywordType::KwAssert;
    m_keywordToType["استثناء"]  = KeywordType::KwException;
    m_keywordToType["نوع_الـ"]  = KeywordType::KwTypeOf;
    m_keywordToType["حجم_الـ"]  = KeywordType::KwSizeOf;
    m_keywordToType["زد"]       = KeywordType::KwIncrement;
    m_keywordToType["انقص"]     = KeywordType::KwDecrement;

    // 6. الأنظمة المتقدمة، التزامن، والوحدات (10)
    m_keywordToType["تزامن"]    = KeywordType::KwSync;
    m_keywordToType["انتظر"]    = KeywordType::KwAwait;
    m_keywordToType["بانتظار"]  = KeywordType::KwAwait;   // بديل
    m_keywordToType["خيط"]      = KeywordType::KwThread;
    m_keywordToType["احجز"]     = KeywordType::KwLock;
    m_keywordToType["مشترك"]    = KeywordType::KwShared;
    m_keywordToType["فريد"]     = KeywordType::KwUnique;
    m_keywordToType["استورد"]   = KeywordType::KwImport;
    m_keywordToType["صدّر"]     = KeywordType::KwExport;
    m_keywordToType["وحدة"]     = KeywordType::KwModule;
    m_keywordToType["بديل"]     = KeywordType::KwAlternative;

    // 8. بديلات إضافية — يتوافق مع Web
    m_keywordToType["اطبع"]    = KeywordType::KwPrint;     // بديل لـ طباعة

    // 8b. بديلات توافق مع Web
    m_keywordToType["بينما"]    = KeywordType::KwWhile;      // بديل لـ طالما
    m_keywordToType["كرر"]      = KeywordType::KwFor;        // بديل لـ لكل
    m_keywordToType["فئة"]      = KeywordType::KwClass;      // بديل لـ صنف

    // 8c. منطق عربي — يتوافق مع Web
    m_keywordToType["و"]        = KeywordType::KwAndArabic;
    m_keywordToType["أو"]       = KeywordType::KwOrArabic;

    // 7. برمجة الواجهات الرسومية GUI (14)
    m_keywordToType["زر_أمر"]         = KeywordType::KwButton;
    m_keywordToType["حقل_نص"]         = KeywordType::KwTextField;
    m_keywordToType["قائمة_خيارات"]   = KeywordType::KwComboBox;
    m_keywordToType["صورة"]           = KeywordType::KwImage;
    m_keywordToType["مربع_اختيار"]    = KeywordType::KwCheckBox;
    m_keywordToType["شريط_تمرير"]     = KeywordType::KwSlider;
    m_keywordToType["قائمة_منسدلة"]   = KeywordType::KwDropDown;
    m_keywordToType["لوحة"]           = KeywordType::KwPanel;
    m_keywordToType["تسمية"]          = KeywordType::KwLabel;
    m_keywordToType["عمود"]           = KeywordType::KwColumn;
    m_keywordToType["صف"]             = KeywordType::KwRow;
    m_keywordToType["شبكة"]           = KeywordType::KwGrid;
    m_keywordToType["شريط_تلوين"]     = KeywordType::KwProgressBar;
    m_keywordToType["علامة_تبويب"]    = KeywordType::KwTabBar;

    // 8. طباعة (1)
    m_keywordToType["طباعة"]    = KeywordType::KwPrint;     // الكلمة المفتاحية الأساسية

    // 8.5 الإدخال من المستخدم (1)
    m_keywordToType["ادخل"]     = KeywordType::KwInput;     // قراءة قيمة من لوحة المفاتيح

    // 9. معالجة الصور (20) — يتوافق مع Web
    m_keywordToType["حمّل_صورة"]    = KeywordType::KwLoadImage;
    m_keywordToType["ارسم_صورة"]    = KeywordType::KwDrawImage;
    m_keywordToType["حجم_صورة"]     = KeywordType::KwImageSize;
    m_keywordToType["احفظ_صورة"]    = KeywordType::KwSaveImage;
    m_keywordToType["قص_صورة"]      = KeywordType::KwCropImage;
    m_keywordToType["غيّر_حجم"]     = KeywordType::KwResize;
    m_keywordToType["لف_صورة"]      = KeywordType::KwRotateImage;
    m_keywordToType["قلب_صورة"]     = KeywordType::KwFlipImage;
    m_keywordToType["شفافية"]       = KeywordType::KwOpacity;
    m_keywordToType["فلتر"]         = KeywordType::KwFilter;
    m_keywordToType["تراكب"]        = KeywordType::KwOverlay;
    m_keywordToType["خلفية"]        = KeywordType::KwBackground;
    m_keywordToType["بكسل"]         = KeywordType::KwPixel;
    m_keywordToType["ارسم"]         = KeywordType::KwDraw;
    m_keywordToType["ملء"]          = KeywordType::KwFill;
    m_keywordToType["مستطيل"]       = KeywordType::KwRectangle;
    m_keywordToType["دائرة"]        = KeywordType::KwCircle;
    m_keywordToType["خط"]           = KeywordType::KwLine;
    m_keywordToType["نص_على_لوحة"]  = KeywordType::KwTextOnCanvas;
    m_keywordToType["مسح"]          = KeywordType::KwClear;

    // بناء خرائط الاتجاه المعاكس
    for (const auto& [text, type] : m_keywordToType) {
        KeywordInfo info{};
        info.arabicText = text;

        switch (type) {
            // 1. أنواع البيانات
            case KeywordType::KwInt:       info.cppEquivalent = "int"; break;
            case KeywordType::KwDouble:    info.cppEquivalent = "double"; break;
            case KeywordType::KwBool:      info.cppEquivalent = "bool"; break;
            case KeywordType::KwChar:      info.cppEquivalent = "char"; break;
            case KeywordType::KwString:    info.cppEquivalent = "std::string"; break;
            case KeywordType::KwConst:     info.cppEquivalent = "const"; break;
            case KeywordType::KwVoid:      info.cppEquivalent = "void"; break;
            case KeywordType::KwAuto:      info.cppEquivalent = "auto"; break;
            case KeywordType::KwTrue:      info.cppEquivalent = "true"; break;
            case KeywordType::KwFalse:     info.cppEquivalent = "false"; break;
            case KeywordType::KwNullptr:   info.cppEquivalent = "nullptr"; break;
            case KeywordType::KwTypedef:   info.cppEquivalent = "typedef"; break;

            // 2. التحكم في التدفق
            case KeywordType::KwIf:        info.cppEquivalent = "if"; break;
            case KeywordType::KwElse:      info.cppEquivalent = "else"; break;
            case KeywordType::KwWhile:     info.cppEquivalent = "while"; break;
            case KeywordType::KwDo:        info.cppEquivalent = "do"; break;
            case KeywordType::KwFor:       info.cppEquivalent = "for"; break;
            case KeywordType::KwIn:        info.cppEquivalent = "for"; break;
            case KeywordType::KwSwitch:    info.cppEquivalent = "switch"; break;
            case KeywordType::KwCase:      info.cppEquivalent = "case"; break;
            case KeywordType::KwDefault:   info.cppEquivalent = "default"; break;
            case KeywordType::KwBreak:     info.cppEquivalent = "break"; break;
            case KeywordType::KwContinue:  info.cppEquivalent = "continue"; break;
            case KeywordType::KwGoto:      info.cppEquivalent = "goto"; break;
            case KeywordType::KwX:         info.cppEquivalent = "x"; break;
            case KeywordType::KwY:         info.cppEquivalent = "y"; break;

            // 3. OOP والنطاقات
            case KeywordType::KwClass:     info.cppEquivalent = "class"; break;
            case KeywordType::KwStruct:    info.cppEquivalent = "struct"; break;
            case KeywordType::KwEnum:      info.cppEquivalent = "enum"; break;
            case KeywordType::KwInterface: info.cppEquivalent = "class"; break;
            case KeywordType::KwNamespace: info.cppEquivalent = "namespace"; break;
            case KeywordType::KwPublic:    info.cppEquivalent = "public"; break;
            case KeywordType::KwPrivate:   info.cppEquivalent = "private"; break;
            case KeywordType::KwProtected: info.cppEquivalent = "protected"; break;
            case KeywordType::KwInherit:   info.cppEquivalent = "public"; break;
            case KeywordType::KwSelf:      info.cppEquivalent = "this"; break;
            case KeywordType::KwBase:      info.cppEquivalent = "this"; break;
            case KeywordType::KwAbstract:  info.cppEquivalent = "virtual"; break;

            // 4. الدوال وإدارة الذاكرة
            case KeywordType::KwFunction:  info.cppEquivalent = "void"; break;
            case KeywordType::KwReturn:    info.cppEquivalent = "return"; break;
            case KeywordType::KwNew:       info.cppEquivalent = "new"; break;
            case KeywordType::KwDelete:    info.cppEquivalent = "delete"; break;
            case KeywordType::KwPointer:   info.cppEquivalent = "*"; break;
            case KeywordType::KwReference: info.cppEquivalent = "&"; break;
            case KeywordType::KwStatic:    info.cppEquivalent = "static"; break;
            case KeywordType::KwInline:    info.cppEquivalent = "inline"; break;
            case KeywordType::KwExtern:    info.cppEquivalent = "extern"; break;
            case KeywordType::KwTemplate:  info.cppEquivalent = "template"; break;

            // 5. معالجة الأخطاء
            case KeywordType::KwTry:       info.cppEquivalent = "try"; break;
            case KeywordType::KwCatch:     info.cppEquivalent = "catch"; break;
            case KeywordType::KwFinally:   info.cppEquivalent = "finally"; break;
            case KeywordType::KwThrow:     info.cppEquivalent = "throw"; break;
            case KeywordType::KwAssert:    info.cppEquivalent = "assert"; break;
            case KeywordType::KwException:  info.cppEquivalent = "std::exception"; break;
            case KeywordType::KwTypeOf:    info.cppEquivalent = "decltype"; break;
            case KeywordType::KwSizeOf:    info.cppEquivalent = "sizeof"; break;
            case KeywordType::KwIncrement: info.cppEquivalent = "++"; break;
            case KeywordType::KwDecrement: info.cppEquivalent = "--"; break;

            // 6. الأنظمة المتقدمة
            case KeywordType::KwSync:        info.cppEquivalent = "std::mutex"; break;
            case KeywordType::KwAwait:       info.cppEquivalent = "co_await"; break;
            case KeywordType::KwThread:      info.cppEquivalent = "std::thread"; break;
            case KeywordType::KwLock:        info.cppEquivalent = "std::lock_guard"; break;
            case KeywordType::KwShared:      info.cppEquivalent = "std::shared_ptr"; break;
            case KeywordType::KwUnique:      info.cppEquivalent = "std::unique_ptr"; break;
            case KeywordType::KwImport:      info.cppEquivalent = "#include"; break;
            case KeywordType::KwExport:      info.cppEquivalent = "export"; break;
            case KeywordType::KwModule:      info.cppEquivalent = "module"; break;
            case KeywordType::KwAlternative: info.cppEquivalent = "else if"; break;

            // 7. برمجة الواجهات الرسومية
            case KeywordType::KwButton:      info.cppEquivalent = "QPushButton"; break;
            case KeywordType::KwTextField:   info.cppEquivalent = "QLineEdit"; break;
            case KeywordType::KwComboBox:    info.cppEquivalent = "QComboBox"; break;
            case KeywordType::KwImage:       info.cppEquivalent = "QLabel"; break;
            case KeywordType::KwCheckBox:    info.cppEquivalent = "QCheckBox"; break;
            case KeywordType::KwSlider:      info.cppEquivalent = "QSlider"; break;
            case KeywordType::KwDropDown:    info.cppEquivalent = "QComboBox"; break;
            case KeywordType::KwPanel:       info.cppEquivalent = "QWidget"; break;
            case KeywordType::KwLabel:       info.cppEquivalent = "QLabel"; break;
            case KeywordType::KwColumn:      info.cppEquivalent = "QVBoxLayout"; break;
            case KeywordType::KwRow:         info.cppEquivalent = "QHBoxLayout"; break;
            case KeywordType::KwGrid:        info.cppEquivalent = "QGridLayout"; break;
            case KeywordType::KwProgressBar: info.cppEquivalent = "QProgressBar"; break;
            case KeywordType::KwTabBar:      info.cppEquivalent = "QTabWidget"; break;

            // 8. طباعة
            case KeywordType::KwPrint:       info.cppEquivalent = "daad::runtime::daad_print"; break;

            // 8b. منطق عربي
            case KeywordType::KwAndArabic:   info.cppEquivalent = "&&"; break;
            case KeywordType::KwOrArabic:    info.cppEquivalent = "||"; break;

            // 9. معالجة الصور
            case KeywordType::KwLoadImage:   info.cppEquivalent = "daad::image::load"; break;
            case KeywordType::KwDrawImage:   info.cppEquivalent = "daad::image::draw"; break;
            case KeywordType::KwImageSize:   info.cppEquivalent = "daad::image::size"; break;
            case KeywordType::KwSaveImage:   info.cppEquivalent = "daad::image::save"; break;
            case KeywordType::KwCropImage:   info.cppEquivalent = "daad::image::crop"; break;
            case KeywordType::KwResize:      info.cppEquivalent = "daad::image::resize"; break;
            case KeywordType::KwRotateImage: info.cppEquivalent = "daad::image::rotate"; break;
            case KeywordType::KwFlipImage:   info.cppEquivalent = "daad::image::flip"; break;
            case KeywordType::KwOpacity:     info.cppEquivalent = "daad::image::opacity"; break;
            case KeywordType::KwFilter:      info.cppEquivalent = "daad::image::filter"; break;
            case KeywordType::KwOverlay:     info.cppEquivalent = "daad::image::overlay"; break;
            case KeywordType::KwBackground:  info.cppEquivalent = "daad::image::background"; break;
            case KeywordType::KwPixel:       info.cppEquivalent = "daad::image::pixel"; break;
            case KeywordType::KwDraw:        info.cppEquivalent = "daad::image::drawShape"; break;
            case KeywordType::KwFill:        info.cppEquivalent = "daad::image::fill"; break;
            case KeywordType::KwRectangle:   info.cppEquivalent = "daad::image::rectangle"; break;
            case KeywordType::KwCircle:      info.cppEquivalent = "daad::image::circle"; break;
            case KeywordType::KwLine:        info.cppEquivalent = "daad::image::line"; break;
            case KeywordType::KwTextOnCanvas: info.cppEquivalent = "daad::image::text"; break;
            case KeywordType::KwClear:       info.cppEquivalent = "daad::image::clear"; break;
        }

        m_typeToInfo[type] = info;
    }
}

std::optional<KeywordType> KeywordRegistry::findKeyword(std::string_view text) const noexcept {
    auto it = m_keywordToType.find(text);
    if (it != m_keywordToType.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string_view KeywordRegistry::getKeywordText(KeywordType type) const noexcept {
    auto it = m_typeToInfo.find(type);
    if (it != m_typeToInfo.end()) {
        return it->second.arabicText;
    }
    return "";
}

std::string_view KeywordRegistry::getCppEquivalent(KeywordType type) const noexcept {
    auto it = m_typeToInfo.find(type);
    if (it != m_typeToInfo.end()) {
        return it->second.cppEquivalent;
    }
    return "";
}

bool KeywordRegistry::isDataType(KeywordType type) noexcept {
    switch (type) {
        case KeywordType::KwInt:
        case KeywordType::KwDouble:
        case KeywordType::KwBool:
        case KeywordType::KwChar:
        case KeywordType::KwString:
        case KeywordType::KwVoid:
        case KeywordType::KwAuto:
            return true;
        default:
            return false;
    }
}

bool KeywordRegistry::isControlFlow(KeywordType type) noexcept {
    switch (type) {
        case KeywordType::KwIf:
        case KeywordType::KwElse:
        case KeywordType::KwWhile:
        case KeywordType::KwDo:
        case KeywordType::KwFor:
        case KeywordType::KwIn:
        case KeywordType::KwSwitch:
        case KeywordType::KwCase:
        case KeywordType::KwDefault:
        case KeywordType::KwBreak:
        case KeywordType::KwContinue:
        case KeywordType::KwGoto:
        case KeywordType::KwReturn:
            return true;
        default:
            return false;
    }
}

bool KeywordRegistry::isAccessSpecifier(KeywordType type) noexcept {
    switch (type) {
        case KeywordType::KwPublic:
        case KeywordType::KwPrivate:
        case KeywordType::KwProtected:
            return true;
        default:
            return false;
    }
}

const KeywordRegistry& getStandardKeywordRegistry() noexcept {
    static KeywordRegistry registry;
    static bool initialized = false;
    if (!initialized) {
        const_cast<KeywordRegistry&>(registry).initializeStandardKeywords();
        initialized = true;
    }
    return registry;
}

} // namespace daad
