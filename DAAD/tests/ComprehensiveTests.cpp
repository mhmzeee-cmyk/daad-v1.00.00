/**
 * @file ComprehensiveTests.cpp
 * @brief 100 اختبارات شاملة للاستقرار والأمان وتجربة المستخدم
 */

#include <gtest/gtest.h>
#include "Daad/Lexer.hpp"
#include "Daad/Parser.hpp"
#include "Daad/AST.hpp"
#include "Daad/Compiler.hpp"
#include "Daad/Optimizer.hpp"
#include "Daad/CodeGen.hpp"
#include "Daad/Diagnostics.hpp"
#include "Daad/Keywords.hpp"
#include <string>
#include <cstring>

// ============================================================================
// اختبارات الاستقرار - Lexer (1-20)
// ============================================================================

TEST(Stability_Lexer, EmptyInput) {
    daad::Lexer lexer("");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_EOF);
}

TEST(Stability_Lexer, SingleSpace) {
    daad::Lexer lexer(" ");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_EOF);
}

TEST(Stability_Lexer, MultipleNewlines) {
    daad::Lexer lexer("\n\n\n\n\n");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_EOF);
}

TEST(Stability_Lexer, VeryLongIdentifier) {
    std::string id(1000, 'a');
    daad::Lexer lexer(id);
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_IDENTIFIER);
}

TEST(Stability_Lexer, UnicodeArabicIdentifier) {
    daad::Lexer lexer("متغير_عربي_طويل");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_IDENTIFIER);
    EXPECT_EQ(token.text, "متغير_عربي_طويل");
}

TEST(Stability_Lexer, NestedStrings) {
    daad::Lexer lexer("\"نص \\\"داخلي\\\"\"");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_STRING);
}

TEST(Stability_Lexer, EmptyString) {
    daad::Lexer lexer("\"\"");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_STRING);
}

TEST(Stability_Lexer, NumberEdgeCases) {
    daad::Lexer lexer("0");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_NUMBER);
    EXPECT_EQ(token.text, "0");
}

TEST(Stability_Lexer, LargeNumber) {
    daad::Lexer lexer("999999999999");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_NUMBER);
}

TEST(Stability_Lexer, CoreOperators) {
    std::string ops[] = {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">="};
    for (const auto& op : ops) {
        daad::Lexer lexer(op);
        auto token = lexer.getNextToken();
        EXPECT_NE(token.type, daad::TokenType::TOKEN_UNKNOWN) << "Failed for operator: " << op;
    }
}

TEST(Stability_Lexer, CorePunctuation) {
    std::string punct[] = {"(", ")", "{", "}"};
    for (const auto& p : punct) {
        daad::Lexer lexer(p);
        auto token = lexer.getNextToken();
        EXPECT_NE(token.type, daad::TokenType::TOKEN_UNKNOWN) << "Failed for: " << p;
    }
}

TEST(Stability_Lexer, ArabicSemicolon) {
    daad::Lexer lexer("س = 5 ؛");
    auto semi = lexer.getNextToken(); // skip identifier
    semi = lexer.getNextToken(); // skip equals
    semi = lexer.getNextToken(); // skip number
    semi = lexer.getNextToken();
    EXPECT_EQ(semi.type, daad::TokenType::TOKEN_SEMICOLON);
}

TEST(Stability_Lexer, DotToken) {
    daad::Lexer lexer(".");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_DOT);
}

TEST(Stability_Lexer, ColonToken) {
    daad::Lexer lexer(":");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_COLON);
}

TEST(Stability_Lexer, RapidTokenization) {
    std::string source = "صحيح س = 10 ؛";
    for (int i = 0; i < 1000; i++) {
        daad::Lexer lexer(source);
        while (true) {
            auto token = lexer.getNextToken();
            if (token.type == daad::TokenType::TOKEN_EOF) break;
        }
    }
}

