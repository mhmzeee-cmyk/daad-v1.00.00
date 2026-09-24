#include "Daad/Optimizer.hpp"
#include "Daad/AST.hpp"
#include <cmath>
#include <memory>

namespace daad {

// ─── Helpers ────────────────────────────────────────────────────────────────

bool OptimizerVisitor::isNumber(const std::unique_ptr<ExprAST>& e, double& out) {
    if (auto* n = dynamic_cast<NumberExprAST*>(e.get())) { out = n->value; return true; }
    return false;
}
bool OptimizerVisitor::isBool(const std::unique_ptr<ExprAST>& e, bool& out) {
    if (auto* b = dynamic_cast<BoolExprAST*>(e.get())) { out = b->value; return true; }
    return false;
}
bool OptimizerVisitor::isString(const std::unique_ptr<ExprAST>& e, std::string& out) {
    if (auto* s = dynamic_cast<StringExprAST*>(e.get())) { out = s->value; return true; }
    return false;
}
bool OptimizerVisitor::isZero(const std::unique_ptr<ExprAST>& e) {
    double v; return isNumber(e, v) && v == 0.0;
}
bool OptimizerVisitor::isOne(const std::unique_ptr<ExprAST>& e) {
    double v; return isNumber(e, v) && v == 1.0;
}
bool OptimizerVisitor::isTrue(const std::unique_ptr<ExprAST>& e) {
    bool b; return isBool(e, b) && b == true;
}
bool OptimizerVisitor::isFalse(const std::unique_ptr<ExprAST>& e) {
    bool b; return isBool(e, b) && b == false;
}
bool OptimizerVisitor::isUnconditionalJump(const std::unique_ptr<StmtAST>& stmt) {
    return dynamic_cast<ReturnStmtAST*>(stmt.get()) != nullptr
        || dynamic_cast<BreakStmtAST*>(stmt.get()) != nullptr
        || dynamic_cast<ContinueStmtAST*>(stmt.get()) != nullptr
        || dynamic_cast<ThrowStmtAST*>(stmt.get()) != nullptr;
}

// ─── Legacy visitor (flag-based) for backward compat with tests ───────────

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
        else if (node.op == "/") {
            if (rightNum->value == 0.0) node.isFolded = false;
            else node.foldedValue = leftNum->value / rightNum->value;
        }
        else if (node.op == "%") {
            if (rightNum->value == 0.0) node.isFolded = false;
            else node.foldedValue = static_cast<int>(leftNum->value) % static_cast<int>(rightNum->value);
        }
        else node.isFolded = false;
    }
}
void OptimizerVisitor::visit(UnaryExprAST& node) {
    if (node.operand) node.operand->accept(*this);
}
void OptimizerVisitor::visit(VarDeclStmtAST& node) { if (node.init) node.init->accept(*this); }
void OptimizerVisitor::visit(AssignmentAST& node) { if (node.value) node.value->accept(*this); }
void OptimizerVisitor::visit(CompoundAssignmentAST& node) { if (node.value) node.value->accept(*this); }
void OptimizerVisitor::visit(IfStmtAST& node) {
    if (node.condition) node.condition->accept(*this);
    for (auto& s : node.thenBody) if (s) s->accept(*this);
    for (auto& s : node.elseBody) if (s) s->accept(*this);
}
void OptimizerVisitor::visit(WhileStmtAST& node) {
    if (node.condition) node.condition->accept(*this);
    for (auto& s : node.body) if (s) s->accept(*this);
}
void OptimizerVisitor::visit(ForStmtAST& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.update) node.update->accept(*this);
    for (auto& s : node.body) if (s) s->accept(*this);
}
void OptimizerVisitor::visit(DoWhileStmtAST& node) {
    for (auto& s : node.body) if (s) s->accept(*this);
    if (node.condition) node.condition->accept(*this);
}
void OptimizerVisitor::visit(SwitchStmtAST& node) {
    if (node.expression) node.expression->accept(*this);
    for (auto& sc : node.cases) { if (sc.value) sc.value->accept(*this); for (auto& s : sc.body) if (s) s->accept(*this); }
    for (auto& s : node.defaultBody) if (s) s->accept(*this);
}
void OptimizerVisitor::visit(ReturnStmtAST& node) { if (node.value) node.value->accept(*this); }
void OptimizerVisitor::visit(FunctionDeclAST& node) { for (auto& s : node.body) if (s) s->accept(*this); }
void OptimizerVisitor::visit(NamespaceDeclAST& node) { for (auto& s : node.body) if (s) s->accept(*this); }
void OptimizerVisitor::visit(TryCatchStmtAST& node) { for (auto& s : node.tryBody) if (s) s->accept(*this); for (auto& s : node.catchBody) if (s) s->accept(*this); }
void OptimizerVisitor::visit(TemplateDeclAST& node) { node.body->accept(*this); }
void OptimizerVisitor::visit(MemberAssignmentAST& node) { if (node.value) node.value->accept(*this); }
void OptimizerVisitor::visit(ThrowStmtAST& node) { if (node.value) node.value->accept(*this); }
void OptimizerVisitor::visit(ExprStmtAST& node) { if (node.expr) node.expr->accept(*this); }

