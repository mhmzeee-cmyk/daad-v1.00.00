/**
 * @file Keywords.hpp
 * @brief تعريف الكلمات المفتاحية للغة 'ض' (64 كلمة مفتاحية) وربطها بمكافئات C++
 * @author فريق ض استديو
 * @version 2.0.0
 */

#pragma once

#include <string>
#include <unordered_map>
#include <string_view>
#include <optional>

namespace daad {

/**
 * @enum KeywordType
 * @brief تعداد أنواع الكلمات المفتاحية في لغة 'ض' - 64 كلمة مفتاحية
 */
enum class KeywordType {
    // 1. أنواع البيانات والتعريفات الأساسية (12)
    KwInt,           // صحيح
    KwDouble,        // عشري
    KwBool,          // منطقي
    KwChar,          // حرف
    KwString,        // نص
    KwConst,         // ثابت
    KwVoid,          // فراغ
    KwAuto,          // تلقائي
    KwTrue,          // صواب
    KwFalse,         // خطأ
    KwNullptr,       // عدم
    KwTypedef,       // عرّف

    // 2. جمل التحكم والتدفق والشرط (12)
    KwIf,            // إذا
    KwElse,          // وإلا
    KwWhile,         // طالما
    KwDo,            // افعل
    KwFor,           // لكل
    KwIn,            // في
    KwSwitch,        // اختر
    KwCase,          // حالة
    KwDefault,       // افتراضي
    KwBreak,         // توقف
    KwContinue,      // استمر
    KwGoto,          // انتقل

    // 3. الكائنات (OOP) والنطاقات وصلاحيات الوصول (12)
    KwClass,         // صنف
    KwStruct,        // هيكل
    KwEnum,          // تعداد
    KwInterface,     // واجهة
    KwNamespace,     // نطاق
    KwPublic,        // عام
    KwPrivate,       // خاص
    KwProtected,     // محمي
    KwInherit,       // يرث
    KwSelf,          // ذاتي
    KwBase,          // الأصل
    KwAbstract,      // مجرّد

    // 4. الدوال، المترجم، وإدارة الذاكرة (10)
    KwFunction,      // دالة
    KwReturn,        // ارجع
    KwNew,           // جديد
    KwDelete,        // احذف
    KwPointer,       // مؤشر
    KwReference,     // مرجع
    KwStatic,        // ساكن
    KwInline,        // مضمن
    KwExtern,        // خارجي
    KwTemplate,      // قالب

    // 5. معالجة الأخطاء والتحقق من الأنظمة (8)
    KwTry,           // حاول
    KwCatch,         // امسك
    KwFinally,       // أخيراً
    KwThrow,         // ارمِ
    KwAssert,        // تأكد
    KwException,     // استثناء
    KwTypeOf,        // نوع_الـ
    KwSizeOf,        // حجم_الـ

    // 6. الأنظمة المتقدمة، التزامن، والوحدات (10)
    KwSync,          // تزامن
    KwAwait,         // انتظر
    KwThread,        // خيط
    KwLock,          // احجز
    KwShared,        // مشترك
    KwUnique,        // فريد
    KwImport,        // استورد
    KwExport,        // صدّر
    KwModule,        // وحدة
    KwAlternative,   // بديل

    // 7. برمجة الواجهات الرسومية GUI (14)
    KwButton,        // زر_أمر
    KwTextField,     // حقل_نص
    KwComboBox,      // قائمة_خيارات
    KwImage,         // صورة
    KwCheckBox,      // مربع_اختيار
    KwSlider,        // شريط_تمرير
    KwDropDown,      // قائمة_منسدلة
    KwPanel,         // لوحة
    KwLabel,         // تسمية
    KwColumn,        // عمود
    KwRow,           // صف
    KwGrid,          // شبكة
    KwProgressBar,   // شريط_تلوين
    KwTabBar         // علامة_تبويب
};

constexpr int KW_TOTAL_COUNT = 78;

/**
 * @class KeywordRegistry
 * @brief سجل الكلمات المفتاحية للغة 'ض' - يدعم البحث ثنائي الاتجاه
 */
class KeywordRegistry {
public:
    KeywordRegistry() = default;
    ~KeywordRegistry() = default;

    KeywordRegistry(const KeywordRegistry&) = delete;
    KeywordRegistry& operator=(const KeywordRegistry&) = delete;
    KeywordRegistry(KeywordRegistry&&) = default;
    KeywordRegistry& operator=(KeywordRegistry&&) = default;

    /**
     * @brief تهيئة السجل مع الخريطة القياسية للغة 'ض' (64 كلمة)
     */
    void initializeStandardKeywords();

    /**
     * @brief التحقق من كون النص كلمة مفتاحية
     * @param text النص المراد فحصه
     * @return نوع الكلمة المفتاحية إذا وجدت، وإلا std::nullopt
     */
    [[nodiscard]] std::optional<KeywordType> findKeyword(std::string_view text) const noexcept;

    /**
     * @brief الحصول على النص العربي لكلمة مفتاحية
     */
    [[nodiscard]] std::string_view getKeywordText(KeywordType type) const noexcept;

    /**
     * @brief الحصول على مكافئ C++ لكلمة مفتاحية
     */
    [[nodiscard]] std::string_view getCppEquivalent(KeywordType type) const noexcept;

    /**
     * @brief التحقق من كون الكلمة المفتاحية نوع بيانات
     */
    [[nodiscard]] static bool isDataType(KeywordType type) noexcept;

    /**
     * @brief التحقق من كون الكلمة المفتاحية جملة تحكم
     */
    [[nodiscard]] static bool isControlFlow(KeywordType type) noexcept;

    /**
     * @brief التحقق من كون الكلمة المفتاحية معدّل وصول
     */
    [[nodiscard]] static bool isAccessSpecifier(KeywordType type) noexcept;

private:
    struct KeywordInfo {
        std::string_view arabicText;
        std::string_view cppEquivalent;
    };

    std::unordered_map<std::string_view, KeywordType> m_keywordToType;

    struct KeywordTypeHash {
        size_t operator()(KeywordType type) const noexcept {
            return std::hash<int>()(static_cast<int>(type));
        }
    };
    std::unordered_map<KeywordType, KeywordInfo, KeywordTypeHash> m_typeToInfo;
};

/**
 * @brief الحصول على مرجع وحيد (Singleton) لسجل الكلمات المفتاحية
 */
[[nodiscard]] const KeywordRegistry& getStandardKeywordRegistry() noexcept;

} // namespace daad
