#include "Daad/Diagnostics.hpp"
#include <iomanip>

namespace daad {

void DiagnosticsEngine::report(Severity severity, size_t line, size_t column, const std::string& message, const std::string& sourceLine) {
    m_diagnostics.push_back({severity, line, column, message, sourceLine});
    if (severity == Severity::Error || severity == Severity::Fatal) m_hasErrors = true;
}

std::string DiagnosticsEngine::severityToString(Severity s) const {
    switch (s) {
        case Severity::Note: return "ملاحظة";
        case Severity::Warning: return "تحذير";
        case Severity::Error: return "خطأ";
        case Severity::Fatal: return "خطأ فادح";
        default: return "غير معروف";
    }
}

void DiagnosticsEngine::printAll(std::ostream& os) const {
    for (const auto& d : m_diagnostics) {
        os << "[" << severityToString(d.severity) << "] "
           << "في السطر " << d.line << ":" << d.column << " - " << d.message << "\n";
        
        if (!d.sourceLine.empty()) {
            os << "  | " << d.sourceLine << "\n";
            os << "  | " << std::string(d.column - 1, ' ') << "^\n";
        }
    }
}

} // namespace daad