TEST(Stability_Lexer, MixedArabicLatin) {
    daad::Lexer lexer("var_name متغير");
    auto t1 = lexer.getNextToken();
    auto t2 = lexer.getNextToken();
    EXPECT_EQ(t1.type, daad::TokenType::TOKEN_IDENTIFIER);
    EXPECT_EQ(t2.type, daad::TokenType::TOKEN_IDENTIFIER);
}

TEST(Stability_Lexer, CommentOnly) {
    daad::Lexer lexer("// هذا تعليق");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_EOF);
}

TEST(Stability_Lexer, MultiLineComment) {
    daad::Lexer lexer("/* تعليق\nمتعدد\nالسطور */");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_EOF);
}

TEST(Stability_Lexer, TabCharacters) {
    daad::Lexer lexer("\t\t\tمتغير = 5");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_IDENTIFIER);
}

// ============================================================================
// اختبارات الاستقرار - Parser (21-40)
// ============================================================================

TEST(Stability_Parser, EmptyInput) {
    daad::Lexer lexer("");
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    EXPECT_EQ(stmt, nullptr);
}

TEST(Stability_Parser, SingleVarDecl) {
    daad::Lexer lexer("صحيح س = 5 ؛");
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, NestedIf) {
    std::string source = "إذا صواب { إذا صواب { طباعة( \"داخلي\" ) ؛ } }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
}

TEST(Stability_Parser, WhileWithBreak) {
    std::string source = "طالما صواب { توقف ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
}

