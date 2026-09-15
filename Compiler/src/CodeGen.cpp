#include "Daad/CodeGen.hpp"
#include <unordered_set>
#include <cmath>
#include <cstdio>

namespace daad {

// تحويل اسم ض إلى معرّف C++ صالح: إزالة النقطة البادئة (مثل .peek) ودمج الكلمات بفاصلة سفلية
static std::string sanitizeIdent(std::string name) {
    while (!name.empty() && name.front() == '.') name = name.substr(1);
    for (auto& c : name) {
        if (c == ' ') c = '_';
    }
    return name;
}

// تخطي أحرف خاصة داخل النصوص الحرفية حتى لا يكسر المخرجات المولّدة
static std::string escapeString(const std::string& value) {
    std::string escaped;
    for (char c : value) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '"':  escaped += "\\\""; break;
            case '\n': escaped += "\\n";  break;
            case '\r': escaped += "\\r";  break;
            case '\t': escaped += "\\t";  break;
            default:   escaped += c;      break;
        }
    }
    return escaped;
}

// ── Helper Methods ────────────────────────────────────────────────────────────

void CodeGenVisitor::emitSourceComment(int line) {
    if (m_debugInfo && line > 0) {
        target() << "// [daad:" << line << "]\n";
    }
}

void CodeGenVisitor::trackAllocation(const std::string& varName) {
    m_allocatedVars.insert(varName);
}

void CodeGenVisitor::trackDeallocation(const std::string& varName) {
    m_freedVars.insert(varName);
}

void CodeGenVisitor::checkMemoryLeaks() {
    for (const auto& var : m_allocatedVars) {
        if (m_freedVars.find(var) == m_freedVars.end()) {
            m_warnings.push_back("⚠️ متغير '" + var + "' تم تخصيص ذاكرة له بـ new لكن لم يتم تحريره بـ delete");
        }
    }
}

CodeGenVisitor::CodeGenVisitor() : m_activeStream(&m_sourceStream) {
    m_typeMap["صحيح"] = "int";
    m_typeMap["عشري"] = "double";
    m_typeMap["حرف"] = "char";
    m_typeMap["منطقي"] = "bool";
    m_typeMap["فراغ"] = "void";
    m_typeMap["نص"] = "std::string";
    m_typeMap["تلقائي"] = "auto";

    // أنواع المصفوفات
    m_typeMap["صحيح[]"] = "std::vector<int>";
    m_typeMap["نص[]"] = "std::vector<std::string>";
    m_typeMap["عشري[]"] = "std::vector<double>";
    m_typeMap["حرف[]"] = "std::vector<char>";
    m_typeMap["منطقي[]"] = "std::vector<bool>";
    m_typeMap["نص[]"] = "std::vector<std::string>";

    m_headerStream << "#pragma once\n\n";
    m_headerStream << "#include <iostream>\n";
    m_headerStream << "#include <string>\n";
    m_headerStream << "#include <vector>\n";
    m_headerStream << "#include <stdexcept>\n\n";
    target() << "#include \"Daad/Runtime/DaadRuntime.hpp\"\n";
    target() << "#include \"stdlib/Math.hpp\"\n";
    target() << "#include \"stdlib/DaadStdlib.hpp\"\n";
    target() << "#include <iostream>\n";
    target() << "#include <string>\n";
    target() << "#include <vector>\n";
    target() << "#include <stdexcept>\n\n";
}

std::string CodeGenVisitor::mapType(const std::string& type) const {
    auto it = m_typeMap.find(type);
    if (it != m_typeMap.end()) return it->second;

    // أنواع مصفوفات متعددة الأبعاد: عشري[][] → std::vector<std::vector<double>>
    std::string base = type;
    int dims = 0;
    while (base.size() >= 2 && base.substr(base.size() - 2) == "[]") {
        base.resize(base.size() - 2);
        dims++;
    }
    auto bit = m_typeMap.find(base);
    if (dims > 0 && bit != m_typeMap.end()) {
        std::string result = bit->second;
        for (int i = 0; i < dims; ++i) result = "std::vector<" + result + ">";
        return result;
    }
    return type;
}


