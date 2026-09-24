/**
 * @file Token.hpp
 * @brief تعريف الرموز (Tokens) التي يولدها المحلل اللفظي (Lexer)
 * @author فريق ض استديو
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <string_view>

namespace daad {

/**
 * @enum TokenType
 * @brief أنواع الرموز الممكنة
 */
enum class TokenType {
    // الرموز الخاصة
    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    
    // الكلمات المفتاحية
    TOKEN_KEYWORD,
    
    // المعاملات
    TOKEN_PLUS,       // +
    TOKEN_MINUS,      // -
    TOKEN_STAR,       // *
    TOKEN_SLASH,      // /
    TOKEN_EQUALS,     // =
    TOKEN_SEMICOLON,  // ؛
    
    // الأقواس
    TOKEN_LEFT_BRACE, // {
    TOKEN_RIGHT_BRACE,// }
    TOKEN_LEFT_PAREN, // (
    TOKEN_RIGHT_PAREN, // )
    TOKEN_LESS,           // <
    TOKEN_GREATER,        // >
    TOKEN_LESS_EQUALS,    // <=
    TOKEN_GREATER_EQUALS, // >=
    TOKEN_EQUAL_EQUAL,    // ==
    TOKEN_NOT_EQUALS,     // !=
    TOKEN_COLON,          // :
    TOKEN_DOT,            // .
    TOKEN_PERCENT,        // %
    TOKEN_PLUS_EQUALS,    // +=
    TOKEN_MINUS_EQUALS,   // -=
    TOKEN_STAR_EQUALS,    // *=
    TOKEN_SLASH_EQUALS,   // /=
    TOKEN_LEFT_BRACKET,   // [
    TOKEN_RIGHT_BRACKET,  // ]
    TOKEN_AND,            // &&
    TOKEN_OR              // ||
};

/**
 * @struct Token
 * @brief بنية تمثل رمزاً واحداً
 */
struct Token {
    TokenType type;
    std::string text;
    size_t line;
    size_t column;
};

} // namespace daad