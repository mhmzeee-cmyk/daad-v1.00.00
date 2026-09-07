/**
 * @file Lexer.hpp
 * @brief المحلل اللفظي للغة 'ض'
 * @author فريق ض استديو
 * @version 1.0.0
 */

#pragma once

#include "Daad/Token.hpp"
#include "Daad/Keywords.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace daad {

class Lexer {
public:
    explicit Lexer(std::string_view source);
    ~Lexer() = default;

    Token getNextToken();

    // حفظ/استعادة حالة المسح (للتطلع للأمام في Parser)
    void saveState(size_t& pos, size_t& line, size_t& col) const;
    void restoreState(size_t pos, size_t line, size_t col);

private:
    std::string_view m_source;
    std::vector<char32_t> m_codepoints;
    std::vector<size_t> m_byteOffsets;
    size_t m_position;
    size_t m_line;
    size_t m_column;

    char32_t peek(size_t offset = 0) const;
    char32_t advance();
    bool isAtEnd() const;

    std::string_view tokenText(size_t startCodepoint, size_t endCodepoint) const;

    Token makeToken(TokenType type, std::string_view text);
    Token errorToken(std::string_view message);

    Token scanIdentifier();
    Token scanNumber();
    Token scanString();
};

} // namespace daad