// ═══ أسماء صادرات المكتبة (تطابق أسطر using السابقة في DaadStdlib.hpp) ═══
const std::vector<std::string>& CodeGenVisitor::stdlibExportedNames() {
    static const std::vector<std::string> names = {
    "طول_مصفوفة",
    "أضف",
    "احذف_من",
    "عكس_مصفوفة",
    "ابحث_في",
    "جذر",
    "مضروب",
    "اقرأ_stdin",
    "طول",
    "استخرج",
    "ابحث",
    "استبدل",
    "صغير",
    "كبير",
    "نظف",
    "يبدأ_بـ",
    "ينتهي_بـ",
    "قسّم",
    "قوة",
    "مطلق",
    "أقصى",
    "أدناه",
    "عشوائي",
    "حجم_المصفوفة",
    "أضف_لمصفوفة",
    "احصل_من_مصفوفة",
    "عّين_في_مصفوفة",
    "احذف_من_مصفوفة",
    "قلب_المصفوفة",
    "فرّغ_المصفوفة",
    "انسخ_مصفوفة",
    "ادمج_مصفوفتين",
    "اكتب_stdout",
    "اكتب_stderr",
    };
    return names;
}

void CodeGenVisitor::setUserFunctionNames(const std::unordered_set<std::string>& names) {
    m_userFunctionNames = names;
    // يُستدعى بعد التحليل وقبل أي إصدار جمل — نرفع أسماء المكتبة
    // انتقائيًا: تخطَّ ما يعرّفه المصدر نفسه (منع تصادم البنية)
#ifdef DAAD_DEBUG_SELECT
    fprintf(stderr, "[select] userFns=%zu\n", m_userFunctionNames.size());
#endif
    for (const auto& name : stdlibExportedNames()) {
        if (m_userFunctionNames.count(name) == 0) {
            target() << "using daad::stdlib::" << name << ";\n";
#ifdef DAAD_DEBUG_SELECT
        } else {
            fprintf(stderr, "[select] skip %s\n", name.c_str());
#endif
        }
    }
}
void CodeGenVisitor::visit(NumberExprAST& node) {
    if (node.value == static_cast<int>(node.value)) {
        m_lastExpr = std::to_string(static_cast<int>(node.value));
    } else {
        m_lastExpr = std::to_string(node.value);
    }
}

void CodeGenVisitor::visit(VariableExprAST& node) {
    m_lastExpr = sanitizeIdent(node.name);
}

void CodeGenVisitor::visit(StringExprAST& node) {
    m_lastExpr = "\"" + escapeString(node.value) + "\"";
}

void CodeGenVisitor::visit(BinaryExprAST& node) {
    if (node.isFolded) {
        m_lastExpr = std::to_string(static_cast<int>(node.foldedValue));
    } else {
        node.lhs->accept(*this);
        std::string lhs = m_lastExpr;
        node.rhs->accept(*this);
        std::string rhs = m_lastExpr;
        // BUG-02 fix: ^ power operator → std::pow()
        if (node.op == "^") {
            m_lastExpr = "std::pow(" + lhs + ", " + rhs + ")";
        } else {
            // Map Arabic logical operators to C++
            std::string op = node.op;
            if (op == "و") op = "&&";
            if (op == "أو") op = "||";
            // دمج نص حرفي مع تعبير نصي: لفّ الطرف الحرفي بـ std::string
            if (op == "+" && lhs.size() >= 2 && lhs.front() == '"' && lhs.back() == '"'
                && (rhs.find("std::string") != std::string::npos
                    || rhs.find("اقتطع") != std::string::npos
                    || rhs.find("فصل(") != std::string::npos
                    || rhs.find("تحويل") != std::string::npos)) {
                m_lastExpr = "(std::string(" + lhs + ") + " + rhs + ")";
                return;
            }
            m_lastExpr = "(" + lhs + " " + op + " " + rhs + ")";
        }
    }
}

void CodeGenVisitor::visit(UnaryExprAST& node) {
    node.operand->accept(*this);
    m_lastExpr = "(" + node.op + m_lastExpr + ")";
}

