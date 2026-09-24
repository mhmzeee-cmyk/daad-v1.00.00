#include <gtest/gtest.h>
#include "Daad/Lexer.hpp"
#include "Daad/Parser.hpp"
#include "Daad/AST.hpp"
#include "Daad/Diagnostics.hpp"

class ASTVerifier : public daad::ASTVisitor {
public:
    bool visitedVarDecl = false;
    bool visitedAssignment = false;
    bool visitedIf = false;
    bool visitedWhile = false;
    bool visitedFunctionDecl = false;
    bool visitedTemplate = false;

    void visit(daad::NumberExprAST&) override {}
    void visit(daad::VariableExprAST&) override {}
    void visit(daad::StringExprAST&) override {}
      void visit(daad::BinaryExprAST&) override {}
      void visit(daad::UnaryExprAST&) override {}
      void visit(daad::RawExprAST&) override {}
      void visit(daad::VarDeclStmtAST&) override { visitedVarDecl = true; }
      void visit(daad::AssignmentAST&) override { visitedAssignment = true; }
      void visit(daad::CompoundAssignmentAST&) override {}
    void visit(daad::IfStmtAST&) override { visitedIf = true; }
    void visit(daad::WhileStmtAST&) override { visitedWhile = true; }
    void visit(daad::ForStmtAST&) override {}
    void visit(daad::DoWhileStmtAST&) override {}
    void visit(daad::SwitchStmtAST&) override {}
    void visit(daad::ReturnStmtAST&) override {}
    void visit(daad::BreakStmtAST&) override {}
    void visit(daad::ContinueStmtAST&) override {}
    void visit(daad::FunctionDeclAST&) override { visitedFunctionDecl = true; }
    void visit(daad::FunctionCallAST&) override {}
    void visit(daad::ClassDeclAST&) override {}
    void visit(daad::StructDeclAST&) override {}
    void visit(daad::NamespaceDeclAST&) override {}
    void visit(daad::EnumDeclAST&) override {}
    void visit(daad::TryCatchStmtAST&) override {}
      void visit(daad::TemplateDeclAST&) override { visitedTemplate = true; }
      void visit(daad::MemberAccessExprAST&) override {}
      void visit(daad::MemberAssignmentAST&) override {}
      void visit(daad::ArraySubscriptExprAST&) override {}
      void visit(daad::ArraySubscriptAssignAST&) override {}
      void visit(daad::ThrowStmtAST&) override {}
      void visit(daad::NewExprAST&) override {}
      void visit(daad::DeleteStmtAST&) override {}
      void visit(daad::ExprStmtAST&) override {}
      void visit(daad::BoolExprAST&) override {}
      void visit(daad::NullExprAST&) override {}
      void visit(daad::TernaryExprAST&) override {}
      void visit(daad::ForEachStmtAST&) override {}
      void visit(daad::ImportStmtAST&) override {}
      void visit(daad::ExportStmtAST&) override {}
      void visit(daad::PrintStmtAST&) override {}
      void visit(daad::InputStmtAST&) override {}
      void visit(daad::ConstructorDeclAST&) override {}
      void visit(daad::SizeofExprAST&) override {}
      void visit(daad::TypeofExprAST&) override {}
      void visit(daad::LoadImageAST&) override {}
      void visit(daad::DrawImageAST&) override {}
      void visit(daad::ImageSizeAST&) override {}
      void visit(daad::SaveImageAST&) override {}
      void visit(daad::CropImageAST&) override {}
      void visit(daad::ResizeAST&) override {}
      void visit(daad::RotateImageAST&) override {}
      void visit(daad::FlipImageAST&) override {}
      void visit(daad::OpacityAST&) override {}
      void visit(daad::FilterAST&) override {}
      void visit(daad::OverlayAST&) override {}
      void visit(daad::BackgroundAST&) override {}
      void visit(daad::PixelAST&) override {}
      void visit(daad::DrawAST&) override {}
      void visit(daad::FillAST&) override {}
      void visit(daad::RectangleAST&) override {}
      void visit(daad::CircleAST&) override {}
      void visit(daad::LineAST&) override {}
      void visit(daad::TextOnCanvasAST&) override {}
      void visit(daad::ClearAST&) override {}
};

TEST(ParserTest, ParseVariableDeclaration) {
    std::string source = "صحيح العمر = 30 ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);

    auto stmt = parser.parseStatement();

    ASSERT_NE(stmt, nullptr);
    ASTVerifier verifier;
    stmt->accept(verifier);
    EXPECT_TRUE(verifier.visitedVarDecl);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(ParserTest, ParseMultipleDeclarations) {
    std::string source = "صحيح س = 1 ؛\nصحيح ص = 2 ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);

    auto stmt1 = parser.parseStatement();
    auto stmt2 = parser.parseStatement();

    ASSERT_NE(stmt1, nullptr);
    ASSERT_NE(stmt2, nullptr);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(ParserTest, ParseInvalidCode) {
    std::string source = "كلمة_خاطئة ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);

    auto stmt = parser.parseStatement();

    EXPECT_TRUE(diag.hasErrors());
}

TEST(ParserTest, ParserErrorRecovery) {
    std::string source = "صحيح س = 1 ؛\nكلمة_خاطئة ؛\nصحيح ص = 2 ؛";
    daad::Lexer lexer(source);
    daad::DiagnosticsEngine diag;
    daad::Parser parser(lexer, diag);

    parser.parseStatement();
    parser.parseStatement();
    parser.parseStatement();

    EXPECT_TRUE(diag.hasErrors());
}