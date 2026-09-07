/**
 * @file Compiler.hpp
 * @brief الواجهة البرمجية لمترجم 'ض' مع نظام التشخيص
 */
#pragma once

#include <string>
#include <vector>
#include "Daad/AST.hpp"
#include "Daad/Diagnostics.hpp"

namespace daad {

struct CompileResult {
    bool success;
    std::string headerOutput;
    std::string sourceOutput;
    std::vector<Diagnostic> diagnostics; // استخدام النظام الجديد
};

class DaadCompiler {
public:
    // headerFileName: اسم ملف الرأس الذي سيتضمنه كود المصدر (افتراضي output.hpp)
    CompileResult compile(const std::string& sourceCode, const std::string& headerFileName = "output.hpp");
};

} // namespace daad