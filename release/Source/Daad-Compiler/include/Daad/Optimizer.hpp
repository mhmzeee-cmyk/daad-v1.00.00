#pragma once

#include "Daad/AST.hpp"

namespace daad {

class OptimizerVisitor : public ExprASTVisitor, public StmtVisitor {
public:
    OptimizerVisitor() = default;

    // ── ExprAST visitors ──────────────────────────────────────────────────────
    void visit(NumberExprAST&) override {}
    void visit(StringExprAST&) override {}
    void visit(VariableExprAST&) override {}
    void visit(BoolExprAST&) override {}
    void visit(NullExprAST&) override {}
    void visit(BinaryExprAST&) override;
    void visit(UnaryExprAST&) override;
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

    // ── StmtAST visitors ──────────────────────────────────────────────────────
    void visit(VarDeclStmtAST&) override;
    void visit(AssignmentAST&) override;
    void visit(CompoundAssignmentAST&) override;
    void visit(IfStmtAST&) override;
    void visit(WhileStmtAST&) override;
    void visit(ForStmtAST&) override;
    void visit(DoWhileStmtAST&) override;
    void visit(SwitchStmtAST&) override;
    void visit(ReturnStmtAST&) override;
    void visit(BreakStmtAST&) override {}
    void visit(ContinueStmtAST&) override {}
    void visit(FunctionDeclAST&) override;
    void visit(ClassDeclAST&) override {}
    void visit(StructDeclAST&) override {}
    void visit(NamespaceDeclAST&) override;
    void visit(EnumDeclAST&) override {}
    void visit(TryCatchStmtAST&) override;
    void visit(TemplateDeclAST&) override;
    void visit(MemberAssignmentAST&) override;
    void visit(ArraySubscriptAssignAST&) override {}
    void visit(ThrowStmtAST&) override;
    void visit(DeleteStmtAST&) override {}
    void visit(ImportStmtAST&) override {}
    void visit(ExportStmtAST&) override {}
    void visit(PrintStmtAST&) override {}
    void visit(InputStmtAST&) override {}
    void visit(ExprStmtAST&) override;
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
};

} // namespace daad
