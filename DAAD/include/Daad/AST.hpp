/**
 * @file AST.hpp
 * @brief شجرة النحو المجردة (AST) الكاملة للغة 'ض'
 */
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace daad {

class NumberExprAST;
class VariableExprAST;
class StringExprAST;
class BinaryExprAST;
class UnaryExprAST;
class RawExprAST;
class VarDeclStmtAST;
class AssignmentAST;
class CompoundAssignmentAST;
class IfStmtAST;
class WhileStmtAST;
class ForStmtAST;
class DoWhileStmtAST;
class SwitchStmtAST;
class ReturnStmtAST;
class BreakStmtAST;
class ContinueStmtAST;
class FunctionDeclAST;
class FunctionCallAST;
class ClassDeclAST;
class StructDeclAST;
class NamespaceDeclAST;
class EnumDeclAST;
class TryCatchStmtAST;
class TemplateDeclAST;
class MemberAccessExprAST;
class MemberAssignmentAST;
class ArraySubscriptExprAST;
class ArraySubscriptAssignAST;
class BoolExprAST;
class NullExprAST;
class TernaryExprAST;
class ForEachStmtAST;
class ThrowStmtAST;
class DeleteStmtAST;
class ImportStmtAST;
class ExportStmtAST;
class PrintStmtAST;
class InputStmtAST;
class ConstructorDeclAST;
class ExprStmtAST;
class NewExprAST;
class SizeofExprAST;
class TypeofExprAST;
class LoadImageAST;
class DrawImageAST;
class ImageSizeAST;
class SaveImageAST;
class CropImageAST;
class ResizeAST;
class RotateImageAST;
class FlipImageAST;
class OpacityAST;
class FilterAST;
class OverlayAST;
class BackgroundAST;
class PixelAST;
class DrawAST;
class FillAST;
class RectangleAST;
class CircleAST;
class LineAST;
class TextOnCanvasAST;
class ClearAST;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(NumberExprAST& node) = 0;
    virtual void visit(VariableExprAST& node) = 0;
    virtual void visit(StringExprAST& node) = 0;
    virtual void visit(BinaryExprAST& node) = 0;
    virtual void visit(UnaryExprAST& node) = 0;
    virtual void visit(RawExprAST& node) = 0;
    virtual void visit(VarDeclStmtAST& node) = 0;
    virtual void visit(AssignmentAST& node) = 0;
    virtual void visit(CompoundAssignmentAST& node) = 0;
    virtual void visit(IfStmtAST& node) = 0;
    virtual void visit(WhileStmtAST& node) = 0;
    virtual void visit(ForStmtAST& node) = 0;
    virtual void visit(DoWhileStmtAST& node) = 0;
    virtual void visit(SwitchStmtAST& node) = 0;
    virtual void visit(ReturnStmtAST& node) = 0;
    virtual void visit(BreakStmtAST& node) = 0;
    virtual void visit(ContinueStmtAST& node) = 0;
    virtual void visit(FunctionDeclAST& node) = 0;
    virtual void visit(FunctionCallAST& node) = 0;
    virtual void visit(ClassDeclAST& node) = 0;
    virtual void visit(StructDeclAST& node) = 0;
    virtual void visit(NamespaceDeclAST& node) = 0;
    virtual void visit(EnumDeclAST& node) = 0;
    virtual void visit(TryCatchStmtAST& node) = 0;
    virtual void visit(TemplateDeclAST& node) = 0;
    virtual void visit(MemberAccessExprAST& node) = 0;
    virtual void visit(MemberAssignmentAST& node) = 0;
    virtual void visit(ArraySubscriptExprAST& node) = 0;
    virtual void visit(ArraySubscriptAssignAST& node) = 0;
    virtual void visit(BoolExprAST& node) = 0;
    virtual void visit(NullExprAST& node) = 0;
    virtual void visit(TernaryExprAST& node) = 0;
    virtual void visit(ForEachStmtAST& node) = 0;
    virtual void visit(ThrowStmtAST& node) = 0;
    virtual void visit(DeleteStmtAST& node) = 0;
    virtual void visit(ImportStmtAST& node) = 0;
    virtual void visit(ExportStmtAST& node) = 0;
    virtual void visit(PrintStmtAST& node) = 0;
    virtual void visit(InputStmtAST& node) = 0;
    virtual void visit(ConstructorDeclAST& node) = 0;
    virtual void visit(ExprStmtAST& node) = 0;
    virtual void visit(NewExprAST& node) = 0;
    virtual void visit(SizeofExprAST& node) = 0;
    virtual void visit(TypeofExprAST& node) = 0;
    virtual void visit(LoadImageAST& node) = 0;
    virtual void visit(DrawImageAST& node) = 0;
    virtual void visit(ImageSizeAST& node) = 0;
    virtual void visit(SaveImageAST& node) = 0;
    virtual void visit(CropImageAST& node) = 0;
    virtual void visit(ResizeAST& node) = 0;
    virtual void visit(RotateImageAST& node) = 0;
    virtual void visit(FlipImageAST& node) = 0;
    virtual void visit(OpacityAST& node) = 0;
    virtual void visit(FilterAST& node) = 0;
    virtual void visit(OverlayAST& node) = 0;
    virtual void visit(BackgroundAST& node) = 0;
    virtual void visit(PixelAST& node) = 0;
    virtual void visit(DrawAST& node) = 0;
    virtual void visit(FillAST& node) = 0;
    virtual void visit(RectangleAST& node) = 0;
    virtual void visit(CircleAST& node) = 0;
    virtual void visit(LineAST& node) = 0;
    virtual void visit(TextOnCanvasAST& node) = 0;
    virtual void visit(ClearAST& node) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

class ExprAST : public ASTNode {};
class StmtAST : public ASTNode {};

class NumberExprAST : public ExprAST {
public:
    double value;
    explicit NumberExprAST(double val) : value(val) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class VariableExprAST : public ExprAST {
public:
    std::string name;
    explicit VariableExprAST(std::string name) : name(std::move(name)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BinaryExprAST : public ExprAST {
public:
    std::string op;
    std::unique_ptr<ExprAST> lhs, rhs;
    bool isFolded = false;
    double foldedValue = 0.0;
    BinaryExprAST(std::string op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class UnaryExprAST : public ExprAST {
public:
    std::string op;
    std::unique_ptr<ExprAST> operand;
    UnaryExprAST(std::string op, std::unique_ptr<ExprAST> operand)
        : op(std::move(op)), operand(std::move(operand)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// تعبير خام يُخرج نص C++ كما هو (مثل {} للمصفوفات الفارغة)
class RawExprAST : public ExprAST {
public:
    std::string raw;
    explicit RawExprAST(std::string raw) : raw(std::move(raw)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class VarDeclStmtAST : public StmtAST {
public:
    std::string type;
    std::string name;
    std::unique_ptr<ExprAST> init;
    VarDeclStmtAST(std::string type, std::string name, std::unique_ptr<ExprAST> init)
        : type(std::move(type)), name(std::move(name)), init(std::move(init)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class AssignmentAST : public StmtAST {
public:
    std::string name;
    std::unique_ptr<ExprAST> value;
    AssignmentAST(std::string name, std::unique_ptr<ExprAST> value)
        : name(std::move(name)), value(std::move(value)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CompoundAssignmentAST : public StmtAST {
public:
    std::string name;
    std::string op;
    std::unique_ptr<ExprAST> value;
    CompoundAssignmentAST(std::string name, std::string op, std::unique_ptr<ExprAST> value)
        : name(std::move(name)), op(std::move(op)), value(std::move(value)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IfStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<StmtAST>> thenBody;
    std::vector<std::unique_ptr<StmtAST>> elseBody;
    IfStmtAST(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<StmtAST>> thenB, std::vector<std::unique_ptr<StmtAST>> elseB)
        : condition(std::move(cond)), thenBody(std::move(thenB)), elseBody(std::move(elseB)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class WhileStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<StmtAST>> body;
    WhileStmtAST(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<StmtAST>> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FunctionDeclAST : public StmtAST {
public:
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;
    std::vector<std::unique_ptr<StmtAST>> body;
    FunctionDeclAST(std::string rType, std::string n, std::vector<std::pair<std::string, std::string>> p, std::vector<std::unique_ptr<StmtAST>> b)
        : returnType(std::move(rType)), name(std::move(n)), params(std::move(p)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FunctionCallAST : public ExprAST {
public:
    std::string name;
    std::vector<std::unique_ptr<ExprAST>> args;
    FunctionCallAST(std::string n, std::vector<std::unique_ptr<ExprAST>> a)
        : name(std::move(n)), args(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ClassDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> members; // type, name
    std::vector<std::unique_ptr<FunctionDeclAST>> methods;
    ClassDeclAST(std::string n) : name(std::move(n)) {}
    ClassDeclAST(std::string n, std::vector<std::pair<std::string, std::string>> m,
                 std::vector<std::unique_ptr<FunctionDeclAST>> meth)
        : name(std::move(n)), members(std::move(m)), methods(std::move(meth)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class StructDeclAST : public StmtAST {
public:
    std::string name;
    StructDeclAST(std::string n) : name(std::move(n)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class TemplateDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<StmtAST> body;
    TemplateDeclAST(std::string n, std::vector<std::string> p, std::unique_ptr<StmtAST> b)
        : name(std::move(n)), params(std::move(p)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class StringExprAST : public ExprAST {
public:
    std::string value;
    explicit StringExprAST(std::string val) : value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ForStmtAST : public StmtAST {
public:
    std::unique_ptr<StmtAST> init;
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<StmtAST> update;
    std::vector<std::unique_ptr<StmtAST>> body;
    ForStmtAST(std::unique_ptr<StmtAST> i, std::unique_ptr<ExprAST> c, std::unique_ptr<StmtAST> u, std::vector<std::unique_ptr<StmtAST>> b)
        : init(std::move(i)), condition(std::move(c)), update(std::move(u)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class DoWhileStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<StmtAST>> body;
    DoWhileStmtAST(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<StmtAST>> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class SwitchCaseAST {
public:
    std::unique_ptr<ExprAST> value;
    std::vector<std::unique_ptr<StmtAST>> body;
};

class SwitchStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> expression;
    std::vector<SwitchCaseAST> cases;
    std::vector<std::unique_ptr<StmtAST>> defaultBody;
    SwitchStmtAST(std::unique_ptr<ExprAST> expr, std::vector<SwitchCaseAST> c, std::vector<std::unique_ptr<StmtAST>> d)
        : expression(std::move(expr)), cases(std::move(c)), defaultBody(std::move(d)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ReturnStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> value;
    explicit ReturnStmtAST(std::unique_ptr<ExprAST> val) : value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BreakStmtAST : public StmtAST {
public:
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ContinueStmtAST : public StmtAST {
public:
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class NamespaceDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::unique_ptr<StmtAST>> body;
    NamespaceDeclAST(std::string n, std::vector<std::unique_ptr<StmtAST>> b)
        : name(std::move(n)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class EnumDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::string> values;
    EnumDeclAST(std::string n, std::vector<std::string> v)
        : name(std::move(n)), values(std::move(v)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class TryCatchStmtAST : public StmtAST {
public:
    std::vector<std::unique_ptr<StmtAST>> tryBody;
    std::string catchVar;
    std::vector<std::unique_ptr<StmtAST>> catchBody;
    std::vector<std::unique_ptr<StmtAST>> finallyBody;
    TryCatchStmtAST(std::vector<std::unique_ptr<StmtAST>> t, std::string cv,
                     std::vector<std::unique_ptr<StmtAST>> c,
                     std::vector<std::unique_ptr<StmtAST>> f = {})
        : tryBody(std::move(t)), catchVar(std::move(cv)), catchBody(std::move(c)),
          finallyBody(std::move(f)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class MemberAccessExprAST : public ExprAST {
public:
    std::string object;
    std::string member;
    MemberAccessExprAST(std::string obj, std::string mem)
        : object(std::move(obj)), member(std::move(mem)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class MemberAssignmentAST : public StmtAST {
public:
    std::string object;
    std::string member;
    std::unique_ptr<ExprAST> value;
    MemberAssignmentAST(std::string obj, std::string mem, std::unique_ptr<ExprAST> val)
        : object(std::move(obj)), member(std::move(mem)), value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ArraySubscriptExprAST : public ExprAST {
public:
    std::string name;
    std::unique_ptr<ExprAST> index;
    std::unique_ptr<ExprAST> base; // تعبير أساسي (للفهرسة المتسلسلة مثل a[i][0])
    ArraySubscriptExprAST(std::string n, std::unique_ptr<ExprAST> idx)
        : name(std::move(n)), index(std::move(idx)) {}
    ArraySubscriptExprAST(std::string n, std::unique_ptr<ExprAST> idx, std::unique_ptr<ExprAST> b)
        : name(std::move(n)), index(std::move(idx)), base(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ArraySubscriptAssignAST : public StmtAST {
public:
    std::string name;
    std::unique_ptr<ExprAST> index;
    std::unique_ptr<ExprAST> value;
    std::unique_ptr<ExprAST> base; // تعبير أساسي (للفهرسة المتسلسلة مثل a[i][0] = x)
    ArraySubscriptAssignAST(std::string n, std::unique_ptr<ExprAST> idx, std::unique_ptr<ExprAST> val)
        : name(std::move(n)), index(std::move(idx)), value(std::move(val)) {}
    ArraySubscriptAssignAST(std::string n, std::unique_ptr<ExprAST> idx, std::unique_ptr<ExprAST> val, std::unique_ptr<ExprAST> b)
        : name(std::move(n)), index(std::move(idx)), value(std::move(val)), base(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BoolExprAST : public ExprAST {
public:
    bool value;
    explicit BoolExprAST(bool val) : value(val) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class NullExprAST : public ExprAST {
public:
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class TernaryExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<ExprAST> trueExpr;
    std::unique_ptr<ExprAST> falseExpr;
    TernaryExprAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<ExprAST> trueE, std::unique_ptr<ExprAST> falseE)
        : condition(std::move(cond)), trueExpr(std::move(trueE)), falseExpr(std::move(falseE)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ForEachStmtAST : public StmtAST {
public:
    std::string varType;
    std::string varName;
    std::unique_ptr<ExprAST> iterable;
    std::vector<std::unique_ptr<StmtAST>> body;
    ForEachStmtAST(std::string vType, std::string vName, std::unique_ptr<ExprAST> iter, std::vector<std::unique_ptr<StmtAST>> b)
        : varType(std::move(vType)), varName(std::move(vName)), iterable(std::move(iter)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ThrowStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> value;
    explicit ThrowStmtAST(std::unique_ptr<ExprAST> val) : value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class DeleteStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> operand;
    explicit DeleteStmtAST(std::unique_ptr<ExprAST> op) : operand(std::move(op)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ImportStmtAST : public StmtAST {
public:
    std::string path;
    explicit ImportStmtAST(std::string p) : path(std::move(p)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ExportStmtAST : public StmtAST {
public:
    std::vector<std::string> names;
    explicit ExportStmtAST(std::vector<std::string> n) : names(std::move(n)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class PrintStmtAST : public StmtAST {
public:
    std::vector<std::unique_ptr<ExprAST>> args;
    explicit PrintStmtAST(std::vector<std::unique_ptr<ExprAST>> a) : args(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// جملة الإدخال: ادخل(متغير) — تقرأ قيمة من لوحة المفاتيح إلى المتغير
class InputStmtAST : public StmtAST {
public:
    std::string varName;
    explicit InputStmtAST(std::string v) : varName(std::move(v)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ConstructorDeclAST : public StmtAST {
public:
    std::vector<std::pair<std::string, std::string>> params;
    std::vector<std::unique_ptr<StmtAST>> body;
    ConstructorDeclAST(std::vector<std::pair<std::string, std::string>> p, std::vector<std::unique_ptr<StmtAST>> b)
        : params(std::move(p)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ExprStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> expr;
    explicit ExprStmtAST(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class NewExprAST : public ExprAST {
public:
    std::string typeName;
    std::vector<std::unique_ptr<ExprAST>> args;
    NewExprAST(std::string type, std::vector<std::unique_ptr<ExprAST>> a)
        : typeName(std::move(type)), args(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class SizeofExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> expr;
    explicit SizeofExprAST(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class TypeofExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> expr;
    explicit TypeofExprAST(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// ── Image Processing AST Nodes (20) — يتوافق مع Web ──────────────────────────
class LoadImageAST : public StmtAST {
public:
    std::string path;
    std::string varName;
    LoadImageAST(std::string p, std::string v) : path(std::move(p)), varName(std::move(v)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class DrawImageAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> x, y, w, h;
    DrawImageAST(std::string img, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_,
                 std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : imgVar(std::move(img)), x(std::move(x_)), y(std::move(y_)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ImageSizeAST : public ExprAST {
public:
    std::string imgVar;
    explicit ImageSizeAST(std::string img) : imgVar(std::move(img)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class SaveImageAST : public StmtAST {
public:
    std::string imgVar;
    std::string path;
    SaveImageAST(std::string img, std::string p) : imgVar(std::move(img)), path(std::move(p)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CropImageAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> x, y, w, h;
    CropImageAST(std::string img, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_,
                 std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : imgVar(std::move(img)), x(std::move(x_)), y(std::move(y_)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ResizeAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> w, h;
    ResizeAST(std::string img, std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : imgVar(std::move(img)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class RotateImageAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> angle;
    RotateImageAST(std::string img, std::unique_ptr<ExprAST> a)
        : imgVar(std::move(img)), angle(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FlipImageAST : public StmtAST {
public:
    std::string imgVar;
    std::string direction;
    FlipImageAST(std::string img, std::string d) : imgVar(std::move(img)), direction(std::move(d)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class OpacityAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> value;
    OpacityAST(std::string img, std::unique_ptr<ExprAST> v)
        : imgVar(std::move(img)), value(std::move(v)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FilterAST : public StmtAST {
public:
    std::string imgVar;
    std::string filterName;
    FilterAST(std::string img, std::string f) : imgVar(std::move(img)), filterName(std::move(f)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class OverlayAST : public StmtAST {
public:
    std::string imgVar1;
    std::string imgVar2;
    std::unique_ptr<ExprAST> x, y;
    OverlayAST(std::string img1, std::string img2, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_)
        : imgVar1(std::move(img1)), imgVar2(std::move(img2)), x(std::move(x_)), y(std::move(y_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BackgroundAST : public StmtAST {
public:
    std::string imgVar;
    std::string bgVar;
    BackgroundAST(std::string img, std::string bg) : imgVar(std::move(img)), bgVar(std::move(bg)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class PixelAST : public ExprAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> x, y;
    PixelAST(std::string img, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_)
        : imgVar(std::move(img)), x(std::move(x_)), y(std::move(y_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class DrawAST : public StmtAST {
public:
    std::string shape;
    std::vector<std::unique_ptr<ExprAST>> args;
    DrawAST(std::string s, std::vector<std::unique_ptr<ExprAST>> a) : shape(std::move(s)), args(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FillAST : public StmtAST {
public:
    std::string shape;
    std::vector<std::unique_ptr<ExprAST>> args;
    FillAST(std::string s, std::vector<std::unique_ptr<ExprAST>> a) : shape(std::move(s)), args(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class RectangleAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> x, y, w, h;
    RectangleAST(std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_,
                 std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : x(std::move(x_)), y(std::move(y_)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CircleAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> x, y, r;
    CircleAST(std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_, std::unique_ptr<ExprAST> r_)
        : x(std::move(x_)), y(std::move(y_)), r(std::move(r_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class LineAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> x1, y1, x2, y2;
    LineAST(std::unique_ptr<ExprAST> x1_, std::unique_ptr<ExprAST> y1_,
            std::unique_ptr<ExprAST> x2_, std::unique_ptr<ExprAST> y2_)
        : x1(std::move(x1_)), y1(std::move(y1_)), x2(std::move(x2_)), y2(std::move(y2_)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class TextOnCanvasAST : public StmtAST {
public:
    std::string text;
    std::unique_ptr<ExprAST> x, y, fontSize;
    TextOnCanvasAST(std::string t, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_, std::unique_ptr<ExprAST> fs)
        : text(std::move(t)), x(std::move(x_)), y(std::move(y_)), fontSize(std::move(fs)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ClearAST : public StmtAST {
public:
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

} // namespace daad