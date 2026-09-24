#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstddef>

namespace daad {

enum class Severity {
    Note,
    Warning,
    Error,
    Fatal
};

struct Diagnostic {
    Severity severity = Severity::Note;
    size_t line = 0;
    size_t column = 0;
    std::string message;
    std::string sourceLine;
};

class DiagnosticsEngine {
public:
    void report(Severity severity, size_t line, size_t column,
                const std::string& message, const std::string& sourceLine = "");
    bool hasErrors() const noexcept { return m_hasErrors; }
    const std::vector<Diagnostic>& getDiagnostics() const noexcept { return m_diagnostics; }
    std::vector<Diagnostic> takeDiagnostics() { return std::move(m_diagnostics); }
    void printAll(std::ostream& os) const;

private:
    std::string severityToString(Severity s) const;
    std::vector<Diagnostic> m_diagnostics;
    bool m_hasErrors = false;
};

} // namespace daad