// ─── Top-level ────────────────────────────────────────────────────────────

void OptimizerVisitor::optimize(std::vector<std::unique_ptr<StmtAST>>& statements) {
    optimizeBlock(reinterpret_cast<StmtList&>(statements));
}

void OptimizerVisitor::optimizeBlock(StmtList& block) {
    for (size_t i = 0; i < block.size(); ) {
        std::unique_ptr<StmtAST>& stmt = block[i];
        if (!stmt) { block.erase(block.begin() + i); continue; }

        // Mutate statement in-place (bottom-up)
        optimizeStmt(stmt);

        // After mutation stmt may have become nullptr (If false with no else)
        if (!stmt) { block.erase(block.begin() + i); continue; }

        // IfStmt pruning: replace constant If with its branch
        if (auto* ifNode = dynamic_cast<IfStmtAST*>(stmt.get())) {
            bool b;
            if (isBool(ifNode->condition, b)) {
                StmtList replacement;
                if (b) replacement = std::move(ifNode->thenBody);
                else   replacement = std::move(ifNode->elseBody);
                // Erase the If itself
                block.erase(block.begin() + i);
                if (!replacement.empty()) {
                    // Already optimized inside optimizeStmt, insert spliced stmts
                    block.insert(block.begin() + i,
                                 std::make_move_iterator(replacement.begin()),
                                 std::make_move_iterator(replacement.end()));
                    // Do not advance i; process first inserted on next iteration
                    // But inserted are already optimized, so skip over them for jump pruning
                    // To avoid infinite loop, advance if we want to skip re-check of same If
                    // We continue loop which will handle jump pruning for inserted
                    continue;
                }
                continue;
            }
        }

        // Block pruning after unconditional jump
        if (isUnconditionalJump(stmt)) {
            if (i + 1 < block.size()) {
                block.erase(block.begin() + i + 1, block.end());
            }
            break;
        }
        ++i;
    }
}

// ─── Expression mutator (post-order) ──────────────────────────────────────

