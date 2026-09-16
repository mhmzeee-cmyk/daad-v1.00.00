#pragma once

#include "Daad/Token.hpp"
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <cstddef>

namespace daad {

// Forward declarations
class ExprAST;
class StmtAST;
class ExprASTVisitor;
class StmtVisitor;

using StmtList = std::vector<std::unique_ptr<StmtAST>>;
using ExprList = std::vector<std::unique_ptr<ExprAST>>;
using ParamList = std::vector<std::pair<std::string, std::string>>;

// ═══════════════════════════════════════════════════════════════════════════════
// Access Level Enum
// ═══════════════════════════════════════════════════════════════════════════════

enum class AccessLevel {
    Public,
    Private,
    Protected
};

// ═══════════════════════════════════════════════════════════════════════════════
// Visitor Base Classes
// ═══════════════════════════════════════════════════════════════════════════════

class ExprASTVisitor {
public:
    virtual ~ExprASTVisitor() = default;
    virtual void visit(class NumberExprAST&) = 0;
    virtual void visit(class StringExprAST&) = 0;
    virtual void visit(class VariableExprAST&) = 0;
    virtual void visit(class BoolExprAST&) = 0;
    virtual void visit(class NullExprAST&) = 0;
    virtual void visit(class BinaryExprAST&) = 0;
    virtual void visit(class UnaryExprAST&) = 0;
    virtual void visit(class RawExprAST&) = 0;
    virtual void visit(class FunctionCallAST&) = 0;
    virtual void visit(class MemberAccessExprAST&) = 0;
    virtual void visit(class ArraySubscriptExprAST&) = 0;
    virtual void visit(class TernaryExprAST&) = 0;
    virtual void visit(class ImageSizeAST&) = 0;
    virtual void visit(class PixelAST&) = 0;
    virtual void visit(class NewExprAST&) = 0;
    virtual void visit(class SizeofExprAST&) = 0;
    virtual void visit(class TypeofExprAST&) = 0;
};

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(class VarDeclStmtAST&) = 0;
    virtual void visit(class AssignmentAST&) = 0;
    virtual void visit(class CompoundAssignmentAST&) = 0;
    virtual void visit(class IfStmtAST&) = 0;
    virtual void visit(class WhileStmtAST&) = 0;
    virtual void visit(class ForStmtAST&) = 0;
    virtual void visit(class DoWhileStmtAST&) = 0;
    virtual void visit(class SwitchStmtAST&) = 0;
    virtual void visit(class ReturnStmtAST&) = 0;
    virtual void visit(class BreakStmtAST&) = 0;
    virtual void visit(class ContinueStmtAST&) = 0;
    virtual void visit(class FunctionDeclAST&) = 0;
    virtual void visit(class ClassDeclAST&) = 0;
    virtual void visit(class StructDeclAST&) = 0;
    virtual void visit(class NamespaceDeclAST&) = 0;
    virtual void visit(class EnumDeclAST&) = 0;
    virtual void visit(class TryCatchStmtAST&) = 0;
    virtual void visit(class TemplateDeclAST&) = 0;
    virtual void visit(class MemberAssignmentAST&) = 0;
    virtual void visit(class ArraySubscriptAssignAST&) = 0;
    virtual void visit(class ThrowStmtAST&) = 0;
    virtual void visit(class DeleteStmtAST&) = 0;
    virtual void visit(class ImportStmtAST&) = 0;
    virtual void visit(class ExportStmtAST&) = 0;
    virtual void visit(class PrintStmtAST&) = 0;
    virtual void visit(class InputStmtAST&) = 0;
    virtual void visit(class ExprStmtAST&) = 0;
    virtual void visit(class ForEachStmtAST&) = 0;
    virtual void visit(class ConstructorDeclAST&) = 0;
    virtual void visit(class LoadImageAST&) = 0;
    virtual void visit(class DrawImageAST&) = 0;
    virtual void visit(class SaveImageAST&) = 0;
    virtual void visit(class CropImageAST&) = 0;
    virtual void visit(class ResizeAST&) = 0;
    virtual void visit(class RotateImageAST&) = 0;
    virtual void visit(class FlipImageAST&) = 0;
    virtual void visit(class OpacityAST&) = 0;
    virtual void visit(class FilterAST&) = 0;
    virtual void visit(class OverlayAST&) = 0;
    virtual void visit(class BackgroundAST&) = 0;
    virtual void visit(class DrawAST&) = 0;
    virtual void visit(class FillAST&) = 0;
    virtual void visit(class RectangleAST&) = 0;
    virtual void visit(class CircleAST&) = 0;
    virtual void visit(class LineAST&) = 0;
    virtual void visit(class TextOnCanvasAST&) = 0;
    virtual void visit(class ClearAST&) = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Base AST Classes
// ═══════════════════════════════════════════════════════════════════════════════

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual void accept(ExprASTVisitor& v) = 0;
};