void CodeGenVisitor::visit(RawExprAST& node) {
    m_lastExpr = node.raw;
}

void CodeGenVisitor::visit(VarDeclStmtAST& node) {
    if (node.type == "__builtin_print") {
        node.init->accept(*this);
        target() << "daad::runtime::daad_print(" << m_lastExpr << ");\n";
        return;
    }
    if (node.type == "__builtin_call") {
        node.init->accept(*this);
        target() << m_lastExpr << ";\n";
        return;
    }

    std::string cppType = mapType(node.type);

    // أنواع المستخدم (صفوف/بنى): الإخراج بلا تهيئة افتراضية (0) — غير صالح في C++
    bool isUserType = m_typeMap.count(node.type) == 0 &&
                      node.type != "__builtin_print" && node.type != "__builtin_call";
    bool isDefaultZero = false;
    if (auto* num = dynamic_cast<NumberExprAST*>(node.init.get())) {
        isDefaultZero = (num->value == 0.0);
        // النص لا يقبل 0 — الصفر الافتراضي له سلسلة فارغة
        if (isDefaultZero && cppType == "std::string") {
            node.init = std::make_unique<StringExprAST>("");
            isDefaultZero = false;
        }
    }

    if (isUserType && isDefaultZero) {
        if (m_inForContext) {
            m_lastExpr = cppType + " " + sanitizeIdent(node.name);
        } else {
            target() << cppType << " " << sanitizeIdent(node.name) << ";\n"; 
        }
        return;
    }

    node.init->accept(*this);

    // مصفوفة فارغة [] مع نوع قياسي مُعلَن: ارتقِ إلى vector من نفس النوع
    // (كان يولّد int م = {} فتفشل أضف/طول عليها)
    if (m_lastExpr == "{}" && !isUserType) {
        if (cppType == "int" || cppType == "double" || cppType == "float" ||
            cppType == "bool" || cppType == "char" || cppType == "long long" ||
            cppType == "std::string")
            cppType = "std::vector<" + cppType + ">";
    }

    // Track new allocations for leak detection
    if (m_lastExpr.find("new ") != std::string::npos) {
        trackAllocation(node.name);
    }
    
    if (m_inForContext) {
        m_lastExpr = cppType + " " + sanitizeIdent(node.name) + " = " + m_lastExpr;
    } else {
        target() << cppType << " " << sanitizeIdent(node.name) << " = " << m_lastExpr << ";\n"; 
    }
}

void CodeGenVisitor::visit(AssignmentAST& node) {
    node.value->accept(*this);
    if (m_inForContext) {
        m_lastExpr = sanitizeIdent(node.name) + " = " + m_lastExpr;
    } else {
        target() << sanitizeIdent(node.name) << " = " << m_lastExpr << ";\n";
    }
}

void CodeGenVisitor::visit(CompoundAssignmentAST& node) {
    node.value->accept(*this);
    // Handle ^= as power assignment (C++ has no ^= for power)
    if (node.op == "^=") {
        if (m_inForContext) {
            m_lastExpr = sanitizeIdent(node.name) + " = std::pow(" + sanitizeIdent(node.name) + ", " + m_lastExpr + ")";
        } else {
            target() << sanitizeIdent(node.name) << " = std::pow(" << sanitizeIdent(node.name) << ", " << m_lastExpr << ");\n";
        }
    } else if (m_inForContext) {
        // In for-loop update position: expose as expression, not a statement
        m_lastExpr = sanitizeIdent(node.name) + " " + node.op + " " + m_lastExpr;
    } else {
        target() << sanitizeIdent(node.name) << " " << node.op << " " << m_lastExpr << ";\n";
    }
}

void CodeGenVisitor::visit(IfStmtAST& node) {
    node.condition->accept(*this);
    target() << "if (" << m_lastExpr << ") {\n";
    for (auto& stmt : node.thenBody) {
        stmt->accept(*this);
    }
    target() << "}\n";

    if (!node.elseBody.empty()) {
        target() << " else {\n";
        for (auto& stmt : node.elseBody) {
            stmt->accept(*this);
        }
        target() << "}\n";
    }
}