bool OptimizerVisitor::optimizeExpr(std::unique_ptr<ExprAST>& expr) {
    if (!expr) return false;

    // Recurse to children first (bottom-up)
    if (auto* bin = dynamic_cast<BinaryExprAST*>(expr.get())) {
        optimizeExpr(bin->lhs);
        optimizeExpr(bin->rhs);
        if (tryFoldBinary(expr)) return true;
        if (trySimplifyBinary(expr)) return true;
        return false;
    }
    if (auto* unary = dynamic_cast<UnaryExprAST*>(expr.get())) {
        optimizeExpr(unary->operand);
        if (tryFoldUnary(expr)) return true;
        return false;
    }
    if (auto* tern = dynamic_cast<TernaryExprAST*>(expr.get())) {
        optimizeExpr(tern->condition);
        optimizeExpr(tern->trueExpr);
        optimizeExpr(tern->falseExpr);
        if (tryFoldTernary(expr)) return true;
        return false;
    }
    if (auto* call = dynamic_cast<FunctionCallAST*>(expr.get())) {
        for (auto& a : call->args) optimizeExpr(a);
        return false;
    }
    if (auto* arr = dynamic_cast<ArraySubscriptExprAST*>(expr.get())) {
        optimizeExpr(arr->index);
        if (arr->base) optimizeExpr(arr->base);
        return false;
    }
    if (auto* ne = dynamic_cast<NewExprAST*>(expr.get())) {
        for (auto& a : ne->args) optimizeExpr(a);
        return false;
    }
    if (auto* px = dynamic_cast<PixelAST*>(expr.get())) {
        optimizeExpr(px->x); optimizeExpr(px->y);
        return false;
    }
    if (auto* sz = dynamic_cast<SizeofExprAST*>(expr.get())) {
        optimizeExpr(sz->expr);
        return false;
    }
    if (auto* tf = dynamic_cast<TypeofExprAST*>(expr.get())) {
        optimizeExpr(tf->expr);
        return false;
    }
    // ImageSize, Variable, Number, String, Bool, Null, Raw, MemberAccess — leaf
    return false;
}

bool OptimizerVisitor::tryFoldBinary(std::unique_ptr<ExprAST>& expr) {
    auto* bin = dynamic_cast<BinaryExprAST*>(expr.get());
    if (!bin) return false;
    const std::string& op = bin->op;

    double lv, rv;
    std::string ls, rs;
    bool lb, rb;

    // String concatenation: String + String
    if (op == "+" && isString(bin->lhs, ls) && isString(bin->rhs, rs)) {
        expr = std::make_unique<StringExprAST>(ls + rs);
        return true;
    }
    // Arithmetic on numbers
    if (isNumber(bin->lhs, lv) && isNumber(bin->rhs, rv)) {
        if (op == "+") { expr = std::make_unique<NumberExprAST>(lv + rv); return true; }
        if (op == "-") { expr = std::make_unique<NumberExprAST>(lv - rv); return true; }
        if (op == "*") { expr = std::make_unique<NumberExprAST>(lv * rv); return true; }
        if (op == "/") {
            if (rv == 0.0) return false; // safe skip
            expr = std::make_unique<NumberExprAST>(lv / rv); return true;
        }
        if (op == "%") {
            if (rv == 0.0) return false;
            expr = std::make_unique<NumberExprAST>(static_cast<double>(static_cast<int>(lv) % static_cast<int>(rv)));
            return true;
        }
        if (op == "^") {
            expr = std::make_unique<NumberExprAST>(std::pow(lv, rv));
            return true;
        }
        // Relational on numbers -> Bool
        if (op == "==") { expr = std::make_unique<BoolExprAST>(lv == rv); return true; }
        if (op == "!=") { expr = std::make_unique<BoolExprAST>(lv != rv); return true; }
        if (op == "<")  { expr = std::make_unique<BoolExprAST>(lv <  rv); return true; }
        if (op == ">")  { expr = std::make_unique<BoolExprAST>(lv >  rv); return true; }
        if (op == "<=") { expr = std::make_unique<BoolExprAST>(lv <= rv); return true; }
        if (op == ">=") { expr = std::make_unique<BoolExprAST>(lv >= rv); return true; }
    }
    // Relational on strings -> Bool
    if (isString(bin->lhs, ls) && isString(bin->rhs, rs)) {
        if (op == "==") { expr = std::make_unique<BoolExprAST>(ls == rs); return true; }
        if (op == "!=") { expr = std::make_unique<BoolExprAST>(ls != rs); return true; }
        if (op == "<")  { expr = std::make_unique<BoolExprAST>(ls <  rs); return true; }
        if (op == ">")  { expr = std::make_unique<BoolExprAST>(ls >  rs); return true; }
        if (op == "<=") { expr = std::make_unique<BoolExprAST>(ls <= rs); return true; }
        if (op == ">=") { expr = std::make_unique<BoolExprAST>(ls >= rs); return true; }
    }
    // Boolean & Logical
    if (isBool(bin->lhs, lb) && isBool(bin->rhs, rb)) {
        if (op == "و" || op == "&&") { expr = std::make_unique<BoolExprAST>(lb && rb); return true; }
        if (op == "أو" || op == "||") { expr = std::make_unique<BoolExprAST>(lb || rb); return true; }
        if (op == "==") { expr = std::make_unique<BoolExprAST>(lb == rb); return true; }
        if (op == "!=") { expr = std::make_unique<BoolExprAST>(lb != rb); return true; }
    }
    // Mixed relational Bool ==/!= already handled; no numeric-bool cross folding
    return false;
}

