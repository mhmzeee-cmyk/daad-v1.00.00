#include "Daad/Lexer.hpp"
#include "Daad/UnicodeUtils.hpp"
#include "Daad/Keywords.hpp"

namespace daad {

Lexer::Lexer(std::string_view source)
    : m_source(source), m_position(0), m_line(1), m_column(1) {
    m_codepoints = unicode::toCodepoints(m_source);
    m_byteOffsets.reserve(m_codepoints.size());
    size_t bytePos = 0;
    for (char32_t cp : m_codepoints) {
        m_byteOffsets.push_back(bytePos);
        if (cp < 0x80) bytePos += 1;
        else if (cp < 0x800) bytePos += 2;
        else if (cp < 0x10000) bytePos += 3;
        else bytePos += 4;
    }
}

std::string_view Lexer::tokenText(size_t startCodepoint, size_t endCodepoint) const {
    if (startCodepoint >= m_codepoints.size()) return "";
    size_t byteStart = m_byteOffsets[startCodepoint];
    size_t byteEnd;
    if (endCodepoint >= m_codepoints.size()) {
        byteEnd = m_source.size();
    } else {
        byteEnd = m_byteOffsets[endCodepoint];
    }
    return m_source.substr(byteStart, byteEnd - byteStart);
}

void Lexer::saveState(size_t& pos, size_t& line, size_t& col) const {
    pos = m_position;
    line = m_line;
    col = m_column;
}

void Lexer::restoreState(size_t pos, size_t line, size_t col) {
    m_position = pos;
    m_line = line;
    m_column = col;
}

Token Lexer::getNextToken() {
    while (!isAtEnd() && unicode::isWhitespace(peek())) {
        if (peek() == '\n') {
            m_line++;
            m_column = 1;
        }
        advance();
    }

    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF, "");

    char32_t c = peek();

    if (c == ';' || c == 0x061B) { advance(); return makeToken(TokenType::TOKEN_SEMICOLON, ";"); }

    if (c == '"' ) return scanString();
    if (unicode::isIdentifierStart(c)) return scanIdentifier();
    if (c >= '0' && c <= '9') return scanNumber();

    if (c == '+' && peek(1) == '+') { advance(); advance(); return makeToken(TokenType::TOKEN_PLUS_PLUS, "++"); }
    if (c == '+' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_PLUS_EQUALS, "+="); }
    if (c == '+' ) { advance(); return makeToken(TokenType::TOKEN_PLUS, "+"); }
    if (c == '-' && peek(1) == '-') { advance(); advance(); return makeToken(TokenType::TOKEN_MINUS_MINUS, "--"); }
    if (c == '-' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_MINUS_EQUALS, "-="); }
    if (c == '-' ) { advance(); return makeToken(TokenType::TOKEN_MINUS, "-"); }
    if (c == '*' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_STAR_EQUALS, "*="); }
    if (c == '*' ) { advance(); return makeToken(TokenType::TOKEN_STAR, "*"); }
    if (c == '/' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_SLASH_EQUALS, "/="); }
    if (c == '/' && peek(1) == '/') {
        while (!isAtEnd() && peek() != '\n') advance();
        return getNextToken();
    }
    if (c == '/' && peek(1) == '*') {
        advance(); advance();
        while (!isAtEnd() && !(peek() == '*' && peek(1) == '/')) {
            if (peek() == '\n') { m_line++; m_column = 1; }
            advance();
        }
        if (isAtEnd()) {
            return errorToken("تعليق غير مغلق: ينقص */");
        }
        advance(); advance();
        return getNextToken();
    }
    if (c == '/' ) { advance(); return makeToken(TokenType::TOKEN_SLASH, "/"); }
    if (c == '%' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_PERCENT_EQUALS, "%="); }
    if (c == '%' ) { advance(); return makeToken(TokenType::TOKEN_PERCENT, "%"); }
    if (c == '=' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_EQUAL_EQUAL, "=="); }
    if (c == '!' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_NOT_EQUALS, "!="); }
    if (c == '!') { advance(); return makeToken(TokenType::TOKEN_NOT, "!"); }
    if (c == '&' && peek(1) == '&') { advance(); advance(); return makeToken(TokenType::TOKEN_AND, "&&"); }
    if (c == '|' && peek(1) == '|') { advance(); advance(); return makeToken(TokenType::TOKEN_OR, "||"); }
    if (c == '<' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_LESS_EQUALS, "<="); }
    if (c == '>' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_GREATER_EQUALS, ">="); }
    if (c == '=' ) { advance(); return makeToken(TokenType::TOKEN_EQUALS, "="); }
    if (c == '{') { advance(); return makeToken(TokenType::TOKEN_LEFT_BRACE, "{"); }
    if (c == '}') { advance(); return makeToken(TokenType::TOKEN_RIGHT_BRACE, "}"); }
    if (c == '(') { advance(); return makeToken(TokenType::TOKEN_LEFT_PAREN, "("); }
    if (c == ')') { advance(); return makeToken(TokenType::TOKEN_RIGHT_PAREN, ")"); }
    if (c == '<') { advance(); return makeToken(TokenType::TOKEN_LESS, "<"); }
    if (c == '>') { advance(); return makeToken(TokenType::TOKEN_GREATER, ">"); }
    if (c == ',') { advance(); return makeToken(TokenType::TOKEN_COMMA, ","); }
    if (c == 0x060C) { advance(); return makeToken(TokenType::TOKEN_COMMA, ","); } //Arabic comma ،
    if (c == ':') { advance(); return makeToken(TokenType::TOKEN_COLON, ":"); }
    if (c == '.') { advance(); return makeToken(TokenType::TOKEN_DOT, "."); }
    if (c == '[') { advance(); return makeToken(TokenType::TOKEN_LEFT_BRACKET, "["); }
    if (c == ']') { advance(); return makeToken(TokenType::TOKEN_RIGHT_BRACKET, "]"); }
    if (c == '?') { advance(); return makeToken(TokenType::TOKEN_QUESTION, "?"); }
    if (c == '^' && peek(1) == '=') { advance(); advance(); return makeToken(TokenType::TOKEN_POWER_ASSIGN, "^="); }
    if (c == '^') { advance(); return makeToken(TokenType::TOKEN_POWER, "^"); }

    advance();
    return errorToken("رمز غير معروف");
}

