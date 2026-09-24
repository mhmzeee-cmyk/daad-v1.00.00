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
#include <unordered_set>

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
    std::unordered_set<std::string> m_knownClassNames; // Track class names as types

    void consume();
    void reportError(const std::string& message);
    // تطلع للأمام: هل الرمز التالي '(' (بدون استهلاك)؟
    bool isNextLeftParen();

    std::unique_ptr<ExprAST> parseExpression();
    std::unique_ptr<ExprAST> parseBinaryExpr(int precedence);
    std::unique_ptr<ExprAST> parseBinaryExprFrom(std::unique_ptr<ExprAST> lhs, int precedence);
    std::unique_ptr<ExprAST> parseUnary();
    std::unique_ptr<ExprAST> parsePrimary();
    std::unique_ptr<ExprAST> parseFunctionCallExpr(const std::string& name);
    int getCurrentPrecedence() const;

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
    std::unique_ptr<ClassDeclAST> parseClassDeclaration();
    // قراءة معامل دالة (يدعم الترتيبين: النوع الاسم و الاسم النوع مع أنواع المصفوفات)
    std::pair<std::string, std::string> parseParam();
    // قراءة اسم نوع (مع دعم لاحقات المصفوفات [])
    std::string parseTypeName();
    // هل النص اسم نوع معروف؟
    bool isKnownTypeName(const std::string& text) const;
    // تخطي الرموز حتى نهاية تعريف العضو (؛ أو } أو نهاية الملف) بعد خطأ
    void skipUntilMemberEnd();
    std::unique_ptr<ForEachStmtAST> parseForEachStatement();
    std::unique_ptr<ThrowStmtAST> parseThrowStatement();
    std::unique_ptr<DeleteStmtAST> parseDeleteStatement();
    std::unique_ptr<ImportStmtAST> parseImportStatement();
    std::unique_ptr<ExportStmtAST> parseExportStatement();
    std::vector<std::unique_ptr<StmtAST>> parseBlock();

    // Image Processing Parsers (20) — يتوافق مع Web
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
};

} // namespace daad