bool OptimizerVisitor::trySimplifyBinary(std::unique_ptr<ExprAST>& expr) {
    auto* bin = dynamic_cast<BinaryExprAST*>(expr.get());
    if (!bin) return false;
    const std::string& op = bin->op;

    // x + 0 -> x
    if (op == "+") {
        if (isZero(bin->lhs)) { expr = std::move(bin->rhs); return true; }
        if (isZero(bin->rhs)) { expr = std::move(bin->lhs); return true; }
    }
    // x * 0 -> 0
    if (op == "*") {
        if (isZero(bin->lhs) || isZero(bin->rhs)) {
            expr = std::make_unique<NumberExprAST>(0.0);
            return true;
        }
        if (isOne(bin->lhs)) { expr = std::move(bin->rhs); return true; }
        if (isOne(bin->rhs)) { expr = std::move(bin->lhs); return true; }
    }
    // x - 0 -> x (bonus)
    if (op == "-" && isZero(bin->rhs)) { expr = std::move(bin->lhs); return true; }
    // x / 1 -> x
    if (op == "/" && isOne(bin->rhs)) { expr = std::move(bin->lhs); return true; }
    // x && true -> x  (و / &&)
    if (op == "و" || op == "&&") {
        if (isTrue(bin->lhs)) { expr = std::move(bin->rhs); return true; }
        if (isTrue(bin->rhs)) { expr = std::move(bin->lhs); return true; }
        if (isFalse(bin->lhs) || isFalse(bin->rhs)) {
            expr = std::make_unique<BoolExprAST>(false);
            return true;
        }
    }
    // x || false -> x ; x || true -> true
    if (op == "أو" || op == "||") {
        if (isFalse(bin->lhs)) { expr = std::move(bin->rhs); return true; }
        if (isFalse(bin->rhs)) { expr = std::move(bin->lhs); return true; }
        if (isTrue(bin->lhs) || isTrue(bin->rhs)) {
            expr = std::make_unique<BoolExprAST>(true);
            return true;
        }
    }
    return false;
}

bool OptimizerVisitor::tryFoldUnary(std::unique_ptr<ExprAST>& expr) {
    auto* u = dynamic_cast<UnaryExprAST*>(expr.get());
    if (!u) return false;
    bool b; double v;
    if ((u->op == "!" || u->op == "ليس") && isBool(u->operand, b)) {
        expr = std::make_unique<BoolExprAST>(!b);
        return true;
    }
    if (u->op == "-" && isNumber(u->operand, v)) {
        expr = std::make_unique<NumberExprAST>(-v);
        return true;
    }
    if (u->op == "!" && isNumber(u->operand, v)) {
        // !0 -> true, !non-zero -> false (defensive, mirrors C++ !)
        expr = std::make_unique<BoolExprAST>(v == 0.0);
        return true;
    }
    return false;
}

