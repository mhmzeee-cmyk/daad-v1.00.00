#pragma once

#include <string>
#include <cstddef>

namespace daad {

enum class TokenType {
    // Literals
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,

    // Operators
    TOKEN_PLUS,
    TOKEN_PLUS_PLUS,
    TOKEN_PLUS_EQUALS,
    TOKEN_MINUS,
    TOKEN_MINUS_MINUS,
    TOKEN_MINUS_EQUALS,
    TOKEN_STAR,
    TOKEN_STAR_EQUALS,
    TOKEN_SLASH,
    TOKEN_SLASH_EQUALS,
    TOKEN_PERCENT,
    TOKEN_PERCENT_EQUALS,
    TOKEN_POWER,
    TOKEN_POWER_ASSIGN,
    TOKEN_EQUALS,
    TOKEN_EQUAL_EQUAL,
    TOKEN_NOT,
    TOKEN_NOT_EQUALS,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_LESS,
    TOKEN_LESS_EQUALS,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUALS,
    TOKEN_QUESTION,

    // Delimiters
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_SEMICOLON,

    // Special
    TOKEN_EOF,
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type = TokenType::TOKEN_UNKNOWN;
    std::string text;
    size_t line = 0;
    size_t column = 0;
};

} // namespace daad
