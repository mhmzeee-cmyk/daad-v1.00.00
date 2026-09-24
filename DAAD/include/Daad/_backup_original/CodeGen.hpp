/**
 * @file CodeGen.hpp
 * @brief مولد كود C++ من شجرة النحو
 * @author فريق ض استديو
 */

#pragma once

#include "Daad/AST.hpp"
#include <sstream>
#include <unordered_map>
#include <string>

namespace daad {

class CodeGenVisitor : public ASTVisitor {
public:
    CodeGenVisitor();

    // استرجاع الكود المولد لكل ملف
    std::string getHeaderCode() const { return m_headerStream.str(); }
    std::string getSourceCode() const { return m_sourceStream.str(); }

    void visit(NumberExprAST& node) override;
    void visit(VariableExprAST& node) override;
    void visit(StringExprAST& node) override;
    void visit(BinaryExprAST& node) override;
    void visit(UnaryExprAST& node) override;
    void visit(VarDeclStmtAST& node) override;
    void visit(AssignmentAST& node) override;
    void visit(CompoundAssignmentAST& node) override;
    void visit(IfStmtAST& node) override;
    void visit(WhileStmtAST& node) override;
    void visit(ForStmtAST& node) override;
    void visit(DoWhileStmtAST& node) override;
    void visit(SwitchStmtAST& node) override;
    void visit(ReturnStmtAST& node) override;
    void visit(BreakStmtAST& node) override;
    void visit(ContinueStmtAST& node) override;
    void visit(FunctionDeclAST& node) override;
    void visit(FunctionCallAST& node) override;
    void visit(ClassDeclAST& node) override;
    void visit(StructDeclAST& node) override;
    void visit(NamespaceDeclAST& node) override;
    void visit(EnumDeclAST& node) override;
    void visit(TryCatchStmtAST& node) override;
    void visit(TemplateDeclAST& node) override;
    void visit(MemberAccessExprAST& node) override;
    void visit(MemberAssignmentAST& node) override;
    void visit(ArraySubscriptExprAST& node) override;
    void visit(ArraySubscriptAssignAST& node) override;
    void visit(ThrowStmtAST& node) override;
    void visit(NewExprAST& node) override;
    void visit(DeleteStmtAST& node) override;
    void visit(ExprStmtAST& node) override;

    std::string getLastExpr() const { return m_lastExpr; }

private:
    std::ostringstream m_headerStream;
    std::ostringstream m_sourceStream;
    std::unordered_map<std::string, std::string> m_typeMap;
    std::string m_lastExpr;
    bool m_inForContext = false;
};

} // namespace daad