bool OptimizerVisitor::tryFoldTernary(std::unique_ptr<ExprAST>& expr) {
    auto* t = dynamic_cast<TernaryExprAST*>(expr.get());
    if (!t) return false;
    bool b;
    if (isBool(t->condition, b)) {
        if (b) expr = std::move(t->trueExpr);
        else   expr = std::move(t->falseExpr);
        return true;
    }
    // Also fold if condition is number (0 false, else true) — not required but safe
    double nv;
    if (isNumber(t->condition, nv)) {
        if (nv != 0.0) expr = std::move(t->trueExpr);
        else expr = std::move(t->falseExpr);
        return true;
    }
    return false;
}

// ─── Statement mutator ──────────────────────────────────────────────────────

bool OptimizerVisitor::optimizeStmt(std::unique_ptr<StmtAST>& stmt) {
    if (!stmt) return false;

    if (auto* n = dynamic_cast<VarDeclStmtAST*>(stmt.get())) {
        if (n->init) optimizeExpr(n->init);
    } else if (auto* n = dynamic_cast<AssignmentAST*>(stmt.get())) {
        if (n->value) optimizeExpr(n->value);
    } else if (auto* n = dynamic_cast<CompoundAssignmentAST*>(stmt.get())) {
        if (n->value) optimizeExpr(n->value);
    } else if (auto* n = dynamic_cast<IfStmtAST*>(stmt.get())) {
        if (n->condition) optimizeExpr(n->condition);
        optimizeBlock(n->thenBody);
        optimizeBlock(n->elseBody);
    } else if (auto* n = dynamic_cast<WhileStmtAST*>(stmt.get())) {
        if (n->condition) optimizeExpr(n->condition);
        optimizeBlock(n->body);
    } else if (auto* n = dynamic_cast<ForStmtAST*>(stmt.get())) {
        if (n->init) optimizeStmt(n->init);
        if (n->condition) optimizeExpr(n->condition);
        if (n->update) optimizeStmt(n->update);
        optimizeBlock(n->body);
    } else if (auto* n = dynamic_cast<DoWhileStmtAST*>(stmt.get())) {
        optimizeBlock(n->body);
        if (n->condition) optimizeExpr(n->condition);
    } else if (auto* n = dynamic_cast<SwitchStmtAST*>(stmt.get())) {
        if (n->expression) optimizeExpr(n->expression);
        for (auto& sc : n->cases) {
            if (sc.value) optimizeExpr(sc.value);
            optimizeBlock(sc.body);
        }
        optimizeBlock(n->defaultBody);
    } else if (auto* n = dynamic_cast<ReturnStmtAST*>(stmt.get())) {
        if (n->value) optimizeExpr(n->value);
    } else if (auto* n = dynamic_cast<FunctionDeclAST*>(stmt.get())) {
        optimizeBlock(n->body);
    } else if (auto* n = dynamic_cast<ClassDeclAST*>(stmt.get())) {
        for (auto& m : n->methods) if (m) optimizeBlock(m->body);
    } else if (auto* n = dynamic_cast<StructDeclAST*>(stmt.get())) {
        // no expr to fold
        (void)n;
    } else if (auto* n = dynamic_cast<NamespaceDeclAST*>(stmt.get())) {
        optimizeBlock(n->body);
    } else if (auto* n = dynamic_cast<TryCatchStmtAST*>(stmt.get())) {
        optimizeBlock(n->tryBody);
        optimizeBlock(n->catchBody);
        optimizeBlock(n->finallyBody);
    } else if (auto* n = dynamic_cast<TemplateDeclAST*>(stmt.get())) {
        if (n->body) optimizeStmt(n->body);
    } else if (auto* n = dynamic_cast<MemberAssignmentAST*>(stmt.get())) {
        if (n->value) optimizeExpr(n->value);
    } else if (auto* n = dynamic_cast<ArraySubscriptAssignAST*>(stmt.get())) {
        if (n->index) optimizeExpr(n->index);
        if (n->value) optimizeExpr(n->value);
        if (n->base) optimizeExpr(n->base);
    } else if (auto* n = dynamic_cast<ThrowStmtAST*>(stmt.get())) {
        if (n->value) optimizeExpr(n->value);
    } else if (auto* n = dynamic_cast<DeleteStmtAST*>(stmt.get())) {
        if (n->operand) optimizeExpr(n->operand);
    } else if (auto* n = dynamic_cast<ExprStmtAST*>(stmt.get())) {
        if (n->expr) optimizeExpr(n->expr);
    } else if (auto* n = dynamic_cast<ForEachStmtAST*>(stmt.get())) {
        if (n->iterable) optimizeExpr(n->iterable);
        optimizeBlock(n->body);
    } else if (auto* n = dynamic_cast<PrintStmtAST*>(stmt.get())) {
        for (auto& a : n->args) optimizeExpr(a);
    } else if (auto* n = dynamic_cast<DrawImageAST*>(stmt.get())) {
        optimizeExpr(n->x); optimizeExpr(n->y); optimizeExpr(n->w); optimizeExpr(n->h);
    } else if (auto* n = dynamic_cast<SaveImageAST*>(stmt.get())) {
        // path is string literal, no expr
        (void)n;
    } else if (auto* n = dynamic_cast<CropImageAST*>(stmt.get())) {
        optimizeExpr(n->x); optimizeExpr(n->y); optimizeExpr(n->w); optimizeExpr(n->h);
    } else if (auto* n = dynamic_cast<ResizeAST*>(stmt.get())) {
        optimizeExpr(n->w); optimizeExpr(n->h);
    } else if (auto* n = dynamic_cast<RotateImageAST*>(stmt.get())) {
        optimizeExpr(n->angle);
    } else if (auto* n = dynamic_cast<FlipImageAST*>(stmt.get())) {
        (void)n;
    } else if (auto* n = dynamic_cast<OpacityAST*>(stmt.get())) {
        optimizeExpr(n->value);
    } else if (auto* n = dynamic_cast<FilterAST*>(stmt.get())) {
        (void)n;
    } else if (auto* n = dynamic_cast<OverlayAST*>(stmt.get())) {
        optimizeExpr(n->x); optimizeExpr(n->y);
    } else if (auto* n = dynamic_cast<BackgroundAST*>(stmt.get())) {
        (void)n;
    } else if (auto* n = dynamic_cast<DrawAST*>(stmt.get())) {
        for (auto& a : n->args) optimizeExpr(a);
    } else if (auto* n = dynamic_cast<FillAST*>(stmt.get())) {
        for (auto& a : n->args) optimizeExpr(a);
    } else if (auto* n = dynamic_cast<RectangleAST*>(stmt.get())) {
        optimizeExpr(n->x); optimizeExpr(n->y); optimizeExpr(n->w); optimizeExpr(n->h);
    } else if (auto* n = dynamic_cast<CircleAST*>(stmt.get())) {
        optimizeExpr(n->x); optimizeExpr(n->y); optimizeExpr(n->r);
    } else if (auto* n = dynamic_cast<LineAST*>(stmt.get())) {
        optimizeExpr(n->x1); optimizeExpr(n->y1); optimizeExpr(n->x2); optimizeExpr(n->y2);
    } else if (auto* n = dynamic_cast<TextOnCanvasAST*>(stmt.get())) {
        optimizeExpr(n->x); optimizeExpr(n->y); optimizeExpr(n->fontSize);
    } else if (auto* n = dynamic_cast<PixelAST*>(stmt.get())) {
        // Pixel as Expr is handled in expr path; as Stmt via Image expr statement not needed
        (void)n;
    }
    // Break, Continue, Import, Export, LoadImage, SaveImage, Clear — leaf or no expr
    return false;
}

} // namespace daad
