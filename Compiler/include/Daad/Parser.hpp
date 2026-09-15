#pragma once

#include "Daad/AST.hpp"
#include "Daad/Token.hpp"
#include "Daad/Lexer.hpp"
#include "Daad/Diagnostics.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <utility>
#include <memory>

namespace daad {

class Lexer;
class DiagnosticsEngine;

class Parser {
public:
    Parser(Lexer& lexer, DiagnosticsEngine& diag)
        : m_lexer(lexer), m_diag(diag) {
        consume(); // Load first token
    }

    std::unique_ptr<StmtAST> parseStatement();
    bool isAtEnd() const { return m_currentToken.type == TokenType::TOKEN_EOF; }

private:
    // Core helpers
    void consume();
    void reportError(const std::string& message);
    bool isNextLeftParen();
    StmtList parseBlock();

    // Expression parsing
    std::unique_ptr<ExprAST> parseBinaryExpr(int precedence);
    std::unique_ptr<ExprAST> parseBinaryExprFrom(std::unique_ptr<ExprAST> lhs, int precedence);
    std::unique_ptr<ExprAST> parseExpression();
    int getCurrentPrecedence() const;
    std::unique_ptr<ExprAST> parseUnary();
    std::unique_ptr<ExprAST> parsePrimary();
    std::unique_ptr<ExprAST> parseFunctionCallExpr(const std::string& name);

    // Statement parsing
    std::unique_ptr<StmtAST> parseVariableDeclaration();
    std::unique_ptr<StmtAST> parseAssignment(const std::string& name);
    std::unique_ptr<StmtAST> parsePrintStatement();
    std::unique_ptr<StmtAST> parseInputStatement();
    std::unique_ptr<IfStmtAST> parseIfStatement();
    std::unique_ptr<WhileStmtAST> parseWhileStatement();
    std::unique_ptr<StmtAST> parseForEachOrForStatement();
    std::unique_ptr<DoWhileStmtAST> parseDoWhileStatement();
    std::unique_ptr<SwitchStmtAST> parseSwitchStatement();
    std::unique_ptr<ReturnStmtAST> parseReturnStatement();
    std::unique_ptr<NamespaceDeclAST> parseNamespaceDeclaration();
    std::unique_ptr<EnumDeclAST> parseEnumDeclaration();
    std::unique_ptr<TryCatchStmtAST> parseTryCatchStatement();
    std::unique_ptr<StmtAST> parseTemplateDeclaration();
    std::unique_ptr<FunctionDeclAST> parseFunctionDefinition();
    std::unique_ptr<ClassDeclAST> parseClassDeclaration(bool isAbstract = false);
    std::unique_ptr<ForEachStmtAST> parseForEachStatement();
    std::unique_ptr<ThrowStmtAST> parseThrowStatement();
    std::unique_ptr<DeleteStmtAST> parseDeleteStatement();
    std::unique_ptr<ImportStmtAST> parseImportStatement();
    std::unique_ptr<ExportStmtAST> parseExportStatement();

    // Image processing parsers
    std::unique_ptr<StmtAST> parseLoadImage();
    std::unique_ptr<StmtAST> parseDrawImage();
    std::unique_ptr<StmtAST> parseImageSize();
    std::unique_ptr<StmtAST> parseSaveImage();
    std::unique_ptr<StmtAST> parseCropImage();
    std::unique_ptr<StmtAST> parseResize();
    std::unique_ptr<StmtAST> parseRotateImage();
    std::unique_ptr<StmtAST> parseFlipImage();
    std::unique_ptr<StmtAST> parseOpacity();
    std::unique_ptr<StmtAST> parseFilter();
    std::unique_ptr<StmtAST> parseOverlay();
    std::unique_ptr<StmtAST> parseBackground();
    std::unique_ptr<StmtAST> parsePixel();
    std::unique_ptr<StmtAST> parseDraw();
    std::unique_ptr<StmtAST> parseFill();
    std::unique_ptr<StmtAST> parseRectangle();
    std::unique_ptr<StmtAST> parseCircle();
    std::unique_ptr<StmtAST> parseLine();
    std::unique_ptr<StmtAST> parseTextOnCanvas();
    std::unique_ptr<StmtAST> parseClear();

    // Helpers
    bool isKnownTypeName(const std::string& text) const;
    std::string parseTypeName();
    std::pair<std::string, std::string> parseParam();
    void skipUntilMemberEnd();

    // Keyword type checking — accepts both TOKEN_KEYWORD and all TOKEN_KW_* types
    static bool isAnyKeyword(TokenType type);

    Lexer& m_lexer;
    DiagnosticsEngine& m_diag;
    Token m_currentToken;
    std::unordered_set<std::string> m_knownClassNames;
};

} // namespace daad