Token Lexer::scanNumber() {
    size_t start = m_position;
    while (!isAtEnd() && peek() >= '0' && peek() <= '9') {
        advance();
    }
    if (!isAtEnd() && peek() == '.') {
        advance();
        while (!isAtEnd() && peek() >= '0' && peek() <= '9') {
            advance();
        }
    }
    return makeToken(TokenType::TOKEN_NUMBER, tokenText(start, m_position));
}

Token Lexer::scanString() {
    advance(); // skip opening "
    size_t startCodepoint = m_position;
    bool hasEscape = false;

    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            hasEscape = true;
            advance(); // skip backslash
            if (!isAtEnd()) advance(); // skip escaped char
        } else {
            advance();
        }
    }

    if (isAtEnd()) {
        return errorToken("نص غير مغلق: ينقص علامة اقتباس \"");
    }

    size_t endCodepoint = m_position;
    advance(); // skip closing "

    if (!hasEscape) {
        return makeToken(TokenType::TOKEN_STRING, tokenText(startCodepoint, endCodepoint));
    }

    // Process escape sequences from raw text
    std::string_view raw = tokenText(startCodepoint, endCodepoint);
    std::string result;
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            ++i;
            unsigned char e = static_cast<unsigned char>(raw[i]);
            // ثماني: \0..\777 (يدعم أكواد ANSI مثل \033 التي كانت تنكسر)
            if (e >= '0' && e <= '7') {
                int val = 0; int digits = 0;
                while (i < raw.size() && raw[i] >= '0' && raw[i] <= '7' && digits < 3) {
                    val = val * 8 + (raw[i] - '0');
                    ++i; ++digits;
                }
                --i;
                result += static_cast<char>(val);
            } else {
                switch (e) {
                    case 'n':  result += '\n'; break;
                    case 't':  result += '\t'; break;
                    case 'r':  result += '\r'; break;
                    case '\\': result += '\\'; break;
                    case '"':  result += '"'; break;
                    default:   result += '\\'; result += e; break; // مجهول: احفظ كما هو
                }
            }
        } else {
            result += raw[i];
        }
    }
    return makeToken(TokenType::TOKEN_STRING, result);
}

char32_t Lexer::peek(size_t offset) const {
    if (m_position + offset >= m_codepoints.size()) return 0;
    return m_codepoints[m_position + offset];
}

char32_t Lexer::advance() {
    if (m_position >= m_codepoints.size()) return 0;
    char32_t c = m_codepoints[m_position++];
    if (c == '\t') m_column += 4;
    else m_column++;
    return c;
}

bool Lexer::isAtEnd() const {
    return m_position >= m_codepoints.size();
}

Token Lexer::makeToken(TokenType type, std::string_view text) {
    return {type, std::string(text), m_line, m_column};
}

Token Lexer::errorToken(std::string_view message) {
    return {TokenType::TOKEN_UNKNOWN, std::string(message), m_line, m_column};
}

Token Lexer::scanIdentifier() {
    size_t start = m_position;
    while (!isAtEnd() && unicode::isIdentifierPart(peek())) {
        advance();
    }
    std::string_view text = tokenText(start, m_position);

    auto& registry = getStandardKeywordRegistry();
    if (registry.findKeyword(text)) {
        return makeToken(TokenType::TOKEN_KEYWORD, text);
    }

    return makeToken(TokenType::TOKEN_IDENTIFIER, text);
}

} // namespace daad
