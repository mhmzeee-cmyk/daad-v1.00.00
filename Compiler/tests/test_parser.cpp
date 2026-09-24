#include <gtest/gtest.h>
#include "Daad/Lexer.hpp"
#include "Daad/Parser.hpp"
#include "Daad/Diagnostics.hpp"
#include "Daad/AST.hpp"

using namespace daad;

// مساعد لإنشاء Parser من نص
static std::unique_ptr<StmtAST> parseOne(const std::string& src, DiagnosticsEngine& diag) {
    Lexer lexer(src);
    Parser parser(lexer, diag);
    return parser.parseStatement();
}

// اختبار إعلان متغير صحيح
TEST(ParserTest, VarDecl_Sahih) {
    DiagnosticsEngine diag;
    auto stmt = parseOne("صحيح س = 10 ؛", diag);
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
    auto* var = dynamic_cast<VarDeclStmtAST*>(stmt.get());
    ASSERT_NE(var, nullptr);
    EXPECT_EQ(var->type, "صحيح");
    EXPECT_EQ(var->name, "س");
    ASSERT_NE(var->init, nullptr);
    auto* num = dynamic_cast<NumberExprAST*>(var->init.get());
    ASSERT_NE(num, nullptr);
    EXPECT_DOUBLE_EQ(num->value, 10.0);
}

// اختبار عبارة إذا مع كتلة {}
TEST(ParserTest, IfStatement) {
    DiagnosticsEngine diag;
    Lexer lexer("إذا (س > 0) { طباعة(س) ؛ }");
    Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
    auto* ifStmt = dynamic_cast<IfStmtAST*>(stmt.get());
    ASSERT_NE(ifStmt, nullptr);
    EXPECT_NE(ifStmt->condition, nullptr);
    EXPECT_EQ(ifStmt->thenBody.size(), 1u);
    EXPECT_TRUE(ifStmt->elseBody.empty());
}

// اختبار حالة فشل: متغير بدون فاصلة منقوطة — يجب أن لا ينهار والتشخيص سليم
TEST(ParserTest, MissingSemicolon_Handled) {
    DiagnosticsEngine diag;
    auto stmt = parseOne("صحيح س = 10", diag);
    // الحالي يعتبر الفاصلة اختيارية، لذا لا خطأ لكن لا انهيار
    ASSERT_NE(stmt, nullptr);
    // لا يجب أن ينهار حتى بدون فاصلة
    auto* var = dynamic_cast<VarDeclStmtAST*>(stmt.get());
    ASSERT_NE(var, nullptr);
    EXPECT_EQ(var->name, "س");
}

// اختبار حالة فشل حقيقية: تعبير ناقص
TEST(ParserTest, InvalidExpression_MissingInit) {
    DiagnosticsEngine diag;
    auto stmt = parseOne("صحيح س = ؛", diag);
    // يجب أن يسجل خطأ "تعبير غير متوقع"
    EXPECT_TRUE(diag.hasErrors());
    // حتى مع الخطأ، يجب أن لا ينهار
    // قد يعيد nullptr أو VarDecl مع 0
}

// اختبار هيكل جديد (FIX-1)
TEST(ParserTest, StructDecl) {
    DiagnosticsEngine diag;
    Lexer lexer("هيكل نقطة { صحيح س ؛ صحيح ص ؛ }");
    Parser parser(lexer, diag);
    auto stmt = parser.parseStatement();
    ASSERT_NE(stmt, nullptr);
    EXPECT_FALSE(diag.hasErrors());
    auto* str = dynamic_cast<StructDeclAST*>(stmt.get());
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str->name, "نقطة");
    ASSERT_EQ(str->members.size(), 2u);
    EXPECT_EQ(str->members[0].first, "صحيح");
    EXPECT_EQ(str->members[0].second, "س");
    EXPECT_EQ(str->members[1].first, "صحيح");
    EXPECT_EQ(str->members[1].second, "ص");
}

// اختبار متغير من نوع هيكل معرف مسبقاً
TEST(ParserTest, StructVarDecl) {
    DiagnosticsEngine diag;
    Lexer lexer("هيكل نقطة { صحيح س ؛ } نقطة ن ؛");
    Parser parser(lexer, diag);
    auto s1 = parser.parseStatement(); // هيكل
    ASSERT_NE(s1, nullptr);
    EXPECT_FALSE(diag.hasErrors());
    auto s2 = parser.parseStatement(); // نقطة ن ؛
    ASSERT_NE(s2, nullptr);
    EXPECT_FALSE(diag.hasErrors());
    auto* var = dynamic_cast<VarDeclStmtAST*>(s2.get());
    ASSERT_NE(var, nullptr);
    EXPECT_EQ(var->type, "نقطة");
    EXPECT_EQ(var->name, "ن");
}
