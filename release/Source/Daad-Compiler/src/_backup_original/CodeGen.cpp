#include "Daad/CodeGen.hpp"

namespace daad {

CodeGenVisitor::CodeGenVisitor() {
    m_typeMap["صحيح"] = "int";
    m_typeMap["عشري"] = "double";
    m_typeMap["حرف"] = "char";
    m_typeMap["منطقي"] = "bool";
    m_typeMap["فراغ"] = "void";
    m_typeMap["نص"] = "std::string";
    m_typeMap["تلقائي"] = "auto";

    // أنواع المصفوفات
    m_typeMap["صحيح[]"] = "std::vector<int>";
    m_typeMap["عشري[]"] = "std::vector<double>";
    m_typeMap["حرف[]"] = "std::vector<char>";
    m_typeMap["منطقي[]"] = "std::vector<bool>";
    m_typeMap["نص[]"] = "std::vector<std::string>";

    m_headerStream << "#pragma once\n\n";
    m_sourceStream << "#include \"output.hpp\"\n";
    m_sourceStream << "#include \"Daad/Runtime/DaadRuntime.hpp\"\n";
    m_sourceStream << "#include \"stdlib/Math.hpp\"\n";
    m_sourceStream << "#include \"stdlib/DaadStdlib.hpp\"\n";
    m_sourceStream << "#include <iostream>\n";
    m_sourceStream << "#include <string>\n";
    m_sourceStream << "#include <vector>\n";
    m_sourceStream << "#include <stdexcept>\n\n";
}

void CodeGenVisitor::visit(NumberExprAST& node) {
    if (node.value == static_cast<int>(node.value)) {
        m_lastExpr = std::to_string(static_cast<int>(node.value));
    } else {
        m_lastExpr = std::to_string(node.value);
    }
}

void CodeGenVisitor::visit(VariableExprAST& node) {
    m_lastExpr = node.name;
}

void CodeGenVisitor::visit(StringExprAST& node) {
    m_lastExpr = "\"" + node.value + "\"";
}

void CodeGenVisitor::visit(BinaryExprAST& node) {
    if (node.isFolded) {
        m_lastExpr = std::to_string(static_cast<int>(node.foldedValue));
    } else {
        node.lhs->accept(*this);
        std::string lhs = m_lastExpr;
        node.rhs->accept(*this);
        std::string rhs = m_lastExpr;
        m_lastExpr = "(" + lhs + " " + node.op + " " + rhs + ")";
    }
}

void CodeGenVisitor::visit(UnaryExprAST& node) {
    node.operand->accept(*this);
    m_lastExpr = "(" + node.op + m_lastExpr + ")";
}

void CodeGenVisitor::visit(VarDeclStmtAST& node) {
    if (node.type == "__builtin_print") {
        node.init->accept(*this);
        m_sourceStream << "daad::runtime::daad_print(" << m_lastExpr << ");\n";
        return;
    }
    if (node.type == "__builtin_call") {
        node.init->accept(*this);
        m_sourceStream << m_lastExpr << ";\n";
        return;
    }

    std::string cppType = m_typeMap.count(node.type) ? m_typeMap[node.type] : node.type;
    node.init->accept(*this);
    if (m_inForContext) {
        m_lastExpr = cppType + " " + node.name + " = " + m_lastExpr;
    } else {
        m_sourceStream << cppType << " " << node.name << " = " << m_lastExpr << ";\n";
    }
}

void CodeGenVisitor::visit(AssignmentAST& node) {
    node.value->accept(*this);
    if (m_inForContext) {
        m_lastExpr = node.name + " = " + m_lastExpr;
    } else {
        m_sourceStream << node.name << " = " << m_lastExpr << ";\n";
    }
}

void CodeGenVisitor::visit(CompoundAssignmentAST& node) {
    node.value->accept(*this);
    m_sourceStream << node.name << " " << node.op << " " << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(IfStmtAST& node) {
    node.condition->accept(*this);
    m_sourceStream << "if (" << m_lastExpr << ") {\n";
    for (auto& stmt : node.thenBody) {
        stmt->accept(*this);
    }
    m_sourceStream << "}\n";

    if (!node.elseBody.empty()) {
        m_sourceStream << " else {\n";
        for (auto& stmt : node.elseBody) {
            stmt->accept(*this);
        }
        m_sourceStream << "}\n";
    }
}

void CodeGenVisitor::visit(WhileStmtAST& node) {
    node.condition->accept(*this);
    m_sourceStream << "while (" << m_lastExpr << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    m_sourceStream << "}\n";
}

void CodeGenVisitor::visit(ForStmtAST& node) {
    m_sourceStream << "for (";
    m_inForContext = true;
    if (node.init) {
        node.init->accept(*this);
        m_sourceStream << m_lastExpr;
    }
    m_sourceStream << "; ";
    if (node.condition) {
        node.condition->accept(*this);
        m_sourceStream << m_lastExpr;
    }
    m_sourceStream << "; ";
    if (node.update) {
        node.update->accept(*this);
        m_sourceStream << m_lastExpr;
    }
    m_inForContext = false;
    m_sourceStream << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    m_sourceStream << "}\n";
}

void CodeGenVisitor::visit(DoWhileStmtAST& node) {
    m_sourceStream << "do {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    m_sourceStream << "} while (";
    node.condition->accept(*this);
    m_sourceStream << m_lastExpr << ");\n";
}

void CodeGenVisitor::visit(SwitchStmtAST& node) {
    node.expression->accept(*this);
    m_sourceStream << "switch (" << m_lastExpr << ") {\n";
    for (auto& sc : node.cases) {
        sc.value->accept(*this);
        m_sourceStream << "case " << m_lastExpr << ":\n";
        for (auto& stmt : sc.body) {
            stmt->accept(*this);
        }
        m_sourceStream << "break;\n";
    }
    if (!node.defaultBody.empty()) {
        m_sourceStream << "default:\n";
        for (auto& stmt : node.defaultBody) {
            stmt->accept(*this);
        }
        m_sourceStream << "break;\n";
    }
    m_sourceStream << "}\n";
}

void CodeGenVisitor::visit(ReturnStmtAST& node) {
    if (node.value) {
        node.value->accept(*this);
        m_sourceStream << "return " << m_lastExpr << ";\n";
    } else {
        m_sourceStream << "return;\n";
    }
}

void CodeGenVisitor::visit(BreakStmtAST& node) {
    m_sourceStream << "break;\n";
}

void CodeGenVisitor::visit(ContinueStmtAST& node) {
    m_sourceStream << "continue;\n";
}

void CodeGenVisitor::visit(FunctionDeclAST& node) {
    std::string cppRetType = m_typeMap.count(node.returnType) ? m_typeMap[node.returnType] : node.returnType;

    m_headerStream << cppRetType << " " << node.name << "(";
    for (size_t i = 0; i < node.params.size(); ++i) {
        std::string pType = m_typeMap.count(node.params[i].first) ? m_typeMap[node.params[i].first] : node.params[i].first;
        m_headerStream << pType << " " << node.params[i].second;
        if (i < node.params.size() - 1) m_headerStream << ", ";
    }
    m_headerStream << ");\n";

    m_sourceStream << cppRetType << " " << node.name << "(";
    for (size_t i = 0; i < node.params.size(); ++i) {
        std::string pType = m_typeMap.count(node.params[i].first) ? m_typeMap[node.params[i].first] : node.params[i].first;
        m_sourceStream << pType << " " << node.params[i].second;
        if (i < node.params.size() - 1) m_sourceStream << ", ";
    }
    m_sourceStream << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    m_sourceStream << "}\n";
}

void CodeGenVisitor::visit(FunctionCallAST& node) {
    std::string call = node.name + "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        call += m_lastExpr;
        if (i < node.args.size() - 1) call += ", ";
    }
    call += ")";
    m_lastExpr = call;
}

void CodeGenVisitor::visit(ClassDeclAST& node) {
    m_headerStream << "class " << node.name << " {\n";
    if (!node.body.empty()) {
        m_headerStream << "public:\n";
        for (auto& stmt : node.body) {
            stmt->accept(*this);
        }
    }
    m_headerStream << "};\n";
}

void CodeGenVisitor::visit(StructDeclAST& node) {
    m_headerStream << "struct " << node.name << " {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    m_headerStream << "};\n";
}

