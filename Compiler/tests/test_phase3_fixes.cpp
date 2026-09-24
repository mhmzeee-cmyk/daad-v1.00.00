#include <gtest/gtest.h>
#include "Daad/Lexer.hpp"
#include "Daad/Parser.hpp"
#include "Daad/Diagnostics.hpp"
#include "Daad/AST.hpp"
#include "Daad/CodeGen.hpp"
#include "Daad/Optimizer.hpp"

using namespace daad;

// Task 1.1: Struct Parsing — يتحقق أن هيكل نقطة يحتوي members بحجم 2
TEST(Phase3Fixes, StructParsing_Members) {
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

// Task 1.1b: Struct CodeGen — يتحقق من توليد struct صحيح في الهيدر
TEST(Phase3Fixes, StructCodeGen_Header) {
    CodeGenVisitor codegen;
    ParamList members;
    members.push_back({"صحيح", "س"});
    members.push_back({"صحيح", "ص"});
    auto str = std::make_unique<StructDeclAST>("نقطة", std::move(members));
    str->accept(codegen);
    std::string header = codegen.getHeaderCode();
    std::string src = codegen.getSourceCode();
    EXPECT_NE(header.find("struct"), std::string::npos);
    EXPECT_NE(header.find("نقطة"), std::string::npos);
    EXPECT_NE(header.find("int"), std::string::npos);
    EXPECT_NE(header.find("س"), std::string::npos);
    EXPECT_NE(header.find("ص"), std::string::npos);
    // يجب أن يكون في الهيدر والـ source (لأن main يحتاج رؤيته)
    EXPECT_NE(src.find("struct"), std::string::npos);
}

// Task 1.2: Finally RAII — يتحقق أن finally يولد guard وليس كتلة عادية
TEST(Phase3Fixes, FinallyRAII_Generation) {
    CodeGenVisitor codegen;
    // حاول { طباعة("try")؛ } أخيراً { طباعة("finally")؛ }
    StmtList tryBody;
    {
        ExprList args;
        args.push_back(std::make_unique<StringExprAST>("try"));
        tryBody.push_back(std::make_unique<PrintStmtAST>(std::move(args)));
    }
    StmtList finallyBody;
    {
        ExprList args;
        args.push_back(std::make_unique<StringExprAST>("finally"));
        finallyBody.push_back(std::make_unique<PrintStmtAST>(std::move(args)));
    }
    auto node = std::make_unique<TryCatchStmtAST>(std::move(tryBody), "", StmtList{}, std::move(finallyBody));
    node->accept(codegen);
    std::string src = codegen.getSourceCode();
    // يجب أن يحتوي على نمط RAII guard وليس فقط "// أخيراً"
    EXPECT_NE(src.find("_daad_finally"), std::string::npos);
    EXPECT_NE(src.find("_DaadFinallyGuard"), std::string::npos);
    EXPECT_NE(src.find("~_DaadFinallyGuard"), std::string::npos);
    // يجب أن لا يكون مجرد كتلة عادية منفصلة بدون guard
    // التأكد من وجود try و catch(...) المضاف
    EXPECT_NE(src.find("try"), std::string::npos);
}

// Task 1.3: Optimizer — القسمة على صفر لا تُطوى
TEST(Phase3Fixes, OptimizerDivByZero_NotFolded) {
    OptimizerVisitor opt;
    auto lhs = std::make_unique<NumberExprAST>(10.0);
    auto rhs = std::make_unique<NumberExprAST>(0.0);
    BinaryExprAST node("/", std::move(lhs), std::move(rhs));
    node.accept(opt);
    EXPECT_FALSE(node.isFolded);
}

// Task 1.3b: Optimizer — باقي القسمة على صفر لا يُطوى
TEST(Phase3Fixes, OptimizerModByZero_NotFolded) {
    OptimizerVisitor opt;
    auto lhs = std::make_unique<NumberExprAST>(10.0);
    auto rhs = std::make_unique<NumberExprAST>(0.0);
    BinaryExprAST node("%", std::move(lhs), std::move(rhs));
    node.accept(opt);
    EXPECT_FALSE(node.isFolded);
}

// Task 1.3c: Optimizer — القسمة العادية لا تزال تُطوى
TEST(Phase3Fixes, OptimizerDivNormal_Folded) {
    OptimizerVisitor opt;
    auto lhs = std::make_unique<NumberExprAST>(10.0);
    auto rhs = std::make_unique<NumberExprAST>(2.0);
    BinaryExprAST node("/", std::move(lhs), std::move(rhs));
    node.accept(opt);
    EXPECT_TRUE(node.isFolded);
    EXPECT_DOUBLE_EQ(node.foldedValue, 5.0);
}
