/**
 * @file Optimizer.hpp
 * @brief مُحسن الكود لتقليل حجم وتكلفة التنفيذ
 */

#pragma once

#include "Daad/AST.hpp"

namespace daad {

class OptimizerVisitor : public ASTVisitor {
public:
    void visit(BinaryExprAST& node) override;
    void visit(UnaryExprAST& node) override;
    void visit(NumberExprAST&) override {}
    void visit(VariableExprAST&) override {}
    void visit(StringExprAST&) override {}
    void visit(VarDeclStmtAST& node) override;
    void visit(AssignmentAST& node) override;
    void visit(CompoundAssignmentAST& node) override;
    void visit(IfStmtAST& node) override;
    void visit(WhileStmtAST& node) override;
    void visit(ForStmtAST& node) override;
    void visit(DoWhileStmtAST& node) override;
    void visit(SwitchStmtAST& node) override;
    void visit(ReturnStmtAST& node) override;
    void visit(BreakStmtAST&) override {}
    void visit(ContinueStmtAST&) override {}
    void visit(FunctionDeclAST& node) override;
    void visit(FunctionCallAST&) override {}
    void visit(ClassDeclAST&) override {}
    void visit(StructDeclAST&) override {}
    void visit(NamespaceDeclAST& node) override;
    void visit(EnumDeclAST&) override {}
    void visit(TryCatchStmtAST& node) override;
    void visit(TemplateDeclAST& node) override;
    void visit(MemberAccessExprAST&) override {}
    void visit(MemberAssignmentAST& node) override;
    void visit(ArraySubscriptExprAST&) override {}
    void visit(ArraySubscriptAssignAST&) override {}
    void visit(ThrowStmtAST& node) override;
    void visit(NewExprAST&) override {}
    void visit(DeleteStmtAST&) override {}
    void visit(ExprStmtAST& node) override;
};

} // namespace daad