void CodeGenVisitor::visit(WhileStmtAST& node) {
    node.condition->accept(*this);
    target() << "while (" << m_lastExpr << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    target() << "}\n";
}

void CodeGenVisitor::visit(ForStmtAST& node) {
    target() << "for (";
    m_inForContext = true;
    if (node.init) {
        node.init->accept(*this);
        target() << m_lastExpr;
    }
    target() << "; ";
    if (node.condition) {
        node.condition->accept(*this);
        target() << m_lastExpr;
    }
    target() << "; ";
    if (node.update) {
        node.update->accept(*this);
        target() << m_lastExpr;
    }
    m_inForContext = false;
    target() << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    target() << "}\n";
}

void CodeGenVisitor::visit(DoWhileStmtAST& node) {
    target() << "do {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    target() << "} while (";
    node.condition->accept(*this);
    target() << m_lastExpr << ");\n";
}

void CodeGenVisitor::visit(SwitchStmtAST& node) {
    node.expression->accept(*this);
    std::string expr = m_lastExpr;
    
    // Check if any case value is a string — C++ switch can't handle strings
    bool hasStringCases = false;
    for (const auto& sc : node.cases) {
        if (auto* strLit = dynamic_cast<StringExprAST*>(sc.value.get())) {
            hasStringCases = true;
            break;
        }
    }
    
    if (hasStringCases) {
        // Generate if-else chain for string comparisons
        bool first = true;
        for (auto& sc : node.cases) {
            sc.value->accept(*this);
            if (first) {
                target() << "if (" << expr << " == " << m_lastExpr << ") {\n";
                first = false;
            } else {
                target() << "} else if (" << expr << " == " << m_lastExpr << ") {\n";
            }
            for (auto& stmt : sc.body) {
                stmt->accept(*this);
            }
        }
        if (!node.defaultBody.empty()) {
            target() << "} else {\n";
            for (auto& stmt : node.defaultBody) {
                stmt->accept(*this);
            }
            target() << "}\n";
        } else {
            target() << "}\n";
        }
    } else {
        // Standard C++ switch for integral types
        target() << "switch (" << expr << ") {\n";
        for (auto& sc : node.cases) {
            sc.value->accept(*this);
            target() << "case " << m_lastExpr << ":\n";
            for (auto& stmt : sc.body) {
                stmt->accept(*this);
            }
            target() << "break;\n";
        }
        if (!node.defaultBody.empty()) {
            target() << "default:\n";
            for (auto& stmt : node.defaultBody) {
                stmt->accept(*this);
            }
            target() << "break;\n";
        }
        target() << "}\n";
    }
}

void CodeGenVisitor::visit(ReturnStmtAST& node) {
    if (node.value) {
        node.value->accept(*this);
        target() << "return " << m_lastExpr << ";\n";
    } else {
        target() << "return;\n";
    }
}

void CodeGenVisitor::visit(BreakStmtAST& node) {
    target() << "break;\n";
}

void CodeGenVisitor::visit(ContinueStmtAST& node) {
    target() << "continue;\n";
}

