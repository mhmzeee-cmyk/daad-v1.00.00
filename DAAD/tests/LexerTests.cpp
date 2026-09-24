#include <gtest/gtest.h>
#include "Daad/Lexer.hpp"

TEST(LexerTest, IdentifierAndKeyword) {
    std::string source = "صحيح متغير";
    daad::Lexer lexer(source);

    auto token1 = lexer.getNextToken();
    EXPECT_EQ(token1.type, daad::TokenType::TOKEN_KEYWORD);
    EXPECT_EQ(token1.text, "صحيح");

    auto token2 = lexer.getNextToken();
    EXPECT_EQ(token2.type, daad::TokenType::TOKEN_IDENTIFIER);
    EXPECT_EQ(token2.text, "متغير");
}

TEST(LexerTest, OperatorsAndParens) {
    std::string source = "( 1 + 2 )";
    daad::Lexer lexer(source);

    auto tokens = std::vector<daad::TokenType>{
        daad::TokenType::TOKEN_LEFT_PAREN,
        daad::TokenType::TOKEN_NUMBER,
        daad::TokenType::TOKEN_PLUS,
        daad::TokenType::TOKEN_NUMBER,
        daad::TokenType::TOKEN_RIGHT_PAREN
    };

    for (auto expected : tokens) {
        auto token = lexer.getNextToken();
        EXPECT_EQ(token.type, expected);
    }
}

TEST(LexerTest, Semicolon) {
    std::string source = "متغير = 5 ؛";
    daad::Lexer lexer(source);

    auto id = lexer.getNextToken();
    EXPECT_EQ(id.type, daad::TokenType::TOKEN_IDENTIFIER);

    auto eq = lexer.getNextToken();
    EXPECT_EQ(eq.type, daad::TokenType::TOKEN_EQUALS);

    auto num = lexer.getNextToken();
    EXPECT_EQ(num.type, daad::TokenType::TOKEN_NUMBER);
    EXPECT_EQ(num.text, "5");

    auto semi = lexer.getNextToken();
    EXPECT_EQ(semi.type, daad::TokenType::TOKEN_SEMICOLON);
}

TEST(LexerTest, StringToken) {
    std::string source = "\"مرحبا بالعالم\"";
    daad::Lexer lexer(source);

    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_STRING);
    EXPECT_EQ(token.text, "مرحبا بالعالم");
}

TEST(LexerTest, MultiLineTracking) {
    std::string source = "صحيح س\n= 5";
    daad::Lexer lexer(source);

    auto kw = lexer.getNextToken();
    EXPECT_EQ(kw.line, 1);

    auto id = lexer.getNextToken();
    EXPECT_EQ(id.line, 1);

    auto eq = lexer.getNextToken();
    EXPECT_EQ(eq.line, 2);
}

TEST(LexerTest, FloatNumber) {
    std::string source = "3.14";
    daad::Lexer lexer(source);

    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_NUMBER);
    EXPECT_EQ(token.text, "3.14");
}

TEST(LexerTest, EOFToken) {
    std::string source = "";
    daad::Lexer lexer(source);

    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_EOF);
}

TEST(LexerTest, CompleteDeclaration) {
    std::string source = "صحيح متغير = 10 ؛";
    daad::Lexer lexer(source);

    EXPECT_EQ(lexer.getNextToken().type, daad::TokenType::TOKEN_KEYWORD);
    EXPECT_EQ(lexer.getNextToken().type, daad::TokenType::TOKEN_IDENTIFIER);
    EXPECT_EQ(lexer.getNextToken().type, daad::TokenType::TOKEN_EQUALS);
    EXPECT_EQ(lexer.getNextToken().type, daad::TokenType::TOKEN_NUMBER);
    EXPECT_EQ(lexer.getNextToken().type, daad::TokenType::TOKEN_SEMICOLON);
    EXPECT_EQ(lexer.getNextToken().type, daad::TokenType::TOKEN_EOF);
}