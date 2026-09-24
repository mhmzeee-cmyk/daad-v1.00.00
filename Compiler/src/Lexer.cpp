#include "Daad/Lexer.hpp"
#include "Daad/UnicodeUtils.hpp"
#include "Daad/Keywords.hpp"

namespace daad {

// ═══ Mapping: KeywordType → TokenType (TOKEN_KW_*) ═════════════════════════════
static TokenType keywordTypeToTokenType(KeywordType kt) {
    switch (kt) {
        // 1. Data Types & Fundamentals
        case KeywordType::KwInt:       return TokenType::TOKEN_KW_INT;
        case KeywordType::KwDouble:    return TokenType::TOKEN_KW_DOUBLE;
        case KeywordType::KwBool:      return TokenType::TOKEN_KW_BOOL;
        case KeywordType::KwChar:      return TokenType::TOKEN_KW_CHAR;
        case KeywordType::KwString:    return TokenType::TOKEN_KW_STRING;
        case KeywordType::KwConst:     return TokenType::TOKEN_KW_CONST;
        case KeywordType::KwVoid:      return TokenType::TOKEN_KW_VOID;
        case KeywordType::KwAuto:      return TokenType::TOKEN_KW_AUTO;

        // 2. Literals & Typedef
        case KeywordType::KwTrue:      return TokenType::TOKEN_KW_TRUE;
        case KeywordType::KwFalse:     return TokenType::TOKEN_KW_FALSE;
        case KeywordType::KwNullptr:   return TokenType::TOKEN_KW_NULLPTR;
        case KeywordType::KwTypedef:   return TokenType::TOKEN_KW_TYPEDEF;

        // 3. Control Flow
        case KeywordType::KwIf:        return TokenType::TOKEN_KW_IF;
        case KeywordType::KwElse:      return TokenType::TOKEN_KW_ELSE;
        case KeywordType::KwWhile:     return TokenType::TOKEN_KW_WHILE;
        case KeywordType::KwDo:        return TokenType::TOKEN_KW_DO;
        case KeywordType::KwFor:       return TokenType::TOKEN_KW_FOR;
        case KeywordType::KwIn:        return TokenType::TOKEN_KW_IN;
        case KeywordType::KwSwitch:    return TokenType::TOKEN_KW_SWITCH;
        case KeywordType::KwCase:      return TokenType::TOKEN_KW_CASE;
        case KeywordType::KwDefault:   return TokenType::TOKEN_KW_DEFAULT;
        case KeywordType::KwBreak:     return TokenType::TOKEN_KW_BREAK;
        case KeywordType::KwContinue:  return TokenType::TOKEN_KW_CONTINUE;
        case KeywordType::KwGoto:      return TokenType::TOKEN_KW_GOTO;
        case KeywordType::KwReturn:    return TokenType::TOKEN_KW_RETURN;

        // 4. OOP & Scoping
        case KeywordType::KwClass:     return TokenType::TOKEN_KW_CLASS;
        case KeywordType::KwStruct:    return TokenType::TOKEN_KW_STRUCT;
        case KeywordType::KwEnum:      return TokenType::TOKEN_KW_ENUM;
        case KeywordType::KwInterface: return TokenType::TOKEN_KW_INTERFACE;
        case KeywordType::KwNamespace: return TokenType::TOKEN_KW_NAMESPACE;

        // 5. Access & Inheritance
        case KeywordType::KwPublic:    return TokenType::TOKEN_KW_PUBLIC;
        case KeywordType::KwPrivate:   return TokenType::TOKEN_KW_PRIVATE;
        case KeywordType::KwProtected: return TokenType::TOKEN_KW_PROTECTED;
        case KeywordType::KwInherit:   return TokenType::TOKEN_KW_INHERIT;
        case KeywordType::KwSelf:      return TokenType::TOKEN_KW_SELF;
        case KeywordType::KwBase:      return TokenType::TOKEN_KW_BASE;
        case KeywordType::KwAbstract:  return TokenType::TOKEN_KW_ABSTRACT;

        // 6. Functions & Memory
        case KeywordType::KwFunction:  return TokenType::TOKEN_KW_FUNCTION;
        case KeywordType::KwNew:       return TokenType::TOKEN_KW_NEW;
        case KeywordType::KwDelete:    return TokenType::TOKEN_KW_DELETE;
        case KeywordType::KwPointer:   return TokenType::TOKEN_KW_POINTER;
        case KeywordType::KwReference: return TokenType::TOKEN_KW_REFERENCE;
        case KeywordType::KwStatic:    return TokenType::TOKEN_KW_STATIC;
        case KeywordType::KwInline:    return TokenType::TOKEN_KW_INLINE;
        case KeywordType::KwExtern:    return TokenType::TOKEN_KW_EXTERN;
        case KeywordType::KwTemplate:  return TokenType::TOKEN_KW_TEMPLATE;

        // 7. Exception Handling
        case KeywordType::KwTry:       return TokenType::TOKEN_KW_TRY;
        case KeywordType::KwCatch:     return TokenType::TOKEN_KW_CATCH;
        case KeywordType::KwFinally:   return TokenType::TOKEN_KW_FINALLY;
        case KeywordType::KwThrow:     return TokenType::TOKEN_KW_THROW;
        case KeywordType::KwAssert:    return TokenType::TOKEN_KW_ASSERT;
        case KeywordType::KwException: return TokenType::TOKEN_KW_EXCEPTION;
        case KeywordType::KwTypeOf:    return TokenType::TOKEN_KW_TYPEOF;
        case KeywordType::KwSizeOf:    return TokenType::TOKEN_KW_SIZEOF;
        case KeywordType::KwIncrement: return TokenType::TOKEN_KW_INCREMENT;
        case KeywordType::KwDecrement: return TokenType::TOKEN_KW_DECREMENT;

        // 8. Advanced Systems
        case KeywordType::KwSync:        return TokenType::TOKEN_KW_SYNC;
        case KeywordType::KwAwait:       return TokenType::TOKEN_KW_AWAIT;
        case KeywordType::KwThread:      return TokenType::TOKEN_KW_THREAD;
        case KeywordType::KwLock:        return TokenType::TOKEN_KW_LOCK;
        case KeywordType::KwShared:      return TokenType::TOKEN_KW_SHARED;
        case KeywordType::KwUnique:      return TokenType::TOKEN_KW_UNIQUE;
        case KeywordType::KwImport:      return TokenType::TOKEN_KW_IMPORT;
        case KeywordType::KwExport:      return TokenType::TOKEN_KW_EXPORT;
        case KeywordType::KwModule:      return TokenType::TOKEN_KW_MODULE;
        case KeywordType::KwAlternative: return TokenType::TOKEN_KW_ALTERNATIVE;

        // 9. GUI Keywords
        case KeywordType::KwButton:      return TokenType::TOKEN_KW_BUTTON;
        case KeywordType::KwTextField:   return TokenType::TOKEN_KW_TEXTFIELD;
        case KeywordType::KwComboBox:    return TokenType::TOKEN_KW_COMBOBOX;
        case KeywordType::KwImage:       return TokenType::TOKEN_KW_IMAGE;
        case KeywordType::KwCheckBox:    return TokenType::TOKEN_KW_CHECKBOX;
        case KeywordType::KwSlider:      return TokenType::TOKEN_KW_SLIDER;
        case KeywordType::KwDropDown:    return TokenType::TOKEN_KW_DROPDOWN;
        case KeywordType::KwPanel:       return TokenType::TOKEN_KW_PANEL;
        case KeywordType::KwLabel:       return TokenType::TOKEN_KW_LABEL;
        case KeywordType::KwColumn:      return TokenType::TOKEN_KW_COLUMN;
        case KeywordType::KwRow:         return TokenType::TOKEN_KW_ROW;
        case KeywordType::KwGrid:        return TokenType::TOKEN_KW_GRID;
        case KeywordType::KwProgressBar: return TokenType::TOKEN_KW_PROGRESSBAR;
        case KeywordType::KwTabBar:      return TokenType::TOKEN_KW_TABBAR;

        // 10. Print / Input
        case KeywordType::KwPrint:       return TokenType::TOKEN_KW_PRINT;
        case KeywordType::KwInput:       return TokenType::TOKEN_KW_INPUT;

        // 11. Arabic logic
        case KeywordType::KwAndArabic:   return TokenType::TOKEN_KW_AND_ARABIC;
        case KeywordType::KwOrArabic:    return TokenType::TOKEN_KW_OR_ARABIC;

        // 12. Image Processing
        case KeywordType::KwLoadImage:   return TokenType::TOKEN_KW_LOAD_IMAGE;
        case KeywordType::KwDrawImage:   return TokenType::TOKEN_KW_DRAW_IMAGE;
        case KeywordType::KwImageSize:   return TokenType::TOKEN_KW_IMAGE_SIZE;
        case KeywordType::KwSaveImage:   return TokenType::TOKEN_KW_SAVE_IMAGE;
        case KeywordType::KwCropImage:   return TokenType::TOKEN_KW_CROP_IMAGE;
        case KeywordType::KwResize:      return TokenType::TOKEN_KW_RESIZE;
        case KeywordType::KwRotateImage: return TokenType::TOKEN_KW_ROTATE_IMAGE;
        case KeywordType::KwFlipImage:   return TokenType::TOKEN_KW_FLIP_IMAGE;
        case KeywordType::KwOpacity:     return TokenType::TOKEN_KW_OPACITY;
        case KeywordType::KwFilter:      return TokenType::TOKEN_KW_FILTER;
        case KeywordType::KwOverlay:     return TokenType::TOKEN_KW_OVERLAY;
        case KeywordType::KwBackground:  return TokenType::TOKEN_KW_BACKGROUND;
        case KeywordType::KwPixel:       return TokenType::TOKEN_KW_PIXEL;
        case KeywordType::KwDraw:        return TokenType::TOKEN_KW_DRAW;
        case KeywordType::KwFill:        return TokenType::TOKEN_KW_FILL;
        case KeywordType::KwRectangle:   return TokenType::TOKEN_KW_RECTANGLE;
        case KeywordType::KwCircle:      return TokenType::TOKEN_KW_CIRCLE;
        case KeywordType::KwLine:        return TokenType::TOKEN_KW_LINE;
        case KeywordType::KwTextOnCanvas: return TokenType::TOKEN_KW_TEXT_ON_CANVAS;
        case KeywordType::KwClear:       return TokenType::TOKEN_KW_CLEAR;

        // 13. Coordinate axes
        case KeywordType::KwX:           return TokenType::TOKEN_KW_X;
        case KeywordType::KwY:           return TokenType::TOKEN_KW_Y;
    }
    return TokenType::TOKEN_KEYWORD; // fallback
}

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
    auto kwType = registry.findKeyword(text);
    if (kwType.has_value()) {
        // Return the distinct TOKEN_KW_* type instead of generic TOKEN_KEYWORD
        return makeToken(keywordTypeToTokenType(kwType.value()), text);
    }

    return makeToken(TokenType::TOKEN_IDENTIFIER, text);
}

} // namespace daad
