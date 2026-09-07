#include <gtest/gtest.h>
#include "Daad/Compiler.hpp"

TEST(RuntimeIntegrationTest, PrintHeaderInjection) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"اختبار\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("#include \"Daad/Runtime/DaadRuntime.hpp\""), std::string::npos);
}

TEST(RuntimeIntegrationTest, MathHeaderInjection) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("#include \"stdlib/Math.hpp\""), std::string::npos);
}

TEST(RuntimeIntegrationTest, PrintTranslatesToDaadPrint) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"اختبار\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("daad::runtime::daad_print("), std::string::npos);
}

TEST(RuntimeIntegrationTest, PrintWithVariable) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛\nطباعة( س ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    int printCount = 0;
    size_t pos = 0;
    while ((pos = result.sourceOutput.find("daad::runtime::daad_print", pos)) != std::string::npos) {
        printCount++;
        pos++;
    }
    EXPECT_EQ(printCount, 1);
}

TEST(RuntimeIntegrationTest, MathLibraryInclude) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("stdlib/Math.hpp"), std::string::npos);
}