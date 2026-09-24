#pragma once
/**
 * @file SandboxValidator.hpp
 * @brief فحص أمان الكود المُولَّد — تقييد includes، حماية الذاكرة، كشف التكرار اللانهائي
 */
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace daad {

struct SandboxViolation {
    enum class Severity { Warning, Error };
    Severity severity;
    std::string message;
    int line;
};

class SandboxValidator {
public:
    SandboxValidator();
    
    std::vector<SandboxViolation> validate(const std::string& cppSource);
    bool hasErrors() const;
    
    // Allowed C++ standard headers (whitelist)
    static const std::unordered_set<std::string>& getAllowedHeaders();
    
    // Blocked patterns (dangerous system calls)
    static const std::vector<std::pair<std::string, std::string>>& getBlockedPatterns();

private:
    std::vector<SandboxViolation> m_violations;
    
    void checkIncludes(const std::string& source, int& lineNum);
    void checkDangerousPatterns(const std::string& source, int& lineNum);
    void checkRecursionDepth(const std::string& source, int& lineNum);
    void checkMemoryAllocation(const std::string& source, int& lineNum);
};

} // namespace daad
