/**
 * @file Compiler.cpp
 * @brief تنفيذ خط أنابيب المترجم مع دمج التشخيص والتحقق الأمني
 */
#include "Daad/Compiler.hpp"
#include "Daad/Lexer.hpp"
#include "Daad/Parser.hpp"
#include "Daad/CodeGen.hpp"
#include "Daad/Optimizer.hpp"
#include "Daad/Diagnostics.hpp"
#include "Daad/SandboxValidator.hpp"
#include <algorithm>

namespace daad {

// Top-level declarations (functions, classes, structs, namespaces, ...)
// are emitted at global scope; executable statements are collected into
// the synthesized main() body so the generated file actually compiles.
static bool isGlobalDeclaration(const StmtAST& stmt) {
    return dynamic_cast<const FunctionDeclAST*>(&stmt) != nullptr ||
           dynamic_cast<const ClassDeclAST*>(&stmt) != nullptr ||
           dynamic_cast<const StructDeclAST*>(&stmt) != nullptr ||
           dynamic_cast<const EnumDeclAST*>(&stmt) != nullptr ||
           dynamic_cast<const NamespaceDeclAST*>(&stmt) != nullptr ||
           dynamic_cast<const TemplateDeclAST*>(&stmt) != nullptr ||
           dynamic_cast<const ImportStmtAST*>(&stmt) != nullptr ||
           dynamic_cast<const ExportStmtAST*>(&stmt) != nullptr;
}

CompileResult DaadCompiler::compile(const std::string& sourceCode, const std::string& headerFileName) {
    DiagnosticsEngine diag;
    
    Lexer lexer(sourceCode);
    Parser parser(lexer, diag);
    
    std::vector<std::unique_ptr<StmtAST>> statements;
    while (true) {
        auto stmt = parser.parseStatement();
        if (!stmt) {
            if (parser.isAtEnd()) break;
            continue;
        }
        statements.push_back(std::move(stmt));
    }
    
    // مرحلة التحسين — Bottom-Up In-Place Mutator
    OptimizerVisitor optimizer;
    optimizer.optimize(statements);
    // Prune any nullptr left by DCE (If false with no else, etc.)
    statements.erase(
        std::remove_if(statements.begin(), statements.end(),
                       [](const std::unique_ptr<StmtAST>& p){ return !p; }),
        statements.end());
    
    // جمع أسماء الدوال المعرفة بالمصدر (للرفع الانتقائي من المكتبة)
    std::unordered_set<std::string> userFns;
    for (auto& stmt : statements) {
        if (auto* fn = dynamic_cast<FunctionDeclAST*>(stmt.get()))
            userFns.insert(fn->name);
    }

    // توليد الكود — فصل النطاق العام عن جسم main
    CodeGenVisitor codegen;
    codegen.setUserFunctionNames(userFns);
    codegen.enableDebugInfo(m_debugLines); // -g فقط: بدونه المخرج بايت-مطابق للسابق
    codegen.setDebugSourcePath(m_sourcePath);
    if (!headerFileName.empty()) codegen.setSourceFileName(headerFileName);

    std::vector<StmtAST*> executable;
    for (auto& stmt : statements) {
        if (!stmt) continue;
        if (isGlobalDeclaration(*stmt)) {
            stmt->accept(codegen);
        } else {
            executable.push_back(stmt.get());
        }
    }

    if (!executable.empty()) {
        codegen.beginMainBody();
        for (auto* stmt : executable) {
            stmt->accept(codegen);
        }
        codegen.endMainBody();
    }
    
    // Check for memory leaks (new without delete)
    for (const auto& warning : codegen.getWarnings()) {
        diag.report(Severity::Warning, 0, 0, warning, "memory");
    }
    
    CompileResult result;
    result.success = !diag.hasErrors();
    result.diagnostics = diag.takeDiagnostics();
    result.headerOutput = codegen.getHeaderCode();
    result.sourceOutput = codegen.getSourceCode();
    
    // ── Sandbox Security Validation ──────────────────────────────────────────
    if (result.success) {
        SandboxValidator sandbox;
        auto violations = sandbox.validate(result.sourceOutput);
        for (const auto& v : violations) {
            Severity sev = (v.severity == SandboxViolation::Severity::Error)
                ? Severity::Error : Severity::Warning;
            diag.report(sev, v.line, 0, v.message, "sandbox");
        }
        if (sandbox.hasErrors()) {
            result.success = false;
            result.diagnostics = diag.takeDiagnostics();
        }
    }
    
    return result;
}

} // namespace daad