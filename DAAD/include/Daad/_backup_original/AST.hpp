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
class ThrowStmtAST;
class NewExprAST;
class DeleteStmtAST;
class ExprStmtAST;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(NumberExprAST& node) = 0;
    virtual void visit(VariableExprAST& node) = 0;
    virtual void visit(StringExprAST& node) = 0;
    virtual void visit(BinaryExprAST& node) = 0;
    virtual void visit(UnaryExprAST& node) = 0;
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
    virtual void visit(ThrowStmtAST& node) = 0;
    virtual void visit(NewExprAST& node) = 0;
    virtual void visit(DeleteStmtAST& node) = 0;
    virtual void visit(ExprStmtAST& node) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

class ExprAST : public ASTNode {};
class StmtAST : public ASTNode {};

class ExprStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> expr;
    explicit ExprStmtAST(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

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
    std::vector<std::unique_ptr<StmtAST>> body;
    ClassDeclAST(std::string n, std::vector<std::unique_ptr<StmtAST>> b = {})
        : name(std::move(n)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class StructDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::unique_ptr<StmtAST>> body;
    StructDeclAST(std::string n, std::vector<std::unique_ptr<StmtAST>> b = {})
        : name(std::move(n)), body(std::move(b)) {}
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
    TryCatchStmtAST(std::vector<std::unique_ptr<StmtAST>> t, std::string cv, std::vector<std::unique_ptr<StmtAST>> c)
        : tryBody(std::move(t)), catchVar(std::move(cv)), catchBody(std::move(c)) {}
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
    ArraySubscriptExprAST(std::string n, std::unique_ptr<ExprAST> idx)
        : name(std::move(n)), index(std::move(idx)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ArraySubscriptAssignAST : public StmtAST {
public:
    std::string name;
    std::unique_ptr<ExprAST> index;
    std::unique_ptr<ExprAST> value;
    ArraySubscriptAssignAST(std::string n, std::unique_ptr<ExprAST> idx, std::unique_ptr<ExprAST> val)
        : name(std::move(n)), index(std::move(idx)), value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ThrowStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> value;
    explicit ThrowStmtAST(std::unique_ptr<ExprAST> val) : value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class NewExprAST : public ExprAST {
public:
    std::string typeName;
    std::vector<std::unique_ptr<ExprAST>> args;
    NewExprAST(std::string t, std::vector<std::unique_ptr<ExprAST>> a)
        : typeName(std::move(t)), args(std::move(a)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class DeleteStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> operand;
    explicit DeleteStmtAST(std::unique_ptr<ExprAST> op) : operand(std::move(op)) {}
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

} // namespace daad