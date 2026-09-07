/**
 * @file SandboxValidator.cpp
 * @brief فحص أمان الكود المُولَّد
 */
#include "Daad/SandboxValidator.hpp"
#include <sstream>
#include <algorithm>

namespace daad {

// ── C++ Standard Library Headers (Whitelist) ─────────────────────────────────
static const std::unordered_set<std::string> ALLOWED_HEADERS = {
    // C++ Standard
    "<iostream>", "<string>", "<vector>", "<map>", "<set>",
    "<algorithm>", "<numeric>", "<functional>", "<memory>",
    "<sstream>", "<fstream>", "<cassert>", "<cmath>",
    "<cstdlib>", "<cstring>", "<ctime>", "<chrono>",
    "<array>", "<tuple>", "<utility>", "<optional>",
    "<variant>", "<any>", "<type_traits>", "<limits>",
    "<stdexcept>", "<exception>", "<regex>",
    "<queue>", "<stack>", "<deque>", "<list>",
    "<unordered_map>", "<unordered_set>",
    // Project headers
    "\"output.hpp\"",
    "\"Daad/Runtime/DaadRuntime.hpp\"",
    "\"stdlib/Math.hpp\"",
};

// ── Blocked Patterns (dangerous system calls / code injection) ────────────────
static const std::vector<std::pair<std::string, std::string>> BLOCKED_PATTERNS = {
    // System commands
    {"system\\s*\\(", "استدعاء أوامر النظام (system()) محظور"},
    {"popen\\s*\\(", "فتح أنابيب (popen()) محظور"},
    {"exec[vlp]*\\s*\\(", "تنفيذ ملفات (exec*) محظور"},
    
    // File system (destructive)
    {"remove\\s*\\(", "حذف ملفات (remove()) محظور"},
    {"rename\\s*\\(", "إعادة تسمية ملفات (rename()) محظور"},
    {"unlink\\s*\\(", "حذف روابط (unlink()) محظور"},
    
    // Memory (unsafe)
    {"reinterpret_cast", "تحويل unsafe (reinterpret_cast) محظور"},
    {"\\*\\s*\\(\\s*\\w+\\s*\\*\\s*\\)", "مؤشر عائم (void*) محظور"},
    
    // Network
    {"socket\\s*\\(", "فتح ملفات وصلات (socket()) محظور"},
    {"connect\\s*\\(", "اتصال شبكة (connect()) محظور"},
    {"bind\\s*\\(", "ربط منفذ (bind()) محظور"},
    {"listen\\s*\\(", "استماع منفذ (listen()) محظور"},
    {"accept\\s*\\(", "قبول وصلة (accept()) محظور"},
    
    // Process
    {"fork\\s*\\(", "إنشاء عملية (fork()) محظور"},
    {"kill\\s*\\(", "إنهاء عملية (kill()) محظور"},
    {"exit\\s*\\(", "إنهاء مفاجئ (exit()) محظور"},
    
    // Dangerous casts
    {"const_cast", "إزالة const (const_cast) محظور"},
    
    // Preprocessor abuse
    {"#pragma\\s+once", ""},  // This is fine, allow
    {"#define\\s+\\w+\\s+\\\\", "ماكرو متعدد الأسطر محظور"},
};

// ── Headers that allow system access ──────────────────────────────────────────
static const std::unordered_set<std::string> DANGEROUS_HEADERS = {
    "<cstdlib>",     // system(), exit(), malloc()
    "<cstdio>",      // FILE*, fopen(), printf()
    "<windows.h>",   // Windows API
    "<unistd.h>",    // POSIX API
    "<sys/",         // System headers
    "<netinet/",     // Network headers
    "<arpa/",        // Network headers
    "<dlfcn.h>",     // Dynamic loading
    "<signal.h>",    // Signal handling
    "<setjmp.h>",    // Non-local jumps
};

SandboxValidator::SandboxValidator() {}

std::vector<SandboxViolation> SandboxValidator::validate(const std::string& cppSource) {
    m_violations.clear();
    
    std::istringstream stream(cppSource);
    std::string line;
    int lineNum = 0;
    
    while (std::getline(stream, line)) {
        lineNum++;
        checkIncludes(line, lineNum);
        checkDangerousPatterns(line, lineNum);
    }
    
    // Reset and re-scan for patterns that span lines
    checkRecursionDepth(cppSource, lineNum);
    checkMemoryAllocation(cppSource, lineNum);
    
    return m_violations;
}

bool SandboxValidator::hasErrors() const {
    for (const auto& v : m_violations) {
        if (v.severity == SandboxViolation::Severity::Error) return true;
    }
    return false;
}

void SandboxValidator::checkIncludes(const std::string& source, int& lineNum) {
    // Skip comments
    if (source.find("//") == 0) return;
    
    // Find #include directives
    size_t includePos = source.find("#include");
    if (includePos == std::string::npos) return;
    
    // Extract header name
    std::string trimmed = source.substr(includePos + 8);
    size_t start = trimmed.find_first_not_of(" \t");
    if (start == std::string::npos) return;
    
    std::string header;
    size_t end = trimmed.find_first_of(" \t\n>", start);
    if (end != std::string::npos) {
        header = trimmed.substr(start, end - start);
    } else {
        header = trimmed.substr(start);
    }
    
    // For angle brackets: include the > in the header name
    std::string cleanHeader = header;
    if (cleanHeader.find('<') != std::string::npos && cleanHeader.find('>') == std::string::npos) {
        size_t closeBracket = trimmed.find('>', start);
        if (closeBracket != std::string::npos) {
            cleanHeader = trimmed.substr(start, closeBracket - start + 1);
        }
    }
    
    // Check against whitelist
    if (ALLOWED_HEADERS.find(cleanHeader) == ALLOWED_HEADERS.end()) {
        // Check if it's a project header: quoted ("Daad/", "stdlib/", or a
        // generated local header like "e2e_out.hpp" written next to the output)
        bool isProjectHeader =
            cleanHeader.find("\"Daad/") == 0 ||
            cleanHeader.find("\"stdlib/") == 0 ||
            (!cleanHeader.empty() && cleanHeader[0] == '"' && cleanHeader.find(".hpp") != std::string::npos);
        if (!isProjectHeader) {
            m_violations.push_back({
                SandboxViolation::Severity::Error,
                "include غير مسموح به: " + cleanHeader + ". استخدم فقط المكتبات المسموح بها.",
                lineNum
            });
        }
    }
    
    // Check dangerous headers
    for (const auto& dangerous : DANGEROUS_HEADERS) {
        if (cleanHeader.find(dangerous) != std::string::npos) {
            m_violations.push_back({
                SandboxViolation::Severity::Error,
                "include خطير: " + cleanHeader + ". هذا الـ header يحتوي على وظائف نظام محظورة.",
                lineNum
            });
        }
    }
}

void SandboxValidator::checkDangerousPatterns(const std::string& source, int& lineNum) {
    // Skip comments
    if (source.find("//") == 0) return;
    
    for (const auto& [pattern, message] : BLOCKED_PATTERNS) {
        if (message.empty()) continue; // Skip allowed patterns
        
        // Simple pattern matching (avoid regex dependency for now)
        if (source.find(pattern) != std::string::npos) {
            m_violations.push_back({
                SandboxViolation::Severity::Error,
                "نمط خطير: " + message,
                lineNum
            });
        }
    }
    
    // Check for raw string injection via daad_print
    if (source.find("daad::runtime::daad_print") != std::string::npos) {
        // This is fine — daad_print is safe
    }
}

void SandboxValidator::checkRecursionDepth(const std::string& source, int& lineNum) {
    // Count function definitions
    int funcCount = 0;
    size_t pos = 0;
    while ((pos = source.find("void ", pos)) != std::string::npos) {
        funcCount++;
        pos += 5;
    }
    
    // Simple heuristic: if there are many functions calling each other,
    // warn about potential stack overflow
    if (funcCount > 20) {
        m_violations.push_back({
            SandboxViolation::Severity::Warning,
            "عدد كبير من الدوال (" + std::to_string(funcCount) + "). تأكد من عدم وجود تكرار لانهائي.",
            0
        });
    }
}

void SandboxValidator::checkMemoryAllocation(const std::string& source, int& lineNum) {
    // Check for unbounded new[]
    if (source.find("new ") != std::string::npos) {
        // Check if the size comes from user input (dangerous)
        if (source.find("new int[") != std::string::npos ||
            source.find("new char[") != std::string::npos ||
            source.find("new double[") != std::string::npos) {
            // This could be a VLA or large allocation
            m_violations.push_back({
                SandboxViolation::Severity::Warning,
                "allocate الذاكرة ديناميكياً. تأكد من تحرير الذاكرة بـ delete.",
                0
            });
        }
    }
}

const std::unordered_set<std::string>& SandboxValidator::getAllowedHeaders() {
    return ALLOWED_HEADERS;
}

const std::vector<std::pair<std::string, std::string>>& SandboxValidator::getBlockedPatterns() {
    return BLOCKED_PATTERNS;
}

} // namespace daad
