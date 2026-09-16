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
    void setDebugLines(bool v) { m_debugLines = v; } // -g: توجيهات #line للتصحيح (F5)
    void setSourcePath(const std::string& p) { m_sourcePath = p; }
private:
    bool m_debugLines = false;
    std::string m_sourcePath;
};

} // namespace daad