void CodeGenVisitor::visit(FunctionDeclAST& node) {
    std::string cppRetType = mapType(node.returnType);
    std::string fnName = sanitizeIdent(node.name);

    // طرق الصف: إعلانها الداخلي كُتب في ClassDeclAST (داخل class)، هنا نكتب التعريف المؤهل فقط
    bool isClassMethod = !m_currentClassScope.empty();
    if (!isClassMethod) {
        if (node.isAbstract) {
            // مجرّد دالة خارج الصف — تحذير + توليد = 0;
            m_headerStream << "virtual " << cppRetType << " " << fnName << "(";
            for (size_t i = 0; i < node.params.size(); ++i) {
                std::string pType = mapType(node.params[i].first);
                m_headerStream << pType << " " << sanitizeIdent(node.params[i].second);
                if (i < node.params.size() - 1) m_headerStream << ", ";
            }
            m_headerStream << ") = 0;\n";
            return; // no definition body for pure virtual
        }
        m_headerStream << cppRetType << " " << fnName << "(";
        for (size_t i = 0; i < node.params.size(); ++i) {
            std::string pType = mapType(node.params[i].first);
            m_headerStream << pType << " " << sanitizeIdent(node.params[i].second);
            if (i < node.params.size() - 1) m_headerStream << ", ";
        }
        m_headerStream << ");\n";
    }

    if (node.isAbstract && isClassMethod) {
        // مجرّد دالة داخل الصف — pure virtual declaration only (no definition)
        return;
    }

    std::string fullName = isClassMethod ? m_currentClassScope + "::" + fnName : fnName;
    target() << cppRetType << " " << fullName << "(";
    for (size_t i = 0; i < node.params.size(); ++i) {
        std::string pType = mapType(node.params[i].first);
        target() << pType << " " << sanitizeIdent(node.params[i].second);
        if (i < node.params.size() - 1) target() << ", ";
    }
    target() << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    target() << "}\n";
}

void CodeGenVisitor::visit(FunctionCallAST& node) {
    std::string call = sanitizeIdent(node.name) + "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        call += m_lastExpr;
        if (i < node.args.size() - 1) call += ", ";
    }
    call += ")";
    m_lastExpr = call;
}

void CodeGenVisitor::visit(ClassDeclAST& node) {
    m_headerStream << "class " << sanitizeIdent(node.name);
    if (node.isAbstract) m_headerStream << " abstract"; // MSVC extension hint
    m_headerStream << " {\n";

    // Helper: emit access specifier section if it differs from previous
    AccessLevel lastAccess = AccessLevel::Private; // C++ default
    auto emitAccess = [&](AccessLevel level) {
        if (level != lastAccess) {
            switch (level) {
                case AccessLevel::Public:    m_headerStream << "public:\n"; break;
                case AccessLevel::Private:   m_headerStream << "private:\n"; break;
                case AccessLevel::Protected: m_headerStream << "protected:\n"; break;
            }
            lastAccess = level;
        }
    };

    // Emit members grouped by access level
    for (size_t i = 0; i < node.members.size(); ++i) {
        AccessLevel access = (i < node.memberAccess.size()) ? node.memberAccess[i] : AccessLevel::Private;
        emitAccess(access);
        std::string cppType = mapType(node.members[i].first);
        m_headerStream << "    " << cppType << " " << sanitizeIdent(node.members[i].second) << ";\n";
    }

    // Emit methods with access levels
    for (size_t i = 0; i < node.methods.size(); ++i) {
        AccessLevel access = (i < node.methodAccess.size()) ? node.methodAccess[i] : AccessLevel::Private;
        emitAccess(access);
        if (!node.methods[i]) continue;
        auto& method = node.methods[i];
        std::string ret = mapType(method->returnType);

        if (method->isAbstract) {
            // Pure virtual: virtual ReturnType name(params) = 0;
            m_headerStream << "    virtual " << ret << " " << sanitizeIdent(method->name) << "(";
            for (size_t j = 0; j < method->params.size(); ++j) {
                std::string pt = mapType(method->params[j].first);
                m_headerStream << pt << " " << sanitizeIdent(method->params[j].second);
                if (j < method->params.size() - 1) m_headerStream << ", ";
            }
            m_headerStream << ") = 0;\n";
        } else {
            // Regular virtual method declaration in header
            m_headerStream << "    virtual " << ret << " " << sanitizeIdent(method->name) << "(";
            for (size_t j = 0; j < method->params.size(); ++j) {
                std::string pt = mapType(method->params[j].first);
                m_headerStream << pt << " " << sanitizeIdent(method->params[j].second);
                if (j < method->params.size() - 1) m_headerStream << ", ";
            }
            m_headerStream << ");\n";
        }
    }

    // For abstract classes, add virtual destructor
    if (node.isAbstract) {
        if (lastAccess != AccessLevel::Public) {
            m_headerStream << "public:\n";
        }
        m_headerStream << "    virtual ~" << sanitizeIdent(node.name) << "() = default;\n";
    }

    m_headerStream << "};\n";
    // تعريفات الطرق المؤهلة بـ Class:: خارج الصف
    std::string prevScope = m_currentClassScope;
    m_currentClassScope = sanitizeIdent(node.name);
    for (auto& method : node.methods) {
        if (method && !method->isAbstract) method->accept(*this);
    }
    m_currentClassScope = prevScope;
}

