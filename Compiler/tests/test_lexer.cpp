#include <gtest/gtest.h>
#include "Daad/Lexer.hpp"
#include "Daad/Token.hpp"
#include "Daad/UnicodeUtils.hpp"

using namespace daad;

// اختبار تحليل الكلمات العربية — صحيح
TEST(LexerArabicTest, KeywordSahih) {
    Lexer lexer("صحيح");
    Token tok = lexer.getNextToken();
    EXPECT_EQ(tok.type, TokenType::TOKEN_KW_INT);
    EXPECT_EQ(tok.text, "صحيح");
    Token eof = lexer.getNextToken();
    EXPECT_EQ(eof.type, TokenType::TOKEN_EOF);
}

// اختبار كلمة طباعة
TEST(LexerArabicTest, KeywordTabaa) {
    Lexer lexer("طباعة");
    Token tok = lexer.getNextToken();
    EXPECT_EQ(tok.type, TokenType::TOKEN_KW_PRINT);
    EXPECT_EQ(tok.text, "طباعة");
}

// اختبار الفاصلة المنقوطة العربية ؛ U+061B
TEST(LexerArabicTest, ArabicSemicolon) {
    Lexer lexer("؛");
    Token tok = lexer.getNextToken();
    EXPECT_EQ(tok.type, TokenType::TOKEN_SEMICOLON);
    // وأيضا داخل جملة كاملة
    Lexer lexer2("صحيح س = 10 ؛");
    // تقدم حتى الفاصلة
    lexer2.getNextToken(); // صحيح
    lexer2.getNextToken(); // س
    lexer2.getNextToken(); // =
    lexer2.getNextToken(); // 10
    Token semi = lexer2.getNextToken();
    EXPECT_EQ(semi.type, TokenType::TOKEN_SEMICOLON);
}

// اختبار الفاصلة العربية ، U+060C
TEST(LexerArabicTest, ArabicComma) {
    Lexer lexer("،");
    Token tok = lexer.getNextToken();
    EXPECT_EQ(tok.type, TokenType::TOKEN_COMMA);
    // داخل استدعاء دالة
    Lexer lexer2("دالة جمع(صحيح أ ، صحيح ب)");
    // نتخطى حتى الفاصلة
    bool foundComma = false;
    for (int i = 0; i < 10; ++i) {
        Token t = lexer2.getNextToken();
        if (t.type == TokenType::TOKEN_COMMA) { foundComma = true; break; }
        if (t.type == TokenType::TOKEN_EOF) break;
    }
    EXPECT_TRUE(foundComma);
}

// اختبار سلسلة غير مغلقة يجب أن ينتج TOKEN_UNKNOWN
TEST(LexerArabicTest, UnclosedString) {
    Lexer lexer("\"مرحبا بالعالم");
    Token tok = lexer.getNextToken();
    EXPECT_EQ(tok.type, TokenType::TOKEN_UNKNOWN);
    // الرسالة يجب أن تحتوي على "نص غير مغلق"
    EXPECT_NE(tok.text.find("نص غير مغلق"), std::string::npos);
}

// اختبار معرف عربي مختلط مع إنجليزي
TEST(LexerArabicTest, MixedIdentifier) {
    Lexer lexer("متغير_1");
    Token tok = lexer.getNextToken();
    EXPECT_EQ(tok.type, TokenType::TOKEN_IDENTIFIER);
    EXPECT_EQ(tok.text, "متغير_1");
}

// اختبار رقم مع فاصلة عربية لا يخلط
TEST(LexerArabicTest, NumberAndArabicComma) {
    Lexer lexer("10 ، 20");
    Token t1 = lexer.getNextToken();
    Token comma = lexer.getNextToken();
    Token t2 = lexer.getNextToken();
    EXPECT_EQ(t1.type, TokenType::TOKEN_NUMBER);
    EXPECT_EQ(t1.text, "10");
    EXPECT_EQ(comma.type, TokenType::TOKEN_COMMA);
    EXPECT_EQ(t2.type, TokenType::TOKEN_NUMBER);
}