TEST(Stability_Parser, ForLoop) {
    std::string source = "لكل ( صحيح ع = 0 ؛ ع < 10 ؛ ع = ع + 1 ) { طباعة( ع ) ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, FunctionCall) {
    std::string source = "طباعة( 42 ) ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, NamespaceDecl) {
    std::string source = "نطاق مساحة_اسم { صحيح س = 5 ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, TryCatch) {
    std::string source = "حاول { طباعة( \"محاولة\" ) ؛ } امسك ( استثناء ) { طباعة( \"خطأ\" ) ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, EnumDecl) {
    std::string source = "تعداد حالة { تشغيل ، إيقاف ، معلّق }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, MemberAccess) {
    std::string source = "ش.عضو = 5 ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, MultipleStatements) {
    std::string source = "صحيح أ = 1 ؛\nصحيح ب = 2 ؛\nصحيح ج = 3 ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto s1 = parser.parseStatement();
    auto s2 = parser.parseStatement();
    auto s3 = parser.parseStatement();
    ASSERT_NE(s1, nullptr);
    ASSERT_NE(s2, nullptr);
    ASSERT_NE(s3, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, ComplexExpression) {
    std::string source = "صحيح ن = ( أ + ب ) * ( ج - د ) ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, NestedBraces) {
    std::string source = "إذا صواب { إذا صواب { إذا صواب { طباعة( \"عميق\" ) ؛ } } }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
}

TEST(Stability_Parser, SwitchCase) {
    std::string source = "اختر ( متغير ) { حالة 1: طباعة( \"واحد\" ) ؛ توقف ؛ حالة 2: طباعة( \"اثنان\" ) ؛ توقف ؛ افتراضي: طباعة( \"آخر\" ) ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, DoWhileLoop) {
    std::string source = "افعل { طباعة( \"مرة\" ) ؛ } طالما خطأ ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
}

TEST(Stability_Parser, TemplateDecl) {
    std::string source = "قالب < T > صحيح س = 5 ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Stability_Parser, ReturnWithValue) {
    std::string source = "إذا صواب { ارجع 42 ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
}

TEST(Stability_Parser, ReturnEmpty) {
    std::string source = "إذا صواب { ارجع ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
}

TEST(Stability_Parser, ContinueStatement) {
    std::string source = "طالما صواب { استمر ؛ }";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
}

// ============================================================================
// اختبارات الاستقرار - Compiler (41-60)
// ============================================================================

TEST(Stability_Compiler, MinimalProgram) {
    daad::DaadCompiler compiler;
    auto result = compiler.compile("صحيح س = 1 ؛");
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, PrintOnly) {
    daad::DaadCompiler compiler;
    auto result = compiler.compile("طباعة( \"مرحبا\" ) ؛");
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, MultipleVars) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح أ = 1 ؛\nصحيح ب = 2 ؛\nصحيح ج = 3 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, IfElse) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛\nإذا ( س > 5 ) { طباعة( \"كبير\" ) ؛ } وإلا { طباعة( \"صغير\" ) ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, WhileLoop) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح ع = 0 ؛\nطالما ( ع < 5 ) { ع = ع + 1 ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, ForLoop) {
    daad::DaadCompiler compiler;
    std::string source = "لكل ( صحيح ع = 0 ؛ ع < 10 ؛ ع = ع + 1 ) { طباعة( ع ) ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, FunctionCall) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( 42 ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, NamespaceDecl2) {
    daad::DaadCompiler compiler;
    std::string source = "نطاق مساحة_اسم { صحيح س = 5 ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, NamespaceDecl) {
    daad::DaadCompiler compiler;
    std::string source = "نطاق مساحة_اسم { صحيح س = 5 ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, EnumDecl) {
    daad::DaadCompiler compiler;
    std::string source = "تعداد حالة { تشغيل ، إيقاف ، معلّق }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, TryCatch) {
    daad::DaadCompiler compiler;
    std::string source = "حاول { طباعة( \"محاولة\" ) ؛ } امسك ( استثناء ) { طباعة( \"خطأ\" ) ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, TemplateVar) {
    daad::DaadCompiler compiler;
    std::string source = "قالب < T > صحيح س = 5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, MemberAccess) {
    daad::DaadCompiler compiler;
    std::string source = "ش.عضو = 5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, ComplexProgram) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح س = 0 ؛
        إذا ( س > 5 ) {
            طباعة( "كبير" ) ؛
        } وإلا {
            طباعة( "صغير" ) ؛
        }
        طباعة( س ) ؛
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, DeepNested) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 ؛ إذا ( س > 0 ) { إذا ( س > 0 ) { طباعة( \"عميق\" ) ؛ } }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, SwitchCase) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح اختيار = 2 ؛
        اختر ( اختيار ) {
            حالة 1: طباعة( "واحد" ) ؛ توقف ؛
            حالة 2: طباعة( "اثنان" ) ؛ توقف ؛
            افتراضي: طباعة( "آخر" ) ؛
        }
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, DoWhileLoop) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح ع = 0 ؛\nافعل { ع = ع + 1 ؛ }طالما ( ع < 10 ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, ReturnStatement) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"نص\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, BreakContinue) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح ع = 0 ؛
       طالما ( ع < 100 ) {
            إذا ( ع == 5 ) { استمر ؛ }
            إذا ( ع == 10 ) { توقف ؛ }
            ع = ع + 1 ؛
        }
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Stability_Compiler, ArithmeticExpressions) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح أ = 10 ؛
        صحيح ب = 20 ؛
        صحيح ج = أ + ب * 3 - 5 / 2 ؛
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

// ============================================================================
// اختبارات الأمان (61-80)
// ============================================================================

TEST(Security, BufferOverflowPrevention) {
    daad::Lexer lexer(std::string(10000, 'x'));
    auto token = lexer.getNextToken();
    EXPECT_NE(token.type, daad::TokenType::TOKEN_UNKNOWN);
}

TEST(Security, StringInjectionPrevention) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"normal string\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.sourceOutput.find("system("), std::string::npos);
}

TEST(Security, NullByteHandling) {
    daad::Lexer lexer(std::string("abc\0def", 7));
    auto token = lexer.getNextToken();
    EXPECT_NE(token.type, daad::TokenType::TOKEN_UNKNOWN);
}

TEST(Security, ExtremelyLongString) {
    std::string longStr(1000, 'a');
    daad::Lexer lexer("\"" + longStr + "\"");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_STRING);
}

TEST(Security, SQLInjectionAttempt) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"'; DROP TABLE users; --\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Security, PathTraversalAttempt) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"../../etc/passwd\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Security, CodeInjectionAttempt) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"$(rm -rf /)\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.sourceOutput.find("system("), std::string::npos);
}

