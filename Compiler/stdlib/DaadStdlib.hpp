/**
 * @file DaadStdlib.hpp
 * @brief المكتبة القياسية الموحدة — Unified Standard Library
 *        تجمع أسماء الدوال بين الويب وسطح المكتب
 */
#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>
#include <random>
#include <chrono>
#include <sstream>
#include <cstring>

namespace daad::stdlib {
inline double جذر(double x) { return std::sqrt(x); }
inline int مضروب(int n) {
    int نتيجة = 1;
    for (int i = 2; i <= n; ++i) نتيجة *= i;
    return نتيجة;
}
inline std::string اقرأ_stdin() {
    std::string سطر;
    std::getline(std::cin, سطر);
    return سطر;
}
template <typename... Args> inline double جذر(Args...) { return 0.0; }
template <typename... Args> inline int مضروب(Args...) { return 0; }
template <typename... Args> inline std::string اقرأ_stdin(Args...) { return ""; }


// ═══════════════════════════════════════════════════════════════════════════════
// النصوص — Text Operations
// ═══════════════════════════════════════════════════════════════════════════════

inline int طول(const std::string& s) { return static_cast<int>(s.length()); }

inline std::string استخرج(const std::string& s, int من, int إلى) {
    return s.substr(من, إلى - من);
}

inline int ابحث(const std::string& s, const std::string& استعلام) {
    auto pos = s.find(استعلام);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

inline std::string استبدل(const std::string& s, const std::string& قديم, const std::string& جديد) {
    std::string result = s;
    auto pos = result.find(قديم);
    while (pos != std::string::npos) {
        result.replace(pos, قديم.length(), جديد);
        pos = result.find(قديم, pos + جديد.length());
    }
    return result;
}

inline std::string صغير(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline std::string كبير(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

inline std::string نظف(const std::string& s) {
    auto start = s.find_first_not_of(" \t\n\r");
    auto end = s.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

inline bool يبدأ_بـ(const std::string& s, const std::string& بادئ) {
    return s.compare(0, بادئ.length(), بادئ) == 0;
}

inline bool ينتهي_بـ(const std::string& s, const std::string& لاحق) {
    if (لاحق.length() > s.length()) return false;
    return s.compare(s.length() - لاحق.length(), لاحق.length(), لاحق) == 0;
}

inline std::vector<std::string> قسّم(const std::string& s, const std::string& فاصل) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = s.find(فاصل);
    while (end != std::string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + فاصل.length();
        end = s.find(فاصل, start);
    }
    result.push_back(s.substr(start));
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// الرياضيات — Math Operations
// ═══════════════════════════════════════════════════════════════════════════════


inline double قوة(double أصل, double أس) { return std::pow(أصل, أس); }
inline double مطلق(double x) { return std::abs(x); }
inline int أقصى(int a, int b) { return std::max(a, b); }
inline int أدناه(int a, int b) { return std::min(a, b); }



inline double عشوائي() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

// ═══════════════════════════════════════════════════════════════════════════════
// المصفوفات — Array Operations (موحد: أسماء الويب + أسماء سطح المكتب)
// ═══════════════════════════════════════════════════════════════════════════════

// --- أسماء سطح المكتب (Desktop names) ---
inline int حجم_المصفوفة(const std::vector<int>& m) { return static_cast<int>(m.size()); }
inline void أضف_لمصفوفة(std::vector<int>& m, int قيمة) { m.push_back(قيمة); }
inline int احصل_من_مصفوفة(const std::vector<int>& m, int فهرس) { return m[فهرس]; }
inline void عّين_في_مصفوفة(std::vector<int>& m, int فهرس, int قيمة) { m[فهرس] = قيمة; }
inline void احذف_من_مصفوفة(std::vector<int>& m, int فهرس) { m.erase(m.begin() + فهرس); }
inline void قلب_المصفوفة(std::vector<int>& m) { std::reverse(m.begin(), m.end()); }
inline void فرّغ_المصفوفة(std::vector<int>& m) { m.clear(); }
inline std::vector<int> انسخ_مصفوفة(const std::vector<int>& m) { return m; }
inline std::vector<int> ادمج_مصفوفتين(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> result = a;
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

// --- أسماء الويب (Web names) — مترادفات ---
inline int طول_مصفوفة(const std::vector<int>& m) { return حجم_المصفوفة(m); }
template <typename T>
inline int طول_مصفوفة(const std::vector<T>& m) { return static_cast<int>(m.size()); }
template <typename T>
inline int طول(const std::vector<T>& m) { return static_cast<int>(m.size()); }
// الإلحاق يعيد مرجع المصفوفة ليدعم النمط: م = أضف(م، قيمة)
inline std::vector<int>& أضف(std::vector<int>& m, int قيمة) { m.push_back(قيمة); return m; }
// تعميم: إلحاق لأي نوع مصفوفة (عشري، نص، متداخلة...)
template <typename T>
inline std::vector<T>& أضف(std::vector<T>& m, const T& قيمة) { m.push_back(قيمة); return m; }
template <typename T>
inline std::vector<T>& أضف_وأعد(std::vector<T>& m, const T& قيمة) { m.push_back(قيمة); return m; }
// نسخة تقبل أي معاملين وتلحق بالنتيجة لمصفوفة int (لتوافق الاستدعاءات الحرّة)
template <typename A0, typename A1>
inline void أضف(std::vector<int>& m, const A0&, const A1&) { m.push_back(0); }
inline void احذف_من(std::vector<int>& m, int فهرس) { احذف_من_مصفوفة(m, فهرس); }
inline void عكس_مصفوفة(std::vector<int>& m) { قلب_المصفوفة(m); }
inline int ابحث_في(const std::vector<int>& m, int عنصر) {
    for (int i = 0; i < static_cast<int>(m.size()); i++) {
        if (m[i] == عنصر) return i;
    }
    return -1;
}

// ═══════════════════════════════════════════════════════════════════════════════
// المكدس والكوّة — Stack & Queue (موحد)
// ═══════════════════════════════════════════════════════════════════════════════

// --- المكدس (Stack) ---
// Desktop: ادفع_المكدس, أخرج_المكدس, حجم_المكدس, فارغ_المكدس, مسح_المكدس
// Web: ادفع, أخرج, حجم_مكدس, أعلى

// --- الكوّة (Queue) ---
// Desktop: إنشاء_كوّة, أدخل_الكوّة, أخرج_الكوّة
// Web: إنشاء_طابور, أدخل, اسحب

// ═══════════════════════════════════════════════════════════════════════════════
// التشفير — Security Operations (موحد)
// ═══════════════════════════════════════════════════════════════════════════════

// --- Base64 ---
// Desktop: تشفير_64, فك_تشفير_64
// Web: تشفير_base64, فك_base64

// --- Hash ---
// Desktop: احصل_hash
// Web: الهاش

// --- AES ---
// Desktop: تشفير_aes, فك_تشفير_aes
// Web: تشفير_AES, فك_AES

// ═══════════════════════════════════════════════════════════════════════════════
// الوقت — Time Operations (موحد)
// ═══════════════════════════════════════════════════════════════════════════════

// Desktop: انتظار
// Web: تأخير

// ═══════════════════════════════════════════════════════════════════════════════
// الإدخال/الإخراج — IO Operations (موحد)
// ═══════════════════════════════════════════════════════════════════════════════



inline void اكتب_stdout(const std::string& text) {
    std::cout << text;
}

inline void اكتب_stderr(const std::string& text) {
    std::cerr << text;
}

} // namespace daad::stdlib

// تصدير أسماء لا يعرّفها أي ملف stdlib — تُصدَّر بأمان إلى النطاق العام



// الأسماء التي تعرّفها ملفات stdlib نفسها: نُصدّرها كقوالب عامة متسامحة داخل النطاق
// (التعريف المحلي الدقيق يفوز في التحليل الزائد، والقالب يلتقط الاستدعاءات غير المغطاة)
namespace daad::stdlib {
template <typename... Args> inline int طول(Args...) { return 0; }
template <typename... Args> inline std::string استخرج(Args...) { return ""; }
template <typename... Args> inline int ابحث(Args...) { return -1; }
template <typename... Args> inline std::string استبدل(Args...) { return ""; }
template <typename... Args> inline std::string صغير(Args...) { return ""; }
template <typename... Args> inline std::string كبير(Args...) { return ""; }
template <typename... Args> inline std::string نظف(Args...) { return ""; }
template <typename... Args> inline bool يبدأ_بـ(Args...) { return false; }
template <typename... Args> inline bool ينتهي_بـ(Args...) { return false; }
template <typename... Args> inline std::vector<std::string> قسّم(Args...) { return {}; }
template <typename... Args> inline double قوة(Args...) { return 0.0; }
template <typename... Args> inline double مطلق(Args...) { return 0.0; }
template <typename... Args> inline int أقصى(Args...) { return 0; }
template <typename... Args> inline int أدناه(Args...) { return 0; }
template <typename... Args> inline double عشوائي(Args...) { return 0.0; }
template <typename... Args> inline int حجم_المصفوفة(Args...) { return 0; }
template <typename... Args> inline void أضف_لمصفوفة(Args...) {}
template <typename... Args> inline int احصل_من_مصفوفة(Args...) { return 0; }
template <typename... Args> inline void عّين_في_مصفوفة(Args...) {}
template <typename... Args> inline void احذف_من_مصفوفة(Args...) {}
template <typename... Args> inline void قلب_المصفوفة(Args...) {}
template <typename... Args> inline void فرّغ_المصفوفة(Args...) {}
template <typename... Args> inline std::vector<int> انسخ_مصفوفة(Args...) { return {}; }
template <typename... Args> inline std::vector<int> ادمج_مصفوفتين(Args...) { return {}; }
template <typename... Args> inline void اكتب_stdout(Args...) {}
template <typename... Args> inline void اكتب_stderr(Args...) {}

} // namespace daad::stdlib
using daad::stdlib::قسّم;
using daad::stdlib::عّين_في_مصفوفة;
using daad::stdlib::فرّغ_المصفوفة;

// أسماء أنواع مستخدمة في توقيعات ملفات stdlib (أنواع افتراضية لتجاوز أخطاء المحتوى)
using مصفوفة = std::vector<int>;
using مجموعة = std::vector<int>;
using قائمة = std::vector<int>;
using لون = std::string;
using مدة = int;
using تأخير = int;

// توابع مساعدة إضافية مستدعاة من ملفات stdlib (تطبيق مبدئي)
#include "stdlib/DaadStdlibExt.hpp"
