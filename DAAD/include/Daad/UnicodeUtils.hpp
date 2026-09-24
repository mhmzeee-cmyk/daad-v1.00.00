/**
 * @file UnicodeUtils.hpp
 * @brief أدوات مساعدة للتعامل مع ترميز UTF-8 لدعم اللغة العربية
 * @author فريق ض استديو
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <vector>

namespace daad::unicode {

/**
 * @brief فحص ما إذا كان الحرف هو حرف عربي (ضمن نطاق Unicode العربي)
 * @param codepoint رمز Unicode للحرف
 * @return true إذا كان الحرف عربياً
 */
[[nodiscard]] bool isArabicChar(char32_t codepoint) noexcept;

/**
 * @brief تحويل UTF-8 إلى نقاط ترميز (Codepoints)
 * @param utf8Text النص بترميز UTF-8
 * @return قائمة بنقاط الترميز
 */
[[nodiscard]] std::vector<char32_t> toCodepoints(std::string_view utf8Text);

/**
 * @brief فحص ما إذا كان الحرف هو مسافة بيضاء
 */
[[nodiscard]] bool isWhitespace(char32_t codepoint) noexcept;

/**
 * @brief فحص ما إذا كان الحرف بداية معرف (Identifier)
 */
[[nodiscard]] bool isIdentifierStart(char32_t codepoint) noexcept;

/**
 * @brief فحص ما إذا كان الحرف جزء من معرف (Identifier)
 */
[[nodiscard]] bool isIdentifierPart(char32_t codepoint) noexcept;

} // namespace daad::unicode