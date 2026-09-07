#include <gtest/gtest.h>
#include "Daad/AST.hpp"
#include "Daad/Optimizer.hpp"
#include "Daad/CodeGen.hpp"

TEST(OptimizerTest, ConstantFoldingAddition) {
    auto lhs = std::make_unique<daad::NumberExprAST>(1.0);
    auto rhs = std::make_unique<daad::NumberExprAST>(2.0);
    auto binaryExpr = std::make_unique<daad::BinaryExprAST>("+", std::move(lhs), std::move(rhs));

    daad::OptimizerVisitor optimizer;
    binaryExpr->accept(optimizer);

    EXPECT_TRUE(binaryExpr->isFolded);
    EXPECT_DOUBLE_EQ(binaryExpr->foldedValue, 3.0);
}

TEST(OptimizerTest, ConstantFoldingMultiplication) {
    auto lhs = std::make_unique<daad::NumberExprAST>(4.0);
    auto rhs = std::make_unique<daad::NumberExprAST>(5.0);
    auto binaryExpr = std::make_unique<daad::BinaryExprAST>("*", std::move(lhs), std::move(rhs));

    daad::OptimizerVisitor optimizer;
    binaryExpr->accept(optimizer);

    EXPECT_TRUE(binaryExpr->isFolded);
    EXPECT_DOUBLE_EQ(binaryExpr->foldedValue, 20.0);
}

TEST(OptimizerTest, ConstantFoldingSubtraction) {
    auto lhs = std::make_unique<daad::NumberExprAST>(10.0);
    auto rhs = std::make_unique<daad::NumberExprAST>(3.0);
    auto binaryExpr = std::make_unique<daad::BinaryExprAST>("-", std::move(lhs), std::move(rhs));

    daad::OptimizerVisitor optimizer;
    binaryExpr->accept(optimizer);

    EXPECT_TRUE(binaryExpr->isFolded);
    EXPECT_DOUBLE_EQ(binaryExpr->foldedValue, 7.0);
}

TEST(OptimizerTest, ConstantFoldingDivision) {
    auto lhs = std::make_unique<daad::NumberExprAST>(20.0);
    auto rhs = std::make_unique<daad::NumberExprAST>(4.0);
    auto binaryExpr = std::make_unique<daad::BinaryExprAST>("/", std::move(lhs), std::move(rhs));

    daad::OptimizerVisitor optimizer;
    binaryExpr->accept(optimizer);

    EXPECT_TRUE(binaryExpr->isFolded);
    EXPECT_DOUBLE_EQ(binaryExpr->foldedValue, 5.0);
}

TEST(OptimizerTest, NoFoldingForVariables) {
    auto lhs = std::make_unique<daad::VariableExprAST>("س");
    auto rhs = std::make_unique<daad::NumberExprAST>(2.0);
    auto binaryExpr = std::make_unique<daad::BinaryExprAST>("+", std::move(lhs), std::move(rhs));

    daad::OptimizerVisitor optimizer;
    binaryExpr->accept(optimizer);

    EXPECT_FALSE(binaryExpr->isFolded);
}

TEST(OptimizerTest, FoldedCodeGeneration) {
    auto lhs = std::make_unique<daad::NumberExprAST>(3.0);
    auto rhs = std::make_unique<daad::NumberExprAST>(7.0);
    auto binaryExpr = std::make_unique<daad::BinaryExprAST>("+", std::move(lhs), std::move(rhs));

    daad::OptimizerVisitor optimizer;
    binaryExpr->accept(optimizer);

    daad::CodeGenVisitor codegen;
    binaryExpr->accept(codegen);

    EXPECT_EQ(codegen.getLastExpr(), "10");
}

TEST(OptimizerTest, TemplateOptimization) {
    auto body = std::make_unique<daad::VarDeclStmtAST>("صحيح", "س", std::make_unique<daad::NumberExprAST>(5.0));
    auto tpl = std::make_unique<daad::TemplateDeclAST>("مكعب", std::vector<std::string>{"T"}, std::move(body));

    daad::OptimizerVisitor optimizer;
    tpl->accept(optimizer);

    EXPECT_TRUE(tpl->body != nullptr);
}