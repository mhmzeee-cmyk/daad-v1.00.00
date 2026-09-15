#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <utility>

namespace daad {

struct SandboxViolation {
    enum class Severity { Error, Warning };

    Severity severity = Severity::Error;
    std::string message;
    int line = 0;
};

class SandboxValidator {
public:
    SandboxValidator();

    std::vector<SandboxViolation> validate(const std::string& cppSource);
    bool hasErrors() const;

    static const std::unordered_set<std::string>& getAllowedHeaders();
    static const std::vector<std::pair<std::string, std::string>>& getBlockedPatterns();

private:
    void checkIncludes(const std::string& source, int& lineNum);
    void checkDangerousPatterns(const std::string& source, int& lineNum);
    void checkRecursionDepth(const std::string& source, int& lineNum);
    void checkMemoryAllocation(const std::string& source, int& lineNum);

    std::vector<SandboxViolation> m_violations;
};

} // namespace daad