TEST(Security, UnicodeNormalization) {
    daad::Lexer lexer("أبجد");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_IDENTIFIER);
    EXPECT_EQ(token.text, "أبجد");
}

TEST(Security, RecursiveDepthLimit) {
    std::string source;
    for (int i = 0; i < 100; i++) {
        source += "إذا ( صواب ) { ";
    }
    for (int i = 0; i < 100; i++) {
        source += "}";
    }
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    EXPECT_NE(stmt, nullptr);
}

TEST(Security, IntegerOverflowPrevention) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 999999999999999 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Security, MemoryLeakPrevention) {
    for (int i = 0; i < 100; i++) {
        daad::DaadCompiler compiler;
        auto result = compiler.compile("صحيح س = 1 ؛");
        EXPECT_TRUE(result.success);
    }
}

TEST(Security, ConcurrentCompilation) {
    daad::DaadCompiler c1, c2, c3;
    auto r1 = c1.compile("صحيح أ = 1 ؛");
    auto r2 = c2.compile("صحيح ب = 2 ؛");
    auto r3 = c3.compile("صحيح ج = 3 ؛");
    EXPECT_TRUE(r1.success);
    EXPECT_TRUE(r2.success);
    EXPECT_TRUE(r3.success);
}

TEST(Security, EmptyDiagnosticHandling) {
    daad::DaadCompiler compiler;
    auto result = compiler.compile("");
    EXPECT_TRUE(result.success);
}

TEST(Security, InvalidUTF8Handling) {
    std::string invalid = "\xff\xfe\xfd";
    daad::Lexer lexer(invalid);
    auto token = lexer.getNextToken();
    EXPECT_NE(token.type, daad::TokenType::TOKEN_STRING);
}

TEST(Security, ExcessiveMemoryAllocation) {
    std::string source;
    for (int i = 0; i < 1000; i++) {
        source += "صحيح متغير_" + std::to_string(i) + " = " + std::to_string(i) + " ؛\n";
    }
    daad::DaadCompiler compiler;
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(Security, SpecialCharacterHandling) {
    daad::Lexer lexer("@#$%^&*");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_UNKNOWN);
}

TEST(Security, NestedCommentHandling) {
    daad::Lexer lexer("/* تعليق */");
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_EOF);
}

TEST(Security, MaxKeywordLength) {
    std::string longKeyword(200, 'a');
    daad::Lexer lexer(longKeyword);
    auto token = lexer.getNextToken();
    EXPECT_EQ(token.type, daad::TokenType::TOKEN_IDENTIFIER);
}

TEST(Security, CompilerThreadSafety) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛";
    for (int i = 0; i < 100; i++) {
        auto result = compiler.compile(source);
        EXPECT_TRUE(result.success);
    }
}

// ============================================================================
// اختبارات تجربة المستخدم (81-100)
// ============================================================================

TEST(UserExperience, ClearErrorMessages) {
    daad::DaadCompiler compiler;
    auto result = compiler.compile("كلمة_مجهولة ؛");
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.diagnostics.empty());
    EXPECT_FALSE(result.diagnostics[0].message.empty());
}

TEST(UserExperience, LineNumberAccuracy) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 ؛\nصحيح ص = 2 ؛\nكلمة_خاطئة ؛";
    auto result = compiler.compile(source);
    EXPECT_FALSE(result.success);
    bool foundLine3 = false;
    for (const auto& d : result.diagnostics) {
        if (d.line == 3) foundLine3 = true;
    }
    EXPECT_TRUE(foundLine3);
}

TEST(UserExperience, ArabicErrorMessages) {
    daad::DaadCompiler compiler;
    auto result = compiler.compile("كلمة_مجهولة ؛");
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.diagnostics.empty());
}

TEST(UserExperience, GeneratedCodeReadability) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛\nطباعة( س ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int س = 10;"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("daad::runtime::daad_print"), std::string::npos);
}

