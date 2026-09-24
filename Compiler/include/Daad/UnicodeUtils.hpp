#pragma once

#include <string_view>
#include <vector>
#include <cstddef>

namespace daad::unicode {

bool isArabicChar(char32_t codepoint) noexcept;
std::vector<char32_t> toCodepoints(std::string_view utf8Text);
bool isWhitespace(char32_t codepoint) noexcept;
bool isIdentifierStart(char32_t codepoint) noexcept;
bool isIdentifierPart(char32_t codepoint) noexcept;

} // namespace daad::unicode
