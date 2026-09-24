#pragma once

#include "Daad/AST.hpp"
#include <memory>
#include <vector>

namespace daad {

/**
 * Bottom-Up In-Place AST Mutator
 * Operates directly on std::unique_ptr references to perform
 * recursive constant folding, algebraic simplification and DCE.
 * Keeps legacy ExprASTVisitor/StmVisitor interface for backward
 * compatibility with existing unit tests (isFolded flag).
 */
class OptimizerVisitor : public ExprASTVisitor, public StmtVisitor {
public:
    OptimizerVisitor() = default;

    // ── Legacy visitor interface (flag-based, for tests) ──────────────────
    void visit(NumberExprAST&) override {}
    void visit(StringExprAST&) override {}
    void visit(VariableExprAST&) override {}
    void visit(BoolExprAST&) override {}
    void visit(NullExprAST&) override {}
    void visit(BinaryExprAST& node) override;
    void visit(UnaryExprAST& node) override;
    void visit(RawExprAST&) override {}
    void visit(FunctionCallAST&) override {}
    void visit(MemberAccessExprAST&) override {}
    void visit(ArraySubscriptExprAST&) override {}
    void visit(TernaryExprAST&) override {}
    void visit(ImageSizeAST&) override {}
    void visit(PixelAST&) override {}
    void visit(NewExprAST&) override {}
    void visit(SizeofExprAST&) override {}
    void visit(TypeofExprAST&) override {}

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
    void visit(ClassDeclAST&) override {}
    void visit(StructDeclAST&) override {}
    void visit(NamespaceDeclAST& node) override;
    void visit(EnumDeclAST&) override {}
    void visit(TryCatchStmtAST& node) override;
    void visit(TemplateDeclAST& node) override;
    void visit(MemberAssignmentAST& node) override;
    void visit(ArraySubscriptAssignAST&) override {}
    void visit(ThrowStmtAST& node) override;
    void visit(DeleteStmtAST&) override {}
    void visit(ImportStmtAST&) override {}
    void visit(ExportStmtAST&) override {}
    void visit(PrintStmtAST&) override {}
    void visit(InputStmtAST&) override {}
    void visit(ExprStmtAST& node) override;
    void visit(ForEachStmtAST&) override {}
    void visit(ConstructorDeclAST&) override {}
    void visit(LoadImageAST&) override {}
    void visit(DrawImageAST&) override {}
    void visit(SaveImageAST&) override {}
    void visit(CropImageAST&) override {}
    void visit(ResizeAST&) override {}
    void visit(RotateImageAST&) override {}
    void visit(FlipImageAST&) override {}
    void visit(OpacityAST&) override {}
    void visit(FilterAST&) override {}
    void visit(OverlayAST&) override {}
    void visit(BackgroundAST&) override {}
    void visit(DrawAST&) override {}
    void visit(FillAST&) override {}
    void visit(RectangleAST&) override {}
    void visit(CircleAST&) override {}
    void visit(LineAST&) override {}
    void visit(TextOnCanvasAST&) override {}
    void visit(ClearAST&) override {}

    // ── New mutator interface (true in-place, bottom-up) ─────────────────
    void optimize(std::vector<std::unique_ptr<StmtAST>>& statements);
    void optimizeBlock(StmtList& block);
    bool optimizeExpr(std::unique_ptr<ExprAST>& expr);
    bool optimizeStmt(std::unique_ptr<StmtAST>& stmt);

private:
    bool tryFoldBinary(std::unique_ptr<ExprAST>& expr);
    bool tryFoldUnary(std::unique_ptr<ExprAST>& expr);
    bool tryFoldTernary(std::unique_ptr<ExprAST>& expr);
    bool trySimplifyBinary(std::unique_ptr<ExprAST>& expr);

    static bool isNumber(const std::unique_ptr<ExprAST>& e, double& out);
    static bool isBool(const std::unique_ptr<ExprAST>& e, bool& out);
    static bool isString(const std::unique_ptr<ExprAST>& e, std::string& out);
    static bool isZero(const std::unique_ptr<ExprAST>& e);
    static bool isOne(const std::unique_ptr<ExprAST>& e);
    static bool isTrue(const std::unique_ptr<ExprAST>& e);
    static bool isFalse(const std::unique_ptr<ExprAST>& e);
    static bool isUnconditionalJump(const std::unique_ptr<StmtAST>& stmt);
};

} // namespace daad
