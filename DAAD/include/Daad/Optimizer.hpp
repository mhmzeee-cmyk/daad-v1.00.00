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
    void visit(RawExprAST&) override {}
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
    void visit(BoolExprAST&) override {}
    void visit(NullExprAST&) override {}
    void visit(TernaryExprAST&) override {}
    void visit(ForEachStmtAST&) override {}
    void visit(ThrowStmtAST&) override;
    void visit(DeleteStmtAST&) override {}
    void visit(ImportStmtAST&) override {}
    void visit(ExportStmtAST&) override {}
    void visit(PrintStmtAST&) override {}
    void visit(InputStmtAST&) override {}
    void visit(ConstructorDeclAST&) override {}
    void visit(ExprStmtAST&) override;
    void visit(NewExprAST&) override {}
    void visit(SizeofExprAST&) override {}
    void visit(TypeofExprAST&) override {}
    void visit(LoadImageAST&) override {}
    void visit(DrawImageAST&) override {}
    void visit(ImageSizeAST&) override {}
    void visit(SaveImageAST&) override {}
    void visit(CropImageAST&) override {}
    void visit(ResizeAST&) override {}
    void visit(RotateImageAST&) override {}
    void visit(FlipImageAST&) override {}
    void visit(OpacityAST&) override {}
    void visit(FilterAST&) override {}
    void visit(OverlayAST&) override {}
    void visit(BackgroundAST&) override {}
    void visit(PixelAST&) override {}
    void visit(DrawAST&) override {}
    void visit(FillAST&) override {}
    void visit(RectangleAST&) override {}
    void visit(CircleAST&) override {}
    void visit(LineAST&) override {}
    void visit(TextOnCanvasAST&) override {}
    void visit(ClearAST&) override {}
};

} // namespace daad
