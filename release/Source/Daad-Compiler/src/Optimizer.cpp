#include "Daad/Optimizer.hpp"
#include "Daad/AST.hpp"
#include <memory>

namespace daad {

void OptimizerVisitor::visit(VarDeclStmtAST& node) {
    if (node.init) node.init->accept(*this);
}

void OptimizerVisitor::visit(AssignmentAST& node) {
    if (node.value) node.value->accept(*this);
}

void OptimizerVisitor::visit(CompoundAssignmentAST& node) {
    if (node.value) node.value->accept(*this);
}

void OptimizerVisitor::visit(UnaryExprAST& node) {
    if (node.operand) node.operand->accept(*this);
}

void OptimizerVisitor::visit(IfStmtAST& node) {
    if (node.condition) node.condition->accept(*this);
    for (auto& stmt : node.thenBody) if (stmt) stmt->accept(*this);
    for (auto& stmt : node.elseBody) if (stmt) stmt->accept(*this);
}

void OptimizerVisitor::visit(WhileStmtAST& node) {
    if (node.condition) node.condition->accept(*this);
    for (auto& stmt : node.body) if (stmt) stmt->accept(*this);
}

void OptimizerVisitor::visit(FunctionDeclAST& node) {
    for (auto& stmt : node.body) if (stmt) stmt->accept(*this);
}

void OptimizerVisitor::visit(BinaryExprAST& node) {
    node.lhs->accept(*this);
    node.rhs->accept(*this);

    auto* leftNum = dynamic_cast<NumberExprAST*>(node.lhs.get());
    auto* rightNum = dynamic_cast<NumberExprAST*>(node.rhs.get());

    if (leftNum && rightNum) {
        node.isFolded = true;
        if (node.op == "+") node.foldedValue = leftNum->value + rightNum->value;
        else if (node.op == "-") node.foldedValue = leftNum->value - rightNum->value;
        else if (node.op == "*") node.foldedValue = leftNum->value * rightNum->value;
        else if (node.op == "/") node.foldedValue = leftNum->value / rightNum->value;
        else if (node.op == "%") node.foldedValue = static_cast<int>(leftNum->value) % static_cast<int>(rightNum->value);
        else node.isFolded = false;
    }
}

void OptimizerVisitor::visit(TemplateDeclAST& node) {
    node.body->accept(*this);
}

void OptimizerVisitor::visit(ForStmtAST& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.update) node.update->accept(*this);
    for (auto& stmt : node.body) if (stmt) stmt->accept(*this);
}

void OptimizerVisitor::visit(DoWhileStmtAST& node) {
    for (auto& stmt : node.body) if (stmt) stmt->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

void OptimizerVisitor::visit(SwitchStmtAST& node) {
    if (node.expression) node.expression->accept(*this);
    for (auto& sc : node.cases) {
        if (sc.value) sc.value->accept(*this);
        for (auto& stmt : sc.body) if (stmt) stmt->accept(*this);
    }
    for (auto& stmt : node.defaultBody) if (stmt) stmt->accept(*this);
}

void OptimizerVisitor::visit(ReturnStmtAST& node) {
    if (node.value) node.value->accept(*this);
}

void OptimizerVisitor::visit(NamespaceDeclAST& node) {
    for (auto& stmt : node.body) if (stmt) stmt->accept(*this);
}

void OptimizerVisitor::visit(TryCatchStmtAST& node) {
    for (auto& stmt : node.tryBody) if (stmt) stmt->accept(*this);
    for (auto& stmt : node.catchBody) if (stmt) stmt->accept(*this);
}

void OptimizerVisitor::visit(MemberAssignmentAST& node) {
    if (node.value) node.value->accept(*this);
}

void OptimizerVisitor::visit(ThrowStmtAST& node) {
    if (node.value) node.value->accept(*this);
}

void OptimizerVisitor::visit(ExprStmtAST& node) {
    if (node.expr) node.expr->accept(*this);
}

} // namespace daad