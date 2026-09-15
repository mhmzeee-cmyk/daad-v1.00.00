/**
 * @file Compiler.cpp
 * @brief تنفيذ خط أنابيب المترجم مع دمج التشخيص
 */
#include "Daad/Compiler.hpp"
#include "Daad/Lexer.hpp"
#include "Daad/Parser.hpp"
#include "Daad/CodeGen.hpp"
#include "Daad/Optimizer.hpp"
#include "Daad/Diagnostics.hpp"

namespace daad {

CompileResult DaadCompiler::compile(const std::string& sourceCode) {
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
    
    // مرحلة التحسين
    OptimizerVisitor optimizer;
    for (auto& stmt : statements) {
        if (stmt) stmt->accept(optimizer);
    }
    
    // توليد الكود
    CodeGenVisitor codegen;
    for (auto& stmt : statements) {
        if (stmt) stmt->accept(codegen);
    }
    
    CompileResult result;
    result.success = !diag.hasErrors();
    result.diagnostics = diag.takeDiagnostics();
    result.headerOutput = codegen.getHeaderCode();
    result.sourceOutput = codegen.getSourceCode();
    
    return result;
}

} // namespace daad