void CodeGenVisitor::visit(NamespaceDeclAST& node) {
    m_sourceStream << "namespace " << node.name << " {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    m_sourceStream << "} // namespace " << node.name << "\n";
}

void CodeGenVisitor::visit(EnumDeclAST& node) {
    m_headerStream << "enum " << node.name << " {\n";
    for (size_t i = 0; i < node.values.size(); ++i) {
        m_headerStream << "    " << node.values[i];
        if (i < node.values.size() - 1) m_headerStream << ",";
        m_headerStream << "\n";
    }
    m_headerStream << "};\n";
}

void CodeGenVisitor::visit(TryCatchStmtAST& node) {
    m_sourceStream << "try {\n";
    for (auto& stmt : node.tryBody) {
        stmt->accept(*this);
    }
    m_sourceStream << "}\n";
    if (!node.catchBody.empty()) {
        m_sourceStream << "catch";
        if (!node.catchVar.empty()) {
            m_sourceStream << " (const std::exception& " << node.catchVar << ")";
        }
        m_sourceStream << " {\n";
        for (auto& stmt : node.catchBody) {
            stmt->accept(*this);
        }
        m_sourceStream << "}\n";
    }
}

void CodeGenVisitor::visit(TemplateDeclAST& node) {
    std::string prefix = "template <";
    for (size_t i = 0; i < node.params.size(); ++i) {
        prefix += "typename " + node.params[i];
        if (i < node.params.size() - 1) prefix += ", ";
    }
    prefix += ">\n";
    m_headerStream << prefix;
    m_sourceStream << prefix;
    node.body->accept(*this);
}

void CodeGenVisitor::visit(MemberAccessExprAST& node) {
    m_lastExpr = node.object + "." + node.member;
    m_sourceStream << m_lastExpr;
}

void CodeGenVisitor::visit(MemberAssignmentAST& node) {
    node.value->accept(*this);
    m_sourceStream << node.object << "." << node.member << " = " << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(ArraySubscriptExprAST& node) {
    node.index->accept(*this);
    m_lastExpr = node.name + "[" + m_lastExpr + "]";
}

void CodeGenVisitor::visit(ArraySubscriptAssignAST& node) {
    node.index->accept(*this);
    std::string idx = m_lastExpr;
    node.value->accept(*this);
    m_sourceStream << node.name << "[" << idx << "] = " << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(ThrowStmtAST& node) {
    node.value->accept(*this);
    m_sourceStream << "throw " << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(NewExprAST& node) {
    std::string call = "new " + node.typeName + "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        call += m_lastExpr;
        if (i < node.args.size() - 1) call += ", ";
    }
    call += ")";
    m_lastExpr = call;
}

void CodeGenVisitor::visit(DeleteStmtAST& node) {
    node.operand->accept(*this);
    m_sourceStream << "delete " << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(ExprStmtAST& node) {
    node.expr->accept(*this);
    m_sourceStream << m_lastExpr << ";\n";
}

} // namespace daad
