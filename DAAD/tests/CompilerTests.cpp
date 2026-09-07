#include <gtest/gtest.h>
#include "Daad/Compiler.hpp"

// اختبار 1: تعريف متغير صحيح
TEST(CompilerTest, VariableDeclaration) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int س = 10;"), std::string::npos);
}

// اختبار 2: طباعة نص
TEST(CompilerTest, PrintStatement) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"مرحبا\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("daad::runtime::daad_print"), std::string::npos);
}

// اختبار 3: عملية حسابية
TEST(CompilerTest, MathExpression) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 + 2 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int س ="), std::string::npos);
}

// اختبار 4: توليد ملفات مزدوجة
TEST(CompilerTest, DualFileOutput) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.sourceOutput.empty());
}

// اختبار 5: التحقق من تضمين Runtime
TEST(CompilerTest, RuntimeHeaderInjection) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("#include \"Daad/Runtime/DaadRuntime.hpp\""), std::string::npos);
}

// اختبار 6: طوي الثوابت (Constant Folding)
TEST(CompilerTest, ConstantFolding) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 1 + 2 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("3"), std::string::npos);
}

// اختبار 7: معالجة الخطأ (Error Recovery)
TEST(DiagnosticsTest, ErrorRecovery) {
    daad::DaadCompiler compiler;
    std::string source = "كلمة_مجهولة ؛";
    auto result = compiler.compile(source);
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.diagnostics.empty());
}

// اختبار 8: طباعة متعددة
TEST(CompilerTest, MultiplePrints) {
    daad::DaadCompiler compiler;
    std::string source = "طباعة( \"أول\" ) ؛\nطباعة( \"ثاني\" ) ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    int count = 0;
    size_t pos = 0;
    while ((pos = result.sourceOutput.find("daad::runtime::daad_print", pos)) != std::string::npos) {
        count++;
        pos++;
    }
    EXPECT_EQ(count, 2);
}

// اختبار 9: ترجمة الأنواع
TEST(CompilerTest, TypeMapping) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح ص = 1 ؛\nعشري ع = 2.5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int ص = 1;"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("double ع ="), std::string::npos);
}

// اختبار 10: التحقق من عدم وجود أخطاء في كود نظيف
TEST(DiagnosticsTest, NoErrorsOnValidCode) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح س = 10 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    bool hasErrors = false;
    for (const auto& d : result.diagnostics) {
        if (d.severity == daad::Severity::Error) hasErrors = true;
    }
    EXPECT_FALSE(hasErrors);
}

// اختبار 11: ترتيب LHS/RHS في تعريف المتغيرات
TEST(CompilerTest, VarDeclLHSRHSOrder) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح أ = 5 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int أ = 5;"), std::string::npos);
    EXPECT_EQ(result.sourceOutput.find("int 5 = أ;"), std::string::npos);
}

// اختبار 12: التعيين بدون auto
TEST(CompilerTest, AssignmentNoAuto) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح أ = 0 ؛\nأ = أ + 1 ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int أ = 0;"), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("أ = (أ + 1);"), std::string::npos);
    EXPECT_EQ(result.sourceOutput.find("auto"), std::string::npos);
}

// اختبار 13: ترتيب LHS/RHS في التعيين
TEST(CompilerTest, AssignmentLHSRHSOrder) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح ب = 3 ؛\nصحيح ج = 7 ؛\nب = ب + ج ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("ب = (ب + ج);"), std::string::npos);
    EXPECT_EQ(result.sourceOutput.find("(ب + ج) = ب;"), std::string::npos);
    EXPECT_EQ(result.sourceOutput.find("ج + ب"), std::string::npos);
}

// اختبار 14: شرط while يحافظ على ترتيب المعاملات
TEST(CompilerTest, WhileConditionOrder) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح أ = 0 ؛\nطالما ( أ < 5 ) { أ = أ + 1 ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("while ((أ < 5))"), std::string::npos);
    EXPECT_EQ(result.sourceOutput.find("while ((5 > أ))"), std::string::npos);
}

// اختبار 15: شرط if يحافظ على ترتيب المعاملات
TEST(CompilerTest, IfConditionOrder) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح أ = 10 ؛\nإذا ( أ >= 18 ) { طباعة( \"بالغ\" ) ؛ }";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("if ((أ >= 18))"), std::string::npos);
    EXPECT_EQ(result.sourceOutput.find("if ((18 <= أ))"), std::string::npos);
}

// اختبار 16: عملية حسابية معبرة تحافظ على الترتيب
TEST(CompilerTest, ComplexExpressionOrder) {
    daad::DaadCompiler compiler;
    std::string source = "صحيح أ = 1 ؛\nصحيح ب = 2 ؛\nصحيح ج = 3 ؛\nصحيح ن = أ + ب + ج ؛";
    auto result = compiler.compile(source);
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.sourceOutput.find("int ن ="), std::string::npos);
    EXPECT_NE(result.sourceOutput.find("أ"), std::string::npos);
}