#include <gtest/gtest.h>
#include "Daad/Diagnostics.hpp"

using namespace daad;

TEST(DiagnosticsTest, InitiallyNoErrors) {
    DiagnosticsEngine diag;
    EXPECT_FALSE(diag.hasErrors());
    EXPECT_TRUE(diag.getDiagnostics().empty());
}

TEST(DiagnosticsTest, ReportErrorSetsHasErrors) {
    DiagnosticsEngine diag;
    diag.report(Severity::Error, 1, 5, "خطأ اختبار", "");
    EXPECT_TRUE(diag.hasErrors());
    auto diags = diag.getDiagnostics();
    ASSERT_EQ(diags.size(), 1u);
    EXPECT_EQ(diags[0].line, 1u);
    EXPECT_EQ(diags[0].column, 5u);
    EXPECT_EQ(diags[0].severity, Severity::Error);
}

TEST(DiagnosticsTest, ReportWarningDoesNotSetHasErrors) {
    DiagnosticsEngine diag;
    diag.report(Severity::Warning, 2, 3, "تحذير", "");
    EXPECT_FALSE(diag.hasErrors());
    EXPECT_EQ(diag.getDiagnostics().size(), 1u);
    EXPECT_EQ(diag.getDiagnostics()[0].severity, Severity::Warning);
}

TEST(DiagnosticsTest, TakeDiagnosticsMoves) {
    DiagnosticsEngine diag;
    diag.report(Severity::Error, 1, 1, "خطأ", "");
    diag.report(Severity::Note, 2, 1, "ملاحظة", "");
    EXPECT_EQ(diag.getDiagnostics().size(), 2u);
    auto taken = diag.takeDiagnostics();
    EXPECT_EQ(taken.size(), 2u);
    // بعد take، قد يكون فارغ أو منقول — نتحقق من أن hasErrors لا يزال true
    EXPECT_TRUE(diag.hasErrors());
}

TEST(DiagnosticsTest, MultipleReports) {
    DiagnosticsEngine diag;
    diag.report(Severity::Error, 1, 1, "أول", "");
    diag.report(Severity::Error, 2, 1, "ثان", "");
    diag.report(Severity::Fatal, 3, 1, "فادح", "");
    EXPECT_TRUE(diag.hasErrors());
    EXPECT_EQ(diag.getDiagnostics().size(), 3u);
    EXPECT_EQ(diag.getDiagnostics()[2].severity, Severity::Fatal);
}

TEST(DiagnosticsTest, ArabicMessagePreserved) {
    DiagnosticsEngine diag;
    diag.report(Severity::Error, 10, 20, "المتغير 'س' غير معرّف", "صحيح س = غير_موجود ؛");
    auto diags = diag.getDiagnostics();
    ASSERT_EQ(diags.size(), 1u);
    EXPECT_NE(diags[0].message.find("س"), std::string::npos);
    EXPECT_EQ(diags[0].sourceLine, "صحيح س = غير_موجود ؛");
}