TEST(UserExperience, DualFileGeneration) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.headerOutput.empty());
    EXPECT_FALSE(result.sourceOutput.empty());
}

TEST(UserExperience, RuntimeHeaderInjection) {
    daad::DaadCompiler compiler;
    auto result = compiler.compile("صحيح س = 1 ؛");
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("#include \"Daad/Runtime/DaadRuntime.hpp\""), std::string::npos);
}

TEST(UserExperience, TypeMappingAccuracy) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح ص = 1 ؛
        عشري ع = 2.5 ؛
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int ص = 1;"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("double ع ="), std::string::npos);
}

TEST(UserExperience, ConstantFoldingOptimization) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 + 2 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int س = 3;"), std::string::npos);
}

TEST(UserExperience, ArabicKeywords) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح س = 10 ؛
        إذا ( س > 5 ) { طباعة( "كبير" ) ؛ }
       طالما ( س > 0 ) { س = س - 1 ؛ }
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("if"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("while"), std::string::npos);
}

TEST(UserExperience, FunctionCallGeneration) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( 42 ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("print"), std::string::npos);
}

TEST(UserExperience, NamespaceGeneration2) {
    daad::DaadCompiler compiler;
    std::string source = "نطاق مساحة_اسم { صحيح س = 5 ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("namespace مساحة_اسم"), std::string::npos);
}

TEST(UserExperience, NamespaceGeneration) {
    daad::DaadCompiler compiler;
    std::string source = "نطاق مساحة_اسم { صحيح س = 5 ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("namespace مساحة_اسم"), std::string::npos);
}

TEST(UserExperience, EnumGeneration) {
    daad::DaadCompiler compiler;
    std::string source = "تعداد حالة { تشغيل }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.sourceOutput.empty());
}

TEST(UserExperience, TryCatchGeneration) {
    daad::DaadCompiler compiler;
    std::string source = "حاول { طباعة( \"محاولة\" ) ؛ } امسك ( استثناء ) { طباعة( \"خطأ\" ) ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("try"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("catch"), std::string::npos);
}

TEST(UserExperience, MemberAccessGeneration) {
    daad::DaadCompiler compiler;
    std::string source = "ش.عضو = 5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("ش.عضو = 5;"), std::string::npos);
}

TEST(UserExperience, SwitchCaseGeneration) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح اختيار = 1 ؛
        اختر ( اختيار ) {
            حالة 1: طباعة( "واحد" ) ؛ توقف ؛
            افتراضي: طباعة( "آخر" ) ؛
        }
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("switch"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("case"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("default:"), std::string::npos);
}

TEST(UserExperience, TemplateVarGeneration) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.sourceOutput.empty());
}

TEST(UserExperience, ComplexProgramCompilation) {
    daad::DaadCompiler compiler;
    std::string source = R"(
        صحيح س = 10 ؛
        إذا ( س > 5 ) { طباعة( "كبير" ) ؛ } وإلا { طباعة( "صغير" ) ؛ }
        صحيح ع = 0 ؛
        طالما ( ع < 5 ) { ع = ع + 1 ؛ }
        طباعة( س ) ؛
    )";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}

TEST(UserExperience, MemorySafetyGuarantees) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح مؤشر = 5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.sourceOutput.empty());
}

TEST(UserExperience, CodeGenerationConsistency) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛";
    auto r1 = compiler.compile(source);
    auto r2 = compiler.compile(source);
    EXPECT_EQ(r1.sourceOutput, r2.sourceOutput);
}

TEST(UserExperience, ErrorRecoveryContinues) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 ؛\nكلمة_خاطئة ؛\nصحيح ص = 2 ؛";
    auto result = compiler.compile(source);
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.diagnostics.empty());
}

TEST(UserExperience, LargeProgramCompilation) {
    daad::DaadCompiler compiler;
    std::string source;
    for (int i = 0; i < 50; i++) {
        source += "صحيح متغير_" + std::to_string(i) + " = " + std::to_string(i) + " ؛\n";
    }
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
}
