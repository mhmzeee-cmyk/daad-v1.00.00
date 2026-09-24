#include <gtest/gtest.h>
#include "Daad/Compiler.hpp"

// اختبارات للصيغ النحوية الجديدة: أنواع مصفوفات متعددة الأبعاد، أسماء مركبة،
// أدلة متسلسلة، وترميز نصوص — أُضيفت في جلسة إصلاح مكتبة stdlib

TEST(NewSyntaxTest, MultiDimArrayTypes) {
    daad::DaadCompiler compiler;
    std::string source = "دالة جمع(أ عشري[][]) -> عشري[][] {\n"
                         "    ارجع أ؛\n"
                         "}\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success) << "فشل تجميع أنواع مصفوفات ثنائية الأبعاد";
    EXPECT_NE(result.sourceOutput.find("std::vector<std::vector<double>> جمع(std::vector<std::vector<double>> أ) {"),
              std::string::npos);
}

TEST(NewSyntaxTest, NameFirstArrayParam) {
    daad::DaadCompiler compiler;
    std::string source = "دالة عكس(ق صحيح[]) -> صحيح[] {\n"
                         "    ارجع ق؛\n"
                         "}\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("std::vector<int> عكس(std::vector<int> ق) {"),
              std::string::npos);
}

TEST(NewSyntaxTest, EmptyArrayLiteral) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح[] عناصر = []؛\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("std::vector<int> عناصر = {};"), std::string::npos);
}

TEST(NewSyntaxTest, EmptyParamPlaceholder) {
    daad::DaadCompiler compiler;
    std::string source = "دالة ف([]) -> صحيح {\n"
                         "    ارجع 2؛\n"
                         "}\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int ف(void ) {"), std::string::npos);
}

TEST(NewSyntaxTest, CompositeFunctionName) {
    daad::DaadCompiler compiler;
    std::string source = "دالة إضافة_ خاصية(م صحيح) -> صحيح {\n"
                         "    ارجع م؛\n"
                         "}\n"
                         "طباعة(إضافة_ خاصية(5))؛\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int إضافة__خاصية(int م) {"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("إضافة__خاصية(5)"), std::string::npos);
}

TEST(NewSyntaxTest, DotPrefixedFunctionName) {
    daad::DaadCompiler compiler;
    std::string source = "دالة .peek() -> صحيح {\n"
                         "    ارجع 1؛\n"
                         "}\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int peek() {"), std::string::npos);
}

TEST(NewSyntaxTest, MultiWordParamName) {
    daad::DaadCompiler compiler;
    std::string source = "دالة عدد_الحالات(عدد حالات صحيح) -> صحيح {\n"
                         "    ارجع عدد حالات؛\n"
                         "}\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int عدد_الحالات(int عدد_حالات) {"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("return عدد_حالات;"), std::string::npos);
}

TEST(NewSyntaxTest, ChainedIndexingAssignment) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح[][] مصفوفة = []؛\n"
                         "مصفوفة[0][0] = 3؛\n"
                         "صحيح نتيجة = مصفوفة[0][0]؛\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("مصفوفة[0][0] = 3;"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("نتيجة = مصفوفة[0][0];"), std::string::npos);
}

TEST(NewSyntaxTest, StringEscaping) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة(\"سطر1\\nسطر2\\\"مقتبس\\\"\")؛\n";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("\\n"), std::string::npos);
}