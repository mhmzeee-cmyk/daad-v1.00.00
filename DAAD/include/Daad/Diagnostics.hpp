/**
 * @file Diagnostics.hpp
 * @brief محرك تشخيص الأخطاء الاحترافي
 */
#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace daad {

enum class Severity { Note, Warning, Error, Fatal };

struct Diagnostic {
    Severity severity;
    size_t line, column;
    std::string message;
    std::string sourceLine; // السطر المصدري الذي حدث فيه الخطأ
};

class DiagnosticsEngine {
public:
    void report(Severity severity, size_t line, size_t column, const std::string& message, const std::string& sourceLine = "");
    bool hasErrors() const { return m_hasErrors; }
    void printAll(std::ostream& os) const;
    std::vector<Diagnostic> takeDiagnostics() { return std::move(m_diagnostics); }

private:
    std::vector<Diagnostic> m_diagnostics;
    bool m_hasErrors = false;
    std::string severityToString(Severity s) const;
};

} // namespace daad
