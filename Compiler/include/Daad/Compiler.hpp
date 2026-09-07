#pragma once

#include "Daad/Diagnostics.hpp"
#include <string>
#include <vector>

namespace daad {

struct CompileResult {
    bool success = false;
    std::vector<Diagnostic> diagnostics;
    std::string headerOutput;
    std::string sourceOutput;
};

class DaadCompiler {
public:
    CompileResult compile(const std::string& sourceCode, const std::string& headerFileName = "");
};

} // namespace daad
