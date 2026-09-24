#include <gtest/gtest.h>
#include "Daad/CodeGen.hpp"
#include "Daad/AST.hpp"
#include <memory>
#include <string>

using namespace daad;

// اختبار توليد كود لعقدة NumberExprAST عبر متغير
TEST(CodeGenTest, NumberExpr) {
    CodeGenVisitor codegen;
    // صحيح س = 42 ؛
    auto init = std::make_unique<NumberExprAST>(42.0);
    auto var = std::make_unique<VarDeclStmtAST>("صحيح", "س", std::move(init));
    var->accept(codegen);
    codegen.endMainBody(); // ليس ضروري لكن للتأكد
    std::string src = codegen.getSourceCode();
    // يجب أن يحتوي على 42
    EXPECT_NE(src.find("42"), std::string::npos);
    // ويحتوي على int (ترجمة صحيح)
    EXPECT_NE(src.find("int"), std::string::npos);
}

// اختبار توليد كود لـ Switch مع حالة نصية (يختبر hasStringCases بدون strLit warning)
TEST(CodeGenTest, SwitchWithString) {
    CodeGenVisitor codegen;
    auto expr = std::make_unique<VariableExprAST>("س");
    SwitchCaseAST sc;
    sc.value = std::make_unique<StringExprAST>("مرحبا");
    sc.body.push_back(std::make_unique<PrintStmtAST>(ExprList{}));
    std::vector<SwitchCaseAST> cases;
    cases.push_back(std::move(sc));
    auto switchNode = std::make_unique<SwitchStmtAST>(std::move(expr), std::move(cases), StmtList{});
    switchNode->accept(codegen);
    std::string src = codegen.getSourceCode();
    // للنصوص يجب أن يولد if-else وليس switch
    EXPECT_NE(src.find("if"), std::string::npos);
    EXPECT_NE(src.find("مرحبا"), std::string::npos);
}

// اختبار توليد كود لـ Switch مع حالة رقمية (يولد switch)
TEST(CodeGenTest, SwitchWithNumber) {
    CodeGenVisitor codegen;
    auto expr = std::make_unique<VariableExprAST>("س");
    SwitchCaseAST sc;
    sc.value = std::make_unique<NumberExprAST>(1.0);
    sc.body.push_back(std::make_unique<PrintStmtAST>(ExprList{}));
    std::vector<SwitchCaseAST> cases;
    cases.push_back(std::move(sc));
    auto switchNode = std::make_unique<SwitchStmtAST>(std::move(expr), std::move(cases), StmtList{});
    switchNode->accept(codegen);
    std::string src = codegen.getSourceCode();
    EXPECT_NE(src.find("switch"), std::string::npos);
    EXPECT_NE(src.find("case"), std::string::npos);
}

// اختبار NullExprAST -> nullptr
TEST(CodeGenTest, NullExpr) {
    CodeGenVisitor codegen;
    // ارجع عدم ؛
    auto ret = std::make_unique<ReturnStmtAST>(std::make_unique<NullExprAST>());
    // نحتاج إلى وضعه داخل دالة أو main — نولده مباشرة عبر زيارة
    // Return يولد "return nullptr;"
    codegen.beginMainBody();
    ret->accept(codegen);
    codegen.endMainBody();
    std::string src = codegen.getSourceCode();
    EXPECT_NE(src.find("nullptr"), std::string::npos);
}

// اختبار ExportStmtAST -> // صدّر
TEST(CodeGenTest, ExportStmt) {
    CodeGenVisitor codegen;
    auto exp = std::make_unique<ExportStmtAST>(std::vector<std::string>{"دالة"});
    exp->accept(codegen);
    std::string src = codegen.getSourceCode();
    EXPECT_NE(src.find("صدّر"), std::string::npos);
}

// اختبار توليد هيكل
TEST(CodeGenTest, StructCodeGen) {
    CodeGenVisitor codegen;
    ParamList members;
    members.push_back({"صحيح", "س"});
    members.push_back({"صحيح", "ص"});
    auto str = std::make_unique<StructDeclAST>("نقطة", std::move(members));
    str->accept(codegen);
    std::string header = codegen.getHeaderCode();
    std::string src = codegen.getSourceCode();
    // يجب أن يكون في الهيدر والـ source
    EXPECT_NE(header.find("struct"), std::string::npos);
    EXPECT_NE(src.find("struct"), std::string::npos);
    EXPECT_NE(header.find("نقطة"), std::string::npos);
}