class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual void accept(StmtVisitor& v) = 0;
    int line = 0; // سطر المصدر (.ض) — يملؤه المحلل، ويستخدمه مولد #line للتصحيح
};

// ═══════════════════════════════════════════════════════════════════════════════
// Expression AST Nodes
// ═══════════════════════════════════════════════════════════════════════════════

class NumberExprAST : public ExprAST {
public:
    double value;
    explicit NumberExprAST(double v) : value(v) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class StringExprAST : public ExprAST {
public:
    std::string value;
    explicit StringExprAST(std::string v) : value(std::move(v)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class VariableExprAST : public ExprAST {
public:
    std::string name;
    explicit VariableExprAST(std::string n) : name(std::move(n)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class BoolExprAST : public ExprAST {
public:
    bool value;
    explicit BoolExprAST(bool v) : value(v) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class NullExprAST : public ExprAST {
public:
    NullExprAST() = default;
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class BinaryExprAST : public ExprAST {
public:
    std::string op;
    std::unique_ptr<ExprAST> lhs;
    std::unique_ptr<ExprAST> rhs;
    bool isFolded = false;
    double foldedValue = 0.0;

    BinaryExprAST(std::string o, std::unique_ptr<ExprAST> l, std::unique_ptr<ExprAST> r)
        : op(std::move(o)), lhs(std::move(l)), rhs(std::move(r)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class UnaryExprAST : public ExprAST {
public:
    std::string op;
    std::unique_ptr<ExprAST> operand;
    UnaryExprAST(std::string o, std::unique_ptr<ExprAST> ope)
        : op(std::move(o)), operand(std::move(ope)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class RawExprAST : public ExprAST {
public:
    std::string raw;
    explicit RawExprAST(std::string r) : raw(std::move(r)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class FunctionCallAST : public ExprAST {
public:
    std::string name;
    ExprList args;
    FunctionCallAST(std::string n, ExprList a) : name(std::move(n)), args(std::move(a)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class MemberAccessExprAST : public ExprAST {
public:
    std::string object;
    std::string member;
    MemberAccessExprAST(std::string o, std::string m) : object(std::move(o)), member(std::move(m)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class ArraySubscriptExprAST : public ExprAST {
public:
    std::string name;
    std::unique_ptr<ExprAST> index;
    std::unique_ptr<ExprAST> base;
    ArraySubscriptExprAST(std::string n, std::unique_ptr<ExprAST> i, std::unique_ptr<ExprAST> b = nullptr)
        : name(std::move(n)), index(std::move(i)), base(std::move(b)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class TernaryExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<ExprAST> trueExpr;
    std::unique_ptr<ExprAST> falseExpr;
    TernaryExprAST(std::unique_ptr<ExprAST> c, std::unique_ptr<ExprAST> t, std::unique_ptr<ExprAST> f)
        : condition(std::move(c)), trueExpr(std::move(t)), falseExpr(std::move(f)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class ImageSizeAST : public ExprAST {
public:
    std::string imgVar;
    explicit ImageSizeAST(std::string iv) : imgVar(std::move(iv)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class PixelAST : public ExprAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> x;
    std::unique_ptr<ExprAST> y;
    PixelAST(std::string iv, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_)
        : imgVar(std::move(iv)), x(std::move(x_)), y(std::move(y_)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class NewExprAST : public ExprAST {
public:
    std::string typeName;
    ExprList args;
    NewExprAST(std::string tn, ExprList a) : typeName(std::move(tn)), args(std::move(a)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class SizeofExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> expr;
    explicit SizeofExprAST(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

class TypeofExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> expr;
    explicit TypeofExprAST(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
    void accept(ExprASTVisitor& v) override { v.visit(*this); }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Statement AST Nodes
// ═══════════════════════════════════════════════════════════════════════════════

class VarDeclStmtAST : public StmtAST {
public:
    std::string type;
    std::string name;
    std::unique_ptr<ExprAST> init;
    VarDeclStmtAST(std::string t, std::string n, std::unique_ptr<ExprAST> i)
        : type(std::move(t)), name(std::move(n)), init(std::move(i)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class AssignmentAST : public StmtAST {
public:
    std::string name;
    std::unique_ptr<ExprAST> value;
    AssignmentAST(std::string n, std::unique_ptr<ExprAST> v) : name(std::move(n)), value(std::move(v)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class CompoundAssignmentAST : public StmtAST {
public:
    std::string name;
    std::string op;
    std::unique_ptr<ExprAST> value;
    CompoundAssignmentAST(std::string n, std::string o, std::unique_ptr<ExprAST> v)
        : name(std::move(n)), op(std::move(o)), value(std::move(v)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class IfStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    StmtList thenBody;
    StmtList elseBody;
    IfStmtAST(std::unique_ptr<ExprAST> c, StmtList t, StmtList e)
        : condition(std::move(c)), thenBody(std::move(t)), elseBody(std::move(e)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class WhileStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    StmtList body;
    WhileStmtAST(std::unique_ptr<ExprAST> c, StmtList b) : condition(std::move(c)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ForStmtAST : public StmtAST {
public:
    std::unique_ptr<StmtAST> init;
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<StmtAST> update;
    StmtList body;
    ForStmtAST(std::unique_ptr<StmtAST> i, std::unique_ptr<ExprAST> c,
               std::unique_ptr<StmtAST> u, StmtList b)
        : init(std::move(i)), condition(std::move(c)), update(std::move(u)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class DoWhileStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    StmtList body;
    DoWhileStmtAST(std::unique_ptr<ExprAST> c, StmtList b) : condition(std::move(c)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

struct SwitchCaseAST {
    std::unique_ptr<ExprAST> value;
    StmtList body;
};

class SwitchStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> expression;
    std::vector<SwitchCaseAST> cases;
    StmtList defaultBody;
    SwitchStmtAST(std::unique_ptr<ExprAST> e, std::vector<SwitchCaseAST> c, StmtList d)
        : expression(std::move(e)), cases(std::move(c)), defaultBody(std::move(d)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ReturnStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> value;
    explicit ReturnStmtAST(std::unique_ptr<ExprAST> v) : value(std::move(v)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class BreakStmtAST : public StmtAST {
public:
    BreakStmtAST() = default;
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ContinueStmtAST : public StmtAST {
public:
    ContinueStmtAST() = default;
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class FunctionDeclAST : public StmtAST {
public:
    std::string returnType;
    std::string name;
    ParamList params;
    StmtList body;
    bool isAbstract = false;  // مجرّد دالة → generates `virtual ... = 0;`
    FunctionDeclAST() = default;
    FunctionDeclAST(std::string rt, std::string n, ParamList p, StmtList b)
        : returnType(std::move(rt)), name(std::move(n)), params(std::move(p)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ClassDeclAST : public StmtAST {
public:
    std::string name;
    ParamList members;
    std::vector<std::unique_ptr<FunctionDeclAST>> methods;
    bool isAbstract = false;  // مجرّد صنف → generates abstract class
    std::vector<AccessLevel> memberAccess;  // Access level for each member
    std::vector<AccessLevel> methodAccess;  // Access level for each method
    explicit ClassDeclAST(std::string n) : name(std::move(n)) {}
    ClassDeclAST(std::string n, ParamList m, std::vector<std::unique_ptr<FunctionDeclAST>> meth)
        : name(std::move(n)), members(std::move(m)), methods(std::move(meth)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class StructDeclAST : public StmtAST {
public:
    std::string name;
    explicit StructDeclAST(std::string n) : name(std::move(n)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class NamespaceDeclAST : public StmtAST {
public:
    std::string name;
    StmtList body;
    NamespaceDeclAST(std::string n, StmtList b) : name(std::move(n)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class EnumDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::string> values;
    EnumDeclAST(std::string n, std::vector<std::string> v) : name(std::move(n)), values(std::move(v)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class TryCatchStmtAST : public StmtAST {
public:
    StmtList tryBody;
    std::string catchVar;
    StmtList catchBody;
    StmtList finallyBody;
    TryCatchStmtAST(StmtList tb, std::string cv, StmtList cb, StmtList fb)
        : tryBody(std::move(tb)), catchVar(std::move(cv)), catchBody(std::move(cb)), finallyBody(std::move(fb)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class TemplateDeclAST : public StmtAST {
public:
    std::string kind;
    std::vector<std::string> params;
    std::unique_ptr<StmtAST> body;
    TemplateDeclAST(std::string k, std::vector<std::string> p, std::unique_ptr<StmtAST> b)
        : kind(std::move(k)), params(std::move(p)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class MemberAssignmentAST : public StmtAST {
public:
    std::string object;
    std::string member;
    std::unique_ptr<ExprAST> value;
    MemberAssignmentAST(std::string o, std::string m, std::unique_ptr<ExprAST> v)
        : object(std::move(o)), member(std::move(m)), value(std::move(v)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ArraySubscriptAssignAST : public StmtAST {
public:
    std::string name;
    std::unique_ptr<ExprAST> index;
    std::unique_ptr<ExprAST> value;
    std::unique_ptr<ExprAST> base;
    ArraySubscriptAssignAST(std::string n, std::unique_ptr<ExprAST> i, std::unique_ptr<ExprAST> v,
                            std::unique_ptr<ExprAST> b = nullptr)
        : name(std::move(n)), index(std::move(i)), value(std::move(v)), base(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ThrowStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> value;
    explicit ThrowStmtAST(std::unique_ptr<ExprAST> v) : value(std::move(v)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class DeleteStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> operand;
    explicit DeleteStmtAST(std::unique_ptr<ExprAST> o) : operand(std::move(o)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ImportStmtAST : public StmtAST {
public:
    std::string path;
    explicit ImportStmtAST(std::string p) : path(std::move(p)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ExportStmtAST : public StmtAST {
public:
    std::vector<std::string> names;
    explicit ExportStmtAST(std::vector<std::string> n) : names(std::move(n)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class PrintStmtAST : public StmtAST {
public:
    ExprList args;
    explicit PrintStmtAST(ExprList a) : args(std::move(a)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class InputStmtAST : public StmtAST {
public:
    std::string varName;
    explicit InputStmtAST(std::string vn) : varName(std::move(vn)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ExprStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> expr;
    explicit ExprStmtAST(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ForEachStmtAST : public StmtAST {
public:
    std::string varType;
    std::string varName;
    std::unique_ptr<ExprAST> iterable;
    StmtList body;
    ForEachStmtAST(std::string vt, std::string vn, std::unique_ptr<ExprAST> it, StmtList b)
        : varType(std::move(vt)), varName(std::move(vn)), iterable(std::move(it)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ConstructorDeclAST : public StmtAST {
public:
    ParamList params;
    StmtList body;
    ConstructorDeclAST(ParamList p, StmtList b) : params(std::move(p)), body(std::move(b)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Image Processing AST Nodes (20)
// ═══════════════════════════════════════════════════════════════════════════════

class LoadImageAST : public StmtAST {
public:
    std::string path;
    std::string varName;
    LoadImageAST(std::string p, std::string vn) : path(std::move(p)), varName(std::move(vn)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class DrawImageAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> x, y, w, h;
    DrawImageAST(std::string iv, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_,
                 std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : imgVar(std::move(iv)), x(std::move(x_)), y(std::move(y_)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class SaveImageAST : public StmtAST {
public:
    std::string imgVar;
    std::string path;
    SaveImageAST(std::string iv, std::string p) : imgVar(std::move(iv)), path(std::move(p)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class CropImageAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> x, y, w, h;
    CropImageAST(std::string iv, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_,
                 std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : imgVar(std::move(iv)), x(std::move(x_)), y(std::move(y_)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ResizeAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> w, h;
    ResizeAST(std::string iv, std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : imgVar(std::move(iv)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class RotateImageAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> angle;
    RotateImageAST(std::string iv, std::unique_ptr<ExprAST> a)
        : imgVar(std::move(iv)), angle(std::move(a)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class FlipImageAST : public StmtAST {
public:
    std::string imgVar;
    std::string direction;
    FlipImageAST(std::string iv, std::string d) : imgVar(std::move(iv)), direction(std::move(d)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class OpacityAST : public StmtAST {
public:
    std::string imgVar;
    std::unique_ptr<ExprAST> value;
    OpacityAST(std::string iv, std::unique_ptr<ExprAST> v) : imgVar(std::move(iv)), value(std::move(v)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class FilterAST : public StmtAST {
public:
    std::string imgVar;
    std::string filterName;
    FilterAST(std::string iv, std::string fn) : imgVar(std::move(iv)), filterName(std::move(fn)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class OverlayAST : public StmtAST {
public:
    std::string imgVar1;
    std::string imgVar2;
    std::unique_ptr<ExprAST> x, y;
    OverlayAST(std::string iv1, std::string iv2, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_)
        : imgVar1(std::move(iv1)), imgVar2(std::move(iv2)), x(std::move(x_)), y(std::move(y_)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class BackgroundAST : public StmtAST {
public:
    std::string imgVar;
    std::string bgVar;
    BackgroundAST(std::string iv, std::string bv) : imgVar(std::move(iv)), bgVar(std::move(bv)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class DrawAST : public StmtAST {
public:
    std::string shape;
    ExprList args;
    DrawAST(std::string s, ExprList a) : shape(std::move(s)), args(std::move(a)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class FillAST : public StmtAST {
public:
    std::string shape;
    ExprList args;
    FillAST(std::string s, ExprList a) : shape(std::move(s)), args(std::move(a)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class RectangleAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> x, y, w, h;
    RectangleAST(std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_,
                 std::unique_ptr<ExprAST> w_, std::unique_ptr<ExprAST> h_)
        : x(std::move(x_)), y(std::move(y_)), w(std::move(w_)), h(std::move(h_)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class CircleAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> x, y, r;
    CircleAST(std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_, std::unique_ptr<ExprAST> r_)
        : x(std::move(x_)), y(std::move(y_)), r(std::move(r_)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class LineAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> x1, y1, x2, y2;
    LineAST(std::unique_ptr<ExprAST> x1_, std::unique_ptr<ExprAST> y1_,
            std::unique_ptr<ExprAST> x2_, std::unique_ptr<ExprAST> y2_)
        : x1(std::move(x1_)), y1(std::move(y1_)), x2(std::move(x2_)), y2(std::move(y2_)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class TextOnCanvasAST : public StmtAST {
public:
    std::string text;
    std::unique_ptr<ExprAST> x, y, fontSize;
    TextOnCanvasAST(std::string t, std::unique_ptr<ExprAST> x_, std::unique_ptr<ExprAST> y_,
                    std::unique_ptr<ExprAST> fs)
        : text(std::move(t)), x(std::move(x_)), y(std::move(y_)), fontSize(std::move(fs)) {}
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

class ClearAST : public StmtAST {
public:
    ClearAST() = default;
    void accept(StmtVisitor& v) override { v.visit(*this); }
};

} // namespace daad