void CodeGenVisitor::visit(StructDeclAST& node) {
    m_headerStream << "struct " << node.name << " {\n};\n";
}

void CodeGenVisitor::visit(NamespaceDeclAST& node) {
    target() << "namespace " << node.name << " {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    target() << "} // namespace " << node.name << "\n";
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
    target() << "try {\n";
    for (auto& stmt : node.tryBody) {
        stmt->accept(*this);
    }
    target() << "}\n";
    if (!node.catchBody.empty()) {
        target() << "catch";
        if (!node.catchVar.empty()) {
            target() << " (const std::exception& " << node.catchVar << ")";
        }
        target() << " {\n";
        for (auto& stmt : node.catchBody) {
            stmt->accept(*this);
        }
        target() << "}\n";
    }
    // أخيراً (finally) — C++ doesn't have finally, generate as scope guard pattern
    if (!node.finallyBody.empty()) {
        target() << "// أخيراً (finally) block\n";
        target() << "{\n";
        for (auto& stmt : node.finallyBody) {
            stmt->accept(*this);
        }
        target() << "}\n";
    }
}

void CodeGenVisitor::visit(TemplateDeclAST& node) {
    m_headerStream << "template <";
    for (size_t i = 0; i < node.params.size(); ++i) {
        m_headerStream << "typename " << node.params[i];
        if (i < node.params.size() - 1) m_headerStream << ", ";
    }
    m_headerStream << ">\n";
    node.body->accept(*this);
}

void CodeGenVisitor::visit(MemberAccessExprAST& node) {
    m_lastExpr = sanitizeIdent(node.object) + "." + sanitizeIdent(node.member);
}

void CodeGenVisitor::visit(MemberAssignmentAST& node) {
    node.value->accept(*this);
    target() << sanitizeIdent(node.object) << "." << sanitizeIdent(node.member) << " = " << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(ArraySubscriptExprAST& node) {
    node.index->accept(*this);
    std::string idx = m_lastExpr;
    if (node.base) {
        node.base->accept(*this);
        m_lastExpr = m_lastExpr + "[" + idx + "]";
    } else {
        m_lastExpr = sanitizeIdent(node.name) + "[" + idx + "]";
    }
}

void CodeGenVisitor::visit(ArraySubscriptAssignAST& node) {
    node.index->accept(*this);
    std::string idx = m_lastExpr;
    node.value->accept(*this);
    std::string val = m_lastExpr;
    if (node.base) {
        node.base->accept(*this);
        target() << m_lastExpr << "[" << idx << "] = " << val << ";\n";
    } else {
        target() << sanitizeIdent(node.name) << "[" << idx << "] = " << val << ";\n";
    }
}

void CodeGenVisitor::visit(BoolExprAST& node) {
    m_lastExpr = node.value ? "true" : "false";
}

void CodeGenVisitor::visit(NullExprAST& node) {
    m_lastExpr = "nullptr";
}

void CodeGenVisitor::visit(TernaryExprAST& node) {
    node.condition->accept(*this);
    std::string cond = m_lastExpr;
    node.trueExpr->accept(*this);
    std::string trueE = m_lastExpr;
    node.falseExpr->accept(*this);
    m_lastExpr = "(" + cond + " ? " + trueE + " : " + m_lastExpr + ")";
}

void CodeGenVisitor::visit(ForEachStmtAST& node) {
    node.iterable->accept(*this);
    std::string iterExpr = m_lastExpr;
    std::string cppType = mapType(node.varType);
    target() << "for (const auto& " << sanitizeIdent(node.varName) << " : " << iterExpr << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    target() << "}\n";
}

void CodeGenVisitor::visit(ThrowStmtAST& node) {
    if (node.value) {
        node.value->accept(*this);
        target() << "throw " << m_lastExpr << ";\n";
    } else {
        target() << "throw;\n";
    }
}

void CodeGenVisitor::visit(DeleteStmtAST& node) {
    node.operand->accept(*this);
    target() << "delete " << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(ImportStmtAST& node) {
    target() << "// استورد: " << node.path << " (no-op in compiled output)\n";
}

void CodeGenVisitor::visit(ExportStmtAST& node) {
    target() << "// صدّر (no-op in compiled output)\n";
}

// ── New AST Node Visitors — يتوافق مع Web ─────────────────────────────────────

void CodeGenVisitor::visit(PrintStmtAST& node) {
    target() << "daad::runtime::daad_print(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        target() << m_lastExpr;
        if (i < node.args.size() - 1) target() << ", ";
    }
    target() << ");\n";
}

// ادخل(متغير) → قراءة من لوحة المفاتيح إلى المتغير
void CodeGenVisitor::visit(InputStmtAST& node) {
    if (node.varName.empty()) return; // خطأ تحليلي — لا شيء يُولَّد
    target() << "std::cin >> " << sanitizeIdent(node.varName) << ";\n";
}

void CodeGenVisitor::visit(ConstructorDeclAST& node) {
    // Constructor handling — will be called within class context
    target() << "(";
    for (size_t i = 0; i < node.params.size(); ++i) {
        std::string pType = mapType(node.params[i].first);
        target() << pType << " " << sanitizeIdent(node.params[i].second);
        if (i < node.params.size() - 1) target() << ", ";
    }
    target() << ") {\n";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    target() << "}\n";
}

void CodeGenVisitor::visit(ExprStmtAST& node) {
    node.expr->accept(*this);
    target() << m_lastExpr << ";\n";
}

void CodeGenVisitor::visit(NewExprAST& node) {
    std::string cppType = mapType(node.typeName);
    std::string call = "new " + cppType + "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        call += m_lastExpr;
        if (i < node.args.size() - 1) call += ", ";
    }
    call += ")";
    m_lastExpr = call;
}

void CodeGenVisitor::visit(SizeofExprAST& node) {
    node.expr->accept(*this);
    m_lastExpr = "sizeof(" + m_lastExpr + ")";
}

void CodeGenVisitor::visit(TypeofExprAST& node) {
    node.expr->accept(*this);
    m_lastExpr = "decltype(" + m_lastExpr + ")";
}

// ── Image Processing CodeGen (20) — يتوافق مع Web ────────────────────────────

void CodeGenVisitor::visit(LoadImageAST& node) {
    target() << "auto " << node.varName << " = daad::image::load(\"" << node.path << "\");\n";
}

void CodeGenVisitor::visit(DrawImageAST& node) {
    node.x->accept(*this); std::string x = m_lastExpr;
    node.y->accept(*this); std::string y = m_lastExpr;
    node.w->accept(*this); std::string w = m_lastExpr;
    node.h->accept(*this); std::string h = m_lastExpr;
    target() << "daad::image::draw(" << node.imgVar << ", " << x << ", " << y << ", " << w << ", " << h << ");\n";
}

void CodeGenVisitor::visit(ImageSizeAST& node) {
    m_lastExpr = "daad::image::size(" + node.imgVar + ")";
}

void CodeGenVisitor::visit(SaveImageAST& node) {
    target() << "daad::image::save(" << node.imgVar << ", \"" << node.path << "\");\n";
}

void CodeGenVisitor::visit(CropImageAST& node) {
    node.x->accept(*this); std::string x = m_lastExpr;
    node.y->accept(*this); std::string y = m_lastExpr;
    node.w->accept(*this); std::string w = m_lastExpr;
    node.h->accept(*this); std::string h = m_lastExpr;
    target() << "daad::image::crop(" << node.imgVar << ", " << x << ", " << y << ", " << w << ", " << h << ");\n";
}

