/**
 * @file Parser.hpp
 * @brief المحلل النحوي للغة 'ض'
 * @author فريق ض استديو
 * @version 1.0.0
 */

#pragma once

#include "Daad/Lexer.hpp"
#include "Daad/AST.hpp"
#include "Daad/Diagnostics.hpp"
#include <vector>
#include <string>

namespace daad {

class Parser {
public:
    explicit Parser(Lexer& lexer, DiagnosticsEngine& diag) : m_lexer(lexer), m_diag(diag) { consume(); }

    std::unique_ptr<StmtAST> parseStatement();
    [[nodiscard]] bool isAtEnd() const noexcept { return m_currentToken.type == TokenType::TOKEN_EOF; }

private:
    Lexer& m_lexer;
    DiagnosticsEngine& m_diag;
    Token m_currentToken;

    void consume();
    void reportError(const std::string& message);

    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<ExprAST> parseBinaryExpr(int precedence);
    std::unique_ptr<ExprAST> parseUnary();
    std::unique_ptr<ExprAST> parsePrimary();
    std::unique_ptr<ExprAST> parseFunctionCallExpr(const std::string& name);
    int getCurrentPrecedence() const;

    std::unique_ptr<VarDeclStmtAST> parseVariableDeclaration();
    std::unique_ptr<StmtAST> parseAssignment(const std::string& name);
    std::unique_ptr<StmtAST> parsePrintStatement();
    std::unique_ptr<IfStmtAST> parseIfStatement();
    std::unique_ptr<WhileStmtAST> parseWhileStatement();
    std::unique_ptr<ForStmtAST> parseForStatement();
    std::unique_ptr<DoWhileStmtAST> parseDoWhileStatement();
    std::unique_ptr<SwitchStmtAST> parseSwitchStatement();
    std::unique_ptr<ReturnStmtAST> parseReturnStatement();
    std::unique_ptr<NamespaceDeclAST> parseNamespaceDeclaration();
    std::unique_ptr<EnumDeclAST> parseEnumDeclaration();
    std::unique_ptr<TryCatchStmtAST> parseTryCatchStatement();
    std::unique_ptr<TemplateDeclAST> parseTemplateDeclaration();
    std::unique_ptr<FunctionDeclAST> parseFunctionDefinition();
    std::unique_ptr<ClassDeclAST> parseClassDeclaration();
    std::unique_ptr<StructDeclAST> parseStructDeclaration();
    std::unique_ptr<ThrowStmtAST> parseThrowStatement();
    std::unique_ptr<NewExprAST> parseNewExpression();
    std::vector<std::unique_ptr<StmtAST>> parseBlock();
};

} // namespace daad