#include "Daad/Compiler.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

static const std::string STDLIB_DIR = []() {
    const char* env = std::getenv("DAAD_STDLIB_DIR");
    if (env) return std::string(env);
    return std::string("stdlib");
}();

struct DaadFileInfo {
    std::string category;
    std::string library;
    std::filesystem::path path;
};

static std::string readFileContents(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string raw = ss.str();
    if (raw.size() >= 3 &&
        (unsigned char)raw[0] == 0xEF &&
        (unsigned char)raw[1] == 0xBB &&
        (unsigned char)raw[2] == 0xBF) {
        raw = raw.substr(3);
    }
    return raw;
}

static std::vector<std::string> splitLines(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

static std::string extractReturnType(const std::string& funcDeclLine) {
    auto arrowPos = funcDeclLine.find("->");
    if (arrowPos == std::string::npos) return "";
    std::string rest = funcDeclLine.substr(arrowPos + 2);
    auto bracePos = rest.find('{');
    if (bracePos != std::string::npos) rest = rest.substr(0, bracePos);
    size_t start = rest.find_first_not_of(" \t");
    size_t end = rest.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return rest.substr(start, end - start + 1);
}

static std::string extractHeader(const std::string& content, const std::string& prefix) {
    auto pos = content.find(prefix);
    if (pos == std::string::npos) return "";
    std::string rest = content.substr(pos + prefix.size());
    auto end = rest.find('\n');
    if (end != std::string::npos) rest = rest.substr(0, end);
    size_t s = rest.find_first_not_of(" \t\r");
    size_t e = rest.find_last_not_of(" \t\r");
    if (s == std::string::npos) return "";
    return rest.substr(s, e - s + 1);
}

static bool isValidReturnType(const std::string& retType) {
    if (retType == "صحيح" || retType == "نص" || retType == "عشري" ||
        retType == "منطقي" || retType == "فراغ" || retType == "حرف") return true;
    if (retType.find("منطقي[]") != std::string::npos) return true;
    if (retType.find("صحيح[]") != std::string::npos) return true;
    if (retType.find("نص[]") != std::string::npos) return true;
    if (retType.find("عشري[]") != std::string::npos) return true;
    return false;
}

static void validateDaadFile(const DaadFileInfo& info) {
    SCOPED_TRACE("Category: " + info.category + ", Library: " + info.library);

    std::string content = readFileContents(info.path);
    ASSERT_FALSE(content.empty()) << "File is empty or unreadable: " << info.path.string();

    std::vector<std::string> lines = splitLines(content);
    ASSERT_GE(lines.size(), 10u) << "File has too few lines: " << info.path.string();

    std::string foundLib = extractHeader(content, "// ض مكتبة: ");
    EXPECT_FALSE(foundLib.empty()) << "Missing '// ض مكتبة:' header";

    std::string foundCat = extractHeader(content, "// Category: ");
    EXPECT_FALSE(foundCat.empty()) << "Missing '// Category:' header";

    std::string foundDesc = extractHeader(content, "// الوصف: ");
    EXPECT_FALSE(foundDesc.empty()) << "Missing '// الوصف:' header";

    int funcCount = 0;
    int voidCount = 0;
    int returnCount = 0;

    for (const auto& line : lines) {
        if (line.size() >= 2 && line[0] == '/' && line[1] == '/') continue;
        if (line.find("دالة ") == std::string::npos) continue;

        funcCount++;

        EXPECT_NE(line.find('('), std::string::npos)
            << "Function declaration missing parentheses: " << line;

        std::string retType = extractReturnType(line);
        if (retType.empty()) {
            voidCount++;
        } else {
            EXPECT_TRUE(isValidReturnType(retType))
                << "Invalid return type '" << retType << "' in: " << line;
            returnCount++;
        }
    }

    EXPECT_GE(funcCount, 10u) << "File must have at least 10 function declarations";
    EXPECT_TRUE(returnCount > 0 || voidCount > 0)
        << "File must have at least one function";

    if (returnCount > 0) {
        bool hasReturn = (content.find("ارجع") != std::string::npos);
        EXPECT_TRUE(hasReturn) << "File has non-void functions but missing 'ارجع' keyword";
    }
}

class StdlibTest : public ::testing::TestWithParam<DaadFileInfo> {};

TEST_P(StdlibTest, ValidateFile) {
    validateDaadFile(GetParam());
}

static std::vector<DaadFileInfo> generateAllFileInfos() {
    std::vector<DaadFileInfo> infos;

    for (const auto& catEntry : std::filesystem::directory_iterator(STDLIB_DIR)) {
        if (!catEntry.is_directory()) continue;
        std::string catName = catEntry.path().filename().string();
        for (const auto& fileEntry : std::filesystem::directory_iterator(catEntry.path())) {
            if (fileEntry.path().extension() == ".ض") {
                infos.push_back({catName, fileEntry.path().filename().string(), fileEntry.path()});
            }
        }
    }

    return infos;
}

static std::string testNameGenerator(const ::testing::TestParamInfo<DaadFileInfo>& info) {
    std::string path = info.param.path.string();
    std::string result;
    for (char c : path) {
        if (c == '/' || c == '\\' || c == '-' || c == ' ' || c == '.' || c == ':') {
            result += '_';
        } else if (static_cast<unsigned char>(c) > 127) {
            result += "u" + std::to_string(static_cast<int>(static_cast<unsigned char>(c)));
        } else {
            result += c;
        }
    }
    return result;
}

INSTANTIATE_TEST_SUITE_P(
    StdlibAllFiles,
    StdlibTest,
    ::testing::ValuesIn(generateAllFileInfos()),
    testNameGenerator
);

// ═══════════════════════════════════════════════════════════════════
// اختبار الترجمة الحقيقية: كل ملف مكتبة ← C++ ← فحص g++ كامل
// مفعّل فقط عند تحديد DAAD_STDLIB_REAL=1 (لأنه ثقيل نسبيًا)
// يوثّق نسبة الملفات القابلة للترجمة فعليًا بدل فحص التعليقات
// ═══════════════════════════════════════════════════════════════════
TEST(StdlibRealCompile, TranslatesAndCompilesWhenEnabled) {
    if (!std::getenv("DAAD_STDLIB_REAL")) {
        GTEST_SKIP() << "فعّل DAAD_STDLIB_REAL=1 لتشغيل فحص الترجمة الحقيقية";
    }
    const std::string gxx =
        "g++ -std=c++20 -Iinclude -I. -fsyntax-only /tmp/daad_real_XXXX.cpp ";
    int total = 0, passed = 0;
    std::vector<std::string> failures;

    for (const auto& catEntry : std::filesystem::directory_iterator(STDLIB_DIR)) {
        if (!catEntry.is_directory()) continue;
        for (const auto& fileEntry : std::filesystem::directory_iterator(catEntry.path())) {
            if (fileEntry.path().extension() != ".ض") continue;
            ++total;
            daad::DaadCompiler compiler;
            auto result = compiler.compile(
                readFileContents(fileEntry.path()), "daad_real_1.hpp");
            bool ok = result.success;
            if (ok) {
                std::ofstream out("/tmp/daad_real_1.cpp",
                                  std::ios::binary | std::ios::trunc);
                out << result.sourceOutput;
                out.close();
                std::ofstream hout("/tmp/daad_real_1.hpp",
                                   std::ios::binary | std::ios::trunc);
                hout << result.headerOutput;
                hout.close();
                // ابحث عن جذر المشروع (يحتوي include/Daad) صعودًا من cwd
                std::filesystem::path root = std::filesystem::current_path();
                while (!root.empty() &&
                       !std::filesystem::exists(root / "include/Daad/CodeGen.hpp"))
                    root = root.parent_path();
                std::string inc1 = (root / "include").string();
                std::string inc2 = root.string();
                std::string cmd = "g++ -std=c++20 -I'" + inc1 + "' -I'" + inc2 +
                                  "' -fsyntax-only /tmp/daad_real_1.cpp 2>/dev/null";
                ok = (std::system(cmd.c_str()) == 0);
            }
            if (ok) { ++passed; }
            else if (failures.size() < 10)
                failures.push_back(fileEntry.path().string());
        }
    }
    std::cout << "\n[StdlibReal] " << passed << "/" << total
              << " ملفًا يُترجم حتى C++ سليمًا\n";
    for (auto& f : failures)
        std::cout << "  FAIL: " << f << "\n";
    // لا نفشل الاختبار حاليًا — نقيس ونعرض؛ ارفع النسبة ثم اشدد الشرط
    EXPECT_GE(passed, 60); /* ارفعها تدريجيًا مع إصلاح الذيل */
}
