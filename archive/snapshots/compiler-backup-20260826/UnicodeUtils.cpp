/**
 * @file UnicodeUtils.cpp
 * @brief تنفيذ أدوات التعامل مع UTF-8
 */

#include "Daad/UnicodeUtils.hpp"

namespace daad::unicode {

bool isArabicChar(char32_t codepoint) noexcept {
    // نطاق الحروف العربية الأساسية في Unicode
    // Note: 0xFEFF is BOM (Byte Order Mark), not Arabic
    return (codepoint >= 0x0600 && codepoint <= 0x06FF) ||
           (codepoint >= 0x0750 && codepoint <= 0x077F) ||
           (codepoint >= 0xFB50 && codepoint <= 0xFDFF) ||
           (codepoint >= 0xFE70 && codepoint <= 0xFEFE);
}

std::vector<char32_t> toCodepoints(std::string_view utf8Text) {
    std::vector<char32_t> codepoints;
    // تنفيذ مبسط لتحويل UTF-8 إلى char32_t (نقاط ترميز)
    // في مشروع حقيقي، يُفضل استخدام مكتبة مثل ICU
    for (size_t i = 0; i < utf8Text.size(); ) {
        unsigned char c = static_cast<unsigned char>(utf8Text[i]);
        if (c < 0x80) {
            codepoints.push_back(static_cast<char32_t>(c));
            i++;
        } else if ((c & 0xE0) == 0xC0) {
            char32_t cp = (static_cast<char32_t>(c & 0x1F) << 6) | (static_cast<char32_t>(utf8Text[i+1]) & 0x3F);
            codepoints.push_back(cp);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            char32_t cp = (static_cast<char32_t>(c & 0x0F) << 12) | (static_cast<char32_t>(utf8Text[i+1] & 0x3F) << 6) | (static_cast<char32_t>(utf8Text[i+2]) & 0x3F);
            codepoints.push_back(cp);
            i += 3;
        } else {
            // تجاهل أحرف 4 بايت للمبسط
            i++;
        }
    }
    return codepoints;
}

bool isWhitespace(char32_t codepoint) noexcept {
    return codepoint == ' ' || codepoint == '\t' || codepoint == '\n' || codepoint == '\r' || codepoint == 0xFEFF;
}

bool isIdentifierStart(char32_t codepoint) noexcept {
    return (codepoint >= 'a' && codepoint <= 'z') ||
           (codepoint >= 'A' && codepoint <= 'Z') ||
           codepoint == '_' ||
           isArabicChar(codepoint);
}

bool isIdentifierPart(char32_t codepoint) noexcept {
    return isIdentifierStart(codepoint) || (codepoint >= '0' && codepoint <= '9');
}

} // namespace daad::unicode