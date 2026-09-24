#pragma once

#include "Daad/Token.hpp"
#include <string_view>
#include <vector>
#include <cstddef>

namespace daad {

class Lexer {
public:
    explicit Lexer(std::string_view source);

    Token getNextToken();
    void saveState(size_t& pos, size_t& line, size_t& col) const;
    void restoreState(size_t pos, size_t line, size_t col);

private:
    Token scanNumber();
    Token scanString();
    Token scanIdentifier();
    char32_t peek(size_t offset = 0) const;
    char32_t advance();
    bool isAtEnd() const;
    std::string_view tokenText(size_t startCodepoint, size_t endCodepoint) const;
    Token makeToken(TokenType type, std::string_view text);
    Token errorToken(std::string_view message);

    std::string_view m_source;
    std::vector<char32_t> m_codepoints;
    std::vector<size_t> m_byteOffsets;
    size_t m_position = 0;
    size_t m_line = 1;
    size_t m_column = 1;
};

} // namespace daad