void CodeGenVisitor::visit(ResizeAST& node) {
    node.w->accept(*this); std::string w = m_lastExpr;
    node.h->accept(*this); std::string h = m_lastExpr;
    target() << "daad::image::resize(" << node.imgVar << ", " << w << ", " << h << ");\n";
}

void CodeGenVisitor::visit(RotateImageAST& node) {
    node.angle->accept(*this);
    target() << "daad::image::rotate(" << node.imgVar << ", " << m_lastExpr << ");\n";
}

void CodeGenVisitor::visit(FlipImageAST& node) {
    target() << "daad::image::flip(" << node.imgVar << ", \"" << node.direction << "\");\n";
}

void CodeGenVisitor::visit(OpacityAST& node) {
    node.value->accept(*this);
    target() << "daad::image::opacity(" << node.imgVar << ", " << m_lastExpr << ");\n";
}

void CodeGenVisitor::visit(FilterAST& node) {
    target() << "daad::image::filter(" << node.imgVar << ", \"" << node.filterName << "\");\n";
}

void CodeGenVisitor::visit(OverlayAST& node) {
    node.x->accept(*this); std::string x = m_lastExpr;
    node.y->accept(*this); std::string y = m_lastExpr;
    target() << "daad::image::overlay(" << node.imgVar1 << ", " << node.imgVar2 << ", " << x << ", " << y << ");\n";
}

void CodeGenVisitor::visit(BackgroundAST& node) {
    target() << "daad::image::background(" << node.imgVar << ", " << node.bgVar << ");\n";
}

void CodeGenVisitor::visit(PixelAST& node) {
    node.x->accept(*this); std::string x = m_lastExpr;
    node.y->accept(*this); std::string y = m_lastExpr;
    m_lastExpr = "daad::image::pixel(" + node.imgVar + ", " + x + ", " + y + ")";
}

void CodeGenVisitor::visit(DrawAST& node) {
    target() << "daad::image::drawShape(\"" << node.shape << "\", ";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        target() << m_lastExpr;
        if (i < node.args.size() - 1) target() << ", ";
    }
    target() << ");\n";
}

void CodeGenVisitor::visit(FillAST& node) {
    target() << "daad::image::fill(\"" << node.shape << "\", ";
    for (size_t i = 0; i < node.args.size(); ++i) {
        node.args[i]->accept(*this);
        target() << m_lastExpr;
        if (i < node.args.size() - 1) target() << ", ";
    }
    target() << ");\n";
}

void CodeGenVisitor::visit(RectangleAST& node) {
    node.x->accept(*this); std::string x = m_lastExpr;
    node.y->accept(*this); std::string y = m_lastExpr;
    node.w->accept(*this); std::string w = m_lastExpr;
    node.h->accept(*this); std::string h = m_lastExpr;
    target() << "daad::image::rectangle(" << x << ", " << y << ", " << w << ", " << h << ");\n";
}

void CodeGenVisitor::visit(CircleAST& node) {
    node.x->accept(*this); std::string x = m_lastExpr;
    node.y->accept(*this); std::string y = m_lastExpr;
    node.r->accept(*this); std::string r = m_lastExpr;
    target() << "daad::image::circle(" << x << ", " << y << ", " << r << ");\n";
}

void CodeGenVisitor::visit(LineAST& node) {
    node.x1->accept(*this); std::string x1 = m_lastExpr;
    node.y1->accept(*this); std::string y1 = m_lastExpr;
    node.x2->accept(*this); std::string x2 = m_lastExpr;
    node.y2->accept(*this); std::string y2 = m_lastExpr;
    target() << "daad::image::line(" << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ");\n";
}

void CodeGenVisitor::visit(TextOnCanvasAST& node) {
    node.x->accept(*this); std::string x = m_lastExpr;
    node.y->accept(*this); std::string y = m_lastExpr;
    node.fontSize->accept(*this); std::string fs = m_lastExpr;
    target() << "daad::image::text(\"" << node.text << "\", " << x << ", " << y << ", " << fs << ");\n";
}

void CodeGenVisitor::visit(ClearAST& node) {
    target() << "daad::image::clear();\n";
}

} // namespace